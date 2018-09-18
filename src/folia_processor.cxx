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
#include <fstream>
#include <cstring>
#include <string>
#include <stdexcept>
#include "ticcutils/PrettyPrint.h"
#include "ticcutils/XMLtools.h"
#include "ticcutils/zipper.h"
#include "libfolia/folia.h"

using namespace std;

namespace folia {

  using TiCC::operator<<;

  Processor::Processor():
    _in_doc(0),
    _out_doc(0),
    _root_node(0),
    _external_node(0),
    _current_node(0),
    _last_added(0),
    _last_depth(2),
    _os(0),
    _header_done(false),
    _finished(false),
    _ok(false),
    _done(false),
    _debug(false)
  {
  }

  Processor::~Processor(){
    xmlFreeTextReader( _in_doc );
    delete _out_doc;
    delete _os;
  }

  void Processor::declare( AnnotationType::AnnotationType at,
			   const string& setname,
			   const string& args ) {
    if ( !ok() ){
      throw logic_error( "declare() called on invalid processor!" );
    }
    else if ( _header_done ){
      throw logic_error( "declare() called on already (partially) saved document!" );
    }
    else {
      _out_doc->declare( at, setname, args );
    }
  }

  bool Processor::is_declared( AnnotationType::AnnotationType at,
			       const string& setname ) const {
    if ( !ok() ){
      throw logic_error( "is_declared() called on invalid processor!" );
    }
    else {
      return _out_doc->isDeclared( at, setname );
    }
  }

  void Processor::declare( AnnotationType::AnnotationType at,
			   const string& setname,
			   const string& annotator,
			   const string& annotator_type,
			   const string& time,
			   const string& args ) {
    if ( !ok() ){
      throw logic_error( "declare() called on invalid processor!" );
    }
    else if ( _header_done ){
      throw logic_error( "declare() called on already (partially) saved document!" );
    }
    else {
      _out_doc->declare( at, setname, annotator, annotator_type, time, args );
    }
  }

  bool Processor::is_declared( AnnotationType::AnnotationType at,
			       const string& setname,
			       const string& annotator,
			       const string& annotator_type ) const {
    if ( !ok() ){
      throw logic_error( "is_declared() called on invalid processor!" );
    }
    else {
      return _out_doc->isDeclared( at, setname, annotator, annotator_type );
    }
  }

  void Processor::set_metadata( const std::string& att,
				const std::string& val){
    if ( !ok() ){
      throw logic_error( "set_metadata() called on invalid processor!" );
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
      vector<string> w;
      TiCC::split_at( v[0], w, "=" );
      if ( w.size() == 2 && w[0] == "type" ){
	type = w[1].substr(1,w[1].length()-2);
      }
      w.clear();
      TiCC::split_at( v[1], w, "=" );
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
	if ( att == "xml:id" ){
	  att = "_id";
	}
	else if ( att == "xlink:href" ){
	  att = "href";
	}
	else if ( att == "xlink:type" ){
	  att = "type";
	}
	else if ( att == "xlink:role" ){
	  att = "role";
	}
	else if ( att == "xlink:title" ){
	  att = "title";
	}
	result[att] = val;
      }
      while ( xmlTextReaderMoveToNextAttribute(tr) );
    }
    return result;
  }

  bool Processor::init_doc( const string& file_name,
			    const string& out_name ){
    _ok = false;
    _out_doc = new Document();
    if ( !out_name.empty() ){
      _os = new ofstream( out_name );
      _out_name = out_name;
    }
    if ( TiCC::match_back( file_name, ".bz2" ) ){
      string buffer = TiCC::bz2ReadFile( file_name );
      if ( buffer.empty() ){
	throw( runtime_error( "folia::Processor(), empty file? (" + file_name
			      + ")" ) );
      }
      string tmp_file = tmpnam(0);
      ofstream os( tmp_file );
      os << buffer << endl;
      os.close();
      _in_doc = xmlNewTextReaderFilename( tmp_file.c_str() );
    }
    else {
      // can handle .xml and .xml.gz
      _in_doc = xmlNewTextReaderFilename( file_name.c_str() );
    }
    if ( _in_doc == 0 ){
      throw( runtime_error( "folia::Processor(), init failed on '" + file_name
			    + "'" ) );
    }
    int ret = xmlTextReaderRead(_in_doc);
    while ( ret > 0 ){
      int type =  xmlTextReaderNodeType(_in_doc );
      string name = (const char*)xmlTextReaderConstName(_in_doc );
      string local_name = (const char*)xmlTextReaderConstLocalName(_in_doc );
      //      cerr << "node type = " << type << " name: " << local_name << endl;
      switch ( type ){
      case 1:
	if ( local_name == "FoLiA" ){
	  // found the root
	  const xmlChar *pnt = xmlTextReaderConstPrefix(_in_doc);
	  if ( pnt ){
	    _out_doc->_foliaNsIn_prefix = xmlStrdup(pnt );
	    ns_prefix = (const char*)pnt;
	  }
	  pnt = xmlTextReaderConstNamespaceUri(_in_doc);
	  if ( pnt ){
	    _out_doc->_foliaNsIn_href = xmlStrdup(pnt);
	  }
	  KWargs in_args = get_attributes( _in_doc );
	  string id;
	  if ( !in_args.empty() ){
	    id = in_args["_id"];
	  }
	  if ( !id.empty() ){
	    _out_doc->setDocumentProps( in_args );
	    KWargs out_args;
	    out_args["id"] = id;
	    FoliaElement *root = new FoLiA( out_args, _out_doc );
	    _out_doc->foliadoc = root;
	  }
	  else {
	    throw XmlError( "Processor: invalid FoLiA. missing ID" );
	  }
	}
	else if ( local_name == "metadata" ) {
	  KWargs atts = get_attributes( _in_doc );
	  string type = TiCC::lowercase(atts["type"]);
	  if ( type.empty() ){
	    type = "native";
	  }
	  string src = atts["src"];
	  if ( !src.empty() ){
	    _out_doc->_metadata = new ExternalMetaData( type, src );
	  }
	  else if ( type == "native" || type == "imdi" ){
	    _out_doc->_metadata = new NativeMetaData( type );
	  }
	  else {
	    _out_doc->_metadata = 0;
	  }
	  xmlNode *node = xmlTextReaderExpand(_in_doc);
	  xmlNode *m = node->children;
	  while ( m ){
	    if ( TiCC::Name( m ) == "METATRANSCRIPT" ){
	      //	      if ( !_out_doc->_metadata ){
		_out_doc->_metadata = new ForeignMetaData( "imdi" );
		//	      }
	      _out_doc->_metadata->add_foreign( xmlCopyNode(m,1) );
	    }
	    else if ( TiCC::Name( m ) == "annotations" ){
	      _out_doc->parseannotations( m );
	    }
	    else if ( TiCC::Name( m ) == "meta" ){
	      _out_doc->parsemeta( m );
	    }
	    else if ( TiCC::Name(m)  == "foreign-data"){
	      FoliaElement *t = FoliaImpl::createElement( "foreign-data", _out_doc );
	      if ( t ){
		t = t->parseXml( m );
		if ( t ){
		  if ( _out_doc->_metadata
		       && _out_doc->_metadata->datatype() == "NativeMetaData" ){
		    cerr << "WARNING: foreign-data found in metadata of type 'native'"  << endl;
		    cerr << "changing type to 'foreign'" << endl;
		    type = "foreign";
		    delete _out_doc->_metadata;
		    _out_doc->_metadata = new ForeignMetaData( type );
		  }
		  if ( ! _out_doc->_metadata ){
		     _out_doc->_metadata = new ForeignMetaData( type );
		  }
		  _out_doc->_metadata->add_foreign( m );
		}
	      }
	    }
	    else if ( TiCC::Name(m)  == "submetadata" ){
	      _out_doc->parsesubmeta( m );
	    }
	    m = m->next;
	  }
	}
	else if ( local_name == "text" ){
	  KWargs args = get_attributes(_in_doc);
	  Text *text = new Text( args, _out_doc );
	  _out_doc->append( text );
	  _root_node = text;
	  _current_node = text;
	  _ok = true;
	  return _ok;
	}
	break;
      case 7:
	// A PI
	if ( name == "xml-stylesheet" ){
	  string sv = (const char*)xmlTextReaderConstValue(_in_doc);
	  pair<string,string> p = extract_style( sv );
	  _out_doc->addStyle( p.first, p.second );
	}
	else {
	  cerr << "unhandled PI: " << xmlTextReaderLocalName(_in_doc ) << endl;
	}
	break;
      case 14:
	// an DTD node
	break;
      default:
	break;
      };
      ret = xmlTextReaderRead(_in_doc);
    }
    _ok = true;
    return _ok;
  }

  bool Processor::next(){
    if ( _done ){
      return false;
    }
    else {
      return xmlTextReaderNext(_in_doc) == 1;
    }
  }

  void Processor::append_node( FoliaElement *t, int new_depth ){
    if ( new_depth == _last_depth ){
      if ( _debug ){
	cerr << "GELIJK: current node = " << _current_node << endl;
	cerr << "last node = " << _last_added << endl;
      }
      _current_node->append( t );
    }
    else if ( new_depth > _last_depth ){
      if ( _debug ){
	cerr << "DIEPER: current node = " << _current_node << endl;
      }
      _current_node = _last_added;
      if ( _debug ){
	cerr << "Dus nu: current node = " << _current_node << endl;
      }
      _current_node->append( t );
      _last_depth = new_depth;
    }
    else if ( new_depth < _last_depth  ){
      if ( _debug ){
	cerr << "OMHOOG current node = " << _current_node << endl;
	cerr << "last node = " << _last_added << endl;
      }
      for ( int i=0; i < _last_depth-new_depth; ++i ){
	if ( _debug ){
	  cerr << "up node = " << _current_node << endl;
	}
	_current_node = _current_node->parent();
      }
      if ( _debug ){
	cerr << "NU current node = " << _current_node << endl;
      }
      _current_node->append( t );
      _last_depth = new_depth;
    }
  }

  FoliaElement *Processor::get_node( const string& tag ){
    if ( _done ){
      if ( _debug ){
	cerr << "get node name(). processor is done" << endl;
      }
      return 0;
    }
    if ( _debug ){
      cerr << "get node name, for tag=" << tag << endl;
    }
    int ret = 0;
    if ( _external_node != 0 ){
      _external_node = 0;
      ret = 1;
      if ( _debug ){
	cerr << "get node name, add external node, read on" << endl;
      }
    }
    else {
      ret = xmlTextReaderRead(_in_doc);
    }
    if ( ret == 0 ){
      if ( _debug ){
	cerr << "get node name, DONE" << endl;
      }
      return 0;
    }
    vector<string> tv = TiCC::split_at( tag, "|" );
    set<string> tags;
    for ( const auto& t : tv ){
      tags.insert(t);
    }
    while ( ret ){
      int type = xmlTextReaderNodeType(_in_doc);
      if ( type == XML_ELEMENT_NODE ){
	string local_name = (const char*)xmlTextReaderConstLocalName(_in_doc);
	int new_depth = xmlTextReaderDepth(_in_doc);
	if ( _debug ){
	  cerr << "get node name=" << local_name
	       << " depth " << _last_depth << " ==> " << new_depth << endl;
	}
	if ( tags.find(local_name) != tags.end() ){
	  KWargs atts = get_attributes( _in_doc );
	  if ( _debug ){
	    cerr << "matched search tag: " << local_name
		 << " atts=" << atts << endl;
	  }
	  FoliaElement *t = FoliaImpl::createElement( local_name, _out_doc );
	  if ( _debug ){
	    cerr << "created FoliaElement: name=" << local_name << endl;
	  }
	  xmlNode *fd = xmlTextReaderExpand(_in_doc);
	  t->parseXml( fd );
	  append_node( t, new_depth );
	  ret = xmlTextReaderNext(_in_doc);
	  _done = (ret == 0);
	  _external_node = t;
	  return t;
	}
	else {
	  KWargs atts = get_attributes( _in_doc );
	  if ( _debug ){
	    cerr << "name=" << local_name << " atts=" << atts << endl;
	  }
	  if ( local_name == "wref" ){
	    FoliaElement *ref = (*_out_doc)[atts["id"]];
	    ref->increfcount();
	    append_node( ref, new_depth );
	  }
	  else if ( local_name == "foreign-data" ){
	    FoliaElement *t = FoliaImpl::createElement( local_name, _out_doc );
	    xmlNode *fd = xmlTextReaderExpand(_in_doc);
	    t->parseXml( fd );
	    append_node( t, new_depth );
	    ret = xmlTextReaderNext(_in_doc);
	  }
	  else if ( local_name == "t" ){
	    FoliaElement *t = FoliaImpl::createElement( local_name, _out_doc );
	    xmlNode *fd = xmlTextReaderExpand(_in_doc);
	    t->parseXml( fd );
	    append_node( t, new_depth );
	    ret = xmlTextReaderNext(_in_doc);
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
	      FoliaElement *t = FoliaImpl::createElement( local_name, _out_doc );
	      if ( local_name == "desc"
		   || local_name == "content"
		   || local_name == "comment" ){
		ret = xmlTextReaderRead(_in_doc);
		const char *val = (const char*)xmlTextReaderConstValue(_in_doc);
		if ( val ) {
		  string value = val;
		  if ( !value.empty() ) {
		    atts["value"] = value;
		  }
		}
	      }
	      t->setAttributes( atts );
	      append_node( t, new_depth );
	      _last_added = t;
	      if ( _debug ){
		cerr << "einde current node = " << _current_node << endl;
		cerr << "last node = " << _last_added << endl;
	      }
	    }
	    else {
	      if ( _debug ){
		cerr << "a node in alien namespace'" << atts["xmlns:"]
		     << " SKIPPED!" << endl;
	      }
	      // just take as is...
	      FoliaElement *t = FoliaImpl::createElement( local_name,
	       						  _out_doc );
	      append_node( t, new_depth );
	      xmlNode *fd = xmlTextReaderExpand(_in_doc);
	      t->parseXml( fd );
	      ret = xmlTextReaderNext(_in_doc);
	    }
	  }
	}
      }
      else if ( type == XML_TEXT_NODE ){
	XmlText *txt = new XmlText();
	string value = (const char*)xmlTextReaderConstValue(_in_doc);
	if ( _debug ){
	  cerr << "TXT VALUE = '" << value << "'" << endl;
	}
	txt->setvalue( value );
	int new_depth = xmlTextReaderDepth(_in_doc);
	append_node( txt, new_depth );
	_last_added = txt;
	if ( _debug ){
	  cerr << "einde current node = " << _current_node << endl;
	  cerr << "last node = " << _last_added << endl;
	}
	_last_depth = xmlTextReaderDepth(_in_doc);
      }
      else if ( type == XML_COMMENT_NODE ){
	cerr << "SKIP comment!" << endl;
      }
      ret = xmlTextReaderRead(_in_doc);
    }
    _done = true;
    return 0;
  }

  struct my_rec {
    my_rec( int d, int i, const string& t, const string& c ):
      depth(d),
      index(i),
      tag(t),
      textclass(c),
      parent(0),
      link(0),
      next(0)
    {};
    int depth;
    int index;
    string tag;
    string textclass;
    my_rec *parent;
    my_rec *link;
    my_rec *next;
  };

  void print( ostream& os, my_rec* recs ){
    my_rec *rec_pnt = recs;
    while ( rec_pnt ){
      os << rec_pnt->index << string( rec_pnt->depth, ' ' )
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

  my_rec *Processor::create_simple_tree() const {
    ///
    /// Loop over the full input, creating a lightweight tree for
    /// enumerating all XML_ELEMENTS encountered
    ///
    if ( _done ){
      throw runtime_error( "create_simple_tree() called on a done processor" );
    }
    if ( _debug ){
      cerr << "enumerate_nodes()" << endl;
    }
    int ret = xmlTextReaderRead(_in_doc);
    if ( ret == 0 ){
      throw runtime_error( "create_simple_tree() could not start" );
    }
    my_rec *records = 0;
    my_rec *rec_pnt = 0;
    int index = 0;
    int current_depth = 0;
    while ( ret ){
      int type = xmlTextReaderNodeType(_in_doc);
      if ( type == XML_ELEMENT_NODE ){
	string local_name = (const char*)xmlTextReaderConstLocalName(_in_doc);
	KWargs atts = get_attributes( _in_doc );
	string nsu;
	string txt_class;
	for ( auto const& v : atts ){
	  if ( v.first.find("xmlns:") == 0 ){
	    nsu = v.second;
	    break;
	  }
	  if ( v.first == "textclass"
	       || ( local_name == "t" && v.first == "class" ) ){
	    txt_class = v.second;
	  }
	}
	if ( nsu.empty() || nsu == NSFOLIA ){
	  int depth = xmlTextReaderDepth(_in_doc);
	  my_rec *add_rec = new my_rec( depth, index, local_name, txt_class );
	  if ( _debug ){
	    cerr << "new record " << index << " " << local_name << " ("
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
	    add_rec->parent = rec_pnt->parent;
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
	  ++index;
	}
	else {
	  if ( _debug ){
	    cerr << "name=" << local_name << " atts=" << atts << endl;
	  }
	  if ( _debug ){
	    cerr << "create_simple_tree() node in alien namespace'"
		 << atts["xmlns:"]
		 << " is SKIPPED!" << endl;
	  }
	}
      }
      ret = xmlTextReaderRead(_in_doc);
    }
    return records;
  }

  set<int> Processor::enumerate_text_parents( const string& textclass ) const {
    ///
    /// Loop over the full input, looking for textnodes in class 'texclass'
    /// for the DEEPEST text possible, enumerate their parents
    ///
    if ( _done ){
      throw runtime_error( "enumerate_text_parents() called on a done processor" );
    }
    if ( _debug ){
      cerr << "enumerate_nodes()" << endl;
    }
    int ret = xmlTextReaderRead(_in_doc);
    if ( ret == 0 ){
      throw runtime_error( "enumerate_text_parents() could not start" );
    }
    //
    // we start by creating a tree of all nodes
    my_rec *tree = create_simple_tree();
    //
    // now search that tree for nodes in 'textclass'
    // if is a <t>, the remember the index of its parent
    set<int> result;
    my_rec *rec_pnt = tree;
    return result;
  }

  FoliaElement *Processor::next_text_parent( ){
    if ( _done ){
      if ( _debug ){
	cerr << "next_text_parent(). processor is done" << endl;
      }
      return 0;
    }
    int ret = 0;
    if ( _external_node != 0 ){
      _external_node = 0;
      ret = 1;
      if ( _debug ){
	cerr << "next text_parent() at external node, read on" << endl;
      }
    }
    else {
      ret = xmlTextReaderRead(_in_doc);
    }
    if ( ret == 0 ){
      if ( _debug ){
	cerr << "next_text_parent(), DONE" << endl;
      }
      return 0;
    }
    int node_count = 0;
    while ( ret ){
      int type = xmlTextReaderNodeType(_in_doc);
      if ( type == XML_ELEMENT_NODE ){
	string local_name = (const char*)xmlTextReaderConstLocalName(_in_doc);
	int new_depth = xmlTextReaderDepth(_in_doc);
	if ( text_parent_set.find( node_count ) != text_parent_set.end() ){
	  // HIT!
	  if ( _debug ){
	    cerr << "next_text_parent() hit a =" << local_name << endl;
	  }
	  KWargs atts = get_attributes( _in_doc );
	  if ( _debug ){
	    cerr << "matched search tag: " << local_name
		 << " atts=" << atts << endl;
	  }
	  FoliaElement *t = FoliaImpl::createElement( local_name, _out_doc );
	  if ( _debug ){
	    cerr << "created FoliaElement: name=" << local_name << endl;
	  }
	  xmlNode *fd = xmlTextReaderExpand(_in_doc);
	  t->parseXml( fd );
	  append_node( t, new_depth );
	  ret = xmlTextReaderNext(_in_doc);
	  _done = (ret == 0);
	  _external_node = t;
	  return t;
	}
	else {
	  KWargs atts = get_attributes( _in_doc );
	  if ( _debug ){
	    cerr << "name=" << local_name << " atts=" << atts << endl;
	  }
	  if ( local_name == "wref" ){
	    FoliaElement *ref = (*_out_doc)[atts["id"]];
	    ref->increfcount();
	    append_node( ref, new_depth );
	  }
	  else if ( local_name == "foreign-data" ){
	    FoliaElement *t = FoliaImpl::createElement( local_name, _out_doc );
	    xmlNode *fd = xmlTextReaderExpand(_in_doc);
	    t->parseXml( fd );
	    append_node( t, new_depth );
	    ret = xmlTextReaderNext(_in_doc);
	  }
	  else if ( local_name == "t" ){
	    FoliaElement *t = FoliaImpl::createElement( local_name, _out_doc );
	    xmlNode *fd = xmlTextReaderExpand(_in_doc);
	    t->parseXml( fd );
	    append_node( t, new_depth );
	    ret = xmlTextReaderNext(_in_doc);
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
	      FoliaElement *t = FoliaImpl::createElement( local_name, _out_doc );
	      if ( local_name == "desc"
		   || local_name == "content"
		   || local_name == "comment" ){
		ret = xmlTextReaderRead(_in_doc);
		const char *val = (const char*)xmlTextReaderConstValue(_in_doc);
		if ( val ) {
		  string value = val;
		  if ( !value.empty() ) {
		    atts["value"] = value;
		  }
		}
	      }
	      t->setAttributes( atts );
	      append_node( t, new_depth );
	      _last_added = t;
	      if ( _debug ){
		cerr << "einde current node = " << _current_node << endl;
		cerr << "last node = " << _last_added << endl;
	      }
	    }
	    else {
	      if ( _debug ){
		cerr << "a node in alien namespace'" << atts["xmlns:"]
		     << " SKIPPED!" << endl;
	      }
	      // just take as is...
	      FoliaElement *t = FoliaImpl::createElement( local_name,
	       						  _out_doc );
	      append_node( t, new_depth );
	      xmlNode *fd = xmlTextReaderExpand(_in_doc);
	      t->parseXml( fd );
	      ret = xmlTextReaderNext(_in_doc);
	    }
	  }
	}
      }
      else if ( type == XML_TEXT_NODE ){
	XmlText *txt = new XmlText();
	string value = (const char*)xmlTextReaderConstValue(_in_doc);
	if ( _debug ){
	  cerr << "TXT VALUE = '" << value << "'" << endl;
	}
	txt->setvalue( value );
	int new_depth = xmlTextReaderDepth(_in_doc);
	append_node( txt, new_depth );
	_last_added = txt;
	if ( _debug ){
	  cerr << "einde current node = " << _current_node << endl;
	  cerr << "last node = " << _last_added << endl;
	}
	_last_depth = xmlTextReaderDepth(_in_doc);
      }
      else if ( type == XML_COMMENT_NODE ){
	cerr << "SKIP comment!" << endl;
      }
      ret = xmlTextReaderRead(_in_doc);
      ++node_count;
    }
    _done = true;
    return 0;
  }


  bool Processor::output_header(){
    if ( _debug ){
      cerr << "Processor::output_header()" << endl;
    }
    if ( !_os ){
      throw logic_error( "folia::Processor::output_header() impossible. No output file specified!" );
      return false;
    }
    if ( _finished ){
      return true;
    }
    else if ( _header_done ){
      throw logic_error( "folia::Processor::output_header() is called twice!" );
      return false;
    }
    _header_done = true;
    stringstream ss;
    _out_doc->save( ss, ns_prefix );
    string data = ss.str();
    string search_b = "<text";
    string search_e = "</text";
    if ( !ns_prefix.empty() ){
      search_b = "<" + ns_prefix + ":" + "text";
      search_e = "</" + ns_prefix + ":" + "text";
    }
    int add = search_e.size();
    string::size_type pos1 = data.find( search_b );
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
      pos2 += add;
    }
    _footer = "  " + search_e + data.substr( pos2 );
    *_os << head << endl;
    return true;
  }

  bool Processor::output_footer(){
    if ( _debug ){
      cerr << "Processor::output_footer()" << endl;
    }
    if ( _finished ){
      return true;
    }
    if ( !_os ){
      throw logic_error( "folia::Processor::output_footer() impossible. No output file specified!" );
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

  bool Processor::flush() {
    if ( _debug ){
      cerr << "Processor::flush()" << endl;
    }
    if ( !_os ){
      throw logic_error( "folia::Processor::flush() impossible. No outputfile specified!" );
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

  bool Processor::finish() {
    if ( _debug ){
      cerr << "Processor::finish()" << endl;
    }
    if ( !_os ){
      throw logic_error( "folia::Processor::finish() impossible. No outputfile specified!" );
      return false;
    }
    if ( _finished ){
      return true;
    }
    return output_footer();
  }

  void Processor::save( const string& name, bool do_kanon ){
    if ( _os && name == _out_name ){
      throw logic_error( "folia::Processor::save() impossible. Already connected to a stream withe the same name (" + name + ")" );
    }
    _out_doc->save( name, ns_prefix, do_kanon );
  }

  void Processor::save( ostream& os, bool do_kanon ){
    _out_doc->save( os, ns_prefix, do_kanon );
  }

} // namespace folia
