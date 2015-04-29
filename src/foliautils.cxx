/*
  $Id$
  $URL$

  Copyright (c) 1998 - 2015
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

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <stdexcept>
#include <algorithm>
#include "ticcutils/StringOps.h"
#include "ticcutils/XMLtools.h"
#include "libfolia/document.h"

using namespace std;
using namespace TiCC;

namespace folia {

  UnicodeString UTF8ToUnicode( const string& s ){
    return UnicodeString::fromUTF8( s );
  }

  string UnicodeToUTF8( const UnicodeString& s ){
    string result;
    s.toUTF8String(result);
    return result;
  }

  string toString( const double d ){
    stringstream dummy;
    if ( !( dummy << d ) ) {
      throw( runtime_error( "conversion to string failed" ) );
    }
    return dummy.str();
  }

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
    case AnnotationType::PHONETIC:
      result = "phonetic";
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
    else if ( at == "phonetic" )
      return AnnotationType::PHONETIC;
    throw ValueError( " unknown translation for attribute: " + at );
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
    case Str_t: result = "str"; break;
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
    case Lang_t: result = "lang"; break;
    case Row_t: result = "row"; break;
    case Caption_t: result = "caption"; break;
    case Label_t: result = "label"; break;
    case List_t: result = "list"; break;
    case Item_t: result = "item"; break;
    case Figure_t: result = "figure"; break;
    case Quote_t: result = "quote"; break;
    case TokenAnnotation_t: result = "tokenannotation"; break;
    case Structure_t: result = "structure"; break;
    case Pos_t: result = "pos"; break;
    case Lemma_t: result = "lemma"; break;
    case Phon_t: result = "phon"; break;
    case Domain_t: result = "domain"; break;
    case Sense_t: result = "sense"; break;
    case Subjectivity_t: result = "subjectivity"; break;
    case Correction_t: result = "correction"; break;
    case AnnotationLayer_t: result = "annotationlayer"; break;
    case SpanAnnotation_t: result = "spanannotation"; break;
    case SyntacticUnit_t: result = "su"; break;
    case SyntaxLayer_t: result = "syntax"; break;
    case Chunk_t: result = "chunk"; break;
    case Chunking_t: result = "chunking"; break;
    case Entity_t: result = "entity"; break;
    case Entities_t: result = "entities"; break;
    case Semroles_t: result = "semroles"; break;
    case Semrole_t: result = "semrole"; break;
    case Coreferences_t: result = "coreferences"; break;
    case CoreferenceLink_t: result = "coreferencelink"; break;
    case CoreferenceChain_t: result = "coreferencechain"; break;
    case Morphology_t: result = "morphology"; break;
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
    case Dependencies_t: result = "dependencies"; break;
    case Dependency_t: result = "dependency"; break;
    case DependencyDependent_t: result = "dep"; break;
    case Headwords_t: result = "hd"; break;
    case Alignment_t: result = "alignment"; break;
    case AlignReference_t: result = "aref"; break;
    case AbstractTextMarkup_t: result = "textmarkup"; break;
    case TextMarkupString_t: result = "t-str"; break;
    case TextMarkupGap_t: result = "t-gap"; break;
    case TextMarkupCorrection_t: result = "t-correction"; break;
    case TextMarkupError_t: result = "t-error"; break;
    case TextMarkupStyle_t: result = "t-style"; break;
    case Part_t: result = "part"; break;
    default:
      result = "Unknown Elementtype " + folia::toString( int(et) );
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
      return Str_t;
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
      return Lang_t;
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
    else if ( tag == "annotationlayer" ){
      return AnnotationLayer_t;
    }
    else if ( tag == "spanannotation" ){
      return SpanAnnotation_t;
    }
    else if ( tag == "tokenannotation" ){
      return TokenAnnotation_t;
    }
    else if ( tag == "structure" ){
      return Structure_t;
    }
    else if ( tag == "pos" ){
      return Pos_t;
    }
    else if ( tag == "lemma" ){
      return Lemma_t;
    }
    else if ( tag == "phon" ){
      return Phon_t;
    }
    else if ( tag == "domain" ){
      return Domain_t;
    }
    else if ( tag == "sense" ){
      return Sense_t;
    }
    else if ( tag == "syntax" ){
      return SyntaxLayer_t;
    }
    else if ( tag == "subjectivity" ){
      return Subjectivity_t;
    }
    else if ( tag == "chunk" ){
      return Chunk_t;
    }
    else if ( tag == "chunking" ){
      return Chunking_t;
    }
    else if ( tag == "entity" ){
      return Entity_t;
    }
    else if ( tag == "entities" ){
      return Entities_t;
    }
    else if ( tag == "semroles" ){
      return Semroles_t;
    }
    else if ( tag == "semrole" ){
      return Semrole_t;
    }
    else if ( tag == "coreferences" ){
      return Coreferences_t;
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
      return Morphology_t;
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
      return Dependencies_t;
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
    else if ( tag == "textmarkup" ){
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

  FoliaElement *FoliaImpl::createElement( Document *doc,
					  const string& tag ){

    ElementType et = BASE;
    try {
      et = stringToET( tag );
    }
    catch ( ValueError& e ){
      cerr << e.what() << endl;
      return 0;
    }
    switch ( et ){
    case BASE:
      return new FoLiA( doc );
    case Text_t:
      return new Text( doc );
    case Speech_t:
      return new Speech( doc );
    case Utterance_t:
      return new Utterance( doc );
    case Entry_t:
      return new Entry( doc );
    case Example_t:
      return new Example( doc );
    case Term_t:
      return new Term( doc );
    case Definition_t:
      return new Definition( doc );
    case PhonContent_t:
      return new PhonContent( doc );
    case Word_t:
      return new Word( doc );
    case Str_t:
      return new String( doc );
    case Event_t:
      return new Event( doc );
    case TimeSegment_t:
      return new TimeSegment( doc );
    case TimingLayer_t:
      return new TimingLayer( doc );
    case Sentence_t:
      return new Sentence( doc );
    case TextContent_t:
      return new TextContent( doc );
    case LineBreak_t:
      return new LineBreak( doc );
    case WhiteSpace_t:
      return new WhiteSpace( doc );
    case Figure_t:
      return new Figure( doc );
    case Caption_t:
      return new Caption( doc );
    case Label_t:
      return new Label( doc );
    case List_t:
      return new List( doc );
    case Item_t:
      return new Item( doc );
    case Paragraph_t:
      return new Paragraph( doc );
    case New_t:
      return new NewElement( doc );
    case Original_t:
      return new Original( doc );
    case Current_t:
      return new Current( doc );
    case Suggestion_t:
      return new Suggestion( doc );
    case Head_t:
      return new Head( doc );
    case Table_t:
      return new Table( doc );
    case TableHead_t:
      return new TableHead( doc );
    case Cell_t:
      return new Cell( doc );
    case Row_t:
      return new Row( doc );
    case Lang_t:
      return new LangAnnotation( doc );
    case SpanAnnotation_t:
      return new AbstractSpanAnnotation( doc );
    case TokenAnnotation_t:
      return new AbstractTokenAnnotation( doc );
    case Structure_t:
      return new AbstractStructureElement( doc );
    case XmlComment_t:
      return new XmlComment( doc );
    case XmlText_t:
      return new XmlText( doc );
    case External_t:
      return new External( doc );
    case Note_t:
      return new Note( doc );
    case Reference_t:
      return new Reference( doc );
    case Description_t:
      return new Description( doc );
    case Gap_t:
      return new Gap( doc );
    case Content_t:
      return new Content( doc );
    case Metric_t:
      return new MetricAnnotation( doc );
    case Division_t:
      return new Division( doc );
    case AnnotationLayer_t:
      return new AbstractAnnotationLayer( doc );
    case Pos_t:
      return new PosAnnotation( doc );
    case Lemma_t:
      return new LemmaAnnotation( doc );
    case Phon_t:
      return new PhonAnnotation( doc );
    case Domain_t:
      return new DomainAnnotation( doc );
    case Sense_t:
      return new SenseAnnotation( doc );
    case SyntaxLayer_t:
      return new SyntaxLayer( doc );
    case Subjectivity_t:
      return new SubjectivityAnnotation( doc );
    case Chunk_t:
      return new Chunk( doc );
    case Chunking_t:
      return new ChunkingLayer( doc );
    case Entity_t:
      return new Entity( doc );
    case Entities_t:
      return new EntitiesLayer( doc );
    case Semroles_t:
      return new SemanticRolesLayer( doc );
    case Semrole_t:
      return new SemanticRole( doc );
    case Coreferences_t:
      return new CoreferenceLayer( doc );
    case CoreferenceLink_t:
      return new CoreferenceLink( doc );
    case CoreferenceChain_t:
      return new CoreferenceChain( doc );
    case Alternative_t:
      return new Alternative( doc );
    case PlaceHolder_t:
      return new PlaceHolder();
    case AlternativeLayers_t:
      return new AlternativeLayers( doc );
    case SyntacticUnit_t:
      return new SyntacticUnit( doc );
    case WordReference_t:
      return new WordReference( doc );
    case Correction_t:
      return new Correction( doc );
    case ErrorDetection_t:
      return new ErrorDetection( doc );
    case Morphology_t:
      return new MorphologyLayer( doc );
    case Morpheme_t:
      return new Morpheme( doc );
    case Feature_t:
      return new Feature( doc );
    case BeginDateTimeFeature_t:
      return new BeginDateTimeFeature( doc );
    case EndDateTimeFeature_t:
      return new EndDateTimeFeature( doc );
    case SynsetFeature_t:
      return new SynsetFeature( doc );
    case ActorFeature_t:
      return new ActorFeature( doc );
    case HeadFeature_t:
      return new HeadFeature( doc );
    case ValueFeature_t:
      return new ValueFeature( doc );
    case TimeFeature_t:
      return new TimeFeature( doc );
    case ModalityFeature_t:
      return new ModalityFeature( doc );
    case FunctionFeature_t:
      return new FunctionFeature( doc );
    case LevelFeature_t:
      return new LevelFeature( doc );
    case Quote_t:
      return new Quote( doc );
    case Dependencies_t:
      return new DependenciesLayer( doc );
    case Dependency_t:
      return new Dependency( doc );
    case DependencyDependent_t:
      return new DependencyDependent( doc );
    case Headwords_t:
      return new Headwords( doc );
    case Alignment_t:
      return new Alignment( doc );
    case AlignReference_t:
      return new AlignReference( doc );
    case AbstractTextMarkup_t:
      return new AbstractTextMarkup( doc );
    case TextMarkupString_t:
      return new TextMarkupString( doc );
    case TextMarkupGap_t:
      return new TextMarkupGap( doc );
    case TextMarkupCorrection_t:
      return new TextMarkupCorrection( doc );
    case TextMarkupError_t:
      return new TextMarkupError( doc );
    case TextMarkupStyle_t:
      return new TextMarkupStyle( doc );
    case Part_t:
      return new Part( doc );
    default:
      throw ValueError( "unknown elementtype(" + toString(int(et)) + ")" );
    }
    return 0;
  }


  KWargs getArgs( const std::string& s ){
    KWargs result;
    bool quoted = false;
    bool parseatt = true;
    bool escaped = false;
    vector<string> parts;
    string att;
    string val;
    //    cerr << "getArgs \\" << s << "\\" << endl;

    for ( size_t i=0; i < s.size(); ++i ){
      //      cerr << "bekijk " << s[i] << endl;
      //      cerr << "quoted = " << (quoted?"YES":"NO")
      // 	   << " parseatt = " << (parseatt?"YES":"NO")
      // 	   << " escaped = " << (escaped?"YES":"NO") << endl;
      if ( s[i] == '\\' ){
	//	cerr << "handle backslash " << endl;
	if ( quoted ){
	  if ( escaped ){
	    val += s[i];
	    escaped = false;
	  }
	  else {
	    escaped = true;
	    continue;
	  }
	}
	else
	  throw ArgsError( s + ", stray \\" );
      }
      else if ( s[i] == '\'' ){
	//	cerr << "handle single quote " << endl;
	if ( quoted ){
	  if ( escaped ){
	    val += s[i];
	    escaped = false;
	  }
	  else {
	    if ( att.empty() || val.empty() )
	      throw ArgsError( s + ", (''?)" );
	    result[att] = val;
	    //	    cerr << "added " << att << "='" << val << "'" << endl;
	    att.clear();
	    val.clear();
	    quoted = false;
	  }
	}
	else {
	  quoted = true;
	}
      }
      else if ( s[i] == '=' ) {
	if ( parseatt ){
	  parseatt = false;
	}
	else if ( quoted )
	  val += s[i];
	else
	  throw ArgsError( s + ", stray '='?" );
      }
      else if ( s[i] == ',' ){
	if ( quoted )
	  val += s[i];
	else if ( !parseatt ){
	  parseatt = true;
	}
	else
	  throw ArgsError( s + ", stray '='?" );
      }
      else if ( s[i] == ' ' ){
	if ( quoted )
	  val += s[i];
      }
      else if ( parseatt )
	att += s[i];
      else if ( quoted ){
	if ( escaped ){
	  val += "\\";
	  escaped = false;
	}
	val += s[i];
      }
      else
	throw ArgsError( s + ", unquoted value or missing , ?" );
      // cerr << "att = '" << att << "'" << endl;
      // cerr << "val = '" << val << "'" << endl;
    }
    if ( quoted )
      throw ArgsError( s + ", unbalanced '?" );
    return result;
  }

  std::string toString( const KWargs& args ){
    string result;
    KWargs::const_iterator it = args.begin();
    while ( it != args.end() ){
      result += it->first + "='" + it->second + "'";
      ++it;
      if ( it != args.end() )
	result += ",";
    }
    return result;
  }

  KWargs getAttributes( const xmlNode *node ){
    KWargs atts;
    if ( node ){
      xmlAttr *a = node->properties;
      while ( a ){
	if ( a->atype == XML_ATTRIBUTE_ID && string((char*)a->name) == "id" ){
	  atts["_id"] = (char *)a->children->content;
	}
	else
	  atts[(char*)a->name] = (char *)a->children->content;
	a = a->next;
      }
    }
    return atts;
  }

  void addAttributes( xmlNode *node, const KWargs& atts ){
    KWargs attribs = atts;
    KWargs::iterator it = attribs.find("_id");
    if ( it != attribs.end() ){ // _id is special for xml:id
      xmlSetProp( node, XML_XML_ID, (const xmlChar *)it->second.c_str() );
      attribs.erase(it);
    }
    it = attribs.find("lang");
    if ( it != attribs.end() ){ // lang is special too
      xmlNodeSetLang( node, (const xmlChar*)it->second.c_str() );
      attribs.erase(it);
    }
    it = attribs.find("id");
    if ( it != attribs.end() ){ // id is te be sorted before the rest
      xmlSetProp( node, (const xmlChar*)("id"), (const xmlChar *)it->second.c_str() );
      attribs.erase(it);
    }
    // and now the rest
    it = attribs.begin();
    while ( it != attribs.end() ){
      xmlSetProp( node,
		  (const xmlChar*)it->first.c_str(),
		  (const xmlChar*)it->second.c_str() );
      ++it;
    }
  }

  int toMonth( const string& ms ){
    int result = 0;
    try {
      result = stringTo<int>( ms );
      return result - 1;
    }
    catch( exception ){
      string m = TiCC::lowercase( ms );
      if ( m == "jan" )
	return 0;
      else if ( m == "feb" )
	return 1;
      else if ( m == "mar" )
	return 2;
      else if ( m == "apr" )
	return 3;
      else if ( m == "may" )
	return 4;
      else if ( m == "jun" )
	return 5;
      else if ( m == "jul" )
	return 6;
      else if ( m == "aug" )
	return 7;
      else if ( m == "sep" )
	return 8;
      else if ( m == "oct" )
	return 9;
      else if ( m == "nov" )
	return 10;
      else if ( m == "dec" )
	return 11;
      else
	throw runtime_error( "invalid month: " + m );
    }
  }

  string parseDate( const string& s ){
    if ( s.empty() )
      return "";
    //    cerr << "try to read a date-time " << s << endl;
    vector<string> date_time;
    size_t num = TiCC::split_at( s, date_time, "T");
    if ( num == 0 ){
      num = TiCC::split_at( s, date_time, " ");
      if ( num == 0 ){
	cerr << "failed to read a date-time " << s << endl;
	return 0;
      }
    }
    //    cerr << "found " << num << " parts" << endl;
    tm *time = new tm();
    if ( num == 1 || num == 2 ){
      //      cerr << "parse date " << date_time[0] << endl;
      vector<string> date_parts;
      size_t dnum = TiCC::split_at( date_time[0], date_parts, "-" );
      switch ( dnum ){
      case 3: {
	int mday = stringTo<int>( date_parts[2] );
	time->tm_mday = mday;
      }
      case 2: {
	int mon = toMonth( date_parts[1] );
	time->tm_mon = mon;
      }
      case 1: {
	int year = stringTo<int>( date_parts[0] );
	time->tm_year = year-1900;
      }
	break;
      default:
	cerr << "failed to read a date from " << date_time[0] << endl;
	return 0;
      }
    }
    if ( num == 2 ){
      //      cerr << "parse time " << date_time[1] << endl;
      vector<string> date_parts;
      num = TiCC::split_at( date_time[1], date_parts, ":" );
      //      cerr << "parts " << date_parts << endl;
      switch ( num ){
      case 4:
	// ignore
      case 3: {
	int sec = stringTo<int>( date_parts[2] );
	time->tm_sec = sec;
      }
      case 2: {
	int min = stringTo<int>( date_parts[1] );
	time->tm_min = min;
      }
      case 1: {
	int hour = stringTo<int>( date_parts[0] );
	time->tm_hour = hour;
      }
	break;
      default:
	cerr << "failed to read a time from " << date_time[1] << endl;
	return 0;
      }
    }
    // cerr << "read _date time = " << toString(time) << endl;
    char buf[100];
    strftime( buf, 100, "%Y-%m-%dT%X", time );
    delete time;
    return buf;
  }

  bool AT_sanity_check(){
    bool sane = true;
    AnnotationType::AnnotationType at = AnnotationType::NO_ANN;
    while ( ++at != AnnotationType::LAST_ANN ){
      string s = toString( at );
      if ( s.empty() ){
	cerr << "no string translation for AnnotationType(" << at << ")" << endl;
	sane = false;
      }
      else {
	try {
	  stringTo<AnnotationType::AnnotationType>( s );
	}
	catch ( ValueError& e ){
	  cerr << "no AnnotationType found for string '" << s << "'" << endl;
	  sane = false;
	}
      }
    }
    return sane;
  }

  bool ET_sanity_check(){
    bool sane = true;
    ElementType et = BASE;
    while ( ++et != LastElement ){
      string s = toString( et );
      if ( s.empty() ){
	cerr << "no string translation for ElementType(" << et << ")" << endl;
	sane = false;
      }
      else {
	ElementType et2;
	try {
	  et2 = stringToET( s );
	}
	catch ( ValueError& e ){
	  cerr << "no element type found for string '" << s << "'" << endl;
	  sane = false;
	  continue;
	}
	if ( et != et2 ){
	  cerr << "Argl: toString(ET) doesn't match original:" << s
	       << " vs " << toString(et2) << endl;
	  sane = false;
	  continue;
	}
	FoliaElement *tmp = FoliaImpl::createElement( 0, s );
	if ( tmp == 0 ) {
	  cerr << "no Element created found for string '" << s << "'" << endl;
	  sane = false;
	}
	else {
	  if ( et != tmp->element_id() ){
	    cerr << "the element type of " << tmp << " != " << et << endl;
	    sane = false;
	  }
	  if ( s != tmp->xmltag() ){
	    cerr << "the xmltag " << tmp->xmltag() << " != " << s << endl;
	    sane = false;
	  }
	}
      }
    }
    return sane;
  }

  bool isNCName( const std::string& s ){
    const string extra=".-_";
    if ( s.empty() )
      throw XmlError( "an empty string is not a valid NCName." );
    else if ( !isalpha(s[0]) )
      throw XmlError( "'"
		      + s
		      + "' is not a valid NCName. (must start with character)." );
    else {
      for ( size_t i=1; i < s.length(); ++i ){
	if ( !isalnum(s[i]) &&
	     extra.find(s[i]) == string::npos )
	  throw XmlError( "'" + s
			  + "' is not a valid NCName.(invalid '"
			  + char(s[i]) + "' found" );
      }
    }
    return true;
  }

} //namespace folia
