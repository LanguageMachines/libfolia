/*
  Copyright (c) 2006 - 2021
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
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
#include <cstdio>
#include <string>
#include <stack>
#include <stdexcept>
#include <algorithm>
#include "ticcutils/PrettyPrint.h"
#include "ticcutils/FileUtils.h"
#include "ticcutils/XMLtools.h"
#include "ticcutils/zipper.h"
#include "libfolia/folia.h"

using namespace std;

/// define a static default LogStream
TiCC::LogStream DBG_CERR(cerr,"folia-engine:");

/// direct Debugging info to the internal file, if present, or to the default stream
#define DBG *TiCC::Log((_dbg_file?_dbg_file:&DBG_CERR))

namespace folia {

  using TiCC::operator<<;

  xml_tree::xml_tree( int d,
		      int i,
		      const std::string& t,
		      const std::string& c ):
    /// create an xml_tree element with the given parameters
    depth(d),
    index(i),
    tag(t),
    textclass(c),
    parent(0),
    link(0),
    next(0)
  {}

  xml_tree::~xml_tree() {
    /// delete an xml_tree
    if ( link ){
      delete link;
    }
    if ( next ){
      delete next;
    }
  }

  void print( ostream& os, const xml_tree* tree ){
    //! pretty print an xml_tree
    /*!
      \param os the output stream
      \param tree the tree
    */
    const xml_tree *rec_pnt = tree;
    while ( rec_pnt ){
      os << setw(10) << rec_pnt->index << string( rec_pnt->depth, ' ' )
	 << rec_pnt->tag;
      if ( rec_pnt->textclass.empty() ){
	os << endl;
      }
      else {
	os << " (" << rec_pnt->textclass << ")" << endl;
      }
      print( os, rec_pnt->link );
      rec_pnt = rec_pnt->next;
    }
  }

  ostream& operator<<( ostream& os, const xml_tree* tree ){
    /// print an xml_tree
    os << endl;
    print( os, tree );
    return os;
  }

  Engine::Engine():
    /// default constructor
    _reader(0),
    _out_doc(0),
    _root_node(0),
    _external_node(0),
    _current_node(0),
    _last_added(0),
    _last_depth(2),
    _doc_type( TEXT ),
    _dbg_file(0),
    _os(0),
    _ok(false),
    _done(false),
    _header_done(false),
    _finished(false),
    _debug(false)
  {
  }

  Engine::~Engine(){
    /// destructor
    xmlFreeTextReader( _reader );
    delete _out_doc;
    delete _os;
  }

  Document *Engine::doc( bool disconnect ){
    /// returns the associated FoLiA document.
    /*!
      \param disconnect When true, handle control over to the caller.
      The caller has to delete it then to avoid memory leaks

    */
    Document *result = _out_doc;
    if ( disconnect ){
      _out_doc = 0;
    }
    return result;
  }

  bool Engine::set_debug( bool d ) {
    /// switch debugging on/off depending on parameter 'd'
    /*!
      \param d when true switch debugging to ON, otherwise OFF

      When debugging is switched ON and NO debug file is associated yet,
      it is created.
    */
    bool res = _debug;
    if ( d ){
      if ( !_dbg_file ){
	_dbg_file
	  = new TiCC::LogStream( cerr, "folia-engine", StampMessage );
      }
    }
    _debug = d;
    return res;
  }

  void Engine::set_dbg_stream( TiCC::LogStream *ls ){
    /// switch debugging to another LogStream
    if ( _dbg_file ){
      delete _dbg_file;
    }
    _dbg_file = ls;
  }

  void Engine::un_declare( const AnnotationType& at,
			   const string& setname ){
    /// remove the annotation declaration for the given type and set
    /*!
      \param at the AnnotationType
      \param setname the set so remove

      \note an AnntotationType can have several set-names assigned to it.
      When setname is empty ("") ALL set-names are removed
    */
    if ( !ok() ){
      throw logic_error( "declare() called on invalid engine!" );
    }
    else if ( _header_done ){
      throw logic_error( "declare() called on already (partially) saved document!" );
    }
    else {
      _out_doc->un_declare( at, setname );
    }
  }

  void Engine::declare( const AnnotationType& at,
			const string& setname,
			const string& args ) {
    /// declare a set for a given annotation type
    /*!
      \param at the AnnotationType
      \param setname The set-name to use
      \param args additional arguments in string annotation. Can be used to add
      extra arguments like a processor name or an annotator
    */
    KWargs kwargs( args );
    declare( at, setname, kwargs );
  }

  void Engine::declare( const AnnotationType& at,
			const string& setname,
			const KWargs& args ) {
    /// declare a set for a given annotation type
    /*!
      \param at the AnnotationType
      \param setname The set-name to use
      \param args additional arguments as a KWargs attribute-value list.
      can be used to add extra arguments like a processor name or
      an annotator
    */
    if ( !ok() ){
      throw logic_error( "declare() called on invalid engine!" );
    }
    else if ( _header_done ){
      throw logic_error( "declare() called on already (partially) saved document!" );
    }
    else {
      _out_doc->declare( at, setname, args );
    }
  }

  bool Engine::is_declared( const AnnotationType& at,
			    const string& setname ) const {
    /// check if an annotation for the provided type and setname is present
    /*!
      \param at the AnnotationType
      \param setname the set-name to test
      \return true if declared, false otherwise.
    */
    if ( !ok() ){
      throw logic_error( "is_declared() called on invalid engine!" );
    }
    else {
      return _out_doc->declared( at, setname );
    }
  }

  bool Engine::is_declared( const AnnotationType& at,
			    const string& setname,
			    const string& annotator,
			    const AnnotatorType& annotator_type,
			    const string& processor ) const {
    /// check if an annotation for the provided type and setname is present
    /*!
      \param at the AnnotationType
      \param setname the set-name to test
      \param annotator the name of the annotator to test
      \param annotator_type the AnnotatorType to test
      \param processor the desired processor
      \return true if declared, false otherwise.
    */
    if ( !ok() ){
      throw logic_error( "is_declared() called on invalid engine!" );
    }
    else {
      return _out_doc->declared( at, setname, annotator, annotator_type, processor );
    }
  }

  bool Engine::is_declared( const AnnotationType& at,
			    const string& setname,
			    const string& annotator,
			    const string& annotator_type,
			    const string& processor ) const {
    /// check if an annotation for the provided type and setname is present
    /*!
      \param at the AnnotationType
      \param setname the set-name to test
      \param annotator the name of the annotator to test
      \param annotator_type the AnnotatorType to test, encoded as a string
      \param processor the desired processor
      \return true if declared, false otherwise.
    */
    AnnotatorType ant = UNDEFINED;
    try {
      ant = TiCC::stringTo<AnnotatorType>(annotator_type);
    }
    catch (...){
      throw logic_error( annotator_type + " is NOT a valid annotator type" );
    }
    return is_declared( at, setname, annotator, ant, processor );
  }

  void Engine::set_metadata( const std::string& att,
			     const std::string& val){
    /// set a metadata value in the associated document
    /*!
      \param att the attribute to set
      \param val the value of the attribute
    */
    if ( !ok() ){
      throw logic_error( "set_metadata() called on invalid engine!" );
    }
    else {
      return _out_doc->set_metadata( att, val );
    }
  }

  pair<string,string> extract_style( const string& value ){
    /// parse a string to extract an xml style-sheet value
    /*!
      \param value the line to parse
      \return a pait of strings containing the type and the href values
    */
    string type;
    string href;
    vector<string> v = TiCC::split( value );
    if ( v.size() == 2 ){
      vector<string> w = TiCC::split_at( v[0], "=" );
      if ( w.size() == 2 && w[0] == "type" ){
	type = w[1].substr(1,w[1].length()-2);
      }
      w = TiCC::split_at( v[1], "=" );
      if ( w.size() == 2 && w[0] == "href" ){
	href = w[1].substr(1,w[1].length()-2);
      }
      return make_pair(type,href);
    }
    else {
      throw XmlError( "couldn't parse xml-style-sheet line: " + value );
    }
  }

  KWargs get_attributes( xmlTextReader *tr ){
    /// extract a KWargs attribute/value list from the TextReader location
    /*!
      \param tr the xmlTextReader pointer
      \return a KWargs list of all attribute/value pairs found
    */
    KWargs result;
    if ( xmlTextReaderHasAttributes(tr) ){
      xmlTextReaderMoveToFirstAttribute(tr);
      do {
	string att = (const char*)xmlTextReaderConstName(tr);
	string val = (const char*)xmlTextReaderConstValue(tr);
	result[att] = val;
      }
      while ( xmlTextReaderMoveToNextAttribute(tr) );
    }
    return result;
  }

  xmlTextReader *create_text_reader( const string& buf ){
    /// create a new xmlTextRead on a buffer
    /*!
      \param buf the input buffer.
      The buffer may contain a complete (FoLiA-) XML document as a string
      OR a filename denoting such a document, which may be .bz2 and .gz
      encoded
    */
    if ( TiCC::match_front( buf, "<?xml " ) ){
      return xmlReaderForMemory( buf.c_str(), buf.size(),
				 "input_buffer", 0, XML_PARSER_OPTIONS );
    }
    else if ( TiCC::match_back( buf, ".bz2" ) ){
      string buffer = TiCC::bz2ReadFile( buf );
      if ( buffer.empty() ){
	throw runtime_error( "folia::Engine(), empty file? (" + buf
			      + ")" );
      }
      //
      // next step fails for unclear reasons
      // so we use an intermediate file. Which works, but is clumsy
      //
      // return xmlReaderForMemory( buffer.c_str(), buffer.size()+1,
      //  				 buf.c_str(), 0, XML_PARSER_OPTIONS );
      TiCC::tmp_stream ts( "folia" );
      string tmp_file = ts.tmp_name();
      ofstream& os = ts.os();
      os << buffer << endl;
      ts.close();
      xmlTextReader *result
	= xmlReaderForFile( tmp_file.c_str(), 0, XML_PARSER_OPTIONS );
      return result;
    }
    // libxml2 can handle .xml and .xml.gz
    return xmlReaderForFile( buf.c_str(), 0, XML_PARSER_OPTIONS );
  }

  void Engine::add_text( int depth ){
    /// when parsing, add a new XmlText node
    /*!
      \param depth the depth (location) in the tree where to add
    */
    string value = (const char*)xmlTextReaderConstValue(_reader);
    string trimmed = TiCC::trim(value);
    if ( !trimmed.empty() ){
      throw XmlError( "spurious text " + trimmed + " found." );
    }
    if ( _debug ){
      DBG << "add_text(" << value << ") depth=" << depth << endl;
    }
    XmlText *txt = new XmlText();
    txt->setvalue( value );
    append_node( txt, depth );
  }

  void Engine::add_comment( int depth ){
    /// when parsing, add a new _XmlComment node
    /*!
      \param depth the depth (location) in the tree where to add
    */
    if ( _debug ){
      DBG << "add_comment " << endl;
    }
    string tag = "_XmlComment";
    FoliaElement *t = AbstractElement::createElement( tag, _out_doc );
    append_node( t, depth );
  }

  void Engine::add_default_node( int depth ){
    /// when debugging, output a message. Does nothing else
    if ( _debug ){
      string local_name = (const char*)xmlTextReaderConstLocalName(_reader);
      int type = xmlTextReaderNodeType(_reader);
      DBG << "add_node " << type <<  " name=" << local_name
	  << " depth " << _last_depth << " ==> " << depth << endl;
    }
  }

  void check_empty( xmlNode *node ){
    /// assure that node == 0 OR just contains whitespace or comment
    /*!
      \param node the node to check
      will throw when node is anything other than xml-comment or whitespace
    */
    if ( node ){
      if ( node->type == XML_COMMENT_NODE ){
	check_empty( node->next );
      }
      else if ( node->type == XML_TEXT_NODE ){
	string txt = TextValue(node);
	txt = TiCC::trim(txt);
	if ( !txt.empty() ){
	  string tg = "<" + TiCC::Name(node->prev) + ">";
	  throw XmlError( "found extra text '" + txt + "' after element "
			  + tg + ", NOT allowed there." );
	}
      }
      else {
	string tg = "<" + TiCC::Name(node->prev) + ">";
	throw XmlError( "found unexpected node '" + TiCC::Name(node)
			+ "' after element " + tg + ", NOT allowed there." );
      }
    }
  }

  bool Engine::init_doc( const string& file_name,
			 const string& out_name ){
    /// init an associated document for this Engine
    /*!
      \param file_name the input file to use for parsing
      \param out_name when not empty, add an output-file with this name

      Initializing includes parsing the Document's metadata, style-sheet
      upto and including the top \<text or \<speech> node
    */
    _ok = false;
    _out_doc = new Document();
    _out_doc->set_incremental( true );
    if ( !out_name.empty() ){
      _os = new ofstream( out_name );
      _out_name = out_name;
    }
    _out_doc->_source_filename = file_name;
    _reader = create_text_reader( file_name );
    if ( _reader == 0 ){
      _ok = false;
      throw( runtime_error( "folia::Engine(), init failed on '" + file_name
			    + "' (File not found)" ) );
    }
    int ret = xmlTextReaderRead(_reader);
    int index = 0;
    while ( ret > 0 ){
      int type =  xmlTextReaderNodeType(_reader );
      string local_name = (const char*)xmlTextReaderConstLocalName(_reader );
      switch ( type ){
      case XML_READER_TYPE_ELEMENT:
	++index;
	if ( local_name == "FoLiA" ){
	  // found the root
	  const xmlChar *pnt = xmlTextReaderConstPrefix(_reader);
	  if ( pnt ){
	    _out_doc->_foliaNsIn_prefix = xmlStrdup(pnt );
	    ns_prefix = (const char*)pnt;
	  }
	  pnt = xmlTextReaderConstNamespaceUri(_reader);
	  if ( pnt ){
	    _out_doc->_foliaNsIn_href = xmlStrdup(pnt);
	    string ns = (const char*)_out_doc->_foliaNsIn_href;
	    if ( ns != NSFOLIA ){
	      _ok = false;
	      throw XmlError( "Folia Document should have namespace declaration "
			      + NSFOLIA + " but found: " + ns );
	    }
	  }
	  KWargs in_args = get_attributes( _reader );
	  string id;
	  if ( !in_args.empty() ){
	    id = in_args["xml:id"];
	  }
	  for ( auto it =in_args.begin(); it != in_args.end();  ){
	    // remove all xmlns attributes
	    if ( it->first.find( "xmlns" ) == 0 ){
	      it = in_args.erase( it );
	    }
	    else {
	      ++it;
	    }
	  }
	  if ( !id.empty() ){
	    FoliaElement *root = new FoLiA( in_args, _out_doc );
	    _out_doc->foliadoc = root;
	  }
	  else {
	    _ok = false;
	    throw XmlError( "Engine: invalid FoLiA. missing ID" );
	  }
	}
	else if ( local_name == "metadata" ) {
	  xmlNode *node = xmlTextReaderExpand(_reader);
	  check_empty( node->next );
	  _out_doc->parse_metadata( node );
	}
	else if ( local_name == "text" ){
	  _doc_type = TEXT;
	  KWargs args = get_attributes(_reader);
	  FoliaElement *text =_out_doc->setTextRoot( args );
	  _root_node = text;
	  _current_node = text;
	  _ok = true;
	  _start_index = index;
	  _out_doc->save_orig_ann_defaults();
	  return _ok;
	}
	else if ( local_name == "speech" ){
	  _doc_type = SPEECH;
	  KWargs args = get_attributes(_reader);
	  FoliaElement *sp = _out_doc->setSpeechRoot( args );
	  _root_node = sp;
	  _current_node = sp;
	  _ok = true;
	  _start_index = index;
	  _out_doc->save_orig_ann_defaults();
	  return _ok;
	}
	break;
      case XML_READER_TYPE_PROCESSING_INSTRUCTION:
	// A PI
	if ( local_name == "xml-stylesheet" ){
	  string sv = (const char*)xmlTextReaderConstValue(_reader);
	  pair<string,string> p = extract_style( sv );
	  _out_doc->addStyle( p.first, p.second );
	}
	else {
	  cerr << "unhandled PI: " << local_name << endl;
	}
	break;
      default:
	break;
      };
      ret = xmlTextReaderRead(_reader);
    }
    _out_doc->save_orig_ann_defaults();
    _ok = true;
    return _ok;
  }

  void Engine::append_node( FoliaElement *t,
			    int depth ){
    /// append a FoliaElement to the associated document
    /*!
      \param t the FoliaElement
      \param depth the location to use for adding
    */
    if ( _debug ){
      DBG << "append_node(" << t << ") current node= " << _current_node << endl;
      DBG << "append_node(): last node= " << _last_added << endl;
    }
    if ( depth == _last_depth ){
      if ( _debug ){
	DBG << "append_node(): EQUAL!" << endl;
      }
    }
    else if ( depth > _last_depth ){
      if ( _debug ){
	DBG << "append_node(): DEEPER!" << endl;
      }
      _current_node = _last_added;
    }
    else if ( depth < _last_depth  ){
      if ( _debug ){
	DBG << "append_node(): UP!" << endl;
      }
      for ( int i=0; i < _last_depth - depth; ++i ){
	_current_node = _current_node->parent();
	if ( _debug ){
	  DBG << "up node = " << _current_node << endl;
	}
      }
    }
    _last_depth = depth;
    _current_node->append( t );
    if ( _debug ){
      DBG << "append_node() result = " << _current_node << endl;
    }
    _last_added = t;
  }

  FoliaElement *Engine::handle_match( const string& local_name,
				      int depth ){
    /// expand a matched tag into a FoLiA subtree
    /*!
      \param local_name the tag to create
      \param depth the location in the Document to attach to
      \return an expanded FoLiA subtree
    */
    FoliaElement *t = AbstractElement::createElement( local_name, _out_doc );
    if ( t ){
      if ( _debug ){
	DBG << "created FoliaElement: name=" << local_name << endl;
      }
      xmlNode *fd = xmlTextReaderExpand(_reader);
      t->parseXml( fd );
      append_node( t, depth );
      _external_node = t;
      if ( _debug ){
	DBG << "expose external node: " << t << endl;
      }
      return t;
    }
    else if ( !_out_doc->permissive() ){
      _ok = false;
      throw XmlError( "folia::engine failed to create node: "
		      + local_name );
    }
    else {
      return 0;
    }
  }

  FoliaElement *Engine::get_node( const string& tag ){
    /// return the next node in the Engine with 'tag'
    /*!
      \param tag the tag or a list of tags we are looking for
      \return the FoliaElement found.

      tag may be a single tag like 'lemma' but also a list of '|' separated
      tags like 'lemma|pos|description'. In the latter case all named tags
      are tested and the first found is returned

      The returned FoliaElement is a FoLiA subtree expaned from the
      xmlTextReader. Further parsing will continue at the next sibbling
      of the parent.
    */
    if ( _done ){
      if ( _debug ){
	DBG << "Engine::get_node(). we are done" << endl;
      }
      return 0;
    }
    if ( _debug ){
      DBG << "Engine::get_node(), for tag=" << tag << endl;
    }
    int ret = 0;
    if ( _external_node != 0 ){
      // so our last action was to output a pointer to a subtree.
      // continue with the next node, avoiding the subtree
      _external_node = 0;
      ret = xmlTextReaderNext(_reader);
    }
    else {
      // so we are the first time here, just get the first node
      ret = xmlTextReaderRead(_reader);
    }
    if ( ret == 0 ){
      if ( _debug ){
	DBG << "get node name, DONE" << endl;
      }
      _done = true;
      return 0;
    }
    vector<string> tv = TiCC::split_at( tag, "|" );
    set<string> tags;
    for ( const auto& t : tv ){
      tags.insert(t);
    }
    while ( ret ){
      int type = xmlTextReaderNodeType(_reader);
      int new_depth = xmlTextReaderDepth(_reader);
      switch ( type ){
      case XML_READER_TYPE_ELEMENT: {
	string local_name = (const char*)xmlTextReaderConstLocalName(_reader);
	if ( _debug ){
	  DBG << "get node XML_ELEMENT name=" << local_name
	      << " depth " << _last_depth << " ==> " << new_depth << endl;
	}
	if ( tags.find(local_name) != tags.end() ){
	  if ( _debug ){
	    DBG << "matched search tag: " << local_name << endl;
	  }
	  _external_node = handle_match( local_name, new_depth );
	  return _external_node;
	}
	else if ( local_name == "t"
		  || local_name == "ph" ){
	  handle_content( local_name, new_depth );
	}
	else {
	  handle_element( local_name, new_depth );
	}
      }
	break;
      case XML_READER_TYPE_TEXT: {
	add_text( new_depth );
      }
	break;
      case XML_READER_TYPE_COMMENT: {
	add_comment( new_depth );
      }
	break;
      default: {
	add_default_node( new_depth );
      }
	break;
      }
      ret = xmlTextReaderRead(_reader);
    }
    _done = true;
    return 0;
  }

  xml_tree *Engine::create_simple_tree( const string& in_file ) const {
    /// create a lightweight tree for enumerating all XML_ELEMENTS encountered
    /*!
      \param in_file The file to create an xmlTextReader on. May be a string
      buffer containing a complete XML file too
      \return the light-weight tree with the relevant nodes
    */
    xmlTextReader *cur_reader = create_text_reader( in_file );
    if ( _debug ){
      DBG << "enumerate_nodes()" << endl;
    }
    int ret = xmlTextReaderRead(cur_reader);
    if ( ret == 0 ){
      throw runtime_error( "create_simple_tree() could not start" );
    }
    xml_tree *records = 0;
    xml_tree *rec_pnt = 0;
    int index = 0;
    int current_depth = 0;
    while ( ret ){
      int depth = xmlTextReaderDepth(cur_reader);
      int type = xmlTextReaderNodeType(cur_reader);
      if ( type == XML_READER_TYPE_ELEMENT
	   || type == XML_READER_TYPE_COMMENT ){
	string local_name = (const char*)xmlTextReaderConstLocalName(cur_reader);
	KWargs atts = get_attributes( cur_reader );
	string nsu;
	string txt_class;
	for ( auto const& v : atts ){
	  if ( v.first == "xmlns:xlink" ){
	    // only at top level
	    continue;
	  }
	  if ( v.first.find("xmlns") == 0 ){
	    nsu = v.second;
	  }
	  if ( v.first == "textclass"
	       || ( local_name == "t" && v.first == "class" ) ){
	    txt_class = v.second;
	  }
	}
	if ( nsu.empty() || nsu == NSFOLIA ){
	  xml_tree *add_rec = new xml_tree( depth, index, local_name, txt_class );
	  if ( _debug ){
	    DBG << "new record " << index << " " << local_name << " ("
		<< depth << ")" << endl;
	  }
	  if ( rec_pnt == 0 ){
	    records = add_rec;
	    rec_pnt = records;
	  }
	  else if ( depth == current_depth ){
	    add_rec->parent = rec_pnt->parent;
	    rec_pnt->next = add_rec;
	    rec_pnt = rec_pnt->next;
	  }
	  else if ( depth > current_depth ){
	    add_rec->parent = rec_pnt;
	    rec_pnt->link = add_rec;
	    rec_pnt = rec_pnt->link;
	  }
	  else { // depth < current_depth
	    while ( rec_pnt && rec_pnt->depth > depth ){
	      rec_pnt = rec_pnt->parent;
	    }
	    if ( rec_pnt == 0 ){
	      rec_pnt = records;
	    }
	    while ( rec_pnt->next ){
	      rec_pnt = rec_pnt->next;
	    }
	    add_rec->parent = rec_pnt->parent;
	    rec_pnt->next = add_rec;
	    rec_pnt = rec_pnt->next;
	  }
	  current_depth = rec_pnt->depth;
	}
	else {
	  if ( _debug ){
	    DBG << "name=" << local_name << " atts=" << atts << endl;
	    DBG << "create_simple_tree() node in alien namespace '"
		<< nsu << "' is SKIPPED!" << endl;
	  }
	}
	++index;
      }
      ret = xmlTextReaderRead(cur_reader);
    }
    xmlFreeTextReader( cur_reader );
    return records;
  }

  int count_nodes( FoliaElement *fe ){
    /// count all 'real' FoliaElements including and below this one
    /*!
      \param fe the The element to start at
      \return the 'size' of the subtree below fe. We need this number to know
      where to proceed processing
    */
    int result = 0;
    //    cerr << "DEPTH " << fe << endl;
    if ( fe
	 && fe->xmltag() != "_XmlText"
	 && fe->element_id() != HeadFeature_t
	 && !isAttributeFeature(fe->xmltag()) ){
      result += 1;
      if ( fe->size() > 0 ){
	//	cerr << "size=" << fe->size() << endl;
	for ( size_t i=0; i < fe->size(); ++i ){
	  //	  cerr << "i=" << i << endl;
	  result += count_nodes( fe->index(i) );
	}
      }
    }
    //    cerr << "return DEPTH " << fe << " ="  << result << endl;
    return result;
  }

  int Engine::handle_content( const string& t_or_ph, int depth ){
    /// process a matched 't' or 'ph' tag into a FoLiA subtree
    /*!
      \param t_or_ph a t or ph tags
      \param depth the location in the Document to attach to
      \return the number of FoliaElement nodes added
    */
    KWargs atts = get_attributes( _reader );
    if ( _debug ){
      DBG << "expanding content of <" << t_or_ph << "> atts=" << atts << endl;
    }
    FoliaElement *t = AbstractElement::createElement( t_or_ph, _out_doc );
    if ( t ){
      t->setAttributes( atts );
      // just take as is...
      xmlNode *fd = xmlTextReaderExpand(_reader);
      t->parseXml( fd );
      if ( _debug ){
	DBG << "parsed " << t << endl;
      }
      append_node( t, depth );
      // skip subtree
      xmlTextReaderNext(_reader);
      int type = xmlTextReaderNodeType(_reader);
      if ( type == XML_READER_TYPE_TEXT ){
	string value = (const char*)xmlTextReaderConstValue(_reader);
	string trimmed = TiCC::trim(value);
	if ( !trimmed.empty() ){
	  throw XmlError( "spurious text " + trimmed + " found after node <"
			  + t_or_ph + ">" );
	}
      }
      return count_nodes( t );
    }
    else {
      _ok = false;
      throw XmlError( "folia::engine failed to create node: " + t_or_ph );
    }
  }

  void Engine::handle_element( const string& local_name,
			       int depth ){
    /// process a matched tag into a FoLiA subtree
    /*!
      \param local_name the tag
      \param depth the location in the Document to attach to
    */
    KWargs atts = get_attributes( _reader );
    if ( _debug ){
      DBG << "name=" << local_name << " atts=" << atts << endl;
    }
    if ( local_name == "wref" ){
      string id = atts["id"];
      if ( id.empty() ){
	_ok = false;
	throw XmlError( "folia::engine, reference missing an 'id'" );
      }
      FoliaElement *ref = (*_out_doc)[id];
      if ( !ref ){
	_ok = false;
	throw XmlError( "folia::engine, unresolvable reference: "
			+ id );
      }
      ref->increfcount();
      append_node( ref, depth );
    }
    else {
      FoliaElement *t = AbstractElement::createElement( local_name, _out_doc );
      if ( t ){
	if ( local_name == "foreign-data" ){
	  xmlNode *fd = xmlTextReaderExpand(_reader);
	  t->parseXml( fd );
	  append_node( t, depth );
	  // skip subtree
	  xmlTextReaderNext(_reader);
	}
	else {
	  string nsu;
	  for ( auto const& v : atts ){
	    if ( v.first.find("xmlns:") == 0 ){
	      nsu = v.second;
	      break;
	    }
	  }

	  // We could use std::find_if here, but that is less readable:
	  // auto const& a = find_if( atts.begin(), atts.end(),
	  // 			   []( const pair<string,string>& av ){
	  // 			     return av.first.find("xmlns:") == 0;
	  // 			   } );
	  // if ( a != atts.end() ){
	  //   nsu = a->second;
	  // }

	  if ( nsu.empty() || nsu == NSFOLIA ){
	    if ( local_name == "desc"
		 || local_name == "content"
		 || local_name == "comment" ){
	      if ( xmlTextReaderIsEmptyElement(_reader) ){
		if ( _debug ){
		  DBG << "Element is empty." << endl;
		}
	      }
	      else {
		xmlTextReaderRead(_reader);
		const char *val = (const char*)xmlTextReaderConstValue(_reader);
		if ( val ) {
		  if ( _debug ){
		    DBG << "processing a <" << local_name << "> with value '"
			<< val << "'" << endl;
		  }
		  atts["value"] = val;
		}
		else {
		  if ( _debug ){
		    DBG << "processing a <" << local_name
			<< "> with empty value " << endl;
		  }
		}
	      }
	    }
	    if ( _debug ){
	      DBG << "SET ATTRIBUTES: " << atts << endl;
	    }
	    t->setAttributes( atts );
	    append_node( t, depth );
	  }
	  else {
	    if ( _debug ){
	      DBG << "a node in an alien namespace'" << nsu << endl;
	    }
	    // just take as is...
	    append_node( t, depth );
	    xmlNode *fd = xmlTextReaderExpand(_reader);
	    t->parseXml( fd );
	    // skip subtree
	    xmlTextReaderNext(_reader);
	  }
	}
      }
      else {
	_ok = false;
	throw XmlError( "folia::engine failed to create node: "
			+ local_name );
      }
    }
  }

  bool Engine::output_header(){
    /// output the 'header' of the Folia document to the associated output
    /// stream

    /// This outputs ALL metadata from the Document upto and including
    /// the opening \<text> of \<speech> node
    if ( _debug ){
      DBG << "Engine::output_header()" << endl;
    }
    if ( !_os ){
      throw logic_error( "folia::Engine::output_header() impossible. No output file specified!" );
      return false;
    }
    if ( _finished ){
      return true;
    }
    else if ( _header_done ){
      throw logic_error( "folia::Engine::output_header() is called twice!" );
      return false;
    }
    _header_done = true;
    stringstream ss;
    _out_doc->save( ss, ns_prefix );
    string data = ss.str();
    string search_b1;
    string search_b2;
    string search_e;
    if ( _doc_type == TEXT ){
      if ( !ns_prefix.empty() ){
	search_b1 = "<" + ns_prefix + ":" + "text>";
	search_b2 = "<" + ns_prefix + ":" + "text ";
	search_e = "</" + ns_prefix + ":" + "text>";
      }
      else {
	search_b1 = "<text>";
	search_b2 = "<text ";
	search_e = "</text>";
      }
    }
    else {
      if ( !ns_prefix.empty() ){
	search_b1 = "<" + ns_prefix + ":" + "speech>";
	search_b2 = "<" + ns_prefix + ":" + "speech ";
	search_e = "</" + ns_prefix + ":" + "speech>";
      }
      else {
	search_b1 = "<speech>";
	search_b2 = "<speech ";
	search_e = "</speech>";
      }
    }
    string::size_type bpos1 = data.find( search_b1 );
    string::size_type bpos2 = data.find( search_b2 );
    string::size_type pos1;
    if ( bpos1 < bpos2 ){
      pos1 = bpos1;
    }
    else {
      pos1 = bpos2;
    }
    string::size_type pos2;
    if ( _root_node->size() == 0 ){
      pos2 = data.find( "/>" , pos1 );
    }
    else {
      pos2 = data.find( ">" , pos1 );
    }
    string head = data.substr( 0, pos2 ) + ">";
    if ( _root_node->size() == 0 ){
      pos2 += 2;
    }
    else {
      pos2 = data.find( search_e, pos1 );
      int add = search_e.size();
      pos2 += add;
    }
    _footer = "  " + search_e + data.substr( pos2 );
    *_os << head << endl;
    return true;
  }

  bool Engine::output_footer(){
    /// output the remains of the associated Document
    /// might call flush() first

    /// further processing in this Engine is illegal
    if ( _debug ){
      DBG << "Engine::output_footer()" << endl;
    }
    if ( _finished ){
      return true;
    }
    if ( !_os ){
      throw logic_error( "folia::Engine::output_footer() impossible. No output file specified!" );
      return false;
    }
    else if ( flush() ){
      *_os << _footer << endl;
      _finished = true;
      return true;
    }
    else {
      return false;
    }
  }

  bool Engine::flush() {
    /// output all NEW information in the output Document to the output stream

    /// may call output_header() first
    if ( _debug ){
      DBG << "Engine::flush()" << endl;
    }
    if ( !_os ){
      throw logic_error( "folia::Engine::flush() impossible. No outputfile specified!" );
      return false;
    }
    if ( _finished ){
      return true;
    }
    else if ( !_header_done ){
      output_header();
    }
    stack<FoliaElement*> rem_list;
    size_t len = _root_node->size();
    for ( size_t i=0; i < len; ++i ){
      rem_list.push( _root_node->index(i) );
      *_os << "    " << _root_node->index(i)->xmlstring(true,2,false) << endl;
    }
    while ( !rem_list.empty() ){
      // we've kept a stack of elements to remove, as removing at the back
      // is the safest and cheapest thing to do
      _root_node->remove( rem_list.top() );
      destroy( rem_list.top() );
      rem_list.pop();
    }
    return true;
  }

  bool Engine::finish() {
    /// finalize the Engine bij calling output_footer
    if ( _debug ){
      DBG << "Engine::finish()" << endl;
    }
    if ( !_os ){
      throw logic_error( "folia::Engine::finish() impossible. No outputfile specified!" );
      return false;
    }
    if ( _finished ){
      return true;
    }
    return output_footer();
  }

  void Engine::save( const string& name, bool do_canon ){
    /// save the associated Document to a file
    /*!
      \param name the file-name
      \param do_canon output in Canonical format
    */
    if ( _os && name == _out_name ){
      throw logic_error( "folia::Engine::save() impossible. Already connected to a stream with the same name (" + name + ")" );
    }
    _out_doc->save( name, ns_prefix, do_canon );
  }

  void Engine::save( ostream& os, bool do_canon ){
    /// save the associated Document to a stream
    /*!
      \param os the stream
      \param do_canon output in Canonical format
    */
    _out_doc->save( os, ns_prefix, do_canon );
  }


  bool TextEngine::init_doc( const string& i, const string& o ){
    /// init an associated document for this TextEngine
    /*!
      \param i the input file to use for parsing
      \param o when not empty, add an output-file with this name

      Sets the _in_file property to i and marks _is_setup FALSE
      then calls Engine::init_doc to do the real work.
    */
    _in_file = i;
    _is_setup = false;
    //    set_debug(true);
    return Engine::init_doc( i, o );
  }

  void TextEngine::setup( const string& textclass, bool prefer_struct ){
    /// set the TextEngine ready for parsing
    /*!
      \param textclass Determines which textnodes to search for
      \param prefer_struct If TRUE, set the TextEngine up for returning
      Structure nodes like sentences or paragraphs above returning
      just Word or String nodes
    */
    string txtc = textclass;
    if ( txtc == "current" ){
      txtc.clear();
    }
    text_parent_map = enumerate_text_parents( txtc, prefer_struct );
    _next_text_node = _start_index;
    if ( !text_parent_map.empty() ){
      _next_text_node = text_parent_map.begin()->first;
    }
    _node_count = _start_index;
    _is_setup = true;
  }

  xml_tree *get_structure_parent( const xml_tree *pnt ){
    ///  return the nearest StructureElement above this node
    /*!
      \param pnt a (text) element in the simple tree.
      \return the first parent which is an AbstractStructureElement
      and NOT a Word
    */
    if ( pnt->parent->tag != "w"
	 && isSubClass( stringToElementType(pnt->parent->tag),
			AbstractStructureElement_t ) ){
      return pnt->parent;
    }
    else {
      return get_structure_parent( pnt->parent );
    }
  }

  map<int,int> TextEngine::search_text_parents( const xml_tree* start,
						const string& textclass,
						bool prefer_struct ) const{
    /// scan the whole TextEngine for TextContent nodes
    /*!
      \param start the tree to search
      \param textclass the text-class we are interested in
      \param prefer_struct If TRUE, set the TextEngine up for returning
      Structure nodes like sentences or paragraphs above returning
      just Word or String nodes
      \return a map containing for every found text_parent the index of
      the NEXT value to search. TO DO: very mysty and mystic
    */
    map<int,int> result;
    const xml_tree *pnt = start;
    while ( pnt ){
      if ( _debug ){
	DBG << "bekijk:" << pnt->tag << "-" << pnt->index << endl;
      }
      if ( pnt->tag == "wref"
	   || pnt->tag == "original" ){
	//
	// DON'T see a wref as a valid textparent.
	// The word is connected elsewhere too
	// Also an 'original' node is assumed to be part of a correction
	// so hope for a 'new' node to be found!
	pnt = pnt->next;
	continue;
      }
      map<int,int> deeper = search_text_parents( pnt->link,
						 textclass,
						 prefer_struct );
      if ( !deeper.empty() ){
	if ( _debug ){
	  DBG << "deeper we found: " << deeper << endl;
	}
	result.insert( deeper.begin(), deeper.end() );
      }
      pnt = pnt->next;
    }
    if ( result.empty() ){
      // so no deeper text found
      // lets see at this level....
      pnt = start;
      while ( pnt ){
	if ( pnt->tag == "t" && pnt->textclass == textclass ){
	  // OK text in the right textclass
	  if ( prefer_struct ){
	    // search for a suitable parent
	    xml_tree *par = get_structure_parent( pnt );
	    int index = par->index;
	    int next = INT_MAX;
	    if ( par->next ){
	      next = par->index;
	    }
	    result[index] = next;
	    break;
	  }
	  else {
	    int index = pnt->parent->index;
	    int next = INT_MAX;
	    if ( pnt->parent->next ){
	      next = pnt->parent->next->index;
	    }
	    else if ( pnt->parent->parent->next ){
	      next = pnt->parent->parent->next->index;
	    }
	    result[index] = next;
	    break;
	  }
	}
	pnt = pnt->next;
      }
    }
    if ( _debug && start && !result.empty() ){
      DBG << "return " << result << " for " << start->parent->tag << endl;
    }
    return result;
  }

  const map<int,int>& TextEngine::enumerate_text_parents( const string& textclass,
							  bool prefer_struct ) {
    /// Loop over the full input, looking for textnodes in class 'textclass'
    /*!
      \param textclass the text-class we are interested in
      \param prefer_struct If TRUE, set the TextEngine up for returning
      Structure nodes like sentences or paragraphs above returning
      just Word or String nodes
      \return a reference to a map of text parent nodes

      this function recurses to the DEEPEST text possible, and enumerates their
      parents. It creates a mapping of text parents indices to their successor
    */
    if ( _done ){
      throw runtime_error( "enumerate_text_parents() called on a done engine" );
    }
    if ( _debug ){
      DBG << "enumerate_text_parents(" << textclass << ")" << endl;
    }
    //
    // we start by creating a tree of all nodes
    xml_tree *tree = create_simple_tree(_in_file);
    //
    // now search that tree for nodes in 'textclass'
    // if is a <t>, then remember the index of its parent
    // but when 'prefer_struct' is specified, return the direct structure above
    // when present.
    text_parent_map.clear();
    xml_tree *rec_pnt = tree;
    while ( rec_pnt ){
      map<int,int> deeper = search_text_parents( rec_pnt->link,
						 textclass,
						 prefer_struct );
      text_parent_map.insert( deeper.begin(), deeper.end() );
      rec_pnt = rec_pnt->next;
    }
    if ( _debug ){
      DBG << "complete tree: " << endl;
      print( DBG, tree );
      DBG << "Search map = " << text_parent_map << endl;
    }
    for ( auto it = text_parent_map.begin();
	  it != text_parent_map.end();
	  ++it ){
      auto nit = it;
      ++nit;
      if ( nit != text_parent_map.end() ){
	it->second = nit->first;
      }
    }
    if ( _debug ){
      DBG << "Reduced Search map = " << text_parent_map << endl;
    }
    delete tree;
    return text_parent_map;
  }

  FoliaElement *TextEngine::next_text_parent(){
    /// return the next node to handle
    /*!
      \return a FoLiAElement pointer to a 'textparent' subtree, or 0 when done

      The caller may use this pointer to modify the subtree BELOW that pointer
      at will.

      next_text_parent should be called until no more candidates are found.
      At that moment, the complete input FoLiA is parsed and stored in _out_doc
      adn can be saved or handled over for further processing.

    */
    if ( _done ){
      if ( _debug ){
	DBG << "next_text_parent(). engine is done" << endl;
      }
      return 0;
    }
    if ( !_is_setup ){
      throw runtime_error( "TextEngine: not setup yet!" );
    }
    if ( text_parent_map.empty() ){
      if ( _debug ){
	DBG << "next_text_parent(). the parent map is empty." << endl;
      }
      return 0;
    }

    int ret = 0;
    if ( _external_node != 0 ){
      // so our last action was to output a pointer to a subtree.
      // continue with the next node, avoiding the subtree
      _external_node = 0;
      ret = xmlTextReaderNext(_reader);
    }
    else {
      // so we are the first time here, get first result
      ret = xmlTextReaderRead(_reader);
    }
    if ( ret == 0 ){
      if ( _debug ){
	DBG << "next_text_parent(), DONE" << endl;
      }
      _done = true;
      return 0;
    }
    while ( ret ){
      int type = xmlTextReaderNodeType(_reader);
      if ( _debug ){
	DBG << "MAIN LOOP search next_text_parent(), type=" << type
	    << " current node=" << _node_count
	    << " search for node=" << _next_text_node << endl;
      }
      int new_depth = xmlTextReaderDepth(_reader);
      switch ( type ){
      case XML_READER_TYPE_ELEMENT: {
	string local_name = (const char*)xmlTextReaderConstLocalName(_reader);
	if ( _debug ){
	  DBG << "next element: " << local_name << " cnt =" << _node_count << endl;
	}
	if ( _node_count == _next_text_node  ){
	  // HIT!
	  if ( _debug ){
	    DBG << "at index=" << _node_count << " WE HIT a next element for: " << local_name << endl;
	  }
	  _external_node = handle_match( local_name, new_depth );
	  int skips = count_nodes( _external_node );
	  // we are to output a tree of skips nodes
	  _node_count += skips; // so next time we resume with this count
	  _next_text_node = text_parent_map[_next_text_node];
	  // and we have to search for _next_text_node
	  if ( _debug ){
	    DBG << " increment _node_count with: " << skips << " to "
		<< _node_count << " searching for: "
		<< _next_text_node << endl;
	  }
	  return _external_node;
	}
	else if ( local_name == "t"
		  || local_name == "ph" ){
	  _node_count += handle_content( local_name, new_depth );
	}
	else {
	  handle_element( local_name, new_depth );
	  ++_node_count;
	}
      }
	break;
      case XML_READER_TYPE_TEXT: {
	add_text( new_depth );
      }
	break;
      case XML_READER_TYPE_COMMENT: {
	add_comment( new_depth );
      }
	break;
      default: {
	add_default_node( new_depth );
      }
	break;
      }
      ret = xmlTextReaderRead(_reader);
    }
    _done = true;
    return 0;
  }

} // namespace folia
