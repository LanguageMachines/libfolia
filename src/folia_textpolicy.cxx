
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

#include <string>
#include <map>
#include <iostream>
#include "libfolia/folia_textpolicy.h"

using namespace std;

namespace folia {

  TextPolicy::TextPolicy( const string& cls, const TEXT_FLAGS flags ):
    _class(cls),
    _text_flags( flags )
  {
  }

  TextPolicy::TextPolicy( const TEXT_FLAGS flags ):
    TextPolicy( "current", flags ) {
  }


  ostream& operator<<( ostream& os, const TextPolicy& tp ){
    bool retain  = tp.is_set( TEXT_FLAGS::RETAIN );
    bool strict  = tp.is_set( TEXT_FLAGS::STRICT );
    bool hide    = tp.is_set( TEXT_FLAGS::HIDDEN );
    bool trim = !tp.is_set( TEXT_FLAGS::NO_TRIM_SPACES );
    os << (strict?"strict":"not strict") << "\t"
       << (retain?"retain":"untokenized") << "\t"
       << (hide?"show_hidden":"hide hidden") << "\t"
       << (trim?"trimming spaces":"not trimming spaces");
    return os;
  }

  bool TextPolicy::is_set( TEXT_FLAGS tf ) const {
    return ( tf & _text_flags ) == tf;
  }

  void TextPolicy::set( TEXT_FLAGS tf ) {
    _text_flags |= tf;
  }

  void TextPolicy::clear( TEXT_FLAGS tf ) {
    _text_flags &= ~tf;
  }

  void TextPolicy::add_handler( const string& label,
				const tag_handler& sfp ){
    _tag_handlers.insert( make_pair( label, sfp ) );
  }

  const TextPolicy::tag_handler TextPolicy::remove_handler( const string& label ){
    auto pnt = _tag_handlers.find( label );
    if ( pnt != _tag_handlers.end() ){
      _tag_handlers.erase( pnt );
      return pnt->second;
    }
    else {
      return 0;
    }
  }

  const TextPolicy::tag_handler TextPolicy::get_handler( const string& label ) const{
    auto pnt = _tag_handlers.find( label );
    if ( pnt != _tag_handlers.end() ){
      return pnt->second;
    }
    else {
      return 0;
    }
  }

} // namespace folia
