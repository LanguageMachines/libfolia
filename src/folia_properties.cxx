#include <set>
#include <string>
#include <iostream>

#include "libfolia/folia.h"
#include "libfolia/folia_properties.h"

//foliaspec:header
//blah blah.....

namespace folia {

  using namespace std;

  //foliaspec:version_major:MAJOR_VERSION
  const int MAJOR_VERSION = 0;

  //foliaspec:version_minor:MINOR_VERSION
  const int MINOR_VERSION = 12;

  //foliaspec:version_sub:SUB_VERSION
  const int SUB_VERSION   = 2;

  //foliaspec:namespace:NSFOLIA
  const string NSFOLIA = "http://ilk.uvt.nl/folia";

  const string NSDCOI = "http://lands.let.ru.nl/projects/d-coi/ns/1.0";
  const string NSIMDI = "http://www.mpi.nl/IMDI/Schema/IMDI";

  //foliaspec:annotationtype_string_map
  const map<AnnotationType::AnnotationType,string> ant_s_map = {
    { AnnotationType::NO_ANN, "NoNe" },
    { AnnotationType::TEXT, "text" },
    { AnnotationType::STRING, "string" },
    { AnnotationType::TOKEN, "token" },
    { AnnotationType::DIVISION, "division" },
    { AnnotationType::PARAGRAPH, "paragraph" },
    { AnnotationType::LIST, "list" },
    { AnnotationType::FIGURE, "figure" },
    { AnnotationType::WHITESPACE, "whitespace" },
    { AnnotationType::LINEBREAK, "linebreak" },
    { AnnotationType::SENTENCE, "sentence" },
    { AnnotationType::POS, "pos" },
    { AnnotationType::LEMMA, "lemma" },
    { AnnotationType::DOMAIN, "domain" },
    { AnnotationType::SENSE, "sense" },
    { AnnotationType::SYNTAX, "syntax" },
    { AnnotationType::CHUNKING, "chunking" },
    { AnnotationType::ENTITY, "entity" },
    { AnnotationType::CORRECTION, "correction" },
    { AnnotationType::SUGGESTION, "suggestion" },
    { AnnotationType::ERRORDETECTION, "errordetection" },
    { AnnotationType::ALTERNATIVE, "alternative" },
    { AnnotationType::PHON, "phon" },
    { AnnotationType::SUBJECTIVITY, "subjectivity" },
    { AnnotationType::MORPHOLOGICAL, "morphological" },
    { AnnotationType::EVENT, "event" },
    { AnnotationType::DEPENDENCY, "dependency" },
    { AnnotationType::TIMESEGMENT, "timesegment" },
    { AnnotationType::GAP, "gap" },
    { AnnotationType::ALIGNMENT, "alignment" },
    { AnnotationType::COMPLEXALIGNMENT, "complexalignment" },
    { AnnotationType::COREFERENCE, "coreference" },
    { AnnotationType::SEMROLE, "semrole" },
    { AnnotationType::METRIC, "metric" },
    { AnnotationType::TABLE, "table" },
    { AnnotationType::LANG, "lang" },
    { AnnotationType::STYLE, "style" },
    { AnnotationType::NOTE, "note" },
    { AnnotationType::PART, "part" },
    { AnnotationType::UTTERANCE, "utterance" },
    { AnnotationType::TERM, "term" },
    { AnnotationType::DEFINITION, "definition" },
    { AnnotationType::EXAMPLE, "example" },
    { AnnotationType::PHONOLOGICAL, "phonology" }
  };

  //foliaspec:string_annotationtype_map
  map<string,AnnotationType::AnnotationType> s_ant_map;

  //foliaspec:elementtype_string_map
  const map<ElementType,string> et_s_map = {
    { BASE, "FoLiA" },
    { TextContent_t, "t" },
    { Text_t, "text" },
    { Speech_t, "speech" },
    { Event_t, "event" },
    { TimeSegment_t, "timesegment" },
    { TimingLayer_t, "timing" },
    { LineBreak_t, "br" },
    { WhiteSpace_t, "whitespace" },
    { Word_t, "w" },
    { WordReference_t, "wref" },
    { String_t, "str" },
    { Utterance_t, "utt" },
    { Entry_t, "entry" },
    { Term_t, "term" },
    { Example_t, "ex" },
    { Definition_t, "def" },
    { PhonContent_t, "ph" },
    { Sentence_t, "s" },
    { Paragraph_t, "p" },
    { Division_t, "div" },
    { Head_t, "head" },
    { Table_t, "table" },
    { TableHead_t, "tablehead" },
    { Cell_t, "cell" },
    { LangAnnotation_t, "lang" },
    { Row_t, "row" },
    { Caption_t, "caption" },
    { Label_t, "label" },
    { List_t, "list" },
    { Item_t, "item" },
    { Figure_t, "figure" },
    { Quote_t, "quote" },
    { PosAnnotation_t, "pos" },
    { LemmaAnnotation_t, "lemma" },
    { PhonologyLayer_t, "phonology" },
    { Phoneme_t, "phoneme" },
    { DomainAnnotation_t, "domain" },
    { SenseAnnotation_t, "sense" },
    { SubjectivityAnnotation_t, "subjectivity" },
    { Correction_t, "correction" },
    { AbstractTokenAnnotation_t, "abstracttokenannotation" },
    { AbstractStructureElement_t, "abstractstructure" },
    { AbstractAnnotationLayer_t, "abstractannotationlayer" },
    { AbstractSpanAnnotation_t, "abstractspanannotation" },
    { AbstractSpanRole_t, "abstractspanrole" },
    { AbstractCorrectionChild_t, "abstractcorrectionchild" },
    { AbstractTextMarkup_t, "abstracttextmarkup" },
    { SyntacticUnit_t, "su" },
    { SyntaxLayer_t, "syntax" },
    { Chunk_t, "chunk" },
    { ChunkingLayer_t, "chunking" },
    { Entity_t, "entity" },
    { EntitiesLayer_t, "entities" },
    { SemanticRolesLayer_t, "semroles" },
    { SemanticRole_t, "semrole" },
    { CoreferenceLayer_t, "coreferences" },
    { CoreferenceLink_t, "coreferencelink" },
    { CoreferenceChain_t, "coreferencechain" },
    { MorphologyLayer_t, "morphology" },
    { Morpheme_t, "morpheme" },
    { ErrorDetection_t, "errordetection" },
    { New_t, "new" },
    { Original_t, "original" },
    { Current_t, "current" },
    { Suggestion_t, "suggestion" },
    { Alternative_t, "alt" },
    { AlternativeLayers_t, "altlayers" },
    { XmlComment_t, "xml-comment" },
    { XmlText_t, "xml-text" },
    { External_t, "external" },
    { Note_t, "note" },
    { Reference_t, "ref" },
    { Description_t, "desc" },
    { Gap_t, "gap" },
    { Content_t, "content" },
    { Metric_t, "metric" },
    { Feature_t, "feat" },
    { SynsetFeature_t, "synset" },
    { ActorFeature_t, "actor" },
    { HeadFeature_t, "headfeature" },
    { ValueFeature_t, "value" },
    { TimeFeature_t, "time" },
    { ModalityFeature_t, "modality" },
    { LevelFeature_t, "level" },
    { FunctionFeature_t, "function" },
    { BeginDateTimeFeature_t, "begindatetime" },
    { EndDateTimeFeature_t, "enddatetime" },
    { PlaceHolder_t, "placeholder" },
    { DependenciesLayer_t, "dependencies" },
    { Dependency_t, "dependency" },
    { DependencyDependent_t, "dep" },
    { Headspan_t, "hd" },
    { ComplexAlignment_t, "complexalignment" },
    { ComplexAlignmentLayer_t, "complexalignments" },
    { Alignment_t, "alignment" },
    { AlignReference_t, "aref" },
    { TextMarkupString_t, "t-str" },
    { TextMarkupGap_t, "t-gap" },
    { TextMarkupCorrection_t, "t-correction" },
    { TextMarkupError_t, "t-error" },
    { TextMarkupStyle_t, "t-style" },
    { Part_t, "part" }
  };

  //foliaspec:string_elementtype_map
  map<string,ElementType> s_et_map;

  //foliaspec:default_ignore
  const set<ElementType> default_ignore =
    { Original_t,
      Suggestion_t,
      Alternative_t,
      AlternativeLayers_t,
    };

  //foliaspec:default_ignore_annotations
  const set<ElementType> default_ignore_annotations =
    { Original_t,
      Suggestion_t,
      Alternative_t,
      AlternativeLayers_t,
      MorphologyLayer_t,
      PhonologyLayer_t };

  //foliaspec:default_ignore_structure
  const set<ElementType> default_ignore_structure =
    { Original_t,
      Suggestion_t,
      Alternative_t,
      AlternativeLayers_t,
      Chunk_t,
      SyntacticUnit_t,
      CoreferenceLayer_t,
      SemanticRolesLayer_t,
      Entity_t,
      Headspan_t,
      TimingLayer_t,
      DependencyDependent_t,
      TimeSegment_t };

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

  //foliaspec:defaultproperties
  properties DEFAULT_PROPERTIES =
    { BASE, "ThIsIsSoWrOnG",
      { Description_t, XmlComment_t }, // may occur 'everywhere'
      {}, // default: no required data
      NO_ATT, NO_ATT,
      AnnotationType::NO_ANN,
      "",
      0, 1,
      "NONE",
      false, false, false, true
    };

  //these are not in the external specification yet:
  properties FoLiA::PROPS = DEFAULT_PROPERTIES;
  properties DCOI::PROPS = DEFAULT_PROPERTIES;
  properties XmlComment::PROPS = DEFAULT_PROPERTIES;

  //foliaspec:instantiateelementproperties
  properties AbstractStructureElement::PROPS = DEFAULT_PROPERTIES;
  properties AbstractTokenAnnotation::PROPS = DEFAULT_PROPERTIES;
  properties AbstractCorrectionChild::PROPS = DEFAULT_PROPERTIES;
  properties AbstractAnnotationLayer::PROPS = DEFAULT_PROPERTIES;
  properties AbstractSpanAnnotation::PROPS = DEFAULT_PROPERTIES;
  properties AbstractSpanRole::PROPS = DEFAULT_PROPERTIES;
  properties AbstractTextMarkup::PROPS = DEFAULT_PROPERTIES;
  //- (no newline so its's all one block)
  properties XmlText::PROPS = DEFAULT_PROPERTIES;
  properties Description::PROPS = DEFAULT_PROPERTIES;
  properties Alignment::PROPS = DEFAULT_PROPERTIES;
  properties ComplexAlignment::PROPS = DEFAULT_PROPERTIES;
  properties ComplexAlignmentLayer::PROPS = DEFAULT_PROPERTIES;
  properties Content::PROPS = DEFAULT_PROPERTIES;
  properties Gap::PROPS = DEFAULT_PROPERTIES;
  properties AlternativeLayers::PROPS = DEFAULT_PROPERTIES;
  properties Metric::PROPS = DEFAULT_PROPERTIES;
  properties External::PROPS = DEFAULT_PROPERTIES;
  properties AlignReference::PROPS = DEFAULT_PROPERTIES;
  properties WordReference::PROPS = DEFAULT_PROPERTIES;
  properties PhonContent::PROPS = DEFAULT_PROPERTIES;
  properties Feature::PROPS = DEFAULT_PROPERTIES;
  properties BeginDateTimeFeature::PROPS = DEFAULT_PROPERTIES;
  properties EndDateTimeFeature::PROPS = DEFAULT_PROPERTIES;
  properties SynsetFeature::PROPS = DEFAULT_PROPERTIES;
  properties ValueFeature::PROPS = DEFAULT_PROPERTIES;
  properties TimeFeature::PROPS = DEFAULT_PROPERTIES;
  properties FunctionFeature::PROPS = DEFAULT_PROPERTIES;
  properties HeadFeature::PROPS = DEFAULT_PROPERTIES;
  properties ActorFeature::PROPS = DEFAULT_PROPERTIES;
  properties ModalityFeature::PROPS = DEFAULT_PROPERTIES;
  properties LevelFeature::PROPS = DEFAULT_PROPERTIES;
  properties TextContent::PROPS = DEFAULT_PROPERTIES;
  properties LemmaAnnotation::PROPS = DEFAULT_PROPERTIES;
  properties SemanticRole::PROPS = DEFAULT_PROPERTIES;
  properties Current::PROPS = DEFAULT_PROPERTIES;
  properties Phoneme::PROPS = DEFAULT_PROPERTIES;
  properties TextMarkupError::PROPS = DEFAULT_PROPERTIES;
  properties Dependency::PROPS = DEFAULT_PROPERTIES;
  properties Word::PROPS = DEFAULT_PROPERTIES;
  properties Row::PROPS = DEFAULT_PROPERTIES;
  properties Part::PROPS = DEFAULT_PROPERTIES;
  properties PlaceHolder::PROPS = DEFAULT_PROPERTIES;
  properties SubjectivityAnnotation::PROPS = DEFAULT_PROPERTIES;
  properties CoreferenceLink::PROPS = DEFAULT_PROPERTIES;
  properties LineBreak::PROPS = DEFAULT_PROPERTIES;
  properties ChunkingLayer::PROPS = DEFAULT_PROPERTIES;
  properties MorphologyLayer::PROPS = DEFAULT_PROPERTIES;
  properties New::PROPS = DEFAULT_PROPERTIES;
  properties Speech::PROPS = DEFAULT_PROPERTIES;
  properties Quote::PROPS = DEFAULT_PROPERTIES;
  properties Event::PROPS = DEFAULT_PROPERTIES;
  properties String::PROPS = DEFAULT_PROPERTIES;
  properties Headspan::PROPS = DEFAULT_PROPERTIES;
  properties Alternative::PROPS = DEFAULT_PROPERTIES;
  properties Division::PROPS = DEFAULT_PROPERTIES;
  properties DependencyDependent::PROPS = DEFAULT_PROPERTIES;
  properties List::PROPS = DEFAULT_PROPERTIES;
  properties CoreferenceChain::PROPS = DEFAULT_PROPERTIES;
  properties Cell::PROPS = DEFAULT_PROPERTIES;
  properties TextMarkupStyle::PROPS = DEFAULT_PROPERTIES;
  properties Paragraph::PROPS = DEFAULT_PROPERTIES;
  properties Entity::PROPS = DEFAULT_PROPERTIES;
  properties SenseAnnotation::PROPS = DEFAULT_PROPERTIES;
  properties SyntacticUnit::PROPS = DEFAULT_PROPERTIES;
  properties Chunk::PROPS = DEFAULT_PROPERTIES;
  properties WhiteSpace::PROPS = DEFAULT_PROPERTIES;
  properties TableHead::PROPS = DEFAULT_PROPERTIES;
  properties Caption::PROPS = DEFAULT_PROPERTIES;
  properties Correction::PROPS = DEFAULT_PROPERTIES;
  properties Head::PROPS = DEFAULT_PROPERTIES;
  properties DomainAnnotation::PROPS = DEFAULT_PROPERTIES;
  properties SemanticRolesLayer::PROPS = DEFAULT_PROPERTIES;
  properties TextMarkupString::PROPS = DEFAULT_PROPERTIES;
  properties Note::PROPS = DEFAULT_PROPERTIES;
  properties Original::PROPS = DEFAULT_PROPERTIES;
  properties TimeSegment::PROPS = DEFAULT_PROPERTIES;
  properties TextMarkupGap::PROPS = DEFAULT_PROPERTIES;
  properties Example::PROPS = DEFAULT_PROPERTIES;
  properties DependenciesLayer::PROPS = DEFAULT_PROPERTIES;
  properties Entry::PROPS = DEFAULT_PROPERTIES;
  properties Term::PROPS = DEFAULT_PROPERTIES;
  properties TimingLayer::PROPS = DEFAULT_PROPERTIES;
  properties PhonologyLayer::PROPS = DEFAULT_PROPERTIES;
  properties Table::PROPS = DEFAULT_PROPERTIES;
  properties Reference::PROPS = DEFAULT_PROPERTIES;
  properties Suggestion::PROPS = DEFAULT_PROPERTIES;
  properties EntitiesLayer::PROPS = DEFAULT_PROPERTIES;
  properties Figure::PROPS = DEFAULT_PROPERTIES;
  properties Definition::PROPS = DEFAULT_PROPERTIES;
  properties ErrorDetection::PROPS = DEFAULT_PROPERTIES;
  properties SyntaxLayer::PROPS = DEFAULT_PROPERTIES;
  properties CoreferenceLayer::PROPS = DEFAULT_PROPERTIES;
  properties Utterance::PROPS = DEFAULT_PROPERTIES;
  properties LangAnnotation::PROPS = DEFAULT_PROPERTIES;
  properties Item::PROPS = DEFAULT_PROPERTIES;
  properties TextMarkupCorrection::PROPS = DEFAULT_PROPERTIES;
  properties Sentence::PROPS = DEFAULT_PROPERTIES;
  properties Text::PROPS = DEFAULT_PROPERTIES;
  properties PosAnnotation::PROPS = DEFAULT_PROPERTIES;
  properties Label::PROPS = DEFAULT_PROPERTIES;
  properties Morpheme::PROPS = DEFAULT_PROPERTIES;

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

    XmlComment::PROPS.XMLTAG = "xml-comment";
    XmlComment::PROPS.ELEMENT_ID = XmlComment_t;

    //foliaspec:begin:setelementproperties
    TextContent::PROPS.ELEMENT_ID = TextContent_t;
    TextContent::PROPS.XMLTAG="t";
    TextContent::PROPS.OPTIONAL_ATTRIBS = CLASS|ANNOTATOR|CONFIDENCE|DATETIME;
    TextContent::PROPS.ACCEPTED_DATA += {
      AbstractTextMarkup_t,
	XmlText_t,
	LineBreak_t };
    TextContent::PROPS.ANNOTATIONTYPE = AnnotationType::TEXT;
    TextContent::PROPS.OCCURRENCES = 0;
    TextContent::PROPS.OCCURRENCES_PER_SET=0;
    TextContent::PROPS.PRINTABLE = true;
    TextContent::PROPS.XLINK = true;

    PhonContent::PROPS.ELEMENT_ID = PhonContent_t;
    PhonContent::PROPS.XMLTAG="ph";
    PhonContent::PROPS.OPTIONAL_ATTRIBS = CLASS|ANNOTATOR|CONFIDENCE|DATETIME;
    PhonContent::PROPS.ACCEPTED_DATA += { XmlText_t };
    PhonContent::PROPS.ANNOTATIONTYPE = AnnotationType::PHON;
    PhonContent::PROPS.OCCURRENCES = 0;
    PhonContent::PROPS.OCCURRENCES_PER_SET=0;
    PhonContent::PROPS.PRINTABLE = false;
    PhonContent::PROPS.SPEAKABLE = true;

    AbstractStructureElement::PROPS.ELEMENT_ID = AbstractStructureElement_t;
    AbstractStructureElement::PROPS.XMLTAG = "structure";
    AbstractStructureElement::PROPS.REQUIRED_ATTRIBS = ID;
    AbstractStructureElement::PROPS.OPTIONAL_ATTRIBS = ALL;
    AbstractStructureElement::PROPS.OCCURRENCES_PER_SET = 0;
    AbstractStructureElement::PROPS.PRINTABLE = true;
    AbstractStructureElement::PROPS.SPEAKABLE = true;
    AbstractStructureElement::PROPS.TEXTDELIMITER = "\n\n";

    Head::PROPS = AbstractStructureElement::PROPS;
    Head::PROPS.ELEMENT_ID = Head_t;
    Head::PROPS.XMLTAG="head";
    Head::PROPS.ACCEPTED_DATA += {
      AbstractStructureElement_t,
	Correction_t,
	String_t,
	TextContent_t,
	PhonContent_t,
	Alignment_t,
	Metric_t,
	AlternativeLayers_t,
	AbstractTokenAnnotation_t,
	Gap_t };
    Head::PROPS.OCCURRENCES=1;
    Head::PROPS.TEXTDELIMITER = "\n\n";

    TableHead::PROPS = AbstractStructureElement::PROPS;
    TableHead::PROPS.ELEMENT_ID = TableHead_t;
    TableHead::PROPS.XMLTAG="tablehead";
    TableHead::PROPS.REQUIRED_ATTRIBS = NO_ATT;
    TableHead::PROPS.ACCEPTED_DATA +=
      { Row_t,
	  AlternativeLayers_t,
	  AbstractAnnotationLayer_t,
	  AbstractTokenAnnotation_t,
	  Correction_t,
	  Part_t };
    TableHead::PROPS.ANNOTATIONTYPE = AnnotationType::TABLE;

    Table::PROPS = AbstractStructureElement::PROPS;
    Table::PROPS.ELEMENT_ID = Table_t;
    Table::PROPS.XMLTAG="table";
    Table::PROPS.ACCEPTED_DATA +=
      { TableHead_t,
	  Row_t,
	  AlternativeLayers_t,
	  AbstractAnnotationLayer_t,
	  AbstractTokenAnnotation_t,
	  Correction_t,
	  Part_t };
    Table::PROPS.ANNOTATIONTYPE = AnnotationType::TABLE;

    Cell::PROPS = AbstractStructureElement::PROPS;
    Cell::PROPS.ELEMENT_ID = Cell_t;
    Cell::PROPS.XMLTAG="cell";
    Cell::PROPS.REQUIRED_ATTRIBS = NO_ATT;
    Cell::PROPS.ACCEPTED_DATA +=
      { AbstractStructureElement_t,
	  EntitiesLayer_t,
	  Alignment_t,
	  Metric_t,
	  Gap_t,
	  AlternativeLayers_t,
	  AbstractAnnotationLayer_t,
	  AbstractTokenAnnotation_t,
	  Correction_t,
	  Part_t,
	  Feature_t };

    Cell::PROPS.ANNOTATIONTYPE = AnnotationType::TABLE;
    Cell::PROPS.TEXTDELIMITER = " | ";

    Row::PROPS = AbstractStructureElement::PROPS;
    Row::PROPS.ELEMENT_ID = Row_t;
    Row::PROPS.XMLTAG="row";
    Row::PROPS.REQUIRED_ATTRIBS = NO_ATT;
    Row::PROPS.ACCEPTED_DATA +=
      { Cell_t,
	  AbstractAnnotationLayer_t,
	  AlternativeLayers_t,
	  AbstractTokenAnnotation_t,
	  Correction_t,
	  Part_t };
    Row::PROPS.ANNOTATIONTYPE = AnnotationType::TABLE;
    Row::PROPS.TEXTDELIMITER = "\n";

    LineBreak::PROPS = AbstractStructureElement::PROPS;
    LineBreak::PROPS.XMLTAG = "br";
    LineBreak::PROPS.ELEMENT_ID = LineBreak_t;
    LineBreak::PROPS.REQUIRED_ATTRIBS = NO_ATT;
    LineBreak::PROPS.ACCEPTED_DATA +=
      { Feature_t,
	  Metric_t };
    LineBreak::PROPS.ANNOTATIONTYPE = AnnotationType::LINEBREAK;

    WhiteSpace::PROPS = AbstractStructureElement::PROPS;
    WhiteSpace::PROPS.XMLTAG = "whitespace";
    WhiteSpace::PROPS.ELEMENT_ID = WhiteSpace_t;
    WhiteSpace::PROPS.REQUIRED_ATTRIBS = NO_ATT;
    WhiteSpace::PROPS.ACCEPTED_DATA +=
      { Feature_t,
	  Metric_t };
    WhiteSpace::PROPS.ANNOTATIONTYPE = AnnotationType::WHITESPACE;

    Word::PROPS = AbstractStructureElement::PROPS;
    Word::PROPS.XMLTAG="w";
    Word::PROPS.ELEMENT_ID = Word_t;
    Word::PROPS.ACCEPTED_DATA +=
      { AbstractTokenAnnotation_t,
	  Correction_t,
	  TextContent_t,
	  PhonContent_t,
	  PhonologyLayer_t,
	  String_t,
	  Alternative_t,
	  AlternativeLayers_t,
	  AbstractAnnotationLayer_t,
	  Alignment_t,
	  Metric_t,
	  Reference_t,
	  Feature_t };
    Word::PROPS.ANNOTATIONTYPE = AnnotationType::TOKEN;
    Word::PROPS.TEXTDELIMITER = " ";

    AbstractTokenAnnotation::PROPS.ELEMENT_ID = AbstractTokenAnnotation_t;
    AbstractTokenAnnotation::PROPS.XMLTAG="tokenannotation";
    AbstractTokenAnnotation::PROPS.REQUIRED_ATTRIBS = CLASS;
    AbstractTokenAnnotation::PROPS.OPTIONAL_ATTRIBS = ALL;
    AbstractTokenAnnotation::PROPS.OCCURRENCES_PER_SET=1;

    String::PROPS = AbstractTokenAnnotation::PROPS;
    String::PROPS.XMLTAG="str";
    String::PROPS.ELEMENT_ID = String_t;
    String::PROPS.REQUIRED_ATTRIBS = NO_ATT;
    String::PROPS.OPTIONAL_ATTRIBS = ID|CLASS|ANNOTATOR|CONFIDENCE|DATETIME;
    String::PROPS.ACCEPTED_DATA +=
      { TextContent_t,
	  PhonContent_t,
	  Alignment_t,
	  Metric_t,
	  Correction_t,
	  AbstractTokenAnnotation_t,
	  LangAnnotation_t,
	  Feature_t };
    String::PROPS.ANNOTATIONTYPE = AnnotationType::STRING;
    String::PROPS.OCCURRENCES = 0;
    String::PROPS.OCCURRENCES_PER_SET=0;
    String::PROPS.PRINTABLE = true;
    String::PROPS.SPEAKABLE = true;

    Part::PROPS = AbstractStructureElement::PROPS;
    Part::PROPS.XMLTAG="part";
    Part::PROPS.ELEMENT_ID = Part_t;
    Part::PROPS.REQUIRED_ATTRIBS = NO_ATT;
    Part::PROPS.ACCEPTED_DATA +=
      { AbstractStructureElement_t,
	  AbstractTokenAnnotation_t,
	  AlternativeLayers_t,
	  AbstractAnnotationLayer_t,
	  Correction_t };
    Part::PROPS.ANNOTATIONTYPE = AnnotationType::PART;
    Part::PROPS.TEXTDELIMITER = " ";

    PlaceHolder::PROPS = Word::PROPS;
    PlaceHolder::PROPS.XMLTAG="placeholder";
    PlaceHolder::PROPS.ELEMENT_ID = PlaceHolder_t;
    PlaceHolder::PROPS.REQUIRED_ATTRIBS = NO_ATT;

    WordReference::PROPS.REQUIRED_ATTRIBS = ID;
    WordReference::PROPS.XMLTAG = "wref";
    WordReference::PROPS.ELEMENT_ID = WordReference_t;

    Alignment::PROPS.OPTIONAL_ATTRIBS = ALL;
    Alignment::PROPS.XMLTAG = "alignment";
    Alignment::PROPS.ELEMENT_ID = Alignment_t;
    Alignment::PROPS.ACCEPTED_DATA +=
      { AlignReference_t,
	  Metric_t };
    Alignment::PROPS.OCCURRENCES_PER_SET=0;
    Alignment::PROPS.ANNOTATIONTYPE = AnnotationType::ALIGNMENT;
    Alignment::PROPS.XLINK = true;

    ComplexAlignmentLayer::PROPS.XMLTAG = "complexalignments";
    ComplexAlignmentLayer::PROPS.ELEMENT_ID = ComplexAlignmentLayer_t;
    ComplexAlignmentLayer::PROPS.ACCEPTED_DATA +=
      { ComplexAlignment_t,
	  Correction_t};

    ComplexAlignment::PROPS.OPTIONAL_ATTRIBS = ALL;
    ComplexAlignment::PROPS.XMLTAG = "complexalignment";
    ComplexAlignment::PROPS.ELEMENT_ID = ComplexAlignment_t;
    ComplexAlignment::PROPS.ACCEPTED_DATA +=
      { Alignment_t,
	  Metric_t,
	  Feature_t};
    ComplexAlignment::PROPS.OCCURRENCES_PER_SET=0;
    ComplexAlignment::PROPS.ANNOTATIONTYPE = AnnotationType::COMPLEXALIGNMENT;

    AlignReference::PROPS.XMLTAG = "aref";
    AlignReference::PROPS.ELEMENT_ID = AlignReference_t;

    Gap::PROPS.XMLTAG = "gap";
    Gap::PROPS.ELEMENT_ID = Gap_t;
    Gap::PROPS.ANNOTATIONTYPE = AnnotationType::GAP;
    Gap::PROPS.ACCEPTED_DATA +=
      { Content_t,
	  Part_t };
    Gap::PROPS.OPTIONAL_ATTRIBS = CLASS|ID|ANNOTATOR|CONFIDENCE|N|DATETIME;

    Metric::PROPS.ELEMENT_ID = Metric_t;
    Metric::PROPS.XMLTAG = "metric";
    Metric::PROPS.ACCEPTED_DATA +=
      { ValueFeature_t };
    Metric::PROPS.OPTIONAL_ATTRIBS = CLASS|ANNOTATOR|CONFIDENCE;
    Metric::PROPS.ANNOTATIONTYPE = AnnotationType::METRIC;

    Content::PROPS.XMLTAG = "content";
    Content::PROPS.ELEMENT_ID = Content_t;
    Content::PROPS.OPTIONAL_ATTRIBS = ALL;
    Content::PROPS.OCCURRENCES_PER_SET=0;
    Content::PROPS.PRINTABLE = true;
    Content::PROPS.SPEAKABLE = true;

    Sentence::PROPS = AbstractStructureElement::PROPS;
    Sentence::PROPS.XMLTAG="s";
    Sentence::PROPS.ELEMENT_ID = Sentence_t;
    Sentence::PROPS.ACCEPTED_DATA +=
      { AbstractStructureElement_t,
	  AbstractTokenAnnotation_t,
	  TextContent_t,
	  PhonContent_t,
	  String_t,
	  Gap_t,
	  AbstractAnnotationLayer_t,
	  Correction_t,
	  Alignment_t,
	  Metric_t,
	  AlternativeLayers_t };
    Sentence::PROPS.ANNOTATIONTYPE = AnnotationType::SENTENCE;
    Sentence::PROPS.TEXTDELIMITER = " ";

    Division::PROPS = AbstractStructureElement::PROPS;
    Division::PROPS.XMLTAG="div";
    Division::PROPS.ELEMENT_ID = Division_t;
    Division::PROPS.REQUIRED_ATTRIBS = ID;
    Division::PROPS.OPTIONAL_ATTRIBS = CLASS|N|SRC|BEGINTIME|ENDTIME|SPEAKER;
    Division::PROPS.ACCEPTED_DATA +=
      { Division_t,
	  Quote_t,
	  Gap_t,
	  Event_t,
	  Example_t,
	  Entry_t,
	  Head_t,
	  Utterance_t,
	  Paragraph_t,
	  Sentence_t,
	  List_t,
	  Figure_t,
	  Table_t,
	  Note_t,
	  Reference_t,
	  AbstractTokenAnnotation_t,
	  LineBreak_t,
	  WhiteSpace_t,
	  Alternative_t,
	  TextContent_t,
	  AlternativeLayers_t,
	  AbstractAnnotationLayer_t,
	  Word_t,
	  Correction_t,
	  Part_t };
    Division::PROPS.ANNOTATIONTYPE = AnnotationType::DIVISION;
    Division::PROPS.TEXTDELIMITER = "\n\n\n";

    Text::PROPS = AbstractStructureElement::PROPS;
    Text::PROPS.XMLTAG="text";
    Text::PROPS.ELEMENT_ID = Text_t;
    Text::PROPS.ACCEPTED_DATA +=
      { Gap_t,
	  Division_t,
	  Paragraph_t,
	  Sentence_t,
	  List_t,
	  Figure_t,
	  Event_t,
	  AbstractTokenAnnotation_t,
	  Quote_t,
	  Word_t,
	  Table_t,
	  Note_t,
	  Reference_t,
	  AbstractAnnotationLayer_t,
	  String_t,
	  Correction_t,
	  TextContent_t,
	  Metric_t,
	  External_t,
	  Part_t };
    Text::PROPS.REQUIRED_ATTRIBS = ID;
    Text::PROPS.TEXTDELIMITER = "\n\n";

    Speech::PROPS = AbstractStructureElement::PROPS;
    Speech::PROPS.XMLTAG="speech";
    Speech::PROPS.
      ELEMENT_ID = Speech_t;
    Speech::PROPS.ACCEPTED_DATA +=
      { Utterance_t,
	  Gap_t,
	  Event_t,
	  Entry_t,
	  Example_t,
	  Division_t,
	  Paragraph_t,
	  Quote_t,
	  Sentence_t,
	  Word_t,
	  List_t,
	  Note_t,
	  Reference_t,
	  AbstractAnnotationLayer_t,
	  AbstractTokenAnnotation_t,
	  TextContent_t,
	  PhonContent_t,
	  String_t,
	  Metric_t,
	  Correction_t };
    Speech::PROPS.REQUIRED_ATTRIBS = ID;
    Speech::PROPS.OPTIONAL_ATTRIBS = N|SRC|BEGINTIME|ENDTIME|SPEAKER;
    Speech::PROPS.TEXTDELIMITER = "\n\n";

    Utterance::PROPS = AbstractStructureElement::PROPS;
    Utterance::PROPS.XMLTAG="utt";
    Utterance::PROPS.ELEMENT_ID = Utterance_t;
    Utterance::PROPS.ACCEPTED_DATA +=
      { Word_t,
	  Sentence_t,
	  Quote_t,
	  AbstractTokenAnnotation_t,
	  Correction_t,
	  TextContent_t,
	  PhonContent_t,
	  String_t,
	  Gap_t,
	  Note_t,
	  Reference_t,
	  Alignment_t,
	  Metric_t,
	  Alternative_t,
	  AlternativeLayers_t,
	  AbstractAnnotationLayer_t,
	  Part_t };
    Utterance::PROPS.ANNOTATIONTYPE = AnnotationType::UTTERANCE;
    Utterance::PROPS.TEXTDELIMITER = " ";
    Utterance::PROPS.PRINTABLE = true;
    Utterance::PROPS.SPEAKABLE = true;

    Event::PROPS = AbstractStructureElement::PROPS;
    Event::PROPS.XMLTAG="event";
    Event::PROPS.ELEMENT_ID = Event_t;
    Event::PROPS.ACCEPTED_DATA +=
      { Gap_t,
	  Division_t,
	  AbstractStructureElement_t,
	  Feature_t,
	  TextContent_t,
	  Part_t };
    Event::PROPS.ANNOTATIONTYPE = AnnotationType::EVENT;
    Event::PROPS.OCCURRENCES_PER_SET=0;

    AbstractSpanAnnotation::PROPS.XMLTAG = "spanannotation";
    AbstractSpanAnnotation::PROPS.ELEMENT_ID = AbstractSpanAnnotation_t;
    AbstractSpanAnnotation::PROPS.REQUIRED_ATTRIBS = NO_ATT;
    AbstractSpanAnnotation::PROPS.OPTIONAL_ATTRIBS = ALL;
    AbstractSpanAnnotation::PROPS.OCCURRENCES_PER_SET = 0;
    AbstractSpanAnnotation::PROPS. PRINTABLE = true;
    AbstractSpanAnnotation::PROPS.SPEAKABLE = true;
    AbstractSpanAnnotation::PROPS.ACCEPTED_DATA +=
      { Metric_t,
	  Alignment_t };

    TimeSegment::PROPS = AbstractSpanAnnotation::PROPS;
    TimeSegment::PROPS.XMLTAG="timesegment";
    TimeSegment::PROPS.ELEMENT_ID = TimeSegment_t;
    TimeSegment::PROPS.ACCEPTED_DATA +=
      { Feature_t,
	  Word_t };
    TimeSegment::PROPS.ANNOTATIONTYPE = AnnotationType::TIMESEGMENT;
    TimeSegment::PROPS.OCCURRENCES_PER_SET=0;

    Caption::PROPS = AbstractStructureElement::PROPS;
    Caption::PROPS.XMLTAG="caption";
    Caption::PROPS.ELEMENT_ID = Caption_t;
    Caption::PROPS.ACCEPTED_DATA +=
      { Sentence_t,
	  Reference_t,
	  AbstractTokenAnnotation_t,
	  TextContent_t,
	  Correction_t,
	  Part_t };
    Caption::PROPS.OCCURRENCES = 1;

    Label::PROPS = AbstractStructureElement::PROPS;
    Label::PROPS.XMLTAG="label";
    Label::PROPS.ELEMENT_ID = Label_t;
    Label::PROPS.ACCEPTED_DATA +=
      { Word_t,
	  TextContent_t,
	  AbstractTokenAnnotation_t,
	  Alignment_t,
	  Correction_t,
	  Part_t };

    Item::PROPS = AbstractStructureElement::PROPS;
    Item::PROPS.XMLTAG="item";
    Item::PROPS.ELEMENT_ID = Item_t;
    Item::PROPS.ACCEPTED_DATA +=
      { AbstractStructureElement_t,
	  TextContent_t,
	  PhonContent_t,
	  String_t,
	  Gap_t,
	  Alignment_t,
	  Metric_t,
	  Alternative_t,
	  AlternativeLayers_t,
	  AbstractAnnotationLayer_t,
	  AbstractTokenAnnotation_t,
	  Correction_t,
	  Feature_t };
    Item::PROPS.ANNOTATIONTYPE = AnnotationType::LIST;

    List::PROPS = AbstractStructureElement::PROPS;
    List::PROPS.XMLTAG="list";
    List::PROPS.ELEMENT_ID = List_t;
    List::PROPS.ACCEPTED_DATA +=
      { AbstractStructureElement_t,
	  Caption_t,
	  Event_t,
	  LangAnnotation_t,
	  TextContent_t,
	  PhonContent_t,
	  Alignment_t,
	  Correction_t,
	  Feature_t };
    List::PROPS.ANNOTATIONTYPE = AnnotationType::LIST;
    List::PROPS.TEXTDELIMITER="\n";

    Figure::PROPS = AbstractStructureElement::PROPS;
    Figure::PROPS.XMLTAG="figure";
    Figure::PROPS.ELEMENT_ID = Figure_t;
    Figure::PROPS.ACCEPTED_DATA +=
      { Sentence_t,
	  Caption_t,
	  TextContent_t,
	  PhonContent_t,
	  String_t,
	  Alignment_t,
	  Metric_t,
	  Alternative_t,
	  AlternativeLayers_t,
	  AbstractAnnotationLayer_t,
	  LangAnnotation_t,
	  Correction_t,
	  Part_t };
    Figure::PROPS.ANNOTATIONTYPE = AnnotationType::FIGURE;

    Paragraph::PROPS = AbstractStructureElement::PROPS;
    Paragraph::PROPS.XMLTAG="p";
    Paragraph::PROPS.ELEMENT_ID = Paragraph_t;
    Paragraph::PROPS.ACCEPTED_DATA +=
      { Sentence_t,
	  Word_t,
	  Quote_t,
	  Example_t,
	  Entry_t,
	  AbstractTokenAnnotation_t,
	  Correction_t,
	  TextContent_t,
	  PhonContent_t,
	  String_t,
	  LineBreak_t,
	  WhiteSpace_t,
	  Gap_t,
	  List_t,
	  Figure_t,
	  Event_t,
	  Head_t,
	  Note_t,
	  Reference_t,
	  Alignment_t,
	  Metric_t,
	  Alternative_t,
	  AlternativeLayers_t,
	  AbstractAnnotationLayer_t,
	  Part_t };
    Paragraph::PROPS.ANNOTATIONTYPE = AnnotationType::PARAGRAPH;


    SyntacticUnit::PROPS = AbstractSpanAnnotation::PROPS;
    SyntacticUnit::PROPS.XMLTAG = "su";
    SyntacticUnit::PROPS.ELEMENT_ID = SyntacticUnit_t;
    SyntacticUnit::PROPS.REQUIRED_ATTRIBS = NO_ATT;
    SyntacticUnit::PROPS.ANNOTATIONTYPE = AnnotationType::SYNTAX;
    SyntacticUnit::PROPS.ACCEPTED_DATA +=
      { SyntacticUnit_t,
	  Word_t,
	  WordReference_t,
	  Feature_t };

    SemanticRole::PROPS = AbstractSpanAnnotation::PROPS;
    SemanticRole::PROPS.XMLTAG = "semrole";
    SemanticRole::PROPS.ELEMENT_ID = SemanticRole_t;
    SemanticRole::PROPS.REQUIRED_ATTRIBS = CLASS;
    SemanticRole::PROPS.ANNOTATIONTYPE = AnnotationType::SEMROLE;
    SemanticRole::PROPS.ACCEPTED_DATA +=
      { Word_t,
	  WordReference_t,
	  LangAnnotation_t,
	  Headspan_t,
	  Alignment_t,
	  Metric_t };

    Chunk::PROPS = AbstractSpanAnnotation::PROPS;
    Chunk::PROPS.REQUIRED_ATTRIBS = NO_ATT;
    Chunk::PROPS.XMLTAG = "chunk";
    Chunk::PROPS.ELEMENT_ID = Chunk_t;
    Chunk::PROPS.ANNOTATIONTYPE = AnnotationType::CHUNKING;
    Chunk::PROPS.ACCEPTED_DATA +=
      { Word_t,
	  WordReference_t,
	  LangAnnotation_t,
	  Feature_t };

    Entity::PROPS = AbstractSpanAnnotation::PROPS;
    Entity::PROPS.REQUIRED_ATTRIBS = NO_ATT;
    Entity::PROPS.OPTIONAL_ATTRIBS = ID|CLASS|ANNOTATOR|CONFIDENCE|DATETIME;
    Entity::PROPS.XMLTAG = "entity";
    Entity::PROPS.ELEMENT_ID = Entity_t;
    Entity::PROPS.ANNOTATIONTYPE = AnnotationType::ENTITY;
    Entity::PROPS.ACCEPTED_DATA +=
      { Word_t,
	  LangAnnotation_t,
	  WordReference_t,
	  Morpheme_t,
	  Feature_t,
	  Metric_t };


    Alternative::PROPS = AbstractStructureElement::PROPS;
    Alternative::PROPS.XMLTAG = "alt";
    Alternative::PROPS.ELEMENT_ID = Alternative_t;
    Alternative::PROPS.REQUIRED_ATTRIBS = NO_ATT;
    Alternative::PROPS.OPTIONAL_ATTRIBS = ALL;
    Alternative::PROPS.ACCEPTED_DATA +=
      { AbstractTokenAnnotation_t,
	  Correction_t,
	  MorphologyLayer_t,
	  PhonologyLayer_t
	  };
    Alternative::PROPS.ANNOTATIONTYPE = AnnotationType::ALTERNATIVE;

    AlternativeLayers::PROPS.XMLTAG = "altlayers";
    AlternativeLayers::PROPS.ELEMENT_ID = AlternativeLayers_t;
    AlternativeLayers::PROPS.OPTIONAL_ATTRIBS = ALL;
    AlternativeLayers::PROPS.ACCEPTED_DATA += { AbstractAnnotationLayer_t };

    AbstractCorrectionChild::PROPS.XMLTAG = "correctionchild";
    AbstractCorrectionChild::PROPS.ELEMENT_ID = AbstractCorrectionChild_t;
    AbstractCorrectionChild::PROPS.OPTIONAL_ATTRIBS = NO_ATT;
    AbstractCorrectionChild::PROPS.ACCEPTED_DATA +=
      { AbstractTokenAnnotation_t,
	  AbstractSpanAnnotation_t,
	  Word_t,
	  WordReference_t,
	  String_t,
	  TextContent_t };
    AbstractCorrectionChild::PROPS.OCCURRENCES = 1;
    AbstractCorrectionChild::PROPS.PRINTABLE=true;
    AbstractCorrectionChild::PROPS.SPEAKABLE=true;

    New::PROPS = AbstractCorrectionChild::PROPS;
    New::PROPS.XMLTAG = "new";
    New::PROPS.ELEMENT_ID = New_t;

    Current::PROPS = AbstractCorrectionChild::PROPS;
    Current::PROPS.XMLTAG = "current";
    Current::PROPS.ELEMENT_ID = Current_t;

    Original::PROPS = AbstractCorrectionChild::PROPS;
    Original::PROPS.XMLTAG = "original";
    Original::PROPS.ELEMENT_ID = Original_t;

    Suggestion::PROPS = AbstractCorrectionChild::PROPS;
    Suggestion::PROPS.XMLTAG = "suggestion";
    Suggestion::PROPS.ELEMENT_ID = Suggestion_t;
    Suggestion::PROPS.OPTIONAL_ATTRIBS = ANNOTATOR|CONFIDENCE|DATETIME|N;
    Suggestion::PROPS.ANNOTATIONTYPE = AnnotationType::SUGGESTION;
    Suggestion::PROPS.OCCURRENCES=0;
    Suggestion::PROPS.OCCURRENCES_PER_SET=0;

    Correction::PROPS = AbstractTokenAnnotation::PROPS;
    Correction::PROPS.XMLTAG = "correction";
    Correction::PROPS.ELEMENT_ID = Correction_t;
    Correction::PROPS.REQUIRED_ATTRIBS = NO_ATT;
    Correction::PROPS.ANNOTATIONTYPE = AnnotationType::CORRECTION;
    Correction::PROPS.ACCEPTED_DATA +=
      { New_t,
	  Original_t,
	  Suggestion_t,
	  Current_t,
	  ErrorDetection_t,
	  Feature_t };
    Correction::PROPS.OCCURRENCES_PER_SET=0;
    Correction::PROPS.PRINTABLE=true;
    Correction::PROPS.SPEAKABLE=true;

    Description::PROPS.XMLTAG = "desc";
    Description::PROPS.ELEMENT_ID = Description_t;
    Description::PROPS.OCCURRENCES = 1;

    Entry::PROPS = AbstractStructureElement::PROPS;
    Entry::PROPS.XMLTAG = "entry";
    Entry::PROPS.ELEMENT_ID = Entry_t;
    Entry::PROPS.ACCEPTED_DATA +=
      { Term_t,
	  Definition_t,
	  Example_t,
	  Correction_t,
	  Metric_t,
	  Alignment_t,
	  AlternativeLayers_t,
	  AbstractAnnotationLayer_t };

    Definition::PROPS = AbstractStructureElement::PROPS;
    Definition::PROPS.XMLTAG = "def";
    Definition::PROPS.ELEMENT_ID = Definition_t;
    Definition::PROPS.ACCEPTED_DATA +=
      { Paragraph_t,
	  Sentence_t,
	  Word_t,
	  Utterance_t,
	  List_t,
	  Figure_t,
	  Table_t,
	  Reference_t,
	  Feature_t,
	  TextContent_t,
	  PhonContent_t,
	  String_t,
	  Metric_t,
	  AbstractTokenAnnotation_t,
	  Correction_t,
	  Part_t };
    Definition::PROPS.ANNOTATIONTYPE = AnnotationType::DEFINITION;

    Term::PROPS = AbstractStructureElement::PROPS;
    Term::PROPS.XMLTAG = "term";
    Term::PROPS.ELEMENT_ID = Term_t;
    Term::PROPS.ACCEPTED_DATA +=
      { Paragraph_t, Event_t, Sentence_t, Word_t, Utterance_t,
	List_t, Figure_t, Table_t, Reference_t, Feature_t,
	TextContent_t, PhonContent_t, String_t, Metric_t,
	AbstractTokenAnnotation_t, Correction_t, Part_t };
    Term::PROPS.ANNOTATIONTYPE = AnnotationType::TERM;

    Example::PROPS = AbstractStructureElement::PROPS;
    Example::PROPS.XMLTAG = "ex";
    Example::PROPS.ELEMENT_ID = Example_t;
    Example::PROPS.ACCEPTED_DATA +=
      { Paragraph_t, Sentence_t, Word_t, Utterance_t,
	  List_t, Figure_t, Table_t, Reference_t, Feature_t,
	  TextContent_t, PhonContent_t, String_t, Metric_t,
	  AbstractTokenAnnotation_t, Correction_t, Part_t };
    Example::PROPS.ANNOTATIONTYPE = AnnotationType::EXAMPLE;

    XmlText::PROPS.XMLTAG = "xml-text";
    XmlText::PROPS.ELEMENT_ID = XmlText_t;
    XmlText::PROPS.TEXTDELIMITER = "*";
    XmlText::PROPS.PRINTABLE = true;
    XmlText::PROPS.SPEAKABLE = true;

    External::PROPS.XMLTAG = "external";
    External::PROPS.ELEMENT_ID = External_t;
    External::PROPS.REQUIRED_ATTRIBS = SRC;
    External::PROPS.PRINTABLE = true;
    External::PROPS.SPEAKABLE = false;

    Note::PROPS = AbstractStructureElement::PROPS;
    Note::PROPS.REQUIRED_ATTRIBS = ID;
    Note::PROPS.XMLTAG = "note";
    Note::PROPS.ELEMENT_ID = Note_t;
    Note::PROPS.ACCEPTED_DATA +=
      { Paragraph_t,
				   Sentence_t,
				   Word_t,
				   Example_t,
				   Head_t,
				   Utterance_t,
				   List_t,
				   Figure_t,
				   Table_t,
				   Reference_t,
				   Feature_t,
				   TextContent_t,
				   PhonContent_t,
				   String_t,
				   Metric_t,
				   AbstractTokenAnnotation_t,
				   Correction_t,
				   Part_t };
    Note::PROPS.ANNOTATIONTYPE = AnnotationType::NOTE;

    Reference::PROPS = AbstractStructureElement::PROPS;
    Reference::PROPS.XMLTAG = "ref";
    Reference::PROPS.ELEMENT_ID = Reference_t;
    Reference::PROPS.REQUIRED_ATTRIBS = NO_ATT;
    Reference::PROPS.OPTIONAL_ATTRIBS = ID|ANNOTATOR|CONFIDENCE|DATETIME;
    Reference::PROPS.ACCEPTED_DATA +=
      { TextContent_t, PhonContent_t, String_t,
	Metric_t };


    Morpheme::PROPS = AbstractStructureElement::PROPS;
    Morpheme::PROPS.ELEMENT_ID = Morpheme_t;
    Morpheme::PROPS.XMLTAG = "morpheme";
    Morpheme::PROPS.REQUIRED_ATTRIBS = NO_ATT;
    Morpheme::PROPS.OPTIONAL_ATTRIBS = ALL;
    Morpheme::PROPS.ACCEPTED_DATA +=
      { Feature_t, FunctionFeature_t, TextContent_t,
	Metric_t, Alignment_t, AbstractTokenAnnotation_t,
	Morpheme_t };
    Morpheme::PROPS.ANNOTATIONTYPE = AnnotationType::MORPHOLOGICAL;

    AbstractAnnotationLayer::PROPS.XMLTAG = "annotationlayer";
    AbstractAnnotationLayer::PROPS.ELEMENT_ID = AbstractAnnotationLayer_t;
    AbstractAnnotationLayer::PROPS.OPTIONAL_ATTRIBS = SETONLY;

    SyntaxLayer::PROPS = AbstractAnnotationLayer::PROPS;
    SyntaxLayer::PROPS.ELEMENT_ID = SyntaxLayer_t;
    SyntaxLayer::PROPS.XMLTAG = "syntax";
    SyntaxLayer::PROPS.ACCEPTED_DATA += { SyntacticUnit_t,
					  Correction_t };
    SyntaxLayer::PROPS.ANNOTATIONTYPE = AnnotationType::SYNTAX;

    ChunkingLayer::PROPS = AbstractAnnotationLayer::PROPS;
    ChunkingLayer::PROPS.ELEMENT_ID = ChunkingLayer_t;
    ChunkingLayer::PROPS.XMLTAG = "chunking";
    ChunkingLayer::PROPS.ACCEPTED_DATA += { Chunk_t,
					    Correction_t };
    ChunkingLayer::PROPS.ANNOTATIONTYPE = AnnotationType::CHUNKING;

    EntitiesLayer::PROPS = AbstractAnnotationLayer::PROPS;
    EntitiesLayer::PROPS.ELEMENT_ID = EntitiesLayer_t;
    EntitiesLayer::PROPS.XMLTAG = "entities";
    EntitiesLayer::PROPS.ACCEPTED_DATA += { Entity_t,
					    Correction_t };
    EntitiesLayer::PROPS.ANNOTATIONTYPE = AnnotationType::ENTITY;

    TimingLayer::PROPS = AbstractAnnotationLayer::PROPS;
    TimingLayer::PROPS.ELEMENT_ID = TimingLayer_t;
    TimingLayer::PROPS.XMLTAG = "timing";
    TimingLayer::PROPS.ACCEPTED_DATA += { TimeSegment_t,
					  Correction_t };

    MorphologyLayer::PROPS = AbstractAnnotationLayer::PROPS;
    MorphologyLayer::PROPS.ELEMENT_ID = MorphologyLayer_t;
    MorphologyLayer::PROPS.XMLTAG = "morphology";
    MorphologyLayer::PROPS.ACCEPTED_DATA += { Morpheme_t, Correction_t };
    MorphologyLayer::PROPS.OCCURRENCES_PER_SET = 1;
    MorphologyLayer::PROPS.ANNOTATIONTYPE = AnnotationType::MORPHOLOGICAL;

    PhonologyLayer::PROPS = AbstractAnnotationLayer::PROPS;
    PhonologyLayer::PROPS.ELEMENT_ID = PhonologyLayer_t;
    PhonologyLayer::PROPS.XMLTAG = "phonology";
    PhonologyLayer::PROPS.ACCEPTED_DATA += { Phoneme_t, Correction_t };
    PhonologyLayer::PROPS.OCCURRENCES_PER_SET = 1;
    PhonologyLayer::PROPS.ANNOTATIONTYPE = AnnotationType::PHONOLOGICAL;

    CoreferenceLayer::PROPS = AbstractAnnotationLayer::PROPS;
    CoreferenceLayer::PROPS.ELEMENT_ID = CoreferenceLayer_t;
    CoreferenceLayer::PROPS.XMLTAG = "coreferences";
    CoreferenceLayer::PROPS.ACCEPTED_DATA += { CoreferenceChain_t,
					       Correction_t };
    CoreferenceLayer::PROPS.ANNOTATIONTYPE = AnnotationType::COREFERENCE;

    AbstractSpanRole::PROPS.XMLTAG = "spanrole";
    AbstractSpanRole::PROPS.ELEMENT_ID = AbstractSpanRole_t;
    AbstractSpanRole::PROPS.REQUIRED_ATTRIBS = NO_ATT;
    AbstractSpanRole::PROPS.OPTIONAL_ATTRIBS = ANNOTATOR|N|DATETIME;
    AbstractSpanRole::PROPS.ACCEPTED_DATA +=
      { Feature_t,
	  Word_t,
	  WordReference_t };

    CoreferenceLink::PROPS = AbstractSpanRole::PROPS;
    CoreferenceLink::PROPS.ELEMENT_ID = CoreferenceLink_t;
    CoreferenceLink::PROPS.XMLTAG = "coreferencelink";
    CoreferenceLink::PROPS.ACCEPTED_DATA +=
      {   Headspan_t,
	  TimeFeature_t,
	  LevelFeature_t,
	  ModalityFeature_t };
    CoreferenceLink::PROPS.ANNOTATIONTYPE = AnnotationType::COREFERENCE;

    CoreferenceChain::PROPS = AbstractSpanAnnotation::PROPS;
    CoreferenceChain::PROPS.ELEMENT_ID = CoreferenceChain_t;
    CoreferenceChain::PROPS.XMLTAG = "coreferencechain";
    CoreferenceChain::PROPS.REQUIRED_ATTRIBS = NO_ATT;
    CoreferenceChain::PROPS.ACCEPTED_DATA += { CoreferenceLink_t,
					       Metric_t };
    CoreferenceChain::PROPS.ANNOTATIONTYPE = AnnotationType::COREFERENCE;

    SemanticRolesLayer::PROPS = AbstractAnnotationLayer::PROPS;
    SemanticRolesLayer::PROPS.ELEMENT_ID = SemanticRolesLayer_t;
    SemanticRolesLayer::PROPS.XMLTAG = "semroles";
    SemanticRolesLayer::PROPS.ACCEPTED_DATA +=
      { SemanticRole_t,
	  Correction_t };
    SemanticRolesLayer::PROPS.ANNOTATIONTYPE = AnnotationType::SEMROLE;

    DependenciesLayer::PROPS = AbstractAnnotationLayer::PROPS;
    DependenciesLayer::PROPS.ELEMENT_ID = DependenciesLayer_t;
    DependenciesLayer::PROPS.XMLTAG = "dependencies";
    DependenciesLayer::PROPS.ACCEPTED_DATA += { Dependency_t,
						Correction_t };
    DependenciesLayer::PROPS.ANNOTATIONTYPE = AnnotationType::DEPENDENCY;

    Dependency::PROPS = AbstractSpanAnnotation::PROPS;
    Dependency::PROPS.ELEMENT_ID = Dependency_t;
    Dependency::PROPS.XMLTAG = "dependency";
    Dependency::PROPS.REQUIRED_ATTRIBS = NO_ATT;
    Dependency::PROPS.ANNOTATIONTYPE = AnnotationType::DEPENDENCY;
    Dependency::PROPS.ACCEPTED_DATA +=
      { DependencyDependent_t,
	  Headspan_t,
	  Feature_t,
	  Alignment_t };

    DependencyDependent::PROPS = AbstractSpanRole::PROPS;
    DependencyDependent::PROPS.ELEMENT_ID = DependencyDependent_t;
    DependencyDependent::PROPS.XMLTAG = "dep";
    DependencyDependent::PROPS.ANNOTATIONTYPE = AnnotationType::DEPENDENCY;

    Headspan::PROPS = AbstractSpanRole::PROPS;
    Headspan::PROPS.ELEMENT_ID = Headspan_t;
    Headspan::PROPS.XMLTAG = "hd";

    PosAnnotation::PROPS = AbstractTokenAnnotation::PROPS;
    PosAnnotation::PROPS.XMLTAG="pos";
    PosAnnotation::PROPS.ELEMENT_ID = PosAnnotation_t;
    PosAnnotation::PROPS.ANNOTATIONTYPE = AnnotationType::POS;
    PosAnnotation::PROPS.ACCEPTED_DATA +=
      { Feature_t,
	  Metric_t };

    LemmaAnnotation::PROPS = AbstractTokenAnnotation::PROPS;
    LemmaAnnotation::PROPS.XMLTAG="lemma";
    LemmaAnnotation::PROPS.ELEMENT_ID = LemmaAnnotation_t;
    LemmaAnnotation::PROPS.ANNOTATIONTYPE = AnnotationType::LEMMA;
    LemmaAnnotation::PROPS.ACCEPTED_DATA +=
      { Feature_t,
	  Metric_t };

    LangAnnotation::PROPS = AbstractTokenAnnotation::PROPS;
    LangAnnotation::PROPS.XMLTAG="lang";
    LangAnnotation::PROPS.ELEMENT_ID = LangAnnotation_t;
    LangAnnotation::PROPS.ANNOTATIONTYPE = AnnotationType::LANG;
    LangAnnotation::PROPS.ACCEPTED_DATA +=
      { Feature_t,
	  Metric_t };

    Phoneme::PROPS = AbstractTokenAnnotation::PROPS;
    Phoneme::PROPS.XMLTAG="phoneme";
    Phoneme::PROPS.ELEMENT_ID = Phoneme_t;
    Phoneme::PROPS.REQUIRED_ATTRIBS = NO_ATT;
    Phoneme::PROPS.OPTIONAL_ATTRIBS = ALL;
    Phoneme::PROPS.ANNOTATIONTYPE = AnnotationType::PHON;
    Phoneme::PROPS.ACCEPTED_DATA +=
      { PhonContent_t,
	  Feature_t  };

    DomainAnnotation::PROPS = AbstractTokenAnnotation::PROPS;
    DomainAnnotation::PROPS.XMLTAG="domain";
    DomainAnnotation::PROPS.ELEMENT_ID = DomainAnnotation_t;
    DomainAnnotation::PROPS.ANNOTATIONTYPE = AnnotationType::DOMAIN;
    DomainAnnotation::PROPS.ACCEPTED_DATA += { Feature_t };

    SenseAnnotation::PROPS = AbstractTokenAnnotation::PROPS;
    SenseAnnotation::PROPS.XMLTAG="sense";
    SenseAnnotation::PROPS.ELEMENT_ID = SenseAnnotation_t;
    SenseAnnotation::PROPS.ANNOTATIONTYPE = AnnotationType::SENSE;
    SenseAnnotation::PROPS.ACCEPTED_DATA +=
      { Feature_t, SynsetFeature_t, Metric_t };

    SubjectivityAnnotation::PROPS = AbstractTokenAnnotation::PROPS;
    SubjectivityAnnotation::PROPS.XMLTAG="subjectivity";
    SubjectivityAnnotation::PROPS.ELEMENT_ID = SubjectivityAnnotation_t;
    SubjectivityAnnotation::PROPS.ANNOTATIONTYPE = AnnotationType::SUBJECTIVITY;
    SubjectivityAnnotation::PROPS.ACCEPTED_DATA += { Feature_t };

    Quote::PROPS = AbstractStructureElement::PROPS;
    Quote::PROPS.XMLTAG="quote";
    Quote::PROPS.ELEMENT_ID = Quote_t;
    Quote::PROPS.REQUIRED_ATTRIBS = NO_ATT;
    Quote::PROPS.ACCEPTED_DATA +=
      { AbstractStructureElement_t,
	  String_t,
	  LangAnnotation_t,
	  TextContent_t,
	  Alignment_t,
	  Gap_t };
    Quote::PROPS.TEXTDELIMITER = " ";

    Feature::PROPS.XMLTAG = "feat";
    Feature::PROPS.ELEMENT_ID = Feature_t;
    Feature::PROPS.OCCURRENCES_PER_SET = 0;

    BeginDateTimeFeature::PROPS.XMLTAG="begindatetime";
    BeginDateTimeFeature::PROPS.ELEMENT_ID = BeginDateTimeFeature_t;

    EndDateTimeFeature::PROPS.XMLTAG="enddatetime";
    EndDateTimeFeature::PROPS.ELEMENT_ID = EndDateTimeFeature_t;

    SynsetFeature::PROPS.XMLTAG="synset";
    SynsetFeature::PROPS.ELEMENT_ID = SynsetFeature_t;
    SynsetFeature::PROPS.ANNOTATIONTYPE = AnnotationType::SENSE;

    ActorFeature::PROPS.XMLTAG = "actor";
    ActorFeature::PROPS.ELEMENT_ID = ActorFeature_t;

    HeadFeature::PROPS.XMLTAG = "headfeature";
    HeadFeature::PROPS.ELEMENT_ID = HeadFeature_t;

    ValueFeature::PROPS.XMLTAG = "value";
    ValueFeature::PROPS.ELEMENT_ID = ValueFeature_t;

    FunctionFeature::PROPS.XMLTAG = "function";
    FunctionFeature::PROPS.ELEMENT_ID = FunctionFeature_t;

    LevelFeature::PROPS.XMLTAG = "level";
    LevelFeature::PROPS.ELEMENT_ID = LevelFeature_t;

    ModalityFeature::PROPS.XMLTAG = "modality";
    ModalityFeature::PROPS.ELEMENT_ID = ModalityFeature_t;

    TimeFeature::PROPS.XMLTAG = "time";
    TimeFeature::PROPS.ELEMENT_ID = TimeFeature_t;

    ErrorDetection::PROPS = AbstractTokenAnnotation::PROPS;
    ErrorDetection::PROPS.
      XMLTAG = "errordetection";
    ErrorDetection::PROPS.
      ELEMENT_ID = ErrorDetection_t;
    ErrorDetection::PROPS.
      ANNOTATIONTYPE = AnnotationType::ERRORDETECTION;
    ErrorDetection::PROPS.
      OCCURRENCES_PER_SET = 0; // Allow duplicates within the same set

    AbstractTextMarkup::PROPS.XMLTAG = "textmarkup";
    AbstractTextMarkup::PROPS.ELEMENT_ID = AbstractTextMarkup_t;
    AbstractTextMarkup::PROPS.REQUIRED_ATTRIBS = NO_ATT;
    AbstractTextMarkup::PROPS.OPTIONAL_ATTRIBS = ALL;
    AbstractTextMarkup::PROPS.ANNOTATIONTYPE = AnnotationType::NO_ANN;
    AbstractTextMarkup::PROPS.ACCEPTED_DATA +=
      { AbstractTextMarkup_t, XmlText_t };
    AbstractTextMarkup::PROPS.PRINTABLE = true;
    AbstractTextMarkup::PROPS.XLINK = true;
    AbstractTextMarkup::PROPS.TEXTDELIMITER = "";
    AbstractTextMarkup::PROPS.OCCURRENCES_PER_SET = 0;

    TextMarkupString::PROPS = AbstractTextMarkup::PROPS;
    TextMarkupString::PROPS.XMLTAG = "t-str";
    TextMarkupString::PROPS.ELEMENT_ID = TextMarkupString_t;
    TextMarkupString::PROPS.ANNOTATIONTYPE = AnnotationType::STRING;

    TextMarkupGap::PROPS = AbstractTextMarkup::PROPS;
    TextMarkupGap::PROPS.XMLTAG = "t-gap";
    TextMarkupGap::PROPS.ELEMENT_ID = TextMarkupGap_t;
    TextMarkupGap::PROPS.ANNOTATIONTYPE = AnnotationType::GAP;

    TextMarkupCorrection::PROPS = AbstractTextMarkup::PROPS;
    TextMarkupCorrection::PROPS.XMLTAG = "t-correction";
    TextMarkupCorrection::PROPS.ELEMENT_ID = TextMarkupCorrection_t;
    TextMarkupCorrection::PROPS.ANNOTATIONTYPE = AnnotationType::CORRECTION;

    TextMarkupError::PROPS = AbstractTextMarkup::PROPS;
    TextMarkupError::PROPS.XMLTAG = "t-error";
    TextMarkupError::PROPS.ELEMENT_ID = TextMarkupError_t;
    TextMarkupError::PROPS.ANNOTATIONTYPE = AnnotationType::ERRORDETECTION;

    TextMarkupStyle::PROPS = AbstractTextMarkup::PROPS;
    TextMarkupStyle::PROPS.XMLTAG = "t-style";
    TextMarkupStyle::PROPS.ELEMENT_ID = TextMarkupStyle_t;
    TextMarkupStyle::PROPS.ANNOTATIONTYPE = AnnotationType::STYLE;
    //foliaspec:end:setelementproperties

  }

  void fill_transmaps(){
    for ( const auto& it : et_s_map ){
      s_et_map[it.second] = it.first;
    }
    for ( const auto& it : ant_s_map ){
      s_ant_map[it.second] = it.first;
    }
  }

  //foliaspec:typehierarchy
  static map<ElementType,set<ElementType> > typeHierarchy;

  bool isSubClass( const FoliaElement *e1, const FoliaElement *e2 ){
    return isSubClass( e1->element_id(), e2->element_id() );
  }

  bool isSubClass( const ElementType e1, const ElementType e2 ){
    if ( e1 == e2 )
      return true;
    const auto& it = typeHierarchy.find( e2 );
    if ( it != typeHierarchy.end() ){
      return it->second.find( e1 ) != it->second.end();
    }
    return false;
  }

  bool FoliaElement::isSubClass( ElementType t ) const {
    return folia::isSubClass( element_id(), t );
  }

  void fill_hierarchy(){
    ElementType et1 = BASE;
    while ( ++et1 != LastElement ){
      FoliaElement *el = 0;
      try {
	el = FoliaImpl::createElement( 0, et1 );
      }
      catch ( exception& e ){
	//	cerr << e.what() << endl;
      }
      if ( el == 0 )
	continue;
      ElementType et2 = BASE;
      while ( ++et2 != LastElement ){
	if ( et2 != et1  && el->has_base( et2 ) ){
	  typeHierarchy[et2].insert( et1 );
	}
      }
      delete el;
    }
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
	fill_transmaps();
	fill_hierarchy();
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
