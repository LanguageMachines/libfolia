/*
  Copyright (c) 2006 - 2017
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
#include "ticcutils/zipper.h"
#include "libfolia/folia.h"
#include "libfolia/folia_properties.h"
#include "libxml/xmlstring.h"

using namespace std;
using namespace TiCC;

namespace folia {

  void initMT(){
    // a NO_OP now
  }

  bool checkNS( xmlNode *n, const string& ns ){
    string tns = getNS(n);
    if ( tns == ns )
      return true;
    else
      throw runtime_error( "namespace conflict for tag:" + Name(n)
			   + ", wanted:" + ns
			   + " got:" + tns );
    return false;
  }

  map<string,string> getNS_definitions( const xmlNode *node ){
    map<string,string> result;
    xmlNs *p = node->nsDef;
    while ( p ){
      string pre;
      string val;
      if ( p->prefix ){
	pre = (char *)p->prefix;
      }
      val = (char *)p->href;
      result[pre] = val;
      p = p->next;
    }
    return result;
  }

  Document::Document(){
    init();
  }

  Document::Document( const string& args ) {
    init();
    KWargs kwargs = getArgs( args );
    setDocumentProps( kwargs );
    if ( !foliadoc ){
      foliadoc = new FoLiA( kwargs, this );
    }
  }

  string versionstring(){
    stringstream ss;
    ss << MAJOR_VERSION << "." << MINOR_VERSION << "." << SUB_VERSION;
    return ss.str();
  }

  string Document::update_version(){
    // override the document version with the current library version
    // return the old value
    string old = version;
    version = versionstring();
    return old;
  }

  void Document::init(){
    _metadata = 0;
    _xmldoc = 0;
    foliadoc = 0;
    _foliaNsIn_href = 0;
    _foliaNsIn_prefix = 0;
    _foliaNsOut = 0;
    debug = 0;
    mode = CHECKTEXT;
    version = versionstring();
    external = false;
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

  void Document::setmode( const string& ms ){
    vector<string> modev;
    TiCC::split_at( ms, modev, "," );
    for ( const auto& mod : modev ){
      if ( mod == "permissive" ){
	mode = Mode( (int)mode | PERMISSIVE );
      }
      else if ( mod == "strip" ){
	mode = Mode( (int)mode | STRIP );
      }
      else if ( mod == "nochecktext" ){
	mode = Mode( int(mode) & ~CHECKTEXT );
      }
      else if ( mod == "fixtext" ){
	mode = Mode( int(mode) | FIXTEXT );
      }
      else {
	throw runtime_error( "FoLiA::Document: unsupported mode value: "+ mod );
      }
    }
  }

  string Document::getmode() const{
    string result = "mode=";
    if ( mode == PERMISSIVE ){
      result += "permissive,";
    }
    if ( mode == STRIP ){
      result += "strip,";
    }
    if ( mode == CHECKTEXT ){
      result += "checktext,";
    }
    if ( mode == FIXTEXT ){
      result += "fixtext,";
    }
    return result;
  }

  void Document::setDocumentProps( KWargs& kwargs ){
    bool happy = false;
    auto it = kwargs.find( "debug" );
    if ( it != kwargs.end() ){
      debug = stringTo<int>( it->second );
    }
    it = kwargs.find( "mode" );
    if ( it != kwargs.end() ){
      setmode( it->second );
    }
    it = kwargs.find( "external" );
    if ( it != kwargs.end() ){
      external = stringTo<bool>( it->second );
      kwargs.erase( it );
    }
    else
      external = false;
    it = kwargs.find( "version" );
    if ( it != kwargs.end() ){
      version = it->second;
      kwargs.erase( it );
    }
    it = kwargs.find( "_id" );
    if ( it == kwargs.end() ){
      it = kwargs.find( "id" );
    }
    if ( it != kwargs.end() ){
      if ( isNCName( it->second ) ){
	_id = it->second;
      }
      else {
	throw XmlError( "'"
			+ it->second
			+ "' is not a valid NCName." );
      }
      happy = true;
    }
    else {
      it = kwargs.find( "file" );
      if ( it != kwargs.end() ){
	filename = it->second;
	happy = readFromFile(filename);
      }
      else {
	it = kwargs.find( "string" );
	if ( it != kwargs.end() ){
	  string s = it->second;
	  happy = readFromString( s );
	}
      }
    }
    if ( !happy )
      throw runtime_error( "No ID, valid filename or string specified" );
    kwargs.erase( "generator" ); // also delete unused att-val(s)
    const char *env = getenv( "FOLIA_TEXT_CHECK" );
    if ( env ){
      string e = env;
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
  }

  void Document::addDocIndex( FoliaElement* el, const string& s ){
    if ( s.empty() ) {
      return;
    }
    // cerr << _id << "-add docindex " << el << " (" << s << ")" << endl;
    // using TiCC::operator <<;
    // cerr << "VOOR: " << sindex << endl;
    if ( sindex.find( s ) == sindex.end() ){
      sindex[s] = el;
      iindex.push_back( el );
    }
    else
      throw DuplicateIDError( s );
    //    cerr << "NA  : " << sindex << endl;
  }

  void Document::delDocIndex( const FoliaElement* el, const string& s ){
    if ( sindex.empty() ){
      // only when ~~Document is in progress
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

  static int error_sink(void *mydata, xmlError *error ){
    int *cnt = (int*)mydata;
    if ( *cnt == 0 ){
      cerr << "\nXML-error: " << error->message << endl;
    }
    (*cnt)++;
    return 1;
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
    if ( match_back( s, ".bz2" ) ){
      string buffer = bz2ReadFile( s );
      return readFromString( buffer );
    }
    else if ( match_back( s, ".gz" ) ) {
      string buffer = gzReadFile( s );
      return readFromString( buffer );
    }
    int cnt = 0;
    xmlSetStructuredErrorFunc( &cnt, (xmlStructuredErrorFunc)error_sink );
    _xmldoc = xmlReadFile( s.c_str(), 0, XML_PARSE_NOBLANKS|XML_PARSE_HUGE );
    if ( _xmldoc ){
      if ( cnt > 0 ){
	throw XmlError( "document is invalid" );
      }
      if ( debug )
	cout << "read a doc from " << s << endl;
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
      cout << "Failed to read a doc from " << s << endl;
    throw XmlError( "No XML document read" );
  }

  bool Document::readFromString( const string& s ){
    if ( foliadoc ){
      throw runtime_error( "Document is already initialized" );
      return false;
    }
    int cnt = 0;
    xmlSetStructuredErrorFunc( &cnt, (xmlStructuredErrorFunc)error_sink );
    _xmldoc = xmlReadMemory( s.c_str(), s.length(), 0, 0,
			    XML_PARSE_NOBLANKS|XML_PARSE_HUGE );
    if ( _xmldoc ){
      if ( cnt > 0 ){
	throw XmlError( "document is invalid" );
      }
      if ( debug )
	cout << "read a doc from string" << endl;
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
      string s = d->toXml( "", d->strip() );
      os << s << endl;
    }
    else {
      os << "MISSING DOCUMENT" << endl;
    }
    return os;
  }

  bool Document::save( ostream& os, const string& nsLabel, bool kanon ) const {
    string s = toXml( nsLabel, ( kanon || strip() ) );
    os << s << endl;
    return os.good();
  }

  bool Document::save( const string& fn, const string& nsLabel, bool kanon ) const {
    try {
      if ( match_back( fn, ".bz2" ) ){
	string tmpname = fn.substr( 0, fn.length() - 3 ) + "tmp";
	if ( toXml( tmpname, nsLabel, ( kanon || strip() ) ) ){
	  bool stat = bz2Compress( tmpname, fn );
	  remove( tmpname.c_str() );
	  return stat;
	}
	else {
	  return false;
	}
      }
      else  if ( match_back( fn, ".gz" ) ){
	string tmpname = fn.substr( 0, fn.length() - 2 ) + "tmp";
	if ( toXml( tmpname, nsLabel,  ( kanon || strip() ) ) ){
	  bool stat = gzCompress( tmpname, fn );
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
    xmlNode *n = xPath( node, "//imdi:Session/imdi:Title" );
    if ( n ){
      _metadata->add_av( "title", XmlContent( n ) );
    }
    n = xPath( node, "//imdi:Session/imdi:Date" );
    if ( n ){
      _metadata->add_av( "date", XmlContent( n ) );
    }
    n = xPath( node, "//imdi:Source/imdi:Access/imdi:Publisher" );
    if ( n ){
      _metadata->add_av( "publisher", XmlContent( n ) );
    }
    n = xPath( node, "//imdi:Source/imdi:Access/imdi:Availability" );
    if ( n ){
      _metadata->add_av( "licence", XmlContent( n ) );
    }
    n = xPath( node, "//imdi:Languages/imdi:Language/imdi:ID" );
    if ( n ){
      _metadata->add_av( "language", XmlContent( n ) );
    }
  }

  void Document::parsemeta( xmlNode *node ){
    if ( node ){
      KWargs att = getAttributes( node );
      string type = att["id"];
      string val = XmlContent( node );
      string get = _metadata->get_val( type );
      if ( !get.empty() ){
	throw runtime_error( "meta tag with id=" + type
			     + " is defined more then once " );
      }
      _metadata->add_av( type, val );
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
    if ( Name( node ) != "foreign-data" ){
      // we need an extra layer then
      xmlNode *n = XmlNewNode( "foreign-data" );
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

  void Document::parseannotations( xmlNode *node ){
    xmlNode *n = node->children;
    anno_sort.clear();
    while ( n ){
      string tag = Name( n );
      if ( tag.length() > 11 && tag.substr( tag.length() - 11 ) == "-annotation" ){
	string prefix = tag.substr( 0,  tag.length() - 11 );
	AnnotationType::AnnotationType type = stringToAT( prefix );
	KWargs att = getAttributes( n );
	string s;
	string a;
	string t;
	string d;
	string alias;
	auto it = att.find("set" );
	if ( it != att.end() ){
	  s = it->second;
	}
	else {
	  s = "undefined"; // default value
	}
	it = att.find( "annotator" );
	if ( it != att.end() )
	  a = it->second;
	it = att.find( "annotatortype" );
	if ( it != att.end() ){
	  t = it->second;
	}
	it = att.find( "datetime" );
	if ( it != att.end() ){
	  d = parseDate( it->second );
	}
	it = att.find( "alias" );
	if ( it != att.end() ){
	  alias = it->second;
	}
	declare( type, s, a, t, d, alias );
      }
      n = n->next;
    }
  }

  void Document::parsesubmeta( xmlNode *node ){
    using TiCC::operator<<;
    if ( node ){
      KWargs att = getAttributes( node );
      string id = att["_id"];
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
	  if ( Name(p) == "meta" &&
	       checkNS( p, NSFOLIA ) ){
	    if ( type == "native" ){
	      string txt = XmlContent( p );
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
	  else if ( Name(p) == "foreign-data" &&
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

  int check_version( const string& vers, bool& no_textcheck ){
    no_textcheck = false;
    int majVersion = 0;
    int minVersion = 0;
    int subVersion = 0;
    vector<string> vec;
    split_at( vers, vec, "." );
    for ( size_t i=0; i < vec.size(); ++i ){
      int val = stringTo<int>( vec[i] );
      if ( i == 0 )
	majVersion = val;
      else if ( i == 1 )
	minVersion = val;
      else
	subVersion += val;
    }
    if ( ( majVersion < 1 )
	 || (majVersion == 1 && minVersion < 5 ) ){
      // don't check text consistency for older documents
      no_textcheck = true;
    }
    if ( majVersion < MAJOR_VERSION ){
      return -1;
    }
    else if ( majVersion > MAJOR_VERSION ){
      return 1;
    }
    else if ( minVersion < MINOR_VERSION ){
      return -1;
    }
    else if ( minVersion > MINOR_VERSION ){
      return 1;
    }
    else if ( subVersion < SUB_VERSION ){
      return -1;
    }
    else if ( subVersion > SUB_VERSION ){
      return 1;
    }
    return 0;
  }

  FoliaElement *Document::resolveExternals( FoliaElement* result ){
    if ( !externals.empty() ){
      for ( const auto& ext : externals ){
	ext->resolve_external();
      }
    }
    return result;
  }

  FoliaElement* Document::parseFoliaDoc( xmlNode *root ){
    KWargs att = getAttributes( root );
    using TiCC::operator<<;
    if ( att["_id"] == "" ){
      throw XmlError("FoLiA Document has no ID!");
      return 0;
    }
    string vers = att["version"];
    bool no_textcheck = false;
    if ( check_version( vers, no_textcheck ) > 0 ){
      cerr << "WARNING!!! FoLiA Document is a newer version than this library ("
	   << vers << " vs " << version
	   << ")\n\t Any possible subsequent failures in parsing or processing may probably be attributed to this." << endl
	   << "\t Please upgrade libfolia!" << endl;
    }
    if ( no_textcheck ){
      setmode( "nochecktext" );
    }
    setDocumentProps( att );
    FoliaElement *result = FoliaImpl::createElement( Name(root), this );
    if ( debug > 2 ){
      cerr << "created " << root << endl;
    }
    result->setAttributes( att );
    xmlNode *p = root->children;
    while ( p ){
      if ( p->type == XML_ELEMENT_NODE ){
	if ( Name(p) == "metadata" &&
	     checkNS( p, NSFOLIA ) ){
	  if ( debug > 1 ){
	    cerr << "Found metadata" << endl;
	  }
	  KWargs atts = getAttributes( p );
	  string type = lowercase(atts["type"]);
	  if ( type.empty() ){
	    type = "native";
	  }
	  string src = atts["src"];
	  if ( !src.empty() ){
	    _metadata = new ExternalMetaData( type, src );
	  }
	  else if ( type == "native" ){
	    _metadata = new NativeMetaData( type );
	  }
	  else {
	    _metadata = 0;
	  }
	  xmlNode *m = p->children;
	  while ( m ){
	    if ( Name(m)  == "METATRANSCRIPT" ){
	      if ( !checkNS( m, NSIMDI ) || type != "imdi" )
		throw runtime_error( "imdi != imdi " );
	      if ( debug > 1 ){
		cerr << "found IMDI" << endl;
	      }
	      if ( !_metadata ){
		_metadata = new ForeignMetaData( "imdi" );
	      }
	      _metadata->add_foreign( xmlCopyNode(m,1) );
	    }
	    else if ( Name( m ) == "annotations" &&
		      checkNS( m, NSFOLIA ) ){
	      if ( debug > 1 ){
		cerr << "found annotations" << endl;
	      }
	      parseannotations( m );
	    }
	    else if ( Name( m ) == "meta" &&
		      checkNS( m, NSFOLIA ) ){
	      if ( debug > 1 ){
		cerr << "found meta node" << endl;
	      }
	      parsemeta( m );
	    }
	    else if ( Name(m)  == "foreign-data" &&
		      checkNS( m, NSFOLIA ) ){
	      FoliaElement *t = FoliaImpl::createElement( "foreign-data", this );
	      if ( t ){
		t = t->parseXml( m );
		if ( t ){
		  if ( _metadata && _metadata->datatype() == "NativeMetaData" ){
		    cerr << "WARNING: foreign-data found in metadata of type 'native'"  << endl;
		    cerr << "changing type to 'foreign'" << endl;
		    type = "foreign";
		    delete _metadata;
		    _metadata = new ForeignMetaData( type );
		  }
		  if ( !_metadata ){
		    _metadata = new ForeignMetaData( type );
		  }
		  _metadata->add_foreign( m );
		}
	      }
	    }
	    else if ( Name(m)  == "submetadata" &&
		      checkNS( m, NSFOLIA ) ){
	      parsesubmeta( m );
	    }
	    m = m->next;
	  }
	  if ( _metadata == 0 && type == "imdi" ){
	    // imdi missing all further info
	    _metadata = new NativeMetaData( type );
	  }
	}
	else {
	  if ( p && getNS(p) == NSFOLIA ){
	    string tag = Name( p );
	    FoliaElement *t = FoliaImpl::createElement( tag, this );
	    if ( t ){
	      if ( debug > 2 ){
		cerr << "created " << t << endl;
	      }
	      t = t->parseXml( p );
	      if ( t ){
		if ( debug > 2 ){
		  cerr << "extend " << result << " met " << tag << endl;
		}
		result->append( t );
	      }
	    }
	  }
	}
      }
      p = p->next;
    }
    result = resolveExternals( result );
    return result;
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
      if ( pnt->type == XML_PI_NODE && Name(pnt) == "xml-stylesheet" ){
	string content = (const char*)pnt->content;
	string type;
	string href;
	vector<string> v;
	split( content, v );
	if ( v.size() == 2 ){
	  vector<string> w;
	  split_at( v[0], w, "=" );
	  if ( w.size() == 2 && w[0] == "type" ){
	    type = w[1].substr(1,w[1].length()-2);
	  }
	  w.clear();
	  split_at( v[1], w, "=" );
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
	  string msg = "Text for " + txt->parent()->xmltag() + ", ID="
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
      using TiCC::operator<<;
      string dum;
      cerr << "root = " << Name( root ) << endl;
      cerr << "in namespace " << getNS( root, dum ) << endl;
      cerr << "namespace list" << getNS_definitions( root ) << endl;
    }
    FoliaElement *result = 0;
    if ( root  ){
      if ( Name( root ) == "FoLiA" ){
	string ns = getNS( root );
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
	  result = parseFoliaDoc( root );
	  if ( result ){
	    if ( isNCName( result->id() ) )
	      _id = result->id();
	    else {
	      // can this ever happen? parseFoliaDoc should fail
	      throw XmlError( "'"
			      + result->id()
			      + "' is not a valid NCName." );
	    }
	  }
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
      else if ( Name( root ) == "DCOI" &&
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
			  const string& setname, const string& args ){
    string st = setname;
    if ( st.empty() )
      st = "undefined";
    KWargs kw = getArgs( args );
    string a = kw["annotator"];
    string t = kw["annotatortype"];
    string d = kw["datetime"];
    string alias = kw["alias"];
    kw.erase("annotator");
    kw.erase("annotatortype");
    kw.erase("datetime");
    kw.erase("alias");
    if ( kw.size() != 0 ){
      throw XmlError( "declaration: expected 'annotator', 'annotatortype', 'alias' or 'datetime', got '" + kw.begin()->first + "'" );
    }
    declare( type, st, a, t, d, alias );
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

  string Document::unalias( AnnotationType::AnnotationType type,
			    const string& alias ) const {
    const auto& ti = alias_set.find(type);
    if ( ti != alias_set.end() ){
      const auto& sti = ti->second.find( alias );
      if ( sti != ti->second.end() ){
	return sti->second;
      }
    }
    return "";
  }

  string Document::alias( AnnotationType::AnnotationType type,
			  const string& st ) const {
    const auto& ti = set_alias.find(type);
    if ( ti != set_alias.end() ){
      const auto& ali = ti->second.find( st );
      if ( ali != ti->second.end() ){
	return ali->second;
      }
    }
    return "";
  }

  void Document::declare( AnnotationType::AnnotationType type,
			  const string& setname,
			  const string& annotator,
			  const string& annotator_type,
			  const string& date_time,
			  const string& _alias ){
    if ( !_alias.empty() ){
      string set_ali = alias(type,setname);
      if ( !set_ali.empty() ){
	if ( set_ali != _alias ){
	  throw XmlError( "setname: " + setname + " already has an alias: "
			  + set_ali );
	}
      }
      string ali_ali = alias(type,_alias);
      string ali_set = unalias(type,_alias);
      if ( !ali_ali.empty() ){
	if( ali_ali != _alias ){
	  throw XmlError( "alias: " + _alias +
			  " is also in use as a setname for set:'"
			  + ali_set + "'" );
	}
      }
      if ( !ali_set.empty()
	   && ali_set != setname ){
	throw XmlError( "alias: " + _alias + " already used for setname: "
			+ ali_set );
      }
    }
    if ( !isDeclared( type, setname, annotator, annotator_type ) ){
      if ( !unalias(type,setname).empty()
	   && unalias(type,setname) != setname ){
	throw XmlError( "setname: " + setname + " is also in use as an alias" );
      }
      string d = date_time;
      if ( d == "now()" ){
	d = getNow();
      }
      annotationdefaults[type].insert( make_pair( setname,
						  at_t(annotator,annotator_type,d) ) );
      anno_sort.push_back(make_pair(type,setname));
      if ( !_alias.empty() ){
	alias_set[type][_alias] = setname;
	set_alias[type][setname] = _alias;
      }
      else {
	alias_set[type][setname] = setname;
	set_alias[type][setname] = setname;
      }
    }
  }

  void Document::un_declare( AnnotationType::AnnotationType type,
			     const string& set_name ){
    string setname = unalias(type,set_name);
    if (  annotationrefs[type][setname] != 0 ){
      throw XmlError( "unable to undeclare " + toString(type) + "-type("
		      + setname + ") (references remain)" );
    }
    auto const adt = annotationdefaults.find(type);
    if ( adt != annotationdefaults.end() ){
      auto it = adt->second.begin();
      while ( it != adt->second.end() ){
	if ( setname.empty() || it->first == setname ){
	  it = adt->second.erase(it);
	}
	else {
	  ++it;
	}
      }
      auto it2 = anno_sort.begin();
      while ( it2 != anno_sort.end() ){
	if ( it2->first == type && it2->second == setname ){
	  it2 = anno_sort.erase( it2 );
	}
	else {
	  ++it2;
	}
      }
      auto it3 = alias_set[type].begin();
      while ( it3 != alias_set[type].end() ){
	if ( it3->first == setname || it3->second == setname ){
	  it3 = alias_set[type].erase( it3 );
	}
	else {
	  ++it3;
	}
      }
      auto it4 = set_alias[type].begin();
      while ( it4 != set_alias[type].end() ){
	if ( it4->first == setname || it4->second == setname ){
	  it4 = set_alias[type].erase( it4 );
	}
	else {
	  ++it4;
	}
      }
    }
  }

  Text* Document::addText( const KWargs& kwargs ){
    Text *res = new Text( kwargs, this );
    foliadoc->append( res );
    return res;
  }

  Text* Document::addText( Text *t ){
    foliadoc->append( t );
    return t;
  }

  bool Document::isDeclared( AnnotationType::AnnotationType type,
			     const string& set_name,
			     const string& annotator,
			     const string& annotator_type){
    //
    // We DO NOT check the date. if all parameters match, it is OK
    //
    if ( set_name.empty() ){
	throw runtime_error("isDeclared called with empty set.");
    }
    if ( type == AnnotationType::NO_ANN ){
      return true;
    }
    string setname = unalias(type,set_name);

    const auto& it1 = annotationdefaults.find(type);
    if ( it1 != annotationdefaults.end() ){
      auto mit2 = it1->second.lower_bound(setname);
      while ( mit2 != it1->second.upper_bound(setname) ){
	if ( mit2->second.a == annotator && mit2->second.t == annotator_type )
	  return true;
	++mit2;
      }
    }
    return false;
  }

  void Document::incrRef( AnnotationType::AnnotationType type,
			  const string& s ){
    if ( type != AnnotationType::NO_ANN ){
      string st = s;
      if ( st.empty() ){
	st = defaultset(type);
      }
      ++annotationrefs[type][st];
      //      cerr << "increment " << toString(type) << "(" << st << ")" << endl;
    }
  }

  void Document::decrRef( AnnotationType::AnnotationType type,
			  const string& s ){
    if ( type != AnnotationType::NO_ANN ){
      --annotationrefs[type][s];
      //      cerr << "decrement " << toString(type) << "(" << s << ")" << endl;
    }
  }

  bool Document::isDeclared( AnnotationType::AnnotationType type,
			     const string& setname ){
    if ( type == AnnotationType::NO_ANN ){
      return true;
    }
    const auto& mit1 = annotationdefaults.find(type);
    if ( mit1 != annotationdefaults.end() ){
      if ( setname.empty() )
	return true;
      const auto& mit2 = mit1->second.find(setname);
      return mit2 != mit1->second.end();
    }
    return false;
  }

  string Document::defaultset( AnnotationType::AnnotationType type ) const {
    if ( type == AnnotationType::NO_ANN )
      return "";
    // search a set. it must be unique. Otherwise return ""
    //    cerr << "zoek '" << type << "' default set " <<  annotationdefaults << endl;
    string result;
    const auto& mit1 = annotationdefaults.find(type);
    if ( mit1 != annotationdefaults.end() ){
      //      cerr << "vind tussen " <<  mit1->second << endl;
      if ( mit1->second.size() == 1 )
	result = mit1->second.begin()->first;
    }
    //    cerr << "defaultset ==> " << result << endl;
    return result;
  }

  string Document::defaultannotator( AnnotationType::AnnotationType type,
				     const string& st ) const {
    if ( type == AnnotationType::NO_ANN ){
      return "";
    }
    // if ( !st.empty() ){
    //   cerr << "zoek '" << st << "' default annotator " <<  annotationdefaults << endl;
    // }
    const auto& mit1 = annotationdefaults.find(type);
    string result;
    if ( mit1 != annotationdefaults.end() ){
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

  string Document::defaultannotatortype( AnnotationType::AnnotationType type,
					 const string& st ) const {
    if ( type == AnnotationType::NO_ANN ){
      return "";
    }
    const auto& mit1 = annotationdefaults.find(type);
    string result;
    if ( mit1 != annotationdefaults.end() ){
      if ( st.empty() ){
	if ( mit1->second.size() == 1 )
	  result = mit1->second.begin()->second.t;
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
    const auto& mit1 = annotationdefaults.find(type);
    string result;
    if ( mit1 != annotationdefaults.end() ){
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

  void Document::setannotations( xmlNode *node ) const {
    for ( const auto& pair : anno_sort ){
      // Find the 'label'
      AnnotationType::AnnotationType type = pair.first;
      string label = toString( type );
      label += "-annotation";
      string sett = pair.second;
      const auto& mm = annotationdefaults.find(type);
      auto it = mm->second.lower_bound(sett);
      while ( it != mm->second.upper_bound(sett) ){
	KWargs args;
	string s = it->second.a;
	if ( !s.empty() )
	  args["annotator"] = s;
	s = it->second.t;
	if ( !s.empty() )
	  args["annotatortype"] = s;
	if ( !strip() ){
	  s = it->second.d;
	  if ( !s.empty() )
	    args["datetime"] = s;
	}
	s = it->first;
	if ( s != "undefined" ) // the default
	  args["set"] = s;
	const auto& ti = set_alias.find(type);
	if ( ti != set_alias.end() ){
	  const auto& alias = ti->second.find(s);
	  if ( alias->second != s ){
	    args["alias"] = alias->second;
	  }
	}
	xmlNode *n = XmlNewNode( foliaNs(), label );
	addAttributes( n, args );
	xmlAddChild( node, n );
	++it;
      }
    }
  }

  void Document::addsubmetadata( xmlNode *node ) const {
    for ( const auto& it : submetadata ){
      xmlNode *sm = XmlNewNode( foliaNs(), "submetadata" );
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
	// using TiCC::operator<<;
	// cerr << "atts: " << atts << endl;
	for ( const auto& av : atts ){
	  xmlNode *m = XmlNewNode( foliaNs(), "meta" );
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
	  xmlNode *m = XmlNewNode( foliaNs(), "meta" );
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
    attribs["_id"] = foliadoc->id(); // sort "id" in front!
    if ( strip() ){
      attribs["generator"] = "";
      attribs["version"] = "";
    }
    else {
      attribs["generator"] = string("libfolia-v") + VERSION;
      if ( !version.empty() )
	attribs["version"] = version;
    }
    if ( external )
      attribs["external"] = "yes";
    addAttributes( root, attribs );

    xmlNode *md = xmlAddChild( root, XmlNewNode( foliaNs(), "metadata" ) );
    xmlNode *an = xmlAddChild( md, XmlNewNode( foliaNs(), "annotations" ) );
    setannotations( an );
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

  bool Document::toXml( const string& filename,
			const string& nsLabel, bool kanon ) const {
    if ( foliadoc ){
      xmlDoc *outDoc = to_xmlDoc( nsLabel, kanon );
      long int res = xmlSaveFormatFileEnc( filename.c_str(), outDoc, "UTF-8", 1 );
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
      leftcontext = stringTo<size_t>(val);
    val = kw["rightcontext"];
    if ( !val.empty() )
      rightcontext = stringTo<size_t>(val);
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
	  value = UTF8ToUnicode(v[0]->cls());
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
      regexp = stringTo<bool>( kw["regexp"] );
    if (kw["maxgapsize"] != "" )
      maxgapsize = stringTo<int>( kw["maxgapsize"] );
    else
      maxgapsize = 10;
    if ( kw["casesensitive"] != "" )
      case_sensitive = stringTo<bool>( kw["casesensitive"] );
    for ( const auto& pat : pat_vec ){
      if ( pat.find( "regexp('" ) == 0 &&
	   pat.rfind( "')" ) == pat.length()-2 ){
	string tmp = pat.substr( 8, pat.length() - 10 );
	UnicodeString us = UTF8ToUnicode( tmp );
	UErrorCode u_stat = U_ZERO_ERROR;
	RegexMatcher *matcher = new RegexMatcher(us, 0, u_stat);
	if ( U_FAILURE(u_stat) ){
	  throw runtime_error( "failed to create a regexp matcher with '" + tmp + "'" );
	}
	matchers.push_back( matcher );
	sequence.push_back( "" );
      }
      else {
	sequence.push_back( UTF8ToUnicode(pat) );
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
      regexp = stringTo<bool>( kw["regexp"] );
    if (kw["maxgapsize"] != "" )
      maxgapsize = stringTo<int>( kw["maxgapsize"] );
    else
      maxgapsize = 10;
    if ( kw["casesensitive"] != "" )
      case_sensitive = stringTo<bool>( kw["casesensitive"] );
    for ( const auto& pat : pat_vec ){
      if ( pat.find( "regexp('" ) == 0 &&
	   pat.rfind( "')" ) == pat.length()-2 ){
	string tmp = pat.substr( 8, pat.length() - 10 );
	UnicodeString us = UTF8ToUnicode( tmp );
	UErrorCode u_stat = U_ZERO_ERROR;
	RegexMatcher *matcher = new RegexMatcher(us, 0, u_stat);
	if ( U_FAILURE(u_stat) ){
	  throw runtime_error( "failed to create a regexp matcher with '" + tmp + "'" );
	}
	matchers.push_back( matcher );
	sequence.push_back( "" );
      }
      else {
	sequence.push_back( UTF8ToUnicode(pat) );
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
