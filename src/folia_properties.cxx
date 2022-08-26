/*
  Copyright (c) 2006 - 2022
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

#include <set>
#include <string>
#include <iostream>

#include "libfolia/folia.h"
#include "libfolia/folia_properties.h"

//foliaspec:header
//This file was last updated according to the FoLiA specification for version 2.5.1 on 2021-08-19 11:28:55, using foliaspec.py
//Code blocks after a foliaspec comment (until the next newline) are automatically generated. **DO NOT EDIT THOSE** and **DO NOT REMOVE ANY FOLIASPEC COMMENTS** !!!

namespace folia {

  using namespace std;

  //foliaspec:version_major:MAJOR_VERSION
  //The FoLiA version (major)
  const int MAJOR_VERSION = 2;

  //foliaspec:version_minor:MINOR_VERSION
  //The FoLiA version (minor)
  const int MINOR_VERSION = 5;

  //foliaspec:version_sub:SUB_VERSION
  //The FoLiA version (sub/rev)
  const int SUB_VERSION = 1;

  //foliaspec:namespace:NSFOLIA
  //The FoLiA XML namespace
  const string NSFOLIA = "http://ilk.uvt.nl/folia";

  const string NSDCOI = "http://lands.let.ru.nl/projects/d-coi/ns/1.0";
  const string NSIMDI = "http://www.mpi.nl/IMDI/Schema/IMDI";

  const string DEFAULT_TEXT_SET = "https://raw.githubusercontent.com/proycon/folia/master/setdefinitions/text.foliaset.ttl";
  const string DEFAULT_PHON_SET = "https://raw.githubusercontent.com/proycon/folia/master/setdefinitions/phon.foliaset.ttl";

  //foliaspec:attributefeatures
  const set<string> AttributeFeatures = { "actor", "begindatetime", "enddatetime", "font", "function", "headfeature", "level", "mod", "polarity", "size", "strength", "style", "synset", "time", "value" };

  //foliaspec:annotationtype_string_map
  //A mapping from annotation types to strings
  const map<AnnotationType,string> ant_s_map = {
    { AnnotationType::NO_ANN, "NONE" },
    { AnnotationType::ALTERNATIVE,  "alternative" },
    { AnnotationType::CHUNKING,  "chunking" },
    { AnnotationType::COMMENT,  "comment" },
    { AnnotationType::RAWCONTENT,  "rawcontent" },
    { AnnotationType::COREFERENCE,  "coreference" },
    { AnnotationType::CORRECTION,  "correction" },
    { AnnotationType::DEFINITION,  "definition" },
    { AnnotationType::DEPENDENCY,  "dependency" },
    { AnnotationType::DESCRIPTION,  "description" },
    { AnnotationType::DIVISION,  "division" },
    { AnnotationType::DOMAIN,  "domain" },
    { AnnotationType::ENTITY,  "entity" },
    { AnnotationType::ENTRY,  "entry" },
    { AnnotationType::ERRORDETECTION,  "errordetection" },
    { AnnotationType::EVENT,  "event" },
    { AnnotationType::EXAMPLE,  "example" },
    { AnnotationType::EXTERNAL,  "external" },
    { AnnotationType::FIGURE,  "figure" },
    { AnnotationType::GAP,  "gap" },
    { AnnotationType::HEAD,  "head" },
    { AnnotationType::HIDDENTOKEN,  "hiddentoken" },
    { AnnotationType::HYPHENATION,  "hyphenation" },
    { AnnotationType::LANG,  "lang" },
    { AnnotationType::LEMMA,  "lemma" },
    { AnnotationType::LINEBREAK,  "linebreak" },
    { AnnotationType::LIST,  "list" },
    { AnnotationType::METRIC,  "metric" },
    { AnnotationType::MODALITY,  "modality" },
    { AnnotationType::MORPHOLOGICAL,  "morphological" },
    { AnnotationType::NOTE,  "note" },
    { AnnotationType::OBSERVATION,  "observation" },
    { AnnotationType::PARAGRAPH,  "paragraph" },
    { AnnotationType::PART,  "part" },
    { AnnotationType::PHON,  "phon" },
    { AnnotationType::PHONOLOGICAL,  "phonological" },
    { AnnotationType::POS,  "pos" },
    { AnnotationType::PREDICATE,  "predicate" },
    { AnnotationType::QUOTE,  "quote" },
    { AnnotationType::REFERENCE,  "reference" },
    { AnnotationType::RELATION,  "relation" },
    { AnnotationType::SEMROLE,  "semrole" },
    { AnnotationType::SENSE,  "sense" },
    { AnnotationType::SENTENCE,  "sentence" },
    { AnnotationType::SENTIMENT,  "sentiment" },
    { AnnotationType::SPANRELATION,  "spanrelation" },
    { AnnotationType::STATEMENT,  "statement" },
    { AnnotationType::STRING,  "string" },
    { AnnotationType::SUBJECTIVITY,  "subjectivity" },
    { AnnotationType::SYNTAX,  "syntax" },
    { AnnotationType::TABLE,  "table" },
    { AnnotationType::TERM,  "term" },
    { AnnotationType::TEXT,  "text" },
    { AnnotationType::HSPACE,  "hspace" },
    { AnnotationType::STYLE,  "style" },
    { AnnotationType::WHITESPACE,  "whitespace" },
    { AnnotationType::TIMESEGMENT,  "timesegment" },
    { AnnotationType::UTTERANCE,  "utterance" },
    { AnnotationType::TOKEN,  "token" },
  };

  //foliaspec:string_annotationtype_map
  //A mapping from strings to annotation types
  const map<string,AnnotationType> s_ant_map = {
    { "NONE", AnnotationType::NO_ANN },
    { "alternative", AnnotationType::ALTERNATIVE },
    { "chunking", AnnotationType::CHUNKING },
    { "comment", AnnotationType::COMMENT },
    { "rawcontent", AnnotationType::RAWCONTENT },
    { "coreference", AnnotationType::COREFERENCE },
    { "correction", AnnotationType::CORRECTION },
    { "definition", AnnotationType::DEFINITION },
    { "dependency", AnnotationType::DEPENDENCY },
    { "description", AnnotationType::DESCRIPTION },
    { "division", AnnotationType::DIVISION },
    { "domain", AnnotationType::DOMAIN },
    { "entity", AnnotationType::ENTITY },
    { "entry", AnnotationType::ENTRY },
    { "errordetection", AnnotationType::ERRORDETECTION },
    { "event", AnnotationType::EVENT },
    { "example", AnnotationType::EXAMPLE },
    { "external", AnnotationType::EXTERNAL },
    { "figure", AnnotationType::FIGURE },
    { "gap", AnnotationType::GAP },
    { "head", AnnotationType::HEAD },
    { "hiddentoken", AnnotationType::HIDDENTOKEN },
    { "hyphenation", AnnotationType::HYPHENATION },
    { "lang", AnnotationType::LANG },
    { "lemma", AnnotationType::LEMMA },
    { "linebreak", AnnotationType::LINEBREAK },
    { "list", AnnotationType::LIST },
    { "metric", AnnotationType::METRIC },
    { "modality", AnnotationType::MODALITY },
    { "morphological", AnnotationType::MORPHOLOGICAL },
    { "note", AnnotationType::NOTE },
    { "observation", AnnotationType::OBSERVATION },
    { "paragraph", AnnotationType::PARAGRAPH },
    { "part", AnnotationType::PART },
    { "phon", AnnotationType::PHON },
    { "phonological", AnnotationType::PHONOLOGICAL },
    { "pos", AnnotationType::POS },
    { "predicate", AnnotationType::PREDICATE },
    { "quote", AnnotationType::QUOTE },
    { "reference", AnnotationType::REFERENCE },
    { "relation", AnnotationType::RELATION },
    { "semrole", AnnotationType::SEMROLE },
    { "sense", AnnotationType::SENSE },
    { "sentence", AnnotationType::SENTENCE },
    { "sentiment", AnnotationType::SENTIMENT },
    { "spanrelation", AnnotationType::SPANRELATION },
    { "statement", AnnotationType::STATEMENT },
    { "string", AnnotationType::STRING },
    { "subjectivity", AnnotationType::SUBJECTIVITY },
    { "syntax", AnnotationType::SYNTAX },
    { "table", AnnotationType::TABLE },
    { "term", AnnotationType::TERM },
    { "text", AnnotationType::TEXT },
    { "hspace", AnnotationType::HSPACE },
    { "style", AnnotationType::STYLE },
    { "whitespace", AnnotationType::WHITESPACE },
    { "timesegment", AnnotationType::TIMESEGMENT },
    { "utterance", AnnotationType::UTTERANCE },
    { "token", AnnotationType::TOKEN },
  };

  //foliaspec:elementtype_string_map
  const map<ElementType,string> et_s_map = {
    { BASE, "FoLiA" },
    { AbstractAnnotationLayer_t,  "_AbstractAnnotationLayer" },
    { AbstractContentAnnotation_t,  "_AbstractContentAnnotation" },
    { AbstractCorrectionChild_t,  "_AbstractCorrectionChild" },
    { AbstractHigherOrderAnnotation_t,  "_AbstractHigherOrderAnnotation" },
    { AbstractInlineAnnotation_t,  "_AbstractInlineAnnotation" },
    { AbstractSpanAnnotation_t,  "_AbstractSpanAnnotation" },
    { AbstractSpanRole_t,  "_AbstractSpanRole" },
    { AbstractStructureElement_t,  "_AbstractStructureElement" },
    { AbstractSubtokenAnnotation_t,  "_AbstractSubtokenAnnotation" },
    { AbstractTextMarkup_t,  "_AbstractTextMarkup" },
    { ActorFeature_t,  "actor" },
    { Alternative_t,  "alt" },
    { AlternativeLayers_t,  "altlayers" },
    { BegindatetimeFeature_t,  "begindatetime" },
    { Caption_t,  "caption" },
    { Cell_t,  "cell" },
    { Chunk_t,  "chunk" },
    { ChunkingLayer_t,  "chunking" },
    { Comment_t,  "comment" },
    { Content_t,  "content" },
    { CoreferenceChain_t,  "coreferencechain" },
    { CoreferenceLayer_t,  "coreferences" },
    { CoreferenceLink_t,  "coreferencelink" },
    { Correction_t,  "correction" },
    { Cue_t,  "cue" },
    { Current_t,  "current" },
    { Definition_t,  "def" },
    { DependenciesLayer_t,  "dependencies" },
    { Dependency_t,  "dependency" },
    { DependencyDependent_t,  "dep" },
    { Description_t,  "desc" },
    { Division_t,  "div" },
    { DomainAnnotation_t,  "domain" },
    { EnddatetimeFeature_t,  "enddatetime" },
    { EntitiesLayer_t,  "entities" },
    { Entity_t,  "entity" },
    { Entry_t,  "entry" },
    { ErrorDetection_t,  "errordetection" },
    { Event_t,  "event" },
    { Example_t,  "ex" },
    { External_t,  "external" },
    { Feature_t,  "feat" },
    { Figure_t,  "figure" },
    { FontFeature_t,  "font" },
    { ForeignData_t,  "foreign-data" },
    { FunctionFeature_t,  "function" },
    { Gap_t,  "gap" },
    { Head_t,  "head" },
    { HeadFeature_t,  "headfeature" },
    { Headspan_t,  "hd" },
    { Hiddenword_t,  "hiddenw" },
    { Hyphbreak_t,  "t-hbr" },
    { Label_t,  "label" },
    { LangAnnotation_t,  "lang" },
    { LemmaAnnotation_t,  "lemma" },
    { LevelFeature_t,  "level" },
    { Linebreak_t,  "br" },
    { LinkReference_t,  "xref" },
    { List_t,  "list" },
    { ListItem_t,  "item" },
    { Metric_t,  "metric" },
    { ModalitiesLayer_t,  "modalities" },
    { Modality_t,  "modality" },
    { ModalityFeature_t,  "mod" },
    { Morpheme_t,  "morpheme" },
    { MorphologyLayer_t,  "morphology" },
    { New_t,  "new" },
    { Note_t,  "note" },
    { Observation_t,  "observation" },
    { ObservationLayer_t,  "observations" },
    { Original_t,  "original" },
    { Paragraph_t,  "p" },
    { Part_t,  "part" },
    { PhonContent_t,  "ph" },
    { Phoneme_t,  "phoneme" },
    { PhonologyLayer_t,  "phonology" },
    { PolarityFeature_t,  "polarity" },
    { PosAnnotation_t,  "pos" },
    { Predicate_t,  "predicate" },
    { Quote_t,  "quote" },
    { Reference_t,  "ref" },
    { Relation_t,  "relation" },
    { Row_t,  "row" },
    { Scope_t,  "scope" },
    { SemanticRole_t,  "semrole" },
    { SemanticRolesLayer_t,  "semroles" },
    { SenseAnnotation_t,  "sense" },
    { Sentence_t,  "s" },
    { Sentiment_t,  "sentiment" },
    { SentimentLayer_t,  "sentiments" },
    { SizeFeature_t,  "size" },
    { Source_t,  "source" },
    { SpanRelation_t,  "spanrelation" },
    { SpanRelationLayer_t,  "spanrelations" },
    { Speech_t,  "speech" },
    { Statement_t,  "statement" },
    { StatementLayer_t,  "statements" },
    { StatementRelation_t,  "rel" },
    { StrengthFeature_t,  "strength" },
    { String_t,  "str" },
    { StyleFeature_t,  "style" },
    { SubjectivityAnnotation_t,  "subjectivity" },
    { Suggestion_t,  "suggestion" },
    { SynsetFeature_t,  "synset" },
    { SyntacticUnit_t,  "su" },
    { SyntaxLayer_t,  "syntax" },
    { Table_t,  "table" },
    { TableHead_t,  "tablehead" },
    { Target_t,  "target" },
    { Term_t,  "term" },
    { Text_t,  "text" },
    { TextContent_t,  "t" },
    { TextMarkupCorrection_t,  "t-correction" },
    { TextMarkupError_t,  "t-error" },
    { TextMarkupGap_t,  "t-gap" },
    { TextMarkupHSpace_t,  "t-hspace" },
    { TextMarkupLanguage_t,  "t-lang" },
    { TextMarkupReference_t,  "t-ref" },
    { TextMarkupString_t,  "t-str" },
    { TextMarkupStyle_t,  "t-style" },
    { TextMarkupWhitespace_t,  "t-whitespace" },
    { TimeFeature_t,  "time" },
    { TimeSegment_t,  "timesegment" },
    { TimingLayer_t,  "timing" },
    { Utterance_t,  "utt" },
    { ValueFeature_t,  "value" },
    { Whitespace_t,  "whitespace" },
    { Word_t,  "w" },
    { WordReference_t,  "wref" },
    { PlaceHolder_t, "_PlaceHolder" },
    { XmlComment_t, "_XmlComment" },
    { XmlText_t, "_XmlText" }
  };

  //foliaspec:string_elementtype_map
  const map<string,ElementType> s_et_map = {
    { "FoLiA", BASE },
    { "_AbstractAnnotationLayer", AbstractAnnotationLayer_t  },
    { "_AbstractContentAnnotation", AbstractContentAnnotation_t  },
    { "_AbstractCorrectionChild", AbstractCorrectionChild_t  },
    { "_AbstractHigherOrderAnnotation", AbstractHigherOrderAnnotation_t  },
    { "_AbstractInlineAnnotation", AbstractInlineAnnotation_t  },
    { "_AbstractSpanAnnotation", AbstractSpanAnnotation_t  },
    { "_AbstractSpanRole", AbstractSpanRole_t  },
    { "_AbstractStructureElement", AbstractStructureElement_t  },
    { "_AbstractSubtokenAnnotation", AbstractSubtokenAnnotation_t  },
    { "_AbstractTextMarkup", AbstractTextMarkup_t  },
    { "actor", ActorFeature_t  },
    { "alt", Alternative_t  },
    { "altlayers", AlternativeLayers_t  },
    { "begindatetime", BegindatetimeFeature_t  },
    { "caption", Caption_t  },
    { "cell", Cell_t  },
    { "chunk", Chunk_t  },
    { "chunking", ChunkingLayer_t  },
    { "comment", Comment_t  },
    { "content", Content_t  },
    { "coreferencechain", CoreferenceChain_t  },
    { "coreferences", CoreferenceLayer_t  },
    { "coreferencelink", CoreferenceLink_t  },
    { "correction", Correction_t  },
    { "cue", Cue_t  },
    { "current", Current_t  },
    { "def", Definition_t  },
    { "dependencies", DependenciesLayer_t  },
    { "dependency", Dependency_t  },
    { "dep", DependencyDependent_t  },
    { "desc", Description_t  },
    { "div", Division_t  },
    { "domain", DomainAnnotation_t  },
    { "enddatetime", EnddatetimeFeature_t  },
    { "entities", EntitiesLayer_t  },
    { "entity", Entity_t  },
    { "entry", Entry_t  },
    { "errordetection", ErrorDetection_t  },
    { "event", Event_t  },
    { "ex", Example_t  },
    { "external", External_t  },
    { "feat", Feature_t  },
    { "figure", Figure_t  },
    { "font", FontFeature_t  },
    { "foreign-data", ForeignData_t  },
    { "function", FunctionFeature_t  },
    { "gap", Gap_t  },
    { "head", Head_t  },
    { "headfeature", HeadFeature_t },
    { "hd", Headspan_t  },
    { "hiddenw", Hiddenword_t  },
    { "t-hbr", Hyphbreak_t  },
    { "label", Label_t  },
    { "lang", LangAnnotation_t  },
    { "lemma", LemmaAnnotation_t  },
    { "level", LevelFeature_t  },
    { "br", Linebreak_t  },
    { "xref", LinkReference_t  },
    { "list", List_t  },
    { "item", ListItem_t  },
    { "metric", Metric_t  },
    { "modalities", ModalitiesLayer_t  },
    { "modality", Modality_t  },
    { "mod", ModalityFeature_t  },
    { "morpheme", Morpheme_t  },
    { "morphology", MorphologyLayer_t  },
    { "new", New_t  },
    { "note", Note_t  },
    { "observation", Observation_t  },
    { "observations", ObservationLayer_t  },
    { "original", Original_t  },
    { "p", Paragraph_t  },
    { "part", Part_t  },
    { "ph", PhonContent_t  },
    { "phoneme", Phoneme_t  },
    { "phonology", PhonologyLayer_t  },
    { "polarity", PolarityFeature_t  },
    { "pos", PosAnnotation_t  },
    { "predicate", Predicate_t  },
    { "quote", Quote_t  },
    { "ref", Reference_t  },
    { "relation", Relation_t  },
    { "row", Row_t  },
    { "scope", Scope_t  },
    { "semrole", SemanticRole_t  },
    { "semroles", SemanticRolesLayer_t  },
    { "sense", SenseAnnotation_t  },
    { "s", Sentence_t  },
    { "sentiment", Sentiment_t  },
    { "sentiments", SentimentLayer_t  },
    { "size", SizeFeature_t  },
    { "source", Source_t  },
    { "spanrelation", SpanRelation_t  },
    { "spanrelations", SpanRelationLayer_t  },
    { "speech", Speech_t  },
    { "statement", Statement_t  },
    { "statements", StatementLayer_t  },
    { "rel", StatementRelation_t  },
    { "strength", StrengthFeature_t  },
    { "str", String_t  },
    { "style", StyleFeature_t  },
    { "subjectivity", SubjectivityAnnotation_t  },
    { "suggestion", Suggestion_t  },
    { "synset", SynsetFeature_t  },
    { "su", SyntacticUnit_t  },
    { "syntax", SyntaxLayer_t  },
    { "table", Table_t  },
    { "tablehead", TableHead_t  },
    { "target", Target_t  },
    { "term", Term_t  },
    { "text", Text_t  },
    { "t", TextContent_t  },
    { "t-correction", TextMarkupCorrection_t  },
    { "t-error", TextMarkupError_t  },
    { "t-gap", TextMarkupGap_t  },
    { "t-hspace", TextMarkupHSpace_t  },
    { "t-lang", TextMarkupLanguage_t  },
    { "t-ref", TextMarkupReference_t  },
    { "t-str", TextMarkupString_t  },
    { "t-style", TextMarkupStyle_t  },
    { "t-whitespace", TextMarkupWhitespace_t  },
    { "time", TimeFeature_t  },
    { "timesegment", TimeSegment_t  },
    { "timing", TimingLayer_t  },
    { "utt", Utterance_t  },
    { "value", ValueFeature_t  },
    { "whitespace", Whitespace_t  },
    { "w", Word_t  },
    { "wref", WordReference_t  },
    { "_PlaceHolder", PlaceHolder_t  },
    { "_XmlComment", XmlComment_t  },
    { "_XmlText", XmlText_t  }
  };

  //foliaspec:default_ignore
  //Default ignore list for the select() method, do not descend into these
  const set<ElementType> default_ignore = { Alternative_t, AlternativeLayers_t, ForeignData_t, Original_t, Suggestion_t };

  //foliaspec:default_ignore_annotations
  //Default ignore list for token annotation
  const set<ElementType> default_ignore_annotations = { Alternative_t, AlternativeLayers_t, MorphologyLayer_t, Original_t, PhonologyLayer_t, Suggestion_t };

  //foliaspec:default_ignore_structure
  //Default ignore list for structure annotation
  const set<ElementType> default_ignore_structure = { Alternative_t, AlternativeLayers_t, ChunkingLayer_t, CoreferenceLayer_t, DependenciesLayer_t, EntitiesLayer_t, ModalitiesLayer_t, MorphologyLayer_t, ObservationLayer_t, Original_t, PhonologyLayer_t, SemanticRolesLayer_t, SentimentLayer_t, SpanRelationLayer_t, StatementLayer_t, Suggestion_t, SyntaxLayer_t, TimingLayer_t };

  const set<ElementType> AnnoExcludeSet = { Original_t, Suggestion_t };

  const set<ElementType> SpanSet = { SyntacticUnit_t,
				     Chunk_t,
				     Entity_t,
				     Headspan_t,
				     DependencyDependent_t,
				     CoreferenceLink_t,
				     CoreferenceChain_t,
				     SemanticRole_t,
				     SemanticRolesLayer_t,
				     TimeSegment_t };

  properties DEFAULT_PROPERTIES;

  //these are not in the external specification yet:
  properties FoLiA::PROPS = DEFAULT_PROPERTIES;
  properties DCOI::PROPS = DEFAULT_PROPERTIES;
  properties XmlComment::PROPS = DEFAULT_PROPERTIES;
  properties PlaceHolder::PROPS = DEFAULT_PROPERTIES;
  properties XmlText::PROPS = DEFAULT_PROPERTIES;

  //foliaspec:instantiateelementproperties
  //Instantiates all element properties for the first time, setting them to the default properties
  properties AbstractAnnotationLayer::PROPS = DEFAULT_PROPERTIES;
  properties AbstractContentAnnotation::PROPS = DEFAULT_PROPERTIES;
  properties AbstractCorrectionChild::PROPS = DEFAULT_PROPERTIES;
  properties AbstractHigherOrderAnnotation::PROPS = DEFAULT_PROPERTIES;
  properties AbstractInlineAnnotation::PROPS = DEFAULT_PROPERTIES;
  properties AbstractSpanAnnotation::PROPS = DEFAULT_PROPERTIES;
  properties AbstractSpanRole::PROPS = DEFAULT_PROPERTIES;
  properties AbstractStructureElement::PROPS = DEFAULT_PROPERTIES;
  properties AbstractSubtokenAnnotation::PROPS = DEFAULT_PROPERTIES;
  properties AbstractTextMarkup::PROPS = DEFAULT_PROPERTIES;
  properties ActorFeature::PROPS = DEFAULT_PROPERTIES;
  properties Alternative::PROPS = DEFAULT_PROPERTIES;
  properties AlternativeLayers::PROPS = DEFAULT_PROPERTIES;
  properties BegindatetimeFeature::PROPS = DEFAULT_PROPERTIES;
  properties Caption::PROPS = DEFAULT_PROPERTIES;
  properties Cell::PROPS = DEFAULT_PROPERTIES;
  properties Chunk::PROPS = DEFAULT_PROPERTIES;
  properties ChunkingLayer::PROPS = DEFAULT_PROPERTIES;
  properties Comment::PROPS = DEFAULT_PROPERTIES;
  properties Content::PROPS = DEFAULT_PROPERTIES;
  properties CoreferenceChain::PROPS = DEFAULT_PROPERTIES;
  properties CoreferenceLayer::PROPS = DEFAULT_PROPERTIES;
  properties CoreferenceLink::PROPS = DEFAULT_PROPERTIES;
  properties Correction::PROPS = DEFAULT_PROPERTIES;
  properties Cue::PROPS = DEFAULT_PROPERTIES;
  properties Current::PROPS = DEFAULT_PROPERTIES;
  properties Definition::PROPS = DEFAULT_PROPERTIES;
  properties DependenciesLayer::PROPS = DEFAULT_PROPERTIES;
  properties Dependency::PROPS = DEFAULT_PROPERTIES;
  properties DependencyDependent::PROPS = DEFAULT_PROPERTIES;
  properties Description::PROPS = DEFAULT_PROPERTIES;
  properties Division::PROPS = DEFAULT_PROPERTIES;
  properties DomainAnnotation::PROPS = DEFAULT_PROPERTIES;
  properties EnddatetimeFeature::PROPS = DEFAULT_PROPERTIES;
  properties EntitiesLayer::PROPS = DEFAULT_PROPERTIES;
  properties Entity::PROPS = DEFAULT_PROPERTIES;
  properties Entry::PROPS = DEFAULT_PROPERTIES;
  properties ErrorDetection::PROPS = DEFAULT_PROPERTIES;
  properties Event::PROPS = DEFAULT_PROPERTIES;
  properties Example::PROPS = DEFAULT_PROPERTIES;
  properties External::PROPS = DEFAULT_PROPERTIES;
  properties Feature::PROPS = DEFAULT_PROPERTIES;
  properties Figure::PROPS = DEFAULT_PROPERTIES;
  properties FontFeature::PROPS = DEFAULT_PROPERTIES;
  properties ForeignData::PROPS = DEFAULT_PROPERTIES;
  properties FunctionFeature::PROPS = DEFAULT_PROPERTIES;
  properties Gap::PROPS = DEFAULT_PROPERTIES;
  properties Head::PROPS = DEFAULT_PROPERTIES;
  properties HeadFeature::PROPS = DEFAULT_PROPERTIES;
  properties Headspan::PROPS = DEFAULT_PROPERTIES;
  properties Hiddenword::PROPS = DEFAULT_PROPERTIES;
  properties Hyphbreak::PROPS = DEFAULT_PROPERTIES;
  properties Label::PROPS = DEFAULT_PROPERTIES;
  properties LangAnnotation::PROPS = DEFAULT_PROPERTIES;
  properties LemmaAnnotation::PROPS = DEFAULT_PROPERTIES;
  properties LevelFeature::PROPS = DEFAULT_PROPERTIES;
  properties Linebreak::PROPS = DEFAULT_PROPERTIES;
  properties LinkReference::PROPS = DEFAULT_PROPERTIES;
  properties List::PROPS = DEFAULT_PROPERTIES;
  properties ListItem::PROPS = DEFAULT_PROPERTIES;
  properties Metric::PROPS = DEFAULT_PROPERTIES;
  properties ModalitiesLayer::PROPS = DEFAULT_PROPERTIES;
  properties Modality::PROPS = DEFAULT_PROPERTIES;
  properties ModalityFeature::PROPS = DEFAULT_PROPERTIES;
  properties Morpheme::PROPS = DEFAULT_PROPERTIES;
  properties MorphologyLayer::PROPS = DEFAULT_PROPERTIES;
  properties New::PROPS = DEFAULT_PROPERTIES;
  properties Note::PROPS = DEFAULT_PROPERTIES;
  properties Observation::PROPS = DEFAULT_PROPERTIES;
  properties ObservationLayer::PROPS = DEFAULT_PROPERTIES;
  properties Original::PROPS = DEFAULT_PROPERTIES;
  properties Paragraph::PROPS = DEFAULT_PROPERTIES;
  properties Part::PROPS = DEFAULT_PROPERTIES;
  properties PhonContent::PROPS = DEFAULT_PROPERTIES;
  properties Phoneme::PROPS = DEFAULT_PROPERTIES;
  properties PhonologyLayer::PROPS = DEFAULT_PROPERTIES;
  properties PolarityFeature::PROPS = DEFAULT_PROPERTIES;
  properties PosAnnotation::PROPS = DEFAULT_PROPERTIES;
  properties Predicate::PROPS = DEFAULT_PROPERTIES;
  properties Quote::PROPS = DEFAULT_PROPERTIES;
  properties Reference::PROPS = DEFAULT_PROPERTIES;
  properties Relation::PROPS = DEFAULT_PROPERTIES;
  properties Row::PROPS = DEFAULT_PROPERTIES;
  properties Scope::PROPS = DEFAULT_PROPERTIES;
  properties SemanticRole::PROPS = DEFAULT_PROPERTIES;
  properties SemanticRolesLayer::PROPS = DEFAULT_PROPERTIES;
  properties SenseAnnotation::PROPS = DEFAULT_PROPERTIES;
  properties Sentence::PROPS = DEFAULT_PROPERTIES;
  properties Sentiment::PROPS = DEFAULT_PROPERTIES;
  properties SentimentLayer::PROPS = DEFAULT_PROPERTIES;
  properties SizeFeature::PROPS = DEFAULT_PROPERTIES;
  properties Source::PROPS = DEFAULT_PROPERTIES;
  properties SpanRelation::PROPS = DEFAULT_PROPERTIES;
  properties SpanRelationLayer::PROPS = DEFAULT_PROPERTIES;
  properties Speech::PROPS = DEFAULT_PROPERTIES;
  properties Statement::PROPS = DEFAULT_PROPERTIES;
  properties StatementLayer::PROPS = DEFAULT_PROPERTIES;
  properties StatementRelation::PROPS = DEFAULT_PROPERTIES;
  properties StrengthFeature::PROPS = DEFAULT_PROPERTIES;
  properties String::PROPS = DEFAULT_PROPERTIES;
  properties StyleFeature::PROPS = DEFAULT_PROPERTIES;
  properties SubjectivityAnnotation::PROPS = DEFAULT_PROPERTIES;
  properties Suggestion::PROPS = DEFAULT_PROPERTIES;
  properties SynsetFeature::PROPS = DEFAULT_PROPERTIES;
  properties SyntacticUnit::PROPS = DEFAULT_PROPERTIES;
  properties SyntaxLayer::PROPS = DEFAULT_PROPERTIES;
  properties Table::PROPS = DEFAULT_PROPERTIES;
  properties TableHead::PROPS = DEFAULT_PROPERTIES;
  properties Target::PROPS = DEFAULT_PROPERTIES;
  properties Term::PROPS = DEFAULT_PROPERTIES;
  properties Text::PROPS = DEFAULT_PROPERTIES;
  properties TextContent::PROPS = DEFAULT_PROPERTIES;
  properties TextMarkupCorrection::PROPS = DEFAULT_PROPERTIES;
  properties TextMarkupError::PROPS = DEFAULT_PROPERTIES;
  properties TextMarkupGap::PROPS = DEFAULT_PROPERTIES;
  properties TextMarkupHSpace::PROPS = DEFAULT_PROPERTIES;
  properties TextMarkupLanguage::PROPS = DEFAULT_PROPERTIES;
  properties TextMarkupReference::PROPS = DEFAULT_PROPERTIES;
  properties TextMarkupString::PROPS = DEFAULT_PROPERTIES;
  properties TextMarkupStyle::PROPS = DEFAULT_PROPERTIES;
  properties TextMarkupWhitespace::PROPS = DEFAULT_PROPERTIES;
  properties TimeFeature::PROPS = DEFAULT_PROPERTIES;
  properties TimeSegment::PROPS = DEFAULT_PROPERTIES;
  properties TimingLayer::PROPS = DEFAULT_PROPERTIES;
  properties Utterance::PROPS = DEFAULT_PROPERTIES;
  properties ValueFeature::PROPS = DEFAULT_PROPERTIES;
  properties Whitespace::PROPS = DEFAULT_PROPERTIES;
  properties Word::PROPS = DEFAULT_PROPERTIES;
  properties WordReference::PROPS = DEFAULT_PROPERTIES;

  set<ElementType>& operator+=( set<ElementType>& a ,
				const set<ElementType>& b ){
    a.insert(b.begin(), b.end());
    return a;
  }

  map<ElementType,properties*> element_props;

  void static_init(){
    /// initialize a lot of statics ('constants')
    /// This function should be called once.
    FoLiA::PROPS.XMLTAG = "FoLiA";
    FoLiA::PROPS.ACCEPTED_DATA += { Text_t, Speech_t };
    FoLiA::PROPS.OPTIONAL_ATTRIBS = ID;
    element_props[BASE] = &FoLiA::PROPS;

    DCOI::PROPS.XMLTAG = "DCOI";
    DCOI::PROPS.ACCEPTED_DATA += { Text_t, Speech_t };

    //foliaspec:begin:setelementproperties
    //Sets all element properties for all elements
//------ AbstractAnnotationLayer -------
    AbstractAnnotationLayer::PROPS.ELEMENT_ID = AbstractAnnotationLayer_t;
    AbstractAnnotationLayer::PROPS.ACCEPTED_DATA += {Comment_t, Correction_t, Description_t, ForeignData_t};
    AbstractAnnotationLayer::PROPS.OPTIONAL_ATTRIBS = ID|TAG;
    AbstractAnnotationLayer::PROPS.PRINTABLE = false;
    AbstractAnnotationLayer::PROPS.SETONLY = true;
    AbstractAnnotationLayer::PROPS.SPEAKABLE = false;
    element_props[AbstractAnnotationLayer_t] = &AbstractAnnotationLayer::PROPS;
//------ AbstractContentAnnotation -------
    AbstractContentAnnotation::PROPS.ELEMENT_ID = AbstractContentAnnotation_t;
    AbstractContentAnnotation::PROPS.OCCURRENCES = 0;
    AbstractContentAnnotation::PROPS.OPTIONAL_ATTRIBS = CLASS|ANNOTATOR|CONFIDENCE|DATETIME|METADATA|TAG;
    AbstractContentAnnotation::PROPS.REQUIRED_ATTRIBS = NO_ATT;
    element_props[AbstractContentAnnotation_t] = &AbstractContentAnnotation::PROPS;
//------ AbstractCorrectionChild -------
    AbstractCorrectionChild::PROPS.ELEMENT_ID = AbstractCorrectionChild_t;
    AbstractCorrectionChild::PROPS.ACCEPTED_DATA += {AbstractInlineAnnotation_t, AbstractSpanAnnotation_t, AbstractStructureElement_t, Comment_t, Correction_t, Description_t, ForeignData_t, Metric_t, PhonContent_t, String_t, TextContent_t};
    AbstractCorrectionChild::PROPS.OPTIONAL_ATTRIBS = ID|ANNOTATOR|CONFIDENCE|DATETIME|N|TAG;
    AbstractCorrectionChild::PROPS.PRINTABLE = true;
    AbstractCorrectionChild::PROPS.SPEAKABLE = true;
    AbstractCorrectionChild::PROPS.TEXTDELIMITER = "NONE";
    element_props[AbstractCorrectionChild_t] = &AbstractCorrectionChild::PROPS;
//------ AbstractHigherOrderAnnotation -------
    AbstractHigherOrderAnnotation::PROPS.ELEMENT_ID = AbstractHigherOrderAnnotation_t;
    AbstractHigherOrderAnnotation::PROPS.OPTIONAL_ATTRIBS = NO_ATT;
    AbstractHigherOrderAnnotation::PROPS.REQUIRED_ATTRIBS = NO_ATT;
    element_props[AbstractHigherOrderAnnotation_t] = &AbstractHigherOrderAnnotation::PROPS;
//------ AbstractInlineAnnotation -------
    AbstractInlineAnnotation::PROPS.ELEMENT_ID = AbstractInlineAnnotation_t;
    AbstractInlineAnnotation::PROPS.ACCEPTED_DATA += {Comment_t, Description_t, Feature_t, ForeignData_t, Metric_t};
    AbstractInlineAnnotation::PROPS.OCCURRENCES_PER_SET = 1;
    AbstractInlineAnnotation::PROPS.OPTIONAL_ATTRIBS = ID|CLASS|ANNOTATOR|N|CONFIDENCE|DATETIME|SRC|BEGINTIME|ENDTIME|SPEAKER|TEXTCLASS|METADATA|TAG;
    AbstractInlineAnnotation::PROPS.REQUIRED_ATTRIBS = CLASS;
    element_props[AbstractInlineAnnotation_t] = &AbstractInlineAnnotation::PROPS;
//------ AbstractSpanAnnotation -------
    AbstractSpanAnnotation::PROPS.ELEMENT_ID = AbstractSpanAnnotation_t;
    AbstractSpanAnnotation::PROPS.ACCEPTED_DATA += {AbstractInlineAnnotation_t, Comment_t, Description_t, ForeignData_t, LinkReference_t, Metric_t, Relation_t};
    AbstractSpanAnnotation::PROPS.OPTIONAL_ATTRIBS = ID|CLASS|ANNOTATOR|N|CONFIDENCE|DATETIME|SRC|BEGINTIME|ENDTIME|SPEAKER|TEXTCLASS|METADATA|TAG;
    AbstractSpanAnnotation::PROPS.PRINTABLE = true;
    AbstractSpanAnnotation::PROPS.SPEAKABLE = true;
    element_props[AbstractSpanAnnotation_t] = &AbstractSpanAnnotation::PROPS;
//------ AbstractSpanRole -------
    AbstractSpanRole::PROPS = AbstractSpanAnnotation::PROPS;
    AbstractSpanRole::PROPS.ELEMENT_ID = AbstractSpanRole_t;
    AbstractSpanRole::PROPS.ACCEPTED_DATA += {AbstractInlineAnnotation_t, Comment_t, Description_t, Feature_t, ForeignData_t, LinkReference_t, Metric_t, Relation_t, WordReference_t, Hiddenword_t, Morpheme_t, Phoneme_t, Word_t};
    AbstractSpanRole::PROPS.OPTIONAL_ATTRIBS = ID|TAG;
    element_props[AbstractSpanRole_t] = &AbstractSpanRole::PROPS;
//------ AbstractStructureElement -------
    AbstractStructureElement::PROPS.ELEMENT_ID = AbstractStructureElement_t;
    AbstractStructureElement::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, Alternative_t, AlternativeLayers_t, Comment_t, Correction_t, Description_t, External_t, Feature_t, ForeignData_t, Metric_t, Part_t, Relation_t};
    AbstractStructureElement::PROPS.AUTO_GENERATE_ID = true;
    AbstractStructureElement::PROPS.OPTIONAL_ATTRIBS = ID|CLASS|ANNOTATOR|N|CONFIDENCE|DATETIME|SRC|BEGINTIME|ENDTIME|SPEAKER|METADATA|SPACE|TAG;
    AbstractStructureElement::PROPS.PRINTABLE = true;
    AbstractStructureElement::PROPS.REQUIRED_ATTRIBS = NO_ATT;
    AbstractStructureElement::PROPS.SPEAKABLE = true;
    AbstractStructureElement::PROPS.TEXTDELIMITER = "\n\n";
    element_props[AbstractStructureElement_t] = &AbstractStructureElement::PROPS;
//------ AbstractSubtokenAnnotation -------
    AbstractSubtokenAnnotation::PROPS.ELEMENT_ID = AbstractSubtokenAnnotation_t;
    AbstractSubtokenAnnotation::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, Alternative_t, AlternativeLayers_t, Comment_t, Correction_t, Description_t, Feature_t, ForeignData_t, Metric_t, Part_t, Relation_t};
    AbstractSubtokenAnnotation::PROPS.AUTO_GENERATE_ID = true;
    AbstractSubtokenAnnotation::PROPS.OPTIONAL_ATTRIBS = ID|CLASS|ANNOTATOR|N|CONFIDENCE|DATETIME|SRC|BEGINTIME|ENDTIME|SPEAKER|METADATA|TAG;
    AbstractSubtokenAnnotation::PROPS.PRINTABLE = true;
    AbstractSubtokenAnnotation::PROPS.REQUIRED_ATTRIBS = NO_ATT;
    AbstractSubtokenAnnotation::PROPS.SPEAKABLE = true;
    AbstractSubtokenAnnotation::PROPS.TEXTDELIMITER = "\n\n";
    element_props[AbstractSubtokenAnnotation_t] = &AbstractSubtokenAnnotation::PROPS;
//------ AbstractTextMarkup -------
    AbstractTextMarkup::PROPS.ELEMENT_ID = AbstractTextMarkup_t;
    AbstractTextMarkup::PROPS.ACCEPTED_DATA += {AbstractTextMarkup_t, Comment_t, Description_t, Feature_t, Linebreak_t, XmlText_t};
    AbstractTextMarkup::PROPS.OPTIONAL_ATTRIBS = ID|CLASS|ANNOTATOR|N|CONFIDENCE|DATETIME|SRC|BEGINTIME|ENDTIME|SPEAKER|METADATA|TAG;
    AbstractTextMarkup::PROPS.PRINTABLE = true;
    AbstractTextMarkup::PROPS.TEXTCONTAINER = true;
    AbstractTextMarkup::PROPS.TEXTDELIMITER = "";
    AbstractTextMarkup::PROPS.XLINK = true;
    element_props[AbstractTextMarkup_t] = &AbstractTextMarkup::PROPS;
//------ Alternative -------
    Alternative::PROPS = AbstractHigherOrderAnnotation::PROPS;
    Alternative::PROPS.ELEMENT_ID = Alternative_t;
    Alternative::PROPS.ACCEPTED_DATA += {AbstractInlineAnnotation_t, Comment_t, Correction_t, Description_t, ForeignData_t, MorphologyLayer_t, PhonologyLayer_t};
    Alternative::PROPS.ANNOTATIONTYPE = AnnotationType::ALTERNATIVE;
    Alternative::PROPS.AUTH = false;
    Alternative::PROPS.LABEL = "Alternative";
    Alternative::PROPS.OPTIONAL_ATTRIBS = ID|ANNOTATOR|N|CONFIDENCE|DATETIME|SRC|BEGINTIME|ENDTIME|SPEAKER|METADATA|TAG;
    Alternative::PROPS.PRINTABLE = false;
    Alternative::PROPS.REQUIRED_ATTRIBS = NO_ATT;
    Alternative::PROPS.SPEAKABLE = false;
    Alternative::PROPS.XMLTAG = "alt";
    element_props[Alternative_t] = &Alternative::PROPS;
//------ AlternativeLayers -------
    AlternativeLayers::PROPS = AbstractHigherOrderAnnotation::PROPS;
    AlternativeLayers::PROPS.ELEMENT_ID = AlternativeLayers_t;
    AlternativeLayers::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, Comment_t, Description_t, ForeignData_t};
    AlternativeLayers::PROPS.ANNOTATIONTYPE = AnnotationType::ALTERNATIVE;
    AlternativeLayers::PROPS.AUTH = false;
    AlternativeLayers::PROPS.LABEL = "Alternative Layers";
    AlternativeLayers::PROPS.OPTIONAL_ATTRIBS = ID|ANNOTATOR|N|CONFIDENCE|DATETIME|SRC|BEGINTIME|ENDTIME|SPEAKER|METADATA|TAG;
    AlternativeLayers::PROPS.PRINTABLE = false;
    AlternativeLayers::PROPS.REQUIRED_ATTRIBS = NO_ATT;
    AlternativeLayers::PROPS.SPEAKABLE = false;
    AlternativeLayers::PROPS.XMLTAG = "altlayers";
    element_props[AlternativeLayers_t] = &AlternativeLayers::PROPS;
//------ Caption -------
    Caption::PROPS = AbstractStructureElement::PROPS;
    Caption::PROPS.ELEMENT_ID = Caption_t;
    Caption::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, AbstractInlineAnnotation_t, Alternative_t, AlternativeLayers_t, Comment_t, Correction_t, Description_t, External_t, Feature_t, ForeignData_t, Gap_t, Linebreak_t, Metric_t, Paragraph_t, Part_t, PhonContent_t, Quote_t, Reference_t, Relation_t, Sentence_t, String_t, TextContent_t, Whitespace_t};
    Caption::PROPS.LABEL = "Caption";
    Caption::PROPS.OCCURRENCES = 1;
    Caption::PROPS.OPTIONAL_ATTRIBS = ID|ANNOTATOR|N|CONFIDENCE|DATETIME|SRC|BEGINTIME|ENDTIME|SPEAKER|METADATA|SPACE|TAG;
    Caption::PROPS.XMLTAG = "caption";
    element_props[Caption_t] = &Caption::PROPS;
//------ Cell -------
    Cell::PROPS = AbstractStructureElement::PROPS;
    Cell::PROPS.ELEMENT_ID = Cell_t;
    Cell::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, AbstractInlineAnnotation_t, Alternative_t, AlternativeLayers_t, Comment_t, Correction_t, Description_t, Entry_t, Event_t, Example_t, External_t, Feature_t, Figure_t, ForeignData_t, Gap_t, Head_t, Hiddenword_t, Linebreak_t, List_t, Metric_t, Note_t, Paragraph_t, Part_t, Quote_t, Reference_t, Relation_t, Sentence_t, String_t, TextContent_t, Whitespace_t, Word_t};
    Cell::PROPS.LABEL = "Cell";
    Cell::PROPS.OPTIONAL_ATTRIBS = ID|ANNOTATOR|N|CONFIDENCE|DATETIME|SRC|BEGINTIME|ENDTIME|SPEAKER|METADATA|SPACE|TAG;
    Cell::PROPS.TEXTDELIMITER = " | ";
    Cell::PROPS.XMLTAG = "cell";
    element_props[Cell_t] = &Cell::PROPS;
//------ Chunk -------
    Chunk::PROPS = AbstractSpanAnnotation::PROPS;
    Chunk::PROPS.ELEMENT_ID = Chunk_t;
    Chunk::PROPS.ACCEPTED_DATA += {AbstractInlineAnnotation_t, Comment_t, Description_t, Feature_t, ForeignData_t, LinkReference_t, Metric_t, Relation_t, WordReference_t, Hiddenword_t, Morpheme_t, Phoneme_t, Word_t};
    Chunk::PROPS.ANNOTATIONTYPE = AnnotationType::CHUNKING;
    Chunk::PROPS.LABEL = "Chunk";
    Chunk::PROPS.XMLTAG = "chunk";
    element_props[Chunk_t] = &Chunk::PROPS;
//------ ChunkingLayer -------
    ChunkingLayer::PROPS = AbstractAnnotationLayer::PROPS;
    ChunkingLayer::PROPS.ELEMENT_ID = ChunkingLayer_t;
    ChunkingLayer::PROPS.ACCEPTED_DATA += {Chunk_t, Comment_t, Correction_t, Description_t, ForeignData_t};
    ChunkingLayer::PROPS.ANNOTATIONTYPE = AnnotationType::CHUNKING;
    ChunkingLayer::PROPS.XMLTAG = "chunking";
    element_props[ChunkingLayer_t] = &ChunkingLayer::PROPS;
//------ Comment -------
    Comment::PROPS = AbstractHigherOrderAnnotation::PROPS;
    Comment::PROPS.ELEMENT_ID = Comment_t;
    Comment::PROPS.ANNOTATIONTYPE = AnnotationType::COMMENT;
    Comment::PROPS.LABEL = "Comment";
    Comment::PROPS.OPTIONAL_ATTRIBS = ID|ANNOTATOR|CONFIDENCE|DATETIME|N|METADATA|TAG;
    Comment::PROPS.PRINTABLE = false;
    Comment::PROPS.XMLTAG = "comment";
    element_props[Comment_t] = &Comment::PROPS;
//------ Content -------
    Content::PROPS = AbstractContentAnnotation::PROPS;
    Content::PROPS.ELEMENT_ID = Content_t;
    Content::PROPS.ANNOTATIONTYPE = AnnotationType::RAWCONTENT;
    Content::PROPS.LABEL = "Raw Content";
    Content::PROPS.OCCURRENCES = 1;
    Content::PROPS.PRINTABLE = true;
    Content::PROPS.XMLTAG = "content";
    element_props[Content_t] = &Content::PROPS;
//------ CoreferenceChain -------
    CoreferenceChain::PROPS = AbstractSpanAnnotation::PROPS;
    CoreferenceChain::PROPS.ELEMENT_ID = CoreferenceChain_t;
    CoreferenceChain::PROPS.ACCEPTED_DATA += {AbstractInlineAnnotation_t, Comment_t, CoreferenceLink_t, Description_t, Feature_t, ForeignData_t, LinkReference_t, Metric_t, Relation_t};
    CoreferenceChain::PROPS.ANNOTATIONTYPE = AnnotationType::COREFERENCE;
    CoreferenceChain::PROPS.LABEL = "Coreference Chain";
    CoreferenceChain::PROPS.REQUIRED_DATA += {CoreferenceLink_t};
    CoreferenceChain::PROPS.XMLTAG = "coreferencechain";
    element_props[CoreferenceChain_t] = &CoreferenceChain::PROPS;
//------ CoreferenceLayer -------
    CoreferenceLayer::PROPS = AbstractAnnotationLayer::PROPS;
    CoreferenceLayer::PROPS.ELEMENT_ID = CoreferenceLayer_t;
    CoreferenceLayer::PROPS.ACCEPTED_DATA += {Comment_t, CoreferenceChain_t, Correction_t, Description_t, ForeignData_t};
    CoreferenceLayer::PROPS.ANNOTATIONTYPE = AnnotationType::COREFERENCE;
    CoreferenceLayer::PROPS.XMLTAG = "coreferences";
    element_props[CoreferenceLayer_t] = &CoreferenceLayer::PROPS;
//------ CoreferenceLink -------
    CoreferenceLink::PROPS = AbstractSpanRole::PROPS;
    CoreferenceLink::PROPS.ELEMENT_ID = CoreferenceLink_t;
    CoreferenceLink::PROPS.ACCEPTED_DATA += {AbstractInlineAnnotation_t, Comment_t, Description_t, Feature_t, ForeignData_t, Headspan_t, LevelFeature_t, LinkReference_t, Metric_t, ModalityFeature_t, Relation_t, TimeFeature_t, WordReference_t, Hiddenword_t, Morpheme_t, Phoneme_t, Word_t};
    CoreferenceLink::PROPS.ANNOTATIONTYPE = AnnotationType::COREFERENCE;
    CoreferenceLink::PROPS.LABEL = "Coreference Link";
    CoreferenceLink::PROPS.XMLTAG = "coreferencelink";
    element_props[CoreferenceLink_t] = &CoreferenceLink::PROPS;
//------ Correction -------
    Correction::PROPS = AbstractHigherOrderAnnotation::PROPS;
    Correction::PROPS.ELEMENT_ID = Correction_t;
    Correction::PROPS.ACCEPTED_DATA += {Comment_t, Current_t, Description_t, ErrorDetection_t, Feature_t, ForeignData_t, Metric_t, New_t, Original_t, Suggestion_t};
    Correction::PROPS.ANNOTATIONTYPE = AnnotationType::CORRECTION;
    Correction::PROPS.LABEL = "Correction";
    Correction::PROPS.OPTIONAL_ATTRIBS = ID|CLASS|ANNOTATOR|N|CONFIDENCE|DATETIME|SRC|BEGINTIME|ENDTIME|SPEAKER|METADATA|TAG;
    Correction::PROPS.PRINTABLE = true;
    Correction::PROPS.SPEAKABLE = true;
    Correction::PROPS.TEXTDELIMITER = "NONE";
    Correction::PROPS.XMLTAG = "correction";
    element_props[Correction_t] = &Correction::PROPS;
//------ Cue -------
    Cue::PROPS = AbstractSpanRole::PROPS;
    Cue::PROPS.ELEMENT_ID = Cue_t;
    Cue::PROPS.LABEL = "Cue";
    Cue::PROPS.OCCURRENCES = 1;
    Cue::PROPS.XMLTAG = "cue";
    element_props[Cue_t] = &Cue::PROPS;
//------ Current -------
    Current::PROPS = AbstractCorrectionChild::PROPS;
    Current::PROPS.ELEMENT_ID = Current_t;
    Current::PROPS.ANNOTATIONTYPE = AnnotationType::CORRECTION;
    Current::PROPS.OCCURRENCES = 1;
    Current::PROPS.OPTIONAL_ATTRIBS = TAG;
    Current::PROPS.XMLTAG = "current";
    element_props[Current_t] = &Current::PROPS;
//------ Definition -------
    Definition::PROPS = AbstractStructureElement::PROPS;
    Definition::PROPS.ELEMENT_ID = Definition_t;
    Definition::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, AbstractInlineAnnotation_t, Alternative_t, AlternativeLayers_t, Comment_t, Correction_t, Description_t, External_t, Feature_t, Figure_t, ForeignData_t, Hiddenword_t, Linebreak_t, List_t, Metric_t, Paragraph_t, Part_t, PhonContent_t, Reference_t, Relation_t, Sentence_t, String_t, Table_t, TextContent_t, Utterance_t, Whitespace_t, Word_t};
    Definition::PROPS.ANNOTATIONTYPE = AnnotationType::DEFINITION;
    Definition::PROPS.LABEL = "Definition";
    Definition::PROPS.XMLTAG = "def";
    element_props[Definition_t] = &Definition::PROPS;
//------ DependenciesLayer -------
    DependenciesLayer::PROPS = AbstractAnnotationLayer::PROPS;
    DependenciesLayer::PROPS.ELEMENT_ID = DependenciesLayer_t;
    DependenciesLayer::PROPS.ACCEPTED_DATA += {Comment_t, Correction_t, Dependency_t, Description_t, ForeignData_t};
    DependenciesLayer::PROPS.ANNOTATIONTYPE = AnnotationType::DEPENDENCY;
    DependenciesLayer::PROPS.XMLTAG = "dependencies";
    element_props[DependenciesLayer_t] = &DependenciesLayer::PROPS;
//------ Dependency -------
    Dependency::PROPS = AbstractSpanAnnotation::PROPS;
    Dependency::PROPS.ELEMENT_ID = Dependency_t;
    Dependency::PROPS.ACCEPTED_DATA += {AbstractInlineAnnotation_t, Comment_t, DependencyDependent_t, Description_t, Feature_t, ForeignData_t, Headspan_t, LinkReference_t, Metric_t, Relation_t};
    Dependency::PROPS.ANNOTATIONTYPE = AnnotationType::DEPENDENCY;
    Dependency::PROPS.LABEL = "Dependency";
    Dependency::PROPS.REQUIRED_DATA += {DependencyDependent_t, Headspan_t};
    Dependency::PROPS.XMLTAG = "dependency";
    element_props[Dependency_t] = &Dependency::PROPS;
//------ DependencyDependent -------
    DependencyDependent::PROPS = AbstractSpanRole::PROPS;
    DependencyDependent::PROPS.ELEMENT_ID = DependencyDependent_t;
    DependencyDependent::PROPS.LABEL = "Dependent";
    DependencyDependent::PROPS.OCCURRENCES = 1;
    DependencyDependent::PROPS.XMLTAG = "dep";
    element_props[DependencyDependent_t] = &DependencyDependent::PROPS;
//------ Description -------
    Description::PROPS = AbstractHigherOrderAnnotation::PROPS;
    Description::PROPS.ELEMENT_ID = Description_t;
    Description::PROPS.ANNOTATIONTYPE = AnnotationType::DESCRIPTION;
    Description::PROPS.LABEL = "Description";
    Description::PROPS.OCCURRENCES = 1;
    Description::PROPS.OPTIONAL_ATTRIBS = ID|ANNOTATOR|CONFIDENCE|DATETIME|N|METADATA|TAG;
    Description::PROPS.XMLTAG = "desc";
    element_props[Description_t] = &Description::PROPS;
//------ Division -------
    Division::PROPS = AbstractStructureElement::PROPS;
    Division::PROPS.ELEMENT_ID = Division_t;
    Division::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, AbstractInlineAnnotation_t, Alternative_t, AlternativeLayers_t, Comment_t, Correction_t, Description_t, Division_t, Entry_t, Event_t, Example_t, External_t, Feature_t, Figure_t, ForeignData_t, Gap_t, Head_t, Linebreak_t, List_t, Metric_t, Note_t, Paragraph_t, Part_t, PhonContent_t, Quote_t, Reference_t, Relation_t, Sentence_t, Table_t, TextContent_t, Utterance_t, Whitespace_t, Word_t};
    Division::PROPS.ANNOTATIONTYPE = AnnotationType::DIVISION;
    Division::PROPS.LABEL = "Division";
    Division::PROPS.TEXTDELIMITER = "\n\n\n";
    Division::PROPS.XMLTAG = "div";
    element_props[Division_t] = &Division::PROPS;
//------ DomainAnnotation -------
    DomainAnnotation::PROPS = AbstractInlineAnnotation::PROPS;
    DomainAnnotation::PROPS.ELEMENT_ID = DomainAnnotation_t;
    DomainAnnotation::PROPS.ANNOTATIONTYPE = AnnotationType::DOMAIN;
    DomainAnnotation::PROPS.LABEL = "Domain";
    DomainAnnotation::PROPS.OCCURRENCES_PER_SET = 0;
    DomainAnnotation::PROPS.XMLTAG = "domain";
    element_props[DomainAnnotation_t] = &DomainAnnotation::PROPS;
//------ EntitiesLayer -------
    EntitiesLayer::PROPS = AbstractAnnotationLayer::PROPS;
    EntitiesLayer::PROPS.ELEMENT_ID = EntitiesLayer_t;
    EntitiesLayer::PROPS.ACCEPTED_DATA += {Comment_t, Correction_t, Description_t, Entity_t, ForeignData_t};
    EntitiesLayer::PROPS.ANNOTATIONTYPE = AnnotationType::ENTITY;
    EntitiesLayer::PROPS.XMLTAG = "entities";
    element_props[EntitiesLayer_t] = &EntitiesLayer::PROPS;
//------ Entity -------
    Entity::PROPS = AbstractSpanAnnotation::PROPS;
    Entity::PROPS.ELEMENT_ID = Entity_t;
    Entity::PROPS.ACCEPTED_DATA += {AbstractInlineAnnotation_t, Comment_t, Description_t, Feature_t, ForeignData_t, LinkReference_t, Metric_t, Relation_t, WordReference_t, Hiddenword_t, Morpheme_t, Phoneme_t, Word_t};
    Entity::PROPS.ANNOTATIONTYPE = AnnotationType::ENTITY;
    Entity::PROPS.LABEL = "Entity";
    Entity::PROPS.XMLTAG = "entity";
    element_props[Entity_t] = &Entity::PROPS;
//------ Entry -------
    Entry::PROPS = AbstractStructureElement::PROPS;
    Entry::PROPS.ELEMENT_ID = Entry_t;
    Entry::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, Alternative_t, AlternativeLayers_t, Comment_t, Correction_t, Definition_t, Description_t, Example_t, External_t, Feature_t, ForeignData_t, Metric_t, Part_t, Relation_t, String_t, Term_t, TextContent_t};
    Entry::PROPS.ANNOTATIONTYPE = AnnotationType::ENTRY;
    Entry::PROPS.LABEL = "Entry";
    Entry::PROPS.XMLTAG = "entry";
    element_props[Entry_t] = &Entry::PROPS;
//------ ErrorDetection -------
    ErrorDetection::PROPS = AbstractInlineAnnotation::PROPS;
    ErrorDetection::PROPS.ELEMENT_ID = ErrorDetection_t;
    ErrorDetection::PROPS.ANNOTATIONTYPE = AnnotationType::ERRORDETECTION;
    ErrorDetection::PROPS.LABEL = "Error Detection";
    ErrorDetection::PROPS.OCCURRENCES_PER_SET = 0;
    ErrorDetection::PROPS.XMLTAG = "errordetection";
    element_props[ErrorDetection_t] = &ErrorDetection::PROPS;
//------ Event -------
    Event::PROPS = AbstractStructureElement::PROPS;
    Event::PROPS.ELEMENT_ID = Event_t;
    Event::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, AbstractInlineAnnotation_t, ActorFeature_t, Alternative_t, AlternativeLayers_t, BegindatetimeFeature_t, Comment_t, Correction_t, Description_t, Division_t, EnddatetimeFeature_t, Entry_t, Event_t, Example_t, External_t, Feature_t, Figure_t, ForeignData_t, Gap_t, Head_t, Hiddenword_t, Linebreak_t, List_t, Metric_t, Note_t, Paragraph_t, Part_t, PhonContent_t, Quote_t, Reference_t, Relation_t, Sentence_t, String_t, Table_t, TextContent_t, Utterance_t, Whitespace_t, Word_t};
    Event::PROPS.ANNOTATIONTYPE = AnnotationType::EVENT;
    Event::PROPS.LABEL = "Event";
    Event::PROPS.XMLTAG = "event";
    element_props[Event_t] = &Event::PROPS;
//------ Example -------
    Example::PROPS = AbstractStructureElement::PROPS;
    Example::PROPS.ELEMENT_ID = Example_t;
    Example::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, AbstractInlineAnnotation_t, Alternative_t, AlternativeLayers_t, Comment_t, Correction_t, Description_t, External_t, Feature_t, Figure_t, ForeignData_t, Hiddenword_t, Linebreak_t, List_t, Metric_t, Paragraph_t, Part_t, PhonContent_t, Reference_t, Relation_t, Sentence_t, String_t, Table_t, TextContent_t, Utterance_t, Whitespace_t, Word_t};
    Example::PROPS.ANNOTATIONTYPE = AnnotationType::EXAMPLE;
    Example::PROPS.LABEL = "Example";
    Example::PROPS.XMLTAG = "ex";
    element_props[Example_t] = &Example::PROPS;
//------ External -------
    External::PROPS = AbstractHigherOrderAnnotation::PROPS;
    External::PROPS.ELEMENT_ID = External_t;
    External::PROPS.ACCEPTED_DATA += {Comment_t, Description_t};
    External::PROPS.ANNOTATIONTYPE = AnnotationType::EXTERNAL;
    External::PROPS.LABEL = "External";
    External::PROPS.OPTIONAL_ATTRIBS = ID|ANNOTATOR|CONFIDENCE|DATETIME|N|METADATA|BEGINTIME|ENDTIME|TAG;
    External::PROPS.PRINTABLE = true;
    External::PROPS.REQUIRED_ATTRIBS = SRC;
    External::PROPS.SPEAKABLE = true;
    External::PROPS.XMLTAG = "external";
    element_props[External_t] = &External::PROPS;
//------ Feature -------
    Feature::PROPS = AbstractHigherOrderAnnotation::PROPS;
    Feature::PROPS.ELEMENT_ID = Feature_t;
    Feature::PROPS.LABEL = "Feature";
    Feature::PROPS.XMLTAG = "feat";
    element_props[Feature_t] = &Feature::PROPS;
//------ ActorFeature -------
    ActorFeature::PROPS = Feature::PROPS;
    ActorFeature::PROPS.ELEMENT_ID = ActorFeature_t;
    ActorFeature::PROPS.SUBSET = "actor";
    ActorFeature::PROPS.XMLTAG = "actor";
    element_props[ActorFeature_t] = &ActorFeature::PROPS;
//------ BegindatetimeFeature -------
    BegindatetimeFeature::PROPS = Feature::PROPS;
    BegindatetimeFeature::PROPS.ELEMENT_ID = BegindatetimeFeature_t;
    BegindatetimeFeature::PROPS.SUBSET = "begindatetime";
    BegindatetimeFeature::PROPS.XMLTAG = "begindatetime";
    element_props[BegindatetimeFeature_t] = &BegindatetimeFeature::PROPS;
//------ EnddatetimeFeature -------
    EnddatetimeFeature::PROPS = Feature::PROPS;
    EnddatetimeFeature::PROPS.ELEMENT_ID = EnddatetimeFeature_t;
    EnddatetimeFeature::PROPS.SUBSET = "enddatetime";
    EnddatetimeFeature::PROPS.XMLTAG = "enddatetime";
    element_props[EnddatetimeFeature_t] = &EnddatetimeFeature::PROPS;
//------ Figure -------
    Figure::PROPS = AbstractStructureElement::PROPS;
    Figure::PROPS.ELEMENT_ID = Figure_t;
    Figure::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, Alternative_t, AlternativeLayers_t, Caption_t, Comment_t, Correction_t, Description_t, External_t, Feature_t, ForeignData_t, Linebreak_t, Metric_t, Part_t, Relation_t, String_t, TextContent_t};
    Figure::PROPS.ANNOTATIONTYPE = AnnotationType::FIGURE;
    Figure::PROPS.LABEL = "Figure";
    Figure::PROPS.SPEAKABLE = false;
    Figure::PROPS.TEXTDELIMITER = "\n\n";
    Figure::PROPS.XMLTAG = "figure";
    element_props[Figure_t] = &Figure::PROPS;
//------ FontFeature -------
    FontFeature::PROPS = Feature::PROPS;
    FontFeature::PROPS.ELEMENT_ID = FontFeature_t;
    FontFeature::PROPS.SUBSET = "font";
    FontFeature::PROPS.XMLTAG = "font";
    element_props[FontFeature_t] = &FontFeature::PROPS;
//------ ForeignData -------
    ForeignData::PROPS = AbstractHigherOrderAnnotation::PROPS;
    ForeignData::PROPS.ELEMENT_ID = ForeignData_t;
    ForeignData::PROPS.XMLTAG = "foreign-data";
    element_props[ForeignData_t] = &ForeignData::PROPS;
//------ FunctionFeature -------
    FunctionFeature::PROPS = Feature::PROPS;
    FunctionFeature::PROPS.ELEMENT_ID = FunctionFeature_t;
    FunctionFeature::PROPS.SUBSET = "function";
    FunctionFeature::PROPS.XMLTAG = "function";
    element_props[FunctionFeature_t] = &FunctionFeature::PROPS;
//------ Gap -------
    Gap::PROPS = AbstractHigherOrderAnnotation::PROPS;
    Gap::PROPS.ELEMENT_ID = Gap_t;
    Gap::PROPS.ACCEPTED_DATA += {Comment_t, Content_t, Description_t, Feature_t, ForeignData_t, Metric_t, Part_t};
    Gap::PROPS.ANNOTATIONTYPE = AnnotationType::GAP;
    Gap::PROPS.LABEL = "Gap";
    Gap::PROPS.OPTIONAL_ATTRIBS = ID|CLASS|ANNOTATOR|N|DATETIME|SRC|BEGINTIME|ENDTIME|METADATA|TAG;
    Gap::PROPS.XMLTAG = "gap";
    element_props[Gap_t] = &Gap::PROPS;
//------ Head -------
    Head::PROPS = AbstractStructureElement::PROPS;
    Head::PROPS.ELEMENT_ID = Head_t;
    Head::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, AbstractInlineAnnotation_t, Alternative_t, AlternativeLayers_t, Comment_t, Correction_t, Description_t, Event_t, External_t, Feature_t, ForeignData_t, Gap_t, Hiddenword_t, Linebreak_t, Metric_t, Paragraph_t, Part_t, PhonContent_t, Reference_t, Relation_t, Sentence_t, String_t, TextContent_t, Whitespace_t, Word_t};
    Head::PROPS.ANNOTATIONTYPE = AnnotationType::HEAD;
    Head::PROPS.LABEL = "Head";
    Head::PROPS.TEXTDELIMITER = "\n\n";
    Head::PROPS.XMLTAG = "head";
    element_props[Head_t] = &Head::PROPS;
//------ HeadFeature -------
    HeadFeature::PROPS = Feature::PROPS;
    HeadFeature::PROPS.ELEMENT_ID = HeadFeature_t;
    HeadFeature::PROPS.SUBSET = "head";
    HeadFeature::PROPS.XMLTAG = "head";
    element_props[HeadFeature_t] = &HeadFeature::PROPS;
//------ Headspan -------
    Headspan::PROPS = AbstractSpanRole::PROPS;
    Headspan::PROPS.ELEMENT_ID = Headspan_t;
    Headspan::PROPS.LABEL = "Head";
    Headspan::PROPS.OCCURRENCES = 1;
    Headspan::PROPS.XMLTAG = "hd";
    element_props[Headspan_t] = &Headspan::PROPS;
//------ Hiddenword -------
    Hiddenword::PROPS = AbstractStructureElement::PROPS;
    Hiddenword::PROPS.ELEMENT_ID = Hiddenword_t;
    Hiddenword::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, AbstractInlineAnnotation_t, Alternative_t, AlternativeLayers_t, Comment_t, Correction_t, Description_t, External_t, Feature_t, ForeignData_t, Metric_t, Part_t, PhonContent_t, Reference_t, Relation_t, String_t, TextContent_t};
    Hiddenword::PROPS.ANNOTATIONTYPE = AnnotationType::HIDDENTOKEN;
    Hiddenword::PROPS.HIDDEN = true;
    Hiddenword::PROPS.LABEL = "Hidden Word/Token";
    Hiddenword::PROPS.OPTIONAL_ATTRIBS = ID|CLASS|ANNOTATOR|N|CONFIDENCE|DATETIME|SRC|BEGINTIME|ENDTIME|SPEAKER|TEXTCLASS|METADATA|SPACE|TAG;
    Hiddenword::PROPS.TEXTDELIMITER = " ";
    Hiddenword::PROPS.WREFABLE = true;
    Hiddenword::PROPS.XMLTAG = "hiddenw";
    element_props[Hiddenword_t] = &Hiddenword::PROPS;
//------ Hyphbreak -------
    Hyphbreak::PROPS = AbstractTextMarkup::PROPS;
    Hyphbreak::PROPS.ELEMENT_ID = Hyphbreak_t;
    Hyphbreak::PROPS.ANNOTATIONTYPE = AnnotationType::HYPHENATION;
    Hyphbreak::PROPS.IMPLICITSPACE = true;
    Hyphbreak::PROPS.LABEL = "Hyphbreak";
    Hyphbreak::PROPS.TEXTDELIMITER = "";
    Hyphbreak::PROPS.XMLTAG = "t-hbr";
    element_props[Hyphbreak_t] = &Hyphbreak::PROPS;
//------ Label -------
    Label::PROPS = AbstractStructureElement::PROPS;
    Label::PROPS.ELEMENT_ID = Label_t;
    Label::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, AbstractInlineAnnotation_t, Alternative_t, AlternativeLayers_t, Comment_t, Correction_t, Description_t, External_t, Feature_t, ForeignData_t, Hiddenword_t, Linebreak_t, Metric_t, Part_t, PhonContent_t, Reference_t, Relation_t, String_t, TextContent_t, Whitespace_t, Word_t};
    Label::PROPS.LABEL = "Label";
    Label::PROPS.XMLTAG = "label";
    element_props[Label_t] = &Label::PROPS;
//------ LangAnnotation -------
    LangAnnotation::PROPS = AbstractInlineAnnotation::PROPS;
    LangAnnotation::PROPS.ELEMENT_ID = LangAnnotation_t;
    LangAnnotation::PROPS.ANNOTATIONTYPE = AnnotationType::LANG;
    LangAnnotation::PROPS.LABEL = "Language";
    LangAnnotation::PROPS.XMLTAG = "lang";
    element_props[LangAnnotation_t] = &LangAnnotation::PROPS;
//------ LemmaAnnotation -------
    LemmaAnnotation::PROPS = AbstractInlineAnnotation::PROPS;
    LemmaAnnotation::PROPS.ELEMENT_ID = LemmaAnnotation_t;
    LemmaAnnotation::PROPS.ANNOTATIONTYPE = AnnotationType::LEMMA;
    LemmaAnnotation::PROPS.LABEL = "Lemma";
    LemmaAnnotation::PROPS.XMLTAG = "lemma";
    element_props[LemmaAnnotation_t] = &LemmaAnnotation::PROPS;
//------ LevelFeature -------
    LevelFeature::PROPS = Feature::PROPS;
    LevelFeature::PROPS.ELEMENT_ID = LevelFeature_t;
    LevelFeature::PROPS.SUBSET = "level";
    LevelFeature::PROPS.XMLTAG = "level";
    element_props[LevelFeature_t] = &LevelFeature::PROPS;
//------ Linebreak -------
    Linebreak::PROPS = AbstractStructureElement::PROPS;
    Linebreak::PROPS.ELEMENT_ID = Linebreak_t;
    Linebreak::PROPS.ANNOTATIONTYPE = AnnotationType::LINEBREAK;
    Linebreak::PROPS.IMPLICITSPACE = true;
    Linebreak::PROPS.LABEL = "Linebreak";
    Linebreak::PROPS.TEXTDELIMITER = "";
    Linebreak::PROPS.XLINK = true;
    Linebreak::PROPS.XMLTAG = "br";
    element_props[Linebreak_t] = &Linebreak::PROPS;
//------ LinkReference -------
    LinkReference::PROPS.ELEMENT_ID = LinkReference_t;
    LinkReference::PROPS.OPTIONAL_ATTRIBS = IDREF|TAG;
    LinkReference::PROPS.XMLTAG = "xref";
    element_props[LinkReference_t] = &LinkReference::PROPS;
//------ List -------
    List::PROPS = AbstractStructureElement::PROPS;
    List::PROPS.ELEMENT_ID = List_t;
    List::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, AbstractInlineAnnotation_t, Alternative_t, AlternativeLayers_t, Caption_t, Comment_t, Correction_t, Description_t, Event_t, External_t, Feature_t, ForeignData_t, Linebreak_t, ListItem_t, Metric_t, Note_t, Part_t, PhonContent_t, Reference_t, Relation_t, String_t, TextContent_t};
    List::PROPS.ANNOTATIONTYPE = AnnotationType::LIST;
    List::PROPS.LABEL = "List";
    List::PROPS.TEXTDELIMITER = "\n\n";
    List::PROPS.XMLTAG = "list";
    element_props[List_t] = &List::PROPS;
//------ ListItem -------
    ListItem::PROPS = AbstractStructureElement::PROPS;
    ListItem::PROPS.ELEMENT_ID = ListItem_t;
    ListItem::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, AbstractInlineAnnotation_t, Alternative_t, AlternativeLayers_t, Comment_t, Correction_t, Description_t, Event_t, External_t, Feature_t, ForeignData_t, Gap_t, Hiddenword_t, Label_t, Linebreak_t, List_t, Metric_t, Note_t, Paragraph_t, Part_t, PhonContent_t, Quote_t, Reference_t, Relation_t, Sentence_t, String_t, TextContent_t, Whitespace_t, Word_t};
    ListItem::PROPS.LABEL = "List Item";
    ListItem::PROPS.OPTIONAL_ATTRIBS = ID|ANNOTATOR|N|CONFIDENCE|DATETIME|SRC|BEGINTIME|ENDTIME|SPEAKER|METADATA|TAG;
    ListItem::PROPS.TEXTDELIMITER = "\n";
    ListItem::PROPS.XMLTAG = "item";
    element_props[ListItem_t] = &ListItem::PROPS;
//------ Metric -------
    Metric::PROPS = AbstractHigherOrderAnnotation::PROPS;
    Metric::PROPS.ELEMENT_ID = Metric_t;
    Metric::PROPS.ACCEPTED_DATA += {Comment_t, Description_t, Feature_t, ForeignData_t, ValueFeature_t};
    Metric::PROPS.ANNOTATIONTYPE = AnnotationType::METRIC;
    Metric::PROPS.LABEL = "Metric";
    Metric::PROPS.OPTIONAL_ATTRIBS = ID|CLASS|ANNOTATOR|N|CONFIDENCE|DATETIME|SRC|BEGINTIME|ENDTIME|SPEAKER|METADATA|TAG;
    Metric::PROPS.XMLTAG = "metric";
    element_props[Metric_t] = &Metric::PROPS;
//------ ModalitiesLayer -------
    ModalitiesLayer::PROPS = AbstractAnnotationLayer::PROPS;
    ModalitiesLayer::PROPS.ELEMENT_ID = ModalitiesLayer_t;
    ModalitiesLayer::PROPS.ACCEPTED_DATA += {Comment_t, Correction_t, Description_t, ForeignData_t, Modality_t};
    ModalitiesLayer::PROPS.ANNOTATIONTYPE = AnnotationType::MODALITY;
    ModalitiesLayer::PROPS.XMLTAG = "modalities";
    element_props[ModalitiesLayer_t] = &ModalitiesLayer::PROPS;
//------ Modality -------
    Modality::PROPS = AbstractSpanAnnotation::PROPS;
    Modality::PROPS.ELEMENT_ID = Modality_t;
    Modality::PROPS.ACCEPTED_DATA += {AbstractInlineAnnotation_t, Comment_t, Cue_t, Description_t, Feature_t, ForeignData_t, LinkReference_t, Metric_t, PolarityFeature_t, Relation_t, Scope_t, Source_t, StrengthFeature_t, Target_t};
    Modality::PROPS.ANNOTATIONTYPE = AnnotationType::MODALITY;
    Modality::PROPS.LABEL = "Modality";
    Modality::PROPS.XMLTAG = "modality";
    element_props[Modality_t] = &Modality::PROPS;
//------ ModalityFeature -------
    ModalityFeature::PROPS = Feature::PROPS;
    ModalityFeature::PROPS.ELEMENT_ID = ModalityFeature_t;
    ModalityFeature::PROPS.SUBSET = "mod";
    ModalityFeature::PROPS.XMLTAG = "mod";
    element_props[ModalityFeature_t] = &ModalityFeature::PROPS;
//------ Morpheme -------
    Morpheme::PROPS = AbstractSubtokenAnnotation::PROPS;
    Morpheme::PROPS.ELEMENT_ID = Morpheme_t;
    Morpheme::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, AbstractInlineAnnotation_t, Alternative_t, AlternativeLayers_t, Comment_t, Correction_t, Description_t, Feature_t, ForeignData_t, FunctionFeature_t, Metric_t, Morpheme_t, Part_t, PhonContent_t, Relation_t, String_t, TextContent_t};
    Morpheme::PROPS.ANNOTATIONTYPE = AnnotationType::MORPHOLOGICAL;
    Morpheme::PROPS.LABEL = "Morpheme";
    Morpheme::PROPS.TEXTDELIMITER = "";
    Morpheme::PROPS.WREFABLE = true;
    Morpheme::PROPS.XMLTAG = "morpheme";
    element_props[Morpheme_t] = &Morpheme::PROPS;
//------ MorphologyLayer -------
    MorphologyLayer::PROPS = AbstractAnnotationLayer::PROPS;
    MorphologyLayer::PROPS.ELEMENT_ID = MorphologyLayer_t;
    MorphologyLayer::PROPS.ACCEPTED_DATA += {Comment_t, Correction_t, Description_t, ForeignData_t, Morpheme_t};
    MorphologyLayer::PROPS.ANNOTATIONTYPE = AnnotationType::MORPHOLOGICAL;
    MorphologyLayer::PROPS.XMLTAG = "morphology";
    element_props[MorphologyLayer_t] = &MorphologyLayer::PROPS;
//------ New -------
    New::PROPS = AbstractCorrectionChild::PROPS;
    New::PROPS.ELEMENT_ID = New_t;
    New::PROPS.ANNOTATIONTYPE = AnnotationType::CORRECTION;
    New::PROPS.OCCURRENCES = 1;
    New::PROPS.OPTIONAL_ATTRIBS = TAG;
    New::PROPS.XMLTAG = "new";
    element_props[New_t] = &New::PROPS;
//------ Note -------
    Note::PROPS = AbstractStructureElement::PROPS;
    Note::PROPS.ELEMENT_ID = Note_t;
    Note::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, AbstractInlineAnnotation_t, Alternative_t, AlternativeLayers_t, Comment_t, Correction_t, Description_t, Example_t, External_t, Feature_t, Figure_t, ForeignData_t, Head_t, Hiddenword_t, Linebreak_t, List_t, Metric_t, Paragraph_t, Part_t, PhonContent_t, Reference_t, Relation_t, Sentence_t, String_t, Table_t, TextContent_t, Utterance_t, Whitespace_t, Word_t};
    Note::PROPS.ANNOTATIONTYPE = AnnotationType::NOTE;
    Note::PROPS.LABEL = "Note";
    Note::PROPS.XMLTAG = "note";
    element_props[Note_t] = &Note::PROPS;
//------ Observation -------
    Observation::PROPS = AbstractSpanAnnotation::PROPS;
    Observation::PROPS.ELEMENT_ID = Observation_t;
    Observation::PROPS.ACCEPTED_DATA += {AbstractInlineAnnotation_t, Comment_t, Description_t, Feature_t, ForeignData_t, LinkReference_t, Metric_t, Relation_t, WordReference_t, Hiddenword_t, Morpheme_t, Phoneme_t, Word_t};
    Observation::PROPS.ANNOTATIONTYPE = AnnotationType::OBSERVATION;
    Observation::PROPS.LABEL = "Observation";
    Observation::PROPS.XMLTAG = "observation";
    element_props[Observation_t] = &Observation::PROPS;
//------ ObservationLayer -------
    ObservationLayer::PROPS = AbstractAnnotationLayer::PROPS;
    ObservationLayer::PROPS.ELEMENT_ID = ObservationLayer_t;
    ObservationLayer::PROPS.ACCEPTED_DATA += {Comment_t, Correction_t, Description_t, ForeignData_t, Observation_t};
    ObservationLayer::PROPS.ANNOTATIONTYPE = AnnotationType::OBSERVATION;
    ObservationLayer::PROPS.XMLTAG = "observations";
    element_props[ObservationLayer_t] = &ObservationLayer::PROPS;
//------ Original -------
    Original::PROPS = AbstractCorrectionChild::PROPS;
    Original::PROPS.ELEMENT_ID = Original_t;
    Original::PROPS.ANNOTATIONTYPE = AnnotationType::CORRECTION;
    Original::PROPS.AUTH = false;
    Original::PROPS.OCCURRENCES = 1;
    Original::PROPS.OPTIONAL_ATTRIBS = TAG;
    Original::PROPS.XMLTAG = "original";
    element_props[Original_t] = &Original::PROPS;
//------ Paragraph -------
    Paragraph::PROPS = AbstractStructureElement::PROPS;
    Paragraph::PROPS.ELEMENT_ID = Paragraph_t;
    Paragraph::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, AbstractInlineAnnotation_t, Alternative_t, AlternativeLayers_t, Comment_t, Correction_t, Description_t, Entry_t, Event_t, Example_t, External_t, Feature_t, Figure_t, ForeignData_t, Gap_t, Head_t, Hiddenword_t, Linebreak_t, List_t, Metric_t, Note_t, Part_t, PhonContent_t, Quote_t, Reference_t, Relation_t, Sentence_t, String_t, TextContent_t, Whitespace_t, Word_t};
    Paragraph::PROPS.ANNOTATIONTYPE = AnnotationType::PARAGRAPH;
    Paragraph::PROPS.LABEL = "Paragraph";
    Paragraph::PROPS.TEXTDELIMITER = "\n\n";
    Paragraph::PROPS.XMLTAG = "p";
    element_props[Paragraph_t] = &Paragraph::PROPS;
//------ Part -------
    Part::PROPS = AbstractStructureElement::PROPS;
    Part::PROPS.ELEMENT_ID = Part_t;
    Part::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, AbstractInlineAnnotation_t, AbstractStructureElement_t, Alternative_t, AlternativeLayers_t, Comment_t, Correction_t, Description_t, External_t, Feature_t, ForeignData_t, Metric_t, Part_t, PhonContent_t, Relation_t, TextContent_t};
    Part::PROPS.ANNOTATIONTYPE = AnnotationType::PART;
    Part::PROPS.LABEL = "Part";
    Part::PROPS.TEXTDELIMITER = " ";
    Part::PROPS.XMLTAG = "part";
    element_props[Part_t] = &Part::PROPS;
//------ PhonContent -------
    PhonContent::PROPS = AbstractContentAnnotation::PROPS;
    PhonContent::PROPS.ELEMENT_ID = PhonContent_t;
    PhonContent::PROPS.ACCEPTED_DATA += {Comment_t, Description_t, Feature_t, XmlText_t};
    PhonContent::PROPS.ANNOTATIONTYPE = AnnotationType::PHON;
    PhonContent::PROPS.LABEL = "Phonetic Content";
    PhonContent::PROPS.PHONCONTAINER = true;
    PhonContent::PROPS.PRINTABLE = false;
    PhonContent::PROPS.SPEAKABLE = true;
    PhonContent::PROPS.XMLTAG = "ph";
    element_props[PhonContent_t] = &PhonContent::PROPS;
//------ Phoneme -------
    Phoneme::PROPS = AbstractSubtokenAnnotation::PROPS;
    Phoneme::PROPS.ELEMENT_ID = Phoneme_t;
    Phoneme::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, AbstractInlineAnnotation_t, Alternative_t, AlternativeLayers_t, Comment_t, Correction_t, Description_t, Feature_t, ForeignData_t, FunctionFeature_t, Metric_t, Part_t, PhonContent_t, Phoneme_t, Relation_t, String_t, TextContent_t};
    Phoneme::PROPS.ANNOTATIONTYPE = AnnotationType::PHONOLOGICAL;
    Phoneme::PROPS.LABEL = "Phoneme";
    Phoneme::PROPS.TEXTDELIMITER = "";
    Phoneme::PROPS.WREFABLE = true;
    Phoneme::PROPS.XMLTAG = "phoneme";
    element_props[Phoneme_t] = &Phoneme::PROPS;
//------ PhonologyLayer -------
    PhonologyLayer::PROPS = AbstractAnnotationLayer::PROPS;
    PhonologyLayer::PROPS.ELEMENT_ID = PhonologyLayer_t;
    PhonologyLayer::PROPS.ACCEPTED_DATA += {Comment_t, Correction_t, Description_t, ForeignData_t, Phoneme_t};
    PhonologyLayer::PROPS.ANNOTATIONTYPE = AnnotationType::PHONOLOGICAL;
    PhonologyLayer::PROPS.XMLTAG = "phonology";
    element_props[PhonologyLayer_t] = &PhonologyLayer::PROPS;
//------ PolarityFeature -------
    PolarityFeature::PROPS = Feature::PROPS;
    PolarityFeature::PROPS.ELEMENT_ID = PolarityFeature_t;
    PolarityFeature::PROPS.SUBSET = "polarity";
    PolarityFeature::PROPS.XMLTAG = "polarity";
    element_props[PolarityFeature_t] = &PolarityFeature::PROPS;
//------ PosAnnotation -------
    PosAnnotation::PROPS = AbstractInlineAnnotation::PROPS;
    PosAnnotation::PROPS.ELEMENT_ID = PosAnnotation_t;
    PosAnnotation::PROPS.ACCEPTED_DATA += {Comment_t, Description_t, Feature_t, ForeignData_t, HeadFeature_t, Metric_t};
    PosAnnotation::PROPS.ANNOTATIONTYPE = AnnotationType::POS;
    PosAnnotation::PROPS.LABEL = "Part-of-Speech";
    PosAnnotation::PROPS.XMLTAG = "pos";
    element_props[PosAnnotation_t] = &PosAnnotation::PROPS;
//------ Predicate -------
    Predicate::PROPS = AbstractSpanAnnotation::PROPS;
    Predicate::PROPS.ELEMENT_ID = Predicate_t;
    Predicate::PROPS.ACCEPTED_DATA += {AbstractInlineAnnotation_t, Comment_t, Description_t, Feature_t, ForeignData_t, LinkReference_t, Metric_t, Relation_t, SemanticRole_t, WordReference_t, Hiddenword_t, Morpheme_t, Phoneme_t, Word_t};
    Predicate::PROPS.ANNOTATIONTYPE = AnnotationType::PREDICATE;
    Predicate::PROPS.LABEL = "Predicate";
    Predicate::PROPS.XMLTAG = "predicate";
    element_props[Predicate_t] = &Predicate::PROPS;
//------ Quote -------
    Quote::PROPS = AbstractStructureElement::PROPS;
    Quote::PROPS.ELEMENT_ID = Quote_t;
    Quote::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, AbstractInlineAnnotation_t, Alternative_t, AlternativeLayers_t, Comment_t, Correction_t, Description_t, Division_t, External_t, Feature_t, ForeignData_t, Gap_t, Hiddenword_t, Linebreak_t, Metric_t, Paragraph_t, Part_t, Quote_t, Reference_t, Relation_t, Sentence_t, String_t, TextContent_t, Utterance_t, Whitespace_t, Word_t};
    Quote::PROPS.ANNOTATIONTYPE = AnnotationType::QUOTE;
    Quote::PROPS.LABEL = "Quote";
    Quote::PROPS.XMLTAG = "quote";
    element_props[Quote_t] = &Quote::PROPS;
//------ Reference -------
    Reference::PROPS = AbstractStructureElement::PROPS;
    Reference::PROPS.ELEMENT_ID = Reference_t;
    Reference::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, Alternative_t, AlternativeLayers_t, Comment_t, Correction_t, Description_t, External_t, Feature_t, ForeignData_t, Hiddenword_t, Linebreak_t, Metric_t, Paragraph_t, Part_t, PhonContent_t, Quote_t, Relation_t, Sentence_t, String_t, TextContent_t, Utterance_t, Whitespace_t, Word_t};
    Reference::PROPS.ANNOTATIONTYPE = AnnotationType::REFERENCE;
    Reference::PROPS.LABEL = "Reference";
    Reference::PROPS.TEXTDELIMITER = " ";
    Reference::PROPS.XLINK = true;
    Reference::PROPS.XMLTAG = "ref";
    element_props[Reference_t] = &Reference::PROPS;
//------ Relation -------
    Relation::PROPS = AbstractHigherOrderAnnotation::PROPS;
    Relation::PROPS.ELEMENT_ID = Relation_t;
    Relation::PROPS.ACCEPTED_DATA += {Comment_t, Description_t, Feature_t, ForeignData_t, LinkReference_t, Metric_t};
    Relation::PROPS.ANNOTATIONTYPE = AnnotationType::RELATION;
    Relation::PROPS.LABEL = "Relation";
    Relation::PROPS.OPTIONAL_ATTRIBS = ID|CLASS|ANNOTATOR|N|CONFIDENCE|DATETIME|SRC|BEGINTIME|ENDTIME|SPEAKER|METADATA|TAG;
    Relation::PROPS.PRINTABLE = false;
    Relation::PROPS.REQUIRED_ATTRIBS = NO_ATT;
    Relation::PROPS.SPEAKABLE = false;
    Relation::PROPS.XLINK = true;
    Relation::PROPS.XMLTAG = "relation";
    element_props[Relation_t] = &Relation::PROPS;
//------ Row -------
    Row::PROPS = AbstractStructureElement::PROPS;
    Row::PROPS.ELEMENT_ID = Row_t;
    Row::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, AbstractInlineAnnotation_t, Alternative_t, AlternativeLayers_t, Cell_t, Comment_t, Correction_t, Description_t, External_t, Feature_t, ForeignData_t, Metric_t, Part_t, Relation_t};
    Row::PROPS.LABEL = "Table Row";
    Row::PROPS.TEXTDELIMITER = "\n";
    Row::PROPS.XMLTAG = "row";
    element_props[Row_t] = &Row::PROPS;
//------ Scope -------
    Scope::PROPS = AbstractSpanRole::PROPS;
    Scope::PROPS.ELEMENT_ID = Scope_t;
    Scope::PROPS.ACCEPTED_DATA += {AbstractInlineAnnotation_t, Comment_t, Cue_t, Description_t, Feature_t, ForeignData_t, LinkReference_t, Metric_t, Relation_t, Source_t, Target_t, WordReference_t, Hiddenword_t, Morpheme_t, Phoneme_t, Word_t};
    Scope::PROPS.LABEL = "Scope";
    Scope::PROPS.OCCURRENCES = 1;
    Scope::PROPS.XMLTAG = "scope";
    element_props[Scope_t] = &Scope::PROPS;
//------ SemanticRole -------
    SemanticRole::PROPS = AbstractSpanAnnotation::PROPS;
    SemanticRole::PROPS.ELEMENT_ID = SemanticRole_t;
    SemanticRole::PROPS.ACCEPTED_DATA += {AbstractInlineAnnotation_t, Comment_t, Description_t, Feature_t, ForeignData_t, Headspan_t, LinkReference_t, Metric_t, Relation_t, WordReference_t, Hiddenword_t, Morpheme_t, Phoneme_t, Word_t};
    SemanticRole::PROPS.ANNOTATIONTYPE = AnnotationType::SEMROLE;
    SemanticRole::PROPS.LABEL = "Semantic Role";
    SemanticRole::PROPS.REQUIRED_ATTRIBS = CLASS;
    SemanticRole::PROPS.XMLTAG = "semrole";
    element_props[SemanticRole_t] = &SemanticRole::PROPS;
//------ SemanticRolesLayer -------
    SemanticRolesLayer::PROPS = AbstractAnnotationLayer::PROPS;
    SemanticRolesLayer::PROPS.ELEMENT_ID = SemanticRolesLayer_t;
    SemanticRolesLayer::PROPS.ACCEPTED_DATA += {Comment_t, Correction_t, Description_t, ForeignData_t, Predicate_t, SemanticRole_t};
    SemanticRolesLayer::PROPS.ANNOTATIONTYPE = AnnotationType::SEMROLE;
    SemanticRolesLayer::PROPS.XMLTAG = "semroles";
    element_props[SemanticRolesLayer_t] = &SemanticRolesLayer::PROPS;
//------ SenseAnnotation -------
    SenseAnnotation::PROPS = AbstractInlineAnnotation::PROPS;
    SenseAnnotation::PROPS.ELEMENT_ID = SenseAnnotation_t;
    SenseAnnotation::PROPS.ACCEPTED_DATA += {Comment_t, Description_t, Feature_t, ForeignData_t, Metric_t, SynsetFeature_t};
    SenseAnnotation::PROPS.ANNOTATIONTYPE = AnnotationType::SENSE;
    SenseAnnotation::PROPS.LABEL = "Semantic Sense";
    SenseAnnotation::PROPS.OCCURRENCES_PER_SET = 0;
    SenseAnnotation::PROPS.XMLTAG = "sense";
    element_props[SenseAnnotation_t] = &SenseAnnotation::PROPS;
//------ Sentence -------
    Sentence::PROPS = AbstractStructureElement::PROPS;
    Sentence::PROPS.ELEMENT_ID = Sentence_t;
    Sentence::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, AbstractInlineAnnotation_t, Alternative_t, AlternativeLayers_t, Comment_t, Correction_t, Description_t, Entry_t, Event_t, Example_t, External_t, Feature_t, ForeignData_t, Gap_t, Hiddenword_t, Linebreak_t, Metric_t, Note_t, Part_t, PhonContent_t, Quote_t, Reference_t, Relation_t, String_t, TextContent_t, Whitespace_t, Word_t};
    Sentence::PROPS.ANNOTATIONTYPE = AnnotationType::SENTENCE;
    Sentence::PROPS.LABEL = "Sentence";
    Sentence::PROPS.TEXTDELIMITER = " ";
    Sentence::PROPS.XMLTAG = "s";
    element_props[Sentence_t] = &Sentence::PROPS;
//------ Sentiment -------
    Sentiment::PROPS = AbstractSpanAnnotation::PROPS;
    Sentiment::PROPS.ELEMENT_ID = Sentiment_t;
    Sentiment::PROPS.ACCEPTED_DATA += {AbstractInlineAnnotation_t, Comment_t, Description_t, Feature_t, ForeignData_t, Headspan_t, LinkReference_t, Metric_t, PolarityFeature_t, Relation_t, Source_t, StrengthFeature_t, Target_t, WordReference_t, Hiddenword_t, Morpheme_t, Phoneme_t, Word_t};
    Sentiment::PROPS.ANNOTATIONTYPE = AnnotationType::SENTIMENT;
    Sentiment::PROPS.LABEL = "Sentiment";
    Sentiment::PROPS.XMLTAG = "sentiment";
    element_props[Sentiment_t] = &Sentiment::PROPS;
//------ SentimentLayer -------
    SentimentLayer::PROPS = AbstractAnnotationLayer::PROPS;
    SentimentLayer::PROPS.ELEMENT_ID = SentimentLayer_t;
    SentimentLayer::PROPS.ACCEPTED_DATA += {Comment_t, Correction_t, Description_t, ForeignData_t, Sentiment_t};
    SentimentLayer::PROPS.ANNOTATIONTYPE = AnnotationType::SENTIMENT;
    SentimentLayer::PROPS.XMLTAG = "sentiments";
    element_props[SentimentLayer_t] = &SentimentLayer::PROPS;
//------ SizeFeature -------
    SizeFeature::PROPS = Feature::PROPS;
    SizeFeature::PROPS.ELEMENT_ID = SizeFeature_t;
    SizeFeature::PROPS.SUBSET = "size";
    SizeFeature::PROPS.XMLTAG = "size";
    element_props[SizeFeature_t] = &SizeFeature::PROPS;
//------ Source -------
    Source::PROPS = AbstractSpanRole::PROPS;
    Source::PROPS.ELEMENT_ID = Source_t;
    Source::PROPS.LABEL = "Source";
    Source::PROPS.OCCURRENCES = 1;
    Source::PROPS.XMLTAG = "source";
    element_props[Source_t] = &Source::PROPS;
//------ SpanRelation -------
    SpanRelation::PROPS = AbstractHigherOrderAnnotation::PROPS;
    SpanRelation::PROPS.ELEMENT_ID = SpanRelation_t;
    SpanRelation::PROPS.ACCEPTED_DATA += {Comment_t, Description_t, Feature_t, ForeignData_t, Metric_t, Relation_t};
    SpanRelation::PROPS.ANNOTATIONTYPE = AnnotationType::SPANRELATION;
    SpanRelation::PROPS.LABEL = "Span Relation";
    SpanRelation::PROPS.OPTIONAL_ATTRIBS = ID|CLASS|ANNOTATOR|N|CONFIDENCE|DATETIME|SRC|BEGINTIME|ENDTIME|SPEAKER|METADATA|TAG;
    SpanRelation::PROPS.PRINTABLE = false;
    SpanRelation::PROPS.REQUIRED_ATTRIBS = NO_ATT;
    SpanRelation::PROPS.SPEAKABLE = false;
    SpanRelation::PROPS.XMLTAG = "spanrelation";
    element_props[SpanRelation_t] = &SpanRelation::PROPS;
//------ SpanRelationLayer -------
    SpanRelationLayer::PROPS = AbstractAnnotationLayer::PROPS;
    SpanRelationLayer::PROPS.ELEMENT_ID = SpanRelationLayer_t;
    SpanRelationLayer::PROPS.ACCEPTED_DATA += {Comment_t, Correction_t, Description_t, ForeignData_t, SpanRelation_t};
    SpanRelationLayer::PROPS.ANNOTATIONTYPE = AnnotationType::SPANRELATION;
    SpanRelationLayer::PROPS.XMLTAG = "spanrelations";
    element_props[SpanRelationLayer_t] = &SpanRelationLayer::PROPS;
//------ Speech -------
    Speech::PROPS = AbstractStructureElement::PROPS;
    Speech::PROPS.ELEMENT_ID = Speech_t;
    Speech::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, AbstractInlineAnnotation_t, Alternative_t, AlternativeLayers_t, Comment_t, Correction_t, Description_t, Division_t, Entry_t, Event_t, Example_t, External_t, Feature_t, ForeignData_t, Gap_t, Hiddenword_t, List_t, Metric_t, Note_t, Paragraph_t, Part_t, PhonContent_t, Quote_t, Reference_t, Relation_t, Sentence_t, String_t, TextContent_t, Utterance_t, Word_t};
    Speech::PROPS.LABEL = "Speech Body";
    Speech::PROPS.OPTIONAL_ATTRIBS = ID|ANNOTATOR|DATETIME|SRC|BEGINTIME|ENDTIME|SPEAKER|METADATA|SPACE|TAG;
    Speech::PROPS.TEXTDELIMITER = "\n\n\n";
    Speech::PROPS.XMLTAG = "speech";
    element_props[Speech_t] = &Speech::PROPS;
//------ Statement -------
    Statement::PROPS = AbstractSpanAnnotation::PROPS;
    Statement::PROPS.ELEMENT_ID = Statement_t;
    Statement::PROPS.ACCEPTED_DATA += {AbstractInlineAnnotation_t, Comment_t, Description_t, Feature_t, ForeignData_t, Headspan_t, LinkReference_t, Metric_t, Relation_t, Source_t, StatementRelation_t, WordReference_t, Hiddenword_t, Morpheme_t, Phoneme_t, Word_t};
    Statement::PROPS.ANNOTATIONTYPE = AnnotationType::STATEMENT;
    Statement::PROPS.LABEL = "Statement";
    Statement::PROPS.XMLTAG = "statement";
    element_props[Statement_t] = &Statement::PROPS;
//------ StatementLayer -------
    StatementLayer::PROPS = AbstractAnnotationLayer::PROPS;
    StatementLayer::PROPS.ELEMENT_ID = StatementLayer_t;
    StatementLayer::PROPS.ACCEPTED_DATA += {Comment_t, Correction_t, Description_t, ForeignData_t, Statement_t};
    StatementLayer::PROPS.ANNOTATIONTYPE = AnnotationType::STATEMENT;
    StatementLayer::PROPS.XMLTAG = "statements";
    element_props[StatementLayer_t] = &StatementLayer::PROPS;
//------ StatementRelation -------
    StatementRelation::PROPS = AbstractSpanRole::PROPS;
    StatementRelation::PROPS.ELEMENT_ID = StatementRelation_t;
    StatementRelation::PROPS.LABEL = "Relation";
    StatementRelation::PROPS.OCCURRENCES = 1;
    StatementRelation::PROPS.XMLTAG = "rel";
    element_props[StatementRelation_t] = &StatementRelation::PROPS;
//------ StrengthFeature -------
    StrengthFeature::PROPS = Feature::PROPS;
    StrengthFeature::PROPS.ELEMENT_ID = StrengthFeature_t;
    StrengthFeature::PROPS.SUBSET = "strength";
    StrengthFeature::PROPS.XMLTAG = "strength";
    element_props[StrengthFeature_t] = &StrengthFeature::PROPS;
//------ String -------
    String::PROPS = AbstractHigherOrderAnnotation::PROPS;
    String::PROPS.ELEMENT_ID = String_t;
    String::PROPS.ACCEPTED_DATA += {AbstractInlineAnnotation_t, Comment_t, Correction_t, Description_t, Feature_t, ForeignData_t, Metric_t, PhonContent_t, Relation_t, TextContent_t};
    String::PROPS.ANNOTATIONTYPE = AnnotationType::STRING;
    String::PROPS.LABEL = "String";
    String::PROPS.OCCURRENCES = 0;
    String::PROPS.OPTIONAL_ATTRIBS = ID|CLASS|ANNOTATOR|CONFIDENCE|DATETIME|N|SRC|BEGINTIME|ENDTIME|METADATA|TAG;
    String::PROPS.PRINTABLE = true;
    String::PROPS.XMLTAG = "str";
    element_props[String_t] = &String::PROPS;
//------ StyleFeature -------
    StyleFeature::PROPS = Feature::PROPS;
    StyleFeature::PROPS.ELEMENT_ID = StyleFeature_t;
    StyleFeature::PROPS.SUBSET = "style";
    StyleFeature::PROPS.XMLTAG = "style";
    element_props[StyleFeature_t] = &StyleFeature::PROPS;
//------ SubjectivityAnnotation -------
    SubjectivityAnnotation::PROPS = AbstractInlineAnnotation::PROPS;
    SubjectivityAnnotation::PROPS.ELEMENT_ID = SubjectivityAnnotation_t;
    SubjectivityAnnotation::PROPS.ANNOTATIONTYPE = AnnotationType::SUBJECTIVITY;
    SubjectivityAnnotation::PROPS.LABEL = "Subjectivity/Sentiment";
    SubjectivityAnnotation::PROPS.XMLTAG = "subjectivity";
    element_props[SubjectivityAnnotation_t] = &SubjectivityAnnotation::PROPS;
//------ Suggestion -------
    Suggestion::PROPS = AbstractCorrectionChild::PROPS;
    Suggestion::PROPS.ELEMENT_ID = Suggestion_t;
    Suggestion::PROPS.ANNOTATIONTYPE = AnnotationType::CORRECTION;
    Suggestion::PROPS.AUTH = false;
    Suggestion::PROPS.OCCURRENCES = 0;
    Suggestion::PROPS.OPTIONAL_ATTRIBS = CONFIDENCE|N|TAG;
    Suggestion::PROPS.XMLTAG = "suggestion";
    element_props[Suggestion_t] = &Suggestion::PROPS;
//------ SynsetFeature -------
    SynsetFeature::PROPS = Feature::PROPS;
    SynsetFeature::PROPS.ELEMENT_ID = SynsetFeature_t;
    SynsetFeature::PROPS.SUBSET = "synset";
    SynsetFeature::PROPS.XMLTAG = "synset";
    element_props[SynsetFeature_t] = &SynsetFeature::PROPS;
//------ SyntacticUnit -------
    SyntacticUnit::PROPS = AbstractSpanAnnotation::PROPS;
    SyntacticUnit::PROPS.ELEMENT_ID = SyntacticUnit_t;
    SyntacticUnit::PROPS.ACCEPTED_DATA += {AbstractInlineAnnotation_t, Comment_t, Description_t, Feature_t, ForeignData_t, LinkReference_t, Metric_t, Relation_t, SyntacticUnit_t, WordReference_t, Hiddenword_t, Morpheme_t, Phoneme_t, Word_t};
    SyntacticUnit::PROPS.ANNOTATIONTYPE = AnnotationType::SYNTAX;
    SyntacticUnit::PROPS.LABEL = "Syntactic Unit";
    SyntacticUnit::PROPS.XMLTAG = "su";
    element_props[SyntacticUnit_t] = &SyntacticUnit::PROPS;
//------ SyntaxLayer -------
    SyntaxLayer::PROPS = AbstractAnnotationLayer::PROPS;
    SyntaxLayer::PROPS.ELEMENT_ID = SyntaxLayer_t;
    SyntaxLayer::PROPS.ACCEPTED_DATA += {Comment_t, Correction_t, Description_t, ForeignData_t, SyntacticUnit_t};
    SyntaxLayer::PROPS.ANNOTATIONTYPE = AnnotationType::SYNTAX;
    SyntaxLayer::PROPS.XMLTAG = "syntax";
    element_props[SyntaxLayer_t] = &SyntaxLayer::PROPS;
//------ Table -------
    Table::PROPS = AbstractStructureElement::PROPS;
    Table::PROPS.ELEMENT_ID = Table_t;
    Table::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, AbstractInlineAnnotation_t, Alternative_t, AlternativeLayers_t, Comment_t, Correction_t, Description_t, External_t, Feature_t, ForeignData_t, Linebreak_t, Metric_t, Part_t, Relation_t, Row_t, TableHead_t};
    Table::PROPS.ANNOTATIONTYPE = AnnotationType::TABLE;
    Table::PROPS.LABEL = "Table";
    Table::PROPS.XMLTAG = "table";
    element_props[Table_t] = &Table::PROPS;
//------ TableHead -------
    TableHead::PROPS = AbstractStructureElement::PROPS;
    TableHead::PROPS.ELEMENT_ID = TableHead_t;
    TableHead::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, AbstractInlineAnnotation_t, Alternative_t, AlternativeLayers_t, Comment_t, Correction_t, Description_t, External_t, Feature_t, ForeignData_t, Metric_t, Part_t, Relation_t, Row_t};
    TableHead::PROPS.LABEL = "Table Header";
    TableHead::PROPS.OPTIONAL_ATTRIBS = ID|ANNOTATOR|N|CONFIDENCE|DATETIME|SRC|BEGINTIME|ENDTIME|SPEAKER|METADATA|TAG;
    TableHead::PROPS.XMLTAG = "tablehead";
    element_props[TableHead_t] = &TableHead::PROPS;
//------ Target -------
    Target::PROPS = AbstractSpanRole::PROPS;
    Target::PROPS.ELEMENT_ID = Target_t;
    Target::PROPS.LABEL = "Target";
    Target::PROPS.OCCURRENCES = 1;
    Target::PROPS.XMLTAG = "target";
    element_props[Target_t] = &Target::PROPS;
//------ Term -------
    Term::PROPS = AbstractStructureElement::PROPS;
    Term::PROPS.ELEMENT_ID = Term_t;
    Term::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, AbstractInlineAnnotation_t, Alternative_t, AlternativeLayers_t, Comment_t, Correction_t, Description_t, Event_t, External_t, Feature_t, Figure_t, ForeignData_t, Gap_t, Hiddenword_t, Linebreak_t, List_t, Metric_t, Paragraph_t, Part_t, PhonContent_t, Reference_t, Relation_t, Sentence_t, String_t, Table_t, TextContent_t, Utterance_t, Whitespace_t, Word_t};
    Term::PROPS.ANNOTATIONTYPE = AnnotationType::TERM;
    Term::PROPS.LABEL = "Term";
    Term::PROPS.XMLTAG = "term";
    element_props[Term_t] = &Term::PROPS;
//------ Text -------
    Text::PROPS = AbstractStructureElement::PROPS;
    Text::PROPS.ELEMENT_ID = Text_t;
    Text::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, AbstractInlineAnnotation_t, Alternative_t, AlternativeLayers_t, Comment_t, Correction_t, Description_t, Division_t, Entry_t, Event_t, Example_t, External_t, Feature_t, Figure_t, ForeignData_t, Gap_t, Hiddenword_t, Linebreak_t, List_t, Metric_t, Note_t, Paragraph_t, Part_t, PhonContent_t, Quote_t, Reference_t, Relation_t, Sentence_t, String_t, Table_t, TextContent_t, Whitespace_t, Word_t};
    Text::PROPS.LABEL = "Text Body";
    Text::PROPS.OPTIONAL_ATTRIBS = ID|ANNOTATOR|DATETIME|SRC|BEGINTIME|ENDTIME|SPEAKER|METADATA|SPACE|TAG;
    Text::PROPS.TEXTDELIMITER = "\n\n\n";
    Text::PROPS.XMLTAG = "text";
    element_props[Text_t] = &Text::PROPS;
//------ TextContent -------
    TextContent::PROPS = AbstractContentAnnotation::PROPS;
    TextContent::PROPS.ELEMENT_ID = TextContent_t;
    TextContent::PROPS.ACCEPTED_DATA += {AbstractTextMarkup_t, Comment_t, Description_t, Feature_t, Linebreak_t, XmlText_t};
    TextContent::PROPS.ANNOTATIONTYPE = AnnotationType::TEXT;
    TextContent::PROPS.LABEL = "Text";
    TextContent::PROPS.PRINTABLE = true;
    TextContent::PROPS.SPEAKABLE = false;
    TextContent::PROPS.TEXTCONTAINER = true;
    TextContent::PROPS.XLINK = true;
    TextContent::PROPS.XMLTAG = "t";
    element_props[TextContent_t] = &TextContent::PROPS;
//------ TextMarkupCorrection -------
    TextMarkupCorrection::PROPS = AbstractTextMarkup::PROPS;
    TextMarkupCorrection::PROPS.ELEMENT_ID = TextMarkupCorrection_t;
    TextMarkupCorrection::PROPS.ANNOTATIONTYPE = AnnotationType::CORRECTION;
    TextMarkupCorrection::PROPS.XMLTAG = "t-correction";
    element_props[TextMarkupCorrection_t] = &TextMarkupCorrection::PROPS;
//------ TextMarkupError -------
    TextMarkupError::PROPS = AbstractTextMarkup::PROPS;
    TextMarkupError::PROPS.ELEMENT_ID = TextMarkupError_t;
    TextMarkupError::PROPS.ANNOTATIONTYPE = AnnotationType::ERRORDETECTION;
    TextMarkupError::PROPS.XMLTAG = "t-error";
    element_props[TextMarkupError_t] = &TextMarkupError::PROPS;
//------ TextMarkupGap -------
    TextMarkupGap::PROPS = AbstractTextMarkup::PROPS;
    TextMarkupGap::PROPS.ELEMENT_ID = TextMarkupGap_t;
    TextMarkupGap::PROPS.ANNOTATIONTYPE = AnnotationType::GAP;
    TextMarkupGap::PROPS.XMLTAG = "t-gap";
    element_props[TextMarkupGap_t] = &TextMarkupGap::PROPS;
//------ TextMarkupHSpace -------
    TextMarkupHSpace::PROPS = AbstractTextMarkup::PROPS;
    TextMarkupHSpace::PROPS.ELEMENT_ID = TextMarkupHSpace_t;
    TextMarkupHSpace::PROPS.ANNOTATIONTYPE = AnnotationType::HSPACE;
    TextMarkupHSpace::PROPS.IMPLICITSPACE = true;
    TextMarkupHSpace::PROPS.TEXTDELIMITER = "";
    TextMarkupHSpace::PROPS.XMLTAG = "t-hspace";
    element_props[TextMarkupHSpace_t] = &TextMarkupHSpace::PROPS;
//------ TextMarkupLanguage -------
    TextMarkupLanguage::PROPS = AbstractTextMarkup::PROPS;
    TextMarkupLanguage::PROPS.ELEMENT_ID = TextMarkupLanguage_t;
    TextMarkupLanguage::PROPS.ANNOTATIONTYPE = AnnotationType::LANG;
    TextMarkupLanguage::PROPS.XMLTAG = "t-lang";
    element_props[TextMarkupLanguage_t] = &TextMarkupLanguage::PROPS;
//------ TextMarkupReference -------
    TextMarkupReference::PROPS = AbstractTextMarkup::PROPS;
    TextMarkupReference::PROPS.ELEMENT_ID = TextMarkupReference_t;
    TextMarkupReference::PROPS.ANNOTATIONTYPE = AnnotationType::REFERENCE;
    TextMarkupReference::PROPS.XMLTAG = "t-ref";
    element_props[TextMarkupReference_t] = &TextMarkupReference::PROPS;
//------ TextMarkupString -------
    TextMarkupString::PROPS = AbstractTextMarkup::PROPS;
    TextMarkupString::PROPS.ELEMENT_ID = TextMarkupString_t;
    TextMarkupString::PROPS.ANNOTATIONTYPE = AnnotationType::STRING;
    TextMarkupString::PROPS.XMLTAG = "t-str";
    element_props[TextMarkupString_t] = &TextMarkupString::PROPS;
//------ TextMarkupStyle -------
    TextMarkupStyle::PROPS = AbstractTextMarkup::PROPS;
    TextMarkupStyle::PROPS.ELEMENT_ID = TextMarkupStyle_t;
    TextMarkupStyle::PROPS.ACCEPTED_DATA += {AbstractTextMarkup_t, Comment_t, Description_t, Feature_t, FontFeature_t, Linebreak_t, SizeFeature_t};
    TextMarkupStyle::PROPS.ANNOTATIONTYPE = AnnotationType::STYLE;
    TextMarkupStyle::PROPS.XMLTAG = "t-style";
    element_props[TextMarkupStyle_t] = &TextMarkupStyle::PROPS;
//------ TextMarkupWhitespace -------
    TextMarkupWhitespace::PROPS = AbstractTextMarkup::PROPS;
    TextMarkupWhitespace::PROPS.ELEMENT_ID = TextMarkupWhitespace_t;
    TextMarkupWhitespace::PROPS.ANNOTATIONTYPE = AnnotationType::WHITESPACE;
    TextMarkupWhitespace::PROPS.IMPLICITSPACE = true;
    TextMarkupWhitespace::PROPS.TEXTDELIMITER = "";
    TextMarkupWhitespace::PROPS.XMLTAG = "t-whitespace";
    element_props[TextMarkupWhitespace_t] = &TextMarkupWhitespace::PROPS;
//------ TimeFeature -------
    TimeFeature::PROPS = Feature::PROPS;
    TimeFeature::PROPS.ELEMENT_ID = TimeFeature_t;
    TimeFeature::PROPS.SUBSET = "time";
    TimeFeature::PROPS.XMLTAG = "time";
    element_props[TimeFeature_t] = &TimeFeature::PROPS;
//------ TimeSegment -------
    TimeSegment::PROPS = AbstractSpanAnnotation::PROPS;
    TimeSegment::PROPS.ELEMENT_ID = TimeSegment_t;
    TimeSegment::PROPS.ACCEPTED_DATA += {AbstractInlineAnnotation_t, ActorFeature_t, BegindatetimeFeature_t, Comment_t, Description_t, EnddatetimeFeature_t, Feature_t, ForeignData_t, LinkReference_t, Metric_t, Relation_t, WordReference_t, Hiddenword_t, Morpheme_t, Phoneme_t, Word_t};
    TimeSegment::PROPS.ANNOTATIONTYPE = AnnotationType::TIMESEGMENT;
    TimeSegment::PROPS.LABEL = "Time Segment";
    TimeSegment::PROPS.XMLTAG = "timesegment";
    element_props[TimeSegment_t] = &TimeSegment::PROPS;
//------ TimingLayer -------
    TimingLayer::PROPS = AbstractAnnotationLayer::PROPS;
    TimingLayer::PROPS.ELEMENT_ID = TimingLayer_t;
    TimingLayer::PROPS.ACCEPTED_DATA += {Comment_t, Correction_t, Description_t, ForeignData_t, TimeSegment_t};
    TimingLayer::PROPS.ANNOTATIONTYPE = AnnotationType::TIMESEGMENT;
    TimingLayer::PROPS.XMLTAG = "timing";
    element_props[TimingLayer_t] = &TimingLayer::PROPS;
//------ Utterance -------
    Utterance::PROPS = AbstractStructureElement::PROPS;
    Utterance::PROPS.ELEMENT_ID = Utterance_t;
    Utterance::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, AbstractInlineAnnotation_t, Alternative_t, AlternativeLayers_t, Comment_t, Correction_t, Description_t, External_t, Feature_t, ForeignData_t, Gap_t, Hiddenword_t, Metric_t, Note_t, Part_t, PhonContent_t, Quote_t, Reference_t, Relation_t, Sentence_t, String_t, TextContent_t, Word_t};
    Utterance::PROPS.ANNOTATIONTYPE = AnnotationType::UTTERANCE;
    Utterance::PROPS.LABEL = "Utterance";
    Utterance::PROPS.TEXTDELIMITER = " ";
    Utterance::PROPS.XMLTAG = "utt";
    element_props[Utterance_t] = &Utterance::PROPS;
//------ ValueFeature -------
    ValueFeature::PROPS = Feature::PROPS;
    ValueFeature::PROPS.ELEMENT_ID = ValueFeature_t;
    ValueFeature::PROPS.SUBSET = "value";
    ValueFeature::PROPS.XMLTAG = "value";
    element_props[ValueFeature_t] = &ValueFeature::PROPS;
//------ Whitespace -------
    Whitespace::PROPS = AbstractStructureElement::PROPS;
    Whitespace::PROPS.ELEMENT_ID = Whitespace_t;
    Whitespace::PROPS.ANNOTATIONTYPE = AnnotationType::WHITESPACE;
    Whitespace::PROPS.IMPLICITSPACE = true;
    Whitespace::PROPS.LABEL = "Whitespace";
    Whitespace::PROPS.TEXTDELIMITER = "";
    Whitespace::PROPS.XMLTAG = "whitespace";
    element_props[Whitespace_t] = &Whitespace::PROPS;
//------ Word -------
    Word::PROPS = AbstractStructureElement::PROPS;
    Word::PROPS.ELEMENT_ID = Word_t;
    Word::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, AbstractInlineAnnotation_t, Alternative_t, AlternativeLayers_t, Comment_t, Correction_t, Description_t, External_t, Feature_t, ForeignData_t, Metric_t, Part_t, PhonContent_t, Reference_t, Relation_t, String_t, TextContent_t};
    Word::PROPS.ANNOTATIONTYPE = AnnotationType::TOKEN;
    Word::PROPS.LABEL = "Word/Token";
    Word::PROPS.OPTIONAL_ATTRIBS = ID|CLASS|ANNOTATOR|N|CONFIDENCE|DATETIME|SRC|BEGINTIME|ENDTIME|SPEAKER|TEXTCLASS|METADATA|SPACE|TAG;
    Word::PROPS.TEXTDELIMITER = " ";
    Word::PROPS.WREFABLE = true;
    Word::PROPS.XMLTAG = "w";
    element_props[Word_t] = &Word::PROPS;
//------ WordReference -------
    WordReference::PROPS.ELEMENT_ID = WordReference_t;
    WordReference::PROPS.OPTIONAL_ATTRIBS = IDREF|TAG;
    WordReference::PROPS.XMLTAG = "wref";
    element_props[WordReference_t] = &WordReference::PROPS;

//foliaspec:end:setelementproperties

    XmlText::PROPS.XMLTAG = "_XmlText";
    XmlText::PROPS.ELEMENT_ID = XmlText_t;
    XmlText::PROPS.TEXTDELIMITER = "*";
    XmlText::PROPS.PRINTABLE = true;
    XmlText::PROPS.SPEAKABLE = true;

    XmlComment::PROPS.XMLTAG = "_XmlComment";
    XmlComment::PROPS.ELEMENT_ID = XmlComment_t;

    PlaceHolder::PROPS = Word::PROPS;
    PlaceHolder::PROPS.XMLTAG="_PlaceHolder";
    PlaceHolder::PROPS.ELEMENT_ID = PlaceHolder_t;
    PlaceHolder::PROPS.REQUIRED_ATTRIBS = NO_ATT;

    for ( const auto& it : oldtags ){
      reverse_old[it.second] = it.first;
    }

    for ( const auto& it : annotationtype_elementtype_map ){
      element_annotation_map[it.second] = it.first;
    }
  }


  //foliaspec:typehierarchy
  static const map<ElementType, set<ElementType> > typeHierarchy = {      { AbstractAnnotationLayer_t, {  } },
     { AbstractContentAnnotation_t, {  } },
     { AbstractCorrectionChild_t, {  } },
     { AbstractHigherOrderAnnotation_t, {  } },
     { AbstractInlineAnnotation_t, {  } },
     { AbstractSpanAnnotation_t, {  } },
     { AbstractSpanRole_t, { AbstractSpanAnnotation_t } },
     { AbstractStructureElement_t, {  } },
     { AbstractSubtokenAnnotation_t, {  } },
     { AbstractTextMarkup_t, {  } },
     { ActorFeature_t, { Feature_t,AbstractHigherOrderAnnotation_t } },
     { Alternative_t, { AbstractHigherOrderAnnotation_t } },
     { AlternativeLayers_t, { AbstractHigherOrderAnnotation_t } },
     { BegindatetimeFeature_t, { Feature_t,AbstractHigherOrderAnnotation_t } },
     { Caption_t, { AbstractStructureElement_t } },
     { Cell_t, { AbstractStructureElement_t } },
     { Chunk_t, { AbstractSpanAnnotation_t } },
     { ChunkingLayer_t, { AbstractAnnotationLayer_t } },
     { Comment_t, { AbstractHigherOrderAnnotation_t } },
     { Content_t, { AbstractContentAnnotation_t } },
     { CoreferenceChain_t, { AbstractSpanAnnotation_t } },
     { CoreferenceLayer_t, { AbstractAnnotationLayer_t } },
     { CoreferenceLink_t, { AbstractSpanRole_t,AbstractSpanAnnotation_t } },
     { Correction_t, { AbstractHigherOrderAnnotation_t } },
     { Cue_t, { AbstractSpanRole_t,AbstractSpanAnnotation_t } },
     { Current_t, { AbstractCorrectionChild_t } },
     { Definition_t, { AbstractStructureElement_t } },
     { DependenciesLayer_t, { AbstractAnnotationLayer_t } },
     { Dependency_t, { AbstractSpanAnnotation_t } },
     { DependencyDependent_t, { AbstractSpanRole_t,AbstractSpanAnnotation_t } },
     { Description_t, { AbstractHigherOrderAnnotation_t } },
     { Division_t, { AbstractStructureElement_t } },
     { DomainAnnotation_t, { AbstractInlineAnnotation_t } },
     { EnddatetimeFeature_t, { Feature_t,AbstractHigherOrderAnnotation_t } },
     { EntitiesLayer_t, { AbstractAnnotationLayer_t } },
     { Entity_t, { AbstractSpanAnnotation_t } },
     { Entry_t, { AbstractStructureElement_t } },
     { ErrorDetection_t, { AbstractInlineAnnotation_t } },
     { Event_t, { AbstractStructureElement_t } },
     { Example_t, { AbstractStructureElement_t } },
     { External_t, { AbstractHigherOrderAnnotation_t } },
     { Feature_t, { AbstractHigherOrderAnnotation_t } },
     { Figure_t, { AbstractStructureElement_t } },
     { FontFeature_t, { Feature_t,AbstractHigherOrderAnnotation_t } },
     { ForeignData_t, { AbstractHigherOrderAnnotation_t } },
     { FunctionFeature_t, { Feature_t,AbstractHigherOrderAnnotation_t } },
     { Gap_t, { AbstractHigherOrderAnnotation_t } },
     { Head_t, { AbstractStructureElement_t } },
     { HeadFeature_t, { Feature_t,AbstractHigherOrderAnnotation_t } },
     { Headspan_t, { AbstractSpanRole_t,AbstractSpanAnnotation_t } },
     { Hiddenword_t, { AbstractStructureElement_t } },
     { Hyphbreak_t, { AbstractTextMarkup_t } },
     { Label_t, { AbstractStructureElement_t } },
     { LangAnnotation_t, { AbstractInlineAnnotation_t } },
     { LemmaAnnotation_t, { AbstractInlineAnnotation_t } },
     { LevelFeature_t, { Feature_t,AbstractHigherOrderAnnotation_t } },
     { Linebreak_t, { AbstractStructureElement_t } },
     { LinkReference_t, {  } },
     { List_t, { AbstractStructureElement_t } },
     { ListItem_t, { AbstractStructureElement_t } },
     { Metric_t, { AbstractHigherOrderAnnotation_t } },
     { ModalitiesLayer_t, { AbstractAnnotationLayer_t } },
     { Modality_t, { AbstractSpanAnnotation_t } },
     { ModalityFeature_t, { Feature_t,AbstractHigherOrderAnnotation_t } },
     { Morpheme_t, { AbstractSubtokenAnnotation_t } },
     { MorphologyLayer_t, { AbstractAnnotationLayer_t } },
     { New_t, { AbstractCorrectionChild_t } },
     { Note_t, { AbstractStructureElement_t } },
     { Observation_t, { AbstractSpanAnnotation_t } },
     { ObservationLayer_t, { AbstractAnnotationLayer_t } },
     { Original_t, { AbstractCorrectionChild_t } },
     { Paragraph_t, { AbstractStructureElement_t } },
     { Part_t, { AbstractStructureElement_t } },
     { PhonContent_t, { AbstractContentAnnotation_t } },
     { Phoneme_t, { AbstractSubtokenAnnotation_t } },
     { PhonologyLayer_t, { AbstractAnnotationLayer_t } },
     { PolarityFeature_t, { Feature_t,AbstractHigherOrderAnnotation_t } },
     { PosAnnotation_t, { AbstractInlineAnnotation_t } },
     { Predicate_t, { AbstractSpanAnnotation_t } },
     { Quote_t, { AbstractStructureElement_t } },
     { Reference_t, { AbstractStructureElement_t } },
     { Relation_t, { AbstractHigherOrderAnnotation_t } },
     { Row_t, { AbstractStructureElement_t } },
     { Scope_t, { AbstractSpanRole_t,AbstractSpanAnnotation_t } },
     { SemanticRole_t, { AbstractSpanAnnotation_t } },
     { SemanticRolesLayer_t, { AbstractAnnotationLayer_t } },
     { SenseAnnotation_t, { AbstractInlineAnnotation_t } },
     { Sentence_t, { AbstractStructureElement_t } },
     { Sentiment_t, { AbstractSpanAnnotation_t } },
     { SentimentLayer_t, { AbstractAnnotationLayer_t } },
     { SizeFeature_t, { Feature_t,AbstractHigherOrderAnnotation_t } },
     { Source_t, { AbstractSpanRole_t,AbstractSpanAnnotation_t } },
     { SpanRelation_t, { AbstractHigherOrderAnnotation_t } },
     { SpanRelationLayer_t, { AbstractAnnotationLayer_t } },
     { Speech_t, { AbstractStructureElement_t } },
     { Statement_t, { AbstractSpanAnnotation_t } },
     { StatementLayer_t, { AbstractAnnotationLayer_t } },
     { StatementRelation_t, { AbstractSpanRole_t,AbstractSpanAnnotation_t } },
     { StrengthFeature_t, { Feature_t,AbstractHigherOrderAnnotation_t } },
     { String_t, { AbstractHigherOrderAnnotation_t } },
     { StyleFeature_t, { Feature_t,AbstractHigherOrderAnnotation_t } },
     { SubjectivityAnnotation_t, { AbstractInlineAnnotation_t } },
     { Suggestion_t, { AbstractCorrectionChild_t } },
     { SynsetFeature_t, { Feature_t,AbstractHigherOrderAnnotation_t } },
     { SyntacticUnit_t, { AbstractSpanAnnotation_t } },
     { SyntaxLayer_t, { AbstractAnnotationLayer_t } },
     { Table_t, { AbstractStructureElement_t } },
     { TableHead_t, { AbstractStructureElement_t } },
     { Target_t, { AbstractSpanRole_t,AbstractSpanAnnotation_t } },
     { Term_t, { AbstractStructureElement_t } },
     { Text_t, { AbstractStructureElement_t } },
     { TextContent_t, { AbstractContentAnnotation_t } },
     { TextMarkupCorrection_t, { AbstractTextMarkup_t } },
     { TextMarkupError_t, { AbstractTextMarkup_t } },
     { TextMarkupGap_t, { AbstractTextMarkup_t } },
     { TextMarkupHSpace_t, { AbstractTextMarkup_t } },
     { TextMarkupLanguage_t, { AbstractTextMarkup_t } },
     { TextMarkupReference_t, { AbstractTextMarkup_t } },
     { TextMarkupString_t, { AbstractTextMarkup_t } },
     { TextMarkupStyle_t, { AbstractTextMarkup_t } },
     { TextMarkupWhitespace_t, { AbstractTextMarkup_t } },
     { TimeFeature_t, { Feature_t,AbstractHigherOrderAnnotation_t } },
     { TimeSegment_t, { AbstractSpanAnnotation_t } },
     { TimingLayer_t, { AbstractAnnotationLayer_t } },
     { Utterance_t, { AbstractStructureElement_t } },
     { ValueFeature_t, { Feature_t,AbstractHigherOrderAnnotation_t } },
     { Whitespace_t, { AbstractStructureElement_t } },
     { Word_t, { AbstractStructureElement_t } },
     { WordReference_t, {  } },
     { PlaceHolder_t , { Word_t, AbstractStructureElement_t } }
  };

  //foliaspec:oldtags_map
  const map<string,string> oldtags = {
    { "alignment", "relation" },
    { "aref", "xref" },
    { "complexalignment", "spanrelation" },
    { "complexalignments", "spanrelations" },
    { "listitem", "item" },
  };

  map<string,string> reverse_old;
  map<ElementType,AnnotationType> element_annotation_map;

  //foliaspec:wrefables
  //Elements that act as words and can be referable from span annotations
  const set<ElementType> wrefables = { Hiddenword_t, Morpheme_t, Phoneme_t, Word_t };

  //foliaspec:annotationtype_elementtype_map
  //A mapping from annotation types to element types, based on the assumption that there is always only one primary element for an annotation type (and possible multiple secondary ones which are not included in this map,w)
  const map<AnnotationType,ElementType> annotationtype_elementtype_map = {
    {  AnnotationType::ALTERNATIVE, Alternative_t },
    {  AnnotationType::CHUNKING, Chunk_t },
    {  AnnotationType::COMMENT, Comment_t },
    {  AnnotationType::RAWCONTENT, Content_t },
    {  AnnotationType::COREFERENCE, CoreferenceChain_t },
    {  AnnotationType::CORRECTION, Correction_t },
    {  AnnotationType::DEFINITION, Definition_t },
    {  AnnotationType::DEPENDENCY, Dependency_t },
    {  AnnotationType::DESCRIPTION, Description_t },
    {  AnnotationType::DIVISION, Division_t },
    {  AnnotationType::DOMAIN, DomainAnnotation_t },
    {  AnnotationType::ENTITY, Entity_t },
    {  AnnotationType::ENTRY, Entry_t },
    {  AnnotationType::ERRORDETECTION, ErrorDetection_t },
    {  AnnotationType::EVENT, Event_t },
    {  AnnotationType::EXAMPLE, Example_t },
    {  AnnotationType::EXTERNAL, External_t },
    {  AnnotationType::FIGURE, Figure_t },
    {  AnnotationType::GAP, Gap_t },
    {  AnnotationType::HEAD, Head_t },
    {  AnnotationType::HIDDENTOKEN, Hiddenword_t },
    {  AnnotationType::HYPHENATION, Hyphbreak_t },
    {  AnnotationType::LANG, LangAnnotation_t },
    {  AnnotationType::LEMMA, LemmaAnnotation_t },
    {  AnnotationType::LINEBREAK, Linebreak_t },
    {  AnnotationType::LIST, List_t },
    {  AnnotationType::METRIC, Metric_t },
    {  AnnotationType::MODALITY, Modality_t },
    {  AnnotationType::MORPHOLOGICAL, Morpheme_t },
    {  AnnotationType::NOTE, Note_t },
    {  AnnotationType::OBSERVATION, Observation_t },
    {  AnnotationType::PARAGRAPH, Paragraph_t },
    {  AnnotationType::PART, Part_t },
    {  AnnotationType::PHON, PhonContent_t },
    {  AnnotationType::PHONOLOGICAL, Phoneme_t },
    {  AnnotationType::POS, PosAnnotation_t },
    {  AnnotationType::PREDICATE, Predicate_t },
    {  AnnotationType::QUOTE, Quote_t },
    {  AnnotationType::REFERENCE, Reference_t },
    {  AnnotationType::RELATION, Relation_t },
    {  AnnotationType::SEMROLE, SemanticRole_t },
    {  AnnotationType::SENSE, SenseAnnotation_t },
    {  AnnotationType::SENTENCE, Sentence_t },
    {  AnnotationType::SENTIMENT, Sentiment_t },
    {  AnnotationType::SPANRELATION, SpanRelation_t },
    {  AnnotationType::STATEMENT, Statement_t },
    {  AnnotationType::STRING, String_t },
    {  AnnotationType::SUBJECTIVITY, SubjectivityAnnotation_t },
    {  AnnotationType::SYNTAX, SyntacticUnit_t },
    {  AnnotationType::TABLE, Table_t },
    {  AnnotationType::TERM, Term_t },
    {  AnnotationType::TEXT, TextContent_t },
    {  AnnotationType::HSPACE, TextMarkupHSpace_t },
    {  AnnotationType::STYLE, TextMarkupStyle_t },
    {  AnnotationType::TIMESEGMENT, TimeSegment_t },
    {  AnnotationType::UTTERANCE, Utterance_t },
    {  AnnotationType::WHITESPACE, Whitespace_t },
    {  AnnotationType::TOKEN, Word_t },
  };

  //foliaspec:annotationtype_xml_map
  //A mapping from annotation types to xml tags (strings)
  const map<AnnotationType,string> annotationtype_xml_map = {
    {  AnnotationType::ALTERNATIVE, "alt" },
    {  AnnotationType::CHUNKING, "chunk" },
    {  AnnotationType::COMMENT, "comment" },
    {  AnnotationType::RAWCONTENT, "content" },
    {  AnnotationType::COREFERENCE, "coreferencechain" },
    {  AnnotationType::CORRECTION, "correction" },
    {  AnnotationType::DEFINITION, "def" },
    {  AnnotationType::DEPENDENCY, "dependency" },
    {  AnnotationType::DESCRIPTION, "desc" },
    {  AnnotationType::DIVISION, "div" },
    {  AnnotationType::DOMAIN, "domain" },
    {  AnnotationType::ENTITY, "entity" },
    {  AnnotationType::ENTRY, "entry" },
    {  AnnotationType::ERRORDETECTION, "errordetection" },
    {  AnnotationType::EVENT, "event" },
    {  AnnotationType::EXAMPLE, "ex" },
    {  AnnotationType::EXTERNAL, "external" },
    {  AnnotationType::FIGURE, "figure" },
    {  AnnotationType::GAP, "gap" },
    {  AnnotationType::HEAD, "head" },
    {  AnnotationType::HIDDENTOKEN, "hiddenw" },
    {  AnnotationType::HYPHENATION, "t-hbr" },
    {  AnnotationType::LANG, "lang" },
    {  AnnotationType::LEMMA, "lemma" },
    {  AnnotationType::LINEBREAK, "br" },
    {  AnnotationType::LIST, "list" },
    {  AnnotationType::METRIC, "metric" },
    {  AnnotationType::MODALITY, "modality" },
    {  AnnotationType::MORPHOLOGICAL, "morpheme" },
    {  AnnotationType::NOTE, "note" },
    {  AnnotationType::OBSERVATION, "observation" },
    {  AnnotationType::PARAGRAPH, "p" },
    {  AnnotationType::PART, "part" },
    {  AnnotationType::PHON, "ph" },
    {  AnnotationType::PHONOLOGICAL, "phoneme" },
    {  AnnotationType::POS, "pos" },
    {  AnnotationType::PREDICATE, "predicate" },
    {  AnnotationType::QUOTE, "quote" },
    {  AnnotationType::REFERENCE, "ref" },
    {  AnnotationType::RELATION, "relation" },
    {  AnnotationType::SEMROLE, "semrole" },
    {  AnnotationType::SENSE, "sense" },
    {  AnnotationType::SENTENCE, "s" },
    {  AnnotationType::SENTIMENT, "sentiment" },
    {  AnnotationType::SPANRELATION, "spanrelation" },
    {  AnnotationType::STATEMENT, "statement" },
    {  AnnotationType::STRING, "str" },
    {  AnnotationType::SUBJECTIVITY, "subjectivity" },
    {  AnnotationType::SYNTAX, "su" },
    {  AnnotationType::TABLE, "table" },
    {  AnnotationType::TERM, "term" },
    {  AnnotationType::TEXT, "t" },
    {  AnnotationType::HSPACE, "t-hspace" },
    {  AnnotationType::STYLE, "t-style" },
    {  AnnotationType::TIMESEGMENT, "timesegment" },
    {  AnnotationType::UTTERANCE, "utt" },
    {  AnnotationType::WHITESPACE, "whitespace" },
    {  AnnotationType::TOKEN, "w" },
  };

  properties::properties() {
      //foliaspec:begin:defaultproperties
      //Default properties which all elements inherit
      ELEMENT_ID = BASE;
      ACCEPTED_DATA.insert(XmlComment_t);
      ACCEPTED_DATA += {Description_t, Comment_t};
      ANNOTATIONTYPE = AnnotationType::NO_ANN;
      AUTH = true;
      AUTO_GENERATE_ID = false;
      HIDDEN = false;
      IMPLICITSPACE = false;
      OCCURRENCES = 0;
      OCCURRENCES_PER_SET = 0;
      OPTIONAL_ATTRIBS = NO_ATT;
      PHONCONTAINER = false;
      PRINTABLE = false;
      REQUIRED_ATTRIBS = NO_ATT;
      REQUIRED_DATA = {};
      SETONLY = false;
      SPEAKABLE = false;
      SUBSET = "";
      TEXTCONTAINER = false;
      TEXTDELIMITER = "NONE";
      WREFABLE = false;
      XLINK = false;
      XMLTAG = "NONE";

//foliaspec:end:defaultproperties
  }

  const int XML_PARSER_OPTIONS = XML_PARSE_NSCLEAN|XML_PARSE_HUGE;

  FoliaElement *FoliaElement::private_createElement( ElementType et ){
    switch ( et ){
    case BASE: return new FoLiA();
    case Text_t: return new Text();
    case Speech_t: return new Speech();
    case Utterance_t: return new Utterance();
    case ForeignData_t: return new ForeignData();
    case Entry_t: return new Entry();
    case Example_t: return new Example();
    case Term_t: return new Term();
    case Definition_t: return new Definition();
    case PhonContent_t: return new PhonContent();
    case Word_t: return new Word();
    case String_t: return new String();
    case Event_t: return new Event();
    case TimeSegment_t: return new TimeSegment();
    case TimingLayer_t: return new TimingLayer();
    case Sentence_t: return new Sentence();
    case TextContent_t: return new TextContent();
    case Linebreak_t: return new Linebreak();
    case Whitespace_t: return new Whitespace();
    case Comment_t: return new Comment();
    case Figure_t: return new Figure();
    case Caption_t: return new Caption();
    case Label_t: return new Label();
    case List_t: return new List();
    case ListItem_t: return new ListItem();
    case Paragraph_t: return new Paragraph();
    case New_t: return new New();
    case Original_t: return new Original();
    case Current_t: return new Current();
    case Suggestion_t: return new Suggestion();
    case Head_t: return new Head();
    case Table_t: return new Table();
    case TableHead_t: return new TableHead();
    case Cell_t: return new Cell();
    case Row_t: return new Row();
    case LangAnnotation_t: return new LangAnnotation();
    case XmlComment_t: return new XmlComment();
    case XmlText_t: return new XmlText();
    case External_t: return new External();
    case Note_t: return new Note();
    case Reference_t: return new Reference();
    case Description_t: return new Description();
    case Gap_t: return new Gap();
    case Content_t: return new Content();
    case Metric_t: return new Metric();
    case Division_t: return new Division();
    case PosAnnotation_t: return new PosAnnotation();
    case LemmaAnnotation_t: return new LemmaAnnotation();
    case PhonologyLayer_t: return new PhonologyLayer();
    case Phoneme_t: return new Phoneme();
    case DomainAnnotation_t: return new DomainAnnotation();
    case SenseAnnotation_t: return new SenseAnnotation();
    case SyntaxLayer_t: return new SyntaxLayer();
    case SubjectivityAnnotation_t: return new SubjectivityAnnotation();
    case Chunk_t: return new Chunk();
    case ChunkingLayer_t: return new ChunkingLayer();
    case Entity_t: return new Entity();
    case EntitiesLayer_t: return new EntitiesLayer();
    case SemanticRolesLayer_t: return new SemanticRolesLayer();
    case SemanticRole_t: return new SemanticRole();
    case CoreferenceLayer_t: return new CoreferenceLayer();
    case CoreferenceLink_t: return new CoreferenceLink();
    case CoreferenceChain_t:  return new CoreferenceChain();
    case Alternative_t: return new Alternative();
    case PlaceHolder_t: return new PlaceHolder();
    case AlternativeLayers_t: return new AlternativeLayers();
    case SyntacticUnit_t: return new SyntacticUnit();
    case WordReference_t: return new WordReference();
    case Correction_t: return new Correction();
    case ErrorDetection_t: return new ErrorDetection();
    case MorphologyLayer_t: return new MorphologyLayer();
    case Morpheme_t: return new Morpheme();
    case Feature_t: return new Feature();
    case BegindatetimeFeature_t: return new BegindatetimeFeature();
    case EnddatetimeFeature_t: return new EnddatetimeFeature();
    case SynsetFeature_t: return new SynsetFeature();
    case ActorFeature_t: return new ActorFeature();
    case HeadFeature_t: return new HeadFeature();
    case ValueFeature_t: return new ValueFeature();
    case TimeFeature_t: return new TimeFeature();
    case ModalityFeature_t: return new ModalityFeature();
    case FunctionFeature_t: return new FunctionFeature();
    case LevelFeature_t: return new LevelFeature();
    case FontFeature_t: return new FontFeature();
    case SizeFeature_t: return new SizeFeature();
    case Quote_t: return new Quote();
    case DependenciesLayer_t: return new DependenciesLayer();
    case Dependency_t: return new Dependency();
    case DependencyDependent_t: return new DependencyDependent();
    case Headspan_t: return new Headspan();
    case SpanRelationLayer_t: return new SpanRelationLayer();
    case SpanRelation_t: return new SpanRelation();
    case LinkReference_t: return new LinkReference();
    case TextMarkupString_t: return new TextMarkupString();
    case TextMarkupGap_t: return new TextMarkupGap();
    case TextMarkupCorrection_t: return new TextMarkupCorrection();
    case TextMarkupError_t: return new TextMarkupError();
    case TextMarkupStyle_t: return new TextMarkupStyle();
    case TextMarkupHSpace_t: return new TextMarkupHSpace();
    case TextMarkupLanguage_t: return new TextMarkupLanguage();
    case TextMarkupWhitespace_t: return new TextMarkupWhitespace();
    case TextMarkupReference_t: return new TextMarkupReference();
    case Part_t: return new Part();
    case Observation_t: return new Observation();
    case ObservationLayer_t: return new ObservationLayer();
    case PolarityFeature_t: return new PolarityFeature();
    case Predicate_t: return new Predicate();
    case Relation_t: return new Relation();
    case Sentiment_t: return new Sentiment();
    case Modality_t: return new Modality();
    case SentimentLayer_t: return new SentimentLayer();
    case ModalitiesLayer_t: return new ModalitiesLayer();
    case Statement_t: return new Statement();
    case StatementRelation_t: return new StatementRelation();
    case StatementLayer_t: return new StatementLayer();
    case StrengthFeature_t: return new StrengthFeature();
    case StyleFeature_t: return new StyleFeature();
    case Source_t: return new Source();
    case Target_t: return new Target();
    case Scope_t: return new Scope();
    case Cue_t: return new Cue();
    case Hiddenword_t: return new Hiddenword();
    case Hyphbreak_t: return new Hyphbreak();
    case AbstractContentAnnotation_t:
    case AbstractHigherOrderAnnotation_t:
    case AbstractSubtokenAnnotation_t:
    case AbstractSpanAnnotation_t:
    case AbstractSpanRole_t:
    case AbstractAnnotationLayer_t:
    case AbstractTextMarkup_t:
    case AbstractInlineAnnotation_t:
    case AbstractStructureElement_t:
    case AbstractCorrectionChild_t:
      throw ValueError( "you may not create an abstract node of type "
			+ TiCC::toString(int(et)) + ")" );
    default:
      throw ValueError( "private_createElement: unknown elementtype("
			+ TiCC::toString(int(et)) + ")" );
    }
    return 0;
  }

  bool isSubClass( const ElementType e1, const ElementType e2 ){
    /// check if an ElementType is a subclass of another one
    /*!
      \param e1 an ElementType
      \param e2 an ElementType
      \return true if e1 is in the typeHierarchy of e2
    */
    if ( e1 == e2 )
      return true;
    const auto& it = typeHierarchy.find( e1 );
    if ( it != typeHierarchy.end() ){
      return it->second.find( e2 ) != it->second.end();
    }
    return false;
  }

  bool isSubClass( const FoliaElement *e1, const FoliaElement *e2 ){
    /// check if a FoliaElement is a subclass of another one
    /*!
      \param e1 a FoliaElement
      \param e2 a FoliaElement
      \return true if e1 is a subclass of e2
      This is about C++ class inheritance: is our class a derivative of c's
      class?
    */
    return isSubClass( e1->element_id(), e2->element_id() );
  }

  bool FoliaElement::isSubClass( ElementType t ) const {
    /// check if this FoliaElement is a subclass of the ElementType \e t
    /*!
      \param t an ElementType
      \return true if our class is a subclass of t
      This is about C++ class inheritance: is our class a derivative of c's
      class?
    */
    return folia::isSubClass( element_id(), t );
  }

  bool isAttributeFeature( const string& att ){
    /// check if an attribute is to be handled as a feature
    /*!
      \param att the attribute to check
      \return true if this attribute has to be stored internally as an Feature

      example, the FoLiA fragment:
      \verbatim
      <sentiment xml:id="sentiment.1" class="disappointment" polarity="negative" strength="strong">
      \endverbatim

      will internally be stored as
      \verbatim
      <sentiment class="disappointment">
        <polarity class="negative"/>
        <strength class="strong"/>
      </sentiment>
      \endverbatim
      because polarity and strenght are AttributeFeatures
     */
    return AttributeFeatures.find( att ) != AttributeFeatures.end();
  }

  void print_type_hierarchy( ostream& os ){
    /// output the type hierarchy. DEBUGGING PURPOSES ONLY
    for ( auto const& top : typeHierarchy ){
      os << toString(top.first) << endl;
      for ( auto const& el : top.second ){
	os << "     -- " << toString(el) << endl;
      }
    }
  }

  namespace {
    //
    // this trick assures that the static_int() function is called
    // exactly once en every run.
    // This because the static 'i' is initialized before main() starts.
    //
    struct initializer {
     initializer() {
	 static_init();
	//	print_type_hierarchy( cout );
      }
      ~initializer() {
	// cout << "Unloading the properties" << endl;
	// cout << "but don't care.." << endl;
      }
    };
    static initializer i;
  }

}
