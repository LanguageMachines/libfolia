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
#include <string>
#include <stdexcept>
#include "ticcutils/PrettyPrint.h"
#include "ticcutils/XMLtools.h"
#include "libfolia/folia.h"

using namespace std;
using TiCC::operator<<;

namespace folia {
  Processor::Processor():
    _in_doc(0),
    _out_doc(0),
    root_node(0),
    current_node(0),
    last_added(0),
    last_depth(2),
    _os(0),
    header_done(false),
    finished(false)
  {
  }

  Processor::~Processor(){
    xmlFreeTextReader( _in_doc );
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
	string att = (const char*)xmlTextReaderName(tr);
	string val = (const char*)xmlTextReaderValue(tr);
	if ( att == "xml:id" ){
	  att = "id";
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
	result[att] = val;
      }
      while ( xmlTextReaderMoveToNextAttribute(tr) );
    }
    return result;
  }

  Document *Processor::init_doc( const string& file_name,
				 const string& out_name ){
    _out_doc = new Document();;
    _in_doc = xmlNewTextReaderFilename( file_name.c_str() );
    if ( _in_doc == 0 ){
      throw( runtime_error( "folia::Processor(), init failed on '" + file_name
			    + "'" ) );
    }
    _os =  new ofstream( out_name );
    int ret = xmlTextReaderRead(_in_doc);
    while ( ret ){
      int type =  xmlTextReaderNodeType(_in_doc );
      string name = (const char*)xmlTextReaderName(_in_doc );
      string local_name = (const char*)xmlTextReaderLocalName(_in_doc );
      //      cerr << "node type = " << type << " name: " << local_name << endl;
      switch ( type ){
      case 1:
	if ( local_name == "FoLiA" ){
	  // found the root
	  string prefix;
	  unsigned char *pnt = xmlTextReaderPrefix(_in_doc);
	  if ( pnt ){
	    prefix = string((const char*)pnt);
	  }
	  string uri = (const char*)xmlTextReaderNamespaceUri(_in_doc);
	  if ( local_name != name ){
	    throw XmlError( "Processor: cannot handle FoLiA with prefix namespace" );
	  }
	  KWargs in_args = get_attributes( _in_doc );
	  string id;
	  if ( !in_args.empty() ){
	    id = in_args["id"];
	  }
	  if ( !id.empty() ){
	    KWargs out_args;
	    out_args["id"] = id;
	    _out_doc->foliadoc = new FoLiA( out_args, _out_doc );
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
	  else if ( type == "native" ){
	    _out_doc->_metadata = new NativeMetaData( type );
	  }
	  else {
	    _out_doc->_metadata = 0;
	  }
	  xmlNode *node = xmlTextReaderExpand(_in_doc);
	  xmlNode *m = node->children;
	  while ( m ){
	    if ( TiCC::Name( m ) == "annotations" ){
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
	else if ( name == "text" ){
	  KWargs args;
	  Text *text = new Text( args, _out_doc );
	  _out_doc->append( text );
	  root_node = text;
	  current_node = text;
	  return _out_doc;
	}
	break;
      case 7:
	// A PI
	if ( name == "xml-stylesheet" ){
	  string sv = (const char*)xmlTextReaderValue(_in_doc);
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
    return _out_doc;
  }

  int Processor::next(){
    return xmlTextReaderNext(_in_doc);
  }

  xmlNode *Processor::get_node( const string& tag ){
    int ret = xmlTextReaderRead(_in_doc);
    while ( ret ){
      int type = xmlTextReaderNodeType(_in_doc);
      if ( type == XML_ELEMENT_NODE ){
	string local_name = (const char*)xmlTextReaderLocalName(_in_doc);
	cerr << "get node name=" << local_name << endl;
	if ( local_name == tag ){
	  return xmlTextReaderExpand(_in_doc);
	}
	else {
	  int new_depth = xmlTextReaderDepth(_in_doc);
	  cerr << "last depth=" << last_depth << " new=" << new_depth << endl;
	  string name = (const char*)xmlTextReaderName(_in_doc);
	  KWargs atts = get_attributes( _in_doc );
	  cerr << "name=" << name << " atts=" << toString(atts) << endl;
	  if ( name == "foreign-data" ){
	    cerr << "ADD FOREOIGN! "<< endl;
	    FoliaElement *t = FoliaImpl::createElement( name, _out_doc );
	    xmlNode *fd = xmlTextReaderExpand(_in_doc);
	    t->parseXml( fd );
	    if ( new_depth == last_depth ){
	      cerr << "GELIJK: current node = " << current_node << endl;
	      cerr << "last node = " << last_added << endl;
	      current_node->append( t );
	    }
	    else if ( new_depth > last_depth ){
	      cerr << "DIEPER: current node = " << current_node << endl;
	      current_node = last_added;
	      cerr << "Dus nu: current node = " << current_node << endl;
	      current_node->append( t );
	      last_depth = new_depth;
	    }
	    else if ( new_depth < last_depth  ){
	      cerr << "OMHOOG current node = " << current_node << endl;
	      cerr << "last node = " << last_added << endl;
	      for ( int i=0; i < last_depth-new_depth; ++i ){
		cerr << "last node = " << last_added << endl;
		current_node = current_node->parent();
	      }
	      cerr << "NU current node = " << current_node << endl;
	      current_node->append( t );
	      last_depth = new_depth;
	    }
	    xmlTextReaderNext(_in_doc);
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
	      FoliaElement *t = FoliaImpl::createElement( name, _out_doc );
	      cerr << "created name=" << name << endl;
	      if ( name == "desc"
		   || name == "comment" ){
		ret = xmlTextReaderRead(_in_doc);
		const char *val = (const char*)xmlTextReaderValue(_in_doc);
		if ( val ) {
		  string value = TiCC::trim(val);
		  if ( !value.empty() ) {
		    cerr << "Node VALUE = '" << value << "'" << endl;
		    atts["value"] = value;
		  }
		}
	      }
	      t->setAttributes( atts );
	      if ( new_depth == last_depth ){
		cerr << "GELIJK: current node = " << current_node << endl;
		cerr << "last node = " << last_added << endl;
		current_node->append( t );
	      }
	      else if ( new_depth > last_depth ){
		cerr << "DIEPER: current node = " << current_node << endl;
		current_node = last_added;
		cerr << "Dus nu: current node = " << current_node << endl;
		current_node->append( t );
		last_depth = new_depth;
	      }
	      else if ( new_depth < last_depth  ){
		cerr << "OMHOOG current node = " << current_node << endl;
		cerr << "last node = " << last_added << endl;
		for ( int i=0; i < last_depth-new_depth; ++i ){
		  cerr << "last node = " << last_added << endl;
		  current_node = current_node->parent();
		}
		cerr << "NU current node = " << current_node << endl;
		current_node->append( t );
		last_depth = new_depth;
	      }
	      last_added = t;
	      cerr << "einde current node = " << current_node << endl;
	      cerr << "last node = " << last_added << endl;
	    }
	    else {
	      // just ignore!
	      cerr << "ignore this node: '" << nsu << "'" << endl;
	      xmlTextReaderNext(_in_doc);
	    }
	  }
	}
      }
      else if ( type == XML_TEXT_NODE ){
	XmlText *txt = new XmlText();
	string value = TiCC::trim((const char*)xmlTextReaderValue(_in_doc));
	cerr << "TXT VALUE = '" << value << "'" << endl;
	if ( value.empty() ) {
	  txt->setvalue( value );
	}
	int new_depth = xmlTextReaderDepth(_in_doc);
	cerr << "TEXT last depth=" << last_depth << " new=" << new_depth << endl;
	if ( last_depth == new_depth ){
	  current_node->append( txt );
	}
	else if ( last_depth > new_depth ){
	  current_node = current_node->parent();
	  current_node->append( txt );
	}
	else {
	  current_node = last_added;
	  current_node->append( txt );
	}
	last_added = txt;
	cerr << "einde current node = " << current_node << endl;
	cerr << "last node = " << last_added << endl;
	last_depth = xmlTextReaderDepth(_in_doc);
      }
      else if ( type == XML_COMMENT_NODE ){
	cerr << "SKIP comment!" << endl;
      }
      ret = xmlTextReaderRead(_in_doc);
    }
    return 0;
  }

  bool Processor::add( FoliaElement *el ){
    try {
      root_node->append( el );
    }
    catch ( exception& e ){
      cerr << "folia::Processor(): " << e.what() << endl;
      return false;
    }
    return true;
  }

  bool Processor::output_header(){
    cerr << "Processor::output_header()" << endl;
    if ( finished ){
      return true;
    }
    else if ( header_done ){
      cerr << "folia::Processor(): output_header is called twice!" << endl;
      return false;
    }
    header_done = true;
    stringstream ss;
    _out_doc->save( ss );
    string data = ss.str();
    string::size_type pos1 = data.find("<text");
    string::size_type pos2;
    if ( root_node->size() == 0 ){
      pos2 = data.find( "/>" , pos1 );
    }
    else {
      pos2 = data.find( ">" , pos1 );
    }
    string head = data.substr( 0, pos2 ) + ">";
    if ( root_node->size() == 0 ){
      pos2 += 2;
    }
    else {
      pos2 = data.find( "</text>" , pos1 );
      pos2 += 6;
    }
    _footer = "  </text>" + data.substr( pos2 );
    *_os << head << endl;
    return true;
  }

  bool Processor::output_footer(){
    cerr << "Processor::output_footer()" << endl;
    if ( finished ){
      return true;
    }
    else if ( flush() ){
      *_os << _footer << endl;
      finished = true;
      return true;
    }
    else {
      return false;
    }
  }

  bool Processor::flush() {
    cerr << "Processor::flush()" << endl;
    if ( finished ){
      return true;
    }
    else if ( !header_done ){
      output_header();
    }
    size_t len = root_node->size();
    for ( size_t i=0; i < len; ++i ){
      *_os << "    " << root_node->index(i)->xmlstring(true,2) << endl;
    }
    for ( size_t i=0; i < len; ++i ){
      root_node->remove( i, true );
    }
    return true;
  }

  bool Processor::finish() {
    cerr << "Processor::finish()" << endl;
    if ( finished ){
      return true;
    }
    return output_footer();
  }

} // namespace folia
