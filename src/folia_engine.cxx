/*
  Copyright (c) 2006 - 2018
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
#include <string>
#include <stdexcept>
#include "ticcutils/PrettyPrint.h"
#include "ticcutils/XMLtools.h"
#include "ticcutils/zipper.h"
#include "libfolia/folia.h"

using namespace std;

#define DBG *TiCC::Log(_dbg_file)

namespace folia {

  using TiCC::operator<<;

  Engine::Engine():
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
    _header_done(false),
    _finished(false),
    _ok(false),
    _done(false),
    _debug(false),
    _text_context(false)
  {
  }

  Engine::~Engine(){
    xmlFreeTextReader( _reader );
    delete _out_doc;
    delete _os;
  }

  Document *Engine::doc( bool disconnect ){
    // returns the FoLiA document. (assumes it is complete!)
    // may disconnect it from the engine. The caller has to delete it later
    Document *result = _out_doc;
    if ( disconnect ){
      _out_doc = 0;
    }
    return result;
  }

  bool Engine::set_debug( bool d ) {
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
    if ( _dbg_file ){
      delete _dbg_file;
    }
    _dbg_file = ls;
  }

  bool TextEngine::init_doc( const string& i, const string& o ){
    _in_file = i;
    _is_setup = false;
    return Engine::init_doc( i, o );
  }

  void TextEngine::setup( const string& textclass, bool prefer_sent ){
    string txtc = textclass;
    if ( txtc == "current" ){
      txtc.clear();
    }
    text_parent_map = enumerate_text_parents( txtc, prefer_sent );
    _next_text_node = _start_index;
    if ( !text_parent_map.empty() ){
      _next_text_node = text_parent_map.begin()->first;
    }
    _node_count = _start_index;
    _is_setup = true;
  }

  void Engine::un_declare( const AnnotationType::AnnotationType& at,
			   const string& setname ){
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

  void Engine::declare( const AnnotationType::AnnotationType& at,
			const string& setname,
			const string& args ) {
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

  void Engine::declare( const AnnotationType::AnnotationType& at,
			const string& setname,
			const KWargs& args ) {
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

  bool Engine::is_declared( const AnnotationType::AnnotationType& at,
			    const string& setname ) const {
    if ( !ok() ){
      throw logic_error( "is_declared() called on invalid engine!" );
    }
    else {
      return _out_doc->declared( at, setname );
    }
  }

  void Engine::declare( const AnnotationType::AnnotationType& at,
			const string& setname,
			const string& format,
			const string& annotator,
			const string& annotator_type,
			const string& time,
			const set<string>& processors,
			const string& alias ) {
    if ( !ok() ){
      throw logic_error( "declare() called on invalid engine!" );
    }
    else if ( _header_done ){
      throw logic_error( "declare() called on already (partially) saved document!" );
    }
    else {
      _out_doc->declare( at, setname, format, annotator, annotator_type, time,
			 processors, alias );
    }
  }

  bool Engine::is_declared( const AnnotationType::AnnotationType& at,
			    const string& setname,
			    const string& annotator,
			    const AnnotatorType& annotator_type,
			    const string& processor ) const {
    if ( !ok() ){
      throw logic_error( "is_declared() called on invalid engine!" );
    }
    else {
      return _out_doc->declared( at, setname, annotator, annotator_type, processor );
    }
  }

  bool Engine::is_declared( const AnnotationType::AnnotationType& at,
			    const string& setname,
			    const string& annotator,
			    const string& annotator_type,
			    const string& processor ) const {
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
    if ( !ok() ){
      throw logic_error( "set_metadata() called on invalid engine!" );
    }
    else {
      return _out_doc->set_metadata( att, val );
    }
  }

  pair<string,string> extract_style( const string& value ){
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
    if ( buf.find("<?xml ") == 0 ){
      return xmlReaderForMemory( buf.c_str(), buf.size(),
				 "input_buffer", 0, XML_PARSE_HUGE );
    }
    else if ( TiCC::match_back( buf, ".bz2" ) ){
      string buffer = TiCC::bz2ReadFile( buf );
      if ( buffer.empty() ){
	throw( runtime_error( "folia::Engine(), empty file? (" + buf
			      + ")" ) );
      }
      // return xmlReaderForMemory( buffer.c_str(), buffer.size(),
      // 				 buf.c_str(), 0, XML_PARSE_HUGE );
      string tmp_file = tmpnam(0);
      ofstream os( tmp_file );
      os << buffer << endl;
      os.close();
      return xmlReaderForFile( tmp_file.c_str(), 0, XML_PARSE_HUGE );
    }
    // libxml2 can handle .xml and .xml.gz
    return xmlReaderForFile( buf.c_str(), 0, XML_PARSE_HUGE );
  }

  void Engine::add_text( int depth ){
    string value = (const char*)xmlTextReaderConstValue(_reader);
    if ( _debug ){
      DBG << "add_text(" << value << ") depth=" << depth << endl;
    }
    XmlText *txt = new XmlText();
    txt->setvalue( value );
    append_node( txt, depth );
  }

  void Engine::add_comment( int depth ){
    if ( _debug ){
      DBG << "add_comment " << endl;
    }
    string tag = "_XmlComment";
    FoliaElement *t = AbstractElement::createElement( tag, _out_doc );
    append_node( t, depth );
  }

  void Engine::add_default_node( int depth ){
    string local_name = (const char*)xmlTextReaderConstLocalName(_reader);
    if ( local_name == "t" || local_name == "ph" ){
      // so we are AT THE END of a <t> or <ph> !
      _text_context = false;
    }
    if ( _debug ){
      int type = xmlTextReaderNodeType(_reader);
      DBG << "add_node " << type <<  " name=" << local_name
	  << " depth " << _last_depth << " ==> " << depth << endl;
    }
    if ( _text_context ){
      const char *val = (const char*)xmlTextReaderConstValue(_reader);
      if ( val ){
	// when inside a text_context, ALL text is relevant! add it
	string value = val;
	if ( _debug ){
	  DBG << "ADD extra tekst '" << value << "' TO TEXT " << endl;
	}
	XmlText *txt = new XmlText();
	txt->setvalue( value );
	append_node( txt, depth );
      }
    }
  }

  bool Engine::init_doc( const string& file_name,
			 const string& out_name ){
    _ok = false;
    _out_doc = new Document();
    if ( !out_name.empty() ){
      _os = new ofstream( out_name );
      _out_name = out_name;
    }
    _out_doc->_source_filename = file_name;
    _reader = create_text_reader( file_name );
    if ( _reader == 0 ){
      throw( runtime_error( "folia::Engine(), init failed on '" + file_name
			    + "'" ) );
    }
    int ret = xmlTextReaderRead(_reader);
    int index = 0;
    while ( ret > 0 ){
      int type =  xmlTextReaderNodeType(_reader );
      string local_name = (const char*)xmlTextReaderConstLocalName(_reader );
      switch ( type ){
      case XML_ELEMENT_NODE:
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
	    throw XmlError( "Engine: invalid FoLiA. missing ID" );
	  }
	}
	else if ( local_name == "metadata" ) {
	  xmlNode *node = xmlTextReaderExpand(_reader);
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
	  return _ok;
	}
	break;
      case XML_PI_NODE:
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
    _ok = true;
    return _ok;
  }

  bool Engine::next(){
    if ( _done ){
      return false;
    }
    else {
      return xmlTextReaderNext(_reader) == 1;
    }
  }

  void Engine::append_node( FoliaElement *t, int new_depth ){
    if ( new_depth == _last_depth ){
      if ( _debug ){
	DBG << "append_node(): EQUAL: current node = " << _current_node << endl;
	DBG << "last node = " << _last_added << endl;
      }
      _current_node->append( t );
    }
    else if ( new_depth > _last_depth ){
      if ( _debug ){
	DBG << "append_node(): DEEPER: current node = " << _current_node << endl;
      }
      _current_node = _last_added;
      if ( _debug ){
	DBG << "Dus nu: current node = " << _current_node << endl;
      }
      _current_node->append( t );
      _last_depth = new_depth;
    }
    else if ( new_depth < _last_depth  ){
      if ( _debug ){
	DBG << "append_node(): UP current node = " << _current_node << endl;
	DBG << "last node = " << _last_added << endl;
      }
      for ( int i=0; i < _last_depth-new_depth; ++i ){
	_current_node = _current_node->parent();
	if ( _debug ){
	  DBG << "up node = " << _current_node << endl;
	}
      }
      if ( _debug ){
	DBG << "so now: current node = " << _current_node << endl;
      }
      _current_node->append( t );
      _last_depth = new_depth;
    }
    _last_added = t;
  }

  FoliaElement *Engine::get_node( const string& tag ){
    if ( _done ){
      if ( _debug ){
	DBG << "get node name(). engine is done" << endl;
      }
      return 0;
    }
    if ( _debug ){
      DBG << "get node name, for tag=" << tag << endl;
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
      case XML_ELEMENT_NODE: {
	string local_name = (const char*)xmlTextReaderConstLocalName(_reader);
	if ( local_name == "t" || local_name == "ph" ){
	  // a <t> or a <ph> node starts. We have a text_context
	  _text_context = true;
	}
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
	else {
	  handle_element( local_name, new_depth, false );
	}
      }
	break;
      case XML_TEXT_NODE: {
	add_text( new_depth );
      }
	break;
      case XML_COMMENT_NODE: {
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

  xml_tree::xml_tree( int d, int i, const std::string& t, const std::string& c ):
    depth(d),
    index(i),
    tag(t),
    textclass(c),
    parent(0),
    link(0),
    next(0)
  {}

  xml_tree::~xml_tree() {
    if ( link ){
      delete link;
    }
    if ( next ){
      delete next;
    }
  }

  void print( ostream& os, const xml_tree* recs ){
    const xml_tree *rec_pnt = recs;
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

  ostream& operator<<( ostream& os, const xml_tree* xt ){
    os << endl;
    print( os, xt );
    return os;
  }

  xml_tree *Engine::create_simple_tree( const string& in_file ) const {
    ///
    /// create an xmlReader and use that to loop over the full input,
    /// creating a lightweight tree for enumerating all XML_ELEMENTS encountered
    ///
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
      if ( type == XML_ELEMENT_NODE || type == XML_COMMENT_NODE ){
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

  xml_tree *get_structure_parent( const xml_tree *pnt ){
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
						   bool prefer_sentences ) const{
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
						 prefer_sentences );
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
	  if ( prefer_sentences ){
	    // search for a non-word parent
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
    if ( _debug && !result.empty() ){
      DBG << "return " << result << " for " << start->parent->tag << endl;
    }
    return result;
  }

  map<int,int> TextEngine::enumerate_text_parents( const string& textclass,
						      bool prefer_sent ) const {
    ///
    /// Loop over the full input, looking for textnodes in class 'textclass'
    /// for the DEEPEST text possible, enumerate their parents
    /// when prefer_sent eqs true, prefer structure elements over the words or
    /// strings within
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
    // but when 'prefer_sent' is specified, return the direct structure above
    // when present.
    map<int,int> result;
    xml_tree *rec_pnt = tree;
    while ( rec_pnt ){
      map<int,int> deeper = search_text_parents( rec_pnt->link,
						 textclass,
						 prefer_sent );
      result.insert( deeper.begin(), deeper.end() );
      rec_pnt = rec_pnt->next;
    }
    if ( _debug ){
      DBG << "complete tree: " << endl;
      print( DBG, tree );
      DBG << "Search map = " << result << endl;
    }
    for ( auto it = result.begin(); it != result.end(); ++it ){
      auto nit = it;
      ++nit;
      if ( nit != result.end() ){
	it->second = nit->first;
      }
    }
    if ( _debug ){
      DBG << "Reduced Search map = " << result << endl;
    }
    delete tree;
    return result;
  }

  int count_nodes( FoliaElement *fe ){
    // count all 'real' FoliaElements including and below this one
    // so this is the 'size' of the subtree
    // we need this number to know where te proceed
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

  FoliaElement *Engine::handle_match( const string& local_name,
					 int depth ){
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
      throw XmlError( "folia::engine failed to create node: "
		      + local_name );
    }
    else {
      return 0;
    }
  }

  void Engine::handle_element( const string& local_name,
				 int depth,
				 bool skip_t ){
    KWargs atts = get_attributes( _reader );
    if ( _debug ){
      DBG << "name=" << local_name << " atts=" << atts << endl;
    }
    if ( local_name == "wref" ){
      string id = atts["id"];
      if ( id.empty() ){
	throw XmlError( "folia::engine, reference missing an 'id'" );
      }
      FoliaElement *ref = (*_out_doc)[id];
      if ( !ref ){
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
	  if ( nsu.empty() || nsu == NSFOLIA ){
	    if ( skip_t && local_name == "t" ){
	    }
	    else if ( ( !skip_t && local_name == "t" )
		      || local_name == "desc"
		      || local_name == "content"
		      || local_name == "comment" ){
	      xmlTextReaderRead(_reader);
	      const char *val = (const char*)xmlTextReaderConstValue(_reader);
	      if ( val ) {
		atts["value"] = val;
	      }
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
	throw XmlError( "folia::engine failed to create node: "
			+ local_name );
      }
    }
  }

  FoliaElement *TextEngine::next_text_parent(){
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
    ///
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
      case XML_ELEMENT_NODE: {
	string local_name = (const char*)xmlTextReaderConstLocalName(_reader);
	if ( _debug ){
	  DBG << "next element: " << local_name << " cnt =" << _node_count << endl;
	}
	if ( local_name == "t" || local_name == "ph" ){
	  // a <t> or a <ph> node starts. We have a text_context
	  _text_context = true;
	}
	if ( _node_count == _next_text_node  ){
	  // HIT!
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
	else {
	  handle_element( local_name, new_depth, true );
	  ++_node_count;
	}
      }
	break;
      case XML_TEXT_NODE: {
	add_text( new_depth );
      }
	break;
      case XML_COMMENT_NODE: {
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


  bool Engine::output_header(){
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
    size_t len = _root_node->size();
    for ( size_t i=0; i < len; ++i ){
      *_os << "    " << _root_node->index(i)->xmlstring(true,2,false) << endl;
    }
    for ( size_t i=0; i < len; ++i ){
      _root_node->remove( i, true );
    }
    return true;
  }

  bool Engine::flush( FoliaElement *root ) {
    if ( _debug ){
      DBG << "Engine::flush( " << root->xmltag() << " )" << endl;
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
    size_t len = root->size();
    for ( size_t i=0; i < len; ++i ){
      *_os << "    " << root->index(i)->xmlstring(true,2,false) << endl;
    }
    for ( size_t i=0; i < len; ++i ){
      root->remove( i, true );
    }
    return true;
  }

  bool Engine::finish() {
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

  void Engine::save( const string& name, bool do_kanon ){
    if ( _os && name == _out_name ){
      throw logic_error( "folia::Engine::save() impossible. Already connected to a stream withe the same name (" + name + ")" );
    }
    _out_doc->save( name, ns_prefix, do_kanon );
  }

  void Engine::save( ostream& os, bool do_kanon ){
    _out_doc->save( os, ns_prefix, do_kanon );
  }

} // namespace folia
