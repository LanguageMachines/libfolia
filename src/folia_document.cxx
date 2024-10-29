/*
  Copyright (c) 2006 - 2024
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
#include <string>
#include <algorithm>
#include <vector>
#include <map>
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

/// the default output encoding, in fact the only one we allow
const char *output_encoding = "UTF-8";

namespace folia {
  using TiCC::operator<<;

  ostream& operator<<( ostream& os,
		       const Document::annotation_info& at ){
    /// output an annotation_info structure (Debugging only)
    /*!
      \param os the output stream
      \param at the annotation_info object
    */
    os << "<" << at._annotator << "," << TiCC::toString(at._ann_type)
       << "," << at._date << "," << at._processors << ">";
    return os;
  }

  ostream& operator<<( ostream& os,
		       const Document::annotation_info *at ){
    /// output an annotation_info structure (Debugging only)
    /*!
      \param os the output stream
      \param at a pointer to the the annotation_info object
    */
    if ( at == NULL ){
      os << "Nill";
    }
    else {
      os << *at;
    }
    return os;
  }

  Document::Document(){
    /// create and initalize a FoLiA Document.
    init();
  }

  void Document::init_args( const KWargs& kwargs ){
    /// init some Document properties from a key-value list
    /*!
      \param kwargs a list of key-value pairs

      this function initializes a Document and can set the attributes
      \e 'debug' and \e 'mode'

      When the attributes \e 'file' or \e 'string' are found, the value is used
      to extract a complete FoLiA document from that file or string.
    */
    init();
    KWargs args = kwargs;
    string value = args.extract( "debug" );
    if ( !value.empty() ){
      debug = TiCC::stringTo<int>( value );
    }
    value = args.extract( "mode" );
    if ( !value.empty() ){
      setmode( value );
    }
    value = args.extract( "file" );
    if ( !value.empty() ){
      // extract a Document from a file
      read_from_file( value );
    }
    else {
      value = args.extract( "string" );
      if ( !value.empty() ){
	// extract a Document from a string
	read_from_string( value );
      }
    }
    if ( !foliadoc ){
      // so NO 'file' or 'string' argument.
      // (read_from_file/read_from_string create a foliadoc OR throw )
      if ( args.find( "version" ) == args.end() ){
	// no version attribute. set it to the current default
	args["version"] = folia_version();
      }
      // create an 'empty' document using the args, with a FoLiA root node.
      foliadoc = new FoLiA( args, this );
    }
  }

  Document::Document( const KWargs& kwargs ) {
    /// initialize a Document using an attribute-value list
    /*!
      \param kwargs an attribute-value list
     */
    init_args( kwargs );
  }

  Document::Document( const string& s ) {
    /// initialize a Document using a string (filename or attribute-value list)
    /*!
      \param s a string representing a filename OR an attribute value list

      the string \e s can be a string encoded attribute value list OR just a
      filename.

      Some examples:

      Document doc("my_first.folia.xml") creates a Document doc from
      the file \e my_first.folia.xml.

      Document doc( "file='my_first.folia.xml', debug='3', mode='nochecktext'" ) This creates a document from the file \e my_first.folia.xml with a
      debugging level of 3 and textchecking set to OFF

      Document doc( "xml:id='test'" ) creates a yet empty document with a
      document ID with value 'test'

    */
    KWargs args = getArgs(s);
    if ( args.empty() ){
      args["file"] = s;
    }
    init_args( args );
  }

  string folia_version(){
    /// return the FoLiA version of this build
    stringstream ss;
    ss << MAJOR_VERSION << "." << MINOR_VERSION << "." << SUB_VERSION;
    return ss.str();
  }

  string Document::doc_version() const {
    /// return the FoLiA version of this Document
    stringstream ss;
    ss << _major_version << "." << _minor_version << "." << _sub_version;
    return ss.str();
  }

  string library_version(){
    /// return the version of the library
    return VERSION;
  }

  string Document::update_version(){
    /// override the document version with the version of the build
    /*!
      \return the old value of the documents version
    */
    string old = _version_string;
    _version_string = folia_version();
    return old;
  }

  void Document::init(){
    /// initialize a Document structure with default values
    _metadata = 0;
    _foreign_metadata = 0;
    _provenance = 0;
    _xmldoc = 0;
    foliadoc = 0;
    _foliaNsIn_href = 0;
    _foliaNsIn_prefix = 0;
    _foliaNsOut = 0;
    debug = 0;
    mode = Mode( CHECKTEXT|AUTODECLARE );
    _external_document = false;
    _incremental_parse = false;
    _preserve_spaces = false;
    _warn_count = 0;
    _major_version = 0;
    _minor_version = 0;
    _sub_version = 0;
  }

  Document::~Document(){
    /// Destroy a Document structure including al it's members
    /*!
      This also finally deletes FoLiA nodes that were marked for deletion
      but not yet really destroyed. (because they might still be referenced)
     */
    xmlFreeDoc( _xmldoc );
    xmlFree( const_cast<xmlChar*>(_foliaNsIn_href) );
    xmlFree( const_cast<xmlChar*>(_foliaNsIn_prefix) );
    sindex.clear();
    if ( foliadoc ){
      foliadoc->destroy();
    }
    set<FoliaElement*> bulk;
    for ( const auto& it : delSet ){
      it->unravel( bulk );
    }
    for ( const auto& it : bulk ){
      it->destroy();
    }
    delete _metadata;
    delete _foreign_metadata;
    for ( const auto& [dummy, val] : submetadata ){
      delete val;
    }
    delete _provenance;
  }

  void Document::setmode( const string& ms ) const {
    /// Sets the  mode attributes of a document
    /*!
      \param ms an encoded string of attribute-values pairs giving modes
      \note mode is mutable, so this even sets mode on CONST documents!

      The following modes can be set:
      '(no)permissive' (default is NO),
      '(no)strip' (default is NO),
      '(no)canonical' (default is NO),
      '(no)checktext' (default is checktext),
      '(no)fixtext' (default is NO),
      '(no)autodeclare' (default is NO)

      example:

      doc.setmode( "strip,nochecktext,autodeclare" );
    */
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
      else if ( mod == "canonical" ){
	mode = Mode( (int)mode | CANONICAL );
      }
      else if ( mod == "nocanonical" ){
	mode = Mode( (int)mode & ~CANONICAL );
      }
      else if ( mod == "kanon" ){ // backward compatible
	mode = Mode( (int)mode | CANONICAL );
      }
      else if ( mod == "nokanon" ){ // backward compatible
	mode = Mode( (int)mode & ~CANONICAL );
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
      else if ( mod == "autodeclare" ){
	mode = Mode( int(mode) | AUTODECLARE );
      }
      else if ( mod == "noautodeclare" ){
	mode = Mode( int(mode) & ~AUTODECLARE );
      }
      else if ( mod == "explicit" ){
	mode = Mode( int(mode) | EXPLICIT );
      }
      else if ( mod == "noexplicit" ){
	mode = Mode( int(mode) & ~EXPLICIT );
      }
      else {
	throw invalid_argument( "FoLiA::Document: unsupported mode value: "+ mod );
      }
    }
  }

  string Document::getmode() const {
    /// returns the curent mode(s) as a string
    /*!
      \return a string explaining the modes set

      example:

      doc.getmode() might return: "mode=strip,nohecktext,autodeclare,"
     */
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
    else {
      result += "nochecktext,";
    }
    if ( mode & FIXTEXT ){
      result += "fixtext,";
    }
    if ( mode & CANONICAL ){
      result += "canonical,";
    }
    if ( mode & AUTODECLARE ){
      result += "autodeclare,";
    }
    else {
      result += "noautodeclare,";
    }
    if ( mode & EXPLICIT ){
      result += "explicit,";
    }
    return result;
  }

  bool Document::set_strip( bool new_val ) const{
    /// sets the 'strip' mode to on/off
    /*!
      \param new_val the boolean to use for on/off
      \return the previous value
    */
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
    /// sets the 'permissive' mode to on/off
    /*!
      \param new_val the boolean to use for on/off
      \return the previous value
    */
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
    /// sets the 'checktext' mode to on/off
    /*!
      \param new_val the boolean to use for on/off
      \return the previous value
    */
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
    /// sets the 'fixtext' mode to on/off
    /*!
      \param new_val the boolean to use for on/off
      \return the previous value
    */
    bool old_val = (mode & FIXTEXT);
    if ( new_val ){
      mode = Mode( (int)mode | FIXTEXT );
    }
    else {
      mode = Mode( (int)mode & ~FIXTEXT );
    }
    return old_val;
  }

  bool Document::set_canonical( bool new_val ) const{
    /// sets the 'canonical' mode to on/off
    /*!
      \param new_val the boolean to use for on/off
      \return the previous value
    */
    bool old_val = (mode & CANONICAL);
    if ( new_val ){
      mode = Mode( (int)mode | CANONICAL );
    }
    else {
      mode = Mode( (int)mode & ~CANONICAL );
    }
    return old_val;
  }

  bool Document::set_autodeclare( bool new_val ) const{
    /// sets the 'autodeclare' mode to on/off
    /*!
      \param new_val the boolean to use for on/off
      \return the previous value
    */
    bool old_val = (mode & AUTODECLARE);
    if ( new_val ){
      mode = Mode( (int)mode | AUTODECLARE );
    }
    else {
      mode = Mode( (int)mode & ~AUTODECLARE );
    }
    return old_val;
  }

  bool Document::set_explicit( bool new_val ) const{
    /// sets the 'explicit' mode to on/off
    /*!
      \param new_val the boolean to use for on/off
      \return the previous value
    */
    bool old_val = (mode & EXPLICIT);
    if ( new_val ){
      mode = Mode( (int)mode | EXPLICIT );
    }
    else {
      mode = Mode( (int)mode & ~EXPLICIT );
    }
    return old_val;
  }

  void Document::add_doc_index( FoliaElement* el ){
    /// add a FoliaElement to the index
    /*!
      \param el the FoliaElement to add
      will throw when \em el->id() is already in the index
     */
    const string my_id = el->id();
    if ( my_id.empty() ) {
      return;
    }
    auto it = sindex.find( my_id );
    if ( it == sindex.end() ){
      sindex[my_id] = el;
    }
    else {
      throw DuplicateIDError( my_id );
    }
  }

  void Document::del_doc_index( const string& id ){
    /// remove an id from the index
    /*!
      \param id The id to remove
    */
    if ( sindex.empty() ){
      // only when ~Document is in progress
      return;
    }
    if ( id.empty() ) {
      return;
    }
    sindex.erase(id);
  }

  string Document::annotation_type_to_string( AnnotationType ann ) const {
    /// return the ANNOTATIONTYPE translated to a string in a Document context.
    /// takes the version into account, for older labels
    /*!
      \param ann the annotationtype
      \return a string representation of \e ann.

      Taking into account the version of the Dcocument, translating to
      old labels for pre 1.6 versions
    */
    const string& result = toString( ann );
    if ( version_below(1,6) ){
      const auto& it = reverse_old.find(result);
      if ( it != reverse_old.end() ){
	return it->second;
      }
    }
    return result;
  }

  static void error_sink(void *mydata, xmlError *error ){
    /// helper function for libxml2 to catch and display problems in an
    /// orderly fashion
    /*!
      \param a pointer to a struct to hold persistent data. In our case just an
      int.
      \param error an xmlEror structure created by a libxml2 function

      For the first error encountered, a message is sent to stderr. Further
      errors are just counted. It is up to calling functions to react on a
      a count > 0
     */
    int *cnt = static_cast<int*>(mydata);
    if ( *cnt == 0 ){
      string line = "\n";
      if ( error->file ){
	line += string(error->file) + ":";
	if ( error->line > 0 ){
	  line += TiCC::toString(error->line) + ":";
	}
      }
      line += " XML-error: " + string(error->message);
      cerr << line;
      if ( error->ctxt ){
	xmlParserCtxt *ctx = static_cast<xmlParserCtxt*>(error->ctxt);
	xmlBuffer *buffer = xmlBufferCreate();
	int size = xmlNodeDump(buffer, ctx->myDoc, ctx->node, 0, 1 );
	cerr << string( ctx->nodeNr*2, ' ') << buffer->content << endl;
	xmlBufferFree( buffer );
	if ( size >=0
	     && error->int2 != 0 ){
	  cerr << string( std::min(error->int2,size), ' ') << "^" << endl;
	}
      }
    }
    (*cnt)++;
    return;
  }

  bool Document::read_from_file( const string& file_name ){
    /// read a FoLiA document from a file
    /*!
      \param file_name the name of the file
      \return true on succes. Will throw otherwise.

      This function also takes care of files in .bz2 or .gz format when the
      right extension is given.
    */
    ifstream is( file_name );
    if ( !is.good() ){
      throw invalid_argument( "file not found: " + file_name );
    }
    if ( foliadoc ){
      throw logic_error( "Document is already initialized" );
    }
    _source_name = file_name;
    if ( TiCC::match_back( file_name, ".bz2" ) ){
      string buffer = TiCC::bz2ReadFile( file_name );
      return read_from_string( buffer );
    }
    int cnt = 0;
    xmlSetStructuredErrorFunc( &cnt, (xmlStructuredErrorFunc)error_sink );
    _xmldoc = xmlReadFile( file_name.c_str(),
			   0,
			   XML_PARSER_OPTIONS );
    if ( _xmldoc ){
      if ( cnt > 0 ){
	throw DocumentError( file_name, "document is invalid" );
      }
      if ( debug ){
	cout << "read a doc from " << file_name << endl;
      }
      foliadoc = parseXml();
      if ( !validate_offsets() ){
	// cannot happen. validate_offsets() throws on error
	throw InconsistentText("MEH");
      }
      if ( debug ){
	if ( foliadoc ){
	  cout << "successful parsed the doc from: " << file_name << endl;
	}
	else {
	  cout << "failed to parse the doc from: " << file_name << endl;
	}
      }
      xmlFreeDoc( _xmldoc );
      _xmldoc = 0;
      return foliadoc != 0;
    }
    if ( debug ){
      cout << "Failed to read a doc from " << file_name << endl;
    }
    throw DocumentError( file_name, "No valid FoLiA read" );
  }

  bool Document::read_from_string( const string& buffer ){
    /// read a FoLiA Document from a string buffer
    /*!
      \param buffer A complete FoLiA document in a string buffer
      \return true on succes. Will throw otherwise.
     */
    if ( foliadoc ){
      throw logic_error( "Document is already initialized" );
    }
    int cnt = 0;
    xmlSetStructuredErrorFunc( &cnt, (xmlStructuredErrorFunc)error_sink );
    _xmldoc = xmlReadMemory( buffer.c_str(), buffer.length(), 0, 0,
			     XML_PARSER_OPTIONS );
    if ( _xmldoc ){
      _source_name = "memory-buffer";
      if ( cnt > 0 ){
	throw DocumentError( _source_name, "document is invalid" );
      }
      if ( debug ){
	cout << "read a doc from string" << endl;
      }
      foliadoc = parseXml();
      if ( !validate_offsets() ){
	// cannot happen. validate_offsets() throws on error
	throw InconsistentText("MEH");
      }
      if ( debug ){
	if ( foliadoc ){
	  cout << "successful parsed the doc" << endl;
	}
	else{
	  cout << "failed to parse the doc" << endl;
	}
      }
      xmlFreeDoc( _xmldoc );
      _xmldoc = 0;
      return foliadoc != 0;
    }
    if ( debug ){
      throw runtime_error( "Failed to read a doc from a string" );
    }
    return false;
  }

  ostream& operator<<( ostream& os, const Document *d ){
    /// output a Document to a stream
    /*!
      \param os the output stream
      \param d the document to output
     */
    if ( d ){
      os << d->toXml( "" );
      // the toXml() string already ends with a newline (i hope....)
      // but flush the stream
      os.flush();
    }
    else {
      os << "MISSING DOCUMENT" << endl;
    }
    return os;
  }

  bool Document::save( ostream& os,
		       const string& ns_label,
		       bool canonical ) const {
    /// save the Document to a stream
    /*!
      \param os the output stream
      \param ns_label the namespace name to use, the default is "" placing all
      FoLiA nodes in the default namespace.
      \param canonical determines to output in canonical order. Default is no.
    */
    bool old_k = set_canonical(canonical);
    os << toXml( ns_label );
    // the toXml() string already ends with a newline (i hope....)
    // but flush the stream
    os.flush();
    set_canonical(old_k);
    return os.good();
  }

  bool Document::save( const string& file_name,
		       const string& ns_label,
		       bool canonical ) const {
    /// save the Document to a file
    /*!
      \param file_name the name of the file to create
      \param ns_label the namespace name to use, the default is "" placing all
      FoLiA nodes in the default namespace.
      \param canonical determines to output in canonical order. Default is no.

      This function also takes care of output to files in .bz2 or .gz format
      when the right extension is given.
    */
    bool old_k = set_canonical(canonical);
    bool result = false;
    try {
      result = toXml( file_name, ns_label );
    }
    catch ( const exception& e ){
      throw runtime_error( "saving to file " + file_name + " failed: " + e.what() );
    }
    set_canonical( old_k );
    return result;
  }

  string Document::xmlstring( bool canonical ) const {
    /// dump the Document in a string buffer
    /*!
      \param canonical determines to output in canonical order. Default is no.
      \return the complete document in an unformatted string
    */
    bool old_k = set_canonical(canonical);
    xmlDoc *outDoc = to_xmlDoc( "" );
    set_canonical(old_k);
    xmlChar *buf; int size;
    xmlDocDumpFormatMemoryEnc( outDoc, &buf, &size,
			       output_encoding, 0 ); // no formatting
    string result = to_string( buf, size );
    xmlFree( buf );
    xmlFreeDoc( outDoc );
    _foliaNsOut = 0;
    return result;
  }

  FoliaElement* Document::index( const string& id ) const {
    /// search for the element with xml:id id
    /*!
      \param id the id we search
      \return the FoliaElement with this \e id or 0, when not present
     */
    const auto& it = sindex.find( id );
    if ( it == sindex.end() ){
      return 0;
    }
    else {
      return it->second;
    }
  }

  FoliaElement* Document::operator []( const string& id ) const {
    /// search for the element with xml:id id
    /*!
      \param id the id we search
      \return the FoliaElement with this \e id or 0, when not present

      example:

      FoliaElement *e = doc["doc.sent.1"];
      when Document doc has a node with id="doc.sent.1", \e e refer that node
      otherwise \e e will be set to 0;
    */
    return index(id);
  }

  UnicodeString Document::text( const TextPolicy& tp ) const {
    /// return the text content of the whole document, restricted by the
    /// parameters.
    /*!
      \param tp The TextPolicy to use
      \return the complete text matching the criteria as an UnicodeString
     */
    return foliadoc->text( tp );
  }

  UnicodeString Document::text( const string& cls,
				bool retaintok,
				bool strict ) const {
    /// return the text content of the whole document, restricted by the
    /// parameters.
    /*!
      \param cls The textclass to use fro searching.
      \param retaintok Should we retain the tokenization. Default NO.
      \param strict Should we perform a strict search? Default NO.
      \return the complete text matching the criteria as an UnicodeString
     */
    TEXT_FLAGS flags = TEXT_FLAGS::NONE;
    if ( retaintok ){
      flags = flags | TEXT_FLAGS::RETAIN;
    }
    if ( strict ){
      flags = flags | TEXT_FLAGS::STRICT;
    }
    return foliadoc->text( cls, flags );
  }

  static const set<ElementType> quoteSet = { Quote_t };
  static const set<ElementType> emptySet;

  vector<Sentence*> Document::sentences() const {
    /// return all Sentences in the Document, except those in Quotes
    return foliadoc->select<Sentence>( quoteSet );
  }

  vector<Sentence*> Document::sentenceParts() const {
    /// return all Sentences in the Document, including those in Quotes
    vector<Sentence*> sents = foliadoc->select<Sentence>( emptySet );
    return sents;
  }

  Sentence *Document::sentences( size_t index ) const {
    /// return the Sentence at position \e index
    /*!
      \param index  the index to search for
      \return The Sentence found.
      will throw when the index is out of range
    */
    vector<Sentence*> v = sentences();
    if ( index < v.size() ){
      return v[index];
    }
    throw range_error( "sentences() index out of range" );
  }

  Sentence *Document::rsentences( size_t index ) const {
    /// return the Sentence at position \e index from the back of the Document
    /*!
      \param index  the index to search for
      \return The Sentence found.
      will throw when the index is out of range
    */
    vector<Sentence*> v = sentences();
    if ( index < v.size() ){
      return v[v.size()-1-index];
    }
    throw range_error( "rsentences() index out of range" );
  }

  vector<Word*> Document::words() const {
    /// return all the Words in the Document, ignoring those within structure
    /// annotations
    /*!
      \return The Words found.
    */
    return foliadoc->select<Word>( default_ignore_structure );
  }

  Word *Document::words( size_t index ) const {
    /// return the Word at position \e index, ignoring those within structure
    /// annotations
    /*!
      \param index the index to search for
      \return The Word found.
      will throw when the index is out of range
    */
    vector<Word*> v = words();
    if ( index < v.size() ){
      return v[index];
    }
    throw range_error( "words() index out of range" );
  }

  Word *Document::rwords( size_t index ) const {
    /// return the Word at position \e index from the back of the Document,
    /// ignoring those within structure annotations
    /*!
      \param index the index to search for
      \return The Word found.
      will throw when the index is out of range
    */
    vector<Word*> v = words();
    if ( index < v.size() ){
      return v[v.size()-1-index];
    }
    throw range_error( "rwords() index out of range" );
  }

  vector<Paragraph*> Document::paragraphs() const {
    /// return all Paragraphs in the Document
    return foliadoc->select<Paragraph>();
  }

  Paragraph *Document::paragraphs( size_t index ) const {
    /// return the Paragraph at position \e index
    /*!
      \param index the index to search for
      \return The Paragraph found.
      will throw when the index is out of range
    */
    vector<Paragraph*> v = paragraphs();
    if ( index < v.size() ){
      return v[index];
    }
    throw range_error( "paragraphs() index out of range" );
  }

  Paragraph *Document::rparagraphs( size_t index ) const {
    /// return the Word at position \e index from the back of the Document
    /*!
      \param index the index to search for
      \return The Paragraph found.
      will throw when the index is out of range
    */
    vector<Paragraph*> v = paragraphs();
    if ( index < v.size() ){
      return v[v.size()-1-index];
    }
    throw range_error( "rparagraphs() index out of range" );
  }

  string Document::language() const {
    /// extract the language from the metadata
    /*!
      \return the metadata language value or "" when not set
    */
    string result;
    if ( _metadata ){
      result = _metadata->get_val("language");
    }
    return result;
  }

  string Document::metadata_type() const {
    /// returns the metadata type
    /*!
      \return the metadata type or "native" when not set
    */
    if ( _metadata ){
      return _metadata->type();
    }
    else if ( _foreign_metadata ){
      return _foreign_metadata->type();
    }
    return "native";
  }

  string Document::metadata_file() const {
    /// returns the metadata filename. if any
    /*!
      \return the metadata file name.
    */
    if ( _metadata ){
      if ( _metadata->datatype() != "ExternalMetaData" ){
	return "";
      }
      return _metadata->src();
    }
    return "";
  }

  void Document::parse_imdi( const xmlNode *node ){
    /// set IMDI values. DEPRECATED
    const xmlNode *n = TiCC::xPath( node, "//imdi:Session/imdi:Title" );
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

  void Document::set_metadata( const string& attribute, const string& value ){
    /// add a metadata attribute/value pair to the Document
    /*!
      \param attribute the attribute to set
      \param value the value of the attribute

      Will throw if the current metadata is NOT 'native'

      May create a new NativeMetaData structure.
     */
    if ( !_metadata ){
      _metadata = new NativeMetaData( "native" );
    }
    else if ( _metadata->datatype() == "ExternalMetaData" ){
      throw MetaDataError( "cannot set meta values on ExternalMetaData" );
    }
    if ( _metadata->type() == "imdi" ){
      throw MetaDataError( "cannot set meta values on IMDI MetaData" );
    }
    _metadata->add_av( attribute, value );
  }

  const string Document::get_metadata( const string& attribute ) const {
    /// return the metadata value for a metadata attribute
    /*!
      \param attribute the attribite to lookup
      \return the requested metadata value. May return "" if no metadata is
      available or the attribute is not found.
     */
    if ( _metadata ){
      return _metadata->get_val( attribute );
    }
    else {
      return "";
    }
  }

  processor *Document::get_default_processor() const {
    /// return the default processor for this document
    /*!
      \return the main processor in the provenance data. can be 0;
     */
    if ( _provenance ){
      return _provenance->get_top_processor();
    }
    else {
      return 0;
    }
  }

  processor *Document::get_processor( const string& pid ) const {
    /// return the processor with ID=pid
    /*!
      \param pid the processorID we look for
      \return the processor found, or 0
    */
    if ( _provenance ){
      return _provenance->get_processor_by_id( pid );
    }
    else {
      return 0;
    }
  }

  vector<processor*> Document::get_processors_by_name( const string& name ) const {
    /// return all the processor with name=name
    /*!
      \param name the name of the processors we look for
      \return al list of matching processors
    */
    vector<processor*> result;
    if ( _provenance ){
      result = _provenance->get_processors_by_name( name );
    }
    return result;
  }

  processor *Document::add_processor( const KWargs& args,
				      processor *parent ){
    /// create new processor and add it to the provenance data
    /*!
      \param args the argument list for creating the new provessor
      \param parent add the new processor as a child to this parent.
      When the parent = 0, add to the Documents provenance structure.

      May create a new Provenance structure if not yet available.
    */
    if ( debug ){
      cerr << "ADD_PROCESSOR: " << args << endl;
    }
    if ( !parent
	 && !_provenance ){
      _provenance = new Provenance(this);
    }
    processor *p = new processor( _provenance, parent, args );
    if ( parent ){
      parent->_processors.push_back( p );
    }
    else {
      _provenance->processors.push_back( p );
    }
    return p;
  }

  void Document::set_foreign_metadata( xmlNode *node ){
    /// create a ForeigMetaData element from 'node'
    /*!
      \param node the xml node we are parsing

      FoLiA treats foreign metadata by adding a copy of the xml tree under node
      to the folia, without further notice.
    */
    if ( !_foreign_metadata ){
      _foreign_metadata = new ForeignMetaData( "foreign" );
    }
    ForeignData *add = new ForeignData();
    if ( TiCC::Name( node ) != "foreign-data" ){
      // we need an extra layer then
      xmlNode *n = TiCC::XmlNewNode( "foreign-data" );
      xmlAddChild( n, xmlCopyNode( node, 1 ) );
      add->set_data( n );
      _foreign_metadata->add_foreign( n );
      xmlFreeNode (n );
    }
    else {
      add->set_data( node );
      _foreign_metadata->add_foreign( node );
    }
  }

  void Document::save_orig_ann_defaults(){
    /// make a copy of the _annotationdefaults
    /*!
      For incremental document creation (using folia::Engine) we need to
      'remember' which annotationdefaults there were initially, so before
      any new annotations are added with declare().

      But we only need those that would return a default annotation or
      default processor.
     */
    for ( const auto& [ann,value] : _annotationdefaults ){
      if ( value.size() == 1 ){
	// so 1 set
	_orig_ann_default_sets.insert( make_pair(ann,value.begin()->first) );
	auto procs = value.begin()->second._processors;
	if ( procs.size() == 1 ){
	  _orig_ann_default_procs.insert( make_pair(ann,*procs.begin()) );
	}
      }
    }
  }

  void Document::test_temporary_text_exception( const string& set_name ) const {
    //
    // Temporary exception. see: https://github.com/proycon/folia/issues/104
    //
    string def_set = default_set( AnnotationType::TEXT );
    auto const at_it = _annotationdefaults.find( AnnotationType::TEXT );
    if ( at_it != _annotationdefaults.end() // some Text annotation
	 && def_set != set_name ){ // not same default
      string my_set = at_it->second.begin()->first;
      throw DocumentError( _source_name,
			   "Multiple text_annotation: cannot add '" + set_name
			   + "' as text_annotation declaration,"
			   + " we already have: '" + my_set + "'" );
    }
  }

  void Document::parse_annotations( const xmlNode *node ){
    /// parse all annotation declarations from the Xml tree given by node
    /*!
      \param node an XML node to extract annotation data from

      The data found will be appended to the Document
    */
    if ( debug ){
      cerr << "parse annotations " << TiCC::Name(node) << endl;
    }
    xmlNode *n = node->children;
    _anno_sort.clear();
    while ( n ){
      string tag = TiCC::Name( n );
      if ( tag.length() > 11
	   && tag.substr( tag.length() - 11 ) == "-annotation" ){
	string prefix = tag.substr( 0,  tag.length() - 11 );
	AnnotationType at_type = TiCC::stringTo<AnnotationType>( prefix );
	if ( debug ){
	  cerr << "parse " << prefix << "-annotation" << endl;
	}
	KWargs atts = getAttributes( n );
	ElementType et = BASE;
	string set_name = atts.extract("set" );
	if ( at_type == AnnotationType::TEXT ){
	  //
	  // Temporary limitation. To be resolved
	  //
	  test_temporary_text_exception( set_name );
	}
	if ( set_name.empty() ){
	  if ( version_below( 1, 6 ) ){
	    set_name = "undefined"; // default value
	  }
	  else if ( at_type == AnnotationType::TEXT ){
	    if ( debug ){
	      cerr << "assign default for TEXT: " <<  DEFAULT_TEXT_SET << endl;
	    }
	    set_name = DEFAULT_TEXT_SET;
	  }
	  else if ( at_type == AnnotationType::PHON ){
	    if ( debug ){
	      cerr << "assign default for PHON: " <<  DEFAULT_PHON_SET << endl;
	    }
	    set_name = DEFAULT_PHON_SET;
	  }
	  else {
	    auto et_it = annotationtype_elementtype_map.find( at_type );
	    if ( et_it == annotationtype_elementtype_map.end() ){
	      throw logic_error( "no matching element_type for annotation_type: "
				 + prefix );
	    }
	    et = et_it->second;
	    properties *prop = element_props[et];
	    if ( prop->REQUIRED_ATTRIBS & Attrib::CLASS ) {
	      throw DocumentError( _source_name,
				   "setname may not be empty for " + prefix
				   + "-annotation",
				   xmlGetLineNo(n) );
	    }
	  }
	}
	if ( set_name.empty() ){
	  set_name = "None";
	}
	string format = atts.extract( "format" );
	string annotator = atts.extract( "annotator" );
	string ann_type = atts.extract( "annotatortype" );
	string datetime = parseDate( atts.extract( "datetime" ) );
	string my_alias = atts.extract( "alias" );
	string gran_val = atts.extract( "groupannotations" );
	if ( !gran_val.empty() ){
	  if ( !isSubClass( et, AbstractSpanAnnotation_t ) ){
	    throw DocumentError( _source_name,
				 "attribute 'groupannotations' not allowed for '"
				 + prefix + "-annotation",
				 xmlGetLineNo(n) );
	  }
	  if ( gran_val == "yes"
	       || gran_val == "true" ){
	    _groupannotations[at_type][set_name] = true;
	  }
	  else {
	    throw DocumentError( _source_name,
				 "invalid value '" + gran_val
				 + "' for attribute groupannotations",
				 xmlGetLineNo(n) );
	  }
	}
	else {
	  _groupannotations[at_type][set_name] = false;
	}
	set<string> processors;
	const xmlNode *sub = n->children;
	while ( sub ){
	  string subtag = TiCC::Name( sub );
	  if ( debug ){
	    cerr << "parse subtag:" << subtag << endl;
	  }
	  if ( subtag == "annotator" ){
	    KWargs args = getAttributes( sub );
	    string proc_id = args["processor"];
	    if ( proc_id.empty() ){
	      throw DocumentError( _source_name,
				   tag + " <annotator> misses attribute 'processor'",
				   xmlGetLineNo(n) );
	    }
	    if ( !get_processor( proc_id ) ){
	      throw DocumentError( _source_name,
				   tag + " uses undefined processor: '"
				   + proc_id + "'",
				   xmlGetLineNo(n) );
	    }
	    processors.insert( proc_id );
	  }
	  sub = sub->next;
	}
	if ( !annotator.empty() && !processors.empty() ){
	  throw DocumentError( _source_name,
			       tag
			       + " has both <annotator> node(s) and annotator attribute.",
			       xmlGetLineNo(n) );
	}
	internal_declare( at_type, set_name, format,
			  annotator, ann_type, datetime,
			  processors, my_alias );
	if ( !atts.empty() ){

	  throw DocumentError( _source_name,
			       "found invalid attribute(s) in <" + prefix
			       + "-declaration> " + atts.toString(),
			       xmlGetLineNo(n) );
	}
      }
      n = n->next;
    }
    if ( debug ){
      cerr << "all group annotations: " << _groupannotations << endl;
      cerr << "done with parse_annotation: " << _annotationdefaults << endl;
      cerr << "sorting: " << _anno_sort << endl;
    }
  }

  void Document::parse_provenance( const xmlNode *node ){
    /// parse provenance data from the XmlTree under node
    /*!
      \param node an XML node to extract provenance data from

      The data found will be appended to the Document
    */
    Provenance *result = new Provenance(this);
    const xmlNode *n = node->children;
    while ( n ){
      string tag = TiCC::Name( n );
      if ( tag == "processor" ){
	result->parse_processor(n);
      }
      n = n->next;
    }
    _provenance = result;
    //    cerr << "provenance=" << _provenance << endl;
  }

  void Document::parse_submeta( const xmlNode *node ){
    /// parse sub metadata from the XmlTree under node
    /*!
      \param node an XML node to extract submeta data from

      The data found will be appended to the Document
    */
    if ( node ){
      KWargs node_att = getAttributes( node );
      string my_id = node_att["xml:id"];
      if ( my_id.empty() ){
	throw MetaDataError( "submetadata without xml:id" );
      }
      //      cerr << "parse submetadata, id=" << my_id << endl;
      string type = node_att["type"];
      //      cerr << "parse submetadata, type=" << type << endl;
      if ( type.empty() ){
	type = "native";
      }
      string src = node_att["src"];
      if ( !src.empty() ){
	submetadata[my_id] = new ExternalMetaData( type, src );
	//	cerr << "created External metadata, id=" << my_id << endl;
      }
      else if ( type == "native" ){
	submetadata[my_id] = new NativeMetaData( type );
	//	cerr << "created Native metadata, id=" << my_id << endl;
      }
      else {
	submetadata[my_id] = 0;
	//	cerr << "set metadata to 0, id=" << my_id << endl;
      }
      const xmlNode *p = node->children;
      while ( p ){
	if ( p->type == XML_ELEMENT_NODE ){
	  if ( TiCC::Name(p) == "meta" &&
	       checkNS( p, NSFOLIA ) ){
	    if ( type == "native" ){
	      string txt = TiCC::XmlContent( p );
	      if ( !txt.empty() ){
		KWargs att = getAttributes( p );
		string sid = att["id"];
		submetadata[my_id]->add_av( sid, txt );
		// cerr << "added node to id=" << my_id
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
	    else if ( submetadata[my_id] == 0 ){
	      submetadata[my_id] = new ForeignMetaData( type );
	      //	      cerr << "add new Foreign " << my_id << endl;
	    }
	    //	  cerr << "in  Foreign " << submetadata[my_id]->type() << endl;
	    submetadata[my_id]->add_foreign( p );
	    //	    cerr << "added a foreign id=" << my_id << endl;
	  }
	}
	p = p->next;
      }
    }
  }

  bool is_number( const string& s ){
    /// check that every character in s is a digit
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
    /// expand a version string vs into ints major, minor and sub
    /*!
      \param[in] vs A string holding version information
      \param[out] major the major version found
      \param[out] minor the minor version found
      \param[out] sub the sub version found
      \param[out] patch the NON-numeric remainder of vs after parsing

      examples:

      "2.1" ==> major=2, minor=1

      "2.0.3-a" ==> major=2, minor=0, sub=3 patch=-a
     */
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
    /// check a version given by 'vers' against the current build
    /*!
      \param vers a version string (like "2.1.5")
      \return 0 when major, minor AND sub version are equal, -1 when the version
      is lower and 1 when the version is greater then the current build

     */
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

  int Document::compare_to_build_version() const {
    /// check the version of the document against the build version
    /*!
      \return 0 when the versions match, -1 when the document version
      is lower and 1 when the version is greater then the current build
    */
    return check_version( version() );
  }

  bool Document::version_below( int major, int minor ) const {
    /// check if current document version is strict lower then asked
    /*!
      \param major the major version we want
      \param minor the minor version we want
      \return true when the Document's major version is lower than mjor OR
      it is equal, but the Document's minor version is lower than minor.
    */
    if ( _major_version < major ){
      return true;
    }
    else if ( _major_version == major ){
      return _minor_version < minor;
    }
    return false;
  }

  void Document::adjustTextMode(){
    /// set the text checking mode of the Document based on an environment
    /// variable and the document version
    /*!
      When the FOLIA_TEXT_CHECK environment variable is set to YES or NO then
      set the CHECKTEXT mode accordingly.

      When the document version is below 1.5 we disable CHECKTEXT except when
      FIXTEXT is also set.
     */
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
  }

  void Document::setDocumentProps( KWargs& kwargs ){
    /// set general properties based on an attribute-value list
    /*!
      \param kwargs the arguments. Normally these are parsed attributes from
      \<FoLiA\> node.
      Even with an empty kwarg list, at least the version of the document is
      set. We use a special value (1.4.987) to signal that is was not
      specified.
     */
    string value = kwargs.extract( "version" );
    if ( !value.empty() ){
      _version_string = value;
      //      cerr << "So we found version " << _version_string << endl;
    }
    else {
      // assign a 'random' version, but PRE 1.5
      _version_string = "1.4.987";
      //      cerr << "NO VERSION version " << _version_string << endl;
    }
    expand_version_string( _version_string,
			   _major_version,
			   _minor_version,
			   _sub_version,
			   _patch_version );
    if ( check_version( _version_string ) > 0 ){
      cerr << "WARNING!!! the Document "
	   << (_source_name.empty()?"":"'")
	   << _source_name
	   << (_source_name.empty()?"":"' ")
	   << "is created for newer FoLiA version than this library ("
	   << _version_string << " vs " << folia_version()
	   << ")\n\t Any possible subsequent failures in parsing or processing may probably be attributed to this." << endl
	   << "\t Please upgrade libfolia!" << endl;
      increment_warn_count();
    }

    adjustTextMode();
    value = kwargs.extract( "external" );
    if ( !value.empty() ){
      _external_document = TiCC::stringTo<bool>( value );
    }
    else {
      _external_document = false;
    }
    bool happy = false;
    value = kwargs.extract( "_id" ); // for backward compatibility
    if ( value.empty() ){
      value = kwargs.extract( "xml:id" );
    }
    if ( !value.empty() ){
      if ( isNCName( value ) ){
	_id = value;
      }
      else {
	throw DocumentError( _source_name,
			     "'" + value + "' is not a valid NCName." );
      }
      happy = true;
      kwargs["xml:id"] = value;
    }
    if ( !foliadoc && !happy ){
      throw runtime_error( "No Document ID specified" );
    }
    kwargs.erase( "generator" ); // also delete this unused att-val
    kwargs.erase( "form" ); //silently discard form attribute (for normal vs explicit form), we should be able to read either fine
  }

  void Document::resolveExternals(){
    /// resolve all external references
    /*!
      external references are stored during parsing in the _externals array
     */
    if ( !_externals.empty() ){
      for ( const auto& ext : _externals ){
	ext->resolve_external();
      }
    }
  }

  void Document::fixup_metadata(){
    _metadata = new NativeMetaData( "native" );
  }

  void Document::parse_metadata( const xmlNode *node ){
    /// parse metadata information from the XmlTree under node
    /*!
      \param node an XML node to extract meta data from

      The data found will be appended to the Document
    */
    KWargs atts = getAttributes( node );
    string type = TiCC::lowercase(atts["type"]);
    if ( debug > 5 ){
      cerr << "metadata type='" << type << "'" << endl;
    }
    if ( type.empty() ){
      type = "native";
      if ( debug > 5 ){
	cerr << "metadata type FORCED to'" << type << "'" << endl;
      }
    }
    string src = atts["src"];
    if ( !src.empty() ){
      _metadata = new ExternalMetaData( type, src );
    }
    else if ( type == "native" || type == "imdi" ){
      _metadata = new NativeMetaData( type );
    }
    else {
      _foreign_metadata = new ForeignMetaData( type );
    }
    xmlNode *m = node->children;
    const xmlNode *a_node = 0;
    while ( m ){
      if ( TiCC::Name(m)  == "METATRANSCRIPT" ){
	if ( !checkNS( m, NSIMDI ) || type != "imdi" ){
	  throw runtime_error( "imdi != imdi " );
	}
	if ( debug > 1 ){
	  cerr << "found IMDI" << endl;
	}
	if ( !_foreign_metadata ){
	  _foreign_metadata = new ForeignMetaData( "imdi" );
	}
	_foreign_metadata->add_foreign( xmlCopyNode(m,1) );
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
	if ( debug > 1 ){
	  cerr << "found provenance data" << endl;
	}
	parse_provenance( m );
	//	cerr << _provenance << endl;
      }
      else if ( TiCC::Name( m ) == "meta" &&
		checkNS( m, NSFOLIA ) ){
	if ( debug > 1 ){
	  cerr << "found meta node:" << getAttributes(m) << endl;
	}
	if ( !_metadata ){
	  if ( type == "external" ){
	    throw runtime_error( "cannot add 'meta' nodes to external metadata" );

	  }
	  _metadata = new NativeMetaData( "native" );
	}
	KWargs att = getAttributes( m );
	string meta_id = att["id"];
	string val = TiCC::XmlContent( m );
	string get = _metadata->get_val( meta_id );
	if ( !get.empty() ){
	  throw runtime_error( "meta tag with id=" + meta_id
			       + " is defined more then once " );
	}
	_metadata->add_av( meta_id, val );
      }
      else if ( TiCC::Name(m)  == "foreign-data" &&
		checkNS( m, NSFOLIA ) ){
	FoliaElement *t = AbstractElement::createElement( "foreign-data", this );
	if ( t ){
	  t = t->parseXml( m );
	  if ( t ){
	    if ( !_foreign_metadata ){
	      _foreign_metadata = new ForeignMetaData( type );
	    }
	    _foreign_metadata->add_foreign( m );
	  }
	}
      }
      else if ( TiCC::Name(m)  == "submetadata" &&
		checkNS( m, NSFOLIA ) ){
	parse_submeta( m );
      }
      m = m->next;
    }
    if ( a_node ){
      //      cerr << "parse deferred annotations" << endl;
      parse_annotations( a_node );
    }
    if ( !_metadata && type == "imdi" ){
      // imdi missing all further info
      _metadata = new NativeMetaData( type );
    }
  }

  void Document::addStyle( const string& type, const string& href ){
    /// add style-sheet information
    /*!
      \param type Which type of sheet
      \param href the external link for this sheet
      We assure that only one "text/xsl" style-sheet is present. All
      other style-sheets are silently added as is.
     */
    if ( type == "text/xsl" ){
      const auto& it = styles.find( type );
      if ( it != styles.end() ){
	throw DocumentError( _source_name,
			     "multiple 'text/xsl' style-sheets defined." );
      }
    }
    styles.insert( make_pair( type, href ) );
  }

  void Document::replaceStyle( const string& type,
			       const string& href ){
    /// replace a style-sheet
    /*!
      \param type Which type of sheet
      \param href the external link for this sheet

      \note this is sloppy, as multiple sheets with the same type may exist
      (except for 'text/xslt') and we replace the first one only.
    */
    const auto& it = styles.find( type );
    if ( it != styles.end() ){
      it->second = href;
    }
    else {
      styles.insert( make_pair( type, href ) );
    }
  }

  void Document::parse_styles(){
    /// retrieve all style-sheets from the current XmlTree
    const xmlNode *pnt = _xmldoc->children;
    while ( pnt ){
      // search for Processing Instructions, ignore all but stylesheet ones
      if ( pnt->type == XML_PI_NODE && TiCC::Name(pnt) == "xml-stylesheet" ){
	string content = TextValue(pnt);
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
	  throw DocumentError( _source_name,
			       "problem parsing line: " + content,
			       xmlGetLineNo(pnt) );
      }
      }
      else if ( pnt->type == XML_COMMENT_NODE ) {
	string xml_tag = "_XmlComment";
	FoliaElement *t = AbstractElement::createElement( xml_tag, this );
	if ( t ) {
	  if ( debug > 2 ) {
	    cerr << "created " << t << endl;
	  }
	  t = t->parseXml( pnt );
	  if ( t ) {
	    if ( debug > 2 ) {
	      cerr << "extend " << this << " met " << t << endl;
	    }
	    preludes.push_back(t);
	  }
	}
      }
      pnt = pnt->next;
    }
  }

  void fixupNs( xmlNode *p, xmlNs *ns ){
    /// make sure that all XmlNodes in the tree p get namespace ns
    /*!
      \param p an XmlTree (fragment)
      \param ns the Namespace value to set
      This function is used when a Document uses PERMISSIVE mode
     */
    while ( p ){
      xmlSetNs( p, ns );
      fixupNs( p->children, ns );
      p = p->next;
    }
  }

  bool Document::validate_offsets() const {
    /// Validate all the offset values as found in all \<t\> and \<ph\> nodes
    /*!
      During Document parsing, \<t\> and \<ph\> nodes are stored in a buffer
      until the whole parsing is done.

      Then we are able to examine those nodes in their context and check the
      offsets used.
     */
    set<TextContent*> t_done;
    int cumulated_offset = 0;
    for ( auto txt_it=t_offset_validation_buffer.begin();
	  txt_it != t_offset_validation_buffer.end();
	  ++txt_it ){
      TextContent *txt = *txt_it;
      if ( t_done.find( txt ) != t_done.end() ){
	continue;
      }
      t_done.insert(txt);
      int offset = txt->offset();
      if ( offset != -1 ){
	try {
	  txt->get_reference(cumulated_offset);
	}
	catch( const UnresolvableTextContent& e ){
	  string msg = "Text for " + txt->parent()->xmltag() + "(ID="
	    + txt->parent()->id() + ", textclass='" + txt->cls()
	    + "'), has incorrect offset " + TiCC::toString(offset);


	  string ref = txt->ref();
	  if ( !ref.empty() ){
	    msg += " or invalid reference:" + ref;
	  }
	  msg += "\n\toriginal msg=";
	  msg += e.what();

          bool warn = false;
          try {
	    txt->get_reference( cumulated_offset, false ); //trim_spaces = false
	    msg += "\nHowever, according to the older rules (<v2.4.1) the offsets are accepted. So we are treating this as a warning rather than an error. We do recommend fixing this if this is a document you intend to publish.";
	    warn = true;
          }
	  catch ( const UnresolvableTextContent& ) {
	    msg += "\n(also checked against older rules prior to FoLiA v2.4.1)";
          }

          if ( warn ){
	    increment_warn_count();
	    cerr << "WARNING: " << msg << endl;
	  }
          else {
	    throw UnresolvableTextContent( msg );
	  }
	}
      }
    }
    set<PhonContent*> p_done;
    cumulated_offset = 0;
    for ( const auto& phon : p_offset_validation_buffer ){
      if ( p_done.find( phon ) != p_done.end() ){
	continue;
      }
      p_done.insert(phon);
      int offset = phon->offset();
      if ( offset != -1 ){
	try {
	  phon->get_reference( cumulated_offset );
	}
	catch( const UnresolvableTextContent& e ){
	  string msg = "Phoneme for " + phon->parent()->xmltag() + ", ID="
	    + phon->parent()->id() + ", textclass='" + phon->cls()
	    + "', has incorrect offset " + TiCC::toString(offset);


	  string ref = phon->ref();
	  if ( !ref.empty() ){
	    msg += " or invalid reference:" + ref;
	  }
	  msg += "\n\toriginal msg=";
	  msg += e.what();

          bool warn = false;
          try {
	    phon->get_reference(cumulated_offset, false ); //trim_spaces = false
	    msg += "\nHowever, according to the older rules (<v2.4.1) the offsets are accepted. So we are treating this as a warning rather than an error. We do recommend fixing this if this is a document you intend to publish.";
	    warn = true;
          }
	  catch ( const UnresolvableTextContent& ) {
	    msg += "\n(also checked against older rules prior to FoLiA v2.4.1)";
          }

          if (warn){
	    increment_warn_count();
	    cerr << "WARNING: " << msg << endl;
	  }
          else {
	    throw UnresolvableTextContent( msg );
	  }
	}
      }
    }
    return true;
  }

  FoliaElement* Document::parseXml( ){
    /// parse a complete FoLiA tree from the XmlTree we have got in _xmldoc
    /*!
      \return an FoLiA element node, whicht is the root of the FoLiA Document
    */
    parse_styles();
    xmlNode *root = xmlDocGetRootElement( _xmldoc );
    if ( root->ns ){
      if ( root->ns->prefix ){
	_foliaNsIn_prefix = xmlStrdup( root->ns->prefix );
      }
      _foliaNsIn_href = xmlStrdup( root->ns->href );
    }
    if ( debug > 2 ){
      string dum;
      cerr << "root = " << TiCC::Name( root ) << endl;
      cerr << "in namespace " << TiCC::getNS( root, dum ) << endl;
      cerr << "namespace list" << TiCC::getDefinedNS( root ) << endl;
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
	    throw DocumentError( _source_name,
				 "Folia Document should have namespace declaration "
				 + NSFOLIA + " but none found " );
	  }
	}
	else if ( ns != NSFOLIA ){
	  throw DocumentError( _source_name,
			       "Folia Document should have namespace declaration "
			       + NSFOLIA + " but found: " + ns );
	}
	try {
	  FoLiA *folia = new FoLiA( this );
	  result = folia->parseXml( root );
	  resolveExternals();
	}
	catch ( const InconsistentText& e ){
	  throw;
	}
	catch ( const DocumentError& e ){
	  throw;
	}
	catch ( const XmlError& e ){
	  throw;
	}
	catch ( const DeclarationError& e ){
	  throw;
	}
	catch ( const ValueError& e ){
	  throw;
	}
	catch ( const exception& e ){
	  throw DocumentError( _source_name, e.what() );
	}
      }
      else if ( TiCC::Name( root ) == "DCOI" &&
		checkNS( root, NSDCOI ) ){
	throw DocumentError( _source_name, "DCOI format not supported" );
      }
      else {
	throw DocumentError( _source_name, "root node must be FoLiA" );
      }
    }
    return result;
  }

  void Document::auto_declare( AnnotationType type,
			       const string& _setname ) {
    /// create a default declaration for the given AnnotationType
    /*!
      \param type which default do we want to add
      \param _setname which setname to add
      If _setname is empty, that is used, except for TEXT and PHON, which
      have a default setname which is assigned
     */
    string setname = _setname;
    if ( setname.empty() ) {
      if ( type == AnnotationType::TEXT ){
	setname = DEFAULT_TEXT_SET;
      }
      else if ( type == AnnotationType::PHON ){
	setname = DEFAULT_PHON_SET;
      }
    }
    declare( type, setname );
  }

  void Document::declare( AnnotationType type,
			  const string& setname,
			  const string& args ){
    /// Add an annotation declaration
    /*!
      \param type The AnnotationType for which to add a setname
      \param setname The Set name to add
      \param args a string representation of an attribute-value list with
      additional parameters
    */
    KWargs kwargs = getArgs( args );
    return declare( type, setname, kwargs );
  }

  void Document::declare( AnnotationType type,
			  const string& setname,
			  const KWargs& _args ){
    /// Add an annotation declaration
    /*!
      \param type The AnnotationType for which to add a setname
      \param setname The Set name to add
      \param _args an attribute-value list with additional parameters
    */
    KWargs args = _args;
    if ( debug ){
      cerr << "declare( " << folia::toString(type) << "," << setname << ", ["
	   << args << "] )" << endl;
    }
    string st = setname;
    if ( st.empty() ){
      if ( version_below( 1, 6 ) ){
	st = "undefined";
      }
      else {
	string prefix = folia::toString(type);
	auto et_it = annotationtype_elementtype_map.find( type );
	if ( et_it == annotationtype_elementtype_map.end() ){
	  throw logic_error( "no matching element_type for annotation_type: "
			     + prefix );
	}
	auto et = et_it->second;
	properties *prop = element_props[et];
	if ( prop->REQUIRED_ATTRIBS & Attrib::CLASS ) {
	  throw DocumentError( _source_name,
			       "setname may not be empty for " + prefix
			       + "-annotation" );
	}
      }
      if ( st.empty() ){
	st = "None";
      }
    }
    set<string> processors;
    string a = args["annotator"];
    string t = args["annotatortype"];
    string f = args["format"];
    string d = args["datetime"];
    string my_alias = args["alias"];
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
      throw DocumentError( _source_name,
			   "declaration: expected 'annotator', 'annotatortype'"
			   " 'processor', 'alias' or 'datetime', got '"
			   + args.begin()->first + "'" );
    }
    internal_declare( type, st, f, a, t, d, processors, my_alias );
  }

  string Document::unalias( AnnotationType type,
			    const string& my_alias ) const {
    /// resolve an alias for a setname to the full setname
    /*!
      \param type the AnnotationType
      \param my_alias the alias to resolve
      \return the setname belonging to alias for this type, or alias if not
      found
    */
    const auto& ti = _alias_set.find(type);
    if ( ti != _alias_set.end() ){
      const auto& sti = ti->second.find( my_alias );
      if ( sti != ti->second.end() ){
	return sti->second;
      }
    }
    return my_alias;
  }

  string Document::alias( AnnotationType type,
			  const string& setname ) const {
    /// give the alias for a setname
    /*!
      \param type the AnnotationType
      \param setname the alias to resolve
      \return the alias belonging setname for this type, or setname if
      not found
     */
    const auto& ti = _set_alias.find(type);
    if ( ti != _set_alias.end() ){
      const auto& ali = ti->second.find( setname );
      if ( ali != ti->second.end() ){
	return ali->second;
      }
    }
    return setname;
  }

  Document::annotation_info* Document::lookup_default( AnnotationType type,
						       const string& setname ){
    /// search for an annotation declaration for this type:setname
    /*!
      \param type the AnnotationType
      \param setname set name, may be empty, triggering a 'wildcard' result
      \return a pointer to the declaration found, or 0 when not found
    */
    if ( type == AnnotationType::NO_ANN ){
      return 0;
    }
    annotation_info *current = 0;
    auto const& t_it = _annotationdefaults.find( type );
    if ( t_it != _annotationdefaults.end() ){
      // there is a match on type
      if ( setname.empty() ){
	// 'wildcard' search
	if ( t_it->second.size() == 1 ){
	  // so it is unique, return it's first entry
	  current = &t_it->second.begin()->second;
	}
      }
      else {
	// setname may be an alias, so resolve
	auto s_it = t_it->second.find( unalias(type,setname) );
	if ( s_it != t_it->second.end() ){
	  current = &s_it->second;
	}
      }
    }
    return current;
  }

  Document::annotation_info const *Document::lookup_default( AnnotationType type,
							     const string& setname ) const {
    /// search for an annotation declaration for this type:setname
    /*!
      \param type the AnnotationType
      \param setname set name, may be empty, triggering a 'wildcard' result
      \return a pointer to the declaration found, or 0 when not found
    */
    if ( type == AnnotationType::NO_ANN ){
      return 0;
    }
    const annotation_info *current = 0;
    auto const& t_it = _annotationdefaults.find( type );
    if ( t_it != _annotationdefaults.end() ){
      // there is a match on type
      if ( setname.empty() ){
	// 'wildcard' search
	if ( t_it->second.size() == 1 ){
	  // so it is unique, return it's first entry
	  current = &t_it->second.begin()->second;
	}
      }
      else {
	// setname may be an alias, so resolve
	auto s_it = t_it->second.find( unalias(type,setname) );
	if ( s_it != t_it->second.end() ){
	  current = &s_it->second;
	}
      }
    }
    return current;
  }

  void Document::internal_declare( AnnotationType type,
				   const string& setname,
				   const string& format,
				   const string& annotator,
				   const string& annotator_type,
				   const string& date_time,
				   const set<string>& _processors,
				   const string& _alias ){
    /// Add an annotation declaration
    /*!
      \param type The AnnotationType for which to add a setname
      \param setname The Set name to add
      \param format the format to add
      \param annotator the name of the annotator
      \param annotator_type the type of annotator
      \param date_time the date and time to set. The value "now()" will set it
      to the current time.
      \param _processors a set of processor id's to relate to this declaration
      \param _alias an alias value for the setname
    */
    if ( debug ){
      cerr << "internal_declare( " << folia::toString(type) << "," << setname
	   << ", format=" << format << "," << annotator << ","
	   << annotator_type << "," << date_time << "," << _alias << ","
	   << _processors << ") " << endl;
    }
    AnnotatorType anno_type = UNDEFINED;
    try {
      anno_type = TiCC::stringTo<AnnotatorType>( annotator_type );
    }
    catch (...) {
      throw XmlError( "internal_declare(): illegal value '"
		      + annotator_type + "' for annotator type" );
    }
    if ( type == AnnotationType::TEXT ){
      //
      // Temporary limitation. To be resolved
      //
      test_temporary_text_exception( setname );
    }
    if ( !_alias.empty() ){
      string set_ali = alias(type,setname);
      if ( !set_ali.empty() ){
	if ( set_ali != setname
	     && set_ali != _alias ){
	  throw DeclarationError( "setname: '" + setname + "' already has an alias: '"
			  + set_ali );
	}
      }
      string ali_ali = alias(type,_alias);
      string ali_set = unalias(type,_alias);
      if ( ali_ali != _alias ){
	throw DeclarationError( "alias: '" + _alias +
			"' is also in use as a setname for set:'"
			+ ali_set + "'" );
      }
      if ( ali_set != _alias
	   && ali_set != setname ){
	throw DeclarationError( "alias: '" + _alias + "' already used for setname: '"
			+ ali_set + "'" );
      }
    }
    set<string> procs = _processors;
    annotation_info *current = lookup_default( type, setname );
    if ( current != 0 ){
      // there is already a fitting declaration, enrich it.
      if ( debug ){
	cerr << "a declaration exists for: " << type << ":" << setname << endl;
	cerr << "value: " << current << endl;
      }
      if ( !procs.empty() ){
	// add the extra processor id's to the set of processsor ID's
	if ( debug ){
	  cerr << "add extra procs: " << procs << endl;
	}
	for ( const auto& p : procs ){
	  current->_processors.insert( p );
	}
      }
      else {
	// old style, overwrite the existing annotator info
	string date = date_time;
	if ( date == "now()" ){
	  date = get_ISO_date();
	}
	*current = annotation_info(annotator,anno_type,date,format,procs);
	if ( debug ){
	  cerr << "overwrite with " << current << endl;
	}
      }
      if ( debug ){
	cerr << "NOW: " << current << endl;
      }
    }
    else {
      // No declaration yet, create one
      if ( debug ){
	cerr << "NO declaration exists for: " << type << ":" << setname << endl;
      }
      string date = date_time;
      if ( date == "now()" ){
	date = get_ISO_date();
      }
      annotation_info new_a(annotator,anno_type,date,format,procs);
      _annotationdefaults[type].insert( make_pair( setname, new_a ) );
      if ( debug ){
	cerr << "ADD to sort: " << folia::toString(type) << " ("
	     << setname << ")"  << endl;
      }
      _anno_sort.push_back(make_pair(type,setname));
      _annotationrefs[type][setname] = 0;
    }
    if ( !_alias.empty() ){
      _alias_set[type][_alias] = setname;
      _set_alias[type][setname] = _alias;
    }
    else {
      _alias_set[type][setname] = setname;
      _set_alias[type][setname] = setname;
    }
  }

  void Document::un_declare( AnnotationType type,
			     const string& set_name ){
    /// remove a declaration for an AnnotationType/setname pair
    /*!
      \param type the AnnotationType
      \param set_name the setname. May be empty ("")

      When \em set_name is "", ALL declarations of \em type are deleted
     */
    string setname = unalias(type,set_name);
    if ( debug ){
      cerr << "undeclare: " << folia::toString(type) << "(" << set_name << "."
	   << setname << ")" << endl;
    }
    if ( _annotationrefs[type][setname] != 0 ){
      throw DeclarationError( "unable to undeclare " + toString(type) + "-type("
		      + setname + ") (some references remain)" );
    }
    auto const adt = _annotationdefaults.find(type);
    if ( adt != _annotationdefaults.end() ){
      if ( debug ){
	cerr << "matched type=" << folia::toString(type) << endl;
      }
      auto it = adt->second.begin();
      while ( it != adt->second.end() ){
	if ( debug ){
	  cerr << "zoek set:" << setname << endl;
	}
	if ( setname.empty() || it->first == setname ){
	  if ( debug ){
	    cerr << "erase:" << setname << "==" << it->first << endl;
	  }
	  it = adt->second.erase(it);
	}
	else {
	  ++it;
	}
      }
      if ( debug ){
	cerr << "ANNO-SORT: IN " << _anno_sort << endl;
      }
      auto it2 = _anno_sort.begin();
      while ( it2 != _anno_sort.end() ){
	if ( debug ){
	  cerr << "zoek set:" << setname << endl;
	}
	if ( it2->first == type
	     && ( setname.empty() || it2->second == setname ) ){
	  if ( debug ){
	    cerr << "_annosort:erase:" << setname << "==" << it->first << endl;
	  }
	  it2 = _anno_sort.erase( it2 );
	}
	else {
	  ++it2;
	}
      }
      if ( debug ){
	cerr << "ANNO-SORT: UIT " << _anno_sort << endl;
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
      if ( adt->second.empty() ){
	_annotationdefaults.erase(adt);
      }
    }
  }

  multimap<AnnotationType, string> Document::unused_declarations( ) const {
    /// search for declarations not referenced in the Document
    /*!
      \return a list of all AnntotationType/setname pairs that are not used
     */
    multimap<AnnotationType,string> result;
    for ( const auto& [ant,ref] : _annotationrefs ){
      for ( const auto& [name,dec] : ref ){
	if ( dec == 0 ){
	  result.insert( make_pair(ant, name ) );
	}
      }
    }
    return result;
  }

  Text* Document::setTextRoot( const KWargs& args ) {
    /// create a Text element as root for the document
    /*!
      \param args extra attribute-value pairs as attributes to use
      \return the created Text node
    */
    Text *t = new Text( args );
    foliadoc->append( t );
    return t;
  }

  Text* Document::setTextRoot() {
    /// create a Text element as root for the document
    KWargs empty;
    return setTextRoot( empty );
  }

  Speech* Document::setSpeechRoot( const KWargs& args ) {
    /// create a Speech element as root for the document
    /*!
      \param args extra attribute-value pairs as attributes to use
      \return the created Speech node
    */
    Speech *s = new Speech( args );
    foliadoc->append( s );
    return s;
  }

  Speech* Document::setSpeechRoot() {
    /// create a Speech element as root for the document
    KWargs empty;
    return setSpeechRoot( empty );
  }

  FoliaElement *Document::getRoot(){
    /// return the root element, if any
    if ( foliadoc && foliadoc->size() > 0 ){
      return foliadoc->index(0);
    }
    else {
      return 0;
    }
  }

  FoliaElement* Document::append( FoliaElement *t ){
    /// append a root element tot the Document
    /*!
      \param t a root element to add
      \return the added root (also t). Throws on error.

      This function will check if a root is already there.
      Is only accepts Speech or Text nodes as root.
     */

    const FoliaElement *root = getRoot();
    if ( root ){
      throw DocumentError( _source_name,
			   "cannot append a root element to a Document. "
			   "Already there." );
    }
    if ( t->element_id() == Text_t
	 || t->element_id() == XmlComment_t
	 || t->element_id() == Speech_t ) {
      foliadoc->append( t );
      return t;
    }
    throw DocumentError( _source_name,
			 "Only can append 'text' or 'speech' as root of a "
			 "Document. (attempted=" + t->xmltag() );
  }

  void Document::incrRef( AnnotationType type,
			  const string& s ){
    /// increment the reference count for the AnnotationType/set combination
    /*!
      \param type the AnnotationType
      \param s the setname
    */
    if ( type != AnnotationType::NO_ANN ){
      string st = s;
      if ( st.empty() ){
	st = default_set(type);
      }
      ++_annotationrefs[type][st];
      // cerr << "increment " << toString(type) << "(" << st << ") to: "
      // 	   << _annotationrefs[type][s] << endl;
    }
  }

  void Document::decrRef( AnnotationType type,
			  const string& s ){
    /// decrement the reference count for the AnnotationType/set combination
    /*!
      \param type the AnnotationType
      \param s the setname
    */
    if ( type != AnnotationType::NO_ANN
	 && _annotationrefs[type][s] > 0 ){
      --_annotationrefs[type][s];
      // cerr << "decrement " << toString(type) << "(" << s << ") to: "
      // 	   << _annotationrefs[type][s] << endl;
    }
  }

  bool Document::declared( const AnnotationType& type,
			   const string& set_name ) const {
    /// check if a given combination of AnnotationType and setname
    /// is declared
    /*!
      \param type the AnnotationType
      \param set_name a setname OR an alias (may be empty)
      \return true when there is a match

      For the type NO_ANN, the result is always true.

      If set_name is empty ("") a match is found when a declarion for \e type
      exists

    */
    if ( debug ){
      cerr << "declared(" << folia::toString(type) << ",'"
	   << set_name << "')" << endl;
    }
    if ( type == AnnotationType::NO_ANN ){
      if ( debug ){
	cerr << "declared() always true for NO_ANN" << endl;
      }
      return true;
    }
    if ( debug ){
      cerr << "Doorzoek: " << _annotationdefaults << endl;
    }
    const auto& mit1 = _annotationdefaults.find(type);
    if ( mit1 != _annotationdefaults.end() ){
      if ( debug ){
	cerr << "found some: " << mit1->second << endl;
      }
      if ( set_name.empty() ){
	// 'wildcard' for setname
	if ( debug ){
	  cerr << "declared() for empty setname return TRUE" << endl;
	}
	return true;
      }
      // set_name may be an alias, so resolve
      string s_name = unalias(type,set_name);
      if ( debug ){
	cerr << "lookup: " << set_name << " (" << s_name << ")" << endl;
      }
      const auto& mit2 = mit1->second.find(s_name);
      if ( mit2 != mit1->second.end() ){
	if ( debug ){
	  cerr << "declared() return TRUE" << endl;
	}
	return true;
      }
      else {
	if ( debug ){
	  cerr << "return FALSE" << endl;
	}
	return false;
      }
    }
    if ( debug ){
      cerr << "return DIRECTLY FALSE" << endl;
    }
    return false;
  }

  bool Document::declared( ElementType et,
			   const string& set_name ) const {
    /// check if the AnnotationType belonging to the ElementType and setname
    /// is declared
    /*!
      \param et the ElementType
      \param set_name a setname OR an alias (may be empty)
      \return true when there is a match

      For the type NO_ANN, the result is always true.

      If set_name is empty ("") a match is found when a declarion for \em type
      exists
    */
    AnnotationType at = element_annotation_map[et];
    return declared( at, set_name );
  }

  string Document::default_set( AnnotationType type ) const {
    /// return the default setname for the type. If any.
    /*!
      \param type the AnnotationType
      \return the setname. May be empty ("") when there is none defined OR it
      is ambiguous.
    */
    if ( type == AnnotationType::NO_ANN ){
      return "";
    }
    // search a set. it must be unique. Otherwise return ""
    if ( debug ){
      cerr << "\nzoek voor '" << toString(type) << "' de default set in:\n"
	   <<  _annotationdefaults << endl;
    }
    string result;
    const auto& mit1 = _annotationdefaults.find(type);
    if ( mit1 != _annotationdefaults.end() ){
      // found a matching type
      if ( mit1->second.size() == 1 ){
	// so it is unique
	result = mit1->second.begin()->first;
      }
      else if ( debug ){
	cerr << "setname is not unique " << endl;
      }

    }
    if ( debug ){
      cerr << "default_set ==> " << result << endl;
    }
    return result;
  }

  string Document::default_annotator( AnnotationType type,
				      const string& setname ) const {
    /// return the default annotator for the type/setname combination.
    /*!
      \param type the AnnotationType
      \param setname the annotation set. An empty string ("") means ANY set.
      \return the annotator. May be empty ("") when there is none defined OR it
      is ambiguous.
    */
    if ( type == AnnotationType::NO_ANN ){
      return "";
    }
    string result;
    auto const cur = lookup_default( type, setname );
    if ( cur != 0 ){
      result = cur->_annotator;
    }
    //    cerr << "get default ==> " << result << endl;
    return result;
  }

  AnnotatorType Document::default_annotatortype( AnnotationType type,
						 const string& setname ) const {
    /// return the default annotator type for the type/setname combination.
    /*!
      \param type the AnnotationType
      \param setname the AnnotationType. An empty string ("") means ANY set.
      \return the annotator. May be empty ("") when there is none defined OR it
      is ambiguous.
    */
    if ( debug ){
      cerr << "annotationdefaults= " <<  _annotationdefaults << endl;
      cerr << "lookup: " << folia::toString(type) << endl;
    }
    AnnotatorType result = UNDEFINED;
    if ( type == AnnotationType::NO_ANN ){
      return result;
    }
    auto const cur = lookup_default( type, setname );
    if ( cur != 0 ){
      result = cur->_ann_type;
    }
    //  cerr << "get default ==> " << result << endl;
    return result;
  }

  string Document::default_datetime( AnnotationType type,
				     const string& setname ) const {
    /// return the default datetime value for the type/setname combination.
    /*!
      \param type the AnnotationType
      \param setname the annotation set.  An empty string ("") means ANY set.
      \return the datetime value. May be empty ("") when there is none defined
      OR it is ambiguous.
    */
    string result;
    const auto* cur = lookup_default( type, setname );
    if ( cur != 0 ){
      result = cur->_date;
    }
    //  cerr << "get default ==> " << result << endl;
    return result;
  }

  string Document::default_processor( AnnotationType type,
				      const string& setname ) const{
    /// return the default processor type for the type/setname combination.
    /*!
      \param type the AnnotationType
      \param setname the annotation set.  An empty string ("") means ANY set.
      \return the processor. May be empty ("") when there is none defined OR it
      is ambiguous.
    */
    if ( debug ){
      cerr << "defaultprocessor(" << toString( type ) << ","
	   << setname << ")" << endl;
    }
    string result;
    const auto cur = lookup_default( type, setname );
    if ( cur != 0 ){
      if ( cur->_processors.size() == 1 ){
	result = *cur->_processors.begin();
      }
      else if ( cur->_processors.size() > 1 ){
	auto const& as = annotationtype_xml_map.find(type);
	if ( as != annotationtype_xml_map.end() ){
	  throw NoDefaultError("No processor specified for <"
			       + as->second
			       +  ">, but the presence of multiple declarations prevent assigning a default");
	}
      }
    }
    return result;
  }

  string Document::original_default_set( AnnotationType type ) const {
    /// return the default setname for the type in the ORIGINAL definitions.
    /*!
      \param type the AnnotationType
      \return the setname. May be empty ("") when there is none defined OR it
      is ambiguous.

      In case of \e incremental Document building, we are allowed to add
      annotation declarations at any moment. That might render the default_set
      of an AnnotationType undefined. With this function, we still are able to
      find the original value and use that e.g. on output.
    */
    auto const& it = _orig_ann_default_sets.find(type);
    if ( it == _orig_ann_default_sets.end() ){
      return "";
    }
    else {
      return it->second;
    }
  }

  string Document::original_default_processor( AnnotationType type ) const {
    /// return the default processor name for the type in the ORIGINAL definitions.
    /*!
      \param type the AnnotationType
      \return the processor name. May be empty ("") when there is none defined
      OR it is ambiguous.

      In case of \e incremental Document building, we are allowed to add
      annotation declarations at any moment. That might render the default
      processor of an AnnotationType undefined. With this function, we still
      are able to find the original value and use that e.g. on output.
    */
    auto const& it = _orig_ann_default_procs.find(type);
    if ( it == _orig_ann_default_procs.end() ){
      return "";
    }
    else {
      return it->second;
    }
  }

  vector<string> Document::get_annotators( AnnotationType type,
					   const string& setname ) const {
    /// return all the annotators for the type/setname combination.
    /*!
      \param type the AnnotationType
      \param setname the annotation set. An empty string ("") means ANY set.
      \return a list of annotators.
    */
    auto const cur = lookup_default( type, setname );
    vector<string> result;
    std::copy( cur->_processors.cbegin(),
	       cur->_processors.cend(),
	       std::back_inserter( result ) );
    return result;

  }

  vector<const processor*> Document::get_processors( AnnotationType type,
						     const string& setname ) const {
    /// return all the processors for the type/setname combination.
    /*!
      \param type the AnnotationType
      \param setname the annotation set. An empty string ("") means ANY set.
      \return a list of processors.
    */
    vector<const processor*> result;
    if ( debug ){
      cerr << "getprocessors(" << toString( type ) << ","
	   << setname << ")" << endl;
    }
    auto const cur = lookup_default( type, setname );
    if ( cur != 0 ){
      transform( cur->_processors.begin(),
		 cur->_processors.end(),
		 back_inserter(result),
		 [&]( const string& p ){ return get_processor(p); } );
    }
    return result;
  }

  void Document::add_one_anno( const pair<AnnotationType,string>& pair,
			       xmlNode *root ) const {
    /// create an annotation declaration entry under the xmlNode node
    /*!
      \param pair an AnnotationType/setname pair
      \param root the node we want to add to
    */
    //    cerr << "add one anno: " << pair << endl;
    AnnotationType type = pair.first;
    string sett = pair.second;
    if ( type == AnnotationType::TEXT ){
      //
      // Temporary limitation. To be resolved
      //
      test_temporary_text_exception( sett );
    }

    string label = annotation_type_to_string( type );
    // cerr << "/nDO: '" << label+sett << "'" << endl;
    // if ( done.find(label+sett) != done.end() ){
    //   return;
    // }
    // //    done.insert(label+sett);
    label += "-annotation";
    const auto *it = lookup_default( type, sett );
    if ( it != 0 ){
      string s;
      KWargs args;
      if ( !strip() ){
	s = it->_date;
	if ( !s.empty() ){
	  args["datetime"] = s;
	}
      }
      s = it->_format;
      if ( !s.empty() ){
	args["format"] = s;
      }
      s = sett;
      if ( s == "None" ){ // "empty" set
	// skip
      }
      else if ( s != "undefined" ){ // the default
	args["set"] = s;
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
	const auto& al = ti->second.find(s);
	if ( al->second != s ){
	  args["alias"] = al->second;
	}
      }
      string an = it->_annotator;
      if ( !an.empty() ){
	args["annotator"] = an;
      }
      AnnotatorType anno_type = it->_ann_type;
      if ( anno_type != UNDEFINED && anno_type != AUTO ){
	args["annotatortype"] = toString(anno_type);
      }
      xmlNode *annotation_node = TiCC::XmlNewNode( foliaNs(), label );
      addAttributes( annotation_node, args );
      xmlAddChild( root, annotation_node );
      for ( const auto& p : it->_processors ){
	KWargs pargs;
	xmlNode *a = TiCC::XmlNewNode( foliaNs(), "annotator" );
	pargs["processor"] = p;
	addAttributes( a, pargs );
	xmlAddChild( annotation_node, a );
      }
    }
  }

  void Document::add_annotations( xmlNode *metadata ) const {
    /// create an annotations block under the xmlNode metadata
    /*!
      \param metadata the parent to add to
      calls add_one_anno() for every annotation declaration.
    */
    if ( debug ){
      cerr << "start add_annotations: " << _annotationdefaults << endl;
      cerr << "sorting: " << _anno_sort << endl;
    }
    xmlNode *node = xmlAddChild( metadata,
				 TiCC::XmlNewNode( foliaNs(),
						   "annotations" ) );
    if ( canonical() ){
      // _anno_sort contains type:setname pair ordered on appearance
      multimap<AnnotationType,
	       pair<AnnotationType,string>> ordered;
      for ( const auto& pair : _anno_sort ){
	// this sorts the _anno_sort entries on AnnotationType
	ordered.insert(make_pair(pair.first,pair));
      }
      // so now we can append annotations in a canonical way
      for ( const auto& [dummy,anno] : ordered ){
	add_one_anno( anno, node );
      }
    }
    else {
      // output in order off appearance
      for ( const auto& pair : _anno_sort ){
	add_one_anno( pair, node );
      }
    }
  }

  void Document::append_processor( xmlNode *node, const processor *p ) const {
    /// add a processor xml structure to the parent 'node'
    /*!
      \param node the xml node to add to
      \param p the processor of which to add te info
    */
    xmlNode *pr = xmlAddChild( node, TiCC::XmlNewNode( foliaNs(), "processor" ) );
    KWargs atts;
    atts["xml:id"] = p->_id;
    atts["name"] = p->_name;
    if ( p->_type != AUTO || has_explicit() ){
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
    if ( !p->_src.empty() ){
      atts["src"] = p->_src;
    }
    if ( !p->_format.empty() ){
      atts["format"] = p->_format;
    }
    addAttributes( pr, atts );
    for ( const auto& [meta_id,val] : p->_metadata ){
      xmlNode *m = xmlAddChild( pr, TiCC::XmlNewNode( foliaNs(), "meta" ) );
      KWargs args;
      args["id"] = meta_id;
      addAttributes( m, args );
      xmlAddChild( m, xmlNewText( to_xmlChar(val) ) );
    }
    for ( const auto* s : p->_processors ){
      append_processor( pr, s );
    }
  }

  void Document::add_provenance( xmlNode *metadata ) const {
    /// create a provenance block under the xmlNode metadata
    /*!
      \param metadata the parent to add to
      calls append_processor() for every processor available
    */
    if ( !_provenance ){
      return;
    }
    xmlNode *node = xmlAddChild( metadata,
				 TiCC::XmlNewNode( foliaNs(),
						   "provenance" ) );
    for ( const auto* p : _provenance->processors ){
      append_processor( node, p );
    }
  }

  void Document::add_submetadata( xmlNode *node ) const {
    /// add a submetadata block to node
    for ( const auto& [id,vals] : submetadata ){
      xmlNode *sm = TiCC::XmlNewNode( foliaNs(), "submetadata" );
      KWargs atts;
      atts["xml:id"] = id;
      addAttributes( sm, atts );
      const MetaData *md = submetadata.find(id)->second;
      string type = md->type();
      atts.clear();
      atts["type"] = type;
      addAttributes( sm, atts );
      xmlAddChild( node, sm );
      if ( type == "native" ){
	atts = vals->get_avs();
	// cerr << "atts: " << atts << endl;
	for ( const auto& [m_id,val] : atts ){
	  xmlNode *m = TiCC::XmlNewNode( foliaNs(), "meta" );
	  KWargs args;
	  args["id"] = m_id;
	  addAttributes( m, args );
	  xmlAddChild( m, xmlNewText( to_xmlChar(val) ) );
	  xmlAddChild( sm, m );
	}
      }
      else if ( md->datatype() == "ExternalMetaData" ){
	KWargs args;
	args["src"] = md->src();
	addAttributes( sm, args );
      }
      else if ( md->datatype() == "ForeignMetaData" ){
	for ( const auto* foreign : md->get_foreigners() ) {
	  xmlNode *f = foreign->xml( true, false );
	  xmlAddChild( sm, f );
	}
      }
    }
  }

  void Document::add_metadata( xmlNode *node ) const{
    /// add a metadata block to node
    if ( _metadata ){
      if ( _metadata->datatype() == "ExternalMetaData" ){
	KWargs atts;
	atts["type"] = "external";
	string src = _metadata->src();
	if ( !src.empty() ){
	  atts["src"] = src;
	}
	addAttributes( node, atts );
      }
      else {
	KWargs atts;
	atts["type"] = _metadata->type();
	addAttributes( node, atts );
	for ( const auto& [id,val] : _metadata->get_avs() ){
	  xmlNode *m = TiCC::XmlNewNode( foliaNs(), "meta" );
	  xmlAddChild( m, xmlNewText( to_xmlChar(val) ) );
	  KWargs meta_atts;
	  meta_atts["id"] = id;
	  addAttributes( m, meta_atts );
	  xmlAddChild( node, m );
	}
      }
    }
    if ( _foreign_metadata ){
      if ( !_metadata ){
	KWargs atts;
	atts["type"] = _foreign_metadata->type();
	addAttributes( node, atts );
      }
      for ( const auto* foreign : _foreign_metadata->get_foreigners() ) {
	xmlNode *f = foreign->xml( true, false );
	xmlAddChild( node, f );
      }
    }
    if ( !_metadata
	 && !_foreign_metadata ){
      KWargs atts;
      atts["type"] = "native";
      addAttributes( node, atts );
    }
    add_submetadata( node );
  }

  void Document::add_styles( xmlDoc* doc ) const {
    /// add a styles block to the output document
    /*!
      \param doc the output document
    */
    for ( const auto& [type,ref] : styles ){
      string content = "type=\"" + type + "\" href=\"" + ref + "\"";
      xmlAddChild( reinterpret_cast<xmlNode*>(doc),
		   xmlNewDocPI( doc,
				to_xmlChar("xml-stylesheet"),
				to_xmlChar( content ) ) );
    }
  }

  xmlDoc *Document::to_xmlDoc( const string& ns_label ) const {
    /// convert the Document to an xmlDoc
    /*!
      \param ns_label a namespace label to use. (default "")
    */
    xmlDoc *outDoc = xmlNewDoc( to_xmlChar("1.0") );
    add_styles( outDoc );
    for ( const auto* pr: preludes ){
      xmlAddChild( reinterpret_cast<xmlNode*>(outDoc),
		   pr->xml( true, canonical() ) );
    }
    xmlNode *root = xmlNewDocNode( outDoc,
				   0,
				   to_xmlChar("FoLiA"),
				   0 );
    xmlDocSetRootElement( outDoc, root );
    xmlNs *xl = xmlNewNs( root,
			  to_xmlChar("http://www.w3.org/1999/xlink"),
			  to_xmlChar("xlink") );
    xmlSetNs( root, xl );
    if ( _foliaNsIn_href == 0 ){
      if ( ns_label.empty() ){
	_foliaNsOut = xmlNewNs( root,
				to_xmlChar(NSFOLIA),
				0 );
      }
      else {
	_foliaNsOut = xmlNewNs( root,
				to_xmlChar(NSFOLIA),
				to_xmlChar(ns_label) );
      }
    }
    else {
      _foliaNsOut = xmlNewNs( root,
			      _foliaNsIn_href,
			      _foliaNsIn_prefix );
    }
    xmlSetNs( root, _foliaNsOut );
    KWargs attribs;
    attribs["xml:id"] = foliadoc->id();
    if ( !strip ){
      attribs["generator"] = "libfolia-v" + library_version();
      attribs["version"] = _version_string;
      // attribs["version"] = folia_version();
    }
    if ( has_explicit() ){
      attribs["form"] = "explicit";
    }
    if ( _external_document ){
      attribs["external"] = "yes";
    }
    addAttributes( root, attribs );
    xmlNode *md = xmlAddChild( root, TiCC::XmlNewNode( foliaNs(), "metadata" ) );
    add_annotations( md );
    add_provenance( md );
    add_metadata( md );
    for ( size_t i=0; i < foliadoc->size(); ++i ){
      const FoliaElement* el = foliadoc->index(i);
      xmlAddChild( root, el->xml( true, canonical() ) );
    }
    return outDoc;
  }

  string Document::toXml( const string& ns_label ) const {
    /// dump the Document to a string
    /*!
      \param ns_label a namespace label to use. (default "")
    */
    string result;
    if ( foliadoc ){
      xmlDoc *outDoc = to_xmlDoc( ns_label );
      xmlChar *buf; int size;
      xmlDocDumpFormatMemoryEnc( outDoc, &buf, &size,
				 output_encoding, 1 );
      result = to_string( buf, size );
      xmlFree( buf );
      xmlFreeDoc( outDoc );
      _foliaNsOut = 0;
    }
    else {
      throw runtime_error( "can't save, no doc" );
    }
    return result;
  }

  bool Document::toXml( const string& file_name,
			const string& ns_label ) const {
    /// write the Document to a file
    /*!
      \param file_name the name of the file to create
      \param ns_label a namespace label to use. (default "")
      \return false on error, true otherwise
      automaticly detects .gz and .bz2 filenames and will handle accordingly
    */
    if ( foliadoc ){
      long int res = 0;
      if ( TiCC::match_back( file_name, ".bz2" ) ){
	string tmpname = file_name.substr( 0, file_name.length() - 3 ) + "tmp";
	if ( toXml( tmpname, ns_label ) ){
	  bool stat = TiCC::bz2Compress( tmpname, file_name );
	  remove( tmpname.c_str() );
	  if ( !stat ){
	    res = -1;
	  }
	}
      }
      else {
	xmlDoc *outDoc = to_xmlDoc( ns_label );
	if ( TiCC::match_back( file_name, ".gz" ) ){
	  xmlSetDocCompressMode(outDoc,9);
	}
	res = xmlSaveFormatFileEnc( file_name.c_str(),
				    outDoc,
				    output_encoding, 1 );
	xmlFreeDoc( outDoc );
	_foliaNsOut = 0;
      }
      if ( res == -1 ){
	return false;
      }
    }
    else {
      return false;
    }
    return true;
  }

  Pattern::Pattern( const vector<string>& pat_vec,
		    const ElementType et,
		    const string& args ): matchannotation(et) {
    /// create a Pattern structure for searching
    /*!
      \param pat_vec a list of search terms (may be regular expressions)
      \param et The kind of elements to match on
      \param args additionale search options as attribute/value pairs
    */
    regexp = false;
    case_sensitive = false;
    KWargs kw = getArgs( args );
    matchannotationset = kw["matchannotationset"];
    if (kw["regexp"] != "" ){
      regexp = TiCC::stringTo<bool>( kw["regexp"] );
    }
    if (kw["maxgapsize"] != "" ){
      maxgapsize = TiCC::stringTo<int>( kw["maxgapsize"] );
    }
    else {
      maxgapsize = 10;
    }
    if ( kw["casesensitive"] != "" ){
      case_sensitive = TiCC::stringTo<bool>( kw["casesensitive"] );
    }
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

  Pattern::Pattern( const vector<string>& pat_vec,
		    const string& args ) : matchannotation(BASE) {
    /// create a Pattern structure for searching
    /*!
      \param pat_vec a list if search terms (may be regular expressions)
      \param args additionale search options as attribute/value pairs
    */
    regexp = false;
    case_sensitive = false;
    KWargs kw = getArgs( args );
    matchannotationset = kw["matchannotationset"];
    if (kw["regexp"] != "" ){
      regexp = TiCC::stringTo<bool>( kw["regexp"] );
    }
    if (kw["maxgapsize"] != "" ){
      maxgapsize = TiCC::stringTo<int>( kw["maxgapsize"] );
    }
    else {
      maxgapsize = 10;
    }
    if ( kw["casesensitive"] != "" ){
      case_sensitive = TiCC::stringTo<bool>( kw["casesensitive"] );
    }
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
    /// destroy a Pattern
    for ( const auto& m : matchers ){
      delete m;
    }
  }

  inline ostream& operator<<( ostream& os, const Pattern& p ){
    /// debugging only: output the sequence part of a Pattern
    using TiCC::operator <<;
    os << "pattern: " << p.sequence;
    return os;
  }

  bool Pattern::match( const UnicodeString& us,
		       size_t& pos,
		       int& gap,
		       bool& done,
		       bool& flag ) const {
    /// try to match the input string to this pattern
    /*!
      \param us A UnicodeString to match
      \param pos the position of the (regex) matcher to try
      \param gap
      \param done
      \param flag
      \return true on a succesful match
    */
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
      if ( !case_sensitive ){
	s.toLower();
      }
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
	else if ( ++gap == maxgapsize ){
	  ++pos;
	}
	else {
	  flag = true;
	}
	return true;
      }
      else {
	++pos;
	return false;
      }
    }
  }

  bool Pattern::variablesize() const {
    /// look if at least one sequence in the Pattern is "*"
    return any_of( sequence.begin(),
		   sequence.end(),
		   []( const UnicodeString& s ) { return s == "*"; } );
  }

  void Pattern::unsetwild() {
    /// replace all sequence in the Pattern with value "*" by "*:1"
    replace_if( sequence.begin(),
		sequence.end(),
		[]( const UnicodeString& s ) { return s == "*"; },
		"*:1"
		);
  }

  set<int> Pattern::variablewildcards() const {
    /// build an index of all "*" sequences
    set<int> result;
    for ( size_t i=0; i < sequence.size(); ++i ){
      if ( sequence[i] == "*" ){
	result.insert( i );
      }
    }
    return result;
  }

  vector<vector<Word*> > Document::findwords( const Pattern& pat,
					      const string& args ) const {
    /// search the Document for vector of Word list matching the Pattern
    /*!
      \param pat The search Pattern
      \param args additional search options as attribute/value pairs
      \return a vector of Word list that matched. (if any)
      supported additional arguments can be 'leftcontext' and 'rightcontext'
    */
    size_t leftcontext = 0;
    size_t rightcontext = 0;
    KWargs kw = getArgs( args );
    string val = kw["leftcontext"];
    if ( !val.empty() ){
      leftcontext = TiCC::stringTo<size_t>(val);
    }
    val = kw["rightcontext"];
    if ( !val.empty() ){
      rightcontext = TiCC::stringTo<size_t>(val);
    }
    vector<vector<Word*> > result;
    vector<Word*> matched;
    if ( pat.regexp ){
      throw runtime_error( "regexp not supported yet in patterns" );
    }
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
	if ( pat.matchannotation == BASE ){
	  value = mywords[i]->text();
	}
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
	  if ( cursor == 0 ){
	    startpos = i; // restart search here
	  }
	  if ( done ){
	    vector<Word*> keep = matched;
	    //	  cerr << "findnodes() tussenresultaat ==> " << matched << endl;
	    vector<Word*> left_v;
	    if ( leftcontext > 0 ){
	      left_v = matched[0]->leftcontext(leftcontext);
	      //	    cerr << "findnodes() left ==> " << left_v << endl;
	      copy( matched.begin(), matched.end(), back_inserter(left_v) );
	      //	    cerr << "findnodes() after copy left_v ==> " << left_v << endl;
	    }
	    else {
	      left_v = matched;
	    }
	    if ( rightcontext > 0 ){
	      vector<Word*> right_v = matched.back()->rightcontext(rightcontext);
	      //	    cerr << "findnodes() right_v ==> " << right_v << endl;
	      copy( right_v.begin(), right_v.end(), back_inserter(left_v) );
	      //	    cerr << "findnodes() right_v na copy ==> " << right_v << endl;
	    }
	    //	  cerr << "findnodes() tussenresultaat 2 ==> " << left_v << endl;
	    result.push_back(left_v);
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
    /// search the Document for vector of Word list matching one of the Pattern
    /*!
      \param pats a list of search Patterns
      \param args additional search options as attribute/value pairs
      \return a vector of Word list that matched. (if any)
      supported additional arguments can be 'leftcontext' and 'rightcontext'
    */
    size_t prevsize = 0;
    bool start = true;
    bool unsetwildcards = false;
    set<int> variablewildcards;
    int ind = 0;
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
	if ( ind > 0 && variablewildcards.empty() ){
	  unsetwildcards = true;
	}
	else {
	  if ( !variablewildcards.empty() &&
	       variablewildcards != it.variablewildcards() ){
	    throw runtime_error("If multiple patterns are provided with variable wildcards, then these wildcards must all be in the same positions!");
	  }
	  variablewildcards = it.variablewildcards();
	}
      }
      else if ( !variablewildcards.empty() ){
	unsetwildcards = true;
      }
      ++ind;
    }
    if ( unsetwildcards ){
      for ( auto& it : pats ){
	it.unsetwild();
      }
    }
    vector<vector<Word*> > result;
    for ( const auto& it : pats ){
      vector<vector<Word*> > res = findwords( it, args );
      if ( result.empty() ){
	result = res;
      }
      else if ( res != result ){
	result.clear();
	break;
      }
    }
    return result;
  }

} // namespace folia
