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
#include "ticcutils/StringOps.h"
#include "libfolia/folia.h"

using namespace std;

namespace folia {

  Reader::Reader( std::ostream& os, const std::string& id ):
    _os(os),
    _id(id),
    header_done(false),
    finished(false)
  {
  }

  Reader::~Reader(){
    output_footer();
  }

  bool Reader::add( FoliaElement *el ){
    try {
      root_node->append( el );
    }
    catch ( exception& e ){
      cerr << "folia::Reader(): " << e.what() << endl;
      return false;
    }
    return true;
  }

  bool Reader::output_header(){
    if ( finished ){
      cerr << "folia::Reader(): output_header is called on finished reader." << endl;
      return false;
    }
    else if ( header_done ){
      cerr << "folia::Reader(): output_header is called twice!" << endl;
      return false;
    }
    header_done = true;
    _os << "HEADER" << endl;
    return true;
  }

  bool Reader::output_footer(){
    if ( finished ){
      cerr << "folia::Reader(): output_footer is called on finished reader." << endl;
      return false;
    }
    else if ( flush() ){
      _os << "FOOTER" << endl;
      finished = true;
      return true;
    }
    else {
      return false;
    }
  }

  bool Reader::flush() {
    if ( finished ){
      cerr << "folia::Reader(): flush() is called on finished reader." << endl;
      return false;
    }
    else if ( !header_done ){
      output_header();
    }
    _os << root_node->xmlstring() << endl;
    delete root_node;
    root_node = 0;
    return true;
  }

} // namespace folia
