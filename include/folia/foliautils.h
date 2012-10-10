/*
  $Id$
  $URL$

  Copyright (c) 1998 - 2012
  ILK   - Tilburg University
  CLiPS - University of Antwerp
 
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
      http://ilk.uvt.nl/software.html
  or send mail to:
      timbl@uvt.nl
*/

#ifndef FOLIA_UTILS_H
#define FOLIA_UTILS_H

#include <sstream>
#include <set>
#include <list>
#include <vector>
#include <stdexcept>
#include <ctime>
#include "ticcutils/PrettyPrint.h"
#include "ticcutils/StringOps.h"
#include "unicode/unistr.h"
#include "unicode/unistr.h"
#include <unicode/ustream.h>
#include "libxml/tree.h"
#include "libxml/xpath.h"

namespace folia {
  enum AnnotatorType{ UNDEFINED = -1, AUTO = 0, MANUAL = 1 };
  
  enum Attrib { NO_ATT=0, ID=1, CLASS=2, ANNOTATOR=4, CONFIDENCE=8, 
		N=16, DATETIME=32, ALL=63 };
  
  inline Attrib& operator++( Attrib & a ){
    return a = ( DATETIME == a ) 
      ? NO_ATT
      : ( NO_ATT == a ? ID : Attrib(a<<1) );
  }

  inline Attrib operator|( Attrib a1, Attrib a2 ){
    return (Attrib) ((int)a1|(int)a2) ;
  }
  
  inline Attrib& operator|=( Attrib& a1, Attrib& a2 ){
    a1 = (a1 | a2);
    return a1;
  }
  
  enum ElementType  {
    BASE=0, TextContent_t,
    Text_t, Word_t, 
    WordReference_t, Event_t, 
    TimeSegment_t, TimingLayer_t,
    LineBreak_t, WhiteSpace_t, 
    Sentence_t, Paragraph_t,
    Division_t, Head_t, 
    Caption_t, Label_t,
    List_t, ListItem_t,
    Figure_t, Quote_t, //structure annotation elements
    Pos_t, Lemma_t, 
    Phon_t, Domain_t, 
    Sense_t, Subjectivity_t,
    Metric_t, Correction_t, //token annotation elements
    AnnotationLayer_t, SyntacticUnit_t, 
    Chunk_t, Chunking_t, 
    Entity_t, Entities_t, 
    Coreferences_t, CoreferenceLink_t, 
    CoreferenceChain_t,  SyntaxLayer_t, 
    Semroles_t, Semrole_t,
    //    Subentity_t, Subentities_t, //annotation layers
    Morphology_t, Morpheme_t, 
    ErrorDetection_t, New_t, 
    Original_t, Current_t, 
    Alternative_t, Alternatives_t, //alternatives
    Description_t, Gap_t, 
    Suggestion_t, Content_t, 
    Feature_t, SynsetFeature_t, 
    ActorFeature_t, HeadFeature_t, 
    ValueFeature_t, TimeFeature_t, 
    ModalityFeature_t, LevelFeature_t,
    BeginDateTimeFeature_t, EndDateTimeFeature_t,
    FunctionFeature_t, //features
    PlaceHolder_t,
    Dependencies_t, Dependency_t, 
    Headwords_t, DependencyDependent_t,
    Alignment_t, AlignReference_t,
    LastElement
  };
  
  inline ElementType& operator++( ElementType &et ){
    return et = ( LastElement == et ) 
      ? BASE 
      : ElementType(et+1);
  }
    
  /*
   * Annotation types tie FoLiA elements to a particular kind of annotation.
   * Especially declarations make use of this.
   *  static const annotation_type = {AnnotationType}
   */
  namespace AnnotationType {
    enum AnnotationType { NO_ANN, TEXT, TOKEN, DIVISION, PARAGRAPH, 
			  LIST, FIGURE, WHITESPACE, LINEBREAK, SENTENCE, 
			  POS, LEMMA, DOMEIN, SENSE, SYNTAX, CHUNKING, ENTITY,
			  // Attention DOMAIN seems to be a defined constant
			  // where/how/why?
			  CORRECTION, SUGGESTION, ERRORDETECTION, ALTERNATIVE, 
			  PHON, SUBJECTIVITY, MORPHOLOGICAL, SUBENTITY, EVENT, 
			  DEPENDENCY, TIMEDEVENT, GAP, ALIGNMENT, 
			  COMPLEXALIGNMENT, COREFERENCE, SEMROLE, METRIC,
			  LAST_ANN
    };
    inline AnnotationType& operator++( AnnotationType &at ){
      return at = ( LAST_ANN == at ) 
	? NO_ANN 
	: AnnotationType(at+1);
    }
  }

  enum MetaDataType { NATIVE, CMDI, IMDI };
  
  class ArgsError: public std::runtime_error {
  public:
  ArgsError( const std::string& s ): std::runtime_error( "error in argument list: " + s  ){};
  };
  
  class KeyError: public std::out_of_range {
  public:
  KeyError(): std::out_of_range( "key out of range" ){};
  };
  
  class NotImplementedError: public std::runtime_error {
  public:
  NotImplementedError( const std::string& s ): 
    std::runtime_error( "NOT IMPLEMENTED: " + s ){};
  };
  
  class ValueError: public std::runtime_error {
  public:
  ValueError( const std::string& s ): std::runtime_error( s ){};
  };
  
  class XmlError: public std::runtime_error {
  public:
  XmlError( const std::string& s ): std::runtime_error( "XML error: " + s ){};
  };
  
  class NoSuchAnnotation: public std::runtime_error {
  public:
  NoSuchAnnotation( ): std::runtime_error( "no such annotation" ){};
  NoSuchAnnotation( const std::string& s ): std::runtime_error( "no such annotation: " + s ){};
  };
  
  class NoSuchText: public std::runtime_error {
  public:
  NoSuchText( ): std::runtime_error( "no such text" ){};
  NoSuchText( const std::string& s ): std::runtime_error( "no such text: " + s ){};
  };
  
  class NoDescription: public std::runtime_error {
  public:
  NoDescription( ): std::runtime_error( "no description" ){};
  };
  
  class DuplicateAnnotationError: public std::runtime_error {
  public:
  DuplicateAnnotationError( const std::string& s ): std::runtime_error( s ){};
  };
  
  class DuplicateIDError: public std::runtime_error {
  public:
  DuplicateIDError( const std::string& s ): std::runtime_error( "duplicate ID : " + s ){};
  };
  
  class NoDefaultError: public std::runtime_error {
  public:
  NoDefaultError( const std::string& s ): std::runtime_error( "No Default found: " + s ){};
  };
  
  UnicodeString UTF8ToUnicode( const std::string& );
  std::string UnicodeToUTF8( const UnicodeString& );
  
  double toDouble( const std::string& );
  
  std::string toString( const double );
  
  std::string toString( const AnnotationType::AnnotationType& );
  AnnotatorType stringToANT( const std::string& );
  
  AnnotationType::AnnotationType stringToAT( const std::string& );
  
  std::string toString( const ElementType& );
  ElementType stringToET( const std::string& );
  
  typedef std::map<std::string, std::string> KWargs;
  
  KWargs getArgs( const std::string& );
  std::string toString( const KWargs& );
  
  inline std::ostream& operator<<( std::ostream& os, 
				   const AnnotationType::AnnotationType& at ){
    os << toString( at );
    return os;
  }
  
  template< typename T >
    inline T stringTo( const std::string& str ) {
    T result;
    std::stringstream dummy ( str );
    if ( !( dummy >> result ) ) {
      throw( std::runtime_error( "conversion from string '"
				 + str + "' failed" ) );
    }
    return result;
  }  
  
  template<>
    inline AnnotationType::AnnotationType stringTo( const std::string& str ) {
    return stringToAT( str );
  }

  template<>
    inline ElementType stringTo( const std::string& str ) {
    return stringToET( str );
  }

  template<>
    inline AnnotatorType stringTo( const std::string& str ) {
    std::string at = TiCC::uppercase( str );
    if ( at == "AUTO" )
      return AUTO;
    else if ( at == "MANUAL" )
      return MANUAL;
    else
      return UNDEFINED;
  }
  
  template<>
    inline bool stringTo( const std::string& str ) {
    std::string b = TiCC::uppercase( str );
    if ( b == "YES" || b == "TRUE" || b == "1" )
      return true;
    else if ( b == "FALSE" || b == "NO" || b == "0" )
      return false;
    else
      throw( std::runtime_error( "conversion from string '"
				 + str + "' to bool failed" ) );
  }
  
  xmlNode *newXMLNode( xmlNs *,  const std::string& );
  void addAttributes( xmlNode *, const KWargs& );
  KWargs getAttributes( const xmlNode * );

  std::string Name( const xmlNode * );
  std::string XmlContent( const xmlNode * );
  std::string getNS( const xmlNode *, std::string& );
  inline std::string getNS( const xmlNode *n ) {
    std::string s;
    return getNS( n, s);
  }

  std::map<std::string,std::string> getNSlist( const xmlNode * );

  std::list<xmlNode*> FindNodes( xmlNode *, const std::string& );
  xmlNode *xPath( xmlNode *, const std::string& );

  std::string parseDate( const std::string& );

  bool AT_sanity_check();
  bool Attrib_sanity_check();
  bool ET_sanity_check();

  bool isNCName( const std::string& );

  inline std::string strip( const std::string& s ){ return TiCC::trim(s); };
} // namespace folia

#endif // FOLIA_UTILS
