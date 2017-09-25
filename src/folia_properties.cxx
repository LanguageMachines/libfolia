#include <set>
#include <string>
#include <iostream>

#include "libfolia/folia.h"
#include "libfolia/folia_properties.h"

//foliaspec:header
//This file was last updated according to the FoLiA specification for version 1.5 on 2017-09-25 09:28:49, using foliaspec.py
//Code blocks after a foliaspec comment (until the next newline) are automatically generated. **DO NOT EDIT THOSE** and **DO NOT REMOVE ANY FOLIASPEC COMMENTS** !!!

namespace folia {

  using namespace std;

  //foliaspec:version_major:MAJOR_VERSION
  //The FoLiA version (major)
  const int MAJOR_VERSION = 1;

  //foliaspec:version_minor:MINOR_VERSION
  //The FoLiA version (minor)
  const int MINOR_VERSION = 5;

  //foliaspec:version_sub:SUB_VERSION
  //The FoLiA version (sub/rev)
  const int SUB_VERSION = 0;

  //foliaspec:namespace:NSFOLIA
  //The FoLiA XML namespace
  const string NSFOLIA = "http://ilk.uvt.nl/folia";

  const string NSDCOI = "http://lands.let.ru.nl/projects/d-coi/ns/1.0";
  const string NSIMDI = "http://www.mpi.nl/IMDI/Schema/IMDI";

  //foliaspec:attributefeatures
  const set<string> AttributeFeatures = { "actor", "begindatetime", "enddatetime", "function", "headfeature", "level", "modality", "polarity", "strength", "style", "synset", "time", "value" };

  //foliaspec:annotationtype_string_map
  //A mapping from annotation types to strings
  const map<AnnotationType::AnnotationType,string> ant_s_map = {
    { AnnotationType::NO_ANN, "NONE" },
    { AnnotationType::ALIGNMENT,  "alignment" },
    { AnnotationType::CHUNKING,  "chunking" },
    { AnnotationType::COMPLEXALIGNMENT,  "complexalignment" },
    { AnnotationType::COREFERENCE,  "coreference" },
    { AnnotationType::CORRECTION,  "correction" },
    { AnnotationType::DEFINITION,  "definition" },
    { AnnotationType::DEPENDENCY,  "dependency" },
    { AnnotationType::DIVISION,  "division" },
    { AnnotationType::DOMAIN,  "domain" },
    { AnnotationType::ENTITY,  "entity" },
    { AnnotationType::ENTRY,  "entry" },
    { AnnotationType::ERRORDETECTION,  "errordetection" },
    { AnnotationType::EVENT,  "event" },
    { AnnotationType::EXAMPLE,  "example" },
    { AnnotationType::FIGURE,  "figure" },
    { AnnotationType::GAP,  "gap" },
    { AnnotationType::LANG,  "lang" },
    { AnnotationType::LEMMA,  "lemma" },
    { AnnotationType::LINEBREAK,  "linebreak" },
    { AnnotationType::LIST,  "list" },
    { AnnotationType::METRIC,  "metric" },
    { AnnotationType::MORPHOLOGICAL,  "morphological" },
    { AnnotationType::NOTE,  "note" },
    { AnnotationType::OBSERVATION,  "observation" },
    { AnnotationType::PARAGRAPH,  "paragraph" },
    { AnnotationType::PART,  "part" },
    { AnnotationType::PHON,  "phon" },
    { AnnotationType::PHONOLOGICAL,  "phonological" },
    { AnnotationType::POS,  "pos" },
    { AnnotationType::PREDICATE,  "predicate" },
    { AnnotationType::SEMROLE,  "semrole" },
    { AnnotationType::SENSE,  "sense" },
    { AnnotationType::SENTENCE,  "sentence" },
    { AnnotationType::SENTIMENT,  "sentiment" },
    { AnnotationType::STATEMENT,  "statement" },
    { AnnotationType::STRING,  "string" },
    { AnnotationType::SUBJECTIVITY,  "subjectivity" },
    { AnnotationType::SYNTAX,  "syntax" },
    { AnnotationType::TABLE,  "table" },
    { AnnotationType::TERM,  "term" },
    { AnnotationType::TEXT,  "text" },
    { AnnotationType::STYLE,  "style" },
    { AnnotationType::TIMESEGMENT,  "timesegment" },
    { AnnotationType::UTTERANCE,  "utterance" },
    { AnnotationType::WHITESPACE,  "whitespace" },
    { AnnotationType::TOKEN,  "token" },
  };

  //foliaspec:string_annotationtype_map
  //A mapping from strings to annotation types
  const map<string,AnnotationType::AnnotationType> s_ant_map = {
    { "NONE", AnnotationType::NO_ANN },
    { "alignment", AnnotationType::ALIGNMENT },
    { "chunking", AnnotationType::CHUNKING },
    { "complexalignment", AnnotationType::COMPLEXALIGNMENT },
    { "coreference", AnnotationType::COREFERENCE },
    { "correction", AnnotationType::CORRECTION },
    { "definition", AnnotationType::DEFINITION },
    { "dependency", AnnotationType::DEPENDENCY },
    { "division", AnnotationType::DIVISION },
    { "domain", AnnotationType::DOMAIN },
    { "entity", AnnotationType::ENTITY },
    { "entry", AnnotationType::ENTRY },
    { "errordetection", AnnotationType::ERRORDETECTION },
    { "event", AnnotationType::EVENT },
    { "example", AnnotationType::EXAMPLE },
    { "figure", AnnotationType::FIGURE },
    { "gap", AnnotationType::GAP },
    { "lang", AnnotationType::LANG },
    { "lemma", AnnotationType::LEMMA },
    { "linebreak", AnnotationType::LINEBREAK },
    { "list", AnnotationType::LIST },
    { "metric", AnnotationType::METRIC },
    { "morphological", AnnotationType::MORPHOLOGICAL },
    { "note", AnnotationType::NOTE },
    { "observation", AnnotationType::OBSERVATION },
    { "paragraph", AnnotationType::PARAGRAPH },
    { "part", AnnotationType::PART },
    { "phon", AnnotationType::PHON },
    { "phonological", AnnotationType::PHONOLOGICAL },
    { "pos", AnnotationType::POS },
    { "predicate", AnnotationType::PREDICATE },
    { "semrole", AnnotationType::SEMROLE },
    { "sense", AnnotationType::SENSE },
    { "sentence", AnnotationType::SENTENCE },
    { "sentiment", AnnotationType::SENTIMENT },
    { "statement", AnnotationType::STATEMENT },
    { "string", AnnotationType::STRING },
    { "subjectivity", AnnotationType::SUBJECTIVITY },
    { "syntax", AnnotationType::SYNTAX },
    { "table", AnnotationType::TABLE },
    { "term", AnnotationType::TERM },
    { "text", AnnotationType::TEXT },
    { "style", AnnotationType::STYLE },
    { "timesegment", AnnotationType::TIMESEGMENT },
    { "utterance", AnnotationType::UTTERANCE },
    { "whitespace", AnnotationType::WHITESPACE },
    { "token", AnnotationType::TOKEN },
  };

  //foliaspec:elementtype_string_map
  const map<ElementType,string> et_s_map = {
    { BASE, "FoLiA" },
    { AbstractAnnotationLayer_t,  "_AbstractAnnotationLayer" },
    { AbstractCorrectionChild_t,  "_AbstractCorrectionChild" },
    { AbstractExtendedTokenAnnotation_t,  "_AbstractExtendedTokenAnnotation" },
    { AbstractSpanAnnotation_t,  "_AbstractSpanAnnotation" },
    { AbstractSpanRole_t,  "_AbstractSpanRole" },
    { AbstractStructureElement_t,  "_AbstractStructureElement" },
    { AbstractTextMarkup_t,  "_AbstractTextMarkup" },
    { AbstractTokenAnnotation_t,  "_AbstractTokenAnnotation" },
    { ActorFeature_t,  "actor" },
    { AlignReference_t,  "aref" },
    { Alignment_t,  "alignment" },
    { Alternative_t,  "alt" },
    { AlternativeLayers_t,  "altlayers" },
    { BegindatetimeFeature_t,  "begindatetime" },
    { Caption_t,  "caption" },
    { Cell_t,  "cell" },
    { Chunk_t,  "chunk" },
    { ChunkingLayer_t,  "chunking" },
    { Comment_t,  "comment" },
    { ComplexAlignment_t,  "complexalignment" },
    { ComplexAlignmentLayer_t,  "complexalignments" },
    { Content_t,  "content" },
    { CoreferenceChain_t,  "coreferencechain" },
    { CoreferenceLayer_t,  "coreferences" },
    { CoreferenceLink_t,  "coreferencelink" },
    { Correction_t,  "correction" },
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
    { ForeignData_t,  "foreign-data" },
    { FunctionFeature_t,  "function" },
    { Gap_t,  "gap" },
    { Head_t,  "head" },
    { HeadFeature_t,  "headfeature" },
    { Headspan_t,  "hd" },
    { Label_t,  "label" },
    { LangAnnotation_t,  "lang" },
    { LemmaAnnotation_t,  "lemma" },
    { LevelFeature_t,  "level" },
    { Linebreak_t,  "br" },
    { List_t,  "list" },
    { ListItem_t,  "item" },
    { Metric_t,  "metric" },
    { ModalityFeature_t,  "modality" },
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
    { SemanticRole_t,  "semrole" },
    { SemanticRolesLayer_t,  "semroles" },
    { SenseAnnotation_t,  "sense" },
    { Sentence_t,  "s" },
    { Sentiment_t,  "sentiment" },
    { SentimentLayer_t,  "sentiments" },
    { Source_t,  "source" },
    { Speech_t,  "speech" },
    { Statement_t,  "statement" },
    { StatementLayer_t,  "statements" },
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
    { TextMarkupString_t,  "t-str" },
    { TextMarkupStyle_t,  "t-style" },
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
    { "_AbstractCorrectionChild", AbstractCorrectionChild_t  },
    { "_AbstractExtendedTokenAnnotation", AbstractExtendedTokenAnnotation_t  },
    { "_AbstractSpanAnnotation", AbstractSpanAnnotation_t  },
    { "_AbstractSpanRole", AbstractSpanRole_t  },
    { "_AbstractStructureElement", AbstractStructureElement_t  },
    { "_AbstractTextMarkup", AbstractTextMarkup_t  },
    { "_AbstractTokenAnnotation", AbstractTokenAnnotation_t  },
    { "actor", ActorFeature_t  },
    { "aref", AlignReference_t  },
    { "alignment", Alignment_t  },
    { "alt", Alternative_t  },
    { "altlayers", AlternativeLayers_t  },
    { "begindatetime", BegindatetimeFeature_t  },
    { "caption", Caption_t  },
    { "cell", Cell_t  },
    { "chunk", Chunk_t  },
    { "chunking", ChunkingLayer_t  },
    { "comment", Comment_t  },
    { "complexalignment", ComplexAlignment_t  },
    { "complexalignments", ComplexAlignmentLayer_t  },
    { "content", Content_t  },
    { "coreferencechain", CoreferenceChain_t  },
    { "coreferences", CoreferenceLayer_t  },
    { "coreferencelink", CoreferenceLink_t  },
    { "correction", Correction_t  },
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
    { "foreign-data", ForeignData_t  },
    { "function", FunctionFeature_t  },
    { "gap", Gap_t  },
    { "head", Head_t  },
    { "headfeature", HeadFeature_t },
    { "hd", Headspan_t  },
    { "label", Label_t  },
    { "lang", LangAnnotation_t  },
    { "lemma", LemmaAnnotation_t  },
    { "level", LevelFeature_t  },
    { "br", Linebreak_t  },
    { "list", List_t  },
    { "item", ListItem_t  },
    { "metric", Metric_t  },
    { "modality", ModalityFeature_t  },
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
    { "semrole", SemanticRole_t  },
    { "semroles", SemanticRolesLayer_t  },
    { "sense", SenseAnnotation_t  },
    { "s", Sentence_t  },
    { "sentiment", Sentiment_t  },
    { "sentiments", SentimentLayer_t  },
    { "source", Source_t  },
    { "speech", Speech_t  },
    { "statement", Statement_t  },
    { "statements", StatementLayer_t  },
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
    { "t-str", TextMarkupString_t  },
    { "t-style", TextMarkupStyle_t  },
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
  const set<ElementType> default_ignore_structure = { Alternative_t, AlternativeLayers_t, ChunkingLayer_t, ComplexAlignmentLayer_t, CoreferenceLayer_t, DependenciesLayer_t, EntitiesLayer_t, MorphologyLayer_t, ObservationLayer_t, Original_t, PhonologyLayer_t, SemanticRolesLayer_t, SentimentLayer_t, StatementLayer_t, Suggestion_t, SyntaxLayer_t, TimingLayer_t };

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
  properties AbstractCorrectionChild::PROPS = DEFAULT_PROPERTIES;
  properties AbstractExtendedTokenAnnotation::PROPS = DEFAULT_PROPERTIES;
  properties AbstractSpanAnnotation::PROPS = DEFAULT_PROPERTIES;
  properties AbstractSpanRole::PROPS = DEFAULT_PROPERTIES;
  properties AbstractStructureElement::PROPS = DEFAULT_PROPERTIES;
  properties AbstractTextMarkup::PROPS = DEFAULT_PROPERTIES;
  properties AbstractTokenAnnotation::PROPS = DEFAULT_PROPERTIES;
  properties ActorFeature::PROPS = DEFAULT_PROPERTIES;
  properties AlignReference::PROPS = DEFAULT_PROPERTIES;
  properties Alignment::PROPS = DEFAULT_PROPERTIES;
  properties Alternative::PROPS = DEFAULT_PROPERTIES;
  properties AlternativeLayers::PROPS = DEFAULT_PROPERTIES;
  properties BegindatetimeFeature::PROPS = DEFAULT_PROPERTIES;
  properties Caption::PROPS = DEFAULT_PROPERTIES;
  properties Cell::PROPS = DEFAULT_PROPERTIES;
  properties Chunk::PROPS = DEFAULT_PROPERTIES;
  properties ChunkingLayer::PROPS = DEFAULT_PROPERTIES;
  properties Comment::PROPS = DEFAULT_PROPERTIES;
  properties ComplexAlignment::PROPS = DEFAULT_PROPERTIES;
  properties ComplexAlignmentLayer::PROPS = DEFAULT_PROPERTIES;
  properties Content::PROPS = DEFAULT_PROPERTIES;
  properties CoreferenceChain::PROPS = DEFAULT_PROPERTIES;
  properties CoreferenceLayer::PROPS = DEFAULT_PROPERTIES;
  properties CoreferenceLink::PROPS = DEFAULT_PROPERTIES;
  properties Correction::PROPS = DEFAULT_PROPERTIES;
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
  properties ForeignData::PROPS = DEFAULT_PROPERTIES;
  properties FunctionFeature::PROPS = DEFAULT_PROPERTIES;
  properties Gap::PROPS = DEFAULT_PROPERTIES;
  properties Head::PROPS = DEFAULT_PROPERTIES;
  properties HeadFeature::PROPS = DEFAULT_PROPERTIES;
  properties Headspan::PROPS = DEFAULT_PROPERTIES;
  properties Label::PROPS = DEFAULT_PROPERTIES;
  properties LangAnnotation::PROPS = DEFAULT_PROPERTIES;
  properties LemmaAnnotation::PROPS = DEFAULT_PROPERTIES;
  properties LevelFeature::PROPS = DEFAULT_PROPERTIES;
  properties Linebreak::PROPS = DEFAULT_PROPERTIES;
  properties List::PROPS = DEFAULT_PROPERTIES;
  properties ListItem::PROPS = DEFAULT_PROPERTIES;
  properties Metric::PROPS = DEFAULT_PROPERTIES;
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
  properties SemanticRole::PROPS = DEFAULT_PROPERTIES;
  properties SemanticRolesLayer::PROPS = DEFAULT_PROPERTIES;
  properties SenseAnnotation::PROPS = DEFAULT_PROPERTIES;
  properties Sentence::PROPS = DEFAULT_PROPERTIES;
  properties Sentiment::PROPS = DEFAULT_PROPERTIES;
  properties SentimentLayer::PROPS = DEFAULT_PROPERTIES;
  properties Source::PROPS = DEFAULT_PROPERTIES;
  properties Speech::PROPS = DEFAULT_PROPERTIES;
  properties Statement::PROPS = DEFAULT_PROPERTIES;
  properties StatementLayer::PROPS = DEFAULT_PROPERTIES;
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
  properties TextMarkupString::PROPS = DEFAULT_PROPERTIES;
  properties TextMarkupStyle::PROPS = DEFAULT_PROPERTIES;
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

  void static_init(){
    FoLiA::PROPS.XMLTAG = "FoLiA";
    FoLiA::PROPS.ACCEPTED_DATA += { Text_t, Speech_t };

    DCOI::PROPS.XMLTAG = "DCOI";
    DCOI::PROPS.ACCEPTED_DATA += { Text_t, Speech_t };


    //foliaspec:begin:setelementproperties
    //Sets all element properties for all elements
//------ AbstractAnnotationLayer -------
    AbstractAnnotationLayer::PROPS.ELEMENT_ID = AbstractAnnotationLayer_t;
    AbstractAnnotationLayer::PROPS.ACCEPTED_DATA += {Comment_t, Correction_t, Description_t, ForeignData_t};
    AbstractAnnotationLayer::PROPS.OPTIONAL_ATTRIBS = ID|ANNOTATOR|CONFIDENCE|DATETIME|N|TEXTCLASS|METADATA;
    AbstractAnnotationLayer::PROPS.PRINTABLE = false;
    AbstractAnnotationLayer::PROPS.SETONLY = true;
    AbstractAnnotationLayer::PROPS.SPEAKABLE = false;
//------ AbstractCorrectionChild -------
    AbstractCorrectionChild::PROPS.ELEMENT_ID = AbstractCorrectionChild_t;
    AbstractCorrectionChild::PROPS.ACCEPTED_DATA += {AbstractSpanAnnotation_t, AbstractStructureElement_t, AbstractTokenAnnotation_t, Comment_t, Correction_t, Description_t, ForeignData_t, Metric_t, PhonContent_t, String_t, TextContent_t};
    AbstractCorrectionChild::PROPS.OPTIONAL_ATTRIBS = ID|ANNOTATOR|CONFIDENCE|DATETIME|N;
    AbstractCorrectionChild::PROPS.PRINTABLE = true;
    AbstractCorrectionChild::PROPS.SPEAKABLE = true;
    AbstractCorrectionChild::PROPS.TEXTDELIMITER = "NONE";
//------ AbstractSpanAnnotation -------
    AbstractSpanAnnotation::PROPS.ELEMENT_ID = AbstractSpanAnnotation_t;
    AbstractSpanAnnotation::PROPS.ACCEPTED_DATA += {AlignReference_t, Alignment_t, Comment_t, Description_t, ForeignData_t, Metric_t};
    AbstractSpanAnnotation::PROPS.OPTIONAL_ATTRIBS = ID|CLASS|ANNOTATOR|N|CONFIDENCE|DATETIME|SRC|BEGINTIME|ENDTIME|SPEAKER|TEXTCLASS|METADATA;
    AbstractSpanAnnotation::PROPS.PRINTABLE = true;
    AbstractSpanAnnotation::PROPS.SPEAKABLE = true;
//------ AbstractSpanRole -------
    AbstractSpanRole::PROPS = AbstractSpanAnnotation::PROPS;
    AbstractSpanRole::PROPS.ELEMENT_ID = AbstractSpanRole_t;
    AbstractSpanRole::PROPS.ACCEPTED_DATA += {AlignReference_t, Alignment_t, Comment_t, Description_t, Feature_t, ForeignData_t, Metric_t, WordReference_t, Word_t, Morpheme_t, Phoneme_t};
    AbstractSpanRole::PROPS.OPTIONAL_ATTRIBS = ID|ANNOTATOR|N|DATETIME;
//------ AbstractStructureElement -------
    AbstractStructureElement::PROPS.ELEMENT_ID = AbstractStructureElement_t;
    AbstractStructureElement::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, Alignment_t, Alternative_t, AlternativeLayers_t, Comment_t, Correction_t, Description_t, Feature_t, ForeignData_t, Metric_t, Part_t};
    AbstractStructureElement::PROPS.AUTO_GENERATE_ID = true;
    AbstractStructureElement::PROPS.OPTIONAL_ATTRIBS = ID|CLASS|ANNOTATOR|N|CONFIDENCE|DATETIME|SRC|BEGINTIME|ENDTIME|SPEAKER|METADATA;
    AbstractStructureElement::PROPS.PRINTABLE = true;
    AbstractStructureElement::PROPS.REQUIRED_ATTRIBS = NO_ATT;
    AbstractStructureElement::PROPS.SPEAKABLE = true;
    AbstractStructureElement::PROPS.TEXTDELIMITER = "\n\n";
//------ AbstractTextMarkup -------
    AbstractTextMarkup::PROPS.ELEMENT_ID = AbstractTextMarkup_t;
    AbstractTextMarkup::PROPS.ACCEPTED_DATA += {AbstractTextMarkup_t, Comment_t, Description_t, Linebreak_t, XmlText_t};
    AbstractTextMarkup::PROPS.OPTIONAL_ATTRIBS = ID|CLASS|ANNOTATOR|N|CONFIDENCE|DATETIME|SRC|BEGINTIME|ENDTIME|SPEAKER|METADATA;
    AbstractTextMarkup::PROPS.PRINTABLE = true;
    AbstractTextMarkup::PROPS.TEXTCONTAINER = true;
    AbstractTextMarkup::PROPS.TEXTDELIMITER = "";
    AbstractTextMarkup::PROPS.XLINK = true;
//------ AbstractTokenAnnotation -------
    AbstractTokenAnnotation::PROPS.ELEMENT_ID = AbstractTokenAnnotation_t;
    AbstractTokenAnnotation::PROPS.ACCEPTED_DATA += {Comment_t, Description_t, Feature_t, ForeignData_t, Metric_t};
    AbstractTokenAnnotation::PROPS.OCCURRENCES_PER_SET = 1;
    AbstractTokenAnnotation::PROPS.OPTIONAL_ATTRIBS = ID|CLASS|ANNOTATOR|N|CONFIDENCE|DATETIME|SRC|BEGINTIME|ENDTIME|SPEAKER|TEXTCLASS|METADATA;
    AbstractTokenAnnotation::PROPS.REQUIRED_ATTRIBS = CLASS;
//------ AbstractExtendedTokenAnnotation -------
    AbstractExtendedTokenAnnotation::PROPS = AbstractTokenAnnotation::PROPS;
    AbstractExtendedTokenAnnotation::PROPS.ELEMENT_ID = AbstractExtendedTokenAnnotation_t;
//------ AlignReference -------
    AlignReference::PROPS.ELEMENT_ID = AlignReference_t;
    AlignReference::PROPS.XMLTAG = "aref";
//------ Alignment -------
    Alignment::PROPS.ELEMENT_ID = Alignment_t;
    Alignment::PROPS.ACCEPTED_DATA += {AlignReference_t, Comment_t, Description_t, Feature_t, ForeignData_t, Metric_t};
    Alignment::PROPS.ANNOTATIONTYPE = AnnotationType::ALIGNMENT;
    Alignment::PROPS.LABEL = "Alignment";
    Alignment::PROPS.OPTIONAL_ATTRIBS = ID|CLASS|ANNOTATOR|N|CONFIDENCE|DATETIME|SRC|BEGINTIME|ENDTIME|SPEAKER|METADATA;
    Alignment::PROPS.PRINTABLE = false;
    Alignment::PROPS.REQUIRED_ATTRIBS = NO_ATT;
    Alignment::PROPS.SPEAKABLE = false;
    Alignment::PROPS.XLINK = true;
    Alignment::PROPS.XMLTAG = "alignment";
//------ Alternative -------
    Alternative::PROPS.ELEMENT_ID = Alternative_t;
    Alternative::PROPS.ACCEPTED_DATA += {AbstractTokenAnnotation_t, Comment_t, Correction_t, Description_t, ForeignData_t, MorphologyLayer_t, PhonologyLayer_t};
    Alternative::PROPS.AUTH = false;
    Alternative::PROPS.LABEL = "Alternative";
    Alternative::PROPS.OPTIONAL_ATTRIBS = ID|CLASS|ANNOTATOR|N|CONFIDENCE|DATETIME|SRC|BEGINTIME|ENDTIME|SPEAKER|METADATA;
    Alternative::PROPS.PRINTABLE = false;
    Alternative::PROPS.REQUIRED_ATTRIBS = NO_ATT;
    Alternative::PROPS.SPEAKABLE = false;
    Alternative::PROPS.XMLTAG = "alt";
//------ AlternativeLayers -------
    AlternativeLayers::PROPS.ELEMENT_ID = AlternativeLayers_t;
    AlternativeLayers::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, Comment_t, Description_t, ForeignData_t};
    AlternativeLayers::PROPS.AUTH = false;
    AlternativeLayers::PROPS.LABEL = "Alternative Layers";
    AlternativeLayers::PROPS.OPTIONAL_ATTRIBS = ID|CLASS|ANNOTATOR|N|CONFIDENCE|DATETIME|SRC|BEGINTIME|ENDTIME|SPEAKER|METADATA;
    AlternativeLayers::PROPS.PRINTABLE = false;
    AlternativeLayers::PROPS.REQUIRED_ATTRIBS = NO_ATT;
    AlternativeLayers::PROPS.SPEAKABLE = false;
    AlternativeLayers::PROPS.XMLTAG = "altlayers";
//------ Caption -------
    Caption::PROPS = AbstractStructureElement::PROPS;
    Caption::PROPS.ELEMENT_ID = Caption_t;
    Caption::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, AbstractExtendedTokenAnnotation_t, Alignment_t, Alternative_t, AlternativeLayers_t, Comment_t, Correction_t, Description_t, Feature_t, ForeignData_t, Gap_t, Linebreak_t, Metric_t, Part_t, PhonContent_t, Reference_t, Sentence_t, String_t, TextContent_t, Whitespace_t};
    Caption::PROPS.LABEL = "Caption";
    Caption::PROPS.OCCURRENCES = 1;
    Caption::PROPS.XMLTAG = "caption";
//------ Cell -------
    Cell::PROPS = AbstractStructureElement::PROPS;
    Cell::PROPS.ELEMENT_ID = Cell_t;
    Cell::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, AbstractExtendedTokenAnnotation_t, Alignment_t, Alternative_t, AlternativeLayers_t, Comment_t, Correction_t, Description_t, Entry_t, Event_t, Example_t, Feature_t, ForeignData_t, Gap_t, Head_t, Linebreak_t, Metric_t, Note_t, Paragraph_t, Part_t, Reference_t, Sentence_t, String_t, TextContent_t, Whitespace_t, Word_t};
    Cell::PROPS.LABEL = "Cell";
    Cell::PROPS.TEXTDELIMITER = " | ";
    Cell::PROPS.XMLTAG = "cell";
//------ Chunk -------
    Chunk::PROPS = AbstractSpanAnnotation::PROPS;
    Chunk::PROPS.ELEMENT_ID = Chunk_t;
    Chunk::PROPS.ACCEPTED_DATA += {AlignReference_t, Alignment_t, Comment_t, Description_t, Feature_t, ForeignData_t, Metric_t, WordReference_t, Word_t, Morpheme_t, Phoneme_t};
    Chunk::PROPS.ANNOTATIONTYPE = AnnotationType::CHUNKING;
    Chunk::PROPS.LABEL = "Chunk";
    Chunk::PROPS.XMLTAG = "chunk";
//------ ChunkingLayer -------
    ChunkingLayer::PROPS = AbstractAnnotationLayer::PROPS;
    ChunkingLayer::PROPS.ELEMENT_ID = ChunkingLayer_t;
    ChunkingLayer::PROPS.ACCEPTED_DATA += {Chunk_t, Comment_t, Correction_t, Description_t, ForeignData_t};
    ChunkingLayer::PROPS.ANNOTATIONTYPE = AnnotationType::CHUNKING;
    ChunkingLayer::PROPS.XMLTAG = "chunking";
//------ Comment -------
    Comment::PROPS.ELEMENT_ID = Comment_t;
    Comment::PROPS.LABEL = "Comment";
    Comment::PROPS.OPTIONAL_ATTRIBS = ID|ANNOTATOR|CONFIDENCE|DATETIME|N|METADATA;
    Comment::PROPS.XMLTAG = "comment";
//------ ComplexAlignment -------
    ComplexAlignment::PROPS.ELEMENT_ID = ComplexAlignment_t;
    ComplexAlignment::PROPS.ACCEPTED_DATA += {Alignment_t, Comment_t, Description_t, Feature_t, ForeignData_t, Metric_t};
    ComplexAlignment::PROPS.ANNOTATIONTYPE = AnnotationType::COMPLEXALIGNMENT;
    ComplexAlignment::PROPS.LABEL = "Complex Alignment";
    ComplexAlignment::PROPS.OPTIONAL_ATTRIBS = ID|CLASS|ANNOTATOR|N|CONFIDENCE|DATETIME|SRC|BEGINTIME|ENDTIME|SPEAKER|METADATA;
    ComplexAlignment::PROPS.PRINTABLE = false;
    ComplexAlignment::PROPS.REQUIRED_ATTRIBS = NO_ATT;
    ComplexAlignment::PROPS.SPEAKABLE = false;
    ComplexAlignment::PROPS.XMLTAG = "complexalignment";
//------ ComplexAlignmentLayer -------
    ComplexAlignmentLayer::PROPS = AbstractAnnotationLayer::PROPS;
    ComplexAlignmentLayer::PROPS.ELEMENT_ID = ComplexAlignmentLayer_t;
    ComplexAlignmentLayer::PROPS.ACCEPTED_DATA += {Comment_t, ComplexAlignment_t, Correction_t, Description_t, ForeignData_t};
    ComplexAlignmentLayer::PROPS.ANNOTATIONTYPE = AnnotationType::COMPLEXALIGNMENT;
    ComplexAlignmentLayer::PROPS.XMLTAG = "complexalignments";
//------ Content -------
    Content::PROPS.ELEMENT_ID = Content_t;
    Content::PROPS.LABEL = "Gap Content";
    Content::PROPS.OCCURRENCES = 1;
    Content::PROPS.XMLTAG = "content";
//------ CoreferenceChain -------
    CoreferenceChain::PROPS = AbstractSpanAnnotation::PROPS;
    CoreferenceChain::PROPS.ELEMENT_ID = CoreferenceChain_t;
    CoreferenceChain::PROPS.ACCEPTED_DATA += {AlignReference_t, Alignment_t, Comment_t, CoreferenceLink_t, Description_t, Feature_t, ForeignData_t, Metric_t};
    CoreferenceChain::PROPS.ANNOTATIONTYPE = AnnotationType::COREFERENCE;
    CoreferenceChain::PROPS.LABEL = "Coreference Chain";
    CoreferenceChain::PROPS.REQUIRED_DATA += {CoreferenceLink_t};
    CoreferenceChain::PROPS.XMLTAG = "coreferencechain";
//------ CoreferenceLayer -------
    CoreferenceLayer::PROPS = AbstractAnnotationLayer::PROPS;
    CoreferenceLayer::PROPS.ELEMENT_ID = CoreferenceLayer_t;
    CoreferenceLayer::PROPS.ACCEPTED_DATA += {Comment_t, CoreferenceChain_t, Correction_t, Description_t, ForeignData_t};
    CoreferenceLayer::PROPS.ANNOTATIONTYPE = AnnotationType::COREFERENCE;
    CoreferenceLayer::PROPS.XMLTAG = "coreferences";
//------ CoreferenceLink -------
    CoreferenceLink::PROPS = AbstractSpanRole::PROPS;
    CoreferenceLink::PROPS.ELEMENT_ID = CoreferenceLink_t;
    CoreferenceLink::PROPS.ACCEPTED_DATA += {AlignReference_t, Alignment_t, Comment_t, Description_t, Feature_t, ForeignData_t, Headspan_t, LevelFeature_t, Metric_t, ModalityFeature_t, TimeFeature_t, WordReference_t, Word_t, Morpheme_t, Phoneme_t};
    CoreferenceLink::PROPS.ANNOTATIONTYPE = AnnotationType::COREFERENCE;
    CoreferenceLink::PROPS.LABEL = "Coreference Link";
    CoreferenceLink::PROPS.XMLTAG = "coreferencelink";
//------ Correction -------
    Correction::PROPS.ELEMENT_ID = Correction_t;
    Correction::PROPS.ACCEPTED_DATA += {Comment_t, Current_t, Description_t, ErrorDetection_t, Feature_t, ForeignData_t, Metric_t, New_t, Original_t, Suggestion_t};
    Correction::PROPS.ANNOTATIONTYPE = AnnotationType::CORRECTION;
    Correction::PROPS.LABEL = "Correction";
    Correction::PROPS.OPTIONAL_ATTRIBS = ID|CLASS|ANNOTATOR|N|CONFIDENCE|DATETIME|SRC|BEGINTIME|ENDTIME|SPEAKER|METADATA;
    Correction::PROPS.PRINTABLE = true;
    Correction::PROPS.SPEAKABLE = true;
    Correction::PROPS.TEXTDELIMITER = "NONE";
    Correction::PROPS.XMLTAG = "correction";
//------ Current -------
    Current::PROPS = AbstractCorrectionChild::PROPS;
    Current::PROPS.ELEMENT_ID = Current_t;
    Current::PROPS.OCCURRENCES = 1;
    Current::PROPS.OPTIONAL_ATTRIBS = NO_ATT;
    Current::PROPS.XMLTAG = "current";
//------ Definition -------
    Definition::PROPS = AbstractStructureElement::PROPS;
    Definition::PROPS.ELEMENT_ID = Definition_t;
    Definition::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, AbstractExtendedTokenAnnotation_t, Alignment_t, Alternative_t, AlternativeLayers_t, Comment_t, Correction_t, Description_t, Feature_t, Figure_t, ForeignData_t, List_t, Metric_t, Paragraph_t, Part_t, PhonContent_t, Reference_t, Sentence_t, String_t, Table_t, TextContent_t, Utterance_t, Word_t};
    Definition::PROPS.ANNOTATIONTYPE = AnnotationType::DEFINITION;
    Definition::PROPS.LABEL = "Definition";
    Definition::PROPS.XMLTAG = "def";
//------ DependenciesLayer -------
    DependenciesLayer::PROPS = AbstractAnnotationLayer::PROPS;
    DependenciesLayer::PROPS.ELEMENT_ID = DependenciesLayer_t;
    DependenciesLayer::PROPS.ACCEPTED_DATA += {Comment_t, Correction_t, Dependency_t, Description_t, ForeignData_t};
    DependenciesLayer::PROPS.ANNOTATIONTYPE = AnnotationType::DEPENDENCY;
    DependenciesLayer::PROPS.XMLTAG = "dependencies";
//------ Dependency -------
    Dependency::PROPS = AbstractSpanAnnotation::PROPS;
    Dependency::PROPS.ELEMENT_ID = Dependency_t;
    Dependency::PROPS.ACCEPTED_DATA += {AlignReference_t, Alignment_t, Comment_t, DependencyDependent_t, Description_t, Feature_t, ForeignData_t, Headspan_t, Metric_t};
    Dependency::PROPS.ANNOTATIONTYPE = AnnotationType::DEPENDENCY;
    Dependency::PROPS.LABEL = "Dependency";
    Dependency::PROPS.REQUIRED_DATA += {DependencyDependent_t, Headspan_t};
    Dependency::PROPS.XMLTAG = "dependency";
//------ DependencyDependent -------
    DependencyDependent::PROPS = AbstractSpanRole::PROPS;
    DependencyDependent::PROPS.ELEMENT_ID = DependencyDependent_t;
    DependencyDependent::PROPS.LABEL = "Dependent";
    DependencyDependent::PROPS.OCCURRENCES = 1;
    DependencyDependent::PROPS.XMLTAG = "dep";
//------ Description -------
    Description::PROPS.ELEMENT_ID = Description_t;
    Description::PROPS.LABEL = "Description";
    Description::PROPS.OCCURRENCES = 1;
    Description::PROPS.OPTIONAL_ATTRIBS = ID|ANNOTATOR|CONFIDENCE|DATETIME|N|METADATA;
    Description::PROPS.XMLTAG = "desc";
//------ Division -------
    Division::PROPS = AbstractStructureElement::PROPS;
    Division::PROPS.ELEMENT_ID = Division_t;
    Division::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, AbstractExtendedTokenAnnotation_t, Alignment_t, Alternative_t, AlternativeLayers_t, Comment_t, Correction_t, Description_t, Division_t, Entry_t, Event_t, Example_t, Feature_t, Figure_t, ForeignData_t, Gap_t, Head_t, Linebreak_t, List_t, Metric_t, Note_t, Paragraph_t, Part_t, PhonContent_t, Quote_t, Reference_t, Sentence_t, Table_t, TextContent_t, Utterance_t, Whitespace_t};
    Division::PROPS.ANNOTATIONTYPE = AnnotationType::DIVISION;
    Division::PROPS.LABEL = "Division";
    Division::PROPS.TEXTDELIMITER = "\n\n\n";
    Division::PROPS.XMLTAG = "div";
//------ DomainAnnotation -------
    DomainAnnotation::PROPS = AbstractExtendedTokenAnnotation::PROPS;
    DomainAnnotation::PROPS.ELEMENT_ID = DomainAnnotation_t;
    DomainAnnotation::PROPS.ANNOTATIONTYPE = AnnotationType::DOMAIN;
    DomainAnnotation::PROPS.LABEL = "Domain";
    DomainAnnotation::PROPS.OCCURRENCES_PER_SET = 0;
    DomainAnnotation::PROPS.XMLTAG = "domain";
//------ EntitiesLayer -------
    EntitiesLayer::PROPS = AbstractAnnotationLayer::PROPS;
    EntitiesLayer::PROPS.ELEMENT_ID = EntitiesLayer_t;
    EntitiesLayer::PROPS.ACCEPTED_DATA += {Comment_t, Correction_t, Description_t, Entity_t, ForeignData_t};
    EntitiesLayer::PROPS.ANNOTATIONTYPE = AnnotationType::ENTITY;
    EntitiesLayer::PROPS.XMLTAG = "entities";
//------ Entity -------
    Entity::PROPS = AbstractSpanAnnotation::PROPS;
    Entity::PROPS.ELEMENT_ID = Entity_t;
    Entity::PROPS.ACCEPTED_DATA += {AlignReference_t, Alignment_t, Comment_t, Description_t, Feature_t, ForeignData_t, Metric_t, WordReference_t, Word_t, Morpheme_t, Phoneme_t};
    Entity::PROPS.ANNOTATIONTYPE = AnnotationType::ENTITY;
    Entity::PROPS.LABEL = "Entity";
    Entity::PROPS.XMLTAG = "entity";
//------ Entry -------
    Entry::PROPS = AbstractStructureElement::PROPS;
    Entry::PROPS.ELEMENT_ID = Entry_t;
    Entry::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, Alignment_t, Alternative_t, AlternativeLayers_t, Comment_t, Correction_t, Definition_t, Description_t, Example_t, Feature_t, ForeignData_t, Metric_t, Part_t, Term_t};
    Entry::PROPS.ANNOTATIONTYPE = AnnotationType::ENTRY;
    Entry::PROPS.LABEL = "Entry";
    Entry::PROPS.XMLTAG = "entry";
//------ ErrorDetection -------
    ErrorDetection::PROPS = AbstractExtendedTokenAnnotation::PROPS;
    ErrorDetection::PROPS.ELEMENT_ID = ErrorDetection_t;
    ErrorDetection::PROPS.ANNOTATIONTYPE = AnnotationType::ERRORDETECTION;
    ErrorDetection::PROPS.LABEL = "Error Detection";
    ErrorDetection::PROPS.OCCURRENCES_PER_SET = 0;
    ErrorDetection::PROPS.XMLTAG = "errordetection";
//------ Event -------
    Event::PROPS = AbstractStructureElement::PROPS;
    Event::PROPS.ELEMENT_ID = Event_t;
    Event::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, AbstractExtendedTokenAnnotation_t, ActorFeature_t, Alignment_t, Alternative_t, AlternativeLayers_t, BegindatetimeFeature_t, Comment_t, Correction_t, Description_t, Division_t, EnddatetimeFeature_t, Entry_t, Event_t, Example_t, Feature_t, Figure_t, ForeignData_t, Gap_t, Head_t, Linebreak_t, List_t, Metric_t, Note_t, Paragraph_t, Part_t, PhonContent_t, Quote_t, Reference_t, Sentence_t, String_t, Table_t, TextContent_t, Utterance_t, Whitespace_t, Word_t};
    Event::PROPS.ANNOTATIONTYPE = AnnotationType::EVENT;
    Event::PROPS.LABEL = "Event";
    Event::PROPS.XMLTAG = "event";
//------ Example -------
    Example::PROPS = AbstractStructureElement::PROPS;
    Example::PROPS.ELEMENT_ID = Example_t;
    Example::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, AbstractExtendedTokenAnnotation_t, Alignment_t, Alternative_t, AlternativeLayers_t, Comment_t, Correction_t, Description_t, Feature_t, Figure_t, ForeignData_t, Linebreak_t, List_t, Metric_t, Paragraph_t, Part_t, PhonContent_t, Reference_t, Sentence_t, String_t, Table_t, TextContent_t, Utterance_t, Whitespace_t, Word_t};
    Example::PROPS.ANNOTATIONTYPE = AnnotationType::EXAMPLE;
    Example::PROPS.LABEL = "Example";
    Example::PROPS.XMLTAG = "ex";
//------ External -------
    External::PROPS.ELEMENT_ID = External_t;
    External::PROPS.ACCEPTED_DATA += {Comment_t, Description_t};
    External::PROPS.AUTH = true;
    External::PROPS.LABEL = "External";
    External::PROPS.OPTIONAL_ATTRIBS = NO_ATT;
    External::PROPS.PRINTABLE = true;
    External::PROPS.REQUIRED_ATTRIBS = SRC;
    External::PROPS.SPEAKABLE = false;
    External::PROPS.XMLTAG = "external";
//------ Feature -------
    Feature::PROPS.ELEMENT_ID = Feature_t;
    Feature::PROPS.LABEL = "Feature";
    Feature::PROPS.XMLTAG = "feat";
//------ ActorFeature -------
    ActorFeature::PROPS = Feature::PROPS;
    ActorFeature::PROPS.ELEMENT_ID = ActorFeature_t;
    ActorFeature::PROPS.SUBSET = "actor";
    ActorFeature::PROPS.XMLTAG = "actor";
//------ BegindatetimeFeature -------
    BegindatetimeFeature::PROPS = Feature::PROPS;
    BegindatetimeFeature::PROPS.ELEMENT_ID = BegindatetimeFeature_t;
    BegindatetimeFeature::PROPS.SUBSET = "begindatetime";
    BegindatetimeFeature::PROPS.XMLTAG = "begindatetime";
//------ EnddatetimeFeature -------
    EnddatetimeFeature::PROPS = Feature::PROPS;
    EnddatetimeFeature::PROPS.ELEMENT_ID = EnddatetimeFeature_t;
    EnddatetimeFeature::PROPS.SUBSET = "enddatetime";
    EnddatetimeFeature::PROPS.XMLTAG = "enddatetime";
//------ Figure -------
    Figure::PROPS = AbstractStructureElement::PROPS;
    Figure::PROPS.ELEMENT_ID = Figure_t;
    Figure::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, Alignment_t, Alternative_t, AlternativeLayers_t, Caption_t, Comment_t, Correction_t, Description_t, Feature_t, ForeignData_t, Metric_t, Part_t, Sentence_t, String_t, TextContent_t};
    Figure::PROPS.ANNOTATIONTYPE = AnnotationType::FIGURE;
    Figure::PROPS.LABEL = "Figure";
    Figure::PROPS.SPEAKABLE = false;
    Figure::PROPS.TEXTDELIMITER = "\n\n";
    Figure::PROPS.XMLTAG = "figure";
//------ ForeignData -------
    ForeignData::PROPS.ELEMENT_ID = ForeignData_t;
    ForeignData::PROPS.XMLTAG = "foreign-data";
//------ FunctionFeature -------
    FunctionFeature::PROPS = Feature::PROPS;
    FunctionFeature::PROPS.ELEMENT_ID = FunctionFeature_t;
    FunctionFeature::PROPS.SUBSET = "function";
    FunctionFeature::PROPS.XMLTAG = "function";
//------ Gap -------
    Gap::PROPS.ELEMENT_ID = Gap_t;
    Gap::PROPS.ACCEPTED_DATA += {Comment_t, Content_t, Description_t, Feature_t, ForeignData_t, Metric_t, Part_t};
    Gap::PROPS.ANNOTATIONTYPE = AnnotationType::GAP;
    Gap::PROPS.LABEL = "Gap";
    Gap::PROPS.OPTIONAL_ATTRIBS = ID|CLASS|ANNOTATOR|N|DATETIME|SRC|BEGINTIME|ENDTIME|METADATA;
    Gap::PROPS.XMLTAG = "gap";
//------ Head -------
    Head::PROPS = AbstractStructureElement::PROPS;
    Head::PROPS.ELEMENT_ID = Head_t;
    Head::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, AbstractExtendedTokenAnnotation_t, Alignment_t, Alternative_t, AlternativeLayers_t, Comment_t, Correction_t, Description_t, Event_t, Feature_t, ForeignData_t, Gap_t, Linebreak_t, Metric_t, Part_t, PhonContent_t, Reference_t, Sentence_t, String_t, TextContent_t, Whitespace_t, Word_t};
    Head::PROPS.LABEL = "Head";
    Head::PROPS.OCCURRENCES = 1;
    Head::PROPS.TEXTDELIMITER = "\n\n";
    Head::PROPS.XMLTAG = "head";
//------ HeadFeature -------
    HeadFeature::PROPS = Feature::PROPS;
    HeadFeature::PROPS.ELEMENT_ID = HeadFeature_t;
    HeadFeature::PROPS.SUBSET = "head";
    HeadFeature::PROPS.XMLTAG = "head";
//------ Headspan -------
    Headspan::PROPS = AbstractSpanRole::PROPS;
    Headspan::PROPS.ELEMENT_ID = Headspan_t;
    Headspan::PROPS.LABEL = "Head";
    Headspan::PROPS.OCCURRENCES = 1;
    Headspan::PROPS.XMLTAG = "hd";
//------ Label -------
    Label::PROPS = AbstractStructureElement::PROPS;
    Label::PROPS.ELEMENT_ID = Label_t;
    Label::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, AbstractExtendedTokenAnnotation_t, Alignment_t, Alternative_t, AlternativeLayers_t, Comment_t, Correction_t, Description_t, Feature_t, ForeignData_t, Metric_t, Part_t, PhonContent_t, Reference_t, String_t, TextContent_t, Word_t};
    Label::PROPS.LABEL = "Label";
    Label::PROPS.XMLTAG = "label";
//------ LangAnnotation -------
    LangAnnotation::PROPS = AbstractExtendedTokenAnnotation::PROPS;
    LangAnnotation::PROPS.ELEMENT_ID = LangAnnotation_t;
    LangAnnotation::PROPS.ANNOTATIONTYPE = AnnotationType::LANG;
    LangAnnotation::PROPS.LABEL = "Language";
    LangAnnotation::PROPS.XMLTAG = "lang";
//------ LemmaAnnotation -------
    LemmaAnnotation::PROPS = AbstractTokenAnnotation::PROPS;
    LemmaAnnotation::PROPS.ELEMENT_ID = LemmaAnnotation_t;
    LemmaAnnotation::PROPS.ANNOTATIONTYPE = AnnotationType::LEMMA;
    LemmaAnnotation::PROPS.LABEL = "Lemma";
    LemmaAnnotation::PROPS.XMLTAG = "lemma";
//------ LevelFeature -------
    LevelFeature::PROPS = Feature::PROPS;
    LevelFeature::PROPS.ELEMENT_ID = LevelFeature_t;
    LevelFeature::PROPS.SUBSET = "level";
    LevelFeature::PROPS.XMLTAG = "level";
//------ Linebreak -------
    Linebreak::PROPS = AbstractStructureElement::PROPS;
    Linebreak::PROPS.ELEMENT_ID = Linebreak_t;
    Linebreak::PROPS.ANNOTATIONTYPE = AnnotationType::LINEBREAK;
    Linebreak::PROPS.LABEL = "Linebreak";
    Linebreak::PROPS.TEXTDELIMITER = "";
    Linebreak::PROPS.XLINK = true;
    Linebreak::PROPS.XMLTAG = "br";
//------ List -------
    List::PROPS = AbstractStructureElement::PROPS;
    List::PROPS.ELEMENT_ID = List_t;
    List::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, AbstractExtendedTokenAnnotation_t, Alignment_t, Alternative_t, AlternativeLayers_t, Caption_t, Comment_t, Correction_t, Description_t, Event_t, Feature_t, ForeignData_t, ListItem_t, Metric_t, Note_t, Part_t, PhonContent_t, Reference_t, String_t, TextContent_t};
    List::PROPS.ANNOTATIONTYPE = AnnotationType::LIST;
    List::PROPS.LABEL = "List";
    List::PROPS.TEXTDELIMITER = "\n\n";
    List::PROPS.XMLTAG = "list";
//------ ListItem -------
    ListItem::PROPS = AbstractStructureElement::PROPS;
    ListItem::PROPS.ELEMENT_ID = ListItem_t;
    ListItem::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, AbstractExtendedTokenAnnotation_t, Alignment_t, Alternative_t, AlternativeLayers_t, Comment_t, Correction_t, Description_t, Event_t, Feature_t, ForeignData_t, Gap_t, Label_t, Linebreak_t, List_t, Metric_t, Note_t, Part_t, PhonContent_t, Reference_t, Sentence_t, String_t, TextContent_t, Whitespace_t, Word_t};
    ListItem::PROPS.LABEL = "List Item";
    ListItem::PROPS.TEXTDELIMITER = "\n";
    ListItem::PROPS.XMLTAG = "item";
//------ Metric -------
    Metric::PROPS.ELEMENT_ID = Metric_t;
    Metric::PROPS.ACCEPTED_DATA += {Comment_t, Description_t, Feature_t, ForeignData_t, ValueFeature_t};
    Metric::PROPS.ANNOTATIONTYPE = AnnotationType::METRIC;
    Metric::PROPS.LABEL = "Metric";
    Metric::PROPS.OPTIONAL_ATTRIBS = ID|CLASS|ANNOTATOR|N|CONFIDENCE|DATETIME|SRC|BEGINTIME|ENDTIME|SPEAKER|METADATA;
    Metric::PROPS.XMLTAG = "metric";
//------ ModalityFeature -------
    ModalityFeature::PROPS = Feature::PROPS;
    ModalityFeature::PROPS.ELEMENT_ID = ModalityFeature_t;
    ModalityFeature::PROPS.SUBSET = "modality";
    ModalityFeature::PROPS.XMLTAG = "modality";
//------ Morpheme -------
    Morpheme::PROPS = AbstractStructureElement::PROPS;
    Morpheme::PROPS.ELEMENT_ID = Morpheme_t;
    Morpheme::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, AbstractTokenAnnotation_t, Alignment_t, Alternative_t, AlternativeLayers_t, Comment_t, Correction_t, Description_t, Feature_t, ForeignData_t, FunctionFeature_t, Metric_t, Morpheme_t, Part_t, PhonContent_t, String_t, TextContent_t};
    Morpheme::PROPS.ANNOTATIONTYPE = AnnotationType::MORPHOLOGICAL;
    Morpheme::PROPS.LABEL = "Morpheme";
    Morpheme::PROPS.TEXTDELIMITER = "";
    Morpheme::PROPS.XMLTAG = "morpheme";
//------ MorphologyLayer -------
    MorphologyLayer::PROPS = AbstractAnnotationLayer::PROPS;
    MorphologyLayer::PROPS.ELEMENT_ID = MorphologyLayer_t;
    MorphologyLayer::PROPS.ACCEPTED_DATA += {Comment_t, Correction_t, Description_t, ForeignData_t, Morpheme_t};
    MorphologyLayer::PROPS.ANNOTATIONTYPE = AnnotationType::MORPHOLOGICAL;
    MorphologyLayer::PROPS.XMLTAG = "morphology";
//------ New -------
    New::PROPS = AbstractCorrectionChild::PROPS;
    New::PROPS.ELEMENT_ID = New_t;
    New::PROPS.OCCURRENCES = 1;
    New::PROPS.OPTIONAL_ATTRIBS = NO_ATT;
    New::PROPS.XMLTAG = "new";
//------ Note -------
    Note::PROPS = AbstractStructureElement::PROPS;
    Note::PROPS.ELEMENT_ID = Note_t;
    Note::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, AbstractExtendedTokenAnnotation_t, Alignment_t, Alternative_t, AlternativeLayers_t, Comment_t, Correction_t, Description_t, Example_t, Feature_t, Figure_t, ForeignData_t, Head_t, Linebreak_t, List_t, Metric_t, Paragraph_t, Part_t, PhonContent_t, Reference_t, Sentence_t, String_t, Table_t, TextContent_t, Utterance_t, Whitespace_t, Word_t};
    Note::PROPS.ANNOTATIONTYPE = AnnotationType::NOTE;
    Note::PROPS.LABEL = "Note";
    Note::PROPS.XMLTAG = "note";
//------ Observation -------
    Observation::PROPS = AbstractSpanAnnotation::PROPS;
    Observation::PROPS.ELEMENT_ID = Observation_t;
    Observation::PROPS.ACCEPTED_DATA += {AlignReference_t, Alignment_t, Comment_t, Description_t, Feature_t, ForeignData_t, Metric_t, WordReference_t, Word_t, Morpheme_t, Phoneme_t};
    Observation::PROPS.ANNOTATIONTYPE = AnnotationType::OBSERVATION;
    Observation::PROPS.LABEL = "Observation";
    Observation::PROPS.XMLTAG = "observation";
//------ ObservationLayer -------
    ObservationLayer::PROPS = AbstractAnnotationLayer::PROPS;
    ObservationLayer::PROPS.ELEMENT_ID = ObservationLayer_t;
    ObservationLayer::PROPS.ACCEPTED_DATA += {Comment_t, Correction_t, Description_t, ForeignData_t, Observation_t};
    ObservationLayer::PROPS.ANNOTATIONTYPE = AnnotationType::OBSERVATION;
    ObservationLayer::PROPS.XMLTAG = "observations";
//------ Original -------
    Original::PROPS = AbstractCorrectionChild::PROPS;
    Original::PROPS.ELEMENT_ID = Original_t;
    Original::PROPS.AUTH = false;
    Original::PROPS.OCCURRENCES = 1;
    Original::PROPS.OPTIONAL_ATTRIBS = NO_ATT;
    Original::PROPS.XMLTAG = "original";
//------ Paragraph -------
    Paragraph::PROPS = AbstractStructureElement::PROPS;
    Paragraph::PROPS.ELEMENT_ID = Paragraph_t;
    Paragraph::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, AbstractExtendedTokenAnnotation_t, Alignment_t, Alternative_t, AlternativeLayers_t, Comment_t, Correction_t, Description_t, Entry_t, Event_t, Example_t, Feature_t, Figure_t, ForeignData_t, Gap_t, Head_t, Linebreak_t, List_t, Metric_t, Note_t, Part_t, PhonContent_t, Quote_t, Reference_t, Sentence_t, String_t, TextContent_t, Whitespace_t, Word_t};
    Paragraph::PROPS.ANNOTATIONTYPE = AnnotationType::PARAGRAPH;
    Paragraph::PROPS.LABEL = "Paragraph";
    Paragraph::PROPS.TEXTDELIMITER = "\n\n";
    Paragraph::PROPS.XMLTAG = "p";
//------ Part -------
    Part::PROPS = AbstractStructureElement::PROPS;
    Part::PROPS.ELEMENT_ID = Part_t;
    Part::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, AbstractExtendedTokenAnnotation_t, AbstractStructureElement_t, Alignment_t, Alternative_t, AlternativeLayers_t, Comment_t, Correction_t, Description_t, Feature_t, ForeignData_t, Metric_t, Part_t};
    Part::PROPS.ANNOTATIONTYPE = AnnotationType::PART;
    Part::PROPS.LABEL = "Part";
    Part::PROPS.TEXTDELIMITER = "NONE";
    Part::PROPS.XMLTAG = "part";
//------ PhonContent -------
    PhonContent::PROPS.ELEMENT_ID = PhonContent_t;
    PhonContent::PROPS.ACCEPTED_DATA += {Comment_t, Description_t, XmlText_t};
    PhonContent::PROPS.ANNOTATIONTYPE = AnnotationType::PHON;
    PhonContent::PROPS.LABEL = "Phonetic Content";
    PhonContent::PROPS.OCCURRENCES = 0;
    PhonContent::PROPS.OPTIONAL_ATTRIBS = CLASS|ANNOTATOR|CONFIDENCE|DATETIME|METADATA;
    PhonContent::PROPS.PHONCONTAINER = true;
    PhonContent::PROPS.PRINTABLE = false;
    PhonContent::PROPS.SPEAKABLE = true;
    PhonContent::PROPS.XMLTAG = "ph";
//------ Phoneme -------
    Phoneme::PROPS = AbstractStructureElement::PROPS;
    Phoneme::PROPS.ELEMENT_ID = Phoneme_t;
    Phoneme::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, AbstractTokenAnnotation_t, Alignment_t, Alternative_t, AlternativeLayers_t, Comment_t, Correction_t, Description_t, Feature_t, ForeignData_t, FunctionFeature_t, Metric_t, Part_t, PhonContent_t, Phoneme_t, String_t, TextContent_t};
    Phoneme::PROPS.ANNOTATIONTYPE = AnnotationType::PHONOLOGICAL;
    Phoneme::PROPS.LABEL = "Phoneme";
    Phoneme::PROPS.TEXTDELIMITER = "";
    Phoneme::PROPS.XMLTAG = "phoneme";
//------ PhonologyLayer -------
    PhonologyLayer::PROPS = AbstractAnnotationLayer::PROPS;
    PhonologyLayer::PROPS.ELEMENT_ID = PhonologyLayer_t;
    PhonologyLayer::PROPS.ACCEPTED_DATA += {Comment_t, Correction_t, Description_t, ForeignData_t, Phoneme_t};
    PhonologyLayer::PROPS.ANNOTATIONTYPE = AnnotationType::PHONOLOGICAL;
    PhonologyLayer::PROPS.XMLTAG = "phonology";
//------ PolarityFeature -------
    PolarityFeature::PROPS = Feature::PROPS;
    PolarityFeature::PROPS.ELEMENT_ID = PolarityFeature_t;
    PolarityFeature::PROPS.SUBSET = "polarity";
    PolarityFeature::PROPS.XMLTAG = "polarity";
//------ PosAnnotation -------
    PosAnnotation::PROPS = AbstractTokenAnnotation::PROPS;
    PosAnnotation::PROPS.ELEMENT_ID = PosAnnotation_t;
    PosAnnotation::PROPS.ACCEPTED_DATA += {Comment_t, Description_t, Feature_t, ForeignData_t, HeadFeature_t, Metric_t};
    PosAnnotation::PROPS.ANNOTATIONTYPE = AnnotationType::POS;
    PosAnnotation::PROPS.LABEL = "Part-of-Speech";
    PosAnnotation::PROPS.XMLTAG = "pos";
//------ Predicate -------
    Predicate::PROPS = AbstractSpanAnnotation::PROPS;
    Predicate::PROPS.ELEMENT_ID = Predicate_t;
    Predicate::PROPS.ACCEPTED_DATA += {AlignReference_t, Alignment_t, Comment_t, Description_t, Feature_t, ForeignData_t, Metric_t, SemanticRole_t, WordReference_t, Word_t, Morpheme_t, Phoneme_t};
    Predicate::PROPS.ANNOTATIONTYPE = AnnotationType::PREDICATE;
    Predicate::PROPS.LABEL = "Predicate";
    Predicate::PROPS.XMLTAG = "predicate";
//------ Quote -------
    Quote::PROPS = AbstractStructureElement::PROPS;
    Quote::PROPS.ELEMENT_ID = Quote_t;
    Quote::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, Alignment_t, Alternative_t, AlternativeLayers_t, Comment_t, Correction_t, Description_t, Division_t, Feature_t, ForeignData_t, Gap_t, Metric_t, Paragraph_t, Part_t, Quote_t, Sentence_t, String_t, TextContent_t, Utterance_t, Word_t};
    Quote::PROPS.LABEL = "Quote";
    Quote::PROPS.XMLTAG = "quote";
//------ Reference -------
    Reference::PROPS = AbstractStructureElement::PROPS;
    Reference::PROPS.ELEMENT_ID = Reference_t;
    Reference::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, Alignment_t, Alternative_t, AlternativeLayers_t, Comment_t, Correction_t, Description_t, Feature_t, ForeignData_t, Metric_t, Paragraph_t, Part_t, PhonContent_t, Quote_t, Sentence_t, String_t, TextContent_t, Utterance_t, Word_t};
    Reference::PROPS.LABEL = "Reference";
    Reference::PROPS.TEXTDELIMITER = "NONE";
    Reference::PROPS.XMLTAG = "ref";
//------ Relation -------
    Relation::PROPS = AbstractSpanRole::PROPS;
    Relation::PROPS.ELEMENT_ID = Relation_t;
    Relation::PROPS.LABEL = "Relation";
    Relation::PROPS.OCCURRENCES = 1;
    Relation::PROPS.XMLTAG = "relation";
//------ Row -------
    Row::PROPS = AbstractStructureElement::PROPS;
    Row::PROPS.ELEMENT_ID = Row_t;
    Row::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, AbstractExtendedTokenAnnotation_t, Alignment_t, Alternative_t, AlternativeLayers_t, Cell_t, Comment_t, Correction_t, Description_t, Feature_t, ForeignData_t, Metric_t, Part_t};
    Row::PROPS.LABEL = "Table Row";
    Row::PROPS.TEXTDELIMITER = "\n";
    Row::PROPS.XMLTAG = "row";
//------ SemanticRole -------
    SemanticRole::PROPS = AbstractSpanAnnotation::PROPS;
    SemanticRole::PROPS.ELEMENT_ID = SemanticRole_t;
    SemanticRole::PROPS.ACCEPTED_DATA += {AlignReference_t, Alignment_t, Comment_t, Description_t, Feature_t, ForeignData_t, Headspan_t, Metric_t, WordReference_t, Word_t, Morpheme_t, Phoneme_t};
    SemanticRole::PROPS.ANNOTATIONTYPE = AnnotationType::SEMROLE;
    SemanticRole::PROPS.LABEL = "Semantic Role";
    SemanticRole::PROPS.REQUIRED_ATTRIBS = CLASS;
    SemanticRole::PROPS.XMLTAG = "semrole";
//------ SemanticRolesLayer -------
    SemanticRolesLayer::PROPS = AbstractAnnotationLayer::PROPS;
    SemanticRolesLayer::PROPS.ELEMENT_ID = SemanticRolesLayer_t;
    SemanticRolesLayer::PROPS.ACCEPTED_DATA += {Comment_t, Correction_t, Description_t, ForeignData_t, Predicate_t, SemanticRole_t};
    SemanticRolesLayer::PROPS.ANNOTATIONTYPE = AnnotationType::SEMROLE;
    SemanticRolesLayer::PROPS.XMLTAG = "semroles";
//------ SenseAnnotation -------
    SenseAnnotation::PROPS = AbstractTokenAnnotation::PROPS;
    SenseAnnotation::PROPS.ELEMENT_ID = SenseAnnotation_t;
    SenseAnnotation::PROPS.ACCEPTED_DATA += {Comment_t, Description_t, Feature_t, ForeignData_t, Metric_t, SynsetFeature_t};
    SenseAnnotation::PROPS.ANNOTATIONTYPE = AnnotationType::SENSE;
    SenseAnnotation::PROPS.LABEL = "Semantic Sense";
    SenseAnnotation::PROPS.OCCURRENCES_PER_SET = 0;
    SenseAnnotation::PROPS.XMLTAG = "sense";
//------ Sentence -------
    Sentence::PROPS = AbstractStructureElement::PROPS;
    Sentence::PROPS.ELEMENT_ID = Sentence_t;
    Sentence::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, AbstractExtendedTokenAnnotation_t, Alignment_t, Alternative_t, AlternativeLayers_t, Comment_t, Correction_t, Description_t, Entry_t, Event_t, Example_t, Feature_t, ForeignData_t, Gap_t, Linebreak_t, Metric_t, Note_t, Part_t, PhonContent_t, Quote_t, Reference_t, String_t, TextContent_t, Whitespace_t, Word_t};
    Sentence::PROPS.ANNOTATIONTYPE = AnnotationType::SENTENCE;
    Sentence::PROPS.LABEL = "Sentence";
    Sentence::PROPS.TEXTDELIMITER = " ";
    Sentence::PROPS.XMLTAG = "s";
//------ Sentiment -------
    Sentiment::PROPS = AbstractSpanAnnotation::PROPS;
    Sentiment::PROPS.ELEMENT_ID = Sentiment_t;
    Sentiment::PROPS.ACCEPTED_DATA += {AlignReference_t, Alignment_t, Comment_t, Description_t, Feature_t, ForeignData_t, Headspan_t, Metric_t, PolarityFeature_t, Source_t, StrengthFeature_t, Target_t, WordReference_t, Word_t, Morpheme_t, Phoneme_t};
    Sentiment::PROPS.ANNOTATIONTYPE = AnnotationType::SENTIMENT;
    Sentiment::PROPS.LABEL = "Sentiment";
    Sentiment::PROPS.XMLTAG = "sentiment";
//------ SentimentLayer -------
    SentimentLayer::PROPS = AbstractAnnotationLayer::PROPS;
    SentimentLayer::PROPS.ELEMENT_ID = SentimentLayer_t;
    SentimentLayer::PROPS.ACCEPTED_DATA += {Comment_t, Correction_t, Description_t, ForeignData_t, Sentiment_t};
    SentimentLayer::PROPS.ANNOTATIONTYPE = AnnotationType::SENTIMENT;
    SentimentLayer::PROPS.XMLTAG = "sentiments";
//------ Source -------
    Source::PROPS = AbstractSpanRole::PROPS;
    Source::PROPS.ELEMENT_ID = Source_t;
    Source::PROPS.LABEL = "Source";
    Source::PROPS.OCCURRENCES = 1;
    Source::PROPS.XMLTAG = "source";
//------ Speech -------
    Speech::PROPS = AbstractStructureElement::PROPS;
    Speech::PROPS.ELEMENT_ID = Speech_t;
    Speech::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, AbstractExtendedTokenAnnotation_t, Alignment_t, Alternative_t, AlternativeLayers_t, Comment_t, Correction_t, Description_t, Division_t, Entry_t, Event_t, Example_t, External_t, Feature_t, ForeignData_t, Gap_t, List_t, Metric_t, Note_t, Paragraph_t, Part_t, PhonContent_t, Quote_t, Reference_t, Sentence_t, String_t, TextContent_t, Utterance_t, Word_t};
    Speech::PROPS.LABEL = "Speech Body";
    Speech::PROPS.TEXTDELIMITER = "\n\n\n";
    Speech::PROPS.XMLTAG = "speech";
//------ Statement -------
    Statement::PROPS = AbstractSpanAnnotation::PROPS;
    Statement::PROPS.ELEMENT_ID = Statement_t;
    Statement::PROPS.ACCEPTED_DATA += {AlignReference_t, Alignment_t, Comment_t, Description_t, Feature_t, ForeignData_t, Headspan_t, Metric_t, Relation_t, Source_t, WordReference_t, Word_t, Morpheme_t, Phoneme_t};
    Statement::PROPS.ANNOTATIONTYPE = AnnotationType::STATEMENT;
    Statement::PROPS.LABEL = "Statement";
    Statement::PROPS.XMLTAG = "statement";
//------ StatementLayer -------
    StatementLayer::PROPS = AbstractAnnotationLayer::PROPS;
    StatementLayer::PROPS.ELEMENT_ID = StatementLayer_t;
    StatementLayer::PROPS.ACCEPTED_DATA += {Comment_t, Correction_t, Description_t, ForeignData_t, Statement_t};
    StatementLayer::PROPS.ANNOTATIONTYPE = AnnotationType::STATEMENT;
    StatementLayer::PROPS.XMLTAG = "statements";
//------ StrengthFeature -------
    StrengthFeature::PROPS = Feature::PROPS;
    StrengthFeature::PROPS.ELEMENT_ID = StrengthFeature_t;
    StrengthFeature::PROPS.SUBSET = "strength";
    StrengthFeature::PROPS.XMLTAG = "strength";
//------ String -------
    String::PROPS.ELEMENT_ID = String_t;
    String::PROPS.ACCEPTED_DATA += {AbstractExtendedTokenAnnotation_t, Alignment_t, Comment_t, Correction_t, Description_t, Feature_t, ForeignData_t, Metric_t, PhonContent_t, TextContent_t};
    String::PROPS.ANNOTATIONTYPE = AnnotationType::STRING;
    String::PROPS.LABEL = "String";
    String::PROPS.OCCURRENCES = 0;
    String::PROPS.OPTIONAL_ATTRIBS = ID|CLASS|ANNOTATOR|CONFIDENCE|DATETIME|N|SRC|BEGINTIME|ENDTIME|METADATA;
    String::PROPS.PRINTABLE = true;
    String::PROPS.XMLTAG = "str";
//------ StyleFeature -------
    StyleFeature::PROPS = Feature::PROPS;
    StyleFeature::PROPS.ELEMENT_ID = StyleFeature_t;
    StyleFeature::PROPS.SUBSET = "style";
    StyleFeature::PROPS.XMLTAG = "style";
//------ SubjectivityAnnotation -------
    SubjectivityAnnotation::PROPS = AbstractTokenAnnotation::PROPS;
    SubjectivityAnnotation::PROPS.ELEMENT_ID = SubjectivityAnnotation_t;
    SubjectivityAnnotation::PROPS.ANNOTATIONTYPE = AnnotationType::SUBJECTIVITY;
    SubjectivityAnnotation::PROPS.LABEL = "Subjectivity/Sentiment";
    SubjectivityAnnotation::PROPS.XMLTAG = "subjectivity";
//------ Suggestion -------
    Suggestion::PROPS = AbstractCorrectionChild::PROPS;
    Suggestion::PROPS.ELEMENT_ID = Suggestion_t;
    Suggestion::PROPS.AUTH = false;
    Suggestion::PROPS.OCCURRENCES = 0;
    Suggestion::PROPS.XMLTAG = "suggestion";
//------ SynsetFeature -------
    SynsetFeature::PROPS = Feature::PROPS;
    SynsetFeature::PROPS.ELEMENT_ID = SynsetFeature_t;
    SynsetFeature::PROPS.SUBSET = "synset";
    SynsetFeature::PROPS.XMLTAG = "synset";
//------ SyntacticUnit -------
    SyntacticUnit::PROPS = AbstractSpanAnnotation::PROPS;
    SyntacticUnit::PROPS.ELEMENT_ID = SyntacticUnit_t;
    SyntacticUnit::PROPS.ACCEPTED_DATA += {AlignReference_t, Alignment_t, Comment_t, Description_t, Feature_t, ForeignData_t, Metric_t, SyntacticUnit_t, WordReference_t, Word_t, Morpheme_t, Phoneme_t};
    SyntacticUnit::PROPS.ANNOTATIONTYPE = AnnotationType::SYNTAX;
    SyntacticUnit::PROPS.LABEL = "Syntactic Unit";
    SyntacticUnit::PROPS.XMLTAG = "su";
//------ SyntaxLayer -------
    SyntaxLayer::PROPS = AbstractAnnotationLayer::PROPS;
    SyntaxLayer::PROPS.ELEMENT_ID = SyntaxLayer_t;
    SyntaxLayer::PROPS.ACCEPTED_DATA += {Comment_t, Correction_t, Description_t, ForeignData_t, SyntacticUnit_t};
    SyntaxLayer::PROPS.ANNOTATIONTYPE = AnnotationType::SYNTAX;
    SyntaxLayer::PROPS.XMLTAG = "syntax";
//------ Table -------
    Table::PROPS = AbstractStructureElement::PROPS;
    Table::PROPS.ELEMENT_ID = Table_t;
    Table::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, AbstractExtendedTokenAnnotation_t, Alignment_t, Alternative_t, AlternativeLayers_t, Comment_t, Correction_t, Description_t, Feature_t, ForeignData_t, Metric_t, Part_t, Row_t, TableHead_t};
    Table::PROPS.ANNOTATIONTYPE = AnnotationType::TABLE;
    Table::PROPS.LABEL = "Table";
    Table::PROPS.XMLTAG = "table";
//------ TableHead -------
    TableHead::PROPS = AbstractStructureElement::PROPS;
    TableHead::PROPS.ELEMENT_ID = TableHead_t;
    TableHead::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, AbstractExtendedTokenAnnotation_t, Alignment_t, Alternative_t, AlternativeLayers_t, Comment_t, Correction_t, Description_t, Feature_t, ForeignData_t, Metric_t, Part_t, Row_t};
    TableHead::PROPS.LABEL = "Table Header";
    TableHead::PROPS.XMLTAG = "tablehead";
//------ Target -------
    Target::PROPS = AbstractSpanRole::PROPS;
    Target::PROPS.ELEMENT_ID = Target_t;
    Target::PROPS.LABEL = "Target";
    Target::PROPS.OCCURRENCES = 1;
    Target::PROPS.XMLTAG = "target";
//------ Term -------
    Term::PROPS = AbstractStructureElement::PROPS;
    Term::PROPS.ELEMENT_ID = Term_t;
    Term::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, AbstractExtendedTokenAnnotation_t, Alignment_t, Alternative_t, AlternativeLayers_t, Comment_t, Correction_t, Description_t, Event_t, Feature_t, Figure_t, ForeignData_t, Gap_t, List_t, Metric_t, Paragraph_t, Part_t, PhonContent_t, Reference_t, Sentence_t, String_t, Table_t, TextContent_t, Utterance_t, Word_t};
    Term::PROPS.ANNOTATIONTYPE = AnnotationType::TERM;
    Term::PROPS.LABEL = "Term";
    Term::PROPS.XMLTAG = "term";
//------ Text -------
    Text::PROPS = AbstractStructureElement::PROPS;
    Text::PROPS.ELEMENT_ID = Text_t;
    Text::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, AbstractExtendedTokenAnnotation_t, Alignment_t, Alternative_t, AlternativeLayers_t, Comment_t, Correction_t, Description_t, Division_t, Entry_t, Event_t, Example_t, External_t, Feature_t, Figure_t, ForeignData_t, Gap_t, List_t, Metric_t, Note_t, Paragraph_t, Part_t, PhonContent_t, Quote_t, Reference_t, Sentence_t, String_t, Table_t, TextContent_t, Word_t};
    Text::PROPS.LABEL = "Text Body";
    Text::PROPS.TEXTDELIMITER = "\n\n\n";
    Text::PROPS.XMLTAG = "text";
//------ TextContent -------
    TextContent::PROPS.ELEMENT_ID = TextContent_t;
    TextContent::PROPS.ACCEPTED_DATA += {AbstractTextMarkup_t, Comment_t, Description_t, Linebreak_t, XmlText_t};
    TextContent::PROPS.ANNOTATIONTYPE = AnnotationType::TEXT;
    TextContent::PROPS.LABEL = "Text";
    TextContent::PROPS.OCCURRENCES = 0;
    TextContent::PROPS.OPTIONAL_ATTRIBS = CLASS|ANNOTATOR|CONFIDENCE|DATETIME|METADATA;
    TextContent::PROPS.PRINTABLE = true;
    TextContent::PROPS.SPEAKABLE = false;
    TextContent::PROPS.TEXTCONTAINER = true;
    TextContent::PROPS.XLINK = true;
    TextContent::PROPS.XMLTAG = "t";
//------ TextMarkupCorrection -------
    TextMarkupCorrection::PROPS = AbstractTextMarkup::PROPS;
    TextMarkupCorrection::PROPS.ELEMENT_ID = TextMarkupCorrection_t;
    TextMarkupCorrection::PROPS.ANNOTATIONTYPE = AnnotationType::CORRECTION;
    TextMarkupCorrection::PROPS.XMLTAG = "t-correction";
//------ TextMarkupError -------
    TextMarkupError::PROPS = AbstractTextMarkup::PROPS;
    TextMarkupError::PROPS.ELEMENT_ID = TextMarkupError_t;
    TextMarkupError::PROPS.ANNOTATIONTYPE = AnnotationType::ERRORDETECTION;
    TextMarkupError::PROPS.XMLTAG = "t-error";
//------ TextMarkupGap -------
    TextMarkupGap::PROPS = AbstractTextMarkup::PROPS;
    TextMarkupGap::PROPS.ELEMENT_ID = TextMarkupGap_t;
    TextMarkupGap::PROPS.ANNOTATIONTYPE = AnnotationType::GAP;
    TextMarkupGap::PROPS.XMLTAG = "t-gap";
//------ TextMarkupString -------
    TextMarkupString::PROPS = AbstractTextMarkup::PROPS;
    TextMarkupString::PROPS.ELEMENT_ID = TextMarkupString_t;
    TextMarkupString::PROPS.ANNOTATIONTYPE = AnnotationType::STRING;
    TextMarkupString::PROPS.XMLTAG = "t-str";
//------ TextMarkupStyle -------
    TextMarkupStyle::PROPS = AbstractTextMarkup::PROPS;
    TextMarkupStyle::PROPS.ELEMENT_ID = TextMarkupStyle_t;
    TextMarkupStyle::PROPS.ANNOTATIONTYPE = AnnotationType::STYLE;
    TextMarkupStyle::PROPS.XMLTAG = "t-style";
//------ TimeFeature -------
    TimeFeature::PROPS = Feature::PROPS;
    TimeFeature::PROPS.ELEMENT_ID = TimeFeature_t;
    TimeFeature::PROPS.SUBSET = "time";
    TimeFeature::PROPS.XMLTAG = "time";
//------ TimeSegment -------
    TimeSegment::PROPS = AbstractSpanAnnotation::PROPS;
    TimeSegment::PROPS.ELEMENT_ID = TimeSegment_t;
    TimeSegment::PROPS.ACCEPTED_DATA += {ActorFeature_t, AlignReference_t, Alignment_t, BegindatetimeFeature_t, Comment_t, Description_t, EnddatetimeFeature_t, Feature_t, ForeignData_t, Metric_t, WordReference_t, Word_t, Morpheme_t, Phoneme_t};
    TimeSegment::PROPS.ANNOTATIONTYPE = AnnotationType::TIMESEGMENT;
    TimeSegment::PROPS.LABEL = "Time Segment";
    TimeSegment::PROPS.XMLTAG = "timesegment";
//------ TimingLayer -------
    TimingLayer::PROPS = AbstractAnnotationLayer::PROPS;
    TimingLayer::PROPS.ELEMENT_ID = TimingLayer_t;
    TimingLayer::PROPS.ACCEPTED_DATA += {Comment_t, Correction_t, Description_t, ForeignData_t, TimeSegment_t};
    TimingLayer::PROPS.ANNOTATIONTYPE = AnnotationType::TIMESEGMENT;
    TimingLayer::PROPS.XMLTAG = "timing";
//------ Utterance -------
    Utterance::PROPS = AbstractStructureElement::PROPS;
    Utterance::PROPS.ELEMENT_ID = Utterance_t;
    Utterance::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, AbstractExtendedTokenAnnotation_t, Alignment_t, Alternative_t, AlternativeLayers_t, Comment_t, Correction_t, Description_t, Feature_t, ForeignData_t, Gap_t, Metric_t, Note_t, Part_t, PhonContent_t, Quote_t, Reference_t, Sentence_t, String_t, TextContent_t, Word_t};
    Utterance::PROPS.ANNOTATIONTYPE = AnnotationType::UTTERANCE;
    Utterance::PROPS.LABEL = "Utterance";
    Utterance::PROPS.TEXTDELIMITER = " ";
    Utterance::PROPS.XMLTAG = "utt";
//------ ValueFeature -------
    ValueFeature::PROPS = Feature::PROPS;
    ValueFeature::PROPS.ELEMENT_ID = ValueFeature_t;
    ValueFeature::PROPS.SUBSET = "value";
    ValueFeature::PROPS.XMLTAG = "value";
//------ Whitespace -------
    Whitespace::PROPS = AbstractStructureElement::PROPS;
    Whitespace::PROPS.ELEMENT_ID = Whitespace_t;
    Whitespace::PROPS.ANNOTATIONTYPE = AnnotationType::WHITESPACE;
    Whitespace::PROPS.LABEL = "Whitespace";
    Whitespace::PROPS.TEXTDELIMITER = "";
    Whitespace::PROPS.XMLTAG = "whitespace";
//------ Word -------
    Word::PROPS = AbstractStructureElement::PROPS;
    Word::PROPS.ELEMENT_ID = Word_t;
    Word::PROPS.ACCEPTED_DATA += {AbstractAnnotationLayer_t, AbstractTokenAnnotation_t, Alignment_t, Alternative_t, AlternativeLayers_t, Comment_t, Correction_t, Description_t, Feature_t, ForeignData_t, Metric_t, Part_t, PhonContent_t, Reference_t, String_t, TextContent_t};
    Word::PROPS.ANNOTATIONTYPE = AnnotationType::TOKEN;
    Word::PROPS.LABEL = "Word/Token";
    Word::PROPS.OPTIONAL_ATTRIBS = ID|CLASS|ANNOTATOR|N|CONFIDENCE|DATETIME|SRC|BEGINTIME|ENDTIME|SPEAKER|TEXTCLASS|METADATA;
    Word::PROPS.TEXTDELIMITER = " ";
    Word::PROPS.XMLTAG = "w";
//------ WordReference -------
    WordReference::PROPS.ELEMENT_ID = WordReference_t;
    WordReference::PROPS.XMLTAG = "wref";

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

  }


  //foliaspec:typehierarchy
  static const map<ElementType, set<ElementType> > typeHierarchy = {      { AbstractAnnotationLayer_t, {  } },
     { AbstractCorrectionChild_t, {  } },
     { AbstractExtendedTokenAnnotation_t, { AbstractTokenAnnotation_t } },
     { AbstractSpanAnnotation_t, {  } },
     { AbstractSpanRole_t, { AbstractSpanAnnotation_t } },
     { AbstractStructureElement_t, {  } },
     { AbstractTextMarkup_t, {  } },
     { AbstractTokenAnnotation_t, {  } },
     { ActorFeature_t, { Feature_t } },
     { AlignReference_t, {  } },
     { Alignment_t, {  } },
     { Alternative_t, {  } },
     { AlternativeLayers_t, {  } },
     { BegindatetimeFeature_t, { Feature_t } },
     { Caption_t, { AbstractStructureElement_t } },
     { Cell_t, { AbstractStructureElement_t } },
     { Chunk_t, { AbstractSpanAnnotation_t } },
     { ChunkingLayer_t, { AbstractAnnotationLayer_t } },
     { Comment_t, {  } },
     { ComplexAlignment_t, {  } },
     { ComplexAlignmentLayer_t, { AbstractAnnotationLayer_t } },
     { Content_t, {  } },
     { CoreferenceChain_t, { AbstractSpanAnnotation_t } },
     { CoreferenceLayer_t, { AbstractAnnotationLayer_t } },
     { CoreferenceLink_t, { AbstractSpanRole_t,AbstractSpanAnnotation_t } },
     { Correction_t, {  } },
     { Current_t, { AbstractCorrectionChild_t } },
     { Definition_t, { AbstractStructureElement_t } },
     { DependenciesLayer_t, { AbstractAnnotationLayer_t } },
     { Dependency_t, { AbstractSpanAnnotation_t } },
     { DependencyDependent_t, { AbstractSpanRole_t,AbstractSpanAnnotation_t } },
     { Description_t, {  } },
     { Division_t, { AbstractStructureElement_t } },
     { DomainAnnotation_t, { AbstractExtendedTokenAnnotation_t,AbstractTokenAnnotation_t } },
     { EnddatetimeFeature_t, { Feature_t } },
     { EntitiesLayer_t, { AbstractAnnotationLayer_t } },
     { Entity_t, { AbstractSpanAnnotation_t } },
     { Entry_t, { AbstractStructureElement_t } },
     { ErrorDetection_t, { AbstractExtendedTokenAnnotation_t,AbstractTokenAnnotation_t } },
     { Event_t, { AbstractStructureElement_t } },
     { Example_t, { AbstractStructureElement_t } },
     { External_t, {  } },
     { Feature_t, {  } },
     { Figure_t, { AbstractStructureElement_t } },
     { ForeignData_t, {  } },
     { FunctionFeature_t, { Feature_t } },
     { Gap_t, {  } },
     { Head_t, { AbstractStructureElement_t } },
     { HeadFeature_t, { Feature_t } },
     { Headspan_t, { AbstractSpanRole_t,AbstractSpanAnnotation_t } },
     { Label_t, { AbstractStructureElement_t } },
     { LangAnnotation_t, { AbstractExtendedTokenAnnotation_t,AbstractTokenAnnotation_t } },
     { LemmaAnnotation_t, { AbstractTokenAnnotation_t } },
     { LevelFeature_t, { Feature_t } },
     { Linebreak_t, { AbstractStructureElement_t } },
     { List_t, { AbstractStructureElement_t } },
     { ListItem_t, { AbstractStructureElement_t } },
     { Metric_t, {  } },
     { ModalityFeature_t, { Feature_t } },
     { Morpheme_t, { AbstractStructureElement_t } },
     { MorphologyLayer_t, { AbstractAnnotationLayer_t } },
     { New_t, { AbstractCorrectionChild_t } },
     { Note_t, { AbstractStructureElement_t } },
     { Observation_t, { AbstractSpanAnnotation_t } },
     { ObservationLayer_t, { AbstractAnnotationLayer_t } },
     { Original_t, { AbstractCorrectionChild_t } },
     { Paragraph_t, { AbstractStructureElement_t } },
     { Part_t, { AbstractStructureElement_t } },
     { PhonContent_t, {  } },
     { Phoneme_t, { AbstractStructureElement_t } },
     { PhonologyLayer_t, { AbstractAnnotationLayer_t } },
     { PolarityFeature_t, { Feature_t } },
     { PosAnnotation_t, { AbstractTokenAnnotation_t } },
     { Predicate_t, { AbstractSpanAnnotation_t } },
     { Quote_t, { AbstractStructureElement_t } },
     { Reference_t, { AbstractStructureElement_t } },
     { Relation_t, { AbstractSpanRole_t,AbstractSpanAnnotation_t } },
     { Row_t, { AbstractStructureElement_t } },
     { SemanticRole_t, { AbstractSpanAnnotation_t } },
     { SemanticRolesLayer_t, { AbstractAnnotationLayer_t } },
     { SenseAnnotation_t, { AbstractTokenAnnotation_t } },
     { Sentence_t, { AbstractStructureElement_t } },
     { Sentiment_t, { AbstractSpanAnnotation_t } },
     { SentimentLayer_t, { AbstractAnnotationLayer_t } },
     { Source_t, { AbstractSpanRole_t,AbstractSpanAnnotation_t } },
     { Speech_t, { AbstractStructureElement_t } },
     { Statement_t, { AbstractSpanAnnotation_t } },
     { StatementLayer_t, { AbstractAnnotationLayer_t } },
     { StrengthFeature_t, { Feature_t } },
     { String_t, {  } },
     { StyleFeature_t, { Feature_t } },
     { SubjectivityAnnotation_t, { AbstractTokenAnnotation_t } },
     { Suggestion_t, { AbstractCorrectionChild_t } },
     { SynsetFeature_t, { Feature_t } },
     { SyntacticUnit_t, { AbstractSpanAnnotation_t } },
     { SyntaxLayer_t, { AbstractAnnotationLayer_t } },
     { Table_t, { AbstractStructureElement_t } },
     { TableHead_t, { AbstractStructureElement_t } },
     { Target_t, { AbstractSpanRole_t,AbstractSpanAnnotation_t } },
     { Term_t, { AbstractStructureElement_t } },
     { Text_t, { AbstractStructureElement_t } },
     { TextContent_t, {  } },
     { TextMarkupCorrection_t, { AbstractTextMarkup_t } },
     { TextMarkupError_t, { AbstractTextMarkup_t } },
     { TextMarkupGap_t, { AbstractTextMarkup_t } },
     { TextMarkupString_t, { AbstractTextMarkup_t } },
     { TextMarkupStyle_t, { AbstractTextMarkup_t } },
     { TimeFeature_t, { Feature_t } },
     { TimeSegment_t, { AbstractSpanAnnotation_t } },
     { TimingLayer_t, { AbstractAnnotationLayer_t } },
     { Utterance_t, { AbstractStructureElement_t } },
     { ValueFeature_t, { Feature_t } },
     { Whitespace_t, { AbstractStructureElement_t } },
     { Word_t, { AbstractStructureElement_t } },
     { WordReference_t, {  } },
     { PlaceHolder_t , { Word_t, AbstractStructureElement_t } }
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
      XLINK = false;
      XMLTAG = "NONE";

//foliaspec:end:defaultproperties
  }

  FoliaElement *FoliaImpl::private_createElement( ElementType et ){
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
    case Quote_t: return new Quote();
    case DependenciesLayer_t: return new DependenciesLayer();
    case Dependency_t: return new Dependency();
    case DependencyDependent_t: return new DependencyDependent();
    case Headspan_t: return new Headspan();
    case ComplexAlignmentLayer_t: return new ComplexAlignmentLayer();
    case ComplexAlignment_t: return new ComplexAlignment();
    case Alignment_t: return new Alignment();
    case AlignReference_t: return new AlignReference();
    case TextMarkupString_t: return new TextMarkupString();
    case TextMarkupGap_t: return new TextMarkupGap();
    case TextMarkupCorrection_t: return new TextMarkupCorrection();
    case TextMarkupError_t: return new TextMarkupError();
    case TextMarkupStyle_t: return new TextMarkupStyle();
    case Part_t: return new Part();
    case Observation_t: return new Observation();
    case ObservationLayer_t: return new ObservationLayer();
    case PolarityFeature_t: return new PolarityFeature();
    case Predicate_t: return new Predicate();
    case Relation_t: return new Relation();
    case Sentiment_t: return new Sentiment();
    case SentimentLayer_t: return new SentimentLayer();
    case Statement_t: return new Statement();
    case StatementLayer_t: return new StatementLayer();
    case StrengthFeature_t: return new StrengthFeature();
    case StyleFeature_t: return new StyleFeature();
    case Source_t: return new Source();
    case Target_t: return new Target();
    case AbstractSpanAnnotation_t:
    case AbstractSpanRole_t:
    case AbstractAnnotationLayer_t:
    case AbstractTextMarkup_t:
    case AbstractTokenAnnotation_t:
    case AbstractStructureElement_t:
    case AbstractExtendedTokenAnnotation_t:
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
    if ( e1 == e2 )
      return true;
    const auto& it = typeHierarchy.find( e1 );
    if ( it != typeHierarchy.end() ){
      return it->second.find( e2 ) != it->second.end();
    }
    return false;
  }

  bool isSubClass( const FoliaElement *e1, const FoliaElement *e2 ){
    return isSubClass( e1->element_id(), e2->element_id() );
  }

  bool FoliaElement::isSubClass( ElementType t ) const {
    return folia::isSubClass( element_id(), t );
  }


  void print_type_hierarchy( ostream& os ){
    for ( auto const& top : typeHierarchy ){
      os << toString(top.first) << endl;
      for ( auto const& el : top.second ){
	os << "     -- " << toString(el) << endl;
      }
    }
  }

  namespace {
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
