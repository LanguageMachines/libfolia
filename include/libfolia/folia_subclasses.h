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

#ifndef FOLIA_SUBCLASSES_H
#define FOLIA_SUBCLASSES_H

namespace folia {

#define ADD_PROTECTED_CONSTRUCTORS( CLASS, BASE )		\
  friend void static_init();					\
protected:							\
 explicit CLASS( const properties& props, Document *d=0 ):	\
   BASE( props, d ){};						\
 CLASS( const properties& props, FoliaElement *p ):		\
   BASE( props, p ){};						\
 CLASS( const CLASS& ) = delete;				\
 CLASS& operator=( const CLASS& ) = delete

#define ADD_DEFAULT_CONSTRUCTORS( CLASS, BASE )		\
  friend void static_init();				\
protected:						\
 ~CLASS() override {};					\
public:							\
 explicit CLASS( const KWargs& a, Document *d=0 ):	\
   BASE( PROPS, d ){ classInit(a); };			\
 explicit CLASS( Document *d=0 ):			\
   BASE( PROPS, d ){};					\
 CLASS( const KWargs& a, FoliaElement *p ):		\
   BASE( PROPS, p ){ classInit(a); };			\
 explicit CLASS( FoliaElement *p ):			\
   BASE( PROPS, p ){}					\
 static properties PROPS

 class AbstractStructureElement:
    public AbstractElement,
    public AllowGenerateID,
    public AllowInlineAnnotation
 {
   // DO NOT USE AbstractStructureElement as a real node!!
 protected:
   ADD_PROTECTED_CONSTRUCTORS( AbstractStructureElement, AbstractElement );
 public:
   FoliaElement *append( FoliaElement* ) override;
   std::vector<Paragraph*> paragraphs() const override;
   std::vector<Sentence*> sentences() const override;
   std::vector<Word*> words( const std::string& ="" ) const override;
   Sentence *sentences( size_t ) const override;
   Sentence *rsentences( size_t ) const override;
   Paragraph *paragraphs( size_t ) const override;
   Paragraph *rparagraphs( size_t ) const override;
   Word *words( size_t, const std::string& ="" ) const override;
   Word *rwords( size_t, const std::string& ="" ) const override;
   const Word* resolveword( const std::string& ) const override;
 };

  class AbstractWord: public AbstractStructureElement {
    /// Interface class that is inherited by word-like (wrefable)
    /// elements (Word, Hiddenword, Morpheme, Phoneme)
  protected:
    ADD_PROTECTED_CONSTRUCTORS( AbstractWord, AbstractStructureElement );
  public:
    Sentence *sentence() const override;
    Paragraph *paragraph() const override;
    Division *division() const override;
    const std::string pos( const std::string& = "" ) const override;
    const std::string lemma( const std::string& = "" ) const override;
    SenseAnnotation *sense( const std::string& = "" ) const;
    DomainAnnotation *domain( const std::string& = "" ) const;
    std::vector<Morpheme *> morphemes( const std::string& ="" ) const override;
    std::vector<Phoneme *> phonemes( const std::string& ="" ) const;
    Morpheme *morpheme( size_t, const std::string& ="" ) const override;
    Phoneme *phoneme( size_t, const std::string& ="" ) const;
    std::vector<Correction *> getcorrections( const std::string& ="",
					      const std::string& ="" ) const NOT_IMPLEMENTED;
    Correction *getcorrection( const std::string& ="",
			       const std::string& ="" ) const NOT_IMPLEMENTED;
    std::vector<AbstractSpanAnnotation*> findspans( ElementType,
						    const std::string& ="" ) const override;
  };

  class AbstractInlineAnnotation:
    public AbstractElement,
    public AllowGenerateID
  {
    // DO NOT USE AbstractInlineAnnotation as a real node!!
  protected:
    ADD_PROTECTED_CONSTRUCTORS( AbstractInlineAnnotation, AbstractElement );
  };

  class AbstractHigherOrderAnnotation:
    public AbstractElement
  {
    // DO NOT USE AbstractHigherOrderAnnotation as a real node!!
  protected:
    ADD_PROTECTED_CONSTRUCTORS( AbstractHigherOrderAnnotation, AbstractElement );
  };

  class AbstractSpanAnnotation:
    public AbstractElement,
    public AllowGenerateID,
    public AllowInlineAnnotation
  {
    // DO NOT USE AbstractSpanAnnotation as a real node!!
  protected:
    ADD_PROTECTED_CONSTRUCTORS( AbstractSpanAnnotation, AbstractElement );
  public:
    xmlNode *xml( bool, bool=false ) const override;
    FoliaElement *append( FoliaElement* ) override;

    std::vector<FoliaElement*> wrefs() const override;
    FoliaElement *wrefs( size_t ) const override;
  };

  class SpanRelation: public AbstractElement {
  public:
    ADD_DEFAULT_CONSTRUCTORS( SpanRelation, AbstractElement );
  };

  const std::string EMPTY_STRING = "";

  class AbstractTextMarkup:
    public AbstractElement,
    public AllowXlink
  {
    // DO NOT USE AbstractTextMarkup as a real node!!
  protected:
    ADD_PROTECTED_CONSTRUCTORS( AbstractTextMarkup, AbstractElement );
  public:
    void setAttributes( KWargs& ) override;
    KWargs collectAttributes() const override;
    const FoliaElement* resolveid() const override;
  protected:
    const std::string& get_delimiter( const TextPolicy& ) const override {
      return EMPTY_STRING; };
    std::string idref;
  };

  class TextMarkupGap: public AbstractTextMarkup {
  public:
    ADD_DEFAULT_CONSTRUCTORS( TextMarkupGap, AbstractTextMarkup );
  };

  class TextMarkupString: public AbstractTextMarkup {
  public:
    ADD_DEFAULT_CONSTRUCTORS( TextMarkupString, AbstractTextMarkup );
  };

  class TextMarkupCorrection: public AbstractTextMarkup {
  public:
    ADD_DEFAULT_CONSTRUCTORS( TextMarkupCorrection, AbstractTextMarkup );
    void setAttributes( KWargs& ) override;
    KWargs collectAttributes() const override;
  private:
    const UnicodeString private_text( const TextPolicy& ) const override;
    std::string _original;
  };

  class TextMarkupError: public AbstractTextMarkup {
  public:
    ADD_DEFAULT_CONSTRUCTORS( TextMarkupError, AbstractTextMarkup );
  };

  class TextMarkupStyle: public AbstractTextMarkup {
  public:
    ADD_DEFAULT_CONSTRUCTORS( TextMarkupStyle, AbstractTextMarkup );
  };

  class Hyphbreak: public AbstractTextMarkup {
  public:
    ADD_DEFAULT_CONSTRUCTORS( Hyphbreak, AbstractTextMarkup );
  private:
    const UnicodeString private_text( const TextPolicy& ) const override;
  };

  class TextMarkupReference: public AbstractTextMarkup {
  public:
    ADD_DEFAULT_CONSTRUCTORS( TextMarkupReference, AbstractTextMarkup );
    KWargs collectAttributes() const override;
    void setAttributes( KWargs& ) override;

  private:
    std::string ref_id;
    std::string ref_type;
    std::string _type;
    std::string _format = "text/folia+xml";
  };

  class TextMarkupHSpace: public AbstractTextMarkup {
  public:
    ADD_DEFAULT_CONSTRUCTORS( TextMarkupHSpace, AbstractTextMarkup );
  private:
    const UnicodeString private_text( const TextPolicy& ) const override;
  };

  class TextMarkupWhitespace: public AbstractTextMarkup {
  public:
    ADD_DEFAULT_CONSTRUCTORS( TextMarkupWhitespace, AbstractTextMarkup );
  };

  class TextMarkupLanguage: public AbstractTextMarkup {
  public:
    ADD_DEFAULT_CONSTRUCTORS( TextMarkupLanguage, AbstractTextMarkup );
  };

  class AbstractContentAnnotation:
    public AbstractElement,
    public AllowGenerateID
  {
    // DO NOT USE AbstractContentAnnotation as a real node!!
  protected:
    ADD_PROTECTED_CONSTRUCTORS( AbstractContentAnnotation, AbstractElement);
  public:
    void setAttributes( KWargs& ) override;
    KWargs collectAttributes() const override;
    FoliaElement *get_reference( int&, bool=true ) const;
    int offset() const override { return _offset; };
    const std::string& ref() const { return _ref; };
  private:
    virtual FoliaElement *find_default_reference() const = 0;
    void set_offset( int o ) const override { _offset = o; };
    mutable int _offset = -1;
    std::string _ref;
  };

  class TextContent:
    public AbstractContentAnnotation,
    public AllowXlink
  {
  public:
    ADD_DEFAULT_CONSTRUCTORS( TextContent, AbstractContentAnnotation );
    void setAttributes( KWargs& ) override;
    KWargs collectAttributes() const override;
    std::vector<FoliaElement*> find_replacables( FoliaElement * ) const override;
    const std::string set_to_current() override {
      // Don't use without thinking twice!
      std::string res = cls();
      set_cls( "current" );
      return res;
    }
    bool addable( const FoliaElement * ) const override;
    FoliaElement *postappend() override;
  private:
    FoliaElement *find_default_reference() const override;
  };

  class PhonContent: public AbstractContentAnnotation {
  public:
    ADD_DEFAULT_CONSTRUCTORS( PhonContent,
			      AbstractContentAnnotation );
    void setAttributes( KWargs& ) override;
    KWargs collectAttributes() const override;
    const UnicodeString phon( const TextPolicy& ) const override;
    const UnicodeString phon( const std::string& = "current",
			      TEXT_FLAGS = TEXT_FLAGS::NONE,
			      bool = false ) const override;
    FoliaElement *postappend() override;
  public:
    FoliaElement *find_default_reference() const override;
  };

  class FoLiA: public AbstractElement {
  public:
    ADD_DEFAULT_CONSTRUCTORS( FoLiA, AbstractElement );
    FoliaElement *parseXml( const xmlNode * ) override;
    void setAttributes( KWargs& ) override;
  private:
    const UnicodeString private_text( const TextPolicy& ) const override;
  };

  class DCOI: public AbstractElement {
  public:
    ADD_DEFAULT_CONSTRUCTORS( DCOI, AbstractElement );
  };

  class Head: public AbstractStructureElement {
  public:
    ADD_DEFAULT_CONSTRUCTORS( Head, AbstractStructureElement );
  };

  class TableHead: public AbstractStructureElement {
  public:
    ADD_DEFAULT_CONSTRUCTORS( TableHead, AbstractStructureElement );
  };

  class Table: public AbstractStructureElement {
  public:
    ADD_DEFAULT_CONSTRUCTORS( Table, AbstractStructureElement );
  };

  class Row: public AbstractStructureElement {
  public:
    ADD_DEFAULT_CONSTRUCTORS( Row, AbstractStructureElement );
    const UnicodeString private_text( const TextPolicy& ) const override;
  };

  class Cell: public AbstractStructureElement {
  public:
    ADD_DEFAULT_CONSTRUCTORS( Cell, AbstractStructureElement );
    const UnicodeString private_text( const TextPolicy& ) const override;
  };

  class Gap: public AbstractElement {
  public:
    ADD_DEFAULT_CONSTRUCTORS( Gap, AbstractElement );
    const std::string content() const override;
  };

  class Content: public AbstractElement {
  public:
    ADD_DEFAULT_CONSTRUCTORS( Content, AbstractElement );

    FoliaElement *parseXml( const xmlNode * ) override;
    xmlNode *xml( bool, bool = false ) const override;
    const std::string content() const override { return value; };
    void setAttributes( KWargs& ) override;
  private:
    std::string value;
  };

  class Metric: public AbstractElement {
  public:
    ADD_DEFAULT_CONSTRUCTORS( Metric, AbstractElement );
  };

  class Division: public AbstractStructureElement {
  public:
    ADD_DEFAULT_CONSTRUCTORS( Division, AbstractStructureElement );
    Head *head() const override;
  };

  class Linebreak:
    public AbstractStructureElement,
    public AllowXlink
  {
  public:
    ADD_DEFAULT_CONSTRUCTORS( Linebreak, AbstractStructureElement );
    void setAttributes( KWargs& ) override;
    KWargs collectAttributes() const override;
  private:
    const UnicodeString private_text( const TextPolicy& ) const override {
      return "\n";
    }
    std::string _pagenr;
    std::string _linenr;
    bool _newpage = false;
  };

  class Whitespace: public AbstractStructureElement {
  public:
    ADD_DEFAULT_CONSTRUCTORS( Whitespace, AbstractStructureElement );
  private:
    const UnicodeString private_text( const TextPolicy& ) const override {
      return "\n\n";
    }
  };

  class Word:
    public AbstractWord
  {
  public:
    ADD_DEFAULT_CONSTRUCTORS( Word, AbstractWord );

    Correction *split( FoliaElement *, FoliaElement *,
		       const std::string& = "" ) override;
    Correction *incorrection() const override;
    Word *previous() const override;
    Word *next() const override;
    std::vector<Word*> context( size_t,
				const std::string& ="" ) const override;
    std::vector<Word*> leftcontext( size_t,
				    const std::string& = "" ) const override;
    std::vector<Word*> rightcontext( size_t,
				     const std::string& ="" ) const override;
    FoliaElement *append( FoliaElement * ) override;
    const Word* resolveword( const std::string& ) const override;
    void setAttributes( KWargs& ) override;
    const std::string& get_delimiter( const TextPolicy& ) const override;
    MorphologyLayer *addMorphologyLayer( const KWargs& ) override;
    MorphologyLayer *getMorphologyLayers( const std::string&,
					  std::vector<MorphologyLayer*>& ) const override;
    bool is_placeholder() const { return _is_placeholder; };
    bool addable( const FoliaElement * ) const override;
  private:
    bool _is_placeholder = false;
  };

  class Hiddenword:
    public AbstractWord
  {
  public:
    ADD_DEFAULT_CONSTRUCTORS( Hiddenword, AbstractWord );
  };

  class Part: public AbstractStructureElement {
  public:
    ADD_DEFAULT_CONSTRUCTORS( Part, AbstractStructureElement );
  };

  class String:
    public AbstractElement,
    public AllowInlineAnnotation,
    public AllowGenerateID
  {
  public:
    ADD_DEFAULT_CONSTRUCTORS( String, AbstractElement );
  };

  class Sentence:
    public AbstractStructureElement
  {
  public:
    ADD_DEFAULT_CONSTRUCTORS( Sentence, AbstractStructureElement );

    Correction *splitWord( FoliaElement *, FoliaElement *,
			   FoliaElement *, const KWargs& );
    Correction *mergewords( FoliaElement *,
			    const std::vector<FoliaElement *>&,
			    const std::string& = "" ) override;
    Correction *deleteword( FoliaElement *, const std::string& ) override;
    Correction *insertword( FoliaElement *, FoliaElement *,
			    const std::string& ) override;
    std::vector<Word*> wordParts() const override;
  private:
    Correction *correctWords( const std::vector<FoliaElement *>&,
			      const std::vector<FoliaElement *>&,
			      const KWargs& );
  };

  class Speech: public AbstractStructureElement {
  public:
    ADD_DEFAULT_CONSTRUCTORS( Speech, AbstractStructureElement );
  };

  class Text: public AbstractStructureElement {
  public:
    ADD_DEFAULT_CONSTRUCTORS( Text, AbstractStructureElement );
  };

  class Utterance: public AbstractStructureElement {
  public:
    ADD_DEFAULT_CONSTRUCTORS( Utterance, AbstractStructureElement );
  };

  class Event: public AbstractStructureElement {
  public:
    ADD_DEFAULT_CONSTRUCTORS( Event, AbstractStructureElement );
  };

  class Caption: public AbstractStructureElement {
  public:
    ADD_DEFAULT_CONSTRUCTORS( Caption, AbstractStructureElement );
  };

  class Label: public AbstractStructureElement {
  public:
    ADD_DEFAULT_CONSTRUCTORS( Label, AbstractStructureElement );
  };

  class ListItem: public AbstractStructureElement {
  public:
    ADD_DEFAULT_CONSTRUCTORS( ListItem, AbstractStructureElement );
  };

  class List: public AbstractStructureElement {
  public:
    ADD_DEFAULT_CONSTRUCTORS( List, AbstractStructureElement );
  };

  class Figure: public AbstractStructureElement {
  public:
    ADD_DEFAULT_CONSTRUCTORS( Figure, AbstractStructureElement );
    const UnicodeString caption() const override;
  };

  class Paragraph: public AbstractStructureElement {
  public:
    ADD_DEFAULT_CONSTRUCTORS( Paragraph, AbstractStructureElement );
  };

  class AbstractSubtokenAnnotation:
    public AbstractStructureElement
  {
    // DO NOT USE AbstractSubtokenAnnotation as a real node!!
  protected:
    ADD_PROTECTED_CONSTRUCTORS( AbstractSubtokenAnnotation,
				AbstractStructureElement );
  };

  class Alternative:
    public AbstractHigherOrderAnnotation,
    public AllowInlineAnnotation,
    public AllowGenerateID
  {
  public:
    ADD_DEFAULT_CONSTRUCTORS( Alternative, AbstractHigherOrderAnnotation );
  };


  class AlternativeLayers: public AbstractElement {
  public:
    ADD_DEFAULT_CONSTRUCTORS( AlternativeLayers, AbstractElement );
  };

  class PosAnnotation: public AbstractInlineAnnotation {
  public:
    ADD_DEFAULT_CONSTRUCTORS( PosAnnotation, AbstractInlineAnnotation );
  };

  class LemmaAnnotation: public AbstractInlineAnnotation {
  public:
    ADD_DEFAULT_CONSTRUCTORS( LemmaAnnotation, AbstractInlineAnnotation );
  };

  class LangAnnotation: public AbstractInlineAnnotation {
  public:
    ADD_DEFAULT_CONSTRUCTORS( LangAnnotation, AbstractInlineAnnotation );
  };

  class Phoneme:
    public AbstractSubtokenAnnotation
  {
  public:
    ADD_DEFAULT_CONSTRUCTORS( Phoneme, AbstractSubtokenAnnotation );
  };

  class DomainAnnotation: public AbstractInlineAnnotation {
  public:
    ADD_DEFAULT_CONSTRUCTORS( DomainAnnotation, AbstractInlineAnnotation );
  };

  class SenseAnnotation: public AbstractInlineAnnotation {
  public:
    ADD_DEFAULT_CONSTRUCTORS( SenseAnnotation, AbstractInlineAnnotation );
  };

  class EtymologyAnnotation: public AbstractInlineAnnotation {
  public:
    ADD_DEFAULT_CONSTRUCTORS( EtymologyAnnotation,
			      AbstractInlineAnnotation );
  };

  class SubjectivityAnnotation: public AbstractInlineAnnotation {
  public:
    ADD_DEFAULT_CONSTRUCTORS( SubjectivityAnnotation,
			      AbstractInlineAnnotation );
  };

  class Quote: public AbstractStructureElement {
  public:
    ADD_DEFAULT_CONSTRUCTORS( Quote, AbstractStructureElement );
    std::vector<Word*> wordParts() const override;
    const std::string& get_delimiter( const TextPolicy& ) const override;
  };

  class AbstractFeature: public AbstractElement {
  protected:
    ADD_PROTECTED_CONSTRUCTORS( AbstractFeature, AbstractElement );
  public:
    void setAttributes( KWargs& ) override;
    KWargs collectAttributes() const override;
    const std::string subset() const override { return _subset; };
    std::string _subset;
  };

  class Feature: public AbstractFeature {
  public:
    ADD_DEFAULT_CONSTRUCTORS( Feature, AbstractFeature );
  };

  class BegindatetimeFeature: public AbstractFeature {
  public:
    ADD_DEFAULT_CONSTRUCTORS( BegindatetimeFeature, AbstractFeature );
  };

  class EnddatetimeFeature: public AbstractFeature {
  public:
    ADD_DEFAULT_CONSTRUCTORS( EnddatetimeFeature, AbstractFeature );
  };

  class SynsetFeature: public AbstractFeature {
  public:
    ADD_DEFAULT_CONSTRUCTORS( SynsetFeature, AbstractFeature );
  };

  class ActorFeature: public AbstractFeature {
  public:
    ADD_DEFAULT_CONSTRUCTORS( ActorFeature, AbstractFeature );
  };

  class PolarityFeature: public AbstractFeature {
  public:
    ADD_DEFAULT_CONSTRUCTORS( PolarityFeature, AbstractFeature );
  };

  class StrengthFeature: public AbstractFeature {
  public:
    ADD_DEFAULT_CONSTRUCTORS( StrengthFeature, AbstractFeature );
  };


  class HeadFeature: public AbstractFeature {
  public:
    ADD_DEFAULT_CONSTRUCTORS( HeadFeature, AbstractFeature );
  };

  class ValueFeature: public AbstractFeature {
  public:
    ADD_DEFAULT_CONSTRUCTORS( ValueFeature, AbstractFeature );
  };

  class FunctionFeature: public AbstractFeature {
  public:
    ADD_DEFAULT_CONSTRUCTORS( FunctionFeature, AbstractFeature );
  };

  class TimeFeature: public AbstractFeature {
  public:
    ADD_DEFAULT_CONSTRUCTORS( TimeFeature, AbstractFeature );
  };

  class LevelFeature: public AbstractFeature {
  public:
    ADD_DEFAULT_CONSTRUCTORS( LevelFeature, AbstractFeature );
  };

  class ModalityFeature: public AbstractFeature {
  public:
    ADD_DEFAULT_CONSTRUCTORS( ModalityFeature, AbstractFeature );
  };

  class StyleFeature: public AbstractFeature {
  public:
    ADD_DEFAULT_CONSTRUCTORS( StyleFeature, AbstractFeature );
  };

  class FontFeature: public AbstractFeature {
  public:
    ADD_DEFAULT_CONSTRUCTORS( FontFeature, AbstractFeature );
  };

  class SizeFeature: public AbstractFeature {
  public:
    ADD_DEFAULT_CONSTRUCTORS( SizeFeature, AbstractFeature );
  };

  class WordReference: public AbstractWord {
    friend std::vector<FoliaElement*> AbstractSpanAnnotation::wrefs() const;
  public:
    ADD_DEFAULT_CONSTRUCTORS( WordReference, AbstractWord );
    bool addable( const FoliaElement * ) const override;
    const bool& printable() const override {
      static bool t = true;
      return t;
    };
  private:
    FoliaElement *parseXml( const xmlNode *node ) override;
    xmlNode *xml( bool, bool=false ) const override;
    const UnicodeString private_text( const TextPolicy& tp ) const override {
      return _ref->private_text( tp );
    }
    const std::string& get_delimiter( const TextPolicy& tp ) const override {
      return _ref->get_delimiter( tp );
    }
    bool space() const override {
      return _ref->space();
    }
    FoliaElement *_ref = NULL;
    std::string _tval;
  };

  class Relation:
    public AbstractHigherOrderAnnotation,
    public AllowXlink {
  public:
    ADD_DEFAULT_CONSTRUCTORS( Relation, AbstractHigherOrderAnnotation );
    std::vector<FoliaElement *>resolve() const override;
    void setAttributes( KWargs& ) override;
    KWargs collectAttributes() const override;
  private:
    std::string _format = "text/folia+xml";
  };

  class LinkReference: public AbstractElement {
    friend class Relation;
  public:
    ADD_DEFAULT_CONSTRUCTORS( LinkReference, AbstractElement );

    KWargs collectAttributes() const override;
    void setAttributes( KWargs& ) override;
    const std::string& refid() const { return ref_id; };
    const std::string& type() const { return ref_type; };
    const std::string& t() const { return _t; };

  private:
    FoliaElement *parseXml( const xmlNode *node ) override;
    FoliaElement *resolve_element( const Relation *ref ) const;
    std::string ref_id;
    std::string ref_type;
    std::string _t;
  };

  class SyntacticUnit: public AbstractSpanAnnotation {
  public:
    ADD_DEFAULT_CONSTRUCTORS( SyntacticUnit, AbstractSpanAnnotation );
  };

  class Chunk: public AbstractSpanAnnotation {
  public:
    ADD_DEFAULT_CONSTRUCTORS( Chunk, AbstractSpanAnnotation );
  };

  class Entity: public AbstractSpanAnnotation {
  public:
    ADD_DEFAULT_CONSTRUCTORS( Entity, AbstractSpanAnnotation );
  };

  class AbstractSpanRole: public AbstractSpanAnnotation {
    // DO NOT USE AbstractSpanRole as a real node!!
  protected:
    ADD_PROTECTED_CONSTRUCTORS( AbstractSpanRole, AbstractSpanAnnotation );
  };

  class Headspan: public AbstractSpanRole {
  public:
    ADD_DEFAULT_CONSTRUCTORS( Headspan, AbstractSpanRole );
  };

  class DependencyDependent: public AbstractSpanRole {
  public:
    ADD_DEFAULT_CONSTRUCTORS( DependencyDependent, AbstractSpanRole );
  };

  class Source: public AbstractSpanRole {
  public:
    ADD_DEFAULT_CONSTRUCTORS( Source, AbstractSpanRole );
  };

  class Target: public AbstractSpanRole {
  public:
    ADD_DEFAULT_CONSTRUCTORS( Target, AbstractSpanRole );
  };

  class Scope: public AbstractSpanRole {
  public:
    ADD_DEFAULT_CONSTRUCTORS( Scope, AbstractSpanRole );
  };

  class Cue: public AbstractSpanRole {
  public:
    ADD_DEFAULT_CONSTRUCTORS( Cue, AbstractSpanRole );
  };

  class StatementRelation: public AbstractSpanRole {
  public:
    ADD_DEFAULT_CONSTRUCTORS( StatementRelation, AbstractSpanRole );
  };

  class Dependency: public AbstractSpanAnnotation {
  public:
    ADD_DEFAULT_CONSTRUCTORS( Dependency, AbstractSpanAnnotation );
    Headspan *head() const override;
    DependencyDependent *dependent() const override;
  };

  class CoreferenceLink: public AbstractSpanRole {
  public:
    ADD_DEFAULT_CONSTRUCTORS( CoreferenceLink, AbstractSpanRole );
  };

  class CoreferenceChain: public AbstractSpanAnnotation {
  public:
    ADD_DEFAULT_CONSTRUCTORS( CoreferenceChain, AbstractSpanAnnotation );
  };

  class SemanticRole: public AbstractSpanAnnotation {
  public:
    ADD_DEFAULT_CONSTRUCTORS( SemanticRole, AbstractSpanAnnotation );
  };

  class Predicate: public AbstractSpanAnnotation {
  public:
    ADD_DEFAULT_CONSTRUCTORS( Predicate, AbstractSpanAnnotation );
  };

  class Sentiment: public AbstractSpanAnnotation {
  public:
    ADD_DEFAULT_CONSTRUCTORS( Sentiment, AbstractSpanAnnotation );
  };

  class Modality: public AbstractSpanAnnotation {
  public:
    ADD_DEFAULT_CONSTRUCTORS( Modality, AbstractSpanAnnotation );
  };

  class Statement: public AbstractSpanAnnotation {
  public:
    ADD_DEFAULT_CONSTRUCTORS( Statement, AbstractSpanAnnotation );
  };

  class Observation: public AbstractSpanAnnotation {
  public:
    ADD_DEFAULT_CONSTRUCTORS( Observation, AbstractSpanAnnotation );
  };


  class AbstractAnnotationLayer:
    public AbstractElement,
    public AllowGenerateID,
    public AllowInlineAnnotation
  {
    // DO NOT USE AbstractAnnotationLayer as a real node!!
  protected:
    ADD_PROTECTED_CONSTRUCTORS( AbstractAnnotationLayer, AbstractElement );
  public:
    AbstractSpanAnnotation *findspan( const std::vector<FoliaElement*>& ) const override;
    FoliaElement *append( FoliaElement * ) override;
    KWargs collectAttributes() const override;
  private:
    void assignset( const  FoliaElement * );
  };

  class AbstractCorrectionChild: public AbstractElement {
    // DO NOT USE AbstractCorrectionChild as a real node!!
  protected:
    ADD_PROTECTED_CONSTRUCTORS( AbstractCorrectionChild, AbstractElement );
  };

  class New: public AbstractCorrectionChild {
  public:
    ADD_DEFAULT_CONSTRUCTORS( New, AbstractCorrectionChild );

    Correction *correct( const std::vector<FoliaElement*>&,
			 const std::vector<FoliaElement*>&,
			 const std::vector<FoliaElement*>&,
			 const std::vector<FoliaElement*>&,
			 const KWargs& ) override;
    Correction *correct( const std::string& = "" ) override;
    bool addable( const FoliaElement * ) const override;
  };

  class Current: public AbstractCorrectionChild {
  public:
    ADD_DEFAULT_CONSTRUCTORS( Current, AbstractCorrectionChild );
    bool addable( const FoliaElement * ) const override;
  };

  class Original: public AbstractCorrectionChild {
  public:
    ADD_DEFAULT_CONSTRUCTORS( Original, AbstractCorrectionChild );
    bool addable( const FoliaElement * ) const override;
  };

  class Suggestion: public AbstractCorrectionChild {
  public:
    ADD_DEFAULT_CONSTRUCTORS( Suggestion, AbstractCorrectionChild );
    void setAttributes( KWargs& ) override;
    KWargs collectAttributes() const override;
  private:
    std::string _split;
    std::string _merge;
  };

  class Description: public AbstractElement {
  public:
    ADD_DEFAULT_CONSTRUCTORS( Description, AbstractElement );

    const std::string description() const override { return _value; };
    void setAttributes( KWargs& ) override;
    FoliaElement *parseXml( const xmlNode * ) override;
    xmlNode *xml( bool, bool=false ) const override;
    void setvalue( const std::string& s ){ _value = s; };
  private:
    std::string _value;
  };

  class Comment: public AbstractElement {
  public:
    ADD_DEFAULT_CONSTRUCTORS( Comment, AbstractElement );

    const std::string& comment() const { return _value; };
    void setAttributes( KWargs& ) override;
    FoliaElement *parseXml( const xmlNode * ) override;
    xmlNode *xml( bool, bool=false ) const override;
    void setvalue( const std::string& s ){ _value = s; };
  private:
    std::string _value;
  };

  class XmlComment: public AbstractElement {
  public:
    ADD_DEFAULT_CONSTRUCTORS( XmlComment, AbstractElement );
    FoliaElement *parseXml( const xmlNode * ) override;
    xmlNode *xml( bool, bool=false ) const override;
    void setvalue( const std::string& s ){ _value = s; };
  private:
    const UnicodeString private_text( const TextPolicy& ) const override {
      return "";
    }
    std::string _value;
  };

  class ProcessingInstruction: public AbstractElement {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( ProcessingInstruction, AbstractElement );
    FoliaElement *parseXml( const xmlNode * ) override;
    xmlNode *xml( bool, bool=false ) const override;
    const std::string& target() const { return _target; };
    const std::string content() const override { return _content; };
  private:
    const UnicodeString private_text( const TextPolicy& ) const override {
      return "";
    }
    std::string _target;
    std::string _content;
  };

  class XmlText: public AbstractElement {
  public:
    ADD_DEFAULT_CONSTRUCTORS( XmlText, AbstractElement );
    FoliaElement *parseXml( const xmlNode * ) override;
    xmlNode *xml( bool, bool=false ) const override;
    void setvalue( const std::string& );
    void setuvalue( const UnicodeString& );
    const std::string& get_delimiter( const TextPolicy& ) const override {
      return EMPTY_STRING; };
    void setAttributes( KWargs& ) override;
  private:
    const UnicodeString private_text( const TextPolicy& ) const override;
    std::string _value; //UTF8 value
  };

  class External: public AbstractElement {
  public:
    ADD_DEFAULT_CONSTRUCTORS( External, AbstractElement );

    FoliaElement *parseXml( const xmlNode * ) override;
    void resolve_external();
  };

  class Note: public AbstractStructureElement {
  public:
    ADD_DEFAULT_CONSTRUCTORS( Note, AbstractStructureElement );
    void setAttributes( KWargs& ) override;
  private:
    std::string ref_id;
  };

  class Definition: public AbstractStructureElement {
  public:
    ADD_DEFAULT_CONSTRUCTORS( Definition, AbstractStructureElement );
  };

  class Term: public AbstractStructureElement {
  public:
    ADD_DEFAULT_CONSTRUCTORS( Term, AbstractStructureElement );
  };

  class Example: public AbstractStructureElement {
  public:
    ADD_DEFAULT_CONSTRUCTORS( Example, AbstractStructureElement );
  };

  class Entry: public AbstractStructureElement {
  public:
    ADD_DEFAULT_CONSTRUCTORS( Entry, AbstractStructureElement );
  };

  class Reference:
    public AbstractStructureElement,
    public AllowXlink
  {
    friend class Note;
  public:
    ADD_DEFAULT_CONSTRUCTORS( Reference, AbstractStructureElement );

    KWargs collectAttributes() const override;
    void setAttributes( KWargs& ) override;
  private:
    std::string ref_id;
    std::string ref_type;
    std::string _format = "text/folia+xml";
  };


  class Correction: public AbstractInlineAnnotation {
  public:
    ADD_DEFAULT_CONSTRUCTORS( Correction, AbstractInlineAnnotation );
    FoliaElement *parseXml( const xmlNode * ) override;
    bool addable( const FoliaElement * ) const override;
    void check_type_consistency() const;
    bool hasNew() const override;
    bool hasOriginal() const override;
    bool hasCurrent() const override;
    bool hasSuggestions() const override;
    New *getNew() const override;
    FoliaElement *getNew( size_t ) const override;
    Original *getOriginal() const override;
    FoliaElement *getOriginal( size_t ) const override;
    Current *getCurrent() const override;
    FoliaElement *getCurrent( size_t ) const override;
    std::vector<Suggestion*> suggestions() const override;
    Suggestion *suggestions( size_t ) const override;
    const TextContent *text_content( const TextPolicy& ) const override;
    const TextContent *text_content( const std::string& = "current",
				     bool = false ) const override;
    const PhonContent *phon_content( const TextPolicy& tp ) const override;
    const PhonContent *phon_content( const std::string& = "current",
				     bool = false ) const override;
    const std::string& get_delimiter( const TextPolicy& ) const override;
    Correction *correct( const std::vector<FoliaElement*>&,
			 const std::vector<FoliaElement*>&,
			 const std::vector<FoliaElement*>&,
			 const std::vector<FoliaElement*>&,
			 const KWargs& ) override;
    Correction *correct( const std::string& = "" ) override;
    bool space() const override;
  private:
    const UnicodeString private_text( const TextPolicy& ) const override;
  };

  class ErrorDetection: public AbstractInlineAnnotation  {
  public:
    ADD_DEFAULT_CONSTRUCTORS( ErrorDetection, AbstractInlineAnnotation );
  };

  class TimeSegment: public AbstractSpanAnnotation {
  public:
    ADD_DEFAULT_CONSTRUCTORS( TimeSegment, AbstractSpanAnnotation );
  };

  class Morpheme:
    public AbstractSubtokenAnnotation
  {
  public:
    ADD_DEFAULT_CONSTRUCTORS( Morpheme, AbstractSubtokenAnnotation );
  };

  class SyntaxLayer: public AbstractAnnotationLayer {
  public:
    ADD_DEFAULT_CONSTRUCTORS( SyntaxLayer, AbstractAnnotationLayer );
  };

  class ChunkingLayer: public AbstractAnnotationLayer {
  public:
    ADD_DEFAULT_CONSTRUCTORS( ChunkingLayer, AbstractAnnotationLayer );
  };

  class EntitiesLayer: public AbstractAnnotationLayer {
  public:
    ADD_DEFAULT_CONSTRUCTORS( EntitiesLayer, AbstractAnnotationLayer );
  };

  class TimingLayer: public AbstractAnnotationLayer {
  public:
    ADD_DEFAULT_CONSTRUCTORS( TimingLayer, AbstractAnnotationLayer );
  };

  class MorphologyLayer: public AbstractAnnotationLayer {
  public:
    ADD_DEFAULT_CONSTRUCTORS( MorphologyLayer, AbstractAnnotationLayer );
  };

  class PhonologyLayer: public AbstractAnnotationLayer {
  public:
    ADD_DEFAULT_CONSTRUCTORS( PhonologyLayer, AbstractAnnotationLayer );
  };

  class DependenciesLayer: public AbstractAnnotationLayer {
  public:
    ADD_DEFAULT_CONSTRUCTORS( DependenciesLayer, AbstractAnnotationLayer );
  };

  class CoreferenceLayer: public AbstractAnnotationLayer {
  public:
    ADD_DEFAULT_CONSTRUCTORS( CoreferenceLayer, AbstractAnnotationLayer );
  };

  class SemanticRolesLayer: public AbstractAnnotationLayer {
  public:
    ADD_DEFAULT_CONSTRUCTORS( SemanticRolesLayer, AbstractAnnotationLayer );
  };

  class StatementLayer: public AbstractAnnotationLayer {
  public:
    ADD_DEFAULT_CONSTRUCTORS( StatementLayer, AbstractAnnotationLayer );
  };

  class SentimentLayer: public AbstractAnnotationLayer {
  public:
    ADD_DEFAULT_CONSTRUCTORS( SentimentLayer, AbstractAnnotationLayer );
  };

  class ModalitiesLayer: public AbstractAnnotationLayer {
  public:
    ADD_DEFAULT_CONSTRUCTORS( ModalitiesLayer, AbstractAnnotationLayer );
  };

  class ObservationLayer: public AbstractAnnotationLayer {
  public:
    ADD_DEFAULT_CONSTRUCTORS( ObservationLayer, AbstractAnnotationLayer );
  };

  class SpanRelationLayer: public AbstractAnnotationLayer {
  public:
    ADD_DEFAULT_CONSTRUCTORS( SpanRelationLayer, AbstractAnnotationLayer );
  };

  ///
  /// ForeignData is special. It implements it's own destructor
  ///
  class ForeignData: public AbstractElement {
    friend void static_init();
  public:
    explicit ForeignData( Document *d=0 ):
      AbstractElement( PROPS, d ){};
    explicit ForeignData( FoliaElement *p ):
      AbstractElement( PROPS, p ){};
    ~ForeignData() override;
    FoliaElement *parseXml( const xmlNode * ) override;
    xmlNode *xml( bool, bool=false ) const override;
    void set_data( const xmlNode * );
    xmlNode* get_data() const;
  private:
    xmlNode *_foreign_data = NULL;
  public:
    static properties PROPS;
  };

} // namespace folia

#endif // FOLIA_SUBCLASSES_H
