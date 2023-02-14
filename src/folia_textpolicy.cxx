
/*
  Copyright (c) 2006 - 2023
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

  /// create a TextPolicy object
  /*!
    \param cls a string representing a text-class
    \param flags the TEXT_FLAGS settings to use
  */
  TextPolicy::TextPolicy( const string& cls, const TEXT_FLAGS flags ):
    _class(cls),
    _text_flags( flags ),
    _correction_handling( CORRECTION_HANDLING::CURRENT ),
    _debug(false)
  {
  }

  /// create a TextPolicy object for the 'current' text-class
  /*!
    \param flags the TEXT_FLAGS settings to use
  */
  TextPolicy::TextPolicy( const TEXT_FLAGS flags ):
    TextPolicy( "current", flags ) {
  }

  string toString( CORRECTION_HANDLING ch ){
    /// give a text representation for a CORRECION_HANDLING
    /*!
      \param ch the CORRECTION_HANDLING
      \return a string representing the CORRECTION_HANDLING
    */
    switch( ch ){
    case CORRECTION_HANDLING::CURRENT:
      return "current";
      break;
    case CORRECTION_HANDLING::ORIGINAL:
      return "original";
      break;
    case CORRECTION_HANDLING::EITHER:
      return "either";
      break;
    default:
      return "CORRECTION_HANDLING.toString() NOT implemented case";
    }
  }

  ostream& operator<<( ostream& os, const TextPolicy& tp ){
    /// Output a TextPolicy (for debugging purposes)
    /*!
      \param os the output stream
      \param tp the TextPolicy
      \return the outputstream after the \em tp is output.
    */
    bool retain  = tp.is_set( TEXT_FLAGS::RETAIN );
    bool strict  = tp.is_set( TEXT_FLAGS::STRICT );
    bool hide    = tp.is_set( TEXT_FLAGS::HIDDEN );
    bool trim = !tp.is_set( TEXT_FLAGS::NO_TRIM_SPACES );
    bool add_format = tp.is_set( TEXT_FLAGS::ADD_FORMATTING );
    os << "class=" << tp.get_class() << "\t"
       << (strict?"strict":"not strict") << ", "
       << (retain?"retain":"untokenized") << ", "
       << (add_format?"add_format":"no format") << ", "
       << (hide?"show_hidden":"hide hidden") << ", "
       << (trim?"trimming spaces":"not trimming spaces") << ", "
       << "cor_handling: " << toString( tp.get_correction_handling() );
    return os;
  }

  bool TextPolicy::is_set( TEXT_FLAGS tf ) const {
    /// check is the flag is set
    /*!
      \param tf the TEXT_FLAGS flag to test
      \return \em true when set, \em false otherwise
    */
    return ( tf & _text_flags ) == tf;
  }

  void TextPolicy::set( TEXT_FLAGS tf ) {
    /// set a flag
    /*!
      \param tf the TEXT_FLAGS flag to set
    */
    _text_flags |= tf;
  }

  void TextPolicy::clear( TEXT_FLAGS tf ) {
    /// clear a flag
    /*!
      \param tf the TEXT_FLAGS flag to clear (unset)
    */
    _text_flags &= ~tf;
  }

  void TextPolicy::add_handler( const string& label,
				const tag_handler& fp ){
    /// add a tag_handler to the currect handlers
    /*!
      \param label a label to identify the handler
      \param fp the function to register

      may override an existing handler, if the label is already in use.
    */
    _tag_handlers.insert( make_pair( label, fp ) );
  }

  const TextPolicy::tag_handler TextPolicy::remove_handler( const string& label ){
    /// remove a tag_handler
    /*!
      \param label the label to identify the handler
      \return the function which is removed. Or 0 when the label didn't match
    */
    auto pnt = _tag_handlers.find( label );
    if ( pnt != _tag_handlers.end() ){
      auto ret_val = pnt->second;
      _tag_handlers.erase( pnt );
      return ret_val;
    }
    else {
      return 0;
    }
  }

  const TextPolicy::tag_handler TextPolicy::get_handler( const string& label ) const{
    /// return a tag_handler for a give label
    /*!
      \param label the label to identify the handler
      \return the function which is found. Or 0 when the label didn't match
    */
    auto pnt = _tag_handlers.find( label );
    if ( pnt != _tag_handlers.end() ){
      return pnt->second;
    }
    else {
      return 0;
    }
  }

} // namespace folia
