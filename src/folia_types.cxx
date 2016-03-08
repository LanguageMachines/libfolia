/*
  Copyright (c) 2006 - 2016
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

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>
#include "ticcutils/StringOps.h"
#include "libfolia/folia_properties.h"
#include "libfolia/folia.h"

using namespace std;

namespace folia {

  string toString( const AnnotationType::AnnotationType& at ){
    auto result = ant_s_map.find(at);
    if ( result == ant_s_map.end() ){
      throw logic_error( "Unknown Annotationtype: index= "
			 + TiCC::toString(int(at)) );
    };
    return result->second;
  }

  AnnotationType::AnnotationType stringToAT( const string& st ){
    string s = st;
    if ( s == "div" ){
      s = "division";  // fix for old (in fact invalid) FoLiA
    }
    auto result = s_ant_map.find( s );
    if ( result == s_ant_map.end() ){
      throw logic_error( "Unknown translation for annotationtype: '"
			 + s + "'" );
    }
    return result->second;
  }

  AnnotatorType stringToANT( const string& str ){
    string at = TiCC::uppercase( str );
    if ( at == "AUTO" )
      return folia::AUTO;
    else if ( at == "MANUAL" )
      return folia::MANUAL;
    else
      return folia::UNDEFINED;
  }

  string toString( const ElementType& et ) {
    auto result = et_s_map.find(et);
    if ( result == et_s_map.end() ){
      throw logic_error( "toString: Unknown Elementtype "
			 + TiCC::toString( int(et) ) );
    }
    return result->second;
  }

  ElementType stringToET( const string& intag ){
    string tag = intag;
    if ( tag == "listitem" ){ // erroneous in older FoLiA docs
      tag = "item";
    }
    auto result = s_et_map.find(tag);
    if ( result == s_et_map.end() ){
      throw ValueError( "unknown tag <" + tag + ">" );
    }
    return result->second;
  }
}
