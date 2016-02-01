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
#include "libfolia/foliatypes.h"
#include "libfolia/foliautils.h"

using namespace std;

namespace folia {

  string toString( const AnnotationType::AnnotationType& at ){
    string result;
    switch ( at ) {
    case AnnotationType::NO_ANN:
      result = "NoNe";
      break;
    case AnnotationType::TEXT:
      result = "text";
      break;
    case AnnotationType::STRING:
      result = "string";
      break;
    case AnnotationType::TOKEN:
      result = "token";
      break;
    case AnnotationType::DIVISION:
      result = "division"; //adapted from erroneous div
      break;
    case AnnotationType::PARAGRAPH:
      result = "paragraph";
      break;
    case AnnotationType::LIST:
      result = "list";
      break;
    case AnnotationType::FIGURE:
      result = "figure";
      break;
    case AnnotationType::WHITESPACE:
      result = "whitespace";
      break;
    case AnnotationType::LINEBREAK:
      result = "linebreak";
      break;
    case AnnotationType::SENTENCE:
      result = "sentence";
      break;
    case AnnotationType::POS:
      result = "pos";
      break;
    case AnnotationType::LEMMA:
      result = "lemma";
      break;
    case AnnotationType::DOMEIN:
      result = "domain";
      break;
    case AnnotationType::SENSE:
      result = "sense";
      break;
    case AnnotationType::SYNTAX:
      result = "syntax";
      break;
    case AnnotationType::CHUNKING:
      result = "chunking";
      break;
    case AnnotationType::ENTITY:
      result = "entity";
      break;
    case AnnotationType::CORRECTION:
      result = "correction";
      break;
    case AnnotationType::SUGGESTION:
      result = "suggestion";
      break;
    case AnnotationType::ERRORDETECTION:
      result = "errordetection";
      break;
    case AnnotationType::ALTERNATIVE:
      result = "alternative";
      break;
    case AnnotationType::PHON:
      result = "phon";
      break;
    case AnnotationType::SUBJECTIVITY:
      result = "subjectivity";
      break;
    case AnnotationType::MORPHOLOGICAL:
      result = "morphological";
      break;
    case AnnotationType::EVENT:
      result = "event";
      break;
    case AnnotationType::DEPENDENCY:
      result = "dependency";
      break;
    case AnnotationType::TIMESEGMENT:
      result = "timesegment";
      break;
    case AnnotationType::GAP:
      result = "gap";
      break;
    case AnnotationType::ALIGNMENT:
      result = "alignment";
      break;
    case AnnotationType::COMPLEXALIGNMENT:
      result = "complexalignment";
      break;
    case AnnotationType::COREFERENCE:
      result = "coreference";
      break;
    case AnnotationType::SEMROLE:
      result = "semrole";
      break;
    case AnnotationType::METRIC:
      result = "metric";
      break;
    case AnnotationType::TABLE:
      result = "table";
      break;
    case AnnotationType::LANG:
      result = "lang";
      break;
    case AnnotationType::STYLE:
      result = "style";
      break;
    case AnnotationType::NOTE:
      result = "note";
      break;
    case AnnotationType::PART:
      result = "part";
      break;
    case AnnotationType::UTTERANCE:
      result = "utterance";
      break;
    case AnnotationType::TERM:
      result = "term";
      break;
    case AnnotationType::DEFINITION:
      result = "definition";
      break;
    case AnnotationType::EXAMPLE:
      result = "example";
      break;
    case AnnotationType::PHONOLOGICAL:
      result = "phonology";
      break;

    case AnnotationType::LAST_ANN:
      throw logic_error("LAST_ANN");
    };
    return result;
  }

  AnnotationType::AnnotationType stringToAT( const string& at ){
    if ( at == "text" )
      return AnnotationType::TEXT;
    else if ( at == "string" )
      return AnnotationType::STRING;
    else if ( at == "token" )
      return AnnotationType::TOKEN;
    else if (( at == "div" ) || ( at == "division")) //patch, documentation and libraries used delse ifferent terms
      return AnnotationType::DIVISION;
    else if ( at == "paragraph" )
      return AnnotationType::PARAGRAPH;
    else if ( at == "list" )
      return AnnotationType::LIST;
    else if ( at == "figure" )
      return AnnotationType::FIGURE;
    else if ( at == "whitespace" )
      return AnnotationType::WHITESPACE;
    else if ( at == "linebreak" )
      return AnnotationType::LINEBREAK;
    else if ( at == "sentence" )
      return AnnotationType::SENTENCE;
    else if ( at == "pos" )
      return AnnotationType::POS;
    else if ( at == "lemma" )
      return AnnotationType::LEMMA;
    else if ( at == "domain" )
      return AnnotationType::DOMEIN;
    else if ( at == "sense" )
      return AnnotationType::SENSE;
    else if ( at == "syntax" )
      return AnnotationType::SYNTAX;
    else if ( at == "chunking" )
      return AnnotationType::CHUNKING;
    else if ( at == "entity" )
      return AnnotationType::ENTITY;
    else if ( at == "correction" )
      return AnnotationType::CORRECTION;
    else if ( at == "suggestion" )
      return AnnotationType::SUGGESTION;
    else if ( at == "errordetection" )
      return AnnotationType::ERRORDETECTION;
    else if ( at == "alternative" )
      return AnnotationType::ALTERNATIVE;
    else if ( at == "phon" )
      return AnnotationType::PHON;
    else if ( at == "subjectivity" )
      return AnnotationType::SUBJECTIVITY;
    else if ( at == "morphological" )
      return AnnotationType::MORPHOLOGICAL;
    else if ( at == "event" )
      return AnnotationType::EVENT;
    else if ( at == "dependency" )
      return AnnotationType::DEPENDENCY;
    else if ( at == "timesegment" )
      return AnnotationType::TIMESEGMENT;
    else if ( at == "gap" )
      return AnnotationType::GAP;
    else if ( at == "complexalignment" )
      return AnnotationType::COMPLEXALIGNMENT;
    else if ( at == "alignment" )
      return AnnotationType::ALIGNMENT;
    else if ( at == "semrole" )
      return AnnotationType::SEMROLE;
    else if ( at == "coreference" )
      return AnnotationType::COREFERENCE;
    else if ( at == "metric" )
      return AnnotationType::METRIC;
    else if ( at == "table" )
      return AnnotationType::TABLE;
    else if ( at == "lang" )
      return AnnotationType::LANG;
    else if ( at == "style" )
      return AnnotationType::STYLE;
    else if ( at == "note" )
      return AnnotationType::NOTE;
    else if ( at == "part" )
      return AnnotationType::PART;
    else if ( at == "utterance" )
      return AnnotationType::UTTERANCE;
    else if ( at == "term" )
      return AnnotationType::TERM;
    else if ( at == "definition" )
      return AnnotationType::DEFINITION;
    else if ( at == "example" )
      return AnnotationType::EXAMPLE;
    else if ( at == "phonology" )
      return AnnotationType::PHONOLOGICAL;
    throw ValueError( " unknown translation for attribute: " + at );
  }

  AnnotatorType stringToANT( const string& str ){
    std::string at = TiCC::uppercase( str );
    if ( at == "AUTO" )
      return folia::AUTO;
    else if ( at == "MANUAL" )
      return folia::MANUAL;
    else
      return folia::UNDEFINED;
  }


  string toString( const ElementType& et ) {
    string result;
    switch( et ){
    case BASE: result = "BASE"; break;
    case TextContent_t: result = "t"; break;
    case Text_t: result = "text"; break;
    case Speech_t: result = "speech"; break;
    case Event_t: result = "event"; break;
    case TimeSegment_t: result = "timesegment"; break;
    case TimingLayer_t: result = "timing"; break;
    case LineBreak_t: result = "br"; break;
    case WhiteSpace_t: result = "whitespace"; break;
    case Word_t: result = "w"; break;
    case WordReference_t: result = "wref"; break;
    case String_t: result = "str"; break;
    case Utterance_t: result = "utt"; break;
    case Entry_t: result = "entry"; break;
    case Term_t: result = "term"; break;
    case Example_t: result = "ex"; break;
    case Definition_t: result = "def"; break;
    case PhonContent_t: result = "ph"; break;
    case Sentence_t: result = "s"; break;
    case Paragraph_t: result = "p"; break;
    case Division_t: result = "div"; break;
    case Head_t: result = "head"; break;
    case Table_t: result = "table"; break;
    case TableHead_t: result = "tablehead"; break;
    case Cell_t: result = "cell"; break;
    case LangAnnotation_t: result = "lang"; break;
    case Row_t: result = "row"; break;
    case Caption_t: result = "caption"; break;
    case Label_t: result = "label"; break;
    case List_t: result = "list"; break;
    case Item_t: result = "item"; break;
    case Figure_t: result = "figure"; break;
    case Quote_t: result = "quote"; break;
    case PosAnnotation_t: result = "pos"; break;
    case LemmaAnnotation_t: result = "lemma"; break;
    case PhonologyLayer_t: result = "phonology"; break;
    case Phoneme_t: result = "phoneme"; break;
    case DomainAnnotation_t: result = "domain"; break;
    case SenseAnnotation_t: result = "sense"; break;
    case SubjectivityAnnotation_t: result = "subjectivity"; break;
    case Correction_t: result = "correction"; break;
    case AbstractTokenAnnotation_t: result = "abstracttokenannotation"; break;
    case AbstractAnnotation_t: result = "abstractannotation"; break;
    case AbstractStructureElement_t: result = "abstractstructure"; break;
    case AbstractAnnotationLayer_t: result = "abstractannotationlayer"; break;
    case AbstractSpanAnnotation_t: result = "abstractspanannotation"; break;
    case AbstractCorrectionChild_t: result = "abstractcorrectionchild"; break;
    case AbstractTextMarkup_t: result = "abstracttextmarkup"; break;
    case SyntacticUnit_t: result = "su"; break;
    case SyntaxLayer_t: result = "syntax"; break;
    case Chunk_t: result = "chunk"; break;
    case ChunkingLayer_t: result = "chunking"; break;
    case Entity_t: result = "entity"; break;
    case EntitiesLayer_t: result = "entities"; break;
    case SemanticRolesLayer_t: result = "semroles"; break;
    case SemanticRole_t: result = "semrole"; break;
    case CoreferenceLayer_t: result = "coreferences"; break;
    case CoreferenceLink_t: result = "coreferencelink"; break;
    case CoreferenceChain_t: result = "coreferencechain"; break;
    case MorphologyLayer_t: result = "morphology"; break;
    case Morpheme_t: result = "morpheme"; break;
    case ErrorDetection_t: result = "errordetection"; break;
    case New_t: result = "new"; break;
    case Original_t: result = "original"; break;
    case Current_t: result = "current"; break;
    case Suggestion_t: result = "suggestion"; break;
    case Alternative_t: result = "alt"; break;
    case AlternativeLayers_t: result = "altlayers"; break;
    case XmlComment_t: result = "xml-comment"; break;
    case XmlText_t: result = "xml-text"; break;
    case External_t: result = "external"; break;
    case Note_t:result = "note"; break;
    case Reference_t: result = "ref"; break;
    case Description_t: result = "desc"; break;
    case Gap_t: result = "gap"; break;
    case Content_t: result = "content"; break;
    case Metric_t: result = "metric"; break;
    case Feature_t: result = "feat"; break;
    case SynsetFeature_t: result = "synset"; break;
    case ActorFeature_t: result = "actor"; break;
    case HeadFeature_t: result = "headfeature"; break;
    case ValueFeature_t: result = "value"; break;
    case TimeFeature_t: result = "time"; break;
    case ModalityFeature_t: result = "modality"; break;
    case LevelFeature_t: result = "level"; break;
    case FunctionFeature_t: result = "function"; break;
    case BeginDateTimeFeature_t: result = "begindatetime"; break;
    case EndDateTimeFeature_t: result = "enddatetime"; break;
    case PlaceHolder_t: result = "placeholder"; break;
    case DependenciesLayer_t: result = "dependencies"; break;
    case Dependency_t: result = "dependency"; break;
    case DependencyDependent_t: result = "dep"; break;
    case Headwords_t: result = "hd"; break;
    case Alignment_t: result = "alignment"; break;
    case AlignReference_t: result = "aref"; break;
    case TextMarkupString_t: result = "t-str"; break;
    case TextMarkupGap_t: result = "t-gap"; break;
    case TextMarkupCorrection_t: result = "t-correction"; break;
    case TextMarkupError_t: result = "t-error"; break;
    case TextMarkupStyle_t: result = "t-style"; break;
    case Part_t: result = "part"; break;
    default:
      result = "Unknown Elementtype " + TiCC::toString( int(et) );
    }
    return result;
  }

  ElementType stringToET( const string& tag ){
    if ( tag == "FoLiA" ){
      return BASE;
    }
    else if ( tag == "text" ){
      return Text_t;
    }
    else if ( tag == "speech" ){
      return Speech_t;
    }
    else if ( tag == "utt" ){
      return Utterance_t;
    }
    else if ( tag == "entry" ){
      return Entry_t;
    }
    else if ( tag == "term" ){
      return Term_t;
    }
    else if ( tag == "def" ){
      return Definition_t;
    }
    else if ( tag == "ex" ){
      return Example_t;
    }
    else if ( tag == "ph" ){
      return PhonContent_t;
    }
    else if ( tag == "w" ){
      return Word_t;
    }
    else if ( tag == "str" ){
      return String_t;
    }
    else if ( tag == "event" ){
      return Event_t;
    }
    else if ( tag == "timesegment" ){
      return TimeSegment_t;
    }
    else if ( tag == "timing" ){
      return TimingLayer_t;
    }
    else if ( tag == "s" ){
      return Sentence_t;
    }
    else if ( tag == "t" ){
      return TextContent_t;
    }
    else if ( tag == "br" ){
      return LineBreak_t;
    }
    else if ( tag == "whitespace" ){
      return WhiteSpace_t;
    }
    else if ( tag == "figure" ){
      return Figure_t;
    }
    else if ( tag == "caption" ){
      return Caption_t;
    }
    else if ( tag == "label" ){
      return Label_t;
    }
    else if ( tag == "list" ){
      return List_t;
    }
    else if ( tag == "listitem" || tag == "item" ){
      return Item_t;
    }
    else if ( tag == "p" ){
      return Paragraph_t;
    }
    else if ( tag == "new" ){
      return New_t;
    }
    else if ( tag == "original" ){
      return Original_t;
    }
    else if ( tag == "current" ){
      return Current_t;
    }
    else if ( tag == "suggestion" ){
      return Suggestion_t;
    }
    else if ( tag == "head" ){
      return Head_t;
    }
    else if ( tag == "table" ){
      return Table_t;
    }
    else if ( tag == "tablehead" ){
      return TableHead_t;
    }
    else if ( tag == "cell" ){
      return Cell_t;
    }
    else if ( tag == "row" ){
      return Row_t;
    }
    else if ( tag == "lang" ){
      return LangAnnotation_t;
    }
    else if ( tag == "xml-comment" ){
      return XmlComment_t;
    }
    else if ( tag == "xml-text" ){
      return XmlText_t;
    }
    else if ( tag == "external" ){
      return External_t;
    }
    else if ( tag == "note" ){
      return Note_t;
    }
    else if ( tag == "ref" ){
      return Reference_t;
    }
    else if ( tag == "desc" ){
      return Description_t;
    }
    else if ( tag == "gap" ){
      return Gap_t;
    }
    else if ( tag == "content" ){
      return Content_t;
    }
    else if ( tag == "metric" ){
      return Metric_t;
    }
    else if ( tag == "div" ){
      return Division_t;
    }
    else if ( tag == "abstractannotationlayer" ){
      return AbstractAnnotationLayer_t;
    }
    else if ( tag == "abstractcorrectionchild" ){
      return AbstractCorrectionChild_t;
    }
    else if ( tag == "abstractspanannotation" ){
      return AbstractSpanAnnotation_t;
    }
    else if ( tag == "abstractannotation" ){
      return AbstractAnnotation_t;
    }
    else if ( tag == "abstracttokenannotation" ){
      return AbstractTokenAnnotation_t;
    }
    else if ( tag == "abstractstructure" ){
      return AbstractStructureElement_t;
    }
    else if ( tag == "pos" ){
      return PosAnnotation_t;
    }
    else if ( tag == "lemma" ){
      return LemmaAnnotation_t;
    }
    else if ( tag == "phonology" ){
      return PhonologyLayer_t;
    }
    else if ( tag == "phoneme" ){
      return Phoneme_t;
    }
    else if ( tag == "domain" ){
      return DomainAnnotation_t;
    }
    else if ( tag == "sense" ){
      return SenseAnnotation_t;
    }
    else if ( tag == "syntax" ){
      return SyntaxLayer_t;
    }
    else if ( tag == "subjectivity" ){
      return SubjectivityAnnotation_t;
    }
    else if ( tag == "chunk" ){
      return Chunk_t;
    }
    else if ( tag == "chunking" ){
      return ChunkingLayer_t;
    }
    else if ( tag == "entity" ){
      return Entity_t;
    }
    else if ( tag == "entities" ){
      return EntitiesLayer_t;
    }
    else if ( tag == "semroles" ){
      return SemanticRolesLayer_t;
    }
    else if ( tag == "semrole" ){
      return SemanticRole_t;
    }
    else if ( tag == "coreferences" ){
      return CoreferenceLayer_t;
    }
    else if ( tag == "coreferencelink" ){
      return CoreferenceLink_t;
    }
    else if ( tag == "coreferencechain" ){
      return CoreferenceChain_t;
    }
    else if ( tag == "alt" ){
      return Alternative_t;
    }
    else if ( tag == "placeholder" ){
      return PlaceHolder_t;
    }
    else if ( tag == "altlayers" ){
      return AlternativeLayers_t;
    }
    else if ( tag == "su" ){
      return SyntacticUnit_t;
    }
    else if ( tag == "wref" ){
      return WordReference_t;
    }
    else if ( tag == "correction" ){
      return Correction_t;
    }
    else if ( tag == "errordetection" ){
      return ErrorDetection_t;
    }
    else if ( tag == "morphology" ){
      return MorphologyLayer_t;
    }
    else if ( tag == "morpheme" ){
      return Morpheme_t;
    }
    else if ( tag == "feat" ){
      return Feature_t;
    }
    else if ( tag == "begindatetime" ){
      return BeginDateTimeFeature_t;
    }
    else if ( tag == "enddatetime" ){
      return EndDateTimeFeature_t;
    }
    else if ( tag == "synset" ){
      return SynsetFeature_t;
    }
    else if ( tag == "actor" ){
      return ActorFeature_t;
    }
    else if ( tag == "headfeature" ){
      return HeadFeature_t;
    }
    else if ( tag == "value" ){
      return ValueFeature_t;
    }
    else if ( tag == "time" ){
      return TimeFeature_t;
    }
    else if ( tag == "level" ){
      return LevelFeature_t;
    }
    else if ( tag == "function" ){
      return FunctionFeature_t;
    }
    else if ( tag == "modality" ){
      return ModalityFeature_t;
    }
    else if ( tag == "quote" ){
      return Quote_t;
    }
    else if ( tag == "dependencies" ){
      return DependenciesLayer_t;
    }
    else if ( tag == "dependency" ){
      return Dependency_t;
    }
    else if ( tag == "dep" ){
      return DependencyDependent_t;
    }
    else if ( tag == "hd" ){
      return Headwords_t;
    }
    else if ( tag == "alignment" ){
      return Alignment_t;
    }
    else if ( tag == "aref" ){
      return AlignReference_t;
    }
    else if ( tag == "abstracttextmarkup" ){
      return AbstractTextMarkup_t;
    }
    else if ( tag == "t-str" ){
      return TextMarkupString_t;
    }
    else if ( tag == "t-gap" ){
      return TextMarkupGap_t;
    }
    else if ( tag == "t-correction" ){
      return TextMarkupCorrection_t;
    }
    else if ( tag == "t-error" ){
      return TextMarkupError_t;
    }
    else if ( tag == "t-style" ){
      return TextMarkupStyle_t;
    }
    else if ( tag == "part" ){
      return Part_t;
    }
    else {
      throw ValueError( "unknown tag <" + tag + ">" );
    }
    return BASE;
  }

}
