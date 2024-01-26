/*
  Copyright (c) 2006 - 2024
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

  string toString( const AnnotationType& at ){
    /// return the string representation of AnnotationType at
    auto result = ant_s_map.find(at);
    if ( result == ant_s_map.end() ){
      throw logic_error( "Unknown Annotationtype: index= "
			 + TiCC::toString(int(at)) );
    };
    return result->second;
  }

  AnnotationType stringToAnnotationType( const string& st ){
    // convert a string into an AnnotationType
    /*!
     * \param st a string representing an AnnotationType
     * \return an AnnotationType. Throws when not found.
     *
     * Also handles 'old' pre v1.5 names.
     */
    string s = st;
    auto const tr = oldtags.find(st);
    if ( tr != oldtags.end() ){
      s = tr->second;
    }
    auto result = s_ant_map.find( s );
    if ( result == s_ant_map.end() ){
      throw logic_error( "Unknown translation for annotationtype: '"
			 + s + "'" );
    }
    return result->second;
  }

  AnnotatorType stringToAnnotatorType( const string& str ){
    // convert a string into an AnnotatorType
    /*!
     * \param str a (case insensitive) string representation
     * \return an AnnotatorType
     */
    string at = TiCC::uppercase( str );
    if ( at == "AUTO" ){
      return folia::AUTO;
    }
    else if ( at == "MANUAL" ){
      return folia::MANUAL;
    }
    else if ( at == "GENERATOR" ){
      return folia::GENERATOR;
    }
    else if ( at == "DATASOURCE" ){
      return folia::DATASOURCE;
    }
    else {
      return folia::UNDEFINED;
    }
  }

  string toString( const AnnotatorType& at ){
    /// return the string representation of AnnotatorType at
    switch ( at ){
    case AUTO:
      return "auto";
      break;
    case MANUAL:
      return "manual";
      break;
    case GENERATOR:
      return "generator";
      break;
    case DATASOURCE:
      return "datasource";
      break;
    default:
      return "UNDEFINED";
    }
  }

  string toString( const ElementType& et ) {
    /// return the string representation of ElementType et
    /*!
     * \param et an ElementType
     * \return a string representation.
     */
    auto result = et_s_map.find(et);
    if ( result == et_s_map.end() ){
      throw logic_error( "toString: Unknown Elementtype "
			 + TiCC::toString( int(et) ) );
    }
    return result->second;
  }

  ElementType stringToElementType( const string& intag ){
    // convert a string into an ElementType
    /*!
     * \param intag a string representing an ElementType
     * \return an ElementType. Throws when not found.
     *
     * Also handles 'old' pre v1.5 names.
     */
    string tag = intag;
    auto const tr = oldtags.find(intag);
    if ( tr != oldtags.end() ){
      tag = tr->second;
    }
    auto result = s_et_map.find(tag);
    if ( result == s_et_map.end() ){
      throw ValueError( "unknown tag <" + tag + ">" );
    }
    return result->second;
  }

  string toString( const Attrib at ){
    /// return the string representation of Attrib at
    switch ( at ){
    case ID:
      return "ID";
      break;
    case CLASS:
      return "CLASS";
      break;
    case ANNOTATOR:
      return "ANNOTATOR";
      break;
    case CONFIDENCE:
      return "CONFIDENCE";
      break;
    case N:
      return "N";
      break;
    case DATETIME:
      return "DATETIME";
      break;
    case BEGINTIME:
      return "BEGINTIME";
      break;
    case ENDTIME:
      return "ENDTIME";
      break;
    case SRC:
      return "SRC";
      break;
    case SPEAKER:
      return "SPEAKER";
      break;
    case TEXTCLASS:
      return "TEXTCLASS";
      break;
    case METADATA:
      return "METADATA";
      break;
    default:
      return "";
    }
  }

  ostream& operator<<( ostream& os, const Attrib& attribs ){
    /// Output operator for an Attrib element
    /*!
     * \param os the output stream
     * \param attribs an Attrib.
     * An Attrib contains a logical OR of serveral values.
     * This function outputs a string value for every value set.
     * separated by '|'
     */
    enum Attrib at = NO_ATT;
    while ( at != ALL ){
      if ( attribs & at ){
	os << toString( at ) << "|";
      }
      ++at;
    }
    return os;
  }

  ElementType layertypeof( ElementType et ) {
    /// return the Layer ElemenType of the given ElementType
    /*!
     * \param et the ElementType to check
     * \return The Layer ElementType er belongs to. Or BASE if it has no Layer.
     */
    switch( et ) {
    case Entity_t:
    case EntitiesLayer_t:
      return EntitiesLayer_t;
    case Chunk_t:
    case ChunkingLayer_t:
      return ChunkingLayer_t;
    case SyntacticUnit_t:
    case SyntaxLayer_t:
      return SyntaxLayer_t;
    case TimeSegment_t:
    case TimingLayer_t:
      return TimingLayer_t;
    case Morpheme_t:
    case MorphologyLayer_t:
      return MorphologyLayer_t;
    case Phoneme_t:
    case PhonologyLayer_t:
      return PhonologyLayer_t;
    case CoreferenceChain_t:
    case CoreferenceLayer_t:
      return CoreferenceLayer_t;
    case Observation_t:
    case ObservationLayer_t:
      return ObservationLayer_t;
    // case Predicate_t:
    // case PredicateLayer_t:
    //   return PredicateLayer_t;
    case SentimentLayer_t:
    case Sentiment_t:
      return SentimentLayer_t;
    case StatementLayer_t:
    case Statement_t:
      return SentimentLayer_t;
    case SemanticRolesLayer_t:
    case SemanticRole_t:
      return SemanticRolesLayer_t;
    case DependenciesLayer_t:
    case Dependency_t:
      return DependenciesLayer_t;
    default:
      return BASE;
    }
  }

} // namespace folia
