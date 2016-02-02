#include <set>
#include <string>
#include <iostream>

#include "libfolia/folia.h"
#include "libfolia/folia_properties.h"

namespace folia {

  using namespace std;

  const set<ElementType> default_ignore =
    { Original_t,
      Suggestion_t,
      Alternative_t };

  const set<ElementType> default_ignore_annotations =
    { Original_t,
      Suggestion_t,
      Alternative_t,
      MorphologyLayer_t };

  const set<ElementType> default_ignore_structure =
    { Original_t,
      Suggestion_t,
      Alternative_t,
      Chunk_t,
      SyntacticUnit_t,
      CoreferenceLayer_t,
      SemanticRolesLayer_t,
      Entity_t,
      Headwords_t,
      TimingLayer_t,
      DependencyDependent_t,
      TimeSegment_t };

  const set<ElementType> AnnoExcludeSet = { Original_t, Suggestion_t };

  const set<ElementType> SpanSet = { SyntacticUnit_t,
				     Chunk_t,
				     Entity_t,
				     Headwords_t,
				     DependencyDependent_t,
				     CoreferenceLink_t,
				     CoreferenceChain_t,
				     SemanticRole_t,
				     SemanticRolesLayer_t,
				     TimeSegment_t };

  properties DEFAULT_PROPERTIES =
    { BASE, "ThIsIsSoWrOnG",
      { Description_t },
      NO_ATT, NO_ATT,
      AnnotationType::NO_ANN,
      0, 1,
      "NONE", false, false, false
    };

  properties AbstractStructureElement::PROPS = DEFAULT_PROPERTIES;
  properties AbstractAnnotation::PROPS = DEFAULT_PROPERTIES;
  properties AbstractTokenAnnotation::PROPS = DEFAULT_PROPERTIES;
  properties AbstractCorrectionChild::PROPS = DEFAULT_PROPERTIES;
  properties AbstractAnnotationLayer::PROPS = DEFAULT_PROPERTIES;
  properties AbstractSpanAnnotation::PROPS = DEFAULT_PROPERTIES;
  properties AbstractTextMarkup::PROPS = DEFAULT_PROPERTIES;

  properties FoLiA::PROPS = DEFAULT_PROPERTIES;
  properties DCOI::PROPS = DEFAULT_PROPERTIES;
  properties XmlText::PROPS = DEFAULT_PROPERTIES;
  properties Description::PROPS = DEFAULT_PROPERTIES;
  properties Alignment::PROPS = DEFAULT_PROPERTIES;
  properties Content::PROPS = DEFAULT_PROPERTIES;
  properties Gap::PROPS = DEFAULT_PROPERTIES;
  properties AlternativeLayers::PROPS = DEFAULT_PROPERTIES;
  properties Metric::PROPS = DEFAULT_PROPERTIES;
  properties XmlComment::PROPS = DEFAULT_PROPERTIES;
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
  properties Headwords::PROPS = DEFAULT_PROPERTIES;
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
    FoLiA::PROPS._xmltag = "FoLiA";
    FoLiA::PROPS._accepted_data += { Text_t, Speech_t };

    DCOI::PROPS._xmltag = "DCOI";
    DCOI::PROPS._accepted_data += { Text_t, Speech_t };

    TextContent::PROPS._element_id = TextContent_t;
    TextContent::PROPS._xmltag="t";
    TextContent::PROPS._optional_attributes = CLASS|ANNOTATOR|CONFIDENCE|DATETIME;
    TextContent::PROPS._accepted_data += {
      AbstractTextMarkup_t,
	XmlText_t,
	LineBreak_t };
    TextContent::PROPS._annotation_type = AnnotationType::TEXT;
    TextContent::PROPS._occurrences = 0;
    TextContent::PROPS._occurrences_per_set=0;
    TextContent::PROPS.PRINTABLE = true;
    TextContent::PROPS.XLINK = true;

    PhonContent::PROPS._element_id = PhonContent_t;
    PhonContent::PROPS._xmltag="ph";
    PhonContent::PROPS._optional_attributes = CLASS|ANNOTATOR|CONFIDENCE|DATETIME;
    PhonContent::PROPS._accepted_data += { XmlText_t };
    PhonContent::PROPS._annotation_type = AnnotationType::PHON;
    PhonContent::PROPS._occurrences = 0;
    PhonContent::PROPS._occurrences_per_set=0;
    PhonContent::PROPS.PRINTABLE = false;
    PhonContent::PROPS.SPEAKABLE = true;

    AbstractStructureElement::PROPS._element_id = AbstractStructureElement_t;
    AbstractStructureElement::PROPS._xmltag = "structure";
    AbstractStructureElement::PROPS._required_attributes = ID;
    AbstractStructureElement::PROPS._optional_attributes = ALL;
    AbstractStructureElement::PROPS._occurrences_per_set = 0;
    AbstractStructureElement::PROPS.PRINTABLE = true;
    AbstractStructureElement::PROPS.SPEAKABLE = true;
    AbstractStructureElement::PROPS.TEXTDELIMITER = "\n\n";

    Head::PROPS = AbstractStructureElement::PROPS;
    Head::PROPS._element_id = Head_t;
    Head::PROPS._xmltag="head";
    Head::PROPS._accepted_data += {
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
    Head::PROPS._occurrences=1;
    Head::PROPS.TEXTDELIMITER = "\n\n";

    TableHead::PROPS = AbstractStructureElement::PROPS;
    TableHead::PROPS._element_id = TableHead_t;
    TableHead::PROPS._xmltag="tablehead";
    TableHead::PROPS._required_attributes = NO_ATT;
    TableHead::PROPS._accepted_data +=
      { Row_t,
	  AlternativeLayers_t,
	  AbstractAnnotationLayer_t,
	  AbstractTokenAnnotation_t,
	  Correction_t,
	  Part_t };
    TableHead::PROPS._annotation_type = AnnotationType::TABLE;

    Table::PROPS = AbstractStructureElement::PROPS;
    Table::PROPS._element_id = Table_t;
    Table::PROPS._xmltag="table";
    Table::PROPS._accepted_data +=
      { TableHead_t,
	  Row_t,
	  AlternativeLayers_t,
	  AbstractAnnotationLayer_t,
	  AbstractTokenAnnotation_t,
	  Correction_t,
	  Part_t };
    Table::PROPS._annotation_type = AnnotationType::TABLE;

    Cell::PROPS = AbstractStructureElement::PROPS;
    Cell::PROPS._element_id = Cell_t;
    Cell::PROPS._xmltag="cell";
    Cell::PROPS._required_attributes = NO_ATT;
    Cell::PROPS._accepted_data +=
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

    Cell::PROPS._annotation_type = AnnotationType::TABLE;
    Cell::PROPS.TEXTDELIMITER = " | ";

    Row::PROPS = AbstractStructureElement::PROPS;
    Row::PROPS._element_id = Row_t;
    Row::PROPS._xmltag="row";
    Row::PROPS._required_attributes = NO_ATT;
    Row::PROPS._accepted_data +=
      { Cell_t,
	  AbstractAnnotationLayer_t,
	  AlternativeLayers_t,
	  AbstractTokenAnnotation_t,
	  Correction_t,
	  Part_t };
    Row::PROPS._annotation_type = AnnotationType::TABLE;
    Row::PROPS.TEXTDELIMITER = "\n";

    LineBreak::PROPS = AbstractStructureElement::PROPS;
    LineBreak::PROPS._xmltag = "br";
    LineBreak::PROPS._element_id = LineBreak_t;
    LineBreak::PROPS._required_attributes = NO_ATT;
    LineBreak::PROPS._accepted_data +=
      { Feature_t,
	  Metric_t };
    LineBreak::PROPS._annotation_type = AnnotationType::LINEBREAK;

    WhiteSpace::PROPS = AbstractStructureElement::PROPS;
    WhiteSpace::PROPS._xmltag = "whitespace";
    WhiteSpace::PROPS._element_id = WhiteSpace_t;
    WhiteSpace::PROPS._required_attributes = NO_ATT;
    WhiteSpace::PROPS._accepted_data +=
      { Feature_t,
	  Metric_t };
    WhiteSpace::PROPS._annotation_type = AnnotationType::WHITESPACE;

    Word::PROPS = AbstractStructureElement::PROPS;
    Word::PROPS._xmltag="w";
    Word::PROPS._element_id = Word_t;
    Word::PROPS._accepted_data +=
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
    Word::PROPS._annotation_type = AnnotationType::TOKEN;
    Word::PROPS.TEXTDELIMITER = " ";

    AbstractTokenAnnotation::PROPS._element_id = AbstractTokenAnnotation_t;
    AbstractTokenAnnotation::PROPS._xmltag="tokenannotation";
    AbstractTokenAnnotation::PROPS._required_attributes = CLASS;
    AbstractTokenAnnotation::PROPS._optional_attributes = ALL;
    AbstractTokenAnnotation::PROPS._occurrences_per_set=1;

    String::PROPS = AbstractTokenAnnotation::PROPS;
    String::PROPS._xmltag="str";
    String::PROPS._element_id = String_t;
    String::PROPS._required_attributes = NO_ATT;
    String::PROPS._optional_attributes = ID|CLASS|ANNOTATOR|CONFIDENCE|DATETIME;
    String::PROPS._accepted_data +=
      { TextContent_t,
	  PhonContent_t,
	  Alignment_t,
	  Metric_t,
	  Correction_t,
	  AbstractTokenAnnotation_t,
	  LangAnnotation_t,
	  Feature_t };
    String::PROPS._annotation_type = AnnotationType::STRING;
    String::PROPS._occurrences = 0;
    String::PROPS._occurrences_per_set=0;
    String::PROPS.PRINTABLE = true;
    String::PROPS.SPEAKABLE = true;

    Part::PROPS = AbstractStructureElement::PROPS;
    Part::PROPS._xmltag="part";
    Part::PROPS._element_id = Part_t;
    Part::PROPS._required_attributes = NO_ATT;
    Part::PROPS._accepted_data +=
      { AbstractStructureElement_t,
	  AbstractTokenAnnotation_t,
	  AlternativeLayers_t,
	  AbstractAnnotationLayer_t,
	  Correction_t };
    Part::PROPS._annotation_type = AnnotationType::PART;
    Part::PROPS.TEXTDELIMITER = " ";

    PlaceHolder::PROPS = Word::PROPS;
    PlaceHolder::PROPS._xmltag="placeholder";
    PlaceHolder::PROPS._element_id = PlaceHolder_t;
    PlaceHolder::PROPS._required_attributes = NO_ATT;

    WordReference::PROPS._required_attributes = ID;
    WordReference::PROPS._xmltag = "wref";
    WordReference::PROPS._element_id = WordReference_t;

    Alignment::PROPS._optional_attributes = ALL;
    Alignment::PROPS._xmltag = "alignment";
    Alignment::PROPS._element_id = Alignment_t;
    Alignment::PROPS._accepted_data +=
      { AlignReference_t,
	  Metric_t };
    Alignment::PROPS._occurrences_per_set=0;
    Alignment::PROPS._annotation_type = AnnotationType::ALIGNMENT;
    Alignment::PROPS.XLINK = true;

    AlignReference::PROPS._xmltag = "aref";
    AlignReference::PROPS._element_id = AlignReference_t;

    Gap::PROPS._xmltag = "gap";
    Gap::PROPS._element_id = Gap_t;
    Gap::PROPS._annotation_type = AnnotationType::GAP;
    Gap::PROPS._accepted_data +=
      { Content_t,
	  Part_t };
    Gap::PROPS._optional_attributes = CLASS|ID|ANNOTATOR|CONFIDENCE|N|DATETIME;

    Metric::PROPS._element_id = Metric_t;
    Metric::PROPS._xmltag = "metric";
    Metric::PROPS._accepted_data +=
      { ValueFeature_t };
    Metric::PROPS._optional_attributes = CLASS|ANNOTATOR|CONFIDENCE;
    Metric::PROPS._annotation_type = AnnotationType::METRIC;

    Content::PROPS._xmltag = "content";
    Content::PROPS._element_id = Content_t;
    Content::PROPS._optional_attributes = ALL;
    Content::PROPS._occurrences_per_set=0;
    Content::PROPS.PRINTABLE = true;
    Content::PROPS.SPEAKABLE = true;

    Sentence::PROPS = AbstractStructureElement::PROPS;
    Sentence::PROPS._xmltag="s";
    Sentence::PROPS._element_id = Sentence_t;
    Sentence::PROPS._accepted_data +=
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
    Sentence::PROPS._annotation_type = AnnotationType::SENTENCE;
    Sentence::PROPS.TEXTDELIMITER = " ";

    Division::PROPS = AbstractStructureElement::PROPS;
    Division::PROPS._xmltag="div";
    Division::PROPS._element_id = Division_t;
    Division::PROPS._required_attributes = ID;
    Division::PROPS._optional_attributes = CLASS|N|SRC|BEGINTIME|ENDTIME|SPEAKER;
    Division::PROPS._accepted_data +=
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
    Division::PROPS._annotation_type = AnnotationType::DIVISION;
    Division::PROPS.TEXTDELIMITER = "\n\n\n";

    Text::PROPS = AbstractStructureElement::PROPS;
    Text::PROPS._xmltag="text";
    Text::PROPS._element_id = Text_t;
    Text::PROPS._accepted_data +=
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
    Text::PROPS._required_attributes = ID;
    Text::PROPS.TEXTDELIMITER = "\n\n";

    Speech::PROPS = AbstractStructureElement::PROPS;
    Speech::PROPS._xmltag="speech";
    Speech::PROPS.
      _element_id = Speech_t;
    Speech::PROPS._accepted_data +=
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
    Speech::PROPS._required_attributes = ID;
    Speech::PROPS._optional_attributes = N|SRC|BEGINTIME|ENDTIME|SPEAKER;
    Speech::PROPS.TEXTDELIMITER = "\n\n";

    Utterance::PROPS = AbstractStructureElement::PROPS;
    Utterance::PROPS._xmltag="utt";
    Utterance::PROPS._element_id = Utterance_t;
    Utterance::PROPS._accepted_data +=
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
    Utterance::PROPS._annotation_type = AnnotationType::UTTERANCE;
    Utterance::PROPS.TEXTDELIMITER = " ";
    Utterance::PROPS.PRINTABLE = true;
    Utterance::PROPS.SPEAKABLE = true;

    Event::PROPS = AbstractStructureElement::PROPS;
    Event::PROPS._xmltag="event";
    Event::PROPS._element_id = Event_t;
    Event::PROPS._accepted_data +=
      { Gap_t,
	  Division_t,
	  AbstractStructureElement_t,
	  Feature_t,
	  TextContent_t,
	  Part_t };
    Event::PROPS._annotation_type = AnnotationType::EVENT;
    Event::PROPS._occurrences_per_set=0;

    AbstractSpanAnnotation::PROPS._xmltag = "spanannotation";
    AbstractSpanAnnotation::PROPS._element_id = AbstractSpanAnnotation_t;
    AbstractSpanAnnotation::PROPS._required_attributes = NO_ATT;
    AbstractSpanAnnotation::PROPS._optional_attributes = ALL;
    AbstractSpanAnnotation::PROPS._occurrences_per_set = 0;
    AbstractSpanAnnotation::PROPS. PRINTABLE = true;
    AbstractSpanAnnotation::PROPS.SPEAKABLE = true;

    TimeSegment::PROPS = AbstractSpanAnnotation::PROPS;
    TimeSegment::PROPS._xmltag="timesegment";
    TimeSegment::PROPS._element_id = TimeSegment_t;
    TimeSegment::PROPS._accepted_data +=
      { Feature_t,
	  Word_t };
    TimeSegment::PROPS._annotation_type = AnnotationType::TIMESEGMENT;
    TimeSegment::PROPS._occurrences_per_set=0;

    Caption::PROPS = AbstractStructureElement::PROPS;
    Caption::PROPS._xmltag="caption";
    Caption::PROPS._element_id = Caption_t;
    Caption::PROPS._accepted_data +=
      { Sentence_t,
	  Reference_t,
	  AbstractTokenAnnotation_t,
	  TextContent_t,
	  Correction_t,
	  Part_t };
    Caption::PROPS._occurrences = 1;

    Label::PROPS = AbstractStructureElement::PROPS;
    Label::PROPS._xmltag="label";
    Label::PROPS._element_id = Label_t;
    Label::PROPS._accepted_data +=
      { Word_t,
	  TextContent_t,
	  AbstractTokenAnnotation_t,
	  Alignment_t,
	  Correction_t,
	  Part_t };

    Item::PROPS = AbstractStructureElement::PROPS;
    Item::PROPS._xmltag="item";
    Item::PROPS._element_id = Item_t;
    Item::PROPS._accepted_data +=
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
    Item::PROPS._annotation_type = AnnotationType::LIST;

    List::PROPS = AbstractStructureElement::PROPS;
    List::PROPS._xmltag="list";
    List::PROPS._element_id = List_t;
    List::PROPS._accepted_data +=
      { AbstractStructureElement_t,
	  Caption_t,
	  Event_t,
	  LangAnnotation_t,
	  TextContent_t,
	  PhonContent_t,
	  Alignment_t,
	  Correction_t,
	  Feature_t };
    List::PROPS._annotation_type = AnnotationType::LIST;
    List::PROPS.TEXTDELIMITER="\n";

    Figure::PROPS = AbstractStructureElement::PROPS;
    Figure::PROPS._xmltag="figure";
    Figure::PROPS._element_id = Figure_t;
    Figure::PROPS._accepted_data +=
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
    Figure::PROPS._annotation_type = AnnotationType::FIGURE;

    Paragraph::PROPS = AbstractStructureElement::PROPS;
    Paragraph::PROPS._xmltag="p";
    Paragraph::PROPS._element_id = Paragraph_t;
    Paragraph::PROPS._accepted_data +=
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
    Paragraph::PROPS._annotation_type = AnnotationType::PARAGRAPH;


    SyntacticUnit::PROPS = AbstractSpanAnnotation::PROPS;
    SyntacticUnit::PROPS._xmltag = "su";
    SyntacticUnit::PROPS._element_id = SyntacticUnit_t;
    SyntacticUnit::PROPS._required_attributes = NO_ATT;
    SyntacticUnit::PROPS._annotation_type = AnnotationType::SYNTAX;
    SyntacticUnit::PROPS._accepted_data +=
      { SyntacticUnit_t,
	  Word_t,
	  WordReference_t,
	  Feature_t };

    SemanticRole::PROPS = AbstractSpanAnnotation::PROPS;
    SemanticRole::PROPS._xmltag = "semrole";
    SemanticRole::PROPS._element_id = SemanticRole_t;
    SemanticRole::PROPS._required_attributes = CLASS;
    SemanticRole::PROPS._annotation_type = AnnotationType::SEMROLE;
    SemanticRole::PROPS._accepted_data +=
      { Word_t,
	  WordReference_t,
	  LangAnnotation_t,
	  Headwords_t,
	  Alignment_t,
	  Metric_t };

    Chunk::PROPS = AbstractSpanAnnotation::PROPS;
    Chunk::PROPS._required_attributes = NO_ATT;
    Chunk::PROPS._xmltag = "chunk";
    Chunk::PROPS._element_id = Chunk_t;
    Chunk::PROPS._annotation_type = AnnotationType::CHUNKING;
    Chunk::PROPS._accepted_data +=
      { Word_t,
	  WordReference_t,
	  LangAnnotation_t,
	  Feature_t };

    Entity::PROPS = AbstractSpanAnnotation::PROPS;
    Entity::PROPS._required_attributes = NO_ATT;
    Entity::PROPS._optional_attributes = ID|CLASS|ANNOTATOR|CONFIDENCE|DATETIME;
    Entity::PROPS._xmltag = "entity";
    Entity::PROPS._element_id = Entity_t;
    Entity::PROPS._annotation_type = AnnotationType::ENTITY;
    Entity::PROPS._accepted_data +=
      { Word_t,
	  LangAnnotation_t,
	  WordReference_t,
	  Morpheme_t,
	  Feature_t,
	  Metric_t };


    Alternative::PROPS = AbstractStructureElement::PROPS;
    Alternative::PROPS._xmltag = "alt";
    Alternative::PROPS._element_id = Alternative_t;
    Alternative::PROPS._required_attributes = NO_ATT;
    Alternative::PROPS._optional_attributes = ALL;
    Alternative::PROPS._accepted_data +=
      { AbstractTokenAnnotation_t,
	  Correction_t,
	  MorphologyLayer_t,
	  PhonologyLayer_t
	  };
    Alternative::PROPS._annotation_type = AnnotationType::ALTERNATIVE;

    AlternativeLayers::PROPS._xmltag = "altlayers";
    AlternativeLayers::PROPS._element_id = AlternativeLayers_t;
    AlternativeLayers::PROPS._optional_attributes = ALL;
    AlternativeLayers::PROPS._accepted_data += { AbstractAnnotationLayer_t };

    AbstractCorrectionChild::PROPS._xmltag = "correctionchild";
    AbstractCorrectionChild::PROPS._element_id = AbstractCorrectionChild_t;
    AbstractCorrectionChild::PROPS._optional_attributes = NO_ATT;
    AbstractCorrectionChild::PROPS._accepted_data +=
      { AbstractTokenAnnotation_t,
	  AbstractSpanAnnotation_t,
	  Word_t,
	  WordReference_t,
	  String_t,
	  TextContent_t };
    AbstractCorrectionChild::PROPS._occurrences = 1;
    AbstractCorrectionChild::PROPS.PRINTABLE=true;
    AbstractCorrectionChild::PROPS.SPEAKABLE=true;

    New::PROPS = AbstractCorrectionChild::PROPS;
    New::PROPS._xmltag = "new";
    New::PROPS._element_id = New_t;

    Current::PROPS = AbstractCorrectionChild::PROPS;
    Current::PROPS._xmltag = "current";
    Current::PROPS._element_id = Current_t;

    Original::PROPS = AbstractCorrectionChild::PROPS;
    Original::PROPS._xmltag = "original";
    Original::PROPS._element_id = Original_t;

    Suggestion::PROPS = AbstractCorrectionChild::PROPS;
    Suggestion::PROPS._xmltag = "suggestion";
    Suggestion::PROPS._element_id = Suggestion_t;
    Suggestion::PROPS._optional_attributes = ANNOTATOR|CONFIDENCE|DATETIME|N;
    Suggestion::PROPS._annotation_type = AnnotationType::SUGGESTION;
    Suggestion::PROPS._occurrences=0;
    Suggestion::PROPS._occurrences_per_set=0;

    Correction::PROPS = AbstractTokenAnnotation::PROPS;
    Correction::PROPS._xmltag = "correction";
    Correction::PROPS._element_id = Correction_t;
    Correction::PROPS._required_attributes = NO_ATT;
    Correction::PROPS._annotation_type = AnnotationType::CORRECTION;
    Correction::PROPS._accepted_data +=
      { New_t,
	  Original_t,
	  Suggestion_t,
	  Current_t,
	  ErrorDetection_t,
	  Feature_t };
    Correction::PROPS._occurrences_per_set=0;
    Correction::PROPS.PRINTABLE=true;
    Correction::PROPS.SPEAKABLE=true;

    Description::PROPS._xmltag = "desc";
    Description::PROPS._element_id = Description_t;
    Description::PROPS._occurrences = 1;

    Entry::PROPS = AbstractStructureElement::PROPS;
    Entry::PROPS._xmltag = "entry";
    Entry::PROPS._element_id = Entry_t;
    Entry::PROPS._accepted_data +=
      { Term_t,
	  Definition_t,
	  Example_t,
	  Correction_t,
	  Metric_t,
	  Alignment_t,
	  AlternativeLayers_t,
	  AbstractAnnotationLayer_t };

    Definition::PROPS = AbstractStructureElement::PROPS;
    Definition::PROPS._xmltag = "def";
    Definition::PROPS._element_id = Definition_t;
    Definition::PROPS._accepted_data +=
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
    Definition::PROPS._annotation_type = AnnotationType::DEFINITION;

    Term::PROPS = AbstractStructureElement::PROPS;
    Term::PROPS._xmltag = "term";
    Term::PROPS._element_id = Term_t;
    Term::PROPS._accepted_data +=
      { Paragraph_t, Event_t, Sentence_t, Word_t, Utterance_t,
	List_t, Figure_t, Table_t, Reference_t, Feature_t,
	TextContent_t, PhonContent_t, String_t, Metric_t,
	AbstractTokenAnnotation_t, Correction_t, Part_t };
    Term::PROPS._annotation_type = AnnotationType::TERM;

    Example::PROPS = AbstractStructureElement::PROPS;
    Example::PROPS._xmltag = "ex";
    Example::PROPS._element_id = Example_t;
    Example::PROPS._accepted_data +=
      { Paragraph_t, Sentence_t, Word_t, Utterance_t,
	  List_t, Figure_t, Table_t, Reference_t, Feature_t,
	  TextContent_t, PhonContent_t, String_t, Metric_t,
	  AbstractTokenAnnotation_t, Correction_t, Part_t };
    Example::PROPS._annotation_type = AnnotationType::EXAMPLE;

    XmlText::PROPS._xmltag = "xml-text";
    XmlText::PROPS._element_id = XmlText_t;
    XmlText::PROPS.TEXTDELIMITER = "*";
    XmlText::PROPS.PRINTABLE = true;
    XmlText::PROPS.SPEAKABLE = true;

    External::PROPS._xmltag = "external";
    External::PROPS._element_id = External_t;
    External::PROPS._required_attributes = SRC;
    External::PROPS.PRINTABLE = true;
    External::PROPS.SPEAKABLE = false;

    Note::PROPS = AbstractStructureElement::PROPS;
    Note::PROPS._required_attributes = ID;
    Note::PROPS._xmltag = "note";
    Note::PROPS._element_id = Note_t;
    Note::PROPS._accepted_data +=
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
    Note::PROPS._annotation_type = AnnotationType::NOTE;

    Reference::PROPS = AbstractStructureElement::PROPS;
    Reference::PROPS._xmltag = "ref";
    Reference::PROPS._element_id = Reference_t;
    Reference::PROPS._required_attributes = NO_ATT;
    Reference::PROPS._optional_attributes = ID|ANNOTATOR|CONFIDENCE|DATETIME;
    Reference::PROPS._accepted_data +=
      { TextContent_t, PhonContent_t, String_t,
	Metric_t };

    XmlComment::PROPS._xmltag = "xml-comment";
    XmlComment::PROPS._element_id = XmlComment_t;

    Morpheme::PROPS = AbstractStructureElement::PROPS;
    Morpheme::PROPS._element_id = Morpheme_t;
    Morpheme::PROPS._xmltag = "morpheme";
    Morpheme::PROPS._required_attributes = NO_ATT;
    Morpheme::PROPS._optional_attributes = ALL;
    Morpheme::PROPS._accepted_data +=
      { Feature_t, FunctionFeature_t, TextContent_t,
	Metric_t, Alignment_t, AbstractTokenAnnotation_t,
	Morpheme_t };
    Morpheme::PROPS._annotation_type = AnnotationType::MORPHOLOGICAL;

    AbstractAnnotationLayer::PROPS._xmltag = "annotationlayer";
    AbstractAnnotationLayer::PROPS._element_id = AbstractAnnotationLayer_t;
    AbstractAnnotationLayer::PROPS._optional_attributes = SETONLY;

    SyntaxLayer::PROPS = AbstractAnnotationLayer::PROPS;
    SyntaxLayer::PROPS._element_id = SyntaxLayer_t;
    SyntaxLayer::PROPS._xmltag = "syntax";
    SyntaxLayer::PROPS._accepted_data += { SyntacticUnit_t,
					  Correction_t };
    SyntaxLayer::PROPS._annotation_type = AnnotationType::SYNTAX;

    ChunkingLayer::PROPS = AbstractAnnotationLayer::PROPS;
    ChunkingLayer::PROPS._element_id = ChunkingLayer_t;
    ChunkingLayer::PROPS._xmltag = "chunking";
    ChunkingLayer::PROPS._accepted_data += { Chunk_t,
					    Correction_t };
    ChunkingLayer::PROPS._annotation_type = AnnotationType::CHUNKING;

    EntitiesLayer::PROPS = AbstractAnnotationLayer::PROPS;
    EntitiesLayer::PROPS._element_id = EntitiesLayer_t;
    EntitiesLayer::PROPS._xmltag = "entities";
    EntitiesLayer::PROPS._accepted_data += { Entity_t,
					    Correction_t };
    EntitiesLayer::PROPS._annotation_type = AnnotationType::ENTITY;

    TimingLayer::PROPS = AbstractAnnotationLayer::PROPS;
    TimingLayer::PROPS._element_id = TimingLayer_t;
    TimingLayer::PROPS._xmltag = "timing";
    TimingLayer::PROPS._accepted_data += { TimeSegment_t,
					  Correction_t };

    MorphologyLayer::PROPS = AbstractAnnotationLayer::PROPS;
    MorphologyLayer::PROPS._element_id = MorphologyLayer_t;
    MorphologyLayer::PROPS._xmltag = "morphology";
    MorphologyLayer::PROPS._accepted_data += { Morpheme_t, Correction_t };
    MorphologyLayer::PROPS._occurrences_per_set = 1;
    MorphologyLayer::PROPS._annotation_type = AnnotationType::MORPHOLOGICAL;

    PhonologyLayer::PROPS = AbstractAnnotationLayer::PROPS;
    PhonologyLayer::PROPS._element_id = PhonologyLayer_t;
    PhonologyLayer::PROPS._xmltag = "phonology";
    PhonologyLayer::PROPS._accepted_data += { Phoneme_t, Correction_t };
    PhonologyLayer::PROPS._occurrences_per_set = 1;
    PhonologyLayer::PROPS._annotation_type = AnnotationType::PHONOLOGICAL;

    CoreferenceLayer::PROPS = AbstractAnnotationLayer::PROPS;
    CoreferenceLayer::PROPS._element_id = CoreferenceLayer_t;
    CoreferenceLayer::PROPS._xmltag = "coreferences";
    CoreferenceLayer::PROPS._accepted_data += { CoreferenceChain_t,
					       Correction_t };
    CoreferenceLayer::PROPS._annotation_type = AnnotationType::COREFERENCE;

    CoreferenceLink::PROPS = AbstractSpanAnnotation::PROPS;
    CoreferenceLink::PROPS._element_id = CoreferenceLink_t;
    CoreferenceLink::PROPS._xmltag = "coreferencelink";
    CoreferenceLink::PROPS._required_attributes = NO_ATT;
    CoreferenceLink::PROPS._optional_attributes = ANNOTATOR|N|DATETIME;
    CoreferenceLink::PROPS._accepted_data +=
      { Word_t,
	  WordReference_t,
	  Headwords_t,
	  LangAnnotation_t,
	  Alignment_t,
	  TimeFeature_t,
	  LevelFeature_t,
	  ModalityFeature_t,
	  Metric_t };
    CoreferenceLink::PROPS._annotation_type = AnnotationType::COREFERENCE;

    CoreferenceChain::PROPS = AbstractSpanAnnotation::PROPS;
    CoreferenceChain::PROPS._element_id = CoreferenceChain_t;
    CoreferenceChain::PROPS._xmltag = "coreferencechain";
    CoreferenceChain::PROPS._required_attributes = NO_ATT;
    CoreferenceChain::PROPS._accepted_data += { CoreferenceLink_t,
					       Metric_t };
    CoreferenceChain::PROPS._annotation_type = AnnotationType::COREFERENCE;

    SemanticRolesLayer::PROPS = AbstractAnnotationLayer::PROPS;
    SemanticRolesLayer::PROPS._element_id = SemanticRolesLayer_t;
    SemanticRolesLayer::PROPS._xmltag = "semroles";
    SemanticRolesLayer::PROPS._accepted_data +=
      { SemanticRole_t,
	  Correction_t };
    SemanticRolesLayer::PROPS._annotation_type = AnnotationType::SEMROLE;

    DependenciesLayer::PROPS = AbstractAnnotationLayer::PROPS;
    DependenciesLayer::PROPS._element_id = DependenciesLayer_t;
    DependenciesLayer::PROPS._xmltag = "dependencies";
    DependenciesLayer::PROPS._accepted_data += { Dependency_t,
						Correction_t };
    DependenciesLayer::PROPS._annotation_type = AnnotationType::DEPENDENCY;

    Dependency::PROPS = AbstractSpanAnnotation::PROPS;
    Dependency::PROPS._element_id = Dependency_t;
    Dependency::PROPS._xmltag = "dependency";
    Dependency::PROPS._required_attributes = NO_ATT;
    Dependency::PROPS._annotation_type = AnnotationType::DEPENDENCY;
    Dependency::PROPS._accepted_data +=
      { DependencyDependent_t,
	  Headwords_t,
	  Feature_t,
	  Alignment_t };

    DependencyDependent::PROPS = AbstractSpanAnnotation::PROPS;
    DependencyDependent::PROPS._element_id = DependencyDependent_t;
    DependencyDependent::PROPS._xmltag = "dep";
    DependencyDependent::PROPS._required_attributes = NO_ATT;
    DependencyDependent::PROPS._optional_attributes = NO_ATT;
    DependencyDependent::PROPS._annotation_type = AnnotationType::DEPENDENCY;
    DependencyDependent::PROPS._accepted_data +=
      { Word_t,
	  WordReference_t,
	  PlaceHolder_t,
	  Feature_t,
	  Alignment_t };

    Headwords::PROPS = AbstractSpanAnnotation::PROPS;
    Headwords::PROPS._element_id = Headwords_t;
    Headwords::PROPS._xmltag = "hd";
    Headwords::PROPS._required_attributes = NO_ATT;
    Headwords::PROPS._optional_attributes = NO_ATT;
    Headwords::PROPS._accepted_data +=
      { Word_t,
	  WordReference_t,
	  PlaceHolder_t,
	  Feature_t,
	  Metric_t,
	  Alignment_t,
	  LangAnnotation_t };

    PosAnnotation::PROPS = AbstractTokenAnnotation::PROPS;
    PosAnnotation::PROPS._xmltag="pos";
    PosAnnotation::PROPS._element_id = PosAnnotation_t;
    PosAnnotation::PROPS._annotation_type = AnnotationType::POS;
    PosAnnotation::PROPS._accepted_data +=
      { Feature_t,
	  Metric_t };

    LemmaAnnotation::PROPS = AbstractTokenAnnotation::PROPS;
    LemmaAnnotation::PROPS._xmltag="lemma";
    LemmaAnnotation::PROPS._element_id = LemmaAnnotation_t;
    LemmaAnnotation::PROPS._annotation_type = AnnotationType::LEMMA;
    LemmaAnnotation::PROPS._accepted_data +=
      { Feature_t,
	  Metric_t };

    LangAnnotation::PROPS = AbstractTokenAnnotation::PROPS;
    LangAnnotation::PROPS._xmltag="lang";
    LangAnnotation::PROPS._element_id = LangAnnotation_t;
    LangAnnotation::PROPS._annotation_type = AnnotationType::LANG;
    LangAnnotation::PROPS._accepted_data +=
      { Feature_t,
	  Metric_t };

    Phoneme::PROPS = AbstractTokenAnnotation::PROPS;
    Phoneme::PROPS._xmltag="phoneme";
    Phoneme::PROPS._element_id = Phoneme_t;
    Phoneme::PROPS._required_attributes = NO_ATT;
    Phoneme::PROPS._optional_attributes = ALL;
    Phoneme::PROPS._annotation_type = AnnotationType::PHON;
    Phoneme::PROPS._accepted_data +=
      { PhonContent_t,
	  Feature_t  };

    DomainAnnotation::PROPS = AbstractTokenAnnotation::PROPS;
    DomainAnnotation::PROPS._xmltag="domain";
    DomainAnnotation::PROPS._element_id = DomainAnnotation_t;
    DomainAnnotation::PROPS._annotation_type = AnnotationType::DOMEIN;
    DomainAnnotation::PROPS._accepted_data += { Feature_t };

    SenseAnnotation::PROPS = AbstractTokenAnnotation::PROPS;
    SenseAnnotation::PROPS._xmltag="sense";
    SenseAnnotation::PROPS._element_id = SenseAnnotation_t;
    SenseAnnotation::PROPS._annotation_type = AnnotationType::SENSE;
    SenseAnnotation::PROPS._accepted_data +=
      { Feature_t, SynsetFeature_t, Metric_t };

    SubjectivityAnnotation::PROPS = AbstractTokenAnnotation::PROPS;
    SubjectivityAnnotation::PROPS._xmltag="subjectivity";
    SubjectivityAnnotation::PROPS._element_id = SubjectivityAnnotation_t;
    SubjectivityAnnotation::PROPS._annotation_type = AnnotationType::SUBJECTIVITY;
    SubjectivityAnnotation::PROPS._accepted_data += { Feature_t };

    Quote::PROPS = AbstractStructureElement::PROPS;
    Quote::PROPS._xmltag="quote";
    Quote::PROPS._element_id = Quote_t;
    Quote::PROPS._required_attributes = NO_ATT;
    Quote::PROPS._accepted_data +=
      { AbstractStructureElement_t,
	  String_t,
	  LangAnnotation_t,
	  TextContent_t,
	  Alignment_t,
	  Gap_t };
    Quote::PROPS.TEXTDELIMITER = " ";

    Feature::PROPS._xmltag = "feat";
    Feature::PROPS._element_id = Feature_t;
    Feature::PROPS._occurrences_per_set = 0;

    BeginDateTimeFeature::PROPS._xmltag="begindatetime";
    BeginDateTimeFeature::PROPS._element_id = BeginDateTimeFeature_t;

    EndDateTimeFeature::PROPS._xmltag="enddatetime";
    EndDateTimeFeature::PROPS._element_id = EndDateTimeFeature_t;

    SynsetFeature::PROPS._xmltag="synset";
    SynsetFeature::PROPS._element_id = SynsetFeature_t;
    SynsetFeature::PROPS._annotation_type = AnnotationType::SENSE;

    ActorFeature::PROPS._xmltag = "actor";
    ActorFeature::PROPS._element_id = ActorFeature_t;

    HeadFeature::PROPS._xmltag = "headfeature";
    HeadFeature::PROPS._element_id = HeadFeature_t;

    ValueFeature::PROPS._xmltag = "value";
    ValueFeature::PROPS._element_id = ValueFeature_t;

    FunctionFeature::PROPS._xmltag = "function";
    FunctionFeature::PROPS._element_id = FunctionFeature_t;

    LevelFeature::PROPS._xmltag = "level";
    LevelFeature::PROPS._element_id = LevelFeature_t;

    ModalityFeature::PROPS._xmltag = "modality";
    ModalityFeature::PROPS._element_id = ModalityFeature_t;

    TimeFeature::PROPS._xmltag = "time";
    TimeFeature::PROPS._element_id = TimeFeature_t;

    ErrorDetection::PROPS = AbstractTokenAnnotation::PROPS;
    ErrorDetection::PROPS.
      _xmltag = "errordetection";
    ErrorDetection::PROPS.
      _element_id = ErrorDetection_t;
    ErrorDetection::PROPS.
      _annotation_type = AnnotationType::ERRORDETECTION;
    ErrorDetection::PROPS.
      _occurrences_per_set = 0; // Allow duplicates within the same set

    AbstractTextMarkup::PROPS._xmltag = "textmarkup";
    AbstractTextMarkup::PROPS._element_id = AbstractTextMarkup_t;
    AbstractTextMarkup::PROPS._required_attributes = NO_ATT;
    AbstractTextMarkup::PROPS._optional_attributes = ALL;
    AbstractTextMarkup::PROPS._annotation_type = AnnotationType::NO_ANN;
    AbstractTextMarkup::PROPS._accepted_data +=
      { AbstractTextMarkup_t, XmlText_t };
    AbstractTextMarkup::PROPS.PRINTABLE = true;
    AbstractTextMarkup::PROPS.XLINK = true;
    AbstractTextMarkup::PROPS.TEXTDELIMITER = "";
    AbstractTextMarkup::PROPS._occurrences_per_set = 0;

    TextMarkupString::PROPS = AbstractTextMarkup::PROPS;
    TextMarkupString::PROPS._xmltag = "t-str";
    TextMarkupString::PROPS._element_id = TextMarkupString_t;
    TextMarkupString::PROPS._annotation_type = AnnotationType::STRING;

    TextMarkupGap::PROPS = AbstractTextMarkup::PROPS;
    TextMarkupGap::PROPS._xmltag = "t-gap";
    TextMarkupGap::PROPS._element_id = TextMarkupGap_t;
    TextMarkupGap::PROPS._annotation_type = AnnotationType::GAP;

    TextMarkupCorrection::PROPS = AbstractTextMarkup::PROPS;
    TextMarkupCorrection::PROPS._xmltag = "t-correction";
    TextMarkupCorrection::PROPS._element_id = TextMarkupCorrection_t;
    TextMarkupCorrection::PROPS._annotation_type = AnnotationType::CORRECTION;

    TextMarkupError::PROPS = AbstractTextMarkup::PROPS;
    TextMarkupError::PROPS._xmltag = "t-error";
    TextMarkupError::PROPS._element_id = TextMarkupError_t;
    TextMarkupError::PROPS._annotation_type = AnnotationType::ERRORDETECTION;

    TextMarkupStyle::PROPS = AbstractTextMarkup::PROPS;
    TextMarkupStyle::PROPS._xmltag = "t-style";
    TextMarkupStyle::PROPS._element_id = TextMarkupStyle_t;
    TextMarkupStyle::PROPS._annotation_type = AnnotationType::STYLE;

  }

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
      catch (...){
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

  void show( const map<ElementType,set<ElementType> >& hier ){
    for ( auto const top : hier ){
      cerr << toString(top.first) << endl;
      for ( auto const el : top.second ){
	cerr << "     -- " << toString(el) << endl;
      }
    }
  }

  namespace {
    struct initializer {
      initializer() {
	//	std::cout << "Loading the static properties" << std::endl;
	static_init();
	fill_hierarchy();
	// cerr << "NEW:" << endl;
	// show( typeHierarchy  );
      }
      ~initializer() {
	// std::cout << "Unloading the properties" << std::endl;
	// std::cout << "but don't care.." << std::endl;
      }
    };
    static initializer i;
  }

}
