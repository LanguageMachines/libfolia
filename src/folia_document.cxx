/*
  Copyright (c) 2006 - 2019
  CLST  - Radboud University
  ILK   - Tilburg University

  This file is part of libfolia

  libfolia is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.

  libfolia is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, see <http://www.gnu.org/licenses/>.

  For questions and suggestions, see:
      https://github.com/LanguageMachines/ticcutils/issues
  or send mail to:
      lamasoftware (at ) science.ru.nl
*/
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <bitset>
#include <set>
#include <list>
#include <vector>
#include <map>
#include <algorithm>
#include <stdexcept>
#include "config.h"
#include "ticcutils/PrettyPrint.h"
#include "ticcutils/XMLtools.h"
#include "ticcutils/StringOps.h"
#include "ticcutils/Unicode.h"
#include "ticcutils/zipper.h"
#include "libfolia/folia.h"
#include "libfolia/folia_properties.h"
#include "libxml/xmlstring.h"

using namespace std;
using namespace icu;

namespace folia {
  using TiCC::operator<<;

  string DEFAULT_TEXT_SET = "https://raw.githubusercontent.com/proycon/folia/master/setdefinitions/text.foliaset.ttl";
  string DEFAULT_PHON_SET =" https://raw.githubusercontent.com/proycon/folia/master/setdefinitions/phon.foliaset.ttl";

  inline std::ostream& operator<<( std::ostream& os, const Document::at_t& at ){
    os << "<" << at.a << "," << TiCC::toString(at.t) << "," << at.d << "," << at.p << ">";
    return os;
  }

  Document::Document(){
    init();
  }

  void Document::init_args( const KWargs& kwargs ){
    init();
    KWargs args = kwargs;
    auto it = args.find( "debug" );
    if ( it != args.end() ){
      debug = TiCC::stringTo<int>( it->second );
      args.erase(it);
    }
    it = args.find( "mode" );
    if ( it != args.end() ){
      setmode( it->second );
      args.erase(it);
    }
    adjustTextMode();
    it = args.find( "file" );
    if ( it != args.end() ){
      // extract a Document from a file
      readFromFile( it->second );
      args.erase(it);
    }
    else {
      it = args.find( "string" );
      if ( it != args.end() ){
	// extract a Document from a string
	readFromString( it->second );
	args.erase(it);
      }
    }
    if ( !foliadoc ){
      // so NO 'file' or 'string' argument.
      // (readFromFile/readFromString throw on error)
      // create an 'empty' document, with a FoLiA root node.
      it = args.find( "version" );
      if ( it == args.end() ){
	// no version attribute. set it to the current default
	args["version"] = folia_version();
      }
      foliadoc = new FoLiA( args, this );
    }
  }

  Document::Document( const KWargs& kwargs ) {
    init_args( kwargs );
  }

  Document::Document( const string& s ) {
    KWargs args = getArgs(s);
    if ( args.empty() ){
      args["file"] = s;
    }
    init_args( args );
  }

  string folia_version(){
    stringstream ss;
    ss << MAJOR_VERSION << "." << MINOR_VERSION << "." << SUB_VERSION;
    return ss.str();
  }

  string Document::doc_version() const {
    stringstream ss;
    ss << major_version << "." << minor_version << "." << sub_version;
    return ss.str();
  }

  string library_version(){
    return VERSION;
  }

  string Document::update_version(){
    // override the document version with the current folia version
    // return the old value
    string old = _version_string;
    _version_string = folia_version();
    return old;
  }

  void Document::init(){
    _metadata = 0;
    _provenance = 0;
    _xmldoc = 0;
    foliadoc = 0;
    _foliaNsIn_href = 0;
    _foliaNsIn_prefix = 0;
    _foliaNsOut = 0;
    debug = 0;
    mode = CHECKTEXT;
    external = false;
    major_version = 0;
    minor_version = 0;
    sub_version = 0;
    declare( AnnotationType::TEXT, DEFAULT_TEXT_SET );
    declare( AnnotationType::PHON, DEFAULT_PHON_SET );
  }

  Document::~Document(){
    xmlFreeDoc( _xmldoc );
    xmlFree( (xmlChar*)_foliaNsIn_href );
    xmlFree( (xmlChar*)_foliaNsIn_prefix );
    sindex.clear();
    iindex.clear();
    delete foliadoc;
    for ( const auto& it : delSet ){
      delete it;
    }
    delete _metadata;
    for ( const auto& it : submetadata ){
      delete it.second;
    }
    delete _provenance;
  }

  bool operator==( const Document& d1, const Document& d2 ){
    if ( d1.data.size() != d2.data.size() )
      return false;
    for ( size_t i = 0; i < d1.data.size(); ++i ){
      if ( *d1.data[i] != *d2.data[i] )
	return false;
    }
    return true;
  }

  bool operator==( const FoliaElement& a1, const FoliaElement& a2){
    if ( a1.element_id() != a2.element_id() )
      return false;
    if ( a1.id() != a2.id() )
      return false;
    if ( a1.sett() != a2.sett() )
      return false;
    if ( a1.cls() != a2.cls() )
      return false;
    if ( a1.annotator() != a2.annotator() )
      return false;
    if ( a1.annotatortype() != a2.annotatortype() )
      return false;
    if ( a1.size() == a2.size() ) {
      for ( size_t i = 0; i < a1.size(); ++i ){
	if ( *a1.index(i) != *a2.index(i) )
	  return false;
      }
    }
    return true;
  }

  void Document::setmode( const string& ms ) const {
    // mode is mutable, so this even sets mode on CONST documents!
    vector<string> modev = TiCC::split_at( ms, "," );
    for ( const auto& mod : modev ){
      if ( mod == "permissive" ){
	mode = Mode( (int)mode | PERMISSIVE );
      }
      else if ( mod == "nopermissive" ){
	mode = Mode( (int)mode & ~PERMISSIVE );
      }
      else if ( mod == "strip" ){
	mode = Mode( (int)mode | STRIP );
      }
      else if ( mod == "nostrip" ){
	mode = Mode( (int)mode & ~STRIP );
      }
      else if ( mod == "checktext" ){
	mode = Mode( int(mode) | CHECKTEXT );
      }
      else if ( mod == "nochecktext" ){
	mode = Mode( int(mode) & ~CHECKTEXT );
      }
      else if ( mod == "fixtext" ){
	mode = Mode( int(mode) | FIXTEXT );
      }
      else if ( mod == "nofixtext" ){
	mode = Mode( int(mode) & ~FIXTEXT );
      }
      else {
	throw runtime_error( "FoLiA::Document: unsupported mode value: "+ mod );
      }
    }
  }

  string Document::getmode() const{
    string result = "mode=";
    if ( mode & PERMISSIVE ){
      result += "permissive,";
    }
    if ( mode & STRIP ){
      result += "strip,";
    }
    if ( mode & CHECKTEXT ){
      result += "checktext,";
    }
    if ( mode & FIXTEXT ){
      result += "fixtext,";
    }
    return result;
  }

  bool Document::set_strip( bool new_val ) const{
    bool old_val = (mode & STRIP);
    if ( new_val ){
      mode = Mode( (int)mode | STRIP );
    }
    else {
      mode = Mode( (int)mode & ~STRIP );
    }
    return old_val;
  }

  bool Document::set_permissive( bool new_val ) const{
    bool old_val = (mode & PERMISSIVE);
    if ( new_val ){
      mode = Mode( (int)mode | PERMISSIVE );
    }
    else {
      mode = Mode( (int)mode & ~PERMISSIVE );
    }
    return old_val;
  }

  bool Document::set_checktext( bool new_val ) const{
    bool old_val = (mode & CHECKTEXT);
    if ( new_val ){
      mode = Mode( (int)mode | CHECKTEXT );
    }
    else {
      mode = Mode( (int)mode & ~CHECKTEXT );
    }
    return old_val;
  }


  bool Document::set_fixtext( bool new_val ) const{
    bool old_val = (mode & FIXTEXT);
    if ( new_val ){
      mode = Mode( (int)mode | FIXTEXT );
    }
    else {
      mode = Mode( (int)mode & ~FIXTEXT );
    }
    return old_val;
  }


  void Document::addDocIndex( FoliaElement* el, const string& s ){
    if ( s.empty() ) {
      return;
    }
    auto it = sindex.find( s );
    if ( it == sindex.end() ){
      sindex[s] = el;
      iindex.push_back( el );
    }
    else {
      throw DuplicateIDError( s );
    }
  }

  void Document::delDocIndex( const FoliaElement* el, const string& s ){
    if ( sindex.empty() ){
      // only when ~Document is in progress
      return;
    }
    if ( s.empty() ) {
      return;
    }
    // cerr << _id << "-del docindex " << el << " (" << s << ")" << endl;
    sindex.erase(s);
    auto pos = iindex.begin();
    while( pos != iindex.end() ){
      if ( *pos == el ){
	iindex.erase( pos );
	return;
      }
      ++pos;
    }
  }

  static void error_sink(void *mydata, xmlError *error ){
    int *cnt = (int*)mydata;
    if ( *cnt == 0 ){
      string line = "\n";
      if ( error->file ){
	line += string(error->file) + ":";
	if ( error->line > 0 ){
	  line += TiCC::toString(error->line) + ":";
	}
      }
      line += " XML-error: " + string(error->message);
      cerr << line << endl;
    }
    (*cnt)++;
    return;
  }

  bool Document::readFromFile( const string& s ){
    ifstream is( s );
    if ( !is.good() ){
      throw runtime_error( "file not found: " + s );
    }
    if ( foliadoc ){
      throw runtime_error( "Document is already initialized" );
      return false;
    }
    _source_filename = s;
    if ( TiCC::match_back( s, ".bz2" ) ){
      string buffer = TiCC::bz2ReadFile( s );
      return readFromString( buffer );
    }
    int cnt = 0;
    xmlSetStructuredErrorFunc( &cnt, (xmlStructuredErrorFunc)error_sink );
    _xmldoc = xmlReadFile( s.c_str(),
			   0,
			   XML_PARSE_HUGE|XML_PARSE_NSCLEAN );
    if ( _xmldoc ){
      if ( cnt > 0 ){
	throw XmlError( "document is invalid" );
      }
      if ( debug ){
	cout << "read a doc from " << s << endl;
      }
      foliadoc = parseXml();
      if ( !validate_offsets() ){
	throw InconsistentText("MEH");
      }
      if ( debug ){
	if ( foliadoc ){
	  cout << "successful parsed the doc from: " << s << endl;
	}
	else {
	  cout << "failed to parse the doc from: " << s << endl;
	}
      }
      xmlFreeDoc( _xmldoc );
      _xmldoc = 0;
      return foliadoc != 0;
    }
    if ( debug ){
      cout << "Failed to read a doc from " << s << endl;
    }
    throw XmlError( "No valid FoLiA read" );
  }

  bool Document::readFromString( const string& s ){
    if ( foliadoc ){
      throw runtime_error( "Document is already initialized" );
      return false;
    }
    int cnt = 0;
    xmlSetStructuredErrorFunc( &cnt, (xmlStructuredErrorFunc)error_sink );
    _xmldoc = xmlReadMemory( s.c_str(), s.length(), 0, 0,
			     XML_PARSE_HUGE|XML_PARSE_NSCLEAN );
    if ( _xmldoc ){
      if ( cnt > 0 ){
	throw XmlError( "document is invalid" );
      }
      if ( debug ){
	cout << "read a doc from string" << endl;
      }
      foliadoc = parseXml();
      if ( !validate_offsets() ){
	throw InconsistentText("MEH");
      }
      if ( debug ){
	if ( foliadoc ){
	  cout << "successful parsed the doc" << endl;
	}
	else
	  cout << "failed to parse the doc" << endl;
      }
      xmlFreeDoc( _xmldoc );
      _xmldoc = 0;
      return foliadoc != 0;
    }
    if ( debug )
      cout << "Failed to read a doc from a string" << endl;
    return false;
  }

  ostream& operator<<( ostream& os, const Document *d ){
    if ( d ){
      os << d->toXml( "", d->strip() );
      // the toXml() string already ends with a newline (i hope....)
    }
    else {
      os << "MISSING DOCUMENT" << endl;
    }
    return os;
  }

  bool Document::save( ostream& os, const string& nsLabel, bool kanon ) const {
    os << toXml( nsLabel, ( kanon || strip() ) );
    // the toXml() string already ends with a newline (i hope....)
    return os.good();
  }

  bool Document::save( const string& fn, const string& nsLabel, bool kanon ) const {
    try {
      if ( TiCC::match_back( fn, ".bz2" ) ){
	string tmpname = fn.substr( 0, fn.length() - 3 ) + "tmp";
	if ( toXml( tmpname, nsLabel, ( kanon || strip() ) ) ){
	  bool stat = TiCC::bz2Compress( tmpname, fn );
	  remove( tmpname.c_str() );
	  return stat;
	}
	else {
	  return false;
	}
      }
      else {
	return toXml( fn, nsLabel,  ( kanon || strip() ) );
      }
    }
    catch ( const exception& e ){
      throw runtime_error( "saving to file " + fn + " failed: " + e.what() );
      return false;
    }
  }

  string Document::xmlstring( bool k ) const {
    xmlDoc *outDoc = to_xmlDoc( "", k );
    xmlChar *buf; int size;
    xmlDocDumpFormatMemoryEnc( outDoc, &buf, &size, "UTF-8", 0 ); // no formatting
    string result = string( (const char *)buf, size );
    xmlFree( buf );
    xmlFreeDoc( outDoc );
    _foliaNsOut = 0;
    return result;
  }

  int Document::size() const {
    if ( foliadoc )
      return foliadoc->size();
    else
      return 0;
  }

  FoliaElement* Document::index( const string& s ) const {
    const auto& it = sindex.find( s );
    if ( it == sindex.end() )
      return 0;
    else
      return it->second;
  }

  FoliaElement* Document::operator []( const string& s ) const {
    return index(s);
  }

  FoliaElement* Document::operator []( size_t i ) const {
    if ( i < iindex.size()-1 )
      return iindex[i+1];
    else
      throw range_error( "Document index out of range" );
  }

  UnicodeString Document::text( const std::string& cls,
				bool retaintok,
				bool strict ) const {
    return foliadoc->text( cls, retaintok, strict );
  }

  vector<Paragraph*> Document::paragraphs() const {
    return foliadoc->select<Paragraph>();
  }

  static const set<ElementType> quoteSet = { Quote_t };
  static const set<ElementType> emptySet;

  vector<Sentence*> Document::sentences() const {
    return foliadoc->select<Sentence>( quoteSet );
  }

  vector<Sentence*> Document::sentenceParts() const {
    vector<Sentence*> sents = foliadoc->select<Sentence>( emptySet );
    return sents;
  }

  Sentence *Document::sentences( size_t index ) const {
    vector<Sentence*> v = sentences();
    if ( index < v.size() ){
      return v[index];
    }
    else
      throw range_error( "sentences() index out of range" );
  }

  Sentence *Document::rsentences( size_t index ) const {
    vector<Sentence*> v = sentences();
    if ( index < v.size() ){
      return v[v.size()-1-index];
    }
    else
      throw range_error( "rsentences() index out of range" );
  }

  vector<Word*> Document::words() const {
    return foliadoc->select<Word>( default_ignore_structure );
  }

  Word *Document::words( size_t index ) const {
    vector<Word*> v = words();
    if ( index < v.size() ){
      return v[index];
    }
    else
      throw range_error( "words() index out of range" );
  }

  Word *Document::rwords( size_t index ) const {
    vector<Word*> v = words();
    if ( index < v.size() ){
      return v[v.size()-1-index];
    }
    else
      throw range_error( "rwords() index out of range" );
  }

  Paragraph *Document::paragraphs( size_t index ) const {
    vector<Paragraph*> v = paragraphs();
    if ( index < v.size() ){
      return v[index];
    }
    else
      throw range_error( "paragraphs() index out of range" );
  }

  Paragraph *Document::rparagraphs( size_t index ) const {
    vector<Paragraph*> v = paragraphs();
    if ( index < v.size() ){
      return v[v.size()-1-index];
    }
    else
      throw range_error( "rparagraphs() index out of range" );
  }

  std::string Document::language() const {
    string result;
    if ( _metadata ){
      result = _metadata->get_val("language");
    }
    return result;
  }

  std::string Document::metadatatype() const {
    if ( _metadata ){
      return _metadata->type();
    }
    return "native";
  }

  std::string Document::metadatafile() const {
    if ( _metadata && _metadata->datatype() == "ExternalMetaData" ){
      return _metadata->src();
    }
    return "";
  }

  void Document::setimdi( xmlNode *node ){
    xmlNode *n = TiCC::xPath( node, "//imdi:Session/imdi:Title" );
    if ( n ){
      _metadata->add_av( "title", TiCC::XmlContent( n ) );
    }
    n = TiCC::xPath( node, "//imdi:Session/imdi:Date" );
    if ( n ){
      _metadata->add_av( "date", TiCC::XmlContent( n ) );
    }
    n = TiCC::xPath( node, "//imdi:Source/imdi:Access/imdi:Publisher" );
    if ( n ){
      _metadata->add_av( "publisher", TiCC::XmlContent( n ) );
    }
    n = TiCC::xPath( node, "//imdi:Source/imdi:Access/imdi:Availability" );
    if ( n ){
      _metadata->add_av( "licence", TiCC::XmlContent( n ) );
    }
    n = TiCC::xPath( node, "//imdi:Languages/imdi:Language/imdi:ID" );
    if ( n ){
      _metadata->add_av( "language", TiCC::XmlContent( n ) );
    }
  }

  void Document::set_metadata( const string& type, const string& value ){
    if ( !_metadata ){
      _metadata = new NativeMetaData( "native" );
    }
    else if ( _metadata->datatype() != "NativeMetaData" ){
      throw MetaDataError( "cannot set '" + type + "=" + value +
			   "' on " +  _metadata->datatype() + "(" +
			   _metadata->type() + ")" );

    }
    _metadata->add_av( type, value );
  }

  const string Document::get_metadata( const string& type ) const {
    return _metadata->get_val( type );
  }

  processor *Document::get_processor( const string& pid ) const {
    if ( _provenance ){
      return _provenance->get_processor( pid );
    }
    else {
      return 0;
    }
  }

  processor *Document::add_processor( const KWargs& _args,
				      processor *parent ){
    KWargs args = _args;
    if ( debug ){
      cerr << "ADD_PROCESSOR: " << args << endl;
    }
    if ( !parent ){
      if ( !_provenance ){
	_provenance = new Provenance();
      }
    }
    auto it = args.find( "id" );
    if ( it == args.end() ){
      it = args.find( "xml:id" );
      if ( it == args.end() ){
	throw runtime_error( "cannot create a processor: missing 'xml:id' argument" );
      }
    }
    string pid = it->second;
    if ( pid == "next()" ){
      if ( debug ){
	cerr << "CALCULATE NEXT" << endl;
      }
      if ( parent ){
	if ( !parent->processors().empty() ){
	  string prev_id = parent->processors().back()->id();
	  if ( debug ){
	    cerr << "prev id = " << prev_id << endl;
	  }
	  vector<string> v = TiCC::split_at( prev_id, "." );
	  int val;
	  if ( TiCC::stringTo( v.back(), val ) ){
	    v.back() = TiCC::toString(++val);
	  }
	  else {
	    // not a number, just add .1 then, and pray
	    v.back() += ".1";
	  }
	  pid.clear();
	  for ( const auto& it :  v ){
	    pid += it + ".";
	  }
	  pid.pop_back();
	}
	else {
	  pid = parent->id() + ".1";
	}
	if ( debug ){
	  cerr << "next pid =" << pid << endl;
	}
      }
      else {
	throw runtime_error( "processor id=next() impossible. No parent" );
      }
    }
    processor *p = get_processor( pid );
    if ( p ){
      throw DuplicateIDError( "processor '" + pid + "' already exist" );
    }
    args["xml:id"] = pid;
    p = new processor( args );
    _provenance->add_index(p);
    if ( args.find("generator") == args.end() ){
      // we automagicly add a subprocessor.
      processor *sub = new processor();
      //      sub->set_system_defaults();
      sub->_folia_version = folia_version();
      sub->_version = library_version();
      sub->_id = p->_id + ".generator";
      sub->_type = GENERATOR;
      sub->_name = "libfolia";
      p->_processors.push_back(sub);
    }
    if ( parent ){
      parent->_processors.push_back( p );
    }
    else {
      _provenance->processors.push_back( p );
    }
    return p;
  }

  void Document::set_foreign_metadata( xmlNode *node ){
    if ( !_metadata ){
      _metadata = new ForeignMetaData( "foreign" );
    }
    else {
      if ( _metadata->datatype() != "ForeignMetaData" ){
	throw MetaDataError( "set_foreign_metadata now allowed for "
			     + _metadata->datatype() );
      }
    }
    ForeignData *add = new ForeignData();
    if ( TiCC::Name( node ) != "foreign-data" ){
      // we need an extra layer then
      xmlNode *n = TiCC::XmlNewNode( "foreign-data" );
      xmlAddChild( n, xmlCopyNode( node, 1 ) );
      add->set_data( n );
      _metadata->add_foreign( n );
      xmlFreeNode (n );
    }
    else {
      add->set_data( node );
      _metadata->add_foreign( node );
    }
  }

  void Document::parseannotations( const xmlNode *node ){
    if ( debug ){
      cerr << "parse annotations " << TiCC::Name(node) << endl;
    }
    xmlNode *n = node->children;
    _anno_sort.clear();
    while ( n ){
      string tag = TiCC::Name( n );
      if ( tag.length() > 11 && tag.substr( tag.length() - 11 ) == "-annotation" ){
	string prefix = tag.substr( 0,  tag.length() - 11 );
	AnnotationType::AnnotationType at_type
	  = TiCC::stringTo<AnnotationType::AnnotationType>( prefix );
	if ( debug ){
	  cerr << "parse " << prefix << "-annotation" << endl;
	}
	KWargs att = getAttributes( n );
	string st;
	string annotator;
	string ann_type;
	string format;
	string datetime;
	string alias;
	ElementType et = BASE;
	auto it = att.find("set" );
	if ( it != att.end() ){
	  st = it->second;
	}
	else if ( version_below( 1, 6 ) ){
	  st = "undefined"; // default value
	}
	else if ( at_type == AnnotationType::TEXT ){
	  if ( debug ){
	    cerr << "assign default for TEXT: " <<  DEFAULT_TEXT_SET << endl;
	  }
	  st = DEFAULT_TEXT_SET;
	}
	else if ( at_type == AnnotationType::PHON ){
	  if ( debug ){
	    cerr << "assign default for PHON: " <<  DEFAULT_PHON_SET << endl;
	  }
	  st = DEFAULT_PHON_SET;
	}
	else {
	  auto et_it = annotationtype_elementtype_map.find( at_type );
	  if ( et_it == annotationtype_elementtype_map.end() ){
	    throw logic_error( "no matching element_type for annotation_type: "
			       + prefix );
	  }
	  et = et_it->second;
	  FoliaElement *tmp = AbstractElement::createElement( et );
	  if ( tmp->required_attributes() & Attrib::CLASS ) {
	    delete tmp;
	    throw XmlError( "setname may not be empty for " + prefix
			    + "-annotation" );
	  }
	  delete tmp;
	}
	it = att.find( "format" );
	if ( it != att.end() ){
	  format = it->second;
	}
	it = att.find( "annotator" );
	if ( it != att.end() ){
	  annotator = it->second;
	}
	it = att.find( "annotatortype" );
	if ( it != att.end() ){
	  ann_type = it->second;
	}
	it = att.find( "datetime" );
	if ( it != att.end() ){
	  datetime = parseDate( it->second );
	}
	it = att.find( "alias" );
	if ( it != att.end() ){
	  alias = it->second;
	}
	it = att.find( "groupannotations" );
	if ( it != att.end() ){
	  if ( !isSubClass( et, AbstractSpanAnnotation_t ) ){
	    throw XmlError( "attribute 'groupannotations' not allowed for '"
			    + prefix + "-annotation" );
	  }
	  if ( it->second == "yes"
	       || it->second == "true" ){
	    _groupannotations[at_type][st] = true;
	  }
	  else {
	    throw XmlError( "invalid value '" + it->second
			    + "' for attribute groupannotations" );
	  }
	}
	else {
	  _groupannotations[at_type][st] = false;
	}
	set<string> processors;
	xmlNode *sub = n->children;
	while ( sub ){
	  string subtag = TiCC::Name( sub );
	  if ( debug ){
	    cerr << "parse subtag:" << subtag << endl;
	  }
	  if ( subtag == "annotator" ){
	    KWargs args = getAttributes( sub );
	    if ( args["processor"].empty() ){
	      throw XmlError( tag + "-annotation: <annotator> misses attribute 'processor'" );
	    }
	    processors.insert( args["processor"] );
	  }
	  sub = sub->next;
	}
	if ( !annotator.empty() && !processors.empty() ){
	  throw XmlError( tag + "-annotation: has both <annotator> node(s) and annotator attribute." );
	}
	declare( at_type, st, format, annotator, ann_type, datetime,
		 processors, alias );
      }
      n = n->next;
    }
    if ( debug ){
      cerr << "all group annotations: " << _groupannotations << endl;
    }
  }

  void processor::print( ostream& os, const int indent ) const {
    string space = string(indent,'\t');
    os << space << "name=" << _name << endl;
    os << space << "id=" << _id << endl;
    os << space << "version=" << _version << endl;
    os << space << "type=" << TiCC::toString(_type) << endl;
    os << space << "folia_version=" << _folia_version << endl;
    os << space << "document_version=" << _document_version << endl;
    os << space << "command=" << _command << endl;
    os << space << "host=" << _host << endl << endl;
    for( const auto& c : _processors ){
      c->print( os, indent+1 );
    }
  }

  ostream& operator<<( ostream& os, const processor& p ){
    p.print( os, 0 );
    return os;
  }

  ostream& operator<<( ostream& os, const processor *p ){
    if ( p ){
      p->print( os, 0 );
    }
    else {
      os << "NO PROCESSOR";
    }
    return os;
  }

  #include <sys/types.h>
  #include <sys/socket.h>
  #include <unistd.h>
  #include <netdb.h>
  string getfqdn( ){
    string result;
    struct addrinfo hints, *info, *p;
    int gai_result;

    char hostname[1024];
    hostname[1023] = '\0';
    gethostname(hostname, 1023);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; /*either IPV4 or IPV6*/
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_CANONNAME;

    if ((gai_result = getaddrinfo(hostname, "http", &hints, &info)) != 0) {
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(gai_result));
      exit(1);
    }

    for(p = info; p != NULL; p = p->ai_next) {
      //      printf("hostname: %s\n", p->ai_canonname);
      result = p->ai_canonname;
      break;
    }
    freeaddrinfo(info);
    return result;
  }

  processor::processor(){
  }

  processor::processor( const KWargs & args ){
    init(args);
  }

  string getNow() {
    time_t Time;
    time(&Time);
    tm curtime;
    localtime_r(&Time,&curtime);
    char buf[256];
    strftime( buf, 100, "%Y-%m-%dT%X", &curtime );
    string res = buf;
    return res;
  }

  string get_user(){
    string result;
    const char *env = getenv( "USER" );
    if ( env ){
      result = env;
    }
    return result;
  }

  void processor::get_system_defaults(){
    _host = getfqdn();
    _begindatetime = getNow();
    _folia_version = folia_version();
    _user = get_user();
  }

  void processor::init( const KWargs& atts ) {
    _type = AUTO;
    for ( const auto& att : atts ){
      if ( att.first == "begindatetime" ){
	_begindatetime = att.second;
      }
      else if ( att.first == "enddatetime" ){
	_enddatetime = att.second;
      }
      else if ( att.first == "id"
		|| att.first == "xml:id" ){
	_id = att.second;
      }
      else if ( att.first == "name" ){
	_name = att.second;
      }
      else if ( att.first == "version" ){
	_version = att.second;
      }
      else if ( att.first == "document_version" ){
	_document_version = att.second;
      }
      else if ( att.first == "command" ){
	_command = att.second;
      }
      else if ( att.first == "folia_version" ){
	_folia_version = att.second;
      }
      else if ( att.first == "type" ){
	try {
	  _type = TiCC::stringTo<AnnotatorType>( att.second );
	}
	catch (...){
	  throw XmlError( "processor: invalid value for 'type' attribute: "
			  + att.second );
	}
      }
      else if ( att.first == "host" ){
	_host = att.second;
      }
      else if ( att.first == "resourcelink" ){
	_resourcelink = att.second;
      }
      else if ( att.first == "user" ){
	_user = att.second;
      }
    }
    if ( _id.empty() ){
      throw XmlError( "processor: missing 'xml:id' attribute" );
    }
    if ( _name.empty() ){
      throw XmlError( "processor: missing 'name' attribute" );
    }
  }

  processor::~processor(){
    for ( const auto& p : _processors ){
      delete p;
    }
  }

  Provenance::~Provenance(){
    for ( const auto& p : processors ){
      delete p;
    }
  }

  processor *Provenance::get_processor( const string& pid ) const {
    const auto& p = _index.find( pid );
    if ( p != _index.end() ){
      return p->second;
    }
    else {
      return 0;
    }
  }

  void Provenance::add_index( processor *p ){
    _index[p->id()] = p;
  }

  processor *Provenance::parse_processor( const xmlNode *node ) {
    KWargs atts = getAttributes( node );
    processor *result = new processor( atts );
    add_index( result );
    xmlNode *n = node->children;
    while ( n ){
      string tag = TiCC::Name( n );
      if ( tag == "processor" ){
     	processor *p = parse_processor(n);
	result->_processors.push_back(p);
      }
      if ( tag == "meta" ){
	KWargs atts = getAttributes( n );
	string id = atts["id"];
	if ( id.empty() ){
	  throw XmlError( "processor: missing 'id' for meta tag" );
	}
	if ( atts.size() != 1 ){
	  throw XmlError( "processor: invalid attribute(s) in meta tag" );
	}
	string value = TiCC::XmlContent( n );
	result->_metadata[id] = value;
      }
      n = n->next;
    }
    return result;
  }

  ostream& operator<<( ostream& os, const Provenance& p ){
    os << "provenance data" << endl;
    for ( const auto& pr : p.processors ){
      pr->print( os, 2 );
      os << endl;
    }
    return os;
  }

  ostream& operator<<( ostream& os, const Provenance* p ){
    if ( p ){
      os << *p;
    }
    else {
      os << "no provenance";
    }
    return os;
  }

  void Document::parseprovenance( const xmlNode *node ){
    Provenance *result = new Provenance();
    xmlNode *n = node->children;
    while ( n ){
      string tag = TiCC::Name( n );
      if ( tag == "processor" ){
	result->processors.push_back( result->parse_processor(n) );
      }
      n = n->next;
    }
    _provenance = result;
  }

  void Document::parsesubmeta( const xmlNode *node ){
    if ( node ){
      KWargs att = getAttributes( node );
      string id = att["xml:id"];
      if ( id.empty() ){
	throw MetaDataError( "submetadata without xml:id" );
      }
      //      cerr << "parse submetadata, id=" << id << endl;
      string type = att["type"];
      //      cerr << "parse submetadata, type=" << type << endl;
      if ( type.empty() ){
	type = "native";
      }
      string src = att["src"];
      if ( !src.empty() ){
	submetadata[id] = new ExternalMetaData( type, src );
	//	cerr << "created External metadata, id=" << id << endl;
      }
      else if ( type == "native" ){
	submetadata[id] = new NativeMetaData( type );
	//	cerr << "created Native metadata, id=" << id << endl;
      }
      else {
	submetadata[id] = 0;
	//	cerr << "set metadata to 0, id=" << id << endl;
      }
      xmlNode *p = node->children;
      while ( p ){
	if ( p->type == XML_ELEMENT_NODE ){
	  if ( TiCC::Name(p) == "meta" &&
	       checkNS( p, NSFOLIA ) ){
	    if ( type == "native" ){
	      string txt = TiCC::XmlContent( p );
	      KWargs att = getAttributes( p );
	      string sid = att["id"];
	      if ( !txt.empty() ){
		submetadata[id]->add_av( sid, txt );
		// cerr << "added node to id=" << id
		//      << "(" << sid << "," << txt << ")" << endl;
	      }
	    }
	    else {
	      throw MetaDataError("Encountered a meta element but metadata type is not native!");
	    }
	  }
	  else if ( TiCC::Name(p) == "foreign-data" &&
		    checkNS( p, NSFOLIA ) ){
	    if ( type == "native" ){
	      throw MetaDataError("Encountered a foreign-data element but metadata type is native!");
	    }
	    else if ( submetadata[id] == 0 ){
	      submetadata[id] = new ForeignMetaData( type );
	      //	      cerr << "add new Foreign " << id << endl;
	    }
	    //	    cerr << "in  Foreign " << submetadata[id]->type() << endl;
	    submetadata[id]->add_foreign( p );
	    //	    cerr << "added a foreign id=" << id << endl;
	  }
	}
	p = p->next;
      }
    }
  }

  bool is_number( const string& s ){
    for ( const auto& c : s ){
      if ( !isdigit(c) ){
	return false;
      }
    }
    return true;
  }

  void expand_version_string( const string& vs,
			      int& major,
			      int& minor,
			      int& sub,
			      string& patch ){
    // expand string @vs into int major, minor and subvalue
    major = 0;
    minor = 0;
    sub = 0;
    patch.clear();
    vector<string> vec = TiCC::split_at( vs, ".", 3 );
    for ( size_t i=0; i < vec.size(); ++i ){
      if ( i == 0 ){
	int val = 0;
	if ( !TiCC::stringTo( vec[i], val ) ){
	  throw XmlError( "unable to extract major-version from: " + vs );
	}
	major= val;
      }
      else if ( i == 1 ){
	int val = 0;
	if ( !TiCC::stringTo( vec[i], val ) ){
	  throw XmlError( "unable to extract minor-version from: " + vs );
	}
	minor = val;
      }
      else if ( i == 2 ){
	if ( is_number( vec[i] ) ){
	  TiCC::stringTo( vec[i], sub );
	}
	else {
	  vector<string> v2 = TiCC::split_at( vec[i], "-", 2 );
	  if ( v2.size() != 2 ){
	    throw XmlError( "invalid sub-version or patch-version in: " + vs );
	  }
	  else {
	    int val = 0;
	    if ( !TiCC::stringTo( v2[0], val ) ){
	      throw XmlError( "unable to extract sub-version from: " + vs );
	    }
	    sub = val;
	    patch = "-" + v2[1]; // include the hyphen
	  }
	}
      }
    }
  }

  int check_version( const string& vers ){
    int maj = 0;
    int min = 0;
    int sub = 0;
    string patch;
    expand_version_string( vers, maj, min, sub, patch );
    if ( maj < MAJOR_VERSION ){
      return -1;
    }
    else if ( maj > MAJOR_VERSION ){
      return 1;
    }
    else if ( min < MINOR_VERSION ){
      return -1;
    }
    else if ( min > MINOR_VERSION ){
      return 1;
    }
    else if ( sub < SUB_VERSION ){
      return -1;
    }
    else if ( sub > SUB_VERSION ){
      return 1;
    }
    return 0;
  }

  int Document::compare_to_lib_version() const {
    return check_version( version() );
  }

  bool Document::version_below( int major, int minor ){
    // check if current document version is strict < major.minor
    if ( major_version < major ){
      return true;
    }
    else if ( major_version == major ){
      return minor_version < minor;
    }
    return false;
  }

  void Document::adjustTextMode(){
    const char *env = getenv( "FOLIA_TEXT_CHECK" );
    if ( env ){
      string e = env;
      delete env;
      cerr << "DETECTED FOLIA_TEXT_CHECK environment variable, value ='"
	   << e << "'"<< endl;
      if ( e == "NO" ){
	mode = Mode( int(mode) & ~CHECKTEXT );
	cerr << "FOLIA_TEXT_CHECK disabled" << endl;
      }
      else if ( e == "YES" ){
	mode = Mode( int(mode) | CHECKTEXT );
	cerr << "FOLIA_TEXT_CHECK enabled" << endl;
      }
      else {
	cerr << "FOLIA_TEXT_CHECK unchanged:" << (checktext()?"YES":"NO")
	     << endl;
      }
    }
    if ( !( mode & FIXTEXT) && version_below( 1, 5 ) ){
      // don't check text consistency for older documents
      mode = Mode( int(mode) & ~CHECKTEXT );
    }
    else if ( !env) {
      mode = Mode( int(mode) | CHECKTEXT );
    }
  }

  void Document::setDocumentProps( KWargs& kwargs ){
    auto it = kwargs.find( "version" );
    if ( it != kwargs.end() ){
      _version_string = it->second;
      //      cerr << "So we found version " << _version_string << endl;
      kwargs.erase( it );
    }
    else {
      // assign a 'random' version, but PRE 1.5
      _version_string = "1.4.987";
      //      cerr << "NO VERSION version " << _version_string << endl;
    }
    expand_version_string( _version_string,
			   major_version,
			   minor_version,
			   sub_version,
			   patch_version );
    if ( check_version( _version_string ) > 0 ){
      cerr << "WARNING!!! the Document is created for newer FoLiA version than this library ("
	   << _version_string << " vs " << folia_version()
	   << ")\n\t Any possible subsequent failures in parsing or processing may probably be attributed to this." << endl
	   << "\t Please upgrade libfolia!" << endl;
    }
    adjustTextMode();
    it = kwargs.find( "external" );
    if ( it != kwargs.end() ){
      external = TiCC::stringTo<bool>( it->second );
      kwargs.erase( it );
    }
    else {
      external = false;
    }
    bool happy = false;
    it = kwargs.find( "_id" ); // for backward compatibility
    if ( it == kwargs.end() ){
      it = kwargs.find( "xml:id" );
    }
    if ( it != kwargs.end() ){
      if ( isNCName( it->second ) ){
	_id = it->second;
      }
      else {
	throw XmlError( "'" + it->second + "' is not a valid NCName." );
      }
      happy = true;
    }
    if ( !foliadoc && !happy ){
      throw runtime_error( "No Document ID specified" );
    }
    kwargs.erase( "generator" ); // also delete unused att-val(s)
  }

  void Document::resolveExternals(){
    if ( !externals.empty() ){
      for ( const auto& ext : externals ){
	ext->resolve_external();
      }
    }
  }

  void Document::parse_metadata( const xmlNode *p ){
    MetaData *result = 0;
    KWargs atts = getAttributes( p );
    string type = TiCC::lowercase(atts["type"]);
    if ( type.empty() ){
      type = "native";
    }
    string src = atts["src"];
    if ( !src.empty() ){
      result = new ExternalMetaData( type, src );
    }
    else if ( type == "native" ){
      result = new NativeMetaData( type );
    }
    xmlNode *m = p->children;
    xmlNode *a_node = 0;
    while ( m ){
      if ( TiCC::Name(m)  == "METATRANSCRIPT" ){
	if ( !checkNS( m, NSIMDI ) || type != "imdi" )
	  throw runtime_error( "imdi != imdi " );
	if ( debug > 1 ){
	  cerr << "found IMDI" << endl;
	}
	if ( !result ){
	  result = new ForeignMetaData( "imdi" );
	}
	result->add_foreign( xmlCopyNode(m,1) );
      }
      else if ( TiCC::Name( m ) == "annotations" &&
		checkNS( m, NSFOLIA ) ){
	if ( debug > 1 ){
	  cerr << "found annotations" << endl;
	}
	// defer parsing until AFTER provenance data
	a_node = m;
      }
      else if ( TiCC::Name( m ) == "provenance" &&
		checkNS( m, NSFOLIA ) ){
	//	if ( debug > 1 ){
	//	cerr << "found provenance data" << endl;
	//	}
	parseprovenance( m );
	//	cerr << _provenance << endl;
      }
      else if ( TiCC::Name( m ) == "meta" &&
		checkNS( m, NSFOLIA ) ){
	if ( debug > 1 ){
	  cerr << "found meta node" << endl;
	}
	if ( !result ){
	  throw runtime_error( "'meta' tag found outside a metadata block" );
	}
	KWargs att = getAttributes( m );
	string type = att["id"];
	string val = TiCC::XmlContent( m );
	string get = result->get_val( type );
	if ( !get.empty() ){
	  throw runtime_error( "meta tag with id=" + type
			       + " is defined more then once " );
	}
	result->add_av( type, val );
      }
      else if ( TiCC::Name(m)  == "foreign-data" &&
		checkNS( m, NSFOLIA ) ){
	FoliaElement *t = AbstractElement::createElement( "foreign-data", this );
	if ( t ){
	  t = t->parseXml( m );
	  if ( t ){
	    if ( result && result->datatype() == "NativeMetaData" ){
	      cerr << "WARNING: foreign-data found in metadata of type 'native'"  << endl;
	      cerr << "changing type to 'foreign'" << endl;
	      type = "foreign";
	      delete result;
	      result = new ForeignMetaData( type );
	    }
	    if ( !result ){
	      result = new ForeignMetaData( type );
	    }
	    result->add_foreign( m );
	  }
	}
      }
      else if ( TiCC::Name(m)  == "submetadata" &&
		checkNS( m, NSFOLIA ) ){
	parsesubmeta( m );
      }
      m = m->next;
    }
    if ( a_node ){
      //      cerr << "parse deferred annotations" << endl;
      parseannotations( a_node );
    }
    if ( result == 0 && type == "imdi" ){
      // imdi missing all further info
      result = new NativeMetaData( type );
    }
    _metadata = result;
  }

  void Document::addStyle( const string& type, const string& href ){
    if ( type == "text/xsl" ){
      const auto& it = styles.find( type );
      if ( it != styles.end() )
	throw XmlError( "multiple 'text/xsl' style-sheets defined." );
    }
    styles.insert( make_pair( type, href ) );
  }

  void Document::replaceStyle( const string& type,
			       const string& href ){
    const auto& it = styles.find( type );
    if ( it != styles.end() ){
      it->second = href;
    }
    else {
      styles.insert( make_pair( type, href ) );
    }
  }

  void Document::getstyles(){
    xmlNode *pnt = _xmldoc->children;
    while ( pnt ){
      if ( pnt->type == XML_PI_NODE && TiCC::Name(pnt) == "xml-stylesheet" ){
	string content = (const char*)pnt->content;
	string type;
	string href;
	vector<string> v = TiCC::split( content );
	if ( v.size() == 2 ){
	  vector<string> w = TiCC::split_at( v[0], "=" );
	  if ( w.size() == 2 && w[0] == "type" ){
	    type = w[1].substr(1,w[1].length()-2);
	  }
	  w = TiCC::split_at( v[1], "=" );
	  if ( w.size() == 2 && w[0] == "href" ){
	    href = w[1].substr(1,w[1].length()-2);
	  }
	}
	if ( !type.empty() && !href.empty() ){
	  addStyle( type, href );
	}
	else {
	  throw XmlError( "problem parsing line: " + content );
	}
      }
      pnt = pnt->next;
    }
  }

  void fixupNs( xmlNode *p, xmlNs *ns ){
    while ( p ){
      xmlSetNs( p, ns );
      fixupNs( p->children, ns );
      p = p->next;
    }
  }

  bool Document::validate_offsets() const {
    set<TextContent*> t_done;
    for ( const auto& txt : t_offset_validation_buffer ){
      if ( t_done.find( txt ) != t_done.end() ){
	continue;
      }
      t_done.insert(txt);
      int offset = txt->offset();
      if ( offset != -1 ){
	try {
	  txt->getreference();
	}
	catch( UnresolvableTextContent& e ){
	  string msg = "Text for " + txt->parent()->xmltag() + "(ID="
	    + txt->parent()->id() + ", textclass='" + txt->cls()
	    + "'), has incorrect offset " + TiCC::toString(offset);
	  string ref = txt->ref();
	  if ( !ref.empty() ){
	    msg += " or invalid reference:" + ref;
	  }
	  msg += "\n\toriginal msg=";
	  msg += e.what();
	  throw UnresolvableTextContent( msg );
	}
      }
    }
    set<PhonContent*> p_done;
    for ( const auto& phon : p_offset_validation_buffer ){
      if ( p_done.find( phon ) != p_done.end() ){
	continue;
      }
      p_done.insert(phon);
      int offset = phon->offset();
      if ( offset != -1 ){
	try {
	  phon->getreference();
	}
	catch( UnresolvableTextContent& e ){
	  string msg = "Phoneme for " + phon->parent()->xmltag() + ", ID="
	    + phon->parent()->id() + ", textclass='" + phon->cls()
	    + "', has incorrect offset " + TiCC::toString(offset);
	  string ref = phon->ref();
	  if ( !ref.empty() ){
	    msg += " or invalid reference:" + ref;
	  }
	  msg += "\n\toriginal msg=";
	  msg += e.what();
	  throw UnresolvableTextContent( msg );
	}
      }
    }
    return true;
  }

  FoliaElement* Document::parseXml( ){
    getstyles();
    xmlNode *root = xmlDocGetRootElement( _xmldoc );
    if ( root->ns ){
      if ( root->ns->prefix )
	_foliaNsIn_prefix = xmlStrdup( root->ns->prefix );
      _foliaNsIn_href = xmlStrdup( root->ns->href );
    }
    if ( debug > 2 ){
      string dum;
      cerr << "root = " << TiCC::Name( root ) << endl;
      cerr << "in namespace " << TiCC::getNS( root, dum ) << endl;
      cerr << "namespace list" << getNS_definitions( root ) << endl;
    }
    FoliaElement *result = 0;
    if ( root  ){
      if ( TiCC::Name( root ) == "FoLiA" ){
	string ns = TiCC::getNS( root );
	if ( ns.empty() ){
	  if ( permissive() ){
	    _foliaNsIn_href = xmlCharStrdup( NSFOLIA.c_str() );
	    _foliaNsIn_prefix = 0;
	    xmlNs *defNs = xmlNewNs( root,
				     _foliaNsIn_href, _foliaNsIn_prefix );
	    fixupNs( root, defNs );
	  }
	  else {
	    throw XmlError( "Folia Document should have namespace declaration "
			    + NSFOLIA + " but none found " );
	  }
	}
	else if ( ns != NSFOLIA ){
	  throw XmlError( "Folia Document should have namespace declaration "
			  + NSFOLIA + " but found: " + ns );
	}
	try {
	  FoLiA *folia = new FoLiA( this );
	  result = folia->parseXml( root );
	  resolveExternals();
	}
	catch ( InconsistentText& e ){
	  throw;
	}
	catch ( XmlError& e ){
	  throw;
	}
	catch ( exception& e ){
	  throw XmlError( e.what() );
	}
      }
      else if ( TiCC::Name( root ) == "DCOI" &&
		checkNS( root, NSDCOI ) ){
	throw XmlError( "DCOI format not supported" );
      }
      else {
	throw XmlError( "root node must be FoLiA" );
      }
    }
    return result;
  }

  void Document::declare( AnnotationType::AnnotationType type,
			  const string& setname,
			  const string& args ){
    KWargs kwargs = getArgs( args );
    return declare( type, setname, kwargs );
  }

  void Document::declare( AnnotationType::AnnotationType type,
			  const string& setname,
			  const KWargs& _args ){
    KWargs args = _args;
    if ( debug ){
      cerr << "declare( " << TiCC::toString(type) << "," << setname << ", ["
	   << args << "] )" << endl;
    }
    string st = setname;
    if ( st.empty() ){
      if ( version_below( 1, 6 ) ){
	st = "undefined";
      }
      else {
	throw XmlError( "setname may not be empty for " + toString(type)
			+ "-annotation" );
      }
    }
    set<string> processors;
    string a = args["annotator"];
    string t = args["annotatortype"];
    string f = args["format"];
    string d = args["datetime"];
    string alias = args["alias"];
    string processor = args["processor"];
    if ( !processor.empty() ){
      processors.insert( processor );
    }
    args.erase("annotator");
    args.erase("annotatortype");
    args.erase("format");
    args.erase("datetime");
    args.erase("alias");
    args.erase("processor");
    if ( args.size() != 0 ){
      throw XmlError( "declaration: expected 'annotator', 'annotatortype', 'processor', 'alias' or 'datetime', got '" + args.begin()->first + "'" );
    }
    declare( type, st, f, a, t, d, processors, alias );
  }

  string Document::unalias( AnnotationType::AnnotationType type,
			    const string& alias ) const {
    const auto& ti = _alias_set.find(type);
    if ( ti != _alias_set.end() ){
      const auto& sti = ti->second.find( alias );
      if ( sti != ti->second.end() ){
	return sti->second;
      }
    }
    return alias;
  }

  string Document::alias( AnnotationType::AnnotationType type,
			  const string& st ) const {
    const auto& ti = _set_alias.find(type);
    if ( ti != _set_alias.end() ){
      const auto& ali = ti->second.find( st );
      if ( ali != ti->second.end() ){
	return ali->second;
      }
    }
    return st;
  }

  void Document::declare( AnnotationType::AnnotationType type,
			  const string& setname,
			  const string& format,
			  const string& annotator,
			  const string& annotator_type,
			  const string& date_time,
			  const set<string>& processors,
			  const string& _alias ){
    if ( debug ){
      cerr << "declare( " << TiCC::toString(type) << "," << setname
	   << ", format=" << format << "," << annotator << ","
	   << annotator_type << "," << date_time << "," << _alias << ","
	   << processors << ") " << endl;
    }
    AnnotatorType ant = UNDEFINED;
    try {
      ant = TiCC::stringTo<AnnotatorType>( annotator_type );
    }
    catch (...) {
      throw XmlError( "declare(): illegal value '"
		      + annotator_type + "' for annotator type" );
    }
    if ( !_alias.empty() ){
      string set_ali = alias(type,setname);
      if ( !set_ali.empty() ){
	if ( set_ali != setname
	     && set_ali != _alias ){
	  throw XmlError( "setname: '" + setname + "' already has an alias: '"
			  + set_ali );
	}
      }
      string ali_ali = alias(type,_alias);
      string ali_set = unalias(type,_alias);
      if ( ali_ali != _alias ){
	throw XmlError( "alias: '" + _alias +
			"' is also in use as a setname for set:'"
			+ ali_set + "'" );
      }
      if ( ali_set != _alias
	   && ali_set != setname ){
	throw XmlError( "alias: '" + _alias + "' already used for setname: '"
			+ ali_set + "'" );
      }
    }
    if ( !isDeclared( type, setname, annotator, ant, processors ) ){
      if ( !unalias(type,setname).empty()
	   && unalias(type,setname) != setname ){
	throw XmlError( "setname: '" + setname
			+ "' is also in use as an alias" );
      }
      string d = date_time;
      if ( d == "now()" ){
	d = getNow();
      }
      if ( processors.empty() ){
	// old style
	_annotationdefaults[type].insert( make_pair( setname,
						     at_t(annotator,ant,d,format,processors) ) );
      }
      else {
	// new style
	auto set_pos = _annotationdefaults[type].find(setname);
	if ( set_pos == _annotationdefaults[type].end() ){
	  // no processer annotations yet
	  _annotationdefaults[type].insert( make_pair( setname,
						       at_t(annotator,ant,d,format,processors) ) );

	}
	else {
	  // add to the existing
	  for ( const auto& p : processors ){
	    set_pos->second.p.insert( p );
	  }
	}
      }
      _anno_sort.push_back(make_pair(type,setname));
      _annotationrefs[type][setname] = 0;
      if ( !_alias.empty() ){
	_alias_set[type][_alias] = setname;
	_set_alias[type][setname] = _alias;
      }
      else {
	_alias_set[type][setname] = setname;
	_set_alias[type][setname] = setname;
      }
    }
  }

  void Document::un_declare( AnnotationType::AnnotationType type,
			     const string& set_name ){
    string setname = unalias(type,set_name);
    if ( _annotationrefs[type][setname] != 0 ){
      throw XmlError( "unable to undeclare " + toString(type) + "-type("
		      + setname + ") (references remain)" );
    }
    auto const adt = _annotationdefaults.find(type);
    if ( adt != _annotationdefaults.end() ){
      auto it = adt->second.begin();
      while ( it != adt->second.end() ){
	if ( setname.empty() || it->first == setname ){
	  it = adt->second.erase(it);
	}
	else {
	  ++it;
	}
      }
      auto it2 = _anno_sort.begin();
      while ( it2 != _anno_sort.end() ){
	if ( it2->first == type && it2->second == setname ){
	  it2 = _anno_sort.erase( it2 );
	}
	else {
	  ++it2;
	}
      }
      auto it3 = _alias_set[type].begin();
      while ( it3 != _alias_set[type].end() ){
	if ( it3->first == setname || it3->second == setname ){
	  it3 = _alias_set[type].erase( it3 );
	}
	else {
	  ++it3;
	}
      }
      auto it4 = _set_alias[type].begin();
      while ( it4 != _set_alias[type].end() ){
	if ( it4->first == setname || it4->second == setname ){
	  it4 = _set_alias[type].erase( it4 );
	}
	else {
	  ++it4;
	}
      }
    }
  }

  multimap<AnnotationType::AnnotationType, string> Document::unused_declarations( ) const {
    multimap<AnnotationType::AnnotationType,string> result;
    for ( const auto& tit : _annotationrefs ){
      for ( const auto& mit : tit.second ){
	if ( mit.second == 0 ){
	  result.insert( make_pair(tit.first, mit.first ) );
	}
      }
    }
    return result;
  }

  Text* Document::setTextRoot( const KWargs& args ) {
    Text *t = new Text( args );
    foliadoc->append( t );
    return t;
  }

  Text* Document::setTextRoot() {
    KWargs empty;
    return setTextRoot( empty );
  }

  Speech* Document::setSpeechRoot( const KWargs& args ) {
    Speech *s = new Speech( args );
    foliadoc->append( s );
    return s;
  }

  Speech* Document::setSpeechRoot() {
    KWargs empty;
    return setSpeechRoot( empty );
  }

  FoliaElement* Document::append( FoliaElement *t ){
    if ( t->element_id() == Text_t
	 || t->element_id() == Speech_t ) {
      foliadoc->append( t );
      return t;
    }
    throw XmlError( "Only can append 'text' or 'speech' as root of a Document." );
  }

  bool Document::isDeclared( const AnnotationType::AnnotationType& type,
			     const string& set_name,
			     const string& annotator,
			     const AnnotatorType& annotator_type,
			     const string& processor ) const {
    if ( debug ){
      cerr << "isdeclared? ( " << toString(type) << "," << set_name << ","
	   << annotator << "," << toString(annotator_type) << "," << processor
	   << ") " << endl;
    }
    //
    // We DO NOT check the date. if all parameters match, it is OK
    //
    // if ( set_name.empty() ){
    //   return true;
    //   throw runtime_error("isDeclared called with empty set.");
    // }
    if ( type == AnnotationType::NO_ANN ){
      if ( debug ){
	cerr << "\t\t TRUE want NO_ANN" << endl;
      }
      return true;
    }
    if ( !processor.empty()
	 && !get_processor( processor ) ){
      throw XmlError( toString(type)
		      + "-annotation is referring an undefined processor '"
		      + processor + "'" );
    }
    string setname = unalias(type,set_name);
    const auto& it1 = _annotationdefaults.find(type);
    if ( it1 != _annotationdefaults.end() ){
      if ( debug > 2){
	cerr << "OK, found an entry for type: " << toString(type) << endl;
      }
      auto mit2 = it1->second.lower_bound(setname);
      while ( mit2 != it1->second.upper_bound(setname) ){
	if ( debug > 2){
	  cerr << "OK, found an entry for set='" << setname  << "'" << endl;
	  cerr << "content: " << mit2->second << endl;
	}
	if ( mit2->second.a == annotator
	     && mit2->second.t == annotator_type
	     && ( mit2->second.p.empty()
		  || mit2->second.p.find(processor) != mit2->second.p.end() ) ){
	  if ( debug ){
	    cerr << "\t\t isDeclared ==> TRUE" << endl;
	  }
	  return true;
	}
	++mit2;
      }
    }
    if ( debug ){
      cerr << "\t\t isDeclared() ==> FALSE" << endl;
    }
    return false;
  }

  bool Document::isDeclared( const AnnotationType::AnnotationType& type,
			     const string& set_name,
			     const string& annotator,
			     const AnnotatorType& annotator_type,
			     const set<string>& processors ) const {
    if ( processors.empty() ){
      return isDeclared( type, set_name, annotator, annotator_type, "" );
    }
    else {
      for ( const auto& s : processors ){
	if ( isDeclared( type, set_name, annotator, annotator_type, s ) ){
	  return true;
	}
      }
      return false;
    }
  }

  void Document::incrRef( AnnotationType::AnnotationType type,
			  const string& s ){
    if ( type != AnnotationType::NO_ANN ){
      string st = s;
      if ( st.empty() ){
	st = defaultset(type);
      }
      ++_annotationrefs[type][st];
      //      cerr << "increment " << toString(type) << "(" << st << ")" << endl;
    }
  }

  void Document::decrRef( AnnotationType::AnnotationType type,
			  const string& s ){
    if ( type != AnnotationType::NO_ANN ){
      --_annotationrefs[type][s];
      //      cerr << "decrement " << toString(type) << "(" << s << ")" << endl;
    }
  }

  bool Document::isDeclared( const AnnotationType::AnnotationType& type,
			     const string& setname ) const {
    if ( type == AnnotationType::NO_ANN ){
      return true;
    }
    const auto& mit1 = _annotationdefaults.find(type);
    if ( mit1 != _annotationdefaults.end() ){
      // cerr << "found some for: " << toString(type) << endl;
      // cerr << mit1->second << endl;
      if ( setname.empty() ){
	return true;
      }
      string set_name = unalias(type,setname);
      //      cerr << "lookup: " << setname << " (" << set_name << ")" << endl;
      const auto& mit2 = mit1->second.find(set_name);
      return mit2 != mit1->second.end();
    }
    return false;
  }

  bool Document::declared( ElementType et,
			   const string& setname ) const {
    FoliaElement *tmp = AbstractElement::createElement( et );
    AnnotationType::AnnotationType at = tmp->annotation_type();
    delete tmp;
    return isDeclared( at, setname );
  }

  string Document::defaultset( AnnotationType::AnnotationType type ) const {
    if ( type == AnnotationType::NO_ANN )
      return "";
    // search a set. it must be unique. Otherwise return ""
    // cerr << "zoek voor '" << toString(type) << "' de default set in:" << endl
    // 	 <<  _annotationdefaults << endl;
    string result;
    const auto& mit1 = _annotationdefaults.find(type);
    if ( mit1 != _annotationdefaults.end() ){
      //      cerr << "vind tussen " <<  mit1->second << endl;
      if ( mit1->second.size() == 1 ){
	result = mit1->second.begin()->first;
      }
    }
    //    cerr << "defaultset ==> " << result << endl;
    return result;
  }

  vector<string> Document::getannotators( AnnotationType::AnnotationType type,
					  const string& st ) const {
    vector<string> result;
    if ( type == AnnotationType::NO_ANN ){
      return result;
    }
    const auto& mit1 = _annotationdefaults.find(type);
    if ( mit1 != _annotationdefaults.end() ){
      //      cerr << "vond iets voor " << toString(type) << endl;
      for ( auto pos = mit1->second.lower_bound(st);
	    pos != mit1->second.upper_bound(st);
	    ++pos ){
	for ( const auto& p : pos->second.p ){
	  result.push_back( p );
	}
      }
    }
    //    cerr << "get default ==> " << result << endl;
    return result;

  }

  string Document::defaultannotator( AnnotationType::AnnotationType type,
				     const string& st ) const {
    if ( type == AnnotationType::NO_ANN ){
      return "";
    }
    // if ( !st.empty() ){
    //   cerr << "zoek '" << st << "' default annotator " <<  _annotationdefaults << endl;
    // }
    const auto& mit1 = _annotationdefaults.find(type);
    string result;
    if ( mit1 != _annotationdefaults.end() ){
      //      cerr << "vind tussen " <<  mit1->second << endl;
      if ( st.empty() ){
	if ( mit1->second.size() == 1 ){
	  result = mit1->second.begin()->second.a;
	  return result;
	}
      }
      else {
	if ( mit1->second.count( st ) == 1 ){
	  const auto& mit2 = mit1->second.find( st );
	  result = mit2->second.a;
	}
      }
    }
    //    cerr << "get default ==> " << result << endl;
    return result;
  }

  AnnotatorType Document::defaultannotatortype( AnnotationType::AnnotationType type,
						const string& st ) const {
    if ( debug ){
      cerr << "annotationdefaults= " <<  _annotationdefaults << endl;
      cerr << "lookup: " << TiCC::toString(type) << endl;
    }
    AnnotatorType result = UNDEFINED;
    if ( type == AnnotationType::NO_ANN ){
      return result;
    }
    const auto& mit1 = _annotationdefaults.find(type);
    if ( mit1 != _annotationdefaults.end() ){
      if ( debug ){
	cerr << "found a hit for type=" << TiCC::toString( type ) << endl;
      }
      if ( st.empty() ){
	if ( mit1->second.size() == 1 ){
	  result = mit1->second.begin()->second.t;
	}
	return result;
      }
      else {
	if ( mit1->second.count( st ) == 1 ){
	  const auto& mit2 = mit1->second.find( st );
	  result = mit2->second.t;
	}
      }
    }
    //  cerr << "get default ==> " << result << endl;
    return result;
  }

  string Document::defaultdatetime( AnnotationType::AnnotationType type,
				    const string& st ) const {
    const auto& mit1 = _annotationdefaults.find(type);
    string result;
    if ( mit1 != _annotationdefaults.end() ){
      if ( st.empty() ){
	if ( mit1->second.size() == 1 )
	  result = mit1->second.begin()->second.d;
	return result;
      }
      else {
	if ( mit1->second.count( st ) == 1 ){
	  const auto& mit2 = mit1->second.find( st );
	  result = mit2->second.d;
	}
      }
    }
    //  cerr << "get default ==> " << result << endl;
    return result;
  }

  string Document::defaultprocessor( AnnotationType::AnnotationType annotationtype,
				     const string& set_name ) const{
    if ( debug ){
      cerr << "defaultprocessor(" << toString( annotationtype ) << ","
	   << set_name << ")" << endl;
    }
    auto const& it = _annotationdefaults.find(annotationtype);
    if ( it != _annotationdefaults.end() ){
      if ( debug ){
	cerr << "found some defs: " << it->second << endl;
      }
      set<string> results;
      auto s_it = it->second.find(set_name);
      while ( s_it != it->second.end() ){
	if ( debug ){
	  cerr << "found sub strings: " << s_it->second << endl;
	}
	results.insert( s_it->second.p.begin(), s_it->second.p.end() );
	++s_it;
      }
      if ( results.size() == 1 ){
	return *results.begin();
      }
      else if ( results.size() > 1 ){
	auto const& as = annotationtype_xml_map.find(annotationtype);
	if ( as != annotationtype_xml_map.end() ){
	  throw NoDefaultError("No processor specified for <"
			       + as->second +  ">, but the presence of multiple declarations prevent assigning a default");
	}
      }
    }
    return "";
  }

  vector<const processor*> Document::getprocessors( AnnotationType::AnnotationType type,
						    const string& st ) const {
    vector<const processor*> result;
    if ( debug ){
      cerr << "getprocessors(" << toString( type ) << ","
	   << st << ")" << endl;
    }
    if ( type == AnnotationType::NO_ANN ){
      return result;
    }
    auto const& it = _annotationdefaults.find(type);
    if ( it != _annotationdefaults.end() ){
      if ( debug ){
	cerr << "found some defs: " << it->second << endl;
      }
      for ( auto pos = it->second.lower_bound(st);
	    pos != it->second.upper_bound(st);
	    ++pos ){
	for ( const auto& p : pos->second.p ){
	  result.push_back( get_processor(p) );
	}
      }
    }
    return result;
  }

  void Document::setannotations( xmlNode *md ) const {
    xmlNode *node = xmlAddChild( md, TiCC::XmlNewNode( foliaNs(), "annotations" ) );
    set<string> done;
    for ( const auto& pair : _anno_sort ){
      // Find the 'label'
      AnnotationType::AnnotationType type = pair.first;
      string sett = pair.second;
      string label = toString( type );
      if ( done.find(label+sett) != done.end() ){
	continue;
      }
      done.insert(label+sett);
      label += "-annotation";
      const auto& mm = _annotationdefaults.find(type);
      auto it = mm->second.lower_bound(sett);
      while ( it != mm->second.upper_bound(sett) ){
	string s = it->second.a;
	if ( !s.empty() ){
	  // old style
	  KWargs args;
	  if ( !s.empty() ){
	    args["annotator"] = s;
	  }
	  AnnotatorType ant = it->second.t;
	  if ( ant != UNDEFINED && ant != AUTO ){
	    args["annotatortype"] = toString(ant);
	  }
	  if ( !strip() ){
	    s = it->second.d;
	    if ( !s.empty() ){
	      args["datetime"] = s;
	    }
	  }
	  s = it->second.f;
	  if ( !s.empty() ){
	    args["format"] = s;
	  }
	  s = it->first;
	  if ( s != "undefined" ){ // the default
	    if ( s == DEFAULT_TEXT_SET
		 || s == DEFAULT_PHON_SET ){
	      if ( debug ){
		cerr << "skip adding default " << label
		     << " set: " << s << endl;
	      }
	      // skip complete!
	      ++it;
	      continue;
	    }
	    else {
	      args["set"] = s;
	    }
	  }
	  auto const& t_it = _groupannotations.find(type);
	  if ( t_it != _groupannotations.end() ){
	    auto const& s_it = t_it->second.find(s);
	    if ( s_it != t_it->second.end()
		&& s_it->second ){
	      args["groupannotations"] = "yes";
	    }
	  }

	  const auto& ti = _set_alias.find(type);
	  if ( ti != _set_alias.end() ){
	    const auto& alias = ti->second.find(s);
	    if ( alias->second != s ){
	      args["alias"] = alias->second;
	    }
	  }
	  xmlNode *n = TiCC::XmlNewNode( foliaNs(), label );
	  addAttributes( n, args );
	  xmlAddChild( node, n );
	}
	else {
	  // we have new style processors
	  KWargs args;
	  if ( !strip() ){
	    s = it->second.d;
	    if ( !s.empty() ){
	      args["datetime"] = s;
	    }
	  }
	  s = it->second.f;
	  if ( !s.empty() ){
	    args["format"] = s;
	  }
	  s = it->first;
	  if ( !s.empty() && s != "undefined" ){ // the default
	    if ( s == DEFAULT_TEXT_SET
		 || s == DEFAULT_PHON_SET ){
	      if ( debug ){
		cerr << "skip adding default " << label
		     << " set: " << s << endl;
	      }
	      // skip complete!
	      ++it;
	      continue;
	    }
	    else {
	      args["set"] = s;
	    }
	  }
	  const auto& ti = _set_alias.find(type);
	  if ( ti != _set_alias.end() ){
	    const auto& alias = ti->second.find(s);
	    if ( alias->second != s ){
	      args["alias"] = alias->second;
	    }
	  }
	  auto const& t_it = _groupannotations.find(type);
	  if ( t_it != _groupannotations.end() ){
	    auto const& s_it = t_it->second.find(s);
	    if ( s_it != t_it->second.end()
		&& s_it->second ){
	      args["groupannotations"] = "yes";
	    }
	  }
	  xmlNode *n = TiCC::XmlNewNode( foliaNs(), label );
	  addAttributes( n, args );
	  xmlAddChild( node, n );
	  args.clear();
	  for ( const auto& p : it->second.p ){
	    xmlNode *a = TiCC::XmlNewNode( foliaNs(), "annotator" );
	    args["processor"] = p;
	    addAttributes( a, args );
	    xmlAddChild( n, a );
	  }
	}
	++it;
      }
    }
  }

  void Document::append_processor( xmlNode *node, const processor *p ) const {
    xmlNode *pr = xmlAddChild( node, TiCC::XmlNewNode( foliaNs(), "processor" ) );
    KWargs atts;
    atts["xml:id"] = p->_id;
    atts["name"] = p->_name;
    if ( p->_type != AUTO ){
      atts["type"] = toString(p->_type);
    }
    if ( !strip() ){
      if ( !p->_version.empty() ){
	atts["version"] = p->_version;
      }
      if ( !p->_folia_version.empty() ){
	atts["folia_version"] = p->_folia_version;
      }
      if ( !p->_command.empty() ){
	atts["command"] = p->_command;
      }
      if ( !p->_host.empty() ){
	atts["host"] = p->_host;
      }
      if ( !p->_user.empty() ){
	atts["user"] = p->_user;
      }
      if ( !p->_begindatetime.empty() ){
	atts["begindatetime"] = p->_begindatetime;
      }
      if ( !p->_enddatetime.empty() ){
	atts["enddatetime"] = p->_enddatetime;
      }
    }
    else {
      if ( p->_name == "libfolia" ){
	atts["name"] = "stripped";
      }
      else if ( p->_name == "foliapy" ){
	atts["name"] = "stripped";
      }
      else if ( !p->_name.empty() ){
	atts["name"] = p->_name;
      }
      if ( !p->_version.empty() ){
	atts["version"] = "stripped";
      }
      if ( !p->_folia_version.empty() ){
	atts["folia_version"] = "stripped";
      }
      if ( !p->_command.empty() ){
	atts["command"] = "stripped";
      }
      if ( !p->_host.empty() ){
	atts["host"] = "stripped";
      }
      if ( !p->_user.empty() ){
	atts["user"] = "stripped";
      }
      if ( !p->_begindatetime.empty() ){
	atts["begindatetime"] = "stripped";
      }
      if ( !p->_enddatetime.empty() ){
	atts["enddatetime"] = "stripped";
      }
    }
    if ( !p->_document_version.empty() ){
      atts["document_version"] = p->_document_version;
    }
    if ( !p->_resourcelink.empty() ){
      atts["resourcelink"] = p->_resourcelink;
    }
    addAttributes( pr, atts );
    for ( const auto& it : p->_metadata ){
      xmlNode *m = xmlAddChild( pr, TiCC::XmlNewNode( foliaNs(), "meta" ) );
      KWargs args;
      args["id"] = it.first;
      addAttributes( m, args );
      xmlAddChild( m, xmlNewText( (const xmlChar*)it.second.c_str()) );
    }
    for ( const auto& s : p->_processors ){
      append_processor( pr, s );
    }
  }

  void Document::setprovenance( xmlNode *md ) const {
    if ( !_provenance ){
      return;
    }
    xmlNode *node = xmlAddChild( md, TiCC::XmlNewNode( foliaNs(), "provenance" ) );
    for ( const auto& p : _provenance->processors ){
      append_processor( node, p );
    }
  }

  void Document::addsubmetadata( xmlNode *node ) const {
    for ( const auto& it : submetadata ){
      xmlNode *sm = TiCC::XmlNewNode( foliaNs(), "submetadata" );
      KWargs atts;
      atts["xml:id"] = it.first;
      addAttributes( sm, atts );
      MetaData *md = submetadata.find(it.first)->second;
      string type = md->type();
      atts.clear();
      atts["type"] = type;
      addAttributes( sm, atts );
      xmlAddChild( node, sm );
      if ( type == "native" ){
	atts = it.second->get_avs();
	// cerr << "atts: " << atts << endl;
	for ( const auto& av : atts ){
	  xmlNode *m = TiCC::XmlNewNode( foliaNs(), "meta" );
	  KWargs args;
	  args["id"] = av.first;
	  addAttributes( m, args );
	  xmlAddChild( m, xmlNewText( (const xmlChar*)av.second.c_str()) );
	  xmlAddChild( sm, m );
	}
      }
      else if ( md->datatype() == "ExternalMetaData" ){
	KWargs args;
	args["src"] = md->src();
	addAttributes( sm, args );
      }
      else if ( md->datatype() == "ForeignMetaData" ){
	for ( const auto& foreign : md->get_foreigners() ) {
	  xmlNode *f = foreign->xml( true, false );
	  xmlAddChild( sm, f );
	}
      }
    }
  }

  void Document::setmetadata( xmlNode *node ) const{
    if ( _metadata ){
      if ( _metadata->datatype() == "ExternalMetaData" ){
	KWargs atts;
	atts["type"] = _metadata->type();
	string src = _metadata->src();
	if ( !src.empty() ){
	  atts["src"] = src;
	}
	addAttributes( node, atts );
      }
      else if ( _metadata->datatype() == "NativeMetaData" ){
	KWargs atts;
	atts["type"] = _metadata->type();
	addAttributes( node, atts );
	for ( const auto& it : _metadata->get_avs() ){
	  xmlNode *m = TiCC::XmlNewNode( foliaNs(), "meta" );
	  xmlAddChild( m, xmlNewText( (const xmlChar*)it.second.c_str()) );
	  KWargs atts;
	  atts["id"] = it.first;
	  addAttributes( m, atts );
	  xmlAddChild( node, m );
	}
      }
      else if ( _metadata->datatype() == "ForeignMetaData" ){
	KWargs atts;
	atts["type"] = _metadata->type();
	addAttributes( node, atts );
	for ( const auto& foreign : _metadata->get_foreigners() ) {
	  xmlNode *f = foreign->xml( true, false );
	  xmlAddChild( node, f );
	}
      }
    }
    else {
      KWargs atts;
      atts["type"] = "native";
      addAttributes( node, atts );
    }
    addsubmetadata( node );
  }

  void Document::setstyles( xmlDoc* doc ) const {
    for ( const auto& it : styles ){
      string content = "type=\"" + it.first + "\" href=\"" + it.second + "\"";
      xmlAddChild( (xmlNode*)doc,
		   xmlNewDocPI( doc,
				(const xmlChar*)"xml-stylesheet",
				(const xmlChar*)content.c_str() ) );
    }
  }

  xmlDoc *Document::to_xmlDoc( const string& nsLabel, bool kanon ) const {
    xmlDoc *outDoc = xmlNewDoc( (const xmlChar*)"1.0" );
    setstyles( outDoc );
    xmlNode *root = xmlNewDocNode( outDoc, 0, (const xmlChar*)"FoLiA", 0 );
    xmlDocSetRootElement( outDoc, root );
    xmlNs *xl = xmlNewNs( root, (const xmlChar *)"http://www.w3.org/1999/xlink",
			  (const xmlChar *)"xlink" );
    xmlSetNs( root, xl );
    if ( _foliaNsIn_href == 0 ){
      if ( nsLabel.empty() )
	_foliaNsOut = xmlNewNs( root, (const xmlChar *)NSFOLIA.c_str(), 0 );
      else
	_foliaNsOut = xmlNewNs( root,
				(const xmlChar *)NSFOLIA.c_str(),
				(const xmlChar*)nsLabel.c_str() );
    }
    else {
      _foliaNsOut = xmlNewNs( root,
			      _foliaNsIn_href,
			      _foliaNsIn_prefix );
    }
    xmlSetNs( root, _foliaNsOut );
    KWargs attribs;
    attribs["xml:id"] = foliadoc->id();
    if ( strip() ){
      attribs["generator"] = "";
      attribs["version"] = "";
    }
    else {
      attribs["generator"] = "libfolia-v" + library_version();
      if ( !_version_string.empty() )
	attribs["version"] = _version_string;
    }
    if ( external )
      attribs["external"] = "yes";
    addAttributes( root, attribs );

    xmlNode *md = xmlAddChild( root, TiCC::XmlNewNode( foliaNs(), "metadata" ) );
    setannotations( md );
    setprovenance( md );
    setmetadata( md );
    for ( size_t i=0; i < foliadoc->size(); ++i ){
      FoliaElement* el = foliadoc->index(i);
      xmlAddChild( root, el->xml( true, kanon ) );
    }
    return outDoc;
  }

  string Document::toXml( const string& nsLabel, bool kanon ) const {
    string result;
    if ( foliadoc ){
      xmlDoc *outDoc = to_xmlDoc( nsLabel, kanon );
      xmlChar *buf; int size;
      xmlDocDumpFormatMemoryEnc( outDoc, &buf, &size, "UTF-8", 1 );
      result = string( (const char *)buf, size );
      xmlFree( buf );
      xmlFreeDoc( outDoc );
      _foliaNsOut = 0;
    }
    else
      throw runtime_error( "can't save, no doc" );
    return result;
  }

  bool Document::toXml( const string& file_name,
			const string& nsLabel, bool kanon ) const {
    if ( foliadoc ){
      xmlDoc *outDoc = to_xmlDoc( nsLabel, kanon );
      if ( TiCC::match_back( file_name, ".gz" ) ){
	xmlSetDocCompressMode(outDoc,9);
      }
      long int res = xmlSaveFormatFileEnc( file_name.c_str(), outDoc,
					   "UTF-8", 1 );
      xmlFreeDoc( outDoc );
      _foliaNsOut = 0;
      if ( res == -1 )
	return false;
    }
    else {
      return false;
    }
    return true;
  }

  vector<vector<Word*> > Document::findwords( const Pattern& pat,
					      const string& args ) const {
    size_t leftcontext = 0;
    size_t rightcontext = 0;
    KWargs kw = getArgs( args );
    string val = kw["leftcontext"];
    if ( !val.empty() )
      leftcontext = TiCC::stringTo<size_t>(val);
    val = kw["rightcontext"];
    if ( !val.empty() )
      rightcontext = TiCC::stringTo<size_t>(val);
    vector<vector<Word*> > result;
    vector<Word*> matched;
    if ( pat.regexp )
      throw runtime_error( "regexp not supported yet in patterns" );
    vector<Word*> mywords = words();
    for ( size_t startpos =0; startpos < mywords.size(); ++startpos ){
      // loop over all words
      //    cerr << "outer loop STARTPOS = " << startpos << endl;
      size_t cursor = 0;
      int gap = 0;
      bool goon = true;
      for ( size_t i = startpos; i < mywords.size() && goon ; ++i ){
	//      cerr << "inner LOOP I = " << i << " myword=" << mywords[i] << endl;
	UnicodeString value;
	if ( pat.matchannotation == BASE )
	  value = mywords[i]->text();
	else {
	  vector<FoliaElement *> v = mywords[i]->select( pat.matchannotation );
	  if ( v.size() != 1 ){
	    continue;
	  }
	  value = TiCC::UnicodeFromUTF8(v[0]->cls());
	}
	bool done = false;
	bool flag = false;
	if ( pat.match( value, cursor, gap, done, flag ) ){
	  // cerr << "matched, " << (done?"done":"not done")
	  //      << (flag?" Flagged!":":{") << endl;
	  matched.push_back(mywords[i]);
	  if ( cursor == 0 )
	    startpos = i; // restart search here
	  if ( done ){
	    vector<Word*> keep = matched;
	    //	  cerr << "findnodes() tussenresultaat ==> " << matched << endl;
	    vector<Word*> tmp1;
	    if ( leftcontext > 0 ){
	      tmp1 = matched[0]->leftcontext(leftcontext);
	      //	    cerr << "findnodes() tmp1 ==> " << tmp1 << endl;
	      copy( matched.begin(), matched.end(), back_inserter(tmp1) );
	      //	    cerr << "findnodes() tmp1 na copy ==> " << tmp1 << endl;
	    }
	    else
	      tmp1 = matched;
	    vector<Word*> tmp2;
	    if ( rightcontext > 0 ){
	      tmp2 = matched.back()->rightcontext(rightcontext);
	      //	    cerr << "findnodes() tmp2 ==> " << tmp2 << endl;
	      copy( tmp2.begin(), tmp2.end(), back_inserter(tmp1) );
	      //	    cerr << "findnodes() tmp2 na copy ==> " << tmp2 << endl;
	    }
	    result.push_back(tmp1);
	    //	  cerr << "findnodes() tussenresultaat 2 ==> " << tmp1 << endl;
	    if ( flag ){
	      matched = keep;
	    }
	    else {
	      cursor = 0;
	      matched.clear();
	      goon = false;
	    }
	  }
	}
	else {
	  cursor = 0;
	  matched.clear();
	  goon = false;
	}
      }
    }
    //  cerr << "findnodes() result ==> " << result << endl;
    return result;
  }

  vector<vector<Word*> > Document::findwords( list<Pattern>& pats,
					      const string& args ) const {
    size_t prevsize = 0;
    bool start = true;
    bool unsetwildcards = false;
    set<int> variablewildcards;
    int index = 0;
    for ( const auto& it : pats ){
      //    cerr << "bekijk patroon : " << *it << endl;
      if ( start ){
	prevsize = it.size();
	start = false;
      }
      else if ( it.size() != prevsize ){
	throw runtime_error( "findnodes(): If multiple patterns are provided, they must all have the same length!" );
      }
      if ( it.variablesize() ){
	if ( index > 0 && variablewildcards.empty() )
	  unsetwildcards = true;
	else {
	  if ( !variablewildcards.empty() &&
	       variablewildcards != it.variablewildcards() ){
	    throw runtime_error("If multiple patterns are provided with variable wildcards, then these wildcards must all be in the same positions!");
	  }
	  variablewildcards = it.variablewildcards();
	}
      }
      else if ( !variablewildcards.empty() )
	unsetwildcards = true;
      ++index;
    }
    if ( unsetwildcards ){
      for ( auto& it : pats ){
	it.unsetwild();
      }
    }
    vector<vector<Word*> > result;
    for ( const auto& it : pats ){
      vector<vector<Word*> > res = findwords( it, args );
      if ( result.empty() )
	result = res;
      else if ( res != result ){
	result.clear();
	break;
      }
    }
    return result;
  }

  Pattern::Pattern( const vector<string>& pat_vec,
		    const ElementType at,
		    const string& args ): matchannotation(at) {
    regexp = false;
    case_sensitive = false;
    KWargs kw = getArgs( args );
    matchannotationset = kw["matchannotationset"];
    if (kw["regexp"] != "" )
      regexp = TiCC::stringTo<bool>( kw["regexp"] );
    if (kw["maxgapsize"] != "" )
      maxgapsize = TiCC::stringTo<int>( kw["maxgapsize"] );
    else
      maxgapsize = 10;
    if ( kw["casesensitive"] != "" )
      case_sensitive = TiCC::stringTo<bool>( kw["casesensitive"] );
    for ( const auto& pat : pat_vec ){
      if ( pat.find( "regexp('" ) == 0 &&
	   pat.rfind( "')" ) == pat.length()-2 ){
	string tmp = pat.substr( 8, pat.length() - 10 );
	UnicodeString us = TiCC::UnicodeFromUTF8( tmp );
	UErrorCode u_stat = U_ZERO_ERROR;
	RegexMatcher *matcher = new RegexMatcher(us, 0, u_stat);
	if ( U_FAILURE(u_stat) ){
	  throw runtime_error( "failed to create a regexp matcher with '" + tmp + "'" );
	}
	matchers.push_back( matcher );
	sequence.push_back( "" );
      }
      else {
	sequence.push_back( TiCC::UnicodeFromUTF8(pat) );
	matchers.push_back( 0 );
	if ( !case_sensitive ){
	  sequence.back().toLower();
	}
      }
    }
  }

  Pattern::Pattern( const std::vector<std::string>& pat_vec,
		    const std::string& args ) : matchannotation(BASE) {
    regexp = false;
    case_sensitive = false;
    KWargs kw = getArgs( args );
    matchannotationset = kw["matchannotationset"];
    if (kw["regexp"] != "" )
      regexp = TiCC::stringTo<bool>( kw["regexp"] );
    if (kw["maxgapsize"] != "" )
      maxgapsize = TiCC::stringTo<int>( kw["maxgapsize"] );
    else
      maxgapsize = 10;
    if ( kw["casesensitive"] != "" )
      case_sensitive = TiCC::stringTo<bool>( kw["casesensitive"] );
    for ( const auto& pat : pat_vec ){
      if ( pat.find( "regexp('" ) == 0 &&
	   pat.rfind( "')" ) == pat.length()-2 ){
	string tmp = pat.substr( 8, pat.length() - 10 );
	UnicodeString us = TiCC::UnicodeFromUTF8( tmp );
	UErrorCode u_stat = U_ZERO_ERROR;
	RegexMatcher *matcher = new RegexMatcher(us, 0, u_stat);
	if ( U_FAILURE(u_stat) ){
	  throw runtime_error( "failed to create a regexp matcher with '" + tmp + "'" );
	}
	matchers.push_back( matcher );
	sequence.push_back( "" );
      }
      else {
	sequence.push_back( TiCC::UnicodeFromUTF8(pat) );
	matchers.push_back( 0 );
	if ( !case_sensitive ){
	  sequence.back().toLower();
	}
      }
    }
  }

  Pattern::~Pattern(){
    for ( const auto& m : matchers ){
      delete m;
    }
  }

  inline ostream& operator<<( ostream& os, const Pattern& p ){
    using TiCC::operator <<;
    os << "pattern: " << p.sequence;
    return os;
  }

  bool Pattern::match( const UnicodeString& us, size_t& pos, int& gap,
		       bool& done, bool& flag ) const {
    UnicodeString s = us;
    //  cerr << "gap = " << gap << "cursor=" << pos << " vergelijk '" <<  sequence[pos] << "' met '" << us << "'" << endl;
    if ( matchers[pos] ){
      matchers[pos]->reset( s );
      UErrorCode u_stat = U_ZERO_ERROR;
      if ( matchers[pos]->matches( u_stat ) ){
	done = ( ++pos >= sequence.size() );
	return true;
      }
      else {
	++pos;
	return false;
      }
    }
    else {
      if ( !case_sensitive )
	s.toLower();
      if ( sequence[pos] == s || sequence[pos] == "*:1" ){
	done = ( ++pos >= sequence.size() );
	return true;
      }
      else if ( sequence[pos] == "*" ){
	if ( (pos + 1 ) >= sequence.size() ){
	  done = true;
	}
	else if ( sequence[pos+1] == s ){
	  //	cerr << "    but next matched!" << endl;
	  flag = ( ++gap < maxgapsize );
	  if ( !flag ){
	    pos = pos + gap;
	    done = ( ++pos >= sequence.size() );
	  }
	  else {
	    done = true;
	  }
	}
	else if ( ++gap == maxgapsize )
	  ++pos;
	else
	  flag = true;
	return true;
      }
      else {
	++pos;
	return false;
      }
    }
  }

  bool Pattern::variablesize() const {
    for ( const auto& s : sequence ){
      if ( s == "*" ){
	return true;
      }
    }
    return false;
  }

  void Pattern::unsetwild() {
    for ( auto& s : sequence ){
      if ( s == "*" )
	s = "*:1";
    }
  }

  set<int> Pattern::variablewildcards() const {
    set<int> result;
    for ( size_t i=0; i < sequence.size(); ++i ){
      if ( sequence[i] == "*" )
	result.insert( i );
    }
    return result;
  }

} // namespace folia
