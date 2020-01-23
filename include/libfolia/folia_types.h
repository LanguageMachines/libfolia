/*
  Copyright (c) 2006 - 2020
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

#ifndef TYPES_H
#define TYPES_H
#include <string>

namespace folia {

  //foliaspec:elementtype
  enum ElementType : unsigned int { BASE=0,AbstractAnnotationLayer_t, AbstractContentAnnotation_t, AbstractCorrectionChild_t, AbstractHigherOrderAnnotation_t, AbstractInlineAnnotation_t, AbstractSpanAnnotation_t, AbstractSpanRole_t, AbstractStructureElement_t, AbstractSubtokenAnnotation_t, AbstractTextMarkup_t, ActorFeature_t, Alternative_t, AlternativeLayers_t, BegindatetimeFeature_t, Caption_t, Cell_t, Chunk_t, ChunkingLayer_t, Comment_t, Content_t, CoreferenceChain_t, CoreferenceLayer_t, CoreferenceLink_t, Correction_t, Current_t, Definition_t, DependenciesLayer_t, Dependency_t, DependencyDependent_t, Description_t, Division_t, DomainAnnotation_t, EnddatetimeFeature_t, EntitiesLayer_t, Entity_t, Entry_t, ErrorDetection_t, Event_t, Example_t, External_t, Feature_t, Figure_t, ForeignData_t, FunctionFeature_t, Gap_t, Head_t, HeadFeature_t, Headspan_t, Hiddenword_t, Hyphbreak_t, Label_t, LangAnnotation_t, LemmaAnnotation_t, LevelFeature_t, Linebreak_t, LinkReference_t, List_t, ListItem_t, Metric_t, ModalityFeature_t, Morpheme_t, MorphologyLayer_t, New_t, Note_t, Observation_t, ObservationLayer_t, Original_t, Paragraph_t, Part_t, PhonContent_t, Phoneme_t, PhonologyLayer_t, PolarityFeature_t, PosAnnotation_t, Predicate_t, Quote_t, Reference_t, Relation_t, Row_t, SemanticRole_t, SemanticRolesLayer_t, SenseAnnotation_t, Sentence_t, Sentiment_t, SentimentLayer_t, Source_t, SpanRelation_t, SpanRelationLayer_t, Speech_t, Statement_t, StatementLayer_t, StatementRelation_t, StrengthFeature_t, String_t, StyleFeature_t, SubjectivityAnnotation_t, Suggestion_t, SynsetFeature_t, SyntacticUnit_t, SyntaxLayer_t, Table_t, TableHead_t, Target_t, Term_t, Text_t, TextContent_t, TextMarkupCorrection_t, TextMarkupError_t, TextMarkupGap_t, TextMarkupReference_t, TextMarkupString_t, TextMarkupStyle_t, TimeFeature_t, TimeSegment_t, TimingLayer_t, Utterance_t, ValueFeature_t, Whitespace_t, Word_t, WordReference_t, PlaceHolder_t, XmlComment_t, XmlText_t,  LastElement };

  inline ElementType& operator++( ElementType &et ){
    return et = ( LastElement == et )
      ? BASE
      : ElementType(et+1);
  }

  /** AnnotatorType is the Internal representation of the Annatator attribute
   *
   */
  enum AnnotatorType: int {
    UNDEFINED = 0,   ///< The value is (yet) unknown
      AUTO = 1,      ///< The value is automaticly assigned
      MANUAL = 2,    ///< The value is manually assigned
      GENERATOR = 3, ///< The value is added by a generator
      DATASOURCE = 4 ///< The value comes from an external datasource
      };

  //foliaspec:attributes
  //Defines all common FoLiA attributes (as part of the Attrib enumeration)
  enum Attrib : int { NO_ATT=0, ID=1, CLASS=2, ANNOTATOR=4, CONFIDENCE=8, N=16, DATETIME=32, BEGINTIME=64, ENDTIME=128, SRC=256, SPEAKER=512, TEXTCLASS=1024, METADATA=2048, IDREF=4096, SPACE=8192, ALL=16384 };

  inline Attrib& operator++( Attrib & a ){
    return a = ( METADATA < a )
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

  std::string toString( const Attrib );
  std::ostream& operator<<( std::ostream&, const Attrib& );

  //#undef DOMAIN // ugly hack but GCC screws up

  /*
   * Annotation types tie FoLiA elements to a particular kind of annotation.
   * Especially declarations make use of this.
   *  static const annotation_type = {AnnotationType}
   */
  namespace AnnotationType {

    //foliaspec:annotationtype
    //Defines all annotation types (as part of the AnnotationType enumeration)
    enum AnnotationType : int { NO_ANN,TEXT, TOKEN, DIVISION, PARAGRAPH, HEAD, LIST, FIGURE, WHITESPACE, LINEBREAK, SENTENCE, POS, LEMMA, DOMAIN, SENSE, SYNTAX, CHUNKING, ENTITY, CORRECTION, ERRORDETECTION, PHON, SUBJECTIVITY, MORPHOLOGICAL, EVENT, DEPENDENCY, TIMESEGMENT, GAP, QUOTE, NOTE, REFERENCE, RELATION, SPANRELATION, COREFERENCE, SEMROLE, METRIC, LANG, STRING, TABLE, STYLE, PART, UTTERANCE, ENTRY, TERM, DEFINITION, EXAMPLE, PHONOLOGICAL, PREDICATE, OBSERVATION, SENTIMENT, STATEMENT, ALTERNATIVE, RAWCONTENT, COMMENT, DESCRIPTION, HYPHENATION, HIDDENTOKEN, LAST_ANN };

    inline AnnotationType& operator++( AnnotationType &at ){
      return at = ( LAST_ANN == at )
	? NO_ANN
	: AnnotationType(at+1);
    }
  }

}
#endif
