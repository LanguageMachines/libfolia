#include <set>
#include <string>

#include "libfolia/foliautils.h"
#include "libfolia/folia.h"
#include "libfolia/properties.h"

using namespace folia;

properties DEFAULT_PROPERTIES =
  { BASE, "ThIsIsSoWrOnG",
    {},
    NO_ATT, NO_ATT,
    AnnotationType::NO_ANN, UNDEFINED, true,
    -1, 0, 1,
    "NONE", false, false, false
  };

properties AbstractStructureElement_PROPS = DEFAULT_PROPERTIES;

properties AbstractTokenAnnotation_PROPS = DEFAULT_PROPERTIES;

properties FoLiA::PROPS = DEFAULT_PROPERTIES;
properties DCOI::PROPS = DEFAULT_PROPERTIES;
properties XmlText::PROPS = DEFAULT_PROPERTIES;
properties Description::PROPS = DEFAULT_PROPERTIES;
properties Alignment::PROPS = DEFAULT_PROPERTIES;
properties Content::PROPS = DEFAULT_PROPERTIES;
properties Gap::PROPS = DEFAULT_PROPERTIES;
properties AlternativeLayers::PROPS = DEFAULT_PROPERTIES;
properties MetricAnnotation::PROPS = DEFAULT_PROPERTIES;
properties XmlComment::PROPS = DEFAULT_PROPERTIES;
properties External::PROPS = DEFAULT_PROPERTIES;
properties AlignReference::PROPS = DEFAULT_PROPERTIES;
properties WordReference::PROPS = DEFAULT_PROPERTIES;
properties PhonContent::PROPS = DEFAULT_PROPERTIES;
properties Feature::PROPS = DEFAULT_PROPERTIES;
properties TextContent::PROPS = DEFAULT_PROPERTIES;
properties LemmaAnnotation::PROPS = DEFAULT_PROPERTIES;
properties SemanticRole::PROPS = DEFAULT_PROPERTIES;
properties Current::PROPS = DEFAULT_PROPERTIES;
properties Phoneme::PROPS = DEFAULT_PROPERTIES;
properties SyntacticUnit::PROPS = DEFAULT_PROPERTIES;
properties TextMarkupError::PROPS = DEFAULT_PROPERTIES;
properties Dependency::PROPS = DEFAULT_PROPERTIES;
properties Word::PROPS = DEFAULT_PROPERTIES;
properties Row::PROPS = DEFAULT_PROPERTIES;
properties Part::PROPS = DEFAULT_PROPERTIES;
properties SubjectivityAnnotation::PROPS = DEFAULT_PROPERTIES;
properties CoreferenceLink::PROPS = DEFAULT_PROPERTIES;
properties LineBreak::PROPS = DEFAULT_PROPERTIES;
properties ChunkingLayer::PROPS = DEFAULT_PROPERTIES;
properties MorphologyLayer::PROPS = DEFAULT_PROPERTIES;
properties NewElement::PROPS = DEFAULT_PROPERTIES;
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

void static_init(){
  FoLiA::PROPS._xmltag = "FoLiA";
  FoLiA::PROPS._accepted_data = { Text_t, Speech_t };

  DCOI::PROPS._xmltag = "DCOI";
  DCOI::PROPS._accepted_data = { Text_t, Speech_t };

  AbstractStructureElement_PROPS._element_id = Structure_t;
  AbstractStructureElement_PROPS._xmltag = "structure";
  AbstractStructureElement_PROPS._required_attributes = ID;
  AbstractStructureElement_PROPS._optional_attributes = ALL;
  AbstractStructureElement_PROPS._occurrences_per_set=0;
  AbstractStructureElement_PROPS.PRINTABLE = true;
  AbstractStructureElement_PROPS.SPEAKABLE = true;
  AbstractStructureElement_PROPS.TEXTDELIMITER = "\n\n";

  AbstractTokenAnnotation_PROPS._element_id = TokenAnnotation_t;
  AbstractTokenAnnotation_PROPS._xmltag="tokenannotation";
  AbstractTokenAnnotation_PROPS._required_attributes = CLASS;
  AbstractTokenAnnotation_PROPS._optional_attributes = ALL;
  AbstractTokenAnnotation_PROPS._occurrences_per_set=1;

  TextContent::PROPS._element_id = TextContent_t;
  TextContent::PROPS._xmltag="t";
  TextContent::PROPS._optional_attributes = CLASS|ANNOTATOR|CONFIDENCE|DATETIME;
  TextContent::PROPS._accepted_data = { AbstractTextMarkup_t,
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
  PhonContent::PROPS._accepted_data = { XmlText_t };
  PhonContent::PROPS._annotation_type = AnnotationType::PHON;
  PhonContent::PROPS._occurrences = 0;
  PhonContent::PROPS._occurrences_per_set=0;
  PhonContent::PROPS.PRINTABLE = false;
  PhonContent::PROPS.SPEAKABLE = true;

  Head::PROPS._element_id = Head_t;
  Head::PROPS._xmltag="head";
  Head::PROPS._accepted_data = { Structure_t, Description_t, Correction_t,
				 String_t, TextContent_t, PhonContent_t,
				 Alignment_t, Metric_t, AlternativeLayers_t,
				 TokenAnnotation_t, Gap_t };
  Head::PROPS._occurrences=1;
  Head::PROPS.TEXTDELIMITER = "\n\n";

  TableHead::PROPS._element_id = TableHead_t;
  TableHead::PROPS._xmltag="tablehead";
  TableHead::PROPS._required_attributes = NO_ATT;
  TableHead::PROPS._accepted_data ={ Row_t,
				     AlternativeLayers_t,
				     AnnotationLayer_t,
				     TokenAnnotation_t,
				     Correction_t,
				     Part_t };
  TableHead::PROPS._annotation_type = AnnotationType::TABLE;

  Table::PROPS._element_id = Table_t;
  Table::PROPS._xmltag="table";
  Table::PROPS._accepted_data = { TableHead_t,
				  Row_t,
				  AlternativeLayers_t,
				  AnnotationLayer_t,
				  TokenAnnotation_t,
				  Correction_t,
				  Part_t };
  Table::PROPS._annotation_type = AnnotationType::TABLE;

  Cell::PROPS._element_id = Cell_t;
  Cell::PROPS._xmltag="cell";
  Cell::PROPS._required_attributes = NO_ATT;
  Cell::PROPS._accepted_data = { Structure_t,
				 Entities_t,
				 Alignment_t,
				 Metric_t,
				 Gap_t,
				 AlternativeLayers_t,
				 AnnotationLayer_t,
				 TokenAnnotation_t,
				 Correction_t,
				 Part_t,
				 Feature_t };

  Cell::PROPS._annotation_type = AnnotationType::TABLE;
  Cell::PROPS.TEXTDELIMITER = " | ";

  Row::PROPS._element_id = Row_t;
  Row::PROPS._xmltag="row";
  Row::PROPS._required_attributes = NO_ATT;
  Row::PROPS._accepted_data = { Cell_t,
				AnnotationLayer_t,
				AlternativeLayers_t,
				TokenAnnotation_t,
				Correction_t,
				Part_t };
  Row::PROPS._annotation_type = AnnotationType::TABLE;
  Row::PROPS.TEXTDELIMITER = "\n";

  LineBreak::PROPS._xmltag = "br";
  LineBreak::PROPS._element_id = LineBreak_t;
  LineBreak::PROPS._required_attributes = NO_ATT;
  LineBreak::PROPS._accepted_data = { Feature_t,
				      Metric_t,
				      Description_t };
  LineBreak::PROPS._annotation_type = AnnotationType::LINEBREAK;

  WhiteSpace::PROPS._xmltag = "whitespace";
  WhiteSpace::PROPS._element_id = WhiteSpace_t;
  WhiteSpace::PROPS._required_attributes = NO_ATT;
  WhiteSpace::PROPS._accepted_data = { Feature_t,
				       Metric_t,
				       Description_t };
  WhiteSpace::PROPS._annotation_type = AnnotationType::WHITESPACE;

  Word::PROPS._xmltag="w";
  Word::PROPS._element_id = Word_t;
  Word::PROPS._accepted_data = { TokenAnnotation_t,
				 Correction_t,
				 TextContent_t,
				 PhonContent_t,
				 PhonologyLayer_t,
				 String_t,
				 Alternative_t,
				 AlternativeLayers_t,
				 Description_t,
				 AnnotationLayer_t,
				 Alignment_t,
				 Metric_t,
				 Reference_t,
				 Feature_t };
  Word::PROPS._annotation_type = AnnotationType::TOKEN;
  Word::PROPS.TEXTDELIMITER = " ";


  String::PROPS._xmltag="str";
  String::PROPS._element_id = String_t;
  String::PROPS._required_attributes = NO_ATT;
  String::PROPS._optional_attributes = ID|CLASS|ANNOTATOR|CONFIDENCE|DATETIME;
  String::PROPS._accepted_data = { TextContent_t,
				   PhonContent_t,
				   Alignment_t,
				   Description_t,
				   Metric_t,
				   Correction_t,
				   TokenAnnotation_t,
				   Lang_t,
				   Feature_t };

  String::PROPS._annotation_type = AnnotationType::STRING;
  String::PROPS._occurrences = 0;
  String::PROPS._occurrences_per_set=0;
  String::PROPS.PRINTABLE = true;
  String::PROPS.SPEAKABLE = true;

}
