/*
  Copyright (c) 2006 - 2026
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
//This file was last updated according to the FoLiA specification for version 2.5.3 on 2024-12-03 16:42:15, using foliaspec.py
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
  const int SUB_VERSION = 3;

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
    { AnnotationType::ETYMOLOGY,  "etymology" },
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
    { "etymology", AnnotationType::ETYMOLOGY },
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
    { ElementType::BASE, "FoLiA" },
    { ElementType::AbstractFeature_t, "_AbstractFeature" },
    { ElementType::AbstractWord_t, "_AbstractWord" },
    { ElementType::AbstractAnnotationLayer_t,  "_AbstractAnnotationLayer" },
    { ElementType::AbstractContentAnnotation_t,  "_AbstractContentAnnotation" },
    { ElementType::AbstractCorrectionChild_t,  "_AbstractCorrectionChild" },
    { ElementType::AbstractHigherOrderAnnotation_t,  "_AbstractHigherOrderAnnotation" },
    { ElementType::AbstractInlineAnnotation_t,  "_AbstractInlineAnnotation" },
    { ElementType::AbstractSpanAnnotation_t,  "_AbstractSpanAnnotation" },
    { ElementType::AbstractSpanRole_t,  "_AbstractSpanRole" },
    { ElementType::AbstractStructureElement_t,  "_AbstractStructureElement" },
    { ElementType::AbstractSubtokenAnnotation_t,  "_AbstractSubtokenAnnotation" },
    { ElementType::AbstractTextMarkup_t,  "_AbstractTextMarkup" },
    { ElementType::ActorFeature_t,  "actor" },
    { ElementType::Alternative_t,  "alt" },
    { ElementType::AlternativeLayers_t,  "altlayers" },
    { ElementType::BegindatetimeFeature_t,  "begindatetime" },
    { ElementType::Caption_t,  "caption" },
    { ElementType::Cell_t,  "cell" },
    { ElementType::Chunk_t,  "chunk" },
    { ElementType::ChunkingLayer_t,  "chunking" },
    { ElementType::Comment_t,  "comment" },
    { ElementType::Content_t,  "content" },
    { ElementType::CoreferenceChain_t,  "coreferencechain" },
    { ElementType::CoreferenceLayer_t,  "coreferences" },
    { ElementType::CoreferenceLink_t,  "coreferencelink" },
    { ElementType::Correction_t,  "correction" },
    { ElementType::Cue_t,  "cue" },
    { ElementType::Current_t,  "current" },
    { ElementType::Definition_t,  "def" },
    { ElementType::DependenciesLayer_t,  "dependencies" },
    { ElementType::Dependency_t,  "dependency" },
    { ElementType::DependencyDependent_t,  "dep" },
    { ElementType::Description_t,  "desc" },
    { ElementType::Division_t,  "div" },
    { ElementType::DomainAnnotation_t,  "domain" },
    { ElementType::EnddatetimeFeature_t,  "enddatetime" },
    { ElementType::EntitiesLayer_t,  "entities" },
    { ElementType::Entity_t,  "entity" },
    { ElementType::Entry_t,  "entry" },
    { ElementType::ErrorDetection_t,  "errordetection" },
    { ElementType::EtymologyAnnotation_t,  "etymology" },
    { ElementType::Event_t,  "event" },
    { ElementType::Example_t,  "ex" },
    { ElementType::External_t,  "external" },
    { ElementType::Feature_t,  "feat" },
    { ElementType::Figure_t,  "figure" },
    { ElementType::FontFeature_t,  "font" },
    { ElementType::ForeignData_t,  "foreign-data" },
    { ElementType::FunctionFeature_t,  "function" },
    { ElementType::Gap_t,  "gap" },
    { ElementType::Head_t,  "head" },
    { ElementType::HeadFeature_t,  "headfeature" },
    { ElementType::Headspan_t,  "hd" },
    { ElementType::Hiddenword_t,  "hiddenw" },
    { ElementType::Hyphbreak_t,  "t-hbr" },
    { ElementType::Label_t,  "label" },
    { ElementType::LangAnnotation_t,  "lang" },
    { ElementType::LemmaAnnotation_t,  "lemma" },
    { ElementType::LevelFeature_t,  "level" },
    { ElementType::Linebreak_t,  "br" },
    { ElementType::LinkReference_t,  "xref" },
    { ElementType::List_t,  "list" },
    { ElementType::ListItem_t,  "item" },
    { ElementType::Metric_t,  "metric" },
    { ElementType::ModalitiesLayer_t,  "modalities" },
    { ElementType::Modality_t,  "modality" },
    { ElementType::ModalityFeature_t,  "mod" },
    { ElementType::Morpheme_t,  "morpheme" },
    { ElementType::MorphologyLayer_t,  "morphology" },
    { ElementType::New_t,  "new" },
    { ElementType::Note_t,  "note" },
    { ElementType::Observation_t,  "observation" },
    { ElementType::ObservationLayer_t,  "observations" },
    { ElementType::Original_t,  "original" },
    { ElementType::Paragraph_t,  "p" },
    { ElementType::Part_t,  "part" },
    { ElementType::PhonContent_t,  "ph" },
    { ElementType::Phoneme_t,  "phoneme" },
    { ElementType::PhonologyLayer_t,  "phonology" },
    { ElementType::PolarityFeature_t,  "polarity" },
    { ElementType::PosAnnotation_t,  "pos" },
    { ElementType::Predicate_t,  "predicate" },
    { ElementType::Quote_t,  "quote" },
    { ElementType::Reference_t,  "ref" },
    { ElementType::Relation_t,  "relation" },
    { ElementType::Row_t,  "row" },
    { ElementType::Scope_t,  "scope" },
    { ElementType::SemanticRole_t,  "semrole" },
    { ElementType::SemanticRolesLayer_t,  "semroles" },
    { ElementType::SenseAnnotation_t,  "sense" },
    { ElementType::Sentence_t,  "s" },
    { ElementType::Sentiment_t,  "sentiment" },
    { ElementType::SentimentLayer_t,  "sentiments" },
    { ElementType::SizeFeature_t,  "size" },
    { ElementType::Source_t,  "source" },
    { ElementType::SpanRelation_t,  "spanrelation" },
    { ElementType::SpanRelationLayer_t,  "spanrelations" },
    { ElementType::Speech_t,  "speech" },
    { ElementType::Statement_t,  "statement" },
    { ElementType::StatementLayer_t,  "statements" },
    { ElementType::StatementRelation_t,  "rel" },
    { ElementType::StrengthFeature_t,  "strength" },
    { ElementType::String_t,  "str" },
    { ElementType::StyleFeature_t,  "style" },
    { ElementType::SubjectivityAnnotation_t,  "subjectivity" },
    { ElementType::Suggestion_t,  "suggestion" },
    { ElementType::SynsetFeature_t,  "synset" },
    { ElementType::SyntacticUnit_t,  "su" },
    { ElementType::SyntaxLayer_t,  "syntax" },
    { ElementType::Table_t,  "table" },
    { ElementType::TableHead_t,  "tablehead" },
    { ElementType::Target_t,  "target" },
    { ElementType::Term_t,  "term" },
    { ElementType::Text_t,  "text" },
    { ElementType::TextContent_t,  "t" },
    { ElementType::TextMarkupCorrection_t,  "t-correction" },
    { ElementType::TextMarkupError_t,  "t-error" },
    { ElementType::TextMarkupGap_t,  "t-gap" },
    { ElementType::TextMarkupHSpace_t,  "t-hspace" },
    { ElementType::TextMarkupLanguage_t,  "t-lang" },
    { ElementType::TextMarkupReference_t,  "t-ref" },
    { ElementType::TextMarkupString_t,  "t-str" },
    { ElementType::TextMarkupStyle_t,  "t-style" },
    { ElementType::TextMarkupWhitespace_t,  "t-whitespace" },
    { ElementType::TimeFeature_t,  "time" },
    { ElementType::TimeSegment_t,  "timesegment" },
    { ElementType::TimingLayer_t,  "timing" },
    { ElementType::Utterance_t,  "utt" },
    { ElementType::ValueFeature_t,  "value" },
    { ElementType::Whitespace_t,  "whitespace" },
    { ElementType::Word_t,  "w" },
    { ElementType::WordReference_t,  "wref" },
    { ElementType::XmlComment_t, "_XmlComment" },
    { ElementType::ProcessingInstruction_t, "PI" },
    { ElementType::XmlText_t, "_XmlText" }
  };

  //foliaspec:string_elementtype_map
  const map<string,ElementType> s_et_map = {
    { "FoLiA", ElementType::BASE },
    { "_AbstractFeature", ElementType::AbstractFeature_t },
    { "_AbstractWord", ElementType::AbstractWord_t },
    { "_AbstractAnnotationLayer", ElementType::AbstractAnnotationLayer_t  },
    { "_AbstractContentAnnotation", ElementType::AbstractContentAnnotation_t  },
    { "_AbstractCorrectionChild", ElementType::AbstractCorrectionChild_t  },
    { "_AbstractHigherOrderAnnotation", ElementType::AbstractHigherOrderAnnotation_t  },
    { "_AbstractInlineAnnotation", ElementType::AbstractInlineAnnotation_t  },
    { "_AbstractSpanAnnotation", ElementType::AbstractSpanAnnotation_t  },
    { "_AbstractSpanRole", ElementType::AbstractSpanRole_t  },
    { "_AbstractStructureElement", ElementType::AbstractStructureElement_t  },
    { "_AbstractSubtokenAnnotation", ElementType::AbstractSubtokenAnnotation_t  },
    { "_AbstractTextMarkup", ElementType::AbstractTextMarkup_t  },
    { "actor", ElementType::ActorFeature_t  },
    { "alt", ElementType::Alternative_t  },
    { "altlayers", ElementType::AlternativeLayers_t  },
    { "begindatetime", ElementType::BegindatetimeFeature_t  },
    { "caption", ElementType::Caption_t  },
    { "cell", ElementType::Cell_t  },
    { "chunk", ElementType::Chunk_t  },
    { "chunking", ElementType::ChunkingLayer_t  },
    { "comment", ElementType::Comment_t  },
    { "content", ElementType::Content_t  },
    { "coreferencechain", ElementType::CoreferenceChain_t  },
    { "coreferences", ElementType::CoreferenceLayer_t  },
    { "coreferencelink", ElementType::CoreferenceLink_t  },
    { "correction", ElementType::Correction_t  },
    { "cue", ElementType::Cue_t  },
    { "current", ElementType::Current_t  },
    { "def", ElementType::Definition_t  },
    { "dependencies", ElementType::DependenciesLayer_t  },
    { "dependency", ElementType::Dependency_t  },
    { "dep", ElementType::DependencyDependent_t  },
    { "desc", ElementType::Description_t  },
    { "div", ElementType::Division_t  },
    { "domain", ElementType::DomainAnnotation_t  },
    { "enddatetime", ElementType::EnddatetimeFeature_t  },
    { "entities", ElementType::EntitiesLayer_t  },
    { "entity", ElementType::Entity_t  },
    { "entry", ElementType::Entry_t  },
    { "errordetection", ElementType::ErrorDetection_t  },
    { "etymology", ElementType::EtymologyAnnotation_t  },
    { "event", ElementType::Event_t  },
    { "ex", ElementType::Example_t  },
    { "external", ElementType::External_t  },
    { "feat", ElementType::Feature_t  },
    { "figure", ElementType::Figure_t  },
    { "font", ElementType::FontFeature_t  },
    { "foreign-data", ElementType::ForeignData_t  },
    { "function", ElementType::FunctionFeature_t  },
    { "gap", ElementType::Gap_t  },
    { "head", ElementType::Head_t  },
    { "headfeature", ElementType::HeadFeature_t },
    { "hd", ElementType::Headspan_t  },
    { "hiddenw", ElementType::Hiddenword_t  },
    { "t-hbr", ElementType::Hyphbreak_t  },
    { "label", ElementType::Label_t  },
    { "lang", ElementType::LangAnnotation_t  },
    { "lemma", ElementType::LemmaAnnotation_t  },
    { "level", ElementType::LevelFeature_t  },
    { "br", ElementType::Linebreak_t  },
    { "xref", ElementType::LinkReference_t  },
    { "list", ElementType::List_t  },
    { "item", ElementType::ListItem_t  },
    { "metric", ElementType::Metric_t  },
    { "modalities", ElementType::ModalitiesLayer_t  },
    { "modality", ElementType::Modality_t  },
    { "mod", ElementType::ModalityFeature_t  },
    { "morpheme", ElementType::Morpheme_t  },
    { "morphology", ElementType::MorphologyLayer_t  },
    { "new", ElementType::New_t  },
    { "note", ElementType::Note_t  },
    { "observation", ElementType::Observation_t  },
    { "observations", ElementType::ObservationLayer_t  },
    { "original", ElementType::Original_t  },
    { "p", ElementType::Paragraph_t  },
    { "part", ElementType::Part_t  },
    { "ph", ElementType::PhonContent_t  },
    { "phoneme", ElementType::Phoneme_t  },
    { "phonology", ElementType::PhonologyLayer_t  },
    { "polarity", ElementType::PolarityFeature_t  },
    { "pos", ElementType::PosAnnotation_t  },
    { "predicate", ElementType::Predicate_t  },
    { "quote", ElementType::Quote_t  },
    { "ref", ElementType::Reference_t  },
    { "relation", ElementType::Relation_t  },
    { "row", ElementType::Row_t  },
    { "scope", ElementType::Scope_t  },
    { "semrole", ElementType::SemanticRole_t  },
    { "semroles", ElementType::SemanticRolesLayer_t  },
    { "sense", ElementType::SenseAnnotation_t  },
    { "s", ElementType::Sentence_t  },
    { "sentiment", ElementType::Sentiment_t  },
    { "sentiments", ElementType::SentimentLayer_t  },
    { "size", ElementType::SizeFeature_t  },
    { "source", ElementType::Source_t  },
    { "spanrelation", ElementType::SpanRelation_t  },
    { "spanrelations", ElementType::SpanRelationLayer_t  },
    { "speech", ElementType::Speech_t  },
    { "statement", ElementType::Statement_t  },
    { "statements", ElementType::StatementLayer_t  },
    { "rel", ElementType::StatementRelation_t  },
    { "strength", ElementType::StrengthFeature_t  },
    { "str", ElementType::String_t  },
    { "style", ElementType::StyleFeature_t  },
    { "subjectivity", ElementType::SubjectivityAnnotation_t  },
    { "suggestion", ElementType::Suggestion_t  },
    { "synset", ElementType::SynsetFeature_t  },
    { "su", ElementType::SyntacticUnit_t  },
    { "syntax", ElementType::SyntaxLayer_t  },
    { "table", ElementType::Table_t  },
    { "tablehead", ElementType::TableHead_t  },
    { "target", ElementType::Target_t  },
    { "term", ElementType::Term_t  },
    { "text", ElementType::Text_t  },
    { "t", ElementType::TextContent_t  },
    { "t-correction", ElementType::TextMarkupCorrection_t  },
    { "t-error", ElementType::TextMarkupError_t  },
    { "t-gap", ElementType::TextMarkupGap_t  },
    { "t-hspace", ElementType::TextMarkupHSpace_t  },
    { "t-lang", ElementType::TextMarkupLanguage_t  },
    { "t-ref", ElementType::TextMarkupReference_t  },
    { "t-str", ElementType::TextMarkupString_t  },
    { "t-style", ElementType::TextMarkupStyle_t  },
    { "t-whitespace", ElementType::TextMarkupWhitespace_t  },
    { "time", ElementType::TimeFeature_t  },
    { "timesegment", ElementType::TimeSegment_t  },
    { "timing", ElementType::TimingLayer_t  },
    { "utt", ElementType::Utterance_t  },
    { "value", ElementType::ValueFeature_t  },
    { "whitespace", ElementType::Whitespace_t  },
    { "w", ElementType::Word_t  },
    { "wref", ElementType::WordReference_t  },
    { "_XmlComment", ElementType::XmlComment_t  },
    { "PI", ElementType::ProcessingInstruction_t  },
    { "_XmlText", ElementType::XmlText_t  }
  };

  //foliaspec:default_ignore
  //Default ignore list for the select() method, do not descend into these
  const set<ElementType> default_ignore = { ElementType::Alternative_t, ElementType::AlternativeLayers_t, ElementType::ForeignData_t, ElementType::Original_t, ElementType::Suggestion_t };

  //foliaspec:default_ignore_annotations
  //Default ignore list for token annotation
  const set<ElementType> default_ignore_annotations = { ElementType::Alternative_t, ElementType::AlternativeLayers_t, ElementType::MorphologyLayer_t, ElementType::Original_t, ElementType::PhonologyLayer_t, ElementType::Suggestion_t };

  //foliaspec:default_ignore_structure
  //Default ignore list for structure annotation
  const set<ElementType> default_ignore_structure = { ElementType::Alternative_t, ElementType::AlternativeLayers_t, ElementType::ChunkingLayer_t, ElementType::CoreferenceLayer_t, ElementType::DependenciesLayer_t, ElementType::EntitiesLayer_t, ElementType::ModalitiesLayer_t, ElementType::MorphologyLayer_t, ElementType::ObservationLayer_t, ElementType::Original_t, ElementType::PhonologyLayer_t, ElementType::SemanticRolesLayer_t, ElementType::SentimentLayer_t, ElementType::SpanRelationLayer_t, ElementType::StatementLayer_t, ElementType::Suggestion_t, ElementType::SyntaxLayer_t, ElementType::TimingLayer_t };

  const set<ElementType> AnnoExcludeSet = { ElementType::Original_t,
					    ElementType::Suggestion_t };

  const set<ElementType> SpanSet = { ElementType::SyntacticUnit_t,
				     ElementType::Chunk_t,
				     ElementType::Entity_t,
				     ElementType::Headspan_t,
				     ElementType::DependencyDependent_t,
				     ElementType::CoreferenceLink_t,
				     ElementType::CoreferenceChain_t,
				     ElementType::SemanticRole_t,
				     ElementType::SemanticRolesLayer_t,
				     ElementType::TimeSegment_t };

  // Abstract properties are not in the external specification
  properties ABSTRACT_STRUCTURE_ELEMENT_PROPERTIES;
  properties ABSTRACT_ANNOTATION_LAYER_PROPERTIES;
  properties ABSTRACT_CONTENT_ANNOTATION_PROPERTIES;
  properties ABSTRACT_CORRECTION_CHILD_PROPERTIES;
  properties ABSTRACT_HIGHER_ORDER_ANNOTATION_PROPERTIES;
  properties ABSTRACT_INLINE_ANNOTATION_PROPERTIES;
  properties ABSTRACT_SPAN_ANNOTATION_PROPERTIES;
  properties ABSTRACT_SPAN_ROLE_PROPERTIES;
  properties ABSTRACT_SUBTOKEN_ANNOTATION_PROPERTIES;
  properties ABSTRACT_TEXT_MARKUP_PROPERTIES;
  properties ABSTRACT_FEATURE_PROPERTIES;
  properties ABSTRACT_WORD_PROPERTIES;
  properties DEFAULT_PROPERTIES;
  //these are also not in the external specification yet: (libfolia internal)
  properties FoLiA::PROPS = DEFAULT_PROPERTIES;
  properties DCOI::PROPS = DEFAULT_PROPERTIES;
  properties XmlComment::PROPS = DEFAULT_PROPERTIES;
  properties XmlText::PROPS = DEFAULT_PROPERTIES;
  properties ProcessingInstruction::PROPS = DEFAULT_PROPERTIES;

  //foliaspec:instantiateelementproperties
  //Instantiates all element properties for the first time, setting them to the default properties
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
  properties EtymologyAnnotation::PROPS = DEFAULT_PROPERTIES;
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
  map<ElementType,ElementType> abstract_parents;

  ElementType get_abstract_parent( const ElementType et ) {
    const auto& it = abstract_parents.find( et );
    if ( it != abstract_parents.end() ){
      return it->second;
    }
    return ElementType::BASE;
  }

  ElementType get_abstract_parent( const FoliaElement *el ) {
    return get_abstract_parent( el->element_id() );
  }

  void static_init(){
    /// initialize a lot of statics ('constants')
    /// This function should be called once.
    FoLiA::PROPS.XMLTAG = "FoLiA";
    FoLiA::PROPS.ACCEPTED_DATA += { ElementType::Text_t, ElementType::Speech_t };
    FoLiA::PROPS.OPTIONAL_ATTRIBS = Attrib::ID;
    element_props[ElementType::BASE] = &FoLiA::PROPS;

    DCOI::PROPS.XMLTAG = "DCOI";
    DCOI::PROPS.ACCEPTED_DATA += { ElementType::Text_t, ElementType::Speech_t };

    //foliaspec:begin:setelementproperties
//------ AbstractFeature -------
    ABSTRACT_FEATURE_PROPERTIES = ABSTRACT_HIGHER_ORDER_ANNOTATION_PROPERTIES;
    ABSTRACT_FEATURE_PROPERTIES.ELEMENT_ID = ElementType::AbstractFeature_t;
    ABSTRACT_FEATURE_PROPERTIES.LABEL = "AbstractFeature";
    element_props[ElementType::AbstractFeature_t] = &ABSTRACT_FEATURE_PROPERTIES;
//------ AbstractWord -------
    ABSTRACT_WORD_PROPERTIES = ABSTRACT_STRUCTURE_ELEMENT_PROPERTIES;
    ABSTRACT_WORD_PROPERTIES.ELEMENT_ID = ElementType::AbstractWord_t;
    ABSTRACT_WORD_PROPERTIES.LABEL = "AbstractWord";
    ABSTRACT_WORD_PROPERTIES.PRINTABLE = true;
    ABSTRACT_WORD_PROPERTIES.SPEAKABLE = true;
    element_props[ElementType::AbstractWord_t] = &ABSTRACT_WORD_PROPERTIES;
//------ AbstractAnnotationLayer -------
    ABSTRACT_ANNOTATION_LAYER_PROPERTIES.ELEMENT_ID = ElementType::AbstractAnnotationLayer_t;
    ABSTRACT_ANNOTATION_LAYER_PROPERTIES.ACCEPTED_DATA += {ElementType::Comment_t, ElementType::Correction_t, ElementType::Description_t, ElementType::ForeignData_t};
    ABSTRACT_ANNOTATION_LAYER_PROPERTIES.OPTIONAL_ATTRIBS = Attrib::ID|Attrib::TAG;
    ABSTRACT_ANNOTATION_LAYER_PROPERTIES.PRINTABLE = false;
    ABSTRACT_ANNOTATION_LAYER_PROPERTIES.SETONLY = true;
    ABSTRACT_ANNOTATION_LAYER_PROPERTIES.SPEAKABLE = false;
    element_props[ElementType::AbstractAnnotationLayer_t] = &ABSTRACT_ANNOTATION_LAYER_PROPERTIES;
//------ AbstractContentAnnotation -------
    ABSTRACT_CONTENT_ANNOTATION_PROPERTIES.ELEMENT_ID = ElementType::AbstractContentAnnotation_t;
    ABSTRACT_CONTENT_ANNOTATION_PROPERTIES.OCCURRENCES = 0;
    ABSTRACT_CONTENT_ANNOTATION_PROPERTIES.OPTIONAL_ATTRIBS = Attrib::CLASS|Attrib::ANNOTATOR|Attrib::CONFIDENCE|Attrib::DATETIME|Attrib::METADATA|Attrib::TAG;
    ABSTRACT_CONTENT_ANNOTATION_PROPERTIES.REQUIRED_ATTRIBS = Attrib::NO_ATT;
    element_props[ElementType::AbstractContentAnnotation_t] = &ABSTRACT_CONTENT_ANNOTATION_PROPERTIES;
//------ AbstractCorrectionChild -------
    ABSTRACT_CORRECTION_CHILD_PROPERTIES.ELEMENT_ID = ElementType::AbstractCorrectionChild_t;
    ABSTRACT_CORRECTION_CHILD_PROPERTIES.ACCEPTED_DATA += {ElementType::AbstractInlineAnnotation_t, ElementType::AbstractSpanAnnotation_t, ElementType::AbstractStructureElement_t, ElementType::Comment_t, ElementType::Correction_t, ElementType::Description_t, ElementType::ForeignData_t, ElementType::Metric_t, ElementType::PhonContent_t, ElementType::String_t, ElementType::TextContent_t};
    ABSTRACT_CORRECTION_CHILD_PROPERTIES.OPTIONAL_ATTRIBS = Attrib::ID|Attrib::ANNOTATOR|Attrib::CONFIDENCE|Attrib::DATETIME|Attrib::N|Attrib::TAG;
    ABSTRACT_CORRECTION_CHILD_PROPERTIES.PRINTABLE = true;
    ABSTRACT_CORRECTION_CHILD_PROPERTIES.SPEAKABLE = true;
    ABSTRACT_CORRECTION_CHILD_PROPERTIES.TEXTDELIMITER = "NONE";
    element_props[ElementType::AbstractCorrectionChild_t] = &ABSTRACT_CORRECTION_CHILD_PROPERTIES;
//------ AbstractHigherOrderAnnotation -------
    ABSTRACT_HIGHER_ORDER_ANNOTATION_PROPERTIES.ELEMENT_ID = ElementType::AbstractHigherOrderAnnotation_t;
    ABSTRACT_HIGHER_ORDER_ANNOTATION_PROPERTIES.OPTIONAL_ATTRIBS = Attrib::NO_ATT;
    ABSTRACT_HIGHER_ORDER_ANNOTATION_PROPERTIES.REQUIRED_ATTRIBS = Attrib::NO_ATT;
    element_props[ElementType::AbstractHigherOrderAnnotation_t] = &ABSTRACT_HIGHER_ORDER_ANNOTATION_PROPERTIES;
//------ AbstractInlineAnnotation -------
    ABSTRACT_INLINE_ANNOTATION_PROPERTIES.ELEMENT_ID = ElementType::AbstractInlineAnnotation_t;
    ABSTRACT_INLINE_ANNOTATION_PROPERTIES.ACCEPTED_DATA += {ElementType::AbstractFeature_t, ElementType::Comment_t, ElementType::Description_t, ElementType::ForeignData_t, ElementType::Metric_t};
    ABSTRACT_INLINE_ANNOTATION_PROPERTIES.OCCURRENCES_PER_SET = 1;
    ABSTRACT_INLINE_ANNOTATION_PROPERTIES.OPTIONAL_ATTRIBS = Attrib::ID|Attrib::CLASS|Attrib::ANNOTATOR|Attrib::N|Attrib::CONFIDENCE|Attrib::DATETIME|Attrib::SRC|Attrib::BEGINTIME|Attrib::ENDTIME|Attrib::SPEAKER|Attrib::TEXTCLASS|Attrib::METADATA|Attrib::TAG;
    ABSTRACT_INLINE_ANNOTATION_PROPERTIES.REQUIRED_ATTRIBS = Attrib::CLASS;
    element_props[ElementType::AbstractInlineAnnotation_t] = &ABSTRACT_INLINE_ANNOTATION_PROPERTIES;
//------ AbstractSpanAnnotation -------
    ABSTRACT_SPAN_ANNOTATION_PROPERTIES.ELEMENT_ID = ElementType::AbstractSpanAnnotation_t;
    ABSTRACT_SPAN_ANNOTATION_PROPERTIES.ACCEPTED_DATA += {ElementType::AbstractInlineAnnotation_t, ElementType::Comment_t, ElementType::Description_t, ElementType::ForeignData_t, ElementType::LinkReference_t, ElementType::Metric_t, ElementType::Relation_t};
    ABSTRACT_SPAN_ANNOTATION_PROPERTIES.OPTIONAL_ATTRIBS = Attrib::ID|Attrib::CLASS|Attrib::ANNOTATOR|Attrib::N|Attrib::CONFIDENCE|Attrib::DATETIME|Attrib::SRC|Attrib::BEGINTIME|Attrib::ENDTIME|Attrib::SPEAKER|Attrib::TEXTCLASS|Attrib::METADATA|Attrib::TAG;
    ABSTRACT_SPAN_ANNOTATION_PROPERTIES.PRINTABLE = true;
    ABSTRACT_SPAN_ANNOTATION_PROPERTIES.SPEAKABLE = true;
    element_props[ElementType::AbstractSpanAnnotation_t] = &ABSTRACT_SPAN_ANNOTATION_PROPERTIES;
//------ AbstractSpanRole -------
    ABSTRACT_SPAN_ROLE_PROPERTIES = ABSTRACT_SPAN_ANNOTATION_PROPERTIES;
    ABSTRACT_SPAN_ROLE_PROPERTIES.ELEMENT_ID = ElementType::AbstractSpanRole_t;
    ABSTRACT_SPAN_ROLE_PROPERTIES.ACCEPTED_DATA += {ElementType::AbstractFeature_t, ElementType::AbstractInlineAnnotation_t, ElementType::AbstractWord_t, ElementType::Comment_t, ElementType::Description_t, ElementType::ForeignData_t, ElementType::LinkReference_t, ElementType::Metric_t, ElementType::Morpheme_t, ElementType::Phoneme_t, ElementType::Relation_t, ElementType::WordReference_t};
    ABSTRACT_SPAN_ROLE_PROPERTIES.OPTIONAL_ATTRIBS = Attrib::ID|Attrib::TAG;
    element_props[ElementType::AbstractSpanRole_t] = &ABSTRACT_SPAN_ROLE_PROPERTIES;
//------ AbstractStructureElement -------
    ABSTRACT_STRUCTURE_ELEMENT_PROPERTIES.ELEMENT_ID = ElementType::AbstractStructureElement_t;
    ABSTRACT_STRUCTURE_ELEMENT_PROPERTIES.ACCEPTED_DATA += {ElementType::AbstractAnnotationLayer_t, ElementType::AbstractFeature_t, ElementType::Alternative_t, ElementType::AlternativeLayers_t, ElementType::Comment_t, ElementType::Correction_t, ElementType::Description_t, ElementType::External_t, ElementType::ForeignData_t, ElementType::Metric_t, ElementType::Part_t, ElementType::Relation_t};
    ABSTRACT_STRUCTURE_ELEMENT_PROPERTIES.AUTO_GENERATE_ID = true;
    ABSTRACT_STRUCTURE_ELEMENT_PROPERTIES.OPTIONAL_ATTRIBS = Attrib::ID|Attrib::CLASS|Attrib::ANNOTATOR|Attrib::N|Attrib::CONFIDENCE|Attrib::DATETIME|Attrib::SRC|Attrib::BEGINTIME|Attrib::ENDTIME|Attrib::SPEAKER|Attrib::METADATA|Attrib::SPACE|Attrib::TAG;
    ABSTRACT_STRUCTURE_ELEMENT_PROPERTIES.PRINTABLE = true;
    ABSTRACT_STRUCTURE_ELEMENT_PROPERTIES.REQUIRED_ATTRIBS = Attrib::NO_ATT;
    ABSTRACT_STRUCTURE_ELEMENT_PROPERTIES.SPEAKABLE = true;
    ABSTRACT_STRUCTURE_ELEMENT_PROPERTIES.TEXTDELIMITER = "\n\n";
    element_props[ElementType::AbstractStructureElement_t] = &ABSTRACT_STRUCTURE_ELEMENT_PROPERTIES;
//------ AbstractSubtokenAnnotation -------
    ABSTRACT_SUBTOKEN_ANNOTATION_PROPERTIES.ELEMENT_ID = ElementType::AbstractSubtokenAnnotation_t;
    ABSTRACT_SUBTOKEN_ANNOTATION_PROPERTIES.ACCEPTED_DATA += {ElementType::AbstractAnnotationLayer_t, ElementType::AbstractFeature_t, ElementType::Alternative_t, ElementType::AlternativeLayers_t, ElementType::Comment_t, ElementType::Correction_t, ElementType::Description_t, ElementType::ForeignData_t, ElementType::Metric_t, ElementType::Part_t, ElementType::Relation_t};
    ABSTRACT_SUBTOKEN_ANNOTATION_PROPERTIES.AUTO_GENERATE_ID = true;
    ABSTRACT_SUBTOKEN_ANNOTATION_PROPERTIES.OPTIONAL_ATTRIBS = Attrib::ID|Attrib::CLASS|Attrib::ANNOTATOR|Attrib::N|Attrib::CONFIDENCE|Attrib::DATETIME|Attrib::SRC|Attrib::BEGINTIME|Attrib::ENDTIME|Attrib::SPEAKER|Attrib::METADATA|Attrib::TAG;
    ABSTRACT_SUBTOKEN_ANNOTATION_PROPERTIES.PRINTABLE = true;
    ABSTRACT_SUBTOKEN_ANNOTATION_PROPERTIES.REQUIRED_ATTRIBS = Attrib::NO_ATT;
    ABSTRACT_SUBTOKEN_ANNOTATION_PROPERTIES.SPEAKABLE = true;
    ABSTRACT_SUBTOKEN_ANNOTATION_PROPERTIES.TEXTDELIMITER = "\n\n";
    element_props[ElementType::AbstractSubtokenAnnotation_t] = &ABSTRACT_SUBTOKEN_ANNOTATION_PROPERTIES;
//------ AbstractTextMarkup -------
    ABSTRACT_TEXT_MARKUP_PROPERTIES.ELEMENT_ID = ElementType::AbstractTextMarkup_t;
    ABSTRACT_TEXT_MARKUP_PROPERTIES.ACCEPTED_DATA += {ElementType::AbstractFeature_t, ElementType::AbstractTextMarkup_t, ElementType::Comment_t, ElementType::Description_t, ElementType::Linebreak_t, ElementType::XmlText_t};
    ABSTRACT_TEXT_MARKUP_PROPERTIES.OPTIONAL_ATTRIBS = Attrib::ID|Attrib::CLASS|Attrib::ANNOTATOR|Attrib::N|Attrib::CONFIDENCE|Attrib::DATETIME|Attrib::SRC|Attrib::BEGINTIME|Attrib::ENDTIME|Attrib::SPEAKER|Attrib::METADATA|Attrib::TAG;
    ABSTRACT_TEXT_MARKUP_PROPERTIES.PRINTABLE = true;
    ABSTRACT_TEXT_MARKUP_PROPERTIES.TEXTCONTAINER = true;
    ABSTRACT_TEXT_MARKUP_PROPERTIES.TEXTDELIMITER = "";
    ABSTRACT_TEXT_MARKUP_PROPERTIES.XLINK = true;
    element_props[ElementType::AbstractTextMarkup_t] = &ABSTRACT_TEXT_MARKUP_PROPERTIES;
//------ Alternative -------
    Alternative::PROPS = ABSTRACT_HIGHER_ORDER_ANNOTATION_PROPERTIES;
    abstract_parents[ElementType::Alternative_t] = ElementType::AbstractHigherOrderAnnotation_t;
    Alternative::PROPS.ELEMENT_ID = ElementType::Alternative_t;
    Alternative::PROPS.ACCEPTED_DATA += {ElementType::AbstractInlineAnnotation_t, ElementType::Comment_t, ElementType::Correction_t, ElementType::Description_t, ElementType::ForeignData_t, ElementType::MorphologyLayer_t, ElementType::PhonologyLayer_t};
    Alternative::PROPS.ANNOTATIONTYPE = AnnotationType::ALTERNATIVE;
    Alternative::PROPS.AUTH = false;
    Alternative::PROPS.LABEL = "Alternative";
    Alternative::PROPS.OPTIONAL_ATTRIBS = Attrib::ID|Attrib::ANNOTATOR|Attrib::N|Attrib::CONFIDENCE|Attrib::DATETIME|Attrib::SRC|Attrib::BEGINTIME|Attrib::ENDTIME|Attrib::SPEAKER|Attrib::METADATA|Attrib::TAG;
    Alternative::PROPS.PRINTABLE = false;
    Alternative::PROPS.REQUIRED_ATTRIBS = Attrib::NO_ATT;
    Alternative::PROPS.SPEAKABLE = false;
    Alternative::PROPS.XMLTAG = "alt";
    element_props[ElementType::Alternative_t] = &Alternative::PROPS;
//------ AlternativeLayers -------
    AlternativeLayers::PROPS = ABSTRACT_HIGHER_ORDER_ANNOTATION_PROPERTIES;
    abstract_parents[ElementType::AlternativeLayers_t] = ElementType::AbstractHigherOrderAnnotation_t;
    AlternativeLayers::PROPS.ELEMENT_ID = ElementType::AlternativeLayers_t;
    AlternativeLayers::PROPS.ACCEPTED_DATA += {ElementType::AbstractAnnotationLayer_t, ElementType::Comment_t, ElementType::Description_t, ElementType::ForeignData_t};
    AlternativeLayers::PROPS.ANNOTATIONTYPE = AnnotationType::ALTERNATIVE;
    AlternativeLayers::PROPS.AUTH = false;
    AlternativeLayers::PROPS.LABEL = "Alternative Layers";
    AlternativeLayers::PROPS.OPTIONAL_ATTRIBS = Attrib::ID|Attrib::ANNOTATOR|Attrib::N|Attrib::CONFIDENCE|Attrib::DATETIME|Attrib::SRC|Attrib::BEGINTIME|Attrib::ENDTIME|Attrib::SPEAKER|Attrib::METADATA|Attrib::TAG;
    AlternativeLayers::PROPS.PRINTABLE = false;
    AlternativeLayers::PROPS.REQUIRED_ATTRIBS = Attrib::NO_ATT;
    AlternativeLayers::PROPS.SPEAKABLE = false;
    AlternativeLayers::PROPS.XMLTAG = "altlayers";
    element_props[ElementType::AlternativeLayers_t] = &AlternativeLayers::PROPS;
//------ Caption -------
    Caption::PROPS = ABSTRACT_STRUCTURE_ELEMENT_PROPERTIES;
    abstract_parents[ElementType::Caption_t] = ElementType::AbstractStructureElement_t;
    Caption::PROPS.ELEMENT_ID = ElementType::Caption_t;
    Caption::PROPS.ACCEPTED_DATA += {ElementType::AbstractAnnotationLayer_t, ElementType::AbstractFeature_t, ElementType::AbstractInlineAnnotation_t, ElementType::Alternative_t, ElementType::AlternativeLayers_t, ElementType::Comment_t, ElementType::Correction_t, ElementType::Description_t, ElementType::External_t, ElementType::ForeignData_t, ElementType::Gap_t, ElementType::Linebreak_t, ElementType::Metric_t, ElementType::Paragraph_t, ElementType::Part_t, ElementType::PhonContent_t, ElementType::Quote_t, ElementType::Reference_t, ElementType::Relation_t, ElementType::Sentence_t, ElementType::String_t, ElementType::TextContent_t, ElementType::Whitespace_t};
    Caption::PROPS.LABEL = "Caption";
    Caption::PROPS.OCCURRENCES = 1;
    Caption::PROPS.OPTIONAL_ATTRIBS = Attrib::ID|Attrib::ANNOTATOR|Attrib::N|Attrib::CONFIDENCE|Attrib::DATETIME|Attrib::SRC|Attrib::BEGINTIME|Attrib::ENDTIME|Attrib::SPEAKER|Attrib::METADATA|Attrib::SPACE|Attrib::TAG;
    Caption::PROPS.XMLTAG = "caption";
    element_props[ElementType::Caption_t] = &Caption::PROPS;
//------ Cell -------
    Cell::PROPS = ABSTRACT_STRUCTURE_ELEMENT_PROPERTIES;
    abstract_parents[ElementType::Cell_t] = ElementType::AbstractStructureElement_t;
    Cell::PROPS.ELEMENT_ID = ElementType::Cell_t;
    Cell::PROPS.ACCEPTED_DATA += {ElementType::AbstractAnnotationLayer_t, ElementType::AbstractFeature_t, ElementType::AbstractInlineAnnotation_t, ElementType::AbstractWord_t, ElementType::Alternative_t, ElementType::AlternativeLayers_t, ElementType::Comment_t, ElementType::Correction_t, ElementType::Description_t, ElementType::Entry_t, ElementType::Event_t, ElementType::Example_t, ElementType::External_t, ElementType::Figure_t, ElementType::ForeignData_t, ElementType::Gap_t, ElementType::Head_t, ElementType::Linebreak_t, ElementType::List_t, ElementType::Metric_t, ElementType::Note_t, ElementType::Paragraph_t, ElementType::Part_t, ElementType::Quote_t, ElementType::Reference_t, ElementType::Relation_t, ElementType::Sentence_t, ElementType::String_t, ElementType::TextContent_t, ElementType::Whitespace_t};
    Cell::PROPS.LABEL = "Cell";
    Cell::PROPS.OPTIONAL_ATTRIBS = Attrib::ID|Attrib::ANNOTATOR|Attrib::N|Attrib::CONFIDENCE|Attrib::DATETIME|Attrib::SRC|Attrib::BEGINTIME|Attrib::ENDTIME|Attrib::SPEAKER|Attrib::METADATA|Attrib::SPACE|Attrib::TAG;
    Cell::PROPS.TEXTDELIMITER = " | ";
    Cell::PROPS.XMLTAG = "cell";
    element_props[ElementType::Cell_t] = &Cell::PROPS;
//------ Chunk -------
    Chunk::PROPS = ABSTRACT_SPAN_ANNOTATION_PROPERTIES;
    abstract_parents[ElementType::Chunk_t] = ElementType::AbstractSpanAnnotation_t;
    Chunk::PROPS.ELEMENT_ID = ElementType::Chunk_t;
    Chunk::PROPS.ACCEPTED_DATA += {ElementType::AbstractFeature_t, ElementType::AbstractInlineAnnotation_t, ElementType::AbstractWord_t, ElementType::Comment_t, ElementType::Description_t, ElementType::ForeignData_t, ElementType::LinkReference_t, ElementType::Metric_t, ElementType::Morpheme_t, ElementType::Phoneme_t, ElementType::Relation_t, ElementType::WordReference_t};
    Chunk::PROPS.ANNOTATIONTYPE = AnnotationType::CHUNKING;
    Chunk::PROPS.LABEL = "Chunk";
    Chunk::PROPS.XMLTAG = "chunk";
    element_props[ElementType::Chunk_t] = &Chunk::PROPS;
//------ ChunkingLayer -------
    ChunkingLayer::PROPS = ABSTRACT_ANNOTATION_LAYER_PROPERTIES;
    abstract_parents[ElementType::ChunkingLayer_t] = ElementType::AbstractAnnotationLayer_t;
    ChunkingLayer::PROPS.ELEMENT_ID = ElementType::ChunkingLayer_t;
    ChunkingLayer::PROPS.ACCEPTED_DATA += {ElementType::Chunk_t, ElementType::Comment_t, ElementType::Correction_t, ElementType::Description_t, ElementType::ForeignData_t};
    ChunkingLayer::PROPS.ANNOTATIONTYPE = AnnotationType::CHUNKING;
    ChunkingLayer::PROPS.XMLTAG = "chunking";
    element_props[ElementType::ChunkingLayer_t] = &ChunkingLayer::PROPS;
//------ Comment -------
    Comment::PROPS = ABSTRACT_HIGHER_ORDER_ANNOTATION_PROPERTIES;
    abstract_parents[ElementType::Comment_t] = ElementType::AbstractHigherOrderAnnotation_t;
    Comment::PROPS.ELEMENT_ID = ElementType::Comment_t;
    Comment::PROPS.ANNOTATIONTYPE = AnnotationType::COMMENT;
    Comment::PROPS.LABEL = "Comment";
    Comment::PROPS.OPTIONAL_ATTRIBS = Attrib::ID|Attrib::ANNOTATOR|Attrib::CONFIDENCE|Attrib::DATETIME|Attrib::N|Attrib::METADATA|Attrib::TAG;
    Comment::PROPS.PRINTABLE = false;
    Comment::PROPS.XMLTAG = "comment";
    element_props[ElementType::Comment_t] = &Comment::PROPS;
//------ Content -------
    Content::PROPS = ABSTRACT_CONTENT_ANNOTATION_PROPERTIES;
    abstract_parents[ElementType::Content_t] = ElementType::AbstractContentAnnotation_t;
    Content::PROPS.ELEMENT_ID = ElementType::Content_t;
    Content::PROPS.ANNOTATIONTYPE = AnnotationType::RAWCONTENT;
    Content::PROPS.LABEL = "Raw Content";
    Content::PROPS.OCCURRENCES = 1;
    Content::PROPS.PRINTABLE = true;
    Content::PROPS.XMLTAG = "content";
    element_props[ElementType::Content_t] = &Content::PROPS;
//------ CoreferenceChain -------
    CoreferenceChain::PROPS = ABSTRACT_SPAN_ANNOTATION_PROPERTIES;
    abstract_parents[ElementType::CoreferenceChain_t] = ElementType::AbstractSpanAnnotation_t;
    CoreferenceChain::PROPS.ELEMENT_ID = ElementType::CoreferenceChain_t;
    CoreferenceChain::PROPS.ACCEPTED_DATA += {ElementType::AbstractFeature_t, ElementType::AbstractInlineAnnotation_t, ElementType::Comment_t, ElementType::CoreferenceLink_t, ElementType::Description_t, ElementType::ForeignData_t, ElementType::LinkReference_t, ElementType::Metric_t, ElementType::Relation_t};
    CoreferenceChain::PROPS.ANNOTATIONTYPE = AnnotationType::COREFERENCE;
    CoreferenceChain::PROPS.LABEL = "Coreference Chain";
    CoreferenceChain::PROPS.REQUIRED_DATA += {ElementType::CoreferenceLink_t};
    CoreferenceChain::PROPS.XMLTAG = "coreferencechain";
    element_props[ElementType::CoreferenceChain_t] = &CoreferenceChain::PROPS;
//------ CoreferenceLayer -------
    CoreferenceLayer::PROPS = ABSTRACT_ANNOTATION_LAYER_PROPERTIES;
    abstract_parents[ElementType::CoreferenceLayer_t] = ElementType::AbstractAnnotationLayer_t;
    CoreferenceLayer::PROPS.ELEMENT_ID = ElementType::CoreferenceLayer_t;
    CoreferenceLayer::PROPS.ACCEPTED_DATA += {ElementType::Comment_t, ElementType::CoreferenceChain_t, ElementType::Correction_t, ElementType::Description_t, ElementType::ForeignData_t};
    CoreferenceLayer::PROPS.ANNOTATIONTYPE = AnnotationType::COREFERENCE;
    CoreferenceLayer::PROPS.XMLTAG = "coreferences";
    element_props[ElementType::CoreferenceLayer_t] = &CoreferenceLayer::PROPS;
//------ CoreferenceLink -------
    CoreferenceLink::PROPS = ABSTRACT_SPAN_ROLE_PROPERTIES;
    abstract_parents[ElementType::CoreferenceLink_t] = ElementType::AbstractSpanRole_t;
    CoreferenceLink::PROPS.ELEMENT_ID = ElementType::CoreferenceLink_t;
    CoreferenceLink::PROPS.ACCEPTED_DATA += {ElementType::AbstractFeature_t, ElementType::AbstractInlineAnnotation_t, ElementType::AbstractWord_t, ElementType::Comment_t, ElementType::Description_t, ElementType::ForeignData_t, ElementType::Headspan_t, ElementType::LevelFeature_t, ElementType::LinkReference_t, ElementType::Metric_t, ElementType::ModalityFeature_t, ElementType::Morpheme_t, ElementType::Phoneme_t, ElementType::Relation_t, ElementType::TimeFeature_t, ElementType::WordReference_t};
    CoreferenceLink::PROPS.ANNOTATIONTYPE = AnnotationType::COREFERENCE;
    CoreferenceLink::PROPS.LABEL = "Coreference Link";
    CoreferenceLink::PROPS.XMLTAG = "coreferencelink";
    element_props[ElementType::CoreferenceLink_t] = &CoreferenceLink::PROPS;
//------ Correction -------
    Correction::PROPS = ABSTRACT_HIGHER_ORDER_ANNOTATION_PROPERTIES;
    abstract_parents[ElementType::Correction_t] = ElementType::AbstractHigherOrderAnnotation_t;
    Correction::PROPS.ELEMENT_ID = ElementType::Correction_t;
    Correction::PROPS.ACCEPTED_DATA += {ElementType::AbstractFeature_t, ElementType::Comment_t, ElementType::Current_t, ElementType::Description_t, ElementType::ErrorDetection_t, ElementType::ForeignData_t, ElementType::Metric_t, ElementType::New_t, ElementType::Original_t, ElementType::Suggestion_t};
    Correction::PROPS.ANNOTATIONTYPE = AnnotationType::CORRECTION;
    Correction::PROPS.LABEL = "Correction";
    Correction::PROPS.OPTIONAL_ATTRIBS = Attrib::ID|Attrib::CLASS|Attrib::ANNOTATOR|Attrib::N|Attrib::CONFIDENCE|Attrib::DATETIME|Attrib::SRC|Attrib::BEGINTIME|Attrib::ENDTIME|Attrib::SPEAKER|Attrib::METADATA|Attrib::TAG;
    Correction::PROPS.PRINTABLE = true;
    Correction::PROPS.SPEAKABLE = true;
    Correction::PROPS.TEXTDELIMITER = "NONE";
    Correction::PROPS.XMLTAG = "correction";
    element_props[ElementType::Correction_t] = &Correction::PROPS;
//------ Cue -------
    Cue::PROPS = ABSTRACT_SPAN_ROLE_PROPERTIES;
    abstract_parents[ElementType::Cue_t] = ElementType::AbstractSpanRole_t;
    Cue::PROPS.ELEMENT_ID = ElementType::Cue_t;
    Cue::PROPS.LABEL = "Cue";
    Cue::PROPS.OCCURRENCES = 1;
    Cue::PROPS.XMLTAG = "cue";
    element_props[ElementType::Cue_t] = &Cue::PROPS;
//------ Current -------
    Current::PROPS = ABSTRACT_CORRECTION_CHILD_PROPERTIES;
    abstract_parents[ElementType::Current_t] = ElementType::AbstractCorrectionChild_t;
    Current::PROPS.ELEMENT_ID = ElementType::Current_t;
    Current::PROPS.ANNOTATIONTYPE = AnnotationType::CORRECTION;
    Current::PROPS.OCCURRENCES = 1;
    Current::PROPS.OPTIONAL_ATTRIBS = Attrib::TAG;
    Current::PROPS.XMLTAG = "current";
    element_props[ElementType::Current_t] = &Current::PROPS;
//------ Definition -------
    Definition::PROPS = ABSTRACT_STRUCTURE_ELEMENT_PROPERTIES;
    abstract_parents[ElementType::Definition_t] = ElementType::AbstractStructureElement_t;
    Definition::PROPS.ELEMENT_ID = ElementType::Definition_t;
    Definition::PROPS.ACCEPTED_DATA += {ElementType::AbstractAnnotationLayer_t, ElementType::AbstractFeature_t, ElementType::AbstractInlineAnnotation_t, ElementType::AbstractWord_t, ElementType::Alternative_t, ElementType::AlternativeLayers_t, ElementType::Comment_t, ElementType::Correction_t, ElementType::Description_t, ElementType::External_t, ElementType::Figure_t, ElementType::ForeignData_t, ElementType::Linebreak_t, ElementType::List_t, ElementType::Metric_t, ElementType::Paragraph_t, ElementType::Part_t, ElementType::PhonContent_t, ElementType::Reference_t, ElementType::Relation_t, ElementType::Sentence_t, ElementType::String_t, ElementType::Table_t, ElementType::TextContent_t, ElementType::Utterance_t, ElementType::Whitespace_t};
    Definition::PROPS.ANNOTATIONTYPE = AnnotationType::DEFINITION;
    Definition::PROPS.LABEL = "Definition";
    Definition::PROPS.XMLTAG = "def";
    element_props[ElementType::Definition_t] = &Definition::PROPS;
//------ DependenciesLayer -------
    DependenciesLayer::PROPS = ABSTRACT_ANNOTATION_LAYER_PROPERTIES;
    abstract_parents[ElementType::DependenciesLayer_t] = ElementType::AbstractAnnotationLayer_t;
    DependenciesLayer::PROPS.ELEMENT_ID = ElementType::DependenciesLayer_t;
    DependenciesLayer::PROPS.ACCEPTED_DATA += {ElementType::Comment_t, ElementType::Correction_t, ElementType::Dependency_t, ElementType::Description_t, ElementType::ForeignData_t};
    DependenciesLayer::PROPS.ANNOTATIONTYPE = AnnotationType::DEPENDENCY;
    DependenciesLayer::PROPS.XMLTAG = "dependencies";
    element_props[ElementType::DependenciesLayer_t] = &DependenciesLayer::PROPS;
//------ Dependency -------
    Dependency::PROPS = ABSTRACT_SPAN_ANNOTATION_PROPERTIES;
    abstract_parents[ElementType::Dependency_t] = ElementType::AbstractSpanAnnotation_t;
    Dependency::PROPS.ELEMENT_ID = ElementType::Dependency_t;
    Dependency::PROPS.ACCEPTED_DATA += {ElementType::AbstractFeature_t, ElementType::AbstractInlineAnnotation_t, ElementType::Comment_t, ElementType::DependencyDependent_t, ElementType::Description_t, ElementType::ForeignData_t, ElementType::Headspan_t, ElementType::LinkReference_t, ElementType::Metric_t, ElementType::Relation_t};
    Dependency::PROPS.ANNOTATIONTYPE = AnnotationType::DEPENDENCY;
    Dependency::PROPS.LABEL = "Dependency";
    Dependency::PROPS.REQUIRED_DATA += {ElementType::DependencyDependent_t, ElementType::Headspan_t};
    Dependency::PROPS.XMLTAG = "dependency";
    element_props[ElementType::Dependency_t] = &Dependency::PROPS;
//------ DependencyDependent -------
    DependencyDependent::PROPS = ABSTRACT_SPAN_ROLE_PROPERTIES;
    abstract_parents[ElementType::DependencyDependent_t] = ElementType::AbstractSpanRole_t;
    DependencyDependent::PROPS.ELEMENT_ID = ElementType::DependencyDependent_t;
    DependencyDependent::PROPS.LABEL = "Dependent";
    DependencyDependent::PROPS.OCCURRENCES = 1;
    DependencyDependent::PROPS.XMLTAG = "dep";
    element_props[ElementType::DependencyDependent_t] = &DependencyDependent::PROPS;
//------ Description -------
    Description::PROPS = ABSTRACT_HIGHER_ORDER_ANNOTATION_PROPERTIES;
    abstract_parents[ElementType::Description_t] = ElementType::AbstractHigherOrderAnnotation_t;
    Description::PROPS.ELEMENT_ID = ElementType::Description_t;
    Description::PROPS.ANNOTATIONTYPE = AnnotationType::DESCRIPTION;
    Description::PROPS.LABEL = "Description";
    Description::PROPS.OCCURRENCES = 1;
    Description::PROPS.OPTIONAL_ATTRIBS = Attrib::ID|Attrib::ANNOTATOR|Attrib::CONFIDENCE|Attrib::DATETIME|Attrib::N|Attrib::METADATA|Attrib::TAG;
    Description::PROPS.XMLTAG = "desc";
    element_props[ElementType::Description_t] = &Description::PROPS;
//------ Division -------
    Division::PROPS = ABSTRACT_STRUCTURE_ELEMENT_PROPERTIES;
    abstract_parents[ElementType::Division_t] = ElementType::AbstractStructureElement_t;
    Division::PROPS.ELEMENT_ID = ElementType::Division_t;
    Division::PROPS.ACCEPTED_DATA += {ElementType::AbstractAnnotationLayer_t, ElementType::AbstractFeature_t, ElementType::AbstractInlineAnnotation_t, ElementType::AbstractWord_t, ElementType::Alternative_t, ElementType::AlternativeLayers_t, ElementType::Comment_t, ElementType::Correction_t, ElementType::Description_t, ElementType::Division_t, ElementType::Entry_t, ElementType::Event_t, ElementType::Example_t, ElementType::External_t, ElementType::Figure_t, ElementType::ForeignData_t, ElementType::Gap_t, ElementType::Head_t, ElementType::Linebreak_t, ElementType::List_t, ElementType::Metric_t, ElementType::Note_t, ElementType::Paragraph_t, ElementType::Part_t, ElementType::PhonContent_t, ElementType::Quote_t, ElementType::Reference_t, ElementType::Relation_t, ElementType::Sentence_t, ElementType::Table_t, ElementType::TextContent_t, ElementType::Utterance_t, ElementType::Whitespace_t};
    Division::PROPS.ANNOTATIONTYPE = AnnotationType::DIVISION;
    Division::PROPS.LABEL = "Division";
    Division::PROPS.TEXTDELIMITER = "\n\n\n";
    Division::PROPS.XMLTAG = "div";
    element_props[ElementType::Division_t] = &Division::PROPS;
//------ DomainAnnotation -------
    DomainAnnotation::PROPS = ABSTRACT_INLINE_ANNOTATION_PROPERTIES;
    abstract_parents[ElementType::DomainAnnotation_t] = ElementType::AbstractInlineAnnotation_t;
    DomainAnnotation::PROPS.ELEMENT_ID = ElementType::DomainAnnotation_t;
    DomainAnnotation::PROPS.ANNOTATIONTYPE = AnnotationType::DOMAIN;
    DomainAnnotation::PROPS.LABEL = "Domain";
    DomainAnnotation::PROPS.OCCURRENCES_PER_SET = 0;
    DomainAnnotation::PROPS.XMLTAG = "domain";
    element_props[ElementType::DomainAnnotation_t] = &DomainAnnotation::PROPS;
//------ EntitiesLayer -------
    EntitiesLayer::PROPS = ABSTRACT_ANNOTATION_LAYER_PROPERTIES;
    abstract_parents[ElementType::EntitiesLayer_t] = ElementType::AbstractAnnotationLayer_t;
    EntitiesLayer::PROPS.ELEMENT_ID = ElementType::EntitiesLayer_t;
    EntitiesLayer::PROPS.ACCEPTED_DATA += {ElementType::Comment_t, ElementType::Correction_t, ElementType::Description_t, ElementType::Entity_t, ElementType::ForeignData_t};
    EntitiesLayer::PROPS.ANNOTATIONTYPE = AnnotationType::ENTITY;
    EntitiesLayer::PROPS.XMLTAG = "entities";
    element_props[ElementType::EntitiesLayer_t] = &EntitiesLayer::PROPS;
//------ Entity -------
    Entity::PROPS = ABSTRACT_SPAN_ANNOTATION_PROPERTIES;
    abstract_parents[ElementType::Entity_t] = ElementType::AbstractSpanAnnotation_t;
    Entity::PROPS.ELEMENT_ID = ElementType::Entity_t;
    Entity::PROPS.ACCEPTED_DATA += {ElementType::AbstractFeature_t, ElementType::AbstractInlineAnnotation_t, ElementType::AbstractWord_t, ElementType::Comment_t, ElementType::Description_t, ElementType::ForeignData_t, ElementType::LinkReference_t, ElementType::Metric_t, ElementType::Morpheme_t, ElementType::Phoneme_t, ElementType::Relation_t, ElementType::WordReference_t};
    Entity::PROPS.ANNOTATIONTYPE = AnnotationType::ENTITY;
    Entity::PROPS.LABEL = "Entity";
    Entity::PROPS.XMLTAG = "entity";
    element_props[ElementType::Entity_t] = &Entity::PROPS;
//------ Entry -------
    Entry::PROPS = ABSTRACT_STRUCTURE_ELEMENT_PROPERTIES;
    abstract_parents[ElementType::Entry_t] = ElementType::AbstractStructureElement_t;
    Entry::PROPS.ELEMENT_ID = ElementType::Entry_t;
    Entry::PROPS.ACCEPTED_DATA += {ElementType::AbstractAnnotationLayer_t, ElementType::AbstractFeature_t, ElementType::Alternative_t, ElementType::AlternativeLayers_t, ElementType::Comment_t, ElementType::Correction_t, ElementType::Definition_t, ElementType::Description_t, ElementType::Example_t, ElementType::External_t, ElementType::ForeignData_t, ElementType::Metric_t, ElementType::Part_t, ElementType::Relation_t, ElementType::String_t, ElementType::Term_t, ElementType::TextContent_t};
    Entry::PROPS.ANNOTATIONTYPE = AnnotationType::ENTRY;
    Entry::PROPS.LABEL = "Entry";
    Entry::PROPS.XMLTAG = "entry";
    element_props[ElementType::Entry_t] = &Entry::PROPS;
//------ ErrorDetection -------
    ErrorDetection::PROPS = ABSTRACT_INLINE_ANNOTATION_PROPERTIES;
    abstract_parents[ElementType::ErrorDetection_t] = ElementType::AbstractInlineAnnotation_t;
    ErrorDetection::PROPS.ELEMENT_ID = ElementType::ErrorDetection_t;
    ErrorDetection::PROPS.ANNOTATIONTYPE = AnnotationType::ERRORDETECTION;
    ErrorDetection::PROPS.LABEL = "Error Detection";
    ErrorDetection::PROPS.OCCURRENCES_PER_SET = 0;
    ErrorDetection::PROPS.XMLTAG = "errordetection";
    element_props[ElementType::ErrorDetection_t] = &ErrorDetection::PROPS;
//------ EtymologyAnnotation -------
    EtymologyAnnotation::PROPS = ABSTRACT_INLINE_ANNOTATION_PROPERTIES;
    abstract_parents[ElementType::EtymologyAnnotation_t] = ElementType::AbstractInlineAnnotation_t;
    EtymologyAnnotation::PROPS.ELEMENT_ID = ElementType::EtymologyAnnotation_t;
    EtymologyAnnotation::PROPS.ANNOTATIONTYPE = AnnotationType::ETYMOLOGY;
    EtymologyAnnotation::PROPS.LABEL = "Etymology";
    EtymologyAnnotation::PROPS.XMLTAG = "etymology";
    element_props[ElementType::EtymologyAnnotation_t] = &EtymologyAnnotation::PROPS;
//------ Event -------
    Event::PROPS = ABSTRACT_STRUCTURE_ELEMENT_PROPERTIES;
    abstract_parents[ElementType::Event_t] = ElementType::AbstractStructureElement_t;
    Event::PROPS.ELEMENT_ID = ElementType::Event_t;
    Event::PROPS.ACCEPTED_DATA += {ElementType::AbstractAnnotationLayer_t, ElementType::AbstractFeature_t, ElementType::AbstractInlineAnnotation_t, ElementType::AbstractWord_t, ElementType::ActorFeature_t, ElementType::Alternative_t, ElementType::AlternativeLayers_t, ElementType::BegindatetimeFeature_t, ElementType::Comment_t, ElementType::Correction_t, ElementType::Description_t, ElementType::Division_t, ElementType::EnddatetimeFeature_t, ElementType::Entry_t, ElementType::Event_t, ElementType::Example_t, ElementType::External_t, ElementType::Figure_t, ElementType::ForeignData_t, ElementType::Gap_t, ElementType::Head_t, ElementType::Linebreak_t, ElementType::List_t, ElementType::Metric_t, ElementType::Note_t, ElementType::Paragraph_t, ElementType::Part_t, ElementType::PhonContent_t, ElementType::Quote_t, ElementType::Reference_t, ElementType::Relation_t, ElementType::Sentence_t, ElementType::String_t, ElementType::Table_t, ElementType::TextContent_t, ElementType::Utterance_t, ElementType::Whitespace_t};
    Event::PROPS.ANNOTATIONTYPE = AnnotationType::EVENT;
    Event::PROPS.LABEL = "Event";
    Event::PROPS.XMLTAG = "event";
    element_props[ElementType::Event_t] = &Event::PROPS;
//------ Example -------
    Example::PROPS = ABSTRACT_STRUCTURE_ELEMENT_PROPERTIES;
    abstract_parents[ElementType::Example_t] = ElementType::AbstractStructureElement_t;
    Example::PROPS.ELEMENT_ID = ElementType::Example_t;
    Example::PROPS.ACCEPTED_DATA += {ElementType::AbstractAnnotationLayer_t, ElementType::AbstractFeature_t, ElementType::AbstractInlineAnnotation_t, ElementType::AbstractWord_t, ElementType::Alternative_t, ElementType::AlternativeLayers_t, ElementType::Comment_t, ElementType::Correction_t, ElementType::Description_t, ElementType::External_t, ElementType::Figure_t, ElementType::ForeignData_t, ElementType::Linebreak_t, ElementType::List_t, ElementType::Metric_t, ElementType::Paragraph_t, ElementType::Part_t, ElementType::PhonContent_t, ElementType::Reference_t, ElementType::Relation_t, ElementType::Sentence_t, ElementType::String_t, ElementType::Table_t, ElementType::TextContent_t, ElementType::Utterance_t, ElementType::Whitespace_t};
    Example::PROPS.ANNOTATIONTYPE = AnnotationType::EXAMPLE;
    Example::PROPS.LABEL = "Example";
    Example::PROPS.XMLTAG = "ex";
    element_props[ElementType::Example_t] = &Example::PROPS;
//------ External -------
    External::PROPS = ABSTRACT_HIGHER_ORDER_ANNOTATION_PROPERTIES;
    abstract_parents[ElementType::External_t] = ElementType::AbstractHigherOrderAnnotation_t;
    External::PROPS.ELEMENT_ID = ElementType::External_t;
    External::PROPS.ACCEPTED_DATA += {ElementType::Comment_t, ElementType::Description_t};
    External::PROPS.ANNOTATIONTYPE = AnnotationType::EXTERNAL;
    External::PROPS.LABEL = "External";
    External::PROPS.OPTIONAL_ATTRIBS = Attrib::ID|Attrib::ANNOTATOR|Attrib::CONFIDENCE|Attrib::DATETIME|Attrib::N|Attrib::METADATA|Attrib::BEGINTIME|Attrib::ENDTIME|Attrib::TAG;
    External::PROPS.PRINTABLE = true;
    External::PROPS.REQUIRED_ATTRIBS = Attrib::SRC;
    External::PROPS.SPEAKABLE = true;
    External::PROPS.XMLTAG = "external";
    element_props[ElementType::External_t] = &External::PROPS;
//------ Feature -------
    Feature::PROPS = ABSTRACT_FEATURE_PROPERTIES;
    abstract_parents[ElementType::Feature_t] = ElementType::AbstractFeature_t;
    Feature::PROPS.ELEMENT_ID = ElementType::Feature_t;
    Feature::PROPS.SUBSET = "feat";
    Feature::PROPS.XMLTAG = "feat";
    element_props[ElementType::Feature_t] = &Feature::PROPS;
//------ ActorFeature -------
    ActorFeature::PROPS = ABSTRACT_FEATURE_PROPERTIES;
    abstract_parents[ElementType::ActorFeature_t] = ElementType::AbstractFeature_t;
    ActorFeature::PROPS.ELEMENT_ID = ElementType::ActorFeature_t;
    ActorFeature::PROPS.SUBSET = "actor";
    ActorFeature::PROPS.XMLTAG = "actor";
    element_props[ElementType::ActorFeature_t] = &ActorFeature::PROPS;
//------ BegindatetimeFeature -------
    BegindatetimeFeature::PROPS = ABSTRACT_FEATURE_PROPERTIES;
    abstract_parents[ElementType::BegindatetimeFeature_t] = ElementType::AbstractFeature_t;
    BegindatetimeFeature::PROPS.ELEMENT_ID = ElementType::BegindatetimeFeature_t;
    BegindatetimeFeature::PROPS.SUBSET = "begindatetime";
    BegindatetimeFeature::PROPS.XMLTAG = "begindatetime";
    element_props[ElementType::BegindatetimeFeature_t] = &BegindatetimeFeature::PROPS;
//------ EnddatetimeFeature -------
    EnddatetimeFeature::PROPS = ABSTRACT_FEATURE_PROPERTIES;
    abstract_parents[ElementType::EnddatetimeFeature_t] = ElementType::AbstractFeature_t;
    EnddatetimeFeature::PROPS.ELEMENT_ID = ElementType::EnddatetimeFeature_t;
    EnddatetimeFeature::PROPS.SUBSET = "enddatetime";
    EnddatetimeFeature::PROPS.XMLTAG = "enddatetime";
    element_props[ElementType::EnddatetimeFeature_t] = &EnddatetimeFeature::PROPS;
//------ Figure -------
    Figure::PROPS = ABSTRACT_STRUCTURE_ELEMENT_PROPERTIES;
    abstract_parents[ElementType::Figure_t] = ElementType::AbstractStructureElement_t;
    Figure::PROPS.ELEMENT_ID = ElementType::Figure_t;
    Figure::PROPS.ACCEPTED_DATA += {ElementType::AbstractAnnotationLayer_t, ElementType::AbstractFeature_t, ElementType::Alternative_t, ElementType::AlternativeLayers_t, ElementType::Caption_t, ElementType::Comment_t, ElementType::Correction_t, ElementType::Description_t, ElementType::External_t, ElementType::ForeignData_t, ElementType::Linebreak_t, ElementType::Metric_t, ElementType::Part_t, ElementType::Relation_t, ElementType::String_t, ElementType::TextContent_t};
    Figure::PROPS.ANNOTATIONTYPE = AnnotationType::FIGURE;
    Figure::PROPS.LABEL = "Figure";
    Figure::PROPS.SPEAKABLE = false;
    Figure::PROPS.TEXTDELIMITER = "\n\n";
    Figure::PROPS.XMLTAG = "figure";
    element_props[ElementType::Figure_t] = &Figure::PROPS;
//------ FontFeature -------
    FontFeature::PROPS = ABSTRACT_FEATURE_PROPERTIES;
    abstract_parents[ElementType::FontFeature_t] = ElementType::AbstractFeature_t;
    FontFeature::PROPS.ELEMENT_ID = ElementType::FontFeature_t;
    FontFeature::PROPS.SUBSET = "font";
    FontFeature::PROPS.XMLTAG = "font";
    element_props[ElementType::FontFeature_t] = &FontFeature::PROPS;
//------ ForeignData -------
    ForeignData::PROPS = ABSTRACT_HIGHER_ORDER_ANNOTATION_PROPERTIES;
    abstract_parents[ElementType::ForeignData_t] = ElementType::AbstractHigherOrderAnnotation_t;
    ForeignData::PROPS.ELEMENT_ID = ElementType::ForeignData_t;
    ForeignData::PROPS.XMLTAG = "foreign-data";
    element_props[ElementType::ForeignData_t] = &ForeignData::PROPS;
//------ FunctionFeature -------
    FunctionFeature::PROPS = ABSTRACT_FEATURE_PROPERTIES;
    abstract_parents[ElementType::FunctionFeature_t] = ElementType::AbstractFeature_t;
    FunctionFeature::PROPS.ELEMENT_ID = ElementType::FunctionFeature_t;
    FunctionFeature::PROPS.SUBSET = "function";
    FunctionFeature::PROPS.XMLTAG = "function";
    element_props[ElementType::FunctionFeature_t] = &FunctionFeature::PROPS;
//------ Gap -------
    Gap::PROPS = ABSTRACT_HIGHER_ORDER_ANNOTATION_PROPERTIES;
    abstract_parents[ElementType::Gap_t] = ElementType::AbstractHigherOrderAnnotation_t;
    Gap::PROPS.ELEMENT_ID = ElementType::Gap_t;
    Gap::PROPS.ACCEPTED_DATA += {ElementType::AbstractFeature_t, ElementType::Comment_t, ElementType::Content_t, ElementType::Description_t, ElementType::ForeignData_t, ElementType::Metric_t, ElementType::Part_t};
    Gap::PROPS.ANNOTATIONTYPE = AnnotationType::GAP;
    Gap::PROPS.LABEL = "Gap";
    Gap::PROPS.OPTIONAL_ATTRIBS = Attrib::ID|Attrib::CLASS|Attrib::ANNOTATOR|Attrib::N|Attrib::DATETIME|Attrib::SRC|Attrib::BEGINTIME|Attrib::ENDTIME|Attrib::METADATA|Attrib::TAG;
    Gap::PROPS.XMLTAG = "gap";
    element_props[ElementType::Gap_t] = &Gap::PROPS;
//------ Head -------
    Head::PROPS = ABSTRACT_STRUCTURE_ELEMENT_PROPERTIES;
    abstract_parents[ElementType::Head_t] = ElementType::AbstractStructureElement_t;
    Head::PROPS.ELEMENT_ID = ElementType::Head_t;
    Head::PROPS.ACCEPTED_DATA += {ElementType::AbstractAnnotationLayer_t, ElementType::AbstractFeature_t, ElementType::AbstractInlineAnnotation_t, ElementType::AbstractWord_t, ElementType::Alternative_t, ElementType::AlternativeLayers_t, ElementType::Comment_t, ElementType::Correction_t, ElementType::Description_t, ElementType::Event_t, ElementType::External_t, ElementType::ForeignData_t, ElementType::Gap_t, ElementType::Linebreak_t, ElementType::Metric_t, ElementType::Paragraph_t, ElementType::Part_t, ElementType::PhonContent_t, ElementType::Reference_t, ElementType::Relation_t, ElementType::Sentence_t, ElementType::String_t, ElementType::TextContent_t, ElementType::Whitespace_t};
    Head::PROPS.ANNOTATIONTYPE = AnnotationType::HEAD;
    Head::PROPS.LABEL = "Head";
    Head::PROPS.TEXTDELIMITER = "\n\n";
    Head::PROPS.XMLTAG = "head";
    element_props[ElementType::Head_t] = &Head::PROPS;
//------ HeadFeature -------
    HeadFeature::PROPS = ABSTRACT_FEATURE_PROPERTIES;
    abstract_parents[ElementType::HeadFeature_t] = ElementType::AbstractFeature_t;
    HeadFeature::PROPS.ELEMENT_ID = ElementType::HeadFeature_t;
    HeadFeature::PROPS.SUBSET = "head";
    HeadFeature::PROPS.XMLTAG = "head";
    element_props[ElementType::HeadFeature_t] = &HeadFeature::PROPS;
//------ Headspan -------
    Headspan::PROPS = ABSTRACT_SPAN_ROLE_PROPERTIES;
    abstract_parents[ElementType::Headspan_t] = ElementType::AbstractSpanRole_t;
    Headspan::PROPS.ELEMENT_ID = ElementType::Headspan_t;
    Headspan::PROPS.LABEL = "Head";
    Headspan::PROPS.OCCURRENCES = 1;
    Headspan::PROPS.XMLTAG = "hd";
    element_props[ElementType::Headspan_t] = &Headspan::PROPS;
//------ Hiddenword -------
    Hiddenword::PROPS = ABSTRACT_WORD_PROPERTIES;
    abstract_parents[ElementType::Hiddenword_t] = ElementType::AbstractWord_t;
    Hiddenword::PROPS.ELEMENT_ID = ElementType::Hiddenword_t;
    Hiddenword::PROPS.ACCEPTED_DATA += {ElementType::AbstractAnnotationLayer_t, ElementType::AbstractFeature_t, ElementType::AbstractInlineAnnotation_t, ElementType::Alternative_t, ElementType::AlternativeLayers_t, ElementType::Comment_t, ElementType::Correction_t, ElementType::Description_t, ElementType::External_t, ElementType::ForeignData_t, ElementType::Metric_t, ElementType::Part_t, ElementType::PhonContent_t, ElementType::Reference_t, ElementType::Relation_t, ElementType::String_t, ElementType::TextContent_t};
    Hiddenword::PROPS.ANNOTATIONTYPE = AnnotationType::HIDDENTOKEN;
    Hiddenword::PROPS.HIDDEN = true;
    Hiddenword::PROPS.LABEL = "Hidden Word/Token";
    Hiddenword::PROPS.OPTIONAL_ATTRIBS = Attrib::ID|Attrib::CLASS|Attrib::ANNOTATOR|Attrib::N|Attrib::CONFIDENCE|Attrib::DATETIME|Attrib::SRC|Attrib::BEGINTIME|Attrib::ENDTIME|Attrib::SPEAKER|Attrib::TEXTCLASS|Attrib::METADATA|Attrib::SPACE|Attrib::TAG;
    Hiddenword::PROPS.TEXTDELIMITER = " ";
    Hiddenword::PROPS.WREFABLE = true;
    Hiddenword::PROPS.XMLTAG = "hiddenw";
    element_props[ElementType::Hiddenword_t] = &Hiddenword::PROPS;
//------ Hyphbreak -------
    Hyphbreak::PROPS = ABSTRACT_TEXT_MARKUP_PROPERTIES;
    abstract_parents[ElementType::Hyphbreak_t] = ElementType::AbstractTextMarkup_t;
    Hyphbreak::PROPS.ELEMENT_ID = ElementType::Hyphbreak_t;
    Hyphbreak::PROPS.ANNOTATIONTYPE = AnnotationType::HYPHENATION;
    Hyphbreak::PROPS.IMPLICITSPACE = true;
    Hyphbreak::PROPS.LABEL = "Hyphbreak";
    Hyphbreak::PROPS.TEXTDELIMITER = "";
    Hyphbreak::PROPS.XMLTAG = "t-hbr";
    element_props[ElementType::Hyphbreak_t] = &Hyphbreak::PROPS;
//------ Label -------
    Label::PROPS = ABSTRACT_STRUCTURE_ELEMENT_PROPERTIES;
    abstract_parents[ElementType::Label_t] = ElementType::AbstractStructureElement_t;
    Label::PROPS.ELEMENT_ID = ElementType::Label_t;
    Label::PROPS.ACCEPTED_DATA += {ElementType::AbstractAnnotationLayer_t, ElementType::AbstractFeature_t, ElementType::AbstractInlineAnnotation_t, ElementType::AbstractWord_t, ElementType::Alternative_t, ElementType::AlternativeLayers_t, ElementType::Comment_t, ElementType::Correction_t, ElementType::Description_t, ElementType::External_t, ElementType::ForeignData_t, ElementType::Linebreak_t, ElementType::Metric_t, ElementType::Part_t, ElementType::PhonContent_t, ElementType::Reference_t, ElementType::Relation_t, ElementType::String_t, ElementType::TextContent_t, ElementType::Whitespace_t};
    Label::PROPS.LABEL = "Label";
    Label::PROPS.XMLTAG = "label";
    element_props[ElementType::Label_t] = &Label::PROPS;
//------ LangAnnotation -------
    LangAnnotation::PROPS = ABSTRACT_INLINE_ANNOTATION_PROPERTIES;
    abstract_parents[ElementType::LangAnnotation_t] = ElementType::AbstractInlineAnnotation_t;
    LangAnnotation::PROPS.ELEMENT_ID = ElementType::LangAnnotation_t;
    LangAnnotation::PROPS.ANNOTATIONTYPE = AnnotationType::LANG;
    LangAnnotation::PROPS.LABEL = "Language";
    LangAnnotation::PROPS.XMLTAG = "lang";
    element_props[ElementType::LangAnnotation_t] = &LangAnnotation::PROPS;
//------ LemmaAnnotation -------
    LemmaAnnotation::PROPS = ABSTRACT_INLINE_ANNOTATION_PROPERTIES;
    abstract_parents[ElementType::LemmaAnnotation_t] = ElementType::AbstractInlineAnnotation_t;
    LemmaAnnotation::PROPS.ELEMENT_ID = ElementType::LemmaAnnotation_t;
    LemmaAnnotation::PROPS.ANNOTATIONTYPE = AnnotationType::LEMMA;
    LemmaAnnotation::PROPS.LABEL = "Lemma";
    LemmaAnnotation::PROPS.XMLTAG = "lemma";
    element_props[ElementType::LemmaAnnotation_t] = &LemmaAnnotation::PROPS;
//------ LevelFeature -------
    LevelFeature::PROPS = ABSTRACT_FEATURE_PROPERTIES;
    abstract_parents[ElementType::LevelFeature_t] = ElementType::AbstractFeature_t;
    LevelFeature::PROPS.ELEMENT_ID = ElementType::LevelFeature_t;
    LevelFeature::PROPS.SUBSET = "level";
    LevelFeature::PROPS.XMLTAG = "level";
    element_props[ElementType::LevelFeature_t] = &LevelFeature::PROPS;
//------ Linebreak -------
    Linebreak::PROPS = ABSTRACT_STRUCTURE_ELEMENT_PROPERTIES;
    abstract_parents[ElementType::Linebreak_t] = ElementType::AbstractStructureElement_t;
    Linebreak::PROPS.ELEMENT_ID = ElementType::Linebreak_t;
    Linebreak::PROPS.ANNOTATIONTYPE = AnnotationType::LINEBREAK;
    Linebreak::PROPS.IMPLICITSPACE = true;
    Linebreak::PROPS.LABEL = "Linebreak";
    Linebreak::PROPS.TEXTDELIMITER = "";
    Linebreak::PROPS.XLINK = true;
    Linebreak::PROPS.XMLTAG = "br";
    element_props[ElementType::Linebreak_t] = &Linebreak::PROPS;
//------ LinkReference -------
    LinkReference::PROPS.ELEMENT_ID = ElementType::LinkReference_t;
    LinkReference::PROPS.OPTIONAL_ATTRIBS = Attrib::IDREF|Attrib::TAG;
    LinkReference::PROPS.XMLTAG = "xref";
    element_props[ElementType::LinkReference_t] = &LinkReference::PROPS;
//------ List -------
    List::PROPS = ABSTRACT_STRUCTURE_ELEMENT_PROPERTIES;
    abstract_parents[ElementType::List_t] = ElementType::AbstractStructureElement_t;
    List::PROPS.ELEMENT_ID = ElementType::List_t;
    List::PROPS.ACCEPTED_DATA += {ElementType::AbstractAnnotationLayer_t, ElementType::AbstractFeature_t, ElementType::AbstractInlineAnnotation_t, ElementType::Alternative_t, ElementType::AlternativeLayers_t, ElementType::Caption_t, ElementType::Comment_t, ElementType::Correction_t, ElementType::Description_t, ElementType::Event_t, ElementType::External_t, ElementType::ForeignData_t, ElementType::Linebreak_t, ElementType::ListItem_t, ElementType::Metric_t, ElementType::Note_t, ElementType::Part_t, ElementType::PhonContent_t, ElementType::Reference_t, ElementType::Relation_t, ElementType::String_t, ElementType::TextContent_t};
    List::PROPS.ANNOTATIONTYPE = AnnotationType::LIST;
    List::PROPS.LABEL = "List";
    List::PROPS.TEXTDELIMITER = "\n\n";
    List::PROPS.XMLTAG = "list";
    element_props[ElementType::List_t] = &List::PROPS;
//------ ListItem -------
    ListItem::PROPS = ABSTRACT_STRUCTURE_ELEMENT_PROPERTIES;
    abstract_parents[ElementType::ListItem_t] = ElementType::AbstractStructureElement_t;
    ListItem::PROPS.ELEMENT_ID = ElementType::ListItem_t;
    ListItem::PROPS.ACCEPTED_DATA += {ElementType::AbstractAnnotationLayer_t, ElementType::AbstractFeature_t, ElementType::AbstractInlineAnnotation_t, ElementType::AbstractWord_t, ElementType::Alternative_t, ElementType::AlternativeLayers_t, ElementType::Comment_t, ElementType::Correction_t, ElementType::Description_t, ElementType::Event_t, ElementType::External_t, ElementType::ForeignData_t, ElementType::Gap_t, ElementType::Label_t, ElementType::Linebreak_t, ElementType::List_t, ElementType::Metric_t, ElementType::Note_t, ElementType::Paragraph_t, ElementType::Part_t, ElementType::PhonContent_t, ElementType::Quote_t, ElementType::Reference_t, ElementType::Relation_t, ElementType::Sentence_t, ElementType::String_t, ElementType::TextContent_t, ElementType::Whitespace_t};
    ListItem::PROPS.LABEL = "List Item";
    ListItem::PROPS.OPTIONAL_ATTRIBS = Attrib::ID|Attrib::ANNOTATOR|Attrib::N|Attrib::CONFIDENCE|Attrib::DATETIME|Attrib::SRC|Attrib::BEGINTIME|Attrib::ENDTIME|Attrib::SPEAKER|Attrib::METADATA|Attrib::TAG;
    ListItem::PROPS.TEXTDELIMITER = "\n";
    ListItem::PROPS.XMLTAG = "item";
    element_props[ElementType::ListItem_t] = &ListItem::PROPS;
//------ Metric -------
    Metric::PROPS = ABSTRACT_HIGHER_ORDER_ANNOTATION_PROPERTIES;
    abstract_parents[ElementType::Metric_t] = ElementType::AbstractHigherOrderAnnotation_t;
    Metric::PROPS.ELEMENT_ID = ElementType::Metric_t;
    Metric::PROPS.ACCEPTED_DATA += {ElementType::AbstractFeature_t, ElementType::Comment_t, ElementType::Description_t, ElementType::ForeignData_t, ElementType::ValueFeature_t};
    Metric::PROPS.ANNOTATIONTYPE = AnnotationType::METRIC;
    Metric::PROPS.LABEL = "Metric";
    Metric::PROPS.OPTIONAL_ATTRIBS = Attrib::ID|Attrib::CLASS|Attrib::ANNOTATOR|Attrib::N|Attrib::CONFIDENCE|Attrib::DATETIME|Attrib::SRC|Attrib::BEGINTIME|Attrib::ENDTIME|Attrib::SPEAKER|Attrib::METADATA|Attrib::TAG;
    Metric::PROPS.XMLTAG = "metric";
    element_props[ElementType::Metric_t] = &Metric::PROPS;
//------ ModalitiesLayer -------
    ModalitiesLayer::PROPS = ABSTRACT_ANNOTATION_LAYER_PROPERTIES;
    abstract_parents[ElementType::ModalitiesLayer_t] = ElementType::AbstractAnnotationLayer_t;
    ModalitiesLayer::PROPS.ELEMENT_ID = ElementType::ModalitiesLayer_t;
    ModalitiesLayer::PROPS.ACCEPTED_DATA += {ElementType::Comment_t, ElementType::Correction_t, ElementType::Description_t, ElementType::ForeignData_t, ElementType::Modality_t};
    ModalitiesLayer::PROPS.ANNOTATIONTYPE = AnnotationType::MODALITY;
    ModalitiesLayer::PROPS.XMLTAG = "modalities";
    element_props[ElementType::ModalitiesLayer_t] = &ModalitiesLayer::PROPS;
//------ Modality -------
    Modality::PROPS = ABSTRACT_SPAN_ANNOTATION_PROPERTIES;
    abstract_parents[ElementType::Modality_t] = ElementType::AbstractSpanAnnotation_t;
    Modality::PROPS.ELEMENT_ID = ElementType::Modality_t;
    Modality::PROPS.ACCEPTED_DATA += {ElementType::AbstractFeature_t, ElementType::AbstractInlineAnnotation_t, ElementType::Comment_t, ElementType::Cue_t, ElementType::Description_t, ElementType::ForeignData_t, ElementType::LinkReference_t, ElementType::Metric_t, ElementType::PolarityFeature_t, ElementType::Relation_t, ElementType::Scope_t, ElementType::Source_t, ElementType::StrengthFeature_t, ElementType::Target_t};
    Modality::PROPS.ANNOTATIONTYPE = AnnotationType::MODALITY;
    Modality::PROPS.LABEL = "Modality";
    Modality::PROPS.XMLTAG = "modality";
    element_props[ElementType::Modality_t] = &Modality::PROPS;
//------ ModalityFeature -------
    ModalityFeature::PROPS = ABSTRACT_FEATURE_PROPERTIES;
    abstract_parents[ElementType::ModalityFeature_t] = ElementType::AbstractFeature_t;
    ModalityFeature::PROPS.ELEMENT_ID = ElementType::ModalityFeature_t;
    ModalityFeature::PROPS.SUBSET = "mod";
    ModalityFeature::PROPS.XMLTAG = "mod";
    element_props[ElementType::ModalityFeature_t] = &ModalityFeature::PROPS;
//------ Morpheme -------
    Morpheme::PROPS = ABSTRACT_SUBTOKEN_ANNOTATION_PROPERTIES;
    abstract_parents[ElementType::Morpheme_t] = ElementType::AbstractSubtokenAnnotation_t;
    Morpheme::PROPS.ELEMENT_ID = ElementType::Morpheme_t;
    Morpheme::PROPS.ACCEPTED_DATA += {ElementType::AbstractAnnotationLayer_t, ElementType::AbstractFeature_t, ElementType::AbstractInlineAnnotation_t, ElementType::Alternative_t, ElementType::AlternativeLayers_t, ElementType::Comment_t, ElementType::Correction_t, ElementType::Description_t, ElementType::ForeignData_t, ElementType::FunctionFeature_t, ElementType::Metric_t, ElementType::Morpheme_t, ElementType::Part_t, ElementType::PhonContent_t, ElementType::Relation_t, ElementType::String_t, ElementType::TextContent_t};
    Morpheme::PROPS.ANNOTATIONTYPE = AnnotationType::MORPHOLOGICAL;
    Morpheme::PROPS.LABEL = "Morpheme";
    Morpheme::PROPS.TEXTDELIMITER = "";
    Morpheme::PROPS.WREFABLE = true;
    Morpheme::PROPS.XMLTAG = "morpheme";
    element_props[ElementType::Morpheme_t] = &Morpheme::PROPS;
//------ MorphologyLayer -------
    MorphologyLayer::PROPS = ABSTRACT_ANNOTATION_LAYER_PROPERTIES;
    abstract_parents[ElementType::MorphologyLayer_t] = ElementType::AbstractAnnotationLayer_t;
    MorphologyLayer::PROPS.ELEMENT_ID = ElementType::MorphologyLayer_t;
    MorphologyLayer::PROPS.ACCEPTED_DATA += {ElementType::Comment_t, ElementType::Correction_t, ElementType::Description_t, ElementType::ForeignData_t, ElementType::Morpheme_t};
    MorphologyLayer::PROPS.ANNOTATIONTYPE = AnnotationType::MORPHOLOGICAL;
    MorphologyLayer::PROPS.XMLTAG = "morphology";
    element_props[ElementType::MorphologyLayer_t] = &MorphologyLayer::PROPS;
//------ New -------
    New::PROPS = ABSTRACT_CORRECTION_CHILD_PROPERTIES;
    abstract_parents[ElementType::New_t] = ElementType::AbstractCorrectionChild_t;
    New::PROPS.ELEMENT_ID = ElementType::New_t;
    New::PROPS.ANNOTATIONTYPE = AnnotationType::CORRECTION;
    New::PROPS.OCCURRENCES = 1;
    New::PROPS.OPTIONAL_ATTRIBS = Attrib::TAG;
    New::PROPS.XMLTAG = "new";
    element_props[ElementType::New_t] = &New::PROPS;
//------ Note -------
    Note::PROPS = ABSTRACT_STRUCTURE_ELEMENT_PROPERTIES;
    abstract_parents[ElementType::Note_t] = ElementType::AbstractStructureElement_t;
    Note::PROPS.ELEMENT_ID = ElementType::Note_t;
    Note::PROPS.ACCEPTED_DATA += {ElementType::AbstractAnnotationLayer_t, ElementType::AbstractFeature_t, ElementType::AbstractInlineAnnotation_t, ElementType::AbstractWord_t, ElementType::Alternative_t, ElementType::AlternativeLayers_t, ElementType::Comment_t, ElementType::Correction_t, ElementType::Description_t, ElementType::Example_t, ElementType::External_t, ElementType::Figure_t, ElementType::ForeignData_t, ElementType::Head_t, ElementType::Linebreak_t, ElementType::List_t, ElementType::Metric_t, ElementType::Paragraph_t, ElementType::Part_t, ElementType::PhonContent_t, ElementType::Reference_t, ElementType::Relation_t, ElementType::Sentence_t, ElementType::String_t, ElementType::Table_t, ElementType::TextContent_t, ElementType::Utterance_t, ElementType::Whitespace_t};
    Note::PROPS.ANNOTATIONTYPE = AnnotationType::NOTE;
    Note::PROPS.LABEL = "Note";
    Note::PROPS.XMLTAG = "note";
    element_props[ElementType::Note_t] = &Note::PROPS;
//------ Observation -------
    Observation::PROPS = ABSTRACT_SPAN_ANNOTATION_PROPERTIES;
    abstract_parents[ElementType::Observation_t] = ElementType::AbstractSpanAnnotation_t;
    Observation::PROPS.ELEMENT_ID = ElementType::Observation_t;
    Observation::PROPS.ACCEPTED_DATA += {ElementType::AbstractFeature_t, ElementType::AbstractInlineAnnotation_t, ElementType::AbstractWord_t, ElementType::Comment_t, ElementType::Description_t, ElementType::ForeignData_t, ElementType::LinkReference_t, ElementType::Metric_t, ElementType::Morpheme_t, ElementType::Phoneme_t, ElementType::Relation_t, ElementType::WordReference_t};
    Observation::PROPS.ANNOTATIONTYPE = AnnotationType::OBSERVATION;
    Observation::PROPS.LABEL = "Observation";
    Observation::PROPS.XMLTAG = "observation";
    element_props[ElementType::Observation_t] = &Observation::PROPS;
//------ ObservationLayer -------
    ObservationLayer::PROPS = ABSTRACT_ANNOTATION_LAYER_PROPERTIES;
    abstract_parents[ElementType::ObservationLayer_t] = ElementType::AbstractAnnotationLayer_t;
    ObservationLayer::PROPS.ELEMENT_ID = ElementType::ObservationLayer_t;
    ObservationLayer::PROPS.ACCEPTED_DATA += {ElementType::Comment_t, ElementType::Correction_t, ElementType::Description_t, ElementType::ForeignData_t, ElementType::Observation_t};
    ObservationLayer::PROPS.ANNOTATIONTYPE = AnnotationType::OBSERVATION;
    ObservationLayer::PROPS.XMLTAG = "observations";
    element_props[ElementType::ObservationLayer_t] = &ObservationLayer::PROPS;
//------ Original -------
    Original::PROPS = ABSTRACT_CORRECTION_CHILD_PROPERTIES;
    abstract_parents[ElementType::Original_t] = ElementType::AbstractCorrectionChild_t;
    Original::PROPS.ELEMENT_ID = ElementType::Original_t;
    Original::PROPS.ANNOTATIONTYPE = AnnotationType::CORRECTION;
    Original::PROPS.AUTH = false;
    Original::PROPS.OCCURRENCES = 1;
    Original::PROPS.OPTIONAL_ATTRIBS = Attrib::TAG;
    Original::PROPS.XMLTAG = "original";
    element_props[ElementType::Original_t] = &Original::PROPS;
//------ Paragraph -------
    Paragraph::PROPS = ABSTRACT_STRUCTURE_ELEMENT_PROPERTIES;
    abstract_parents[ElementType::Paragraph_t] = ElementType::AbstractStructureElement_t;
    Paragraph::PROPS.ELEMENT_ID = ElementType::Paragraph_t;
    Paragraph::PROPS.ACCEPTED_DATA += {ElementType::AbstractAnnotationLayer_t, ElementType::AbstractFeature_t, ElementType::AbstractInlineAnnotation_t, ElementType::AbstractWord_t, ElementType::Alternative_t, ElementType::AlternativeLayers_t, ElementType::Comment_t, ElementType::Correction_t, ElementType::Description_t, ElementType::Entry_t, ElementType::Event_t, ElementType::Example_t, ElementType::External_t, ElementType::Figure_t, ElementType::ForeignData_t, ElementType::Gap_t, ElementType::Head_t, ElementType::Linebreak_t, ElementType::List_t, ElementType::Metric_t, ElementType::Note_t, ElementType::Part_t, ElementType::PhonContent_t, ElementType::Quote_t, ElementType::Reference_t, ElementType::Relation_t, ElementType::Sentence_t, ElementType::String_t, ElementType::TextContent_t, ElementType::Whitespace_t};
    Paragraph::PROPS.ANNOTATIONTYPE = AnnotationType::PARAGRAPH;
    Paragraph::PROPS.LABEL = "Paragraph";
    Paragraph::PROPS.TEXTDELIMITER = "\n\n";
    Paragraph::PROPS.XMLTAG = "p";
    element_props[ElementType::Paragraph_t] = &Paragraph::PROPS;
//------ Part -------
    Part::PROPS = ABSTRACT_STRUCTURE_ELEMENT_PROPERTIES;
    abstract_parents[ElementType::Part_t] = ElementType::AbstractStructureElement_t;
    Part::PROPS.ELEMENT_ID = ElementType::Part_t;
    Part::PROPS.ACCEPTED_DATA += {ElementType::AbstractAnnotationLayer_t, ElementType::AbstractFeature_t, ElementType::AbstractInlineAnnotation_t, ElementType::AbstractStructureElement_t, ElementType::Alternative_t, ElementType::AlternativeLayers_t, ElementType::Comment_t, ElementType::Correction_t, ElementType::Description_t, ElementType::External_t, ElementType::ForeignData_t, ElementType::Metric_t, ElementType::Part_t, ElementType::PhonContent_t, ElementType::Relation_t, ElementType::TextContent_t};
    Part::PROPS.ANNOTATIONTYPE = AnnotationType::PART;
    Part::PROPS.LABEL = "Part";
    Part::PROPS.TEXTDELIMITER = " ";
    Part::PROPS.XMLTAG = "part";
    element_props[ElementType::Part_t] = &Part::PROPS;
//------ PhonContent -------
    PhonContent::PROPS = ABSTRACT_CONTENT_ANNOTATION_PROPERTIES;
    abstract_parents[ElementType::PhonContent_t] = ElementType::AbstractContentAnnotation_t;
    PhonContent::PROPS.ELEMENT_ID = ElementType::PhonContent_t;
    PhonContent::PROPS.ACCEPTED_DATA += {ElementType::AbstractFeature_t, ElementType::Comment_t, ElementType::Description_t, ElementType::XmlText_t};
    PhonContent::PROPS.ANNOTATIONTYPE = AnnotationType::PHON;
    PhonContent::PROPS.LABEL = "Phonetic Content";
    PhonContent::PROPS.PHONCONTAINER = true;
    PhonContent::PROPS.PRINTABLE = false;
    PhonContent::PROPS.SPEAKABLE = true;
    PhonContent::PROPS.XMLTAG = "ph";
    element_props[ElementType::PhonContent_t] = &PhonContent::PROPS;
//------ Phoneme -------
    Phoneme::PROPS = ABSTRACT_SUBTOKEN_ANNOTATION_PROPERTIES;
    abstract_parents[ElementType::Phoneme_t] = ElementType::AbstractSubtokenAnnotation_t;
    Phoneme::PROPS.ELEMENT_ID = ElementType::Phoneme_t;
    Phoneme::PROPS.ACCEPTED_DATA += {ElementType::AbstractAnnotationLayer_t, ElementType::AbstractFeature_t, ElementType::AbstractInlineAnnotation_t, ElementType::Alternative_t, ElementType::AlternativeLayers_t, ElementType::Comment_t, ElementType::Correction_t, ElementType::Description_t, ElementType::ForeignData_t, ElementType::FunctionFeature_t, ElementType::Metric_t, ElementType::Part_t, ElementType::PhonContent_t, ElementType::Phoneme_t, ElementType::Relation_t, ElementType::String_t, ElementType::TextContent_t};
    Phoneme::PROPS.ANNOTATIONTYPE = AnnotationType::PHONOLOGICAL;
    Phoneme::PROPS.LABEL = "Phoneme";
    Phoneme::PROPS.TEXTDELIMITER = "";
    Phoneme::PROPS.WREFABLE = true;
    Phoneme::PROPS.XMLTAG = "phoneme";
    element_props[ElementType::Phoneme_t] = &Phoneme::PROPS;
//------ PhonologyLayer -------
    PhonologyLayer::PROPS = ABSTRACT_ANNOTATION_LAYER_PROPERTIES;
    abstract_parents[ElementType::PhonologyLayer_t] = ElementType::AbstractAnnotationLayer_t;
    PhonologyLayer::PROPS.ELEMENT_ID = ElementType::PhonologyLayer_t;
    PhonologyLayer::PROPS.ACCEPTED_DATA += {ElementType::Comment_t, ElementType::Correction_t, ElementType::Description_t, ElementType::ForeignData_t, ElementType::Phoneme_t};
    PhonologyLayer::PROPS.ANNOTATIONTYPE = AnnotationType::PHONOLOGICAL;
    PhonologyLayer::PROPS.XMLTAG = "phonology";
    element_props[ElementType::PhonologyLayer_t] = &PhonologyLayer::PROPS;
//------ PolarityFeature -------
    PolarityFeature::PROPS = ABSTRACT_FEATURE_PROPERTIES;
    abstract_parents[ElementType::PolarityFeature_t] = ElementType::AbstractFeature_t;
    PolarityFeature::PROPS.ELEMENT_ID = ElementType::PolarityFeature_t;
    PolarityFeature::PROPS.SUBSET = "polarity";
    PolarityFeature::PROPS.XMLTAG = "polarity";
    element_props[ElementType::PolarityFeature_t] = &PolarityFeature::PROPS;
//------ PosAnnotation -------
    PosAnnotation::PROPS = ABSTRACT_INLINE_ANNOTATION_PROPERTIES;
    abstract_parents[ElementType::PosAnnotation_t] = ElementType::AbstractInlineAnnotation_t;
    PosAnnotation::PROPS.ELEMENT_ID = ElementType::PosAnnotation_t;
    PosAnnotation::PROPS.ACCEPTED_DATA += {ElementType::AbstractFeature_t, ElementType::Comment_t, ElementType::Description_t, ElementType::ForeignData_t, ElementType::HeadFeature_t, ElementType::Metric_t};
    PosAnnotation::PROPS.ANNOTATIONTYPE = AnnotationType::POS;
    PosAnnotation::PROPS.LABEL = "Part-of-Speech";
    PosAnnotation::PROPS.XMLTAG = "pos";
    element_props[ElementType::PosAnnotation_t] = &PosAnnotation::PROPS;
//------ Predicate -------
    Predicate::PROPS = ABSTRACT_SPAN_ANNOTATION_PROPERTIES;
    abstract_parents[ElementType::Predicate_t] = ElementType::AbstractSpanAnnotation_t;
    Predicate::PROPS.ELEMENT_ID = ElementType::Predicate_t;
    Predicate::PROPS.ACCEPTED_DATA += {ElementType::AbstractFeature_t, ElementType::AbstractInlineAnnotation_t, ElementType::AbstractWord_t, ElementType::Comment_t, ElementType::Description_t, ElementType::ForeignData_t, ElementType::LinkReference_t, ElementType::Metric_t, ElementType::Morpheme_t, ElementType::Phoneme_t, ElementType::Relation_t, ElementType::SemanticRole_t, ElementType::WordReference_t};
    Predicate::PROPS.ANNOTATIONTYPE = AnnotationType::PREDICATE;
    Predicate::PROPS.LABEL = "Predicate";
    Predicate::PROPS.XMLTAG = "predicate";
    element_props[ElementType::Predicate_t] = &Predicate::PROPS;
//------ Quote -------
    Quote::PROPS = ABSTRACT_STRUCTURE_ELEMENT_PROPERTIES;
    abstract_parents[ElementType::Quote_t] = ElementType::AbstractStructureElement_t;
    Quote::PROPS.ELEMENT_ID = ElementType::Quote_t;
    Quote::PROPS.ACCEPTED_DATA += {ElementType::AbstractAnnotationLayer_t, ElementType::AbstractFeature_t, ElementType::AbstractInlineAnnotation_t, ElementType::AbstractWord_t, ElementType::Alternative_t, ElementType::AlternativeLayers_t, ElementType::Comment_t, ElementType::Correction_t, ElementType::Description_t, ElementType::Division_t, ElementType::External_t, ElementType::ForeignData_t, ElementType::Gap_t, ElementType::Linebreak_t, ElementType::Metric_t, ElementType::Paragraph_t, ElementType::Part_t, ElementType::Quote_t, ElementType::Reference_t, ElementType::Relation_t, ElementType::Sentence_t, ElementType::String_t, ElementType::TextContent_t, ElementType::Utterance_t, ElementType::Whitespace_t};
    Quote::PROPS.ANNOTATIONTYPE = AnnotationType::QUOTE;
    Quote::PROPS.LABEL = "Quote";
    Quote::PROPS.XMLTAG = "quote";
    element_props[ElementType::Quote_t] = &Quote::PROPS;
//------ Reference -------
    Reference::PROPS = ABSTRACT_STRUCTURE_ELEMENT_PROPERTIES;
    abstract_parents[ElementType::Reference_t] = ElementType::AbstractStructureElement_t;
    Reference::PROPS.ELEMENT_ID = ElementType::Reference_t;
    Reference::PROPS.ACCEPTED_DATA += {ElementType::AbstractAnnotationLayer_t, ElementType::AbstractFeature_t, ElementType::AbstractWord_t, ElementType::Alternative_t, ElementType::AlternativeLayers_t, ElementType::Comment_t, ElementType::Correction_t, ElementType::Description_t, ElementType::External_t, ElementType::ForeignData_t, ElementType::Linebreak_t, ElementType::Metric_t, ElementType::Paragraph_t, ElementType::Part_t, ElementType::PhonContent_t, ElementType::Quote_t, ElementType::Relation_t, ElementType::Sentence_t, ElementType::String_t, ElementType::TextContent_t, ElementType::Utterance_t, ElementType::Whitespace_t};
    Reference::PROPS.ANNOTATIONTYPE = AnnotationType::REFERENCE;
    Reference::PROPS.LABEL = "Reference";
    Reference::PROPS.TEXTDELIMITER = " ";
    Reference::PROPS.XLINK = true;
    Reference::PROPS.XMLTAG = "ref";
    element_props[ElementType::Reference_t] = &Reference::PROPS;
//------ Relation -------
    Relation::PROPS = ABSTRACT_HIGHER_ORDER_ANNOTATION_PROPERTIES;
    abstract_parents[ElementType::Relation_t] = ElementType::AbstractHigherOrderAnnotation_t;
    Relation::PROPS.ELEMENT_ID = ElementType::Relation_t;
    Relation::PROPS.ACCEPTED_DATA += {ElementType::AbstractFeature_t, ElementType::Comment_t, ElementType::Description_t, ElementType::ForeignData_t, ElementType::LinkReference_t, ElementType::Metric_t};
    Relation::PROPS.ANNOTATIONTYPE = AnnotationType::RELATION;
    Relation::PROPS.LABEL = "Relation";
    Relation::PROPS.OPTIONAL_ATTRIBS = Attrib::ID|Attrib::CLASS|Attrib::ANNOTATOR|Attrib::N|Attrib::CONFIDENCE|Attrib::DATETIME|Attrib::SRC|Attrib::BEGINTIME|Attrib::ENDTIME|Attrib::SPEAKER|Attrib::METADATA|Attrib::TAG;
    Relation::PROPS.PRINTABLE = false;
    Relation::PROPS.REQUIRED_ATTRIBS = Attrib::NO_ATT;
    Relation::PROPS.SPEAKABLE = false;
    Relation::PROPS.XLINK = true;
    Relation::PROPS.XMLTAG = "relation";
    element_props[ElementType::Relation_t] = &Relation::PROPS;
//------ Row -------
    Row::PROPS = ABSTRACT_STRUCTURE_ELEMENT_PROPERTIES;
    abstract_parents[ElementType::Row_t] = ElementType::AbstractStructureElement_t;
    Row::PROPS.ELEMENT_ID = ElementType::Row_t;
    Row::PROPS.ACCEPTED_DATA += {ElementType::AbstractAnnotationLayer_t, ElementType::AbstractFeature_t, ElementType::AbstractInlineAnnotation_t, ElementType::Alternative_t, ElementType::AlternativeLayers_t, ElementType::Cell_t, ElementType::Comment_t, ElementType::Correction_t, ElementType::Description_t, ElementType::External_t, ElementType::ForeignData_t, ElementType::Metric_t, ElementType::Part_t, ElementType::Relation_t};
    Row::PROPS.LABEL = "Table Row";
    Row::PROPS.TEXTDELIMITER = "\n";
    Row::PROPS.XMLTAG = "row";
    element_props[ElementType::Row_t] = &Row::PROPS;
//------ Scope -------
    Scope::PROPS = ABSTRACT_SPAN_ROLE_PROPERTIES;
    abstract_parents[ElementType::Scope_t] = ElementType::AbstractSpanRole_t;
    Scope::PROPS.ELEMENT_ID = ElementType::Scope_t;
    Scope::PROPS.ACCEPTED_DATA += {ElementType::AbstractFeature_t, ElementType::AbstractInlineAnnotation_t, ElementType::AbstractWord_t, ElementType::Comment_t, ElementType::Cue_t, ElementType::Description_t, ElementType::ForeignData_t, ElementType::LinkReference_t, ElementType::Metric_t, ElementType::Morpheme_t, ElementType::Phoneme_t, ElementType::Relation_t, ElementType::Source_t, ElementType::Target_t, ElementType::WordReference_t};
    Scope::PROPS.LABEL = "Scope";
    Scope::PROPS.OCCURRENCES = 1;
    Scope::PROPS.XMLTAG = "scope";
    element_props[ElementType::Scope_t] = &Scope::PROPS;
//------ SemanticRole -------
    SemanticRole::PROPS = ABSTRACT_SPAN_ANNOTATION_PROPERTIES;
    abstract_parents[ElementType::SemanticRole_t] = ElementType::AbstractSpanAnnotation_t;
    SemanticRole::PROPS.ELEMENT_ID = ElementType::SemanticRole_t;
    SemanticRole::PROPS.ACCEPTED_DATA += {ElementType::AbstractFeature_t, ElementType::AbstractInlineAnnotation_t, ElementType::AbstractWord_t, ElementType::Comment_t, ElementType::Description_t, ElementType::ForeignData_t, ElementType::Headspan_t, ElementType::LinkReference_t, ElementType::Metric_t, ElementType::Morpheme_t, ElementType::Phoneme_t, ElementType::Relation_t, ElementType::WordReference_t};
    SemanticRole::PROPS.ANNOTATIONTYPE = AnnotationType::SEMROLE;
    SemanticRole::PROPS.LABEL = "Semantic Role";
    SemanticRole::PROPS.REQUIRED_ATTRIBS = Attrib::CLASS;
    SemanticRole::PROPS.XMLTAG = "semrole";
    element_props[ElementType::SemanticRole_t] = &SemanticRole::PROPS;
//------ SemanticRolesLayer -------
    SemanticRolesLayer::PROPS = ABSTRACT_ANNOTATION_LAYER_PROPERTIES;
    abstract_parents[ElementType::SemanticRolesLayer_t] = ElementType::AbstractAnnotationLayer_t;
    SemanticRolesLayer::PROPS.ELEMENT_ID = ElementType::SemanticRolesLayer_t;
    SemanticRolesLayer::PROPS.ACCEPTED_DATA += {ElementType::Comment_t, ElementType::Correction_t, ElementType::Description_t, ElementType::ForeignData_t, ElementType::Predicate_t, ElementType::SemanticRole_t};
    SemanticRolesLayer::PROPS.ANNOTATIONTYPE = AnnotationType::SEMROLE;
    SemanticRolesLayer::PROPS.XMLTAG = "semroles";
    element_props[ElementType::SemanticRolesLayer_t] = &SemanticRolesLayer::PROPS;
//------ SenseAnnotation -------
    SenseAnnotation::PROPS = ABSTRACT_INLINE_ANNOTATION_PROPERTIES;
    abstract_parents[ElementType::SenseAnnotation_t] = ElementType::AbstractInlineAnnotation_t;
    SenseAnnotation::PROPS.ELEMENT_ID = ElementType::SenseAnnotation_t;
    SenseAnnotation::PROPS.ACCEPTED_DATA += {ElementType::AbstractFeature_t, ElementType::Comment_t, ElementType::Description_t, ElementType::ForeignData_t, ElementType::Metric_t, ElementType::SynsetFeature_t};
    SenseAnnotation::PROPS.ANNOTATIONTYPE = AnnotationType::SENSE;
    SenseAnnotation::PROPS.LABEL = "Semantic Sense";
    SenseAnnotation::PROPS.OCCURRENCES_PER_SET = 0;
    SenseAnnotation::PROPS.XMLTAG = "sense";
    element_props[ElementType::SenseAnnotation_t] = &SenseAnnotation::PROPS;
//------ Sentence -------
    Sentence::PROPS = ABSTRACT_STRUCTURE_ELEMENT_PROPERTIES;
    abstract_parents[ElementType::Sentence_t] = ElementType::AbstractStructureElement_t;
    Sentence::PROPS.ELEMENT_ID = ElementType::Sentence_t;
    Sentence::PROPS.ACCEPTED_DATA += {ElementType::AbstractAnnotationLayer_t, ElementType::AbstractFeature_t, ElementType::AbstractInlineAnnotation_t, ElementType::AbstractWord_t, ElementType::Alternative_t, ElementType::AlternativeLayers_t, ElementType::Comment_t, ElementType::Correction_t, ElementType::Description_t, ElementType::Entry_t, ElementType::Event_t, ElementType::Example_t, ElementType::External_t, ElementType::ForeignData_t, ElementType::Gap_t, ElementType::Linebreak_t, ElementType::Metric_t, ElementType::Note_t, ElementType::Part_t, ElementType::PhonContent_t, ElementType::Quote_t, ElementType::Reference_t, ElementType::Relation_t, ElementType::String_t, ElementType::TextContent_t, ElementType::Whitespace_t};
    Sentence::PROPS.ANNOTATIONTYPE = AnnotationType::SENTENCE;
    Sentence::PROPS.LABEL = "Sentence";
    Sentence::PROPS.TEXTDELIMITER = " ";
    Sentence::PROPS.XMLTAG = "s";
    element_props[ElementType::Sentence_t] = &Sentence::PROPS;
//------ Sentiment -------
    Sentiment::PROPS = ABSTRACT_SPAN_ANNOTATION_PROPERTIES;
    abstract_parents[ElementType::Sentiment_t] = ElementType::AbstractSpanAnnotation_t;
    Sentiment::PROPS.ELEMENT_ID = ElementType::Sentiment_t;
    Sentiment::PROPS.ACCEPTED_DATA += {ElementType::AbstractFeature_t, ElementType::AbstractInlineAnnotation_t, ElementType::AbstractWord_t, ElementType::Comment_t, ElementType::Description_t, ElementType::ForeignData_t, ElementType::Headspan_t, ElementType::LinkReference_t, ElementType::Metric_t, ElementType::Morpheme_t, ElementType::Phoneme_t, ElementType::PolarityFeature_t, ElementType::Relation_t, ElementType::Source_t, ElementType::StrengthFeature_t, ElementType::Target_t, ElementType::WordReference_t};
    Sentiment::PROPS.ANNOTATIONTYPE = AnnotationType::SENTIMENT;
    Sentiment::PROPS.LABEL = "Sentiment";
    Sentiment::PROPS.XMLTAG = "sentiment";
    element_props[ElementType::Sentiment_t] = &Sentiment::PROPS;
//------ SentimentLayer -------
    SentimentLayer::PROPS = ABSTRACT_ANNOTATION_LAYER_PROPERTIES;
    abstract_parents[ElementType::SentimentLayer_t] = ElementType::AbstractAnnotationLayer_t;
    SentimentLayer::PROPS.ELEMENT_ID = ElementType::SentimentLayer_t;
    SentimentLayer::PROPS.ACCEPTED_DATA += {ElementType::Comment_t, ElementType::Correction_t, ElementType::Description_t, ElementType::ForeignData_t, ElementType::Sentiment_t};
    SentimentLayer::PROPS.ANNOTATIONTYPE = AnnotationType::SENTIMENT;
    SentimentLayer::PROPS.XMLTAG = "sentiments";
    element_props[ElementType::SentimentLayer_t] = &SentimentLayer::PROPS;
//------ SizeFeature -------
    SizeFeature::PROPS = ABSTRACT_FEATURE_PROPERTIES;
    abstract_parents[ElementType::SizeFeature_t] = ElementType::AbstractFeature_t;
    SizeFeature::PROPS.ELEMENT_ID = ElementType::SizeFeature_t;
    SizeFeature::PROPS.SUBSET = "size";
    SizeFeature::PROPS.XMLTAG = "size";
    element_props[ElementType::SizeFeature_t] = &SizeFeature::PROPS;
//------ Source -------
    Source::PROPS = ABSTRACT_SPAN_ROLE_PROPERTIES;
    abstract_parents[ElementType::Source_t] = ElementType::AbstractSpanRole_t;
    Source::PROPS.ELEMENT_ID = ElementType::Source_t;
    Source::PROPS.LABEL = "Source";
    Source::PROPS.OCCURRENCES = 1;
    Source::PROPS.XMLTAG = "source";
    element_props[ElementType::Source_t] = &Source::PROPS;
//------ SpanRelation -------
    SpanRelation::PROPS = ABSTRACT_HIGHER_ORDER_ANNOTATION_PROPERTIES;
    abstract_parents[ElementType::SpanRelation_t] = ElementType::AbstractHigherOrderAnnotation_t;
    SpanRelation::PROPS.ELEMENT_ID = ElementType::SpanRelation_t;
    SpanRelation::PROPS.ACCEPTED_DATA += {ElementType::AbstractFeature_t, ElementType::Comment_t, ElementType::Description_t, ElementType::ForeignData_t, ElementType::Metric_t, ElementType::Relation_t};
    SpanRelation::PROPS.ANNOTATIONTYPE = AnnotationType::SPANRELATION;
    SpanRelation::PROPS.LABEL = "Span Relation";
    SpanRelation::PROPS.OPTIONAL_ATTRIBS = Attrib::ID|Attrib::CLASS|Attrib::ANNOTATOR|Attrib::N|Attrib::CONFIDENCE|Attrib::DATETIME|Attrib::SRC|Attrib::BEGINTIME|Attrib::ENDTIME|Attrib::SPEAKER|Attrib::METADATA|Attrib::TAG;
    SpanRelation::PROPS.PRINTABLE = false;
    SpanRelation::PROPS.REQUIRED_ATTRIBS = Attrib::NO_ATT;
    SpanRelation::PROPS.SPEAKABLE = false;
    SpanRelation::PROPS.XMLTAG = "spanrelation";
    element_props[ElementType::SpanRelation_t] = &SpanRelation::PROPS;
//------ SpanRelationLayer -------
    SpanRelationLayer::PROPS = ABSTRACT_ANNOTATION_LAYER_PROPERTIES;
    abstract_parents[ElementType::SpanRelationLayer_t] = ElementType::AbstractAnnotationLayer_t;
    SpanRelationLayer::PROPS.ELEMENT_ID = ElementType::SpanRelationLayer_t;
    SpanRelationLayer::PROPS.ACCEPTED_DATA += {ElementType::Comment_t, ElementType::Correction_t, ElementType::Description_t, ElementType::ForeignData_t, ElementType::SpanRelation_t};
    SpanRelationLayer::PROPS.ANNOTATIONTYPE = AnnotationType::SPANRELATION;
    SpanRelationLayer::PROPS.XMLTAG = "spanrelations";
    element_props[ElementType::SpanRelationLayer_t] = &SpanRelationLayer::PROPS;
//------ Speech -------
    Speech::PROPS = ABSTRACT_STRUCTURE_ELEMENT_PROPERTIES;
    abstract_parents[ElementType::Speech_t] = ElementType::AbstractStructureElement_t;
    Speech::PROPS.ELEMENT_ID = ElementType::Speech_t;
    Speech::PROPS.ACCEPTED_DATA += {ElementType::AbstractAnnotationLayer_t, ElementType::AbstractFeature_t, ElementType::AbstractInlineAnnotation_t, ElementType::AbstractWord_t, ElementType::Alternative_t, ElementType::AlternativeLayers_t, ElementType::Comment_t, ElementType::Correction_t, ElementType::Description_t, ElementType::Division_t, ElementType::Entry_t, ElementType::Event_t, ElementType::Example_t, ElementType::External_t, ElementType::ForeignData_t, ElementType::Gap_t, ElementType::List_t, ElementType::Metric_t, ElementType::Note_t, ElementType::Paragraph_t, ElementType::Part_t, ElementType::PhonContent_t, ElementType::Quote_t, ElementType::Reference_t, ElementType::Relation_t, ElementType::Sentence_t, ElementType::String_t, ElementType::TextContent_t, ElementType::Utterance_t};
    Speech::PROPS.LABEL = "Speech Body";
    Speech::PROPS.OPTIONAL_ATTRIBS = Attrib::ID|Attrib::ANNOTATOR|Attrib::DATETIME|Attrib::SRC|Attrib::BEGINTIME|Attrib::ENDTIME|Attrib::SPEAKER|Attrib::METADATA|Attrib::SPACE|Attrib::TAG;
    Speech::PROPS.TEXTDELIMITER = "\n\n\n";
    Speech::PROPS.XMLTAG = "speech";
    element_props[ElementType::Speech_t] = &Speech::PROPS;
//------ Statement -------
    Statement::PROPS = ABSTRACT_SPAN_ANNOTATION_PROPERTIES;
    abstract_parents[ElementType::Statement_t] = ElementType::AbstractSpanAnnotation_t;
    Statement::PROPS.ELEMENT_ID = ElementType::Statement_t;
    Statement::PROPS.ACCEPTED_DATA += {ElementType::AbstractFeature_t, ElementType::AbstractInlineAnnotation_t, ElementType::AbstractWord_t, ElementType::Comment_t, ElementType::Description_t, ElementType::ForeignData_t, ElementType::Headspan_t, ElementType::LinkReference_t, ElementType::Metric_t, ElementType::Morpheme_t, ElementType::Phoneme_t, ElementType::Relation_t, ElementType::Source_t, ElementType::StatementRelation_t, ElementType::WordReference_t};
    Statement::PROPS.ANNOTATIONTYPE = AnnotationType::STATEMENT;
    Statement::PROPS.LABEL = "Statement";
    Statement::PROPS.XMLTAG = "statement";
    element_props[ElementType::Statement_t] = &Statement::PROPS;
//------ StatementLayer -------
    StatementLayer::PROPS = ABSTRACT_ANNOTATION_LAYER_PROPERTIES;
    abstract_parents[ElementType::StatementLayer_t] = ElementType::AbstractAnnotationLayer_t;
    StatementLayer::PROPS.ELEMENT_ID = ElementType::StatementLayer_t;
    StatementLayer::PROPS.ACCEPTED_DATA += {ElementType::Comment_t, ElementType::Correction_t, ElementType::Description_t, ElementType::ForeignData_t, ElementType::Statement_t};
    StatementLayer::PROPS.ANNOTATIONTYPE = AnnotationType::STATEMENT;
    StatementLayer::PROPS.XMLTAG = "statements";
    element_props[ElementType::StatementLayer_t] = &StatementLayer::PROPS;
//------ StatementRelation -------
    StatementRelation::PROPS = ABSTRACT_SPAN_ROLE_PROPERTIES;
    abstract_parents[ElementType::StatementRelation_t] = ElementType::AbstractSpanRole_t;
    StatementRelation::PROPS.ELEMENT_ID = ElementType::StatementRelation_t;
    StatementRelation::PROPS.LABEL = "Relation";
    StatementRelation::PROPS.OCCURRENCES = 1;
    StatementRelation::PROPS.XMLTAG = "rel";
    element_props[ElementType::StatementRelation_t] = &StatementRelation::PROPS;
//------ StrengthFeature -------
    StrengthFeature::PROPS = ABSTRACT_FEATURE_PROPERTIES;
    abstract_parents[ElementType::StrengthFeature_t] = ElementType::AbstractFeature_t;
    StrengthFeature::PROPS.ELEMENT_ID = ElementType::StrengthFeature_t;
    StrengthFeature::PROPS.SUBSET = "strength";
    StrengthFeature::PROPS.XMLTAG = "strength";
    element_props[ElementType::StrengthFeature_t] = &StrengthFeature::PROPS;
//------ String -------
    String::PROPS = ABSTRACT_HIGHER_ORDER_ANNOTATION_PROPERTIES;
    abstract_parents[ElementType::String_t] = ElementType::AbstractHigherOrderAnnotation_t;
    String::PROPS.ELEMENT_ID = ElementType::String_t;
    String::PROPS.ACCEPTED_DATA += {ElementType::AbstractFeature_t, ElementType::AbstractInlineAnnotation_t, ElementType::Comment_t, ElementType::Correction_t, ElementType::Description_t, ElementType::ForeignData_t, ElementType::Metric_t, ElementType::PhonContent_t, ElementType::Relation_t, ElementType::TextContent_t};
    String::PROPS.ANNOTATIONTYPE = AnnotationType::STRING;
    String::PROPS.LABEL = "String";
    String::PROPS.OCCURRENCES = 0;
    String::PROPS.OPTIONAL_ATTRIBS = Attrib::ID|Attrib::CLASS|Attrib::ANNOTATOR|Attrib::CONFIDENCE|Attrib::DATETIME|Attrib::N|Attrib::SRC|Attrib::BEGINTIME|Attrib::ENDTIME|Attrib::METADATA|Attrib::TAG;
    String::PROPS.PRINTABLE = true;
    String::PROPS.XMLTAG = "str";
    element_props[ElementType::String_t] = &String::PROPS;
//------ StyleFeature -------
    StyleFeature::PROPS = ABSTRACT_FEATURE_PROPERTIES;
    abstract_parents[ElementType::StyleFeature_t] = ElementType::AbstractFeature_t;
    StyleFeature::PROPS.ELEMENT_ID = ElementType::StyleFeature_t;
    StyleFeature::PROPS.SUBSET = "style";
    StyleFeature::PROPS.XMLTAG = "style";
    element_props[ElementType::StyleFeature_t] = &StyleFeature::PROPS;
//------ SubjectivityAnnotation -------
    SubjectivityAnnotation::PROPS = ABSTRACT_INLINE_ANNOTATION_PROPERTIES;
    abstract_parents[ElementType::SubjectivityAnnotation_t] = ElementType::AbstractInlineAnnotation_t;
    SubjectivityAnnotation::PROPS.ELEMENT_ID = ElementType::SubjectivityAnnotation_t;
    SubjectivityAnnotation::PROPS.ANNOTATIONTYPE = AnnotationType::SUBJECTIVITY;
    SubjectivityAnnotation::PROPS.LABEL = "Subjectivity/Sentiment";
    SubjectivityAnnotation::PROPS.XMLTAG = "subjectivity";
    element_props[ElementType::SubjectivityAnnotation_t] = &SubjectivityAnnotation::PROPS;
//------ Suggestion -------
    Suggestion::PROPS = ABSTRACT_CORRECTION_CHILD_PROPERTIES;
    abstract_parents[ElementType::Suggestion_t] = ElementType::AbstractCorrectionChild_t;
    Suggestion::PROPS.ELEMENT_ID = ElementType::Suggestion_t;
    Suggestion::PROPS.ANNOTATIONTYPE = AnnotationType::CORRECTION;
    Suggestion::PROPS.AUTH = false;
    Suggestion::PROPS.OCCURRENCES = 0;
    Suggestion::PROPS.OPTIONAL_ATTRIBS = Attrib::CONFIDENCE|Attrib::N|Attrib::TAG;
    Suggestion::PROPS.XMLTAG = "suggestion";
    element_props[ElementType::Suggestion_t] = &Suggestion::PROPS;
//------ SynsetFeature -------
    SynsetFeature::PROPS = ABSTRACT_FEATURE_PROPERTIES;
    abstract_parents[ElementType::SynsetFeature_t] = ElementType::AbstractFeature_t;
    SynsetFeature::PROPS.ELEMENT_ID = ElementType::SynsetFeature_t;
    SynsetFeature::PROPS.SUBSET = "synset";
    SynsetFeature::PROPS.XMLTAG = "synset";
    element_props[ElementType::SynsetFeature_t] = &SynsetFeature::PROPS;
//------ SyntacticUnit -------
    SyntacticUnit::PROPS = ABSTRACT_SPAN_ANNOTATION_PROPERTIES;
    abstract_parents[ElementType::SyntacticUnit_t] = ElementType::AbstractSpanAnnotation_t;
    SyntacticUnit::PROPS.ELEMENT_ID = ElementType::SyntacticUnit_t;
    SyntacticUnit::PROPS.ACCEPTED_DATA += {ElementType::AbstractFeature_t, ElementType::AbstractInlineAnnotation_t, ElementType::AbstractWord_t, ElementType::Comment_t, ElementType::Description_t, ElementType::ForeignData_t, ElementType::LinkReference_t, ElementType::Metric_t, ElementType::Morpheme_t, ElementType::Phoneme_t, ElementType::Relation_t, ElementType::SyntacticUnit_t, ElementType::WordReference_t};
    SyntacticUnit::PROPS.ANNOTATIONTYPE = AnnotationType::SYNTAX;
    SyntacticUnit::PROPS.LABEL = "Syntactic Unit";
    SyntacticUnit::PROPS.XMLTAG = "su";
    element_props[ElementType::SyntacticUnit_t] = &SyntacticUnit::PROPS;
//------ SyntaxLayer -------
    SyntaxLayer::PROPS = ABSTRACT_ANNOTATION_LAYER_PROPERTIES;
    abstract_parents[ElementType::SyntaxLayer_t] = ElementType::AbstractAnnotationLayer_t;
    SyntaxLayer::PROPS.ELEMENT_ID = ElementType::SyntaxLayer_t;
    SyntaxLayer::PROPS.ACCEPTED_DATA += {ElementType::Comment_t, ElementType::Correction_t, ElementType::Description_t, ElementType::ForeignData_t, ElementType::SyntacticUnit_t};
    SyntaxLayer::PROPS.ANNOTATIONTYPE = AnnotationType::SYNTAX;
    SyntaxLayer::PROPS.XMLTAG = "syntax";
    element_props[ElementType::SyntaxLayer_t] = &SyntaxLayer::PROPS;
//------ Table -------
    Table::PROPS = ABSTRACT_STRUCTURE_ELEMENT_PROPERTIES;
    abstract_parents[ElementType::Table_t] = ElementType::AbstractStructureElement_t;
    Table::PROPS.ELEMENT_ID = ElementType::Table_t;
    Table::PROPS.ACCEPTED_DATA += {ElementType::AbstractAnnotationLayer_t, ElementType::AbstractFeature_t, ElementType::AbstractInlineAnnotation_t, ElementType::Alternative_t, ElementType::AlternativeLayers_t, ElementType::Comment_t, ElementType::Correction_t, ElementType::Description_t, ElementType::External_t, ElementType::ForeignData_t, ElementType::Linebreak_t, ElementType::Metric_t, ElementType::Part_t, ElementType::Relation_t, ElementType::Row_t, ElementType::TableHead_t};
    Table::PROPS.ANNOTATIONTYPE = AnnotationType::TABLE;
    Table::PROPS.LABEL = "Table";
    Table::PROPS.XMLTAG = "table";
    element_props[ElementType::Table_t] = &Table::PROPS;
//------ TableHead -------
    TableHead::PROPS = ABSTRACT_STRUCTURE_ELEMENT_PROPERTIES;
    abstract_parents[ElementType::TableHead_t] = ElementType::AbstractStructureElement_t;
    TableHead::PROPS.ELEMENT_ID = ElementType::TableHead_t;
    TableHead::PROPS.ACCEPTED_DATA += {ElementType::AbstractAnnotationLayer_t, ElementType::AbstractFeature_t, ElementType::AbstractInlineAnnotation_t, ElementType::Alternative_t, ElementType::AlternativeLayers_t, ElementType::Comment_t, ElementType::Correction_t, ElementType::Description_t, ElementType::External_t, ElementType::ForeignData_t, ElementType::Metric_t, ElementType::Part_t, ElementType::Relation_t, ElementType::Row_t};
    TableHead::PROPS.LABEL = "Table Header";
    TableHead::PROPS.OPTIONAL_ATTRIBS = Attrib::ID|Attrib::ANNOTATOR|Attrib::N|Attrib::CONFIDENCE|Attrib::DATETIME|Attrib::SRC|Attrib::BEGINTIME|Attrib::ENDTIME|Attrib::SPEAKER|Attrib::METADATA|Attrib::TAG;
    TableHead::PROPS.XMLTAG = "tablehead";
    element_props[ElementType::TableHead_t] = &TableHead::PROPS;
//------ Target -------
    Target::PROPS = ABSTRACT_SPAN_ROLE_PROPERTIES;
    abstract_parents[ElementType::Target_t] = ElementType::AbstractSpanRole_t;
    Target::PROPS.ELEMENT_ID = ElementType::Target_t;
    Target::PROPS.LABEL = "Target";
    Target::PROPS.OCCURRENCES = 1;
    Target::PROPS.XMLTAG = "target";
    element_props[ElementType::Target_t] = &Target::PROPS;
//------ Term -------
    Term::PROPS = ABSTRACT_STRUCTURE_ELEMENT_PROPERTIES;
    abstract_parents[ElementType::Term_t] = ElementType::AbstractStructureElement_t;
    Term::PROPS.ELEMENT_ID = ElementType::Term_t;
    Term::PROPS.ACCEPTED_DATA += {ElementType::AbstractAnnotationLayer_t, ElementType::AbstractFeature_t, ElementType::AbstractInlineAnnotation_t, ElementType::AbstractWord_t, ElementType::Alternative_t, ElementType::AlternativeLayers_t, ElementType::Comment_t, ElementType::Correction_t, ElementType::Description_t, ElementType::Event_t, ElementType::External_t, ElementType::Figure_t, ElementType::ForeignData_t, ElementType::Gap_t, ElementType::Linebreak_t, ElementType::List_t, ElementType::Metric_t, ElementType::Paragraph_t, ElementType::Part_t, ElementType::PhonContent_t, ElementType::Reference_t, ElementType::Relation_t, ElementType::Sentence_t, ElementType::String_t, ElementType::Table_t, ElementType::TextContent_t, ElementType::Utterance_t, ElementType::Whitespace_t};
    Term::PROPS.ANNOTATIONTYPE = AnnotationType::TERM;
    Term::PROPS.LABEL = "Term";
    Term::PROPS.XMLTAG = "term";
    element_props[ElementType::Term_t] = &Term::PROPS;
//------ Text -------
    Text::PROPS = ABSTRACT_STRUCTURE_ELEMENT_PROPERTIES;
    abstract_parents[ElementType::Text_t] = ElementType::AbstractStructureElement_t;
    Text::PROPS.ELEMENT_ID = ElementType::Text_t;
    Text::PROPS.ACCEPTED_DATA += {ElementType::AbstractAnnotationLayer_t, ElementType::AbstractFeature_t, ElementType::AbstractInlineAnnotation_t, ElementType::AbstractWord_t, ElementType::Alternative_t, ElementType::AlternativeLayers_t, ElementType::Comment_t, ElementType::Correction_t, ElementType::Description_t, ElementType::Division_t, ElementType::Entry_t, ElementType::Event_t, ElementType::Example_t, ElementType::External_t, ElementType::Figure_t, ElementType::ForeignData_t, ElementType::Gap_t, ElementType::Linebreak_t, ElementType::List_t, ElementType::Metric_t, ElementType::Note_t, ElementType::Paragraph_t, ElementType::Part_t, ElementType::PhonContent_t, ElementType::Quote_t, ElementType::Reference_t, ElementType::Relation_t, ElementType::Sentence_t, ElementType::String_t, ElementType::Table_t, ElementType::TextContent_t, ElementType::Whitespace_t};
    Text::PROPS.LABEL = "Text Body";
    Text::PROPS.OPTIONAL_ATTRIBS = Attrib::ID|Attrib::ANNOTATOR|Attrib::DATETIME|Attrib::SRC|Attrib::BEGINTIME|Attrib::ENDTIME|Attrib::SPEAKER|Attrib::METADATA|Attrib::SPACE|Attrib::TAG;
    Text::PROPS.TEXTDELIMITER = "\n\n\n";
    Text::PROPS.XMLTAG = "text";
    element_props[ElementType::Text_t] = &Text::PROPS;
//------ TextContent -------
    TextContent::PROPS = ABSTRACT_CONTENT_ANNOTATION_PROPERTIES;
    abstract_parents[ElementType::TextContent_t] = ElementType::AbstractContentAnnotation_t;
    TextContent::PROPS.ELEMENT_ID = ElementType::TextContent_t;
    TextContent::PROPS.ACCEPTED_DATA += {ElementType::AbstractFeature_t, ElementType::AbstractTextMarkup_t, ElementType::Comment_t, ElementType::Description_t, ElementType::Linebreak_t, ElementType::XmlText_t};
    TextContent::PROPS.ANNOTATIONTYPE = AnnotationType::TEXT;
    TextContent::PROPS.LABEL = "Text";
    TextContent::PROPS.PRINTABLE = true;
    TextContent::PROPS.SPEAKABLE = false;
    TextContent::PROPS.TEXTCONTAINER = true;
    TextContent::PROPS.XLINK = true;
    TextContent::PROPS.XMLTAG = "t";
    element_props[ElementType::TextContent_t] = &TextContent::PROPS;
//------ TextMarkupCorrection -------
    TextMarkupCorrection::PROPS = ABSTRACT_TEXT_MARKUP_PROPERTIES;
    abstract_parents[ElementType::TextMarkupCorrection_t] = ElementType::AbstractTextMarkup_t;
    TextMarkupCorrection::PROPS.ELEMENT_ID = ElementType::TextMarkupCorrection_t;
    TextMarkupCorrection::PROPS.ANNOTATIONTYPE = AnnotationType::CORRECTION;
    TextMarkupCorrection::PROPS.XMLTAG = "t-correction";
    element_props[ElementType::TextMarkupCorrection_t] = &TextMarkupCorrection::PROPS;
//------ TextMarkupError -------
    TextMarkupError::PROPS = ABSTRACT_TEXT_MARKUP_PROPERTIES;
    abstract_parents[ElementType::TextMarkupError_t] = ElementType::AbstractTextMarkup_t;
    TextMarkupError::PROPS.ELEMENT_ID = ElementType::TextMarkupError_t;
    TextMarkupError::PROPS.ANNOTATIONTYPE = AnnotationType::ERRORDETECTION;
    TextMarkupError::PROPS.XMLTAG = "t-error";
    element_props[ElementType::TextMarkupError_t] = &TextMarkupError::PROPS;
//------ TextMarkupGap -------
    TextMarkupGap::PROPS = ABSTRACT_TEXT_MARKUP_PROPERTIES;
    abstract_parents[ElementType::TextMarkupGap_t] = ElementType::AbstractTextMarkup_t;
    TextMarkupGap::PROPS.ELEMENT_ID = ElementType::TextMarkupGap_t;
    TextMarkupGap::PROPS.ANNOTATIONTYPE = AnnotationType::GAP;
    TextMarkupGap::PROPS.XMLTAG = "t-gap";
    element_props[ElementType::TextMarkupGap_t] = &TextMarkupGap::PROPS;
//------ TextMarkupHSpace -------
    TextMarkupHSpace::PROPS = ABSTRACT_TEXT_MARKUP_PROPERTIES;
    abstract_parents[ElementType::TextMarkupHSpace_t] = ElementType::AbstractTextMarkup_t;
    TextMarkupHSpace::PROPS.ELEMENT_ID = ElementType::TextMarkupHSpace_t;
    TextMarkupHSpace::PROPS.ANNOTATIONTYPE = AnnotationType::HSPACE;
    TextMarkupHSpace::PROPS.IMPLICITSPACE = true;
    TextMarkupHSpace::PROPS.TEXTDELIMITER = "";
    TextMarkupHSpace::PROPS.XMLTAG = "t-hspace";
    element_props[ElementType::TextMarkupHSpace_t] = &TextMarkupHSpace::PROPS;
//------ TextMarkupLanguage -------
    TextMarkupLanguage::PROPS = ABSTRACT_TEXT_MARKUP_PROPERTIES;
    abstract_parents[ElementType::TextMarkupLanguage_t] = ElementType::AbstractTextMarkup_t;
    TextMarkupLanguage::PROPS.ELEMENT_ID = ElementType::TextMarkupLanguage_t;
    TextMarkupLanguage::PROPS.ANNOTATIONTYPE = AnnotationType::LANG;
    TextMarkupLanguage::PROPS.XMLTAG = "t-lang";
    element_props[ElementType::TextMarkupLanguage_t] = &TextMarkupLanguage::PROPS;
//------ TextMarkupReference -------
    TextMarkupReference::PROPS = ABSTRACT_TEXT_MARKUP_PROPERTIES;
    abstract_parents[ElementType::TextMarkupReference_t] = ElementType::AbstractTextMarkup_t;
    TextMarkupReference::PROPS.ELEMENT_ID = ElementType::TextMarkupReference_t;
    TextMarkupReference::PROPS.ANNOTATIONTYPE = AnnotationType::REFERENCE;
    TextMarkupReference::PROPS.XMLTAG = "t-ref";
    element_props[ElementType::TextMarkupReference_t] = &TextMarkupReference::PROPS;
//------ TextMarkupString -------
    TextMarkupString::PROPS = ABSTRACT_TEXT_MARKUP_PROPERTIES;
    abstract_parents[ElementType::TextMarkupString_t] = ElementType::AbstractTextMarkup_t;
    TextMarkupString::PROPS.ELEMENT_ID = ElementType::TextMarkupString_t;
    TextMarkupString::PROPS.ANNOTATIONTYPE = AnnotationType::STRING;
    TextMarkupString::PROPS.XMLTAG = "t-str";
    element_props[ElementType::TextMarkupString_t] = &TextMarkupString::PROPS;
//------ TextMarkupStyle -------
    TextMarkupStyle::PROPS = ABSTRACT_TEXT_MARKUP_PROPERTIES;
    abstract_parents[ElementType::TextMarkupStyle_t] = ElementType::AbstractTextMarkup_t;
    TextMarkupStyle::PROPS.ELEMENT_ID = ElementType::TextMarkupStyle_t;
    TextMarkupStyle::PROPS.ACCEPTED_DATA += {ElementType::AbstractFeature_t, ElementType::AbstractTextMarkup_t, ElementType::Comment_t, ElementType::Description_t, ElementType::FontFeature_t, ElementType::Linebreak_t, ElementType::SizeFeature_t};
    TextMarkupStyle::PROPS.ANNOTATIONTYPE = AnnotationType::STYLE;
    TextMarkupStyle::PROPS.XMLTAG = "t-style";
    element_props[ElementType::TextMarkupStyle_t] = &TextMarkupStyle::PROPS;
//------ TextMarkupWhitespace -------
    TextMarkupWhitespace::PROPS = ABSTRACT_TEXT_MARKUP_PROPERTIES;
    abstract_parents[ElementType::TextMarkupWhitespace_t] = ElementType::AbstractTextMarkup_t;
    TextMarkupWhitespace::PROPS.ELEMENT_ID = ElementType::TextMarkupWhitespace_t;
    TextMarkupWhitespace::PROPS.ANNOTATIONTYPE = AnnotationType::WHITESPACE;
    TextMarkupWhitespace::PROPS.IMPLICITSPACE = true;
    TextMarkupWhitespace::PROPS.TEXTDELIMITER = "";
    TextMarkupWhitespace::PROPS.XMLTAG = "t-whitespace";
    element_props[ElementType::TextMarkupWhitespace_t] = &TextMarkupWhitespace::PROPS;
//------ TimeFeature -------
    TimeFeature::PROPS = ABSTRACT_FEATURE_PROPERTIES;
    abstract_parents[ElementType::TimeFeature_t] = ElementType::AbstractFeature_t;
    TimeFeature::PROPS.ELEMENT_ID = ElementType::TimeFeature_t;
    TimeFeature::PROPS.SUBSET = "time";
    TimeFeature::PROPS.XMLTAG = "time";
    element_props[ElementType::TimeFeature_t] = &TimeFeature::PROPS;
//------ TimeSegment -------
    TimeSegment::PROPS = ABSTRACT_SPAN_ANNOTATION_PROPERTIES;
    abstract_parents[ElementType::TimeSegment_t] = ElementType::AbstractSpanAnnotation_t;
    TimeSegment::PROPS.ELEMENT_ID = ElementType::TimeSegment_t;
    TimeSegment::PROPS.ACCEPTED_DATA += {ElementType::AbstractFeature_t, ElementType::AbstractInlineAnnotation_t, ElementType::AbstractWord_t, ElementType::ActorFeature_t, ElementType::BegindatetimeFeature_t, ElementType::Comment_t, ElementType::Description_t, ElementType::EnddatetimeFeature_t, ElementType::ForeignData_t, ElementType::LinkReference_t, ElementType::Metric_t, ElementType::Morpheme_t, ElementType::Phoneme_t, ElementType::Relation_t, ElementType::WordReference_t};
    TimeSegment::PROPS.ANNOTATIONTYPE = AnnotationType::TIMESEGMENT;
    TimeSegment::PROPS.LABEL = "Time Segment";
    TimeSegment::PROPS.XMLTAG = "timesegment";
    element_props[ElementType::TimeSegment_t] = &TimeSegment::PROPS;
//------ TimingLayer -------
    TimingLayer::PROPS = ABSTRACT_ANNOTATION_LAYER_PROPERTIES;
    abstract_parents[ElementType::TimingLayer_t] = ElementType::AbstractAnnotationLayer_t;
    TimingLayer::PROPS.ELEMENT_ID = ElementType::TimingLayer_t;
    TimingLayer::PROPS.ACCEPTED_DATA += {ElementType::Comment_t, ElementType::Correction_t, ElementType::Description_t, ElementType::ForeignData_t, ElementType::TimeSegment_t};
    TimingLayer::PROPS.ANNOTATIONTYPE = AnnotationType::TIMESEGMENT;
    TimingLayer::PROPS.XMLTAG = "timing";
    element_props[ElementType::TimingLayer_t] = &TimingLayer::PROPS;
//------ Utterance -------
    Utterance::PROPS = ABSTRACT_STRUCTURE_ELEMENT_PROPERTIES;
    abstract_parents[ElementType::Utterance_t] = ElementType::AbstractStructureElement_t;
    Utterance::PROPS.ELEMENT_ID = ElementType::Utterance_t;
    Utterance::PROPS.ACCEPTED_DATA += {ElementType::AbstractAnnotationLayer_t, ElementType::AbstractFeature_t, ElementType::AbstractInlineAnnotation_t, ElementType::AbstractWord_t, ElementType::Alternative_t, ElementType::AlternativeLayers_t, ElementType::Comment_t, ElementType::Correction_t, ElementType::Description_t, ElementType::External_t, ElementType::ForeignData_t, ElementType::Gap_t, ElementType::Metric_t, ElementType::Note_t, ElementType::Part_t, ElementType::PhonContent_t, ElementType::Quote_t, ElementType::Reference_t, ElementType::Relation_t, ElementType::Sentence_t, ElementType::String_t, ElementType::TextContent_t};
    Utterance::PROPS.ANNOTATIONTYPE = AnnotationType::UTTERANCE;
    Utterance::PROPS.LABEL = "Utterance";
    Utterance::PROPS.TEXTDELIMITER = " ";
    Utterance::PROPS.XMLTAG = "utt";
    element_props[ElementType::Utterance_t] = &Utterance::PROPS;
//------ ValueFeature -------
    ValueFeature::PROPS = ABSTRACT_FEATURE_PROPERTIES;
    abstract_parents[ElementType::ValueFeature_t] = ElementType::AbstractFeature_t;
    ValueFeature::PROPS.ELEMENT_ID = ElementType::ValueFeature_t;
    ValueFeature::PROPS.SUBSET = "value";
    ValueFeature::PROPS.XMLTAG = "value";
    element_props[ElementType::ValueFeature_t] = &ValueFeature::PROPS;
//------ Whitespace -------
    Whitespace::PROPS = ABSTRACT_STRUCTURE_ELEMENT_PROPERTIES;
    abstract_parents[ElementType::Whitespace_t] = ElementType::AbstractStructureElement_t;
    Whitespace::PROPS.ELEMENT_ID = ElementType::Whitespace_t;
    Whitespace::PROPS.ANNOTATIONTYPE = AnnotationType::WHITESPACE;
    Whitespace::PROPS.IMPLICITSPACE = true;
    Whitespace::PROPS.LABEL = "Whitespace";
    Whitespace::PROPS.TEXTDELIMITER = "";
    Whitespace::PROPS.XMLTAG = "whitespace";
    element_props[ElementType::Whitespace_t] = &Whitespace::PROPS;
//------ Word -------
    Word::PROPS = ABSTRACT_WORD_PROPERTIES;
    abstract_parents[ElementType::Word_t] = ElementType::AbstractWord_t;
    Word::PROPS.ELEMENT_ID = ElementType::Word_t;
    Word::PROPS.ACCEPTED_DATA += {ElementType::AbstractAnnotationLayer_t, ElementType::AbstractFeature_t, ElementType::AbstractInlineAnnotation_t, ElementType::Alternative_t, ElementType::AlternativeLayers_t, ElementType::Comment_t, ElementType::Correction_t, ElementType::Description_t, ElementType::External_t, ElementType::ForeignData_t, ElementType::Metric_t, ElementType::Part_t, ElementType::PhonContent_t, ElementType::Reference_t, ElementType::Relation_t, ElementType::String_t, ElementType::TextContent_t};
    Word::PROPS.ANNOTATIONTYPE = AnnotationType::TOKEN;
    Word::PROPS.LABEL = "Word/Token";
    Word::PROPS.OPTIONAL_ATTRIBS = Attrib::ID|Attrib::CLASS|Attrib::ANNOTATOR|Attrib::N|Attrib::CONFIDENCE|Attrib::DATETIME|Attrib::SRC|Attrib::BEGINTIME|Attrib::ENDTIME|Attrib::SPEAKER|Attrib::TEXTCLASS|Attrib::METADATA|Attrib::SPACE|Attrib::TAG;
    Word::PROPS.TEXTDELIMITER = " ";
    Word::PROPS.WREFABLE = true;
    Word::PROPS.XMLTAG = "w";
    element_props[ElementType::Word_t] = &Word::PROPS;
//------ WordReference -------
    WordReference::PROPS.ELEMENT_ID = ElementType::WordReference_t;
    WordReference::PROPS.OPTIONAL_ATTRIBS = Attrib::IDREF|Attrib::TAG;
    WordReference::PROPS.XMLTAG = "wref";
    element_props[ElementType::WordReference_t] = &WordReference::PROPS;

//foliaspec:end:setelementproperties

    XmlText::PROPS.XMLTAG = "_XmlText";
    XmlText::PROPS.ELEMENT_ID = ElementType::XmlText_t;
    XmlText::PROPS.TEXTDELIMITER = "*";
    XmlText::PROPS.PRINTABLE = true;
    XmlText::PROPS.SPEAKABLE = true;

    XmlComment::PROPS.XMLTAG = "_XmlComment";
    XmlComment::PROPS.ELEMENT_ID = ElementType::XmlComment_t;

    ProcessingInstruction::PROPS.XMLTAG = "PI";
    ProcessingInstruction::PROPS.ELEMENT_ID = ElementType::ProcessingInstruction_t;

    for ( const auto& [tag,val] : oldtags ){
      reverse_old[val] = tag;
    }

    for ( const auto& [ann,et] : annotationtype_elementtype_map ){
      element_annotation_map[et] = ann;
    }
  }


  //foliaspec:typehierarchy
  static const map<ElementType, set<ElementType> > typeHierarchy = {      { ElementType::AbstractAnnotationLayer_t, {  } },
     { ElementType::AbstractContentAnnotation_t, {  } },
     { ElementType::AbstractCorrectionChild_t, {  } },
     { ElementType::AbstractHigherOrderAnnotation_t, {  } },
     { ElementType::AbstractInlineAnnotation_t, {  } },
     { ElementType::AbstractSpanAnnotation_t, {  } },
     { ElementType::AbstractSpanRole_t, { ElementType::AbstractSpanAnnotation_t } },
     { ElementType::AbstractStructureElement_t, {  } },
     { ElementType::AbstractSubtokenAnnotation_t, {  } },
     { ElementType::AbstractTextMarkup_t, {  } },
     { ElementType::ActorFeature_t, { ElementType::AbstractHigherOrderAnnotation_t,ElementType::AbstractFeature_t } },
     { ElementType::Alternative_t, { ElementType::AbstractHigherOrderAnnotation_t } },
     { ElementType::AlternativeLayers_t, { ElementType::AbstractHigherOrderAnnotation_t } },
     { ElementType::BegindatetimeFeature_t, { ElementType::AbstractHigherOrderAnnotation_t,ElementType::AbstractFeature_t } },
     { ElementType::Caption_t, { ElementType::AbstractStructureElement_t } },
     { ElementType::Cell_t, { ElementType::AbstractStructureElement_t } },
     { ElementType::Chunk_t, { ElementType::AbstractSpanAnnotation_t } },
     { ElementType::ChunkingLayer_t, { ElementType::AbstractAnnotationLayer_t } },
     { ElementType::Comment_t, { ElementType::AbstractHigherOrderAnnotation_t } },
     { ElementType::Content_t, { ElementType::AbstractContentAnnotation_t } },
     { ElementType::CoreferenceChain_t, { ElementType::AbstractSpanAnnotation_t } },
     { ElementType::CoreferenceLayer_t, { ElementType::AbstractAnnotationLayer_t } },
     { ElementType::CoreferenceLink_t, { ElementType::AbstractSpanRole_t,ElementType::AbstractSpanAnnotation_t } },
     { ElementType::Correction_t, { ElementType::AbstractHigherOrderAnnotation_t } },
     { ElementType::Cue_t, { ElementType::AbstractSpanRole_t,ElementType::AbstractSpanAnnotation_t } },
     { ElementType::Current_t, { ElementType::AbstractCorrectionChild_t } },
     { ElementType::Definition_t, { ElementType::AbstractStructureElement_t } },
     { ElementType::DependenciesLayer_t, { ElementType::AbstractAnnotationLayer_t } },
     { ElementType::Dependency_t, { ElementType::AbstractSpanAnnotation_t } },
     { ElementType::DependencyDependent_t, { ElementType::AbstractSpanRole_t,ElementType::AbstractSpanAnnotation_t } },
     { ElementType::Description_t, { ElementType::AbstractHigherOrderAnnotation_t } },
     { ElementType::Division_t, { ElementType::AbstractStructureElement_t } },
     { ElementType::DomainAnnotation_t, { ElementType::AbstractInlineAnnotation_t } },
     { ElementType::EnddatetimeFeature_t, { ElementType::AbstractHigherOrderAnnotation_t,ElementType::AbstractFeature_t } },
     { ElementType::EntitiesLayer_t, { ElementType::AbstractAnnotationLayer_t } },
     { ElementType::Entity_t, { ElementType::AbstractSpanAnnotation_t } },
     { ElementType::Entry_t, { ElementType::AbstractStructureElement_t } },
     { ElementType::ErrorDetection_t, { ElementType::AbstractInlineAnnotation_t } },
     { ElementType::EtymologyAnnotation_t, { ElementType::AbstractInlineAnnotation_t } },
     { ElementType::Event_t, { ElementType::AbstractStructureElement_t } },
     { ElementType::Example_t, { ElementType::AbstractStructureElement_t } },
     { ElementType::External_t, { ElementType::AbstractHigherOrderAnnotation_t } },
     { ElementType::Feature_t, { ElementType::AbstractHigherOrderAnnotation_t,ElementType::AbstractFeature_t } },
     { ElementType::Figure_t, { ElementType::AbstractStructureElement_t } },
     { ElementType::FontFeature_t, { ElementType::AbstractHigherOrderAnnotation_t,ElementType::AbstractFeature_t } },
     { ElementType::ForeignData_t, { ElementType::AbstractHigherOrderAnnotation_t } },
     { ElementType::FunctionFeature_t, { ElementType::AbstractHigherOrderAnnotation_t,ElementType::AbstractFeature_t } },
     { ElementType::Gap_t, { ElementType::AbstractHigherOrderAnnotation_t } },
     { ElementType::Head_t, { ElementType::AbstractStructureElement_t } },
     { ElementType::HeadFeature_t, { ElementType::AbstractHigherOrderAnnotation_t,ElementType::AbstractFeature_t } },
     { ElementType::Headspan_t, { ElementType::AbstractSpanRole_t,ElementType::AbstractSpanAnnotation_t } },
     { ElementType::Hiddenword_t, { ElementType::AbstractStructureElement_t,ElementType::AbstractWord_t } },
     { ElementType::Hyphbreak_t, { ElementType::AbstractTextMarkup_t } },
     { ElementType::Label_t, { ElementType::AbstractStructureElement_t } },
     { ElementType::LangAnnotation_t, { ElementType::AbstractInlineAnnotation_t } },
     { ElementType::LemmaAnnotation_t, { ElementType::AbstractInlineAnnotation_t } },
     { ElementType::LevelFeature_t, { ElementType::AbstractHigherOrderAnnotation_t,ElementType::AbstractFeature_t } },
     { ElementType::Linebreak_t, { ElementType::AbstractStructureElement_t } },
     { ElementType::List_t, { ElementType::AbstractStructureElement_t } },
     { ElementType::ListItem_t, { ElementType::AbstractStructureElement_t } },
     { ElementType::Metric_t, { ElementType::AbstractHigherOrderAnnotation_t } },
     { ElementType::ModalitiesLayer_t, { ElementType::AbstractAnnotationLayer_t } },
     { ElementType::Modality_t, { ElementType::AbstractSpanAnnotation_t } },
     { ElementType::ModalityFeature_t, { ElementType::AbstractHigherOrderAnnotation_t,ElementType::AbstractFeature_t } },
     { ElementType::Morpheme_t, { ElementType::AbstractSubtokenAnnotation_t } },
     { ElementType::MorphologyLayer_t, { ElementType::AbstractAnnotationLayer_t } },
     { ElementType::New_t, { ElementType::AbstractCorrectionChild_t } },
     { ElementType::Note_t, { ElementType::AbstractStructureElement_t } },
     { ElementType::Observation_t, { ElementType::AbstractSpanAnnotation_t } },
     { ElementType::ObservationLayer_t, { ElementType::AbstractAnnotationLayer_t } },
     { ElementType::Original_t, { ElementType::AbstractCorrectionChild_t } },
     { ElementType::Paragraph_t, { ElementType::AbstractStructureElement_t } },
     { ElementType::Part_t, { ElementType::AbstractStructureElement_t } },
     { ElementType::PhonContent_t, { ElementType::AbstractContentAnnotation_t } },
     { ElementType::Phoneme_t, { ElementType::AbstractSubtokenAnnotation_t } },
     { ElementType::PhonologyLayer_t, { ElementType::AbstractAnnotationLayer_t } },
     { ElementType::PolarityFeature_t, { ElementType::AbstractHigherOrderAnnotation_t,ElementType::AbstractFeature_t } },
     { ElementType::PosAnnotation_t, { ElementType::AbstractInlineAnnotation_t } },
     { ElementType::Predicate_t, { ElementType::AbstractSpanAnnotation_t } },
     { ElementType::Quote_t, { ElementType::AbstractStructureElement_t } },
     { ElementType::Reference_t, { ElementType::AbstractStructureElement_t } },
     { ElementType::Relation_t, { ElementType::AbstractHigherOrderAnnotation_t } },
     { ElementType::Row_t, { ElementType::AbstractStructureElement_t } },
     { ElementType::Scope_t, { ElementType::AbstractSpanRole_t,ElementType::AbstractSpanAnnotation_t } },
     { ElementType::SemanticRole_t, { ElementType::AbstractSpanAnnotation_t } },
     { ElementType::SemanticRolesLayer_t, { ElementType::AbstractAnnotationLayer_t } },
     { ElementType::SenseAnnotation_t, { ElementType::AbstractInlineAnnotation_t } },
     { ElementType::Sentence_t, { ElementType::AbstractStructureElement_t } },
     { ElementType::Sentiment_t, { ElementType::AbstractSpanAnnotation_t } },
     { ElementType::SentimentLayer_t, { ElementType::AbstractAnnotationLayer_t } },
     { ElementType::SizeFeature_t, { ElementType::AbstractHigherOrderAnnotation_t,ElementType::AbstractFeature_t } },
     { ElementType::Source_t, { ElementType::AbstractSpanRole_t,ElementType::AbstractSpanAnnotation_t } },
     { ElementType::SpanRelation_t, { ElementType::AbstractHigherOrderAnnotation_t } },
     { ElementType::SpanRelationLayer_t, { ElementType::AbstractAnnotationLayer_t } },
     { ElementType::Speech_t, { ElementType::AbstractStructureElement_t } },
     { ElementType::Statement_t, { ElementType::AbstractSpanAnnotation_t } },
     { ElementType::StatementLayer_t, { ElementType::AbstractAnnotationLayer_t } },
     { ElementType::StatementRelation_t, { ElementType::AbstractSpanRole_t,ElementType::AbstractSpanAnnotation_t } },
     { ElementType::StrengthFeature_t, { ElementType::AbstractHigherOrderAnnotation_t,ElementType::AbstractFeature_t } },
     { ElementType::String_t, { ElementType::AbstractHigherOrderAnnotation_t } },
     { ElementType::StyleFeature_t, { ElementType::AbstractHigherOrderAnnotation_t,ElementType::AbstractFeature_t } },
     { ElementType::SubjectivityAnnotation_t, { ElementType::AbstractInlineAnnotation_t } },
     { ElementType::Suggestion_t, { ElementType::AbstractCorrectionChild_t } },
     { ElementType::SynsetFeature_t, { ElementType::AbstractHigherOrderAnnotation_t,ElementType::AbstractFeature_t } },
     { ElementType::SyntacticUnit_t, { ElementType::AbstractSpanAnnotation_t } },
     { ElementType::SyntaxLayer_t, { ElementType::AbstractAnnotationLayer_t } },
     { ElementType::Table_t, { ElementType::AbstractStructureElement_t } },
     { ElementType::TableHead_t, { ElementType::AbstractStructureElement_t } },
     { ElementType::Target_t, { ElementType::AbstractSpanRole_t,ElementType::AbstractSpanAnnotation_t } },
     { ElementType::Term_t, { ElementType::AbstractStructureElement_t } },
     { ElementType::Text_t, { ElementType::AbstractStructureElement_t } },
     { ElementType::TextContent_t, { ElementType::AbstractContentAnnotation_t } },
     { ElementType::TextMarkupCorrection_t, { ElementType::AbstractTextMarkup_t } },
     { ElementType::TextMarkupError_t, { ElementType::AbstractTextMarkup_t } },
     { ElementType::TextMarkupGap_t, { ElementType::AbstractTextMarkup_t } },
     { ElementType::TextMarkupHSpace_t, { ElementType::AbstractTextMarkup_t } },
     { ElementType::TextMarkupLanguage_t, { ElementType::AbstractTextMarkup_t } },
     { ElementType::TextMarkupReference_t, { ElementType::AbstractTextMarkup_t } },
     { ElementType::TextMarkupString_t, { ElementType::AbstractTextMarkup_t } },
     { ElementType::TextMarkupStyle_t, { ElementType::AbstractTextMarkup_t } },
     { ElementType::TextMarkupWhitespace_t, { ElementType::AbstractTextMarkup_t } },
     { ElementType::TimeFeature_t, { ElementType::AbstractHigherOrderAnnotation_t,ElementType::AbstractFeature_t } },
     { ElementType::TimeSegment_t, { ElementType::AbstractSpanAnnotation_t } },
     { ElementType::TimingLayer_t, { ElementType::AbstractAnnotationLayer_t } },
     { ElementType::Utterance_t, { ElementType::AbstractStructureElement_t } },
     { ElementType::ValueFeature_t, { ElementType::AbstractHigherOrderAnnotation_t,ElementType::AbstractFeature_t } },
     { ElementType::Whitespace_t, { ElementType::AbstractStructureElement_t } },
     { ElementType::Word_t, { ElementType::AbstractStructureElement_t,ElementType::AbstractWord_t } },
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
  const set<ElementType> wrefables = { ElementType::Hiddenword_t, ElementType::Morpheme_t, ElementType::Phoneme_t, ElementType::Word_t };

  //foliaspec:annotationtype_elementtype_map
  //A mapping from annotation types to element types, based on the assumption that there is always only one primary element for an annotation type (and possible multiple secondary ones which are not included in this map,w)
  const map<AnnotationType,ElementType> annotationtype_elementtype_map = {
    {  AnnotationType::ALTERNATIVE, ElementType::Alternative_t },
    {  AnnotationType::CHUNKING, ElementType::Chunk_t },
    {  AnnotationType::COMMENT, ElementType::Comment_t },
    {  AnnotationType::RAWCONTENT, ElementType::Content_t },
    {  AnnotationType::COREFERENCE, ElementType::CoreferenceChain_t },
    {  AnnotationType::CORRECTION, ElementType::Correction_t },
    {  AnnotationType::DEFINITION, ElementType::Definition_t },
    {  AnnotationType::DEPENDENCY, ElementType::Dependency_t },
    {  AnnotationType::DESCRIPTION, ElementType::Description_t },
    {  AnnotationType::DIVISION, ElementType::Division_t },
    {  AnnotationType::DOMAIN, ElementType::DomainAnnotation_t },
    {  AnnotationType::ENTITY, ElementType::Entity_t },
    {  AnnotationType::ENTRY, ElementType::Entry_t },
    {  AnnotationType::ERRORDETECTION, ElementType::ErrorDetection_t },
    {  AnnotationType::ETYMOLOGY, ElementType::EtymologyAnnotation_t },
    {  AnnotationType::EVENT, ElementType::Event_t },
    {  AnnotationType::EXAMPLE, ElementType::Example_t },
    {  AnnotationType::EXTERNAL, ElementType::External_t },
    {  AnnotationType::FIGURE, ElementType::Figure_t },
    {  AnnotationType::GAP, ElementType::Gap_t },
    {  AnnotationType::HEAD, ElementType::Head_t },
    {  AnnotationType::HIDDENTOKEN, ElementType::Hiddenword_t },
    {  AnnotationType::HYPHENATION, ElementType::Hyphbreak_t },
    {  AnnotationType::LANG, ElementType::LangAnnotation_t },
    {  AnnotationType::LEMMA, ElementType::LemmaAnnotation_t },
    {  AnnotationType::LINEBREAK, ElementType::Linebreak_t },
    {  AnnotationType::LIST, ElementType::List_t },
    {  AnnotationType::METRIC, ElementType::Metric_t },
    {  AnnotationType::MODALITY, ElementType::Modality_t },
    {  AnnotationType::MORPHOLOGICAL, ElementType::Morpheme_t },
    {  AnnotationType::NOTE, ElementType::Note_t },
    {  AnnotationType::OBSERVATION, ElementType::Observation_t },
    {  AnnotationType::PARAGRAPH, ElementType::Paragraph_t },
    {  AnnotationType::PART, ElementType::Part_t },
    {  AnnotationType::PHON, ElementType::PhonContent_t },
    {  AnnotationType::PHONOLOGICAL, ElementType::Phoneme_t },
    {  AnnotationType::POS, ElementType::PosAnnotation_t },
    {  AnnotationType::PREDICATE, ElementType::Predicate_t },
    {  AnnotationType::QUOTE, ElementType::Quote_t },
    {  AnnotationType::REFERENCE, ElementType::Reference_t },
    {  AnnotationType::RELATION, ElementType::Relation_t },
    {  AnnotationType::SEMROLE, ElementType::SemanticRole_t },
    {  AnnotationType::SENSE, ElementType::SenseAnnotation_t },
    {  AnnotationType::SENTENCE, ElementType::Sentence_t },
    {  AnnotationType::SENTIMENT, ElementType::Sentiment_t },
    {  AnnotationType::SPANRELATION, ElementType::SpanRelation_t },
    {  AnnotationType::STATEMENT, ElementType::Statement_t },
    {  AnnotationType::STRING, ElementType::String_t },
    {  AnnotationType::SUBJECTIVITY, ElementType::SubjectivityAnnotation_t },
    {  AnnotationType::SYNTAX, ElementType::SyntacticUnit_t },
    {  AnnotationType::TABLE, ElementType::Table_t },
    {  AnnotationType::TERM, ElementType::Term_t },
    {  AnnotationType::TEXT, ElementType::TextContent_t },
    {  AnnotationType::HSPACE, ElementType::TextMarkupHSpace_t },
    {  AnnotationType::STYLE, ElementType::TextMarkupStyle_t },
    {  AnnotationType::TIMESEGMENT, ElementType::TimeSegment_t },
    {  AnnotationType::UTTERANCE, ElementType::Utterance_t },
    {  AnnotationType::WHITESPACE, ElementType::Whitespace_t },
    {  AnnotationType::TOKEN, ElementType::Word_t },
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
    {  AnnotationType::ETYMOLOGY, "etymology" },
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
      ELEMENT_ID = ElementType::BASE;
      ACCEPTED_DATA.insert(ElementType::XmlComment_t);
      ACCEPTED_DATA.insert(ElementType::ProcessingInstruction_t);
      ACCEPTED_DATA += {ElementType::Description_t, ElementType::Comment_t};
      ANNOTATIONTYPE = AnnotationType::NO_ANN;
      AUTH = true;
      AUTO_GENERATE_ID = false;
      HIDDEN = false;
      IMPLICITSPACE = false;
      OCCURRENCES = 0;
      OCCURRENCES_PER_SET = 0;
      OPTIONAL_ATTRIBS = Attrib::NO_ATT;
      PHONCONTAINER = false;
      PRINTABLE = false;
      REQUIRED_ATTRIBS = Attrib::NO_ATT;
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

  const int XML_PARSER_OPTIONS = XML_PARSE_NSCLEAN|XML_PARSE_HUGE|XML_PARSE_BIG_LINES;

  FoliaElement *FoliaElement::private_createElement( ElementType et ){
    switch ( et ){
    case ElementType::BASE: return new FoLiA();
    case ElementType::Text_t: return new Text();
    case ElementType::Speech_t: return new Speech();
    case ElementType::Utterance_t: return new Utterance();
    case ElementType::ForeignData_t: return new ForeignData();
    case ElementType::Entry_t: return new Entry();
    case ElementType::Example_t: return new Example();
    case ElementType::Term_t: return new Term();
    case ElementType::Definition_t: return new Definition();
    case ElementType::PhonContent_t: return new PhonContent();
    case ElementType::Word_t: return new Word();
    case ElementType::String_t: return new String();
    case ElementType::Event_t: return new Event();
    case ElementType::TimeSegment_t: return new TimeSegment();
    case ElementType::TimingLayer_t: return new TimingLayer();
    case ElementType::Sentence_t: return new Sentence();
    case ElementType::TextContent_t: return new TextContent();
    case ElementType::Linebreak_t: return new Linebreak();
    case ElementType::Whitespace_t: return new Whitespace();
    case ElementType::Comment_t: return new Comment();
    case ElementType::Figure_t: return new Figure();
    case ElementType::Caption_t: return new Caption();
    case ElementType::Label_t: return new Label();
    case ElementType::List_t: return new List();
    case ElementType::ListItem_t: return new ListItem();
    case ElementType::Paragraph_t: return new Paragraph();
    case ElementType::New_t: return new New();
    case ElementType::Original_t: return new Original();
    case ElementType::Current_t: return new Current();
    case ElementType::Suggestion_t: return new Suggestion();
    case ElementType::Head_t: return new Head();
    case ElementType::Table_t: return new Table();
    case ElementType::TableHead_t: return new TableHead();
    case ElementType::Cell_t: return new Cell();
    case ElementType::Row_t: return new Row();
    case ElementType::LangAnnotation_t: return new LangAnnotation();
    case ElementType::XmlComment_t: return new XmlComment();
    case ElementType::XmlText_t: return new XmlText();
    case ElementType::ProcessingInstruction_t: return new ProcessingInstruction();
    case ElementType::External_t: return new External();
    case ElementType::Note_t: return new Note();
    case ElementType::Reference_t: return new Reference();
    case ElementType::Description_t: return new Description();
    case ElementType::Gap_t: return new Gap();
    case ElementType::Content_t: return new Content();
    case ElementType::Metric_t: return new Metric();
    case ElementType::Division_t: return new Division();
    case ElementType::PosAnnotation_t: return new PosAnnotation();
    case ElementType::LemmaAnnotation_t: return new LemmaAnnotation();
    case ElementType::PhonologyLayer_t: return new PhonologyLayer();
    case ElementType::Phoneme_t: return new Phoneme();
    case ElementType::DomainAnnotation_t: return new DomainAnnotation();
    case ElementType::SenseAnnotation_t: return new SenseAnnotation();
    case ElementType::SyntaxLayer_t: return new SyntaxLayer();
    case ElementType::SubjectivityAnnotation_t: return new SubjectivityAnnotation();
    case ElementType::Chunk_t: return new Chunk();
    case ElementType::ChunkingLayer_t: return new ChunkingLayer();
    case ElementType::Entity_t: return new Entity();
    case ElementType::EntitiesLayer_t: return new EntitiesLayer();
    case ElementType::SemanticRolesLayer_t: return new SemanticRolesLayer();
    case ElementType::SemanticRole_t: return new SemanticRole();
    case ElementType::CoreferenceLayer_t: return new CoreferenceLayer();
    case ElementType::CoreferenceLink_t: return new CoreferenceLink();
    case ElementType::CoreferenceChain_t:  return new CoreferenceChain();
    case ElementType::Alternative_t: return new Alternative();
    case ElementType::AlternativeLayers_t: return new AlternativeLayers();
    case ElementType::SyntacticUnit_t: return new SyntacticUnit();
    case ElementType::WordReference_t: return new WordReference();
    case ElementType::Correction_t: return new Correction();
    case ElementType::ErrorDetection_t: return new ErrorDetection();
    case ElementType::MorphologyLayer_t: return new MorphologyLayer();
    case ElementType::Morpheme_t: return new Morpheme();
    case ElementType::Feature_t: return new Feature();
    case ElementType::BegindatetimeFeature_t: return new BegindatetimeFeature();
    case ElementType::EnddatetimeFeature_t: return new EnddatetimeFeature();
    case ElementType::SynsetFeature_t: return new SynsetFeature();
    case ElementType::ActorFeature_t: return new ActorFeature();
    case ElementType::HeadFeature_t: return new HeadFeature();
    case ElementType::ValueFeature_t: return new ValueFeature();
    case ElementType::TimeFeature_t: return new TimeFeature();
    case ElementType::ModalityFeature_t: return new ModalityFeature();
    case ElementType::FunctionFeature_t: return new FunctionFeature();
    case ElementType::LevelFeature_t: return new LevelFeature();
    case ElementType::FontFeature_t: return new FontFeature();
    case ElementType::SizeFeature_t: return new SizeFeature();
    case ElementType::Quote_t: return new Quote();
    case ElementType::DependenciesLayer_t: return new DependenciesLayer();
    case ElementType::Dependency_t: return new Dependency();
    case ElementType::DependencyDependent_t: return new DependencyDependent();
    case ElementType::Headspan_t: return new Headspan();
    case ElementType::SpanRelationLayer_t: return new SpanRelationLayer();
    case ElementType::SpanRelation_t: return new SpanRelation();
    case ElementType::LinkReference_t: return new LinkReference();
    case ElementType::TextMarkupString_t: return new TextMarkupString();
    case ElementType::TextMarkupGap_t: return new TextMarkupGap();
    case ElementType::TextMarkupCorrection_t: return new TextMarkupCorrection();
    case ElementType::TextMarkupError_t: return new TextMarkupError();
    case ElementType::TextMarkupStyle_t: return new TextMarkupStyle();
    case ElementType::TextMarkupHSpace_t: return new TextMarkupHSpace();
    case ElementType::TextMarkupLanguage_t: return new TextMarkupLanguage();
    case ElementType::TextMarkupWhitespace_t: return new TextMarkupWhitespace();
    case ElementType::TextMarkupReference_t: return new TextMarkupReference();
    case ElementType::Part_t: return new Part();
    case ElementType::Observation_t: return new Observation();
    case ElementType::ObservationLayer_t: return new ObservationLayer();
    case ElementType::PolarityFeature_t: return new PolarityFeature();
    case ElementType::Predicate_t: return new Predicate();
    case ElementType::Relation_t: return new Relation();
    case ElementType::Sentiment_t: return new Sentiment();
    case ElementType::Modality_t: return new Modality();
    case ElementType::SentimentLayer_t: return new SentimentLayer();
    case ElementType::ModalitiesLayer_t: return new ModalitiesLayer();
    case ElementType::Statement_t: return new Statement();
    case ElementType::StatementRelation_t: return new StatementRelation();
    case ElementType::StatementLayer_t: return new StatementLayer();
    case ElementType::StrengthFeature_t: return new StrengthFeature();
    case ElementType::StyleFeature_t: return new StyleFeature();
    case ElementType::Source_t: return new Source();
    case ElementType::Target_t: return new Target();
    case ElementType::Scope_t: return new Scope();
    case ElementType::Cue_t: return new Cue();
    case ElementType::Hiddenword_t: return new Hiddenword();
    case ElementType::Hyphbreak_t: return new Hyphbreak();
    case ElementType::EtymologyAnnotation_t: return new EtymologyAnnotation();
    case ElementType::AbstractContentAnnotation_t:
    case ElementType::AbstractHigherOrderAnnotation_t:
    case ElementType::AbstractSubtokenAnnotation_t:
    case ElementType::AbstractSpanAnnotation_t:
    case ElementType::AbstractSpanRole_t:
    case ElementType::AbstractAnnotationLayer_t:
    case ElementType::AbstractTextMarkup_t:
    case ElementType::AbstractInlineAnnotation_t:
    case ElementType::AbstractStructureElement_t:
    case ElementType::AbstractCorrectionChild_t:
    case ElementType::AbstractWord_t:
    case ElementType::AbstractFeature_t:
      throw ValueError( "you may not create an abstract node of type "
			+ TiCC::toString(int(et)) + ")" );
    default:
      throw ValueError( "private_createElement: unknown elementtype("
			+ TiCC::toString(int(et)) + ")" );
    }
    return 0;
  }

  bool is_subtype( const ElementType& e1, const ElementType& e2 ){
    /// check if an ElementType is a direct or indirect subclass of another one
    /*!
      \param e1 an ElementType
      \param e2 an ElementType
      \return true if e1 is in the typeHierarchy of e2
    */
    if ( e1 == e2 ){
      return true;
    }
    const auto& it = typeHierarchy.find( e1 );
    if ( it != typeHierarchy.end() ){
      return it->second.find( e2 ) != it->second.end();
    }
    return false;
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
    for ( auto const& [parent,child] : typeHierarchy ){
      os << toString(parent) << endl;
      for ( auto const& el : child ){
	os << "     -- " << toString(el) << endl;
      }
    }
  }

  string host_name;

  namespace {
    //
    // this trick assures that the static_init() function is called
    // exactly once on every run.
    // This because the static 'i' is initialized before main() starts.
    //
    struct initializer {
     initializer() {
       static_init();
       host_name = get_fqdn();
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
