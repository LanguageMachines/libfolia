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

#ifndef TYPES_H
#define TYPES_H

namespace folia {

  //foliaspec:elementtype
  enum ElementType : unsigned int {
    BASE=0,
      TextContent_t,
      Text_t,
      Speech_t,
      Word_t,
      String_t,
      Utterance_t,
      Entry_t,
      Example_t,
      PhonContent_t,
      Term_t,
      Definition_t,
      WordReference_t,
      Event_t,
      TimeSegment_t,
      TimingLayer_t,
      LineBreak_t,
      WhiteSpace_t,
      Sentence_t,
      Paragraph_t,
      Division_t,
      Head_t,
      Caption_t,
      Label_t,
      List_t,
      Item_t,
      Figure_t,
      Quote_t,
      PosAnnotation_t,
      LemmaAnnotation_t,
      PhonologyLayer_t,
      Phoneme_t,
      DomainAnnotation_t,
      SenseAnnotation_t,
      SubjectivityAnnotation_t,
      Metric_t,
      Correction_t,
      AbstractAnnotationLayer_t,
      AbstractSpanAnnotation_t,
      AbstractSpanRole_t,
      SyntacticUnit_t,
      Chunk_t,
      ChunkingLayer_t,
      Entity_t,
      EntitiesLayer_t,
      CoreferenceLayer_t,
      CoreferenceLink_t,
      CoreferenceChain_t,
      SyntaxLayer_t,
      SemanticRolesLayer_t,
      SemanticRole_t,
      MorphologyLayer_t,
      Morpheme_t,
      ErrorDetection_t,
      New_t,
      Original_t,
      Current_t,
      Alternative_t,
      AlternativeLayers_t,
      Description_t,
      Gap_t,
      Suggestion_t,
      Content_t,
      Feature_t,
      SynsetFeature_t,
      ActorFeature_t,
      HeadFeature_t,
      ValueFeature_t,
      TimeFeature_t,
      ModalityFeature_t,
      LevelFeature_t,
      BeginDateTimeFeature_t,
      EndDateTimeFeature_t,
      FunctionFeature_t,
      PlaceHolder_t,
      DependenciesLayer_t,
      Dependency_t,
      Headspan_t,
      DependencyDependent_t,
      ComplexAlignment_t,
      ComplexAlignmentLayer_t,
      Alignment_t,
      AlignReference_t,
      Table_t,
      TableHead_t,
      Row_t,
      Cell_t,
      LangAnnotation_t,
      XmlComment_t,
      AbstractCorrectionChild_t,
      AbstractTokenAnnotation_t,
      AbstractStructureElement_t,
      AbstractTextMarkup_t,
      TextMarkupString_t,
      TextMarkupGap_t,
      TextMarkupCorrection_t,
      TextMarkupError_t,
      TextMarkupStyle_t,
      XmlText_t,
      External_t,
      Note_t,
      Reference_t,
      Part_t,
      LastElement
      };

  inline ElementType& operator++( ElementType &et ){
    return et = ( LastElement == et )
      ? BASE
      : ElementType(et+1);
  }


  enum AnnotatorType: int { UNDEFINED = -1, AUTO = 0, MANUAL = 1 };

  //foliaspec:attributes
  enum Attrib : int { NO_ATT=0, ID=1, CLASS=2, ANNOTATOR=4, CONFIDENCE=8,
      N=16, DATETIME=32, SETONLY=64, BEGINTIME=128, ENDTIME=256,
      SRC=512, SPEAKER=1024, ALL=2047 };

  inline Attrib& operator++( Attrib & a ){
    return a = ( SPEAKER == a )
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

#undef DOMAIN // ugly hack but GCC screws up

  /*
   * Annotation types tie FoLiA elements to a particular kind of annotation.
   * Especially declarations make use of this.
   *  static const annotation_type = {AnnotationType}
   */
  namespace AnnotationType {

    //foliaspec:annotationtype
    enum AnnotationType : int { NO_ANN, TEXT, TOKEN, DIVISION, PARAGRAPH,
	LIST, FIGURE, WHITESPACE, LINEBREAK, SENTENCE,
	POS, LEMMA, DOMAIN, SENSE, SYNTAX, CHUNKING, ENTITY,
	CORRECTION, SUGGESTION, ERRORDETECTION, ALTERNATIVE,
	PHON, SUBJECTIVITY, MORPHOLOGICAL, EVENT,
	DEPENDENCY, TIMESEGMENT, GAP, NOTE, ALIGNMENT,
	COMPLEXALIGNMENT, COREFERENCE, SEMROLE, METRIC,
	LANG, STRING, TABLE, STYLE, PART, UTTERANCE, TERM,
	DEFINITION, EXAMPLE, PHONOLOGICAL,
	LAST_ANN
	};

    inline AnnotationType& operator++( AnnotationType &at ){
      return at = ( LAST_ANN == at )
	? NO_ANN
	: AnnotationType(at+1);
    }
  }

  enum MetaDataType { NATIVE, CMDI, IMDI };

}
#endif
