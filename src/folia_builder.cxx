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
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include "ticcutils/PrettyPrint.h"
#include "ticcutils/StringOps.h"
#include "libfolia/folia.h"

using namespace std;

namespace folia {

  Builder::Builder( std::ostream& os, const std::string& id, doctype dt ):
    _doc_type( dt ),
    _os(os),
    header_done(false),
    finished(false)
  {
    KWargs args;
    args["id"] = id;
    _doc = new Document(args);
    FoliaElement *t;
    if ( dt == TEXT ){
      args["id"] = id + ".text";
      t = new Text( args );
    }
    else {
      args["id"] = id + ".speech";
      t = new Speech( args );
    }
    _doc->setRoot( t );
    root_node = t;
  }

  Builder::~Builder(){
    output_footer();
    delete _doc;
  }

  bool Builder::add( FoliaElement *el ){
    try {
      root_node->append( el );
    }
    catch ( exception& e ){
      cerr << "folia::Builder(): " << e.what() << endl;
      return false;
    }
    return true;
  }

  bool Builder::output_header(){
    if ( finished ){
      return true;
    }
    else if ( header_done ){
      cerr << "folia::Builder(): output_header is called twice!" << endl;
      return false;
    }
    header_done = true;
    stringstream ss;
    _doc->save( ss );
    string data = ss.str();
    string::size_type pos1;
    if ( _doc_type == TEXT ){
      pos1 = data.find("<text");
    }
    else {
      pos1 = data.find("<speech");
    }
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
      if ( _doc_type == TEXT ){
	pos2 = data.find( "</text>" , pos1 );
	pos2 += 6;
      }
      else {
	pos2 = data.find( "</speech>" , pos1 );
	pos2 += 8;
      }
    }
    if ( _doc_type == TEXT ){
      _footer = "  </text>";
    }
    else {
      _footer = "  </speech>";
    }
    _footer += data.substr( pos2 );
    _os << head << endl;
    return true;
  }

  bool Builder::output_footer(){
    if ( finished ){
      return true;
    }
    else if ( flush() ){
      _os << _footer << endl;
      finished = true;
      return true;
    }
    else {
      return false;
    }
  }

  bool Builder::flush() {
    if ( finished ){
      return true;
    }
    else if ( !header_done ){
      output_header();
    }
    size_t len = root_node->size();
    for ( size_t i=0; i < len; ++i ){
      _os << "    " << root_node->index(i)->xmlstring(true,2) << endl;
    }
    for ( size_t i=0; i < len; ++i ){
      root_node->remove( i, true );
    }
    return true;
  }

  bool Builder::finish() {
    if ( finished ){
      return true;
    }
    return output_footer();
  }
} // namespace folia
