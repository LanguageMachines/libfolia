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
#include "libfolia/folia.h"

using namespace std;

namespace folia {

  Reader::Reader( const std::string& file_name, const std::string& id )
  {
    _TR = xmlNewTextReaderFilename( file_name.c_str() );
    if ( _TR == 0 ){
      throw( runtime_error( "folia::Reader(), init failed on '" + file_name
			    + "'" ) );
    }
  }

  xmlNode *Reader::get_node( const string& tag ){
    int ret = xmlTextReaderRead(_TR);
    while ( ret ){
      string local_name = (const char*)xmlTextReaderLocalName(_TR);
      if ( local_name == tag
	   && xmlTextReaderNodeType(_TR) == 1 ){
	return xmlTextReaderExpand(_TR);
      }
      ret = xmlTextReaderRead(_TR);
    }
    return 0;
  }

  Reader::~Reader(){
    xmlFreeTextReader( _TR );
  }

} // namespace folia
