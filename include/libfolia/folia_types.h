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

#ifndef TYPES_H
#define TYPES_H
#include <string>
#include "ticcutils/StringOps.h"
#include "ticcutils/enum_flags.h"

namespace folia {

  //foliaspec:elementtype
  enum class ElementType : unsigned int { BASE=0, AbstractFeature_t, AbstractWord_t, AbstractAnnotationLayer_t, AbstractContentAnnotation_t, AbstractCorrectionChild_t, AbstractHigherOrderAnnotation_t, AbstractInlineAnnotation_t, AbstractSpanAnnotation_t, AbstractSpanRole_t, AbstractStructureElement_t, AbstractSubtokenAnnotation_t, AbstractTextMarkup_t, ActorFeature_t, Alternative_t, AlternativeLayers_t, BegindatetimeFeature_t, Caption_t, Cell_t, Chunk_t, ChunkingLayer_t, Comment_t, Content_t, CoreferenceChain_t, CoreferenceLayer_t, CoreferenceLink_t, Correction_t, Cue_t, Current_t, Definition_t, DependenciesLayer_t, Dependency_t, DependencyDependent_t, Description_t, Division_t, DomainAnnotation_t, EnddatetimeFeature_t, EntitiesLayer_t, Entity_t, Entry_t, ErrorDetection_t, EtymologyAnnotation_t, Event_t, Example_t, External_t, Feature_t, Figure_t, FontFeature_t, ForeignData_t, FunctionFeature_t, Gap_t, Head_t, HeadFeature_t, Headspan_t, Hiddenword_t, Hyphbreak_t, Label_t, LangAnnotation_t, LemmaAnnotation_t, LevelFeature_t, Linebreak_t, LinkReference_t, List_t, ListItem_t, Metric_t, ModalitiesLayer_t, Modality_t, ModalityFeature_t, Morpheme_t, MorphologyLayer_t, New_t, Note_t, Observation_t, ObservationLayer_t, Original_t, Paragraph_t, Part_t, PhonContent_t, Phoneme_t, PhonologyLayer_t, PolarityFeature_t, PosAnnotation_t, Predicate_t, Quote_t, Reference_t, Relation_t, Row_t, Scope_t, SemanticRole_t, SemanticRolesLayer_t, SenseAnnotation_t, Sentence_t, Sentiment_t, SentimentLayer_t, SizeFeature_t, Source_t, SpanRelation_t, SpanRelationLayer_t, Speech_t, Statement_t, StatementLayer_t, StatementRelation_t, StrengthFeature_t, String_t, StyleFeature_t, SubjectivityAnnotation_t, Suggestion_t, SynsetFeature_t, SyntacticUnit_t, SyntaxLayer_t, Table_t, TableHead_t, Target_t, Term_t, Text_t, TextContent_t, TextMarkupCorrection_t, TextMarkupError_t, TextMarkupGap_t, TextMarkupHSpace_t, TextMarkupLanguage_t, TextMarkupReference_t, TextMarkupString_t, TextMarkupStyle_t, TextMarkupWhitespace_t, TimeFeature_t, TimeSegment_t, TimingLayer_t, Utterance_t, ValueFeature_t, Whitespace_t, Word_t, WordReference_t, ProcessingInstruction_t, XmlComment_t, XmlText_t,  LastElement };

  inline ElementType& operator++( ElementType &et ){
    return et = ( ElementType::LastElement == et )
      ? ElementType::BASE
      : ElementType((unsigned int)(et)+1);
  }

  /** AnnotatorType is the Internal representation of the Annatator attribute
   *
   */
  enum class AnnotatorType: int {
    UNDEFINED = 0,   ///< The value is (yet) unknown
      AUTO = 1,      ///< The value is automaticly assigned
      MANUAL = 2,    ///< The value is manually assigned
      GENERATOR = 3, ///< The value is added by a generator
      DATASOURCE = 4 ///< The value comes from an external datasource
      };

  //foliaspec:attributes
  //Defines all common FoLiA attributes (as part of the Attrib enumeration)
  enum class Attrib : int { NO_ATT=0, ///<No attribute
ID=1,  ///<xml:id: The ID of the element; this has to be a unique in the entire document or collection of documents (corpus). All identifiers in FoLiA are of the `XML NCName <https://www.w3.org/TR/1999/WD-xmlschema-2-19990924/#NCName>`_ datatype, which roughly means it is a unique string that has to start with a letter (not a number or symbol), may contain numbers, but may never contain colons or spaces. FoLiA does not define any naming convention for IDs.
CLASS=2,  ///<class: The class of the annotation, i.e. the annotation tag in the vocabulary defined by ``set``.
ANNOTATOR=4,  ///<annotator: This is an older alternative to the ``processor`` attribute, without support for full provenance. The annotator attribute simply refers to the name o ID of the system or human annotator that made the annotation.
CONFIDENCE=8,  ///<confidence: A floating point value between zero and one; expresses the confidence the annotator places in his annotation.
N=16,  ///<n: A number in a sequence, corresponding to a number in the original document, for example chapter numbers, section numbers, list item numbers. This this not have to be an actual number but other sequence identifiers are also possible (think alphanumeric characters or roman numerals).
DATETIME=32,  ///<datetime: The date and time when this annotation was recorded, the format is ``YYYY-MM-DDThh:mm:ss`` (note the literal T in the middle to separate date from time), as per the XSD Datetime data type.
BEGINTIME=64,  ///<begintime: A timestamp in ``HH:MM:SS.MMM`` format, indicating the begin time of the speech. If a sound clip is specified (``src``); the timestamp refers to a location in the soundclip.
ENDTIME=128,  ///<endtime: A timestamp in ``HH:MM:SS.MMM`` format, indicating the end time of the speech. If a sound clip is specified (``src``); the timestamp refers to a location in the soundclip.
SRC=256,  ///<src: Points to a file or full URL of a sound or video file. This attribute is inheritable.
SPEAKER=512,  ///<speaker: A string identifying the speaker. This attribute is inheritable. Multiple speakers are not allowed, simply do not specify a speaker on a certain level if you are unable to link the speech to a specific (single) speaker.
TEXTCLASS=1024,  ///<textclass: Refers to the text class this annotation is based on. This is an advanced attribute, if not specified, it defaults to ``current``. See :ref:`textclass_attribute`.
METADATA=2048,
IDREF=4096,  ///<id: A reference to the ID of another element. This is a reference and not an assignment, unlike xml:id, so do not confuse the two! It is only supported on certain elements that are referential in nature.
SPACE=8192,  ///<space: This attribute indicates whether spacing should be inserted after this element (it's default value is always ``yes``, so it does not need to be specified in that case), but if tokens or other structural elements are glued together then the value should be set to ``no``. This allows for reconstruction of the detokenised original text.
TAG=16384,  ///<tag: Contains a space separated list of processing tags associated with the element. A processing tag carries arbitrary user-defined information that may aid in processing a document. It may carry cues on how a specific tool should treat a specific element. The tag vocabulary is specific to the tool that processes the document. Tags carry no instrinsic meaning for the data representation and should not be used except to inform/aid processors in their task. Processors are encouraged to clean up the tags they use. Ideally, published FoLiA documents at the end of a processing pipeline carry no further tags. For encoding actual data, use ``class`` and optionally features instead.
ALL=32768 };

  inline Attrib& operator++( Attrib & a ){
    return a = ( Attrib::ALL == a )
      ? Attrib::NO_ATT
      : ( Attrib::NO_ATT == a ? Attrib::ID : Attrib(int(a)<<1) );
  }

  DEFINE_ENUM_FLAG_OPERATORS(Attrib)

  std::string toString( const Attrib );
  std::ostream& operator<<( std::ostream&, const Attrib& );

#undef DOMAIN // ugly hack but Clang defines DOMAIN in math.h

  /*
   * Annotation types tie FoLiA elements to a particular kind of annotation.
   * Especially declarations make use of this.
   *  static const annotation_type = {AnnotationType}
   */

  //foliaspec:annotationtype
  //Defines all annotation types (as part of the AnnotationType enumeration)
  enum class AnnotationType : int { NO_ANN, ///<No type dummy
    TEXT, ///<Text Annotation: Text annotation associates actual textual content with structural elements, without it a document would be textless. FoLiA treats it as an annotation like any other.
    TOKEN, ///<Token Annotation: This annotation type introduces a tokenisation layer for the document. The terms **token** and **word** are used interchangeably in FoLiA as FoLiA itself does not commit to a specific tokenisation paradigm. Tokenisation is a prerequisite for the majority of linguistic annotation types offered by FoLiA and it is one of the most fundamental types of Structure Annotation. The words/tokens are typically embedded in other types of structure elements, such as sentences or paragraphs.
    DIVISION, ///<Division Annotation: Structure annotation representing some kind of division, typically used for chapters, sections, subsections (up to the set definition). Divisions may be nested at will, and may include almost all kinds of other structure elements.
    PARAGRAPH, ///<Paragraph Annotation: Represents a paragraph and holds further structure annotation such as sentences.
    HEAD, ///<Head Annotation: The ``head`` element is used to provide a header or title for the structure element in which it is embedded, usually a division (``<div>``)
    LIST, ///<List Annotation: Structure annotation for enumeration/itemisation, e.g. bulleted lists.
    FIGURE, ///<Figure Annotation: Structure annotation for including pictures, optionally captioned, in documents.
    WHITESPACE, ///<Vertical Whitespace: Structure annotation introducing vertical whitespace
    LINEBREAK, ///<Linebreak: Structure annotation representing a single linebreak and with special facilities to denote pagebreaks.
    SENTENCE, ///<Sentence Annotation: Structure annotation representing a sentence. Sentence detection is a common stage in NLP alongside tokenisation.
    POS, ///<Part-of-Speech Annotation: Part-of-Speech Annotation, one of the most common types of linguistic annotation. Assigns a lexical class to words.
    LEMMA, ///<Lemmatisation: Lemma Annotation, one of the most common types of linguistic annotation. Represents the canonical form of a word.
    DOMAIN, ///<Domain/topic Annotation: Domain/topic Annotation. A form of inline annotation used to assign a certain domain or topic to a structure element.
    SENSE, ///<Sense Annotation: Sense Annotation allows to assign a lexical semantic sense to a word.
    SYNTAX, ///<Syntactic Annotation: Assign grammatical categories to spans of words. Syntactic units are nestable and allow representation of complete syntax trees that are usually the result of consistuency parsing.
    CHUNKING, ///<Chunking: Assigns shallow grammatical categories to spans of words. Unlike syntax annotation, chunks are not nestable. They are often produced by a process called Shallow Parsing, or alternatively, chunking.
    ENTITY, ///<Entity Annotation: Entity annotation is a broad and common category in FoLiA. It is used for specifying all kinds of multi-word expressions, including but not limited to named entities. The set definition used determines the vocabulary and therefore the precise nature of the entity annotation.
    CORRECTION, ///<Correction Annotation: Corrections are one of the most complex annotation types in FoLiA. Corrections can be applied not just over text, but over any type of structure annotation, inline annotation or span annotation. Corrections explicitly preserve the original, and recursively so if corrections are done over other corrections.
    ERRORDETECTION, ///<Error Detection: This annotation type is deprecated in favour of `Observation Annotation` and only exists for backward compatibility.
    PHON, ///<Phonetic Annotation: This is the phonetic analogy to text content (``<t>``) and allows associating a phonetic transcription with any structural element, it is often used in a speech context. Note that for actual segmentation into phonemes, FoLiA has another related type: ``Phonological Annotation``
    SUBJECTIVITY, ///<Subjectivity Annotation: This annotation type is deprecated in favour of `Sentiment Annotation` and only exists for backward compatibility.
    MORPHOLOGICAL, ///<Morphological Annotation: Morphological Annotation allows splitting a word/token into morphemes, morphemes itself may be nested. It is embedded within a layer ``morphology`` which can be embedded within word/tokens.
    EVENT, ///<Event Annotation: Structural annotation type representing events, often used in new media contexts for things such as tweets, chat messages and forum posts (as defined by a user-defined set definition). Note that a more linguistic kind of event annotation can be accomplished with `Entity Annotation` or even `Time Segmentation` rather than this one.
    DEPENDENCY, ///<Dependency Annotation: Dependency relations are syntactic relations between spans of tokens. A dependency relation takes a particular class and consists of a single head component and a single dependent component.
    TIMESEGMENT, ///<Time Segmentation: FoLiA supports time segmentation to allow for more fine-grained control of timing information by associating spans of words/tokens with exact timestamps. It can provide a more linguistic alternative to `Event Annotation`.
    GAP, ///<Gap Annotation: Sometimes there are parts of a document you want to skip and not annotate at all, but include as is. This is where gap annotation comes in, the user-defined set may indicate the kind of gap. Common omissions in books are for example front-matter and back-matter, i.e. the cover.
    QUOTE, ///<Quote Annotation: Structural annotation used to explicitly mark quoted speech, i.e. that what is reported to be said and appears in the text in some form of quotation marks.
    NOTE, ///<Note Annotation: Structural annotation used for notes, such as footnotes or warnings or notice blocks.
    REFERENCE, ///<Reference Annotation: Structural annotation for referring to other annotation types. Used e.g. for referring to bibliography entries (citations) and footnotes.
    RELATION, ///<Relation Annotation: FoLiA provides a facility to relate arbitrary parts of your document with other parts of your document, or even with parts of other FoLiA documents or external resources, even in other formats. It thus allows linking resources together. Within this context, the ``xref`` element is used to refer to the linked FoLiA elements.
    SPANRELATION, ///<Span Relation Annotation: Span relations are a stand-off extension of relation annotation that allows for more complex relations, such as word alignments that include many-to-one, one-to-many or many-to-many alignments. One of its uses is in the alignment of multiple translations of (parts) of a text.
    COREFERENCE, ///<Coreference Annotation: Relations between words that refer to the same referent (anaphora) are expressed in FoLiA using Coreference Annotation. The co-reference relations are expressed by specifying the entire chain in which all links are coreferent.
    SEMROLE, ///<Semantic Role Annotation: This span annotation type allows for the expression of semantic roles, or thematic roles. It is often used together with `Predicate Annotation`
    METRIC, ///<Metric Annotation: Metric Annotation is a form of higher-order annotation that allows annotation of some kind of measurement. The type of measurement is defined by the class, which in turn is defined by the set as always. The metric element has a ``value`` attribute that stores the actual measurement, the value is often numeric but this needs not be the case.
    LANG, ///<Language Annotation: Language Annotation simply identifies the language a part of the text is in. Though this information is often part of the metadata, this form is considered an actual annotation.
    STRING, ///<String Annotation: This is a form of higher-order annotation for selecting an arbitrary substring of a text, even untokenised, and allows further forms of higher-order annotation on the substring. It is also tied to a form of text markup annotation.
    TABLE, ///<Table Annotation: Structural annotation type for creating a simple tabular environment, i.e. a table with rows, columns and cells and an optional header.
    STYLE, ///<Style Annotation: This is a text markup annotation type for applying styling to text. The actual styling is defined by the user-defined set definition and can for example included classes such as italics, bold, underline
    PART, ///<Part Annotation: The structure element ``part`` is a fairly abstract structure element that should only be used when a more specific structure element is not available. Most notably, the part element should never be used for representation of morphemes or phonemes! Part can be used to divide a larger structure element, such as a division, or a paragraph into arbitrary subparts.
    UTTERANCE, ///<Utterance Annotation: An utterance is a structure element that may consist of words or sentences, which in turn may contain words. The opposite is also true, a sentence may consist of multiple utterances. Utterances are often used in the absence of sentences in a speech context, where neat grammatical sentences can not always be distinguished.
    ENTRY, ///<Entry Annotation: FoLiA has a set of structure elements that can be used to represent collections such as glossaries, dictionaries, thesauri, and wordnets. `Entry annotation` defines the entries in such collections, `Term annotation` defines the terms, and `Definition Annotation` provides the definitions.
    TERM, ///<Term Annotation: FoLiA has a set of structure elements that can be used to represent collections such as glossaries, dictionaries, thesauri, and wordnets. `Entry annotation` defines the entries in such collections, `Term annotation` defines the terms, and `Definition Annotation` provides the definitions.
    DEFINITION, ///<Definition Annotation: FoLiA has a set of structure elements that can be used to represent collections such as glossaries, dictionaries, thesauri, and wordnets. `Entry annotation` defines the entries in such collections, `Term annotation` defines the terms, and `Definition Annotation` provides the definitions.
    EXAMPLE, ///<Example Annotation: FoLiA has a set of structure elements that can be used to represent collections such as glossaries, dictionaries, thesauri, and wordnets. `Examples annotation` defines examples in such collections.
    PHONOLOGICAL, ///<Phonological Annotation: The smallest unit of annotatable speech in FoLiA is the phoneme level. The phoneme element is a form of structure annotation used for phonemes.  Alike to morphology, it is embedded within a layer ``phonology`` which can be embedded within word/tokens.
    PREDICATE, ///<Predicate Annotation: Allows annotation of predicates, this annotation type is usually used together with Semantic Role Annotation. The types of predicates are defined by a user-defined set definition.
    OBSERVATION, ///<Observation Annotation: Observation annotation is used to make an observation pertaining to one or more word tokens.  Observations offer a an external qualification on part of a text. The qualification is expressed by the class, in turn defined by a set. The precise semantics of the observation depends on the user-defined set.
    SENTIMENT, ///<Sentiment Annotation: Sentiment analysis marks subjective information such as sentiments or attitudes expressed in text. The sentiments/attitudes are defined by a user-defined set definition.
    STATEMENT, ///<Statement Annotation: Statement annotation, sometimes also refered to as attribution, allows to decompose statements into the source of the statement, the content of the statement, and the way these relate, provided these are made explicit in the text.
    ALTERNATIVE, ///<Alternative Annotation: This form of higher-order annotation encapsulates alternative annotations, i.e. annotations that are posed as an alternative option rather than the authoratitive chosen annotation
    RAWCONTENT, ///<Raw Content: This associates raw text content which can not carry any further annotation. It is used in the context of :ref:`gap_annotation`
    COMMENT, ///<Comment Annotation: This is a form of higher-order annotation that allows you to associate comments with almost all other annotation elements
    DESCRIPTION, ///<Description Annotation: This is a form of higher-order annotation that allows you to associate descriptions with almost all other annotation elements
    HYPHENATION, ///<Hyphenation Annotation: This is a text-markup annotation form that indicates where in the original text a linebreak was inserted and a word was hyphenised.
    HIDDENTOKEN, ///<Hidden Token Annotation: This annotation type allows for a hidden token layer in the document. Hidden tokens are ignored for most intents and purposes but may serve a purpose when annotations on implicit tokens is required, for example as targets for syntactic movement annotation.
    MODALITY, ///<Modality Annotation: Modality annotation is used to describe the relationship between cue word(s) and the scope it covers. It is primarily used for the annotation of negation, but also for the annotation of factuality, certainty and truthfulness:.
    EXTERNAL, ///<External Annotation: External annotation makes a reference to an external FoLiA document whose structure is inserted at the exact place the external element occurs.
    HSPACE, ///<Horizontal Whitespace: Markup annotation introducing horizontal whitespace
    ETYMOLOGY, ///<Etymology Annotation: Etymology Annotation allows to relate a word/morpheme to its historic origin (often in some kind of etymological database)
LAST_ANN };

  inline AnnotationType& operator++( AnnotationType &at ){
    return at = ( AnnotationType::LAST_ANN == at )
      ? AnnotationType::NO_ANN
      : AnnotationType(int(at)+1);
  }

  std::string toString( const AnnotationType& );
  AnnotationType stringToAnnotationType( const std::string& );

  AnnotatorType stringToAnnotatorType( const std::string& );
  std::string toString( const AnnotatorType& );

  std::string toString( const ElementType& );
  ElementType stringToElementType( const std::string& );

  ElementType layertypeof( ElementType );

} // namespace folia

namespace TiCC {
  // add specializations to the TiCC stringTo() and toString() family

  template<>
    inline folia::AnnotationType stringTo( const std::string& str ) {
    return folia::stringToAnnotationType( str );
  }

  template<>
    inline folia::ElementType stringTo( const std::string& str ) {
    return folia::stringToElementType( str );
  }

  template<>
    inline folia::AnnotatorType stringTo( const std::string& str ) {
    return folia::stringToAnnotatorType( str );
  }

  inline std::ostream& operator<<( std::ostream& os,
				   const folia::ElementType& el ){
    os << folia::toString( el );
    return os;
  }

  inline std::ostream& operator<<( std::ostream& os,
				   const folia::AnnotatorType& at ){
    os << folia::toString(at);
    return os;
  }

  inline std::ostream& operator<<( std::ostream& os,
				   const folia::AnnotationType& at ){
    os << folia::toString( at );
    return os;
  }

} // namespace TiCC

#endif
