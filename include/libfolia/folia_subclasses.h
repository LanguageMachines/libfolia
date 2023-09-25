/*
  Copyright (c) 2006 - 2023
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

#define ADD_PROTECTED_CONSTRUCTORS( CLASS, BASE )			\
  explicit CLASS( const properties& props, Document *d=0 ):		\
    BASE( props, d ){ classInit(); };					\
  CLASS( const properties& props, FoliaElement *p ):			\
    BASE( props, p ){ classInit(); }

#define ADD_PROTECTED_CONSTRUCTORS_INIT( CLASS, BASE, INIT )		\
  explicit CLASS( const properties& props, Document *d=0 ):		\
    BASE( props, d ), INIT { classInit(); };				\
  CLASS( const properties& props, FoliaElement *p ):			\
    BASE( props, p ), INIT { classInit(); }

#define ADD_DEFAULT_CONSTRUCTORS( CLASS, BASE )			  \
  protected:							  \
  ~CLASS() {};							  \
public:									\
 explicit CLASS( const KWargs& a, Document *d=0 ):			\
   BASE( PROPS, d ){ classInit(a); };					\
 explicit CLASS( Document *d=0 ):					\
   BASE( PROPS, d ){ classInit(); };					\
 CLASS( const KWargs& a, FoliaElement *p ):				\
   BASE( PROPS, p ){ classInit(a); };					\
 explicit CLASS( FoliaElement *p ):					\
   BASE( PROPS, p ){ classInit(); }

#define ADD_DEFAULT_CONSTRUCTORS_INIT( CLASS, BASE, INIT )	  \
 protected:							  \
   ~CLASS() {};							  \
 public:							  \
 explicit CLASS( const KWargs& a, Document *d=0 ):			\
   BASE( PROPS, d ), INIT { classInit(a); };				\
 explicit CLASS( Document *d=0 ):					\
   BASE( PROPS, d ), INIT { classInit(); };				\
 CLASS( const KWargs& a, FoliaElement *p ):				\
   BASE( PROPS, p ), INIT { classInit(a); };				\
 explicit CLASS( FoliaElement *p ):					\
   BASE( PROPS, p ), INIT { classInit(); }

  class AbstractStructureElement:
    public AbstractElement,
    public AllowGenerateID,
    public AllowInlineAnnotation
    {
      friend void static_init();
    protected:
      // DO NOT USE AbstractStructureElement as a real node!!
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
      static properties PROPS;
    };

  class AbstractWord: public virtual FoliaElement {
    /// Interface class that is inherited by word-like (wrefable)
    /// elements (Word, Hiddenword, Morpheme, Phoneme)
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
      friend void static_init();
    protected:
      // DO NOT USE AbstractInlineAnnotation as a real node!!
      ADD_PROTECTED_CONSTRUCTORS( AbstractInlineAnnotation, AbstractElement );
    public:
      static properties PROPS;
    };

  class AbstractHigherOrderAnnotation:
    public AbstractElement
    {
      friend void static_init();
    protected:
      // DO NOT USE AbstractHigherOrderAnnotation as a real node!!
      ADD_PROTECTED_CONSTRUCTORS( AbstractHigherOrderAnnotation, AbstractElement );
    public:
      static properties PROPS;
    };

  class AbstractSpanAnnotation:
    public AbstractElement,
    public AllowGenerateID,
    public AllowInlineAnnotation
    {
      friend void static_init();
    public:
      xmlNode *xml( bool, bool=false ) const override;
      FoliaElement *append( FoliaElement* ) override;

      std::vector<FoliaElement*> wrefs() const override;
      FoliaElement *wrefs( size_t ) const override;
    protected:
      // DO NOT USE AbstractSpanAnnotation as a real node!!
      ADD_PROTECTED_CONSTRUCTORS( AbstractSpanAnnotation, AbstractElement );
    public:
      static properties PROPS;
    };

  class SpanRelation: public AbstractElement {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( SpanRelation, AbstractElement );
  public:
    static properties PROPS;
  };

  const std::string EMPTY_STRING = "";

  class AbstractTextMarkup:
    public AbstractElement,
    public AllowXlink
    {
      friend void static_init();
    public:
      void setAttributes( KWargs& ) override;
      KWargs collectAttributes() const override;
      const FoliaElement* resolveid() const override;
    protected:
      // DO NOT USE AbstractTextMarkup as a real node!!
      ADD_PROTECTED_CONSTRUCTORS( AbstractTextMarkup, AbstractElement );
      const std::string& get_delimiter( const TextPolicy& ) const override {
	return EMPTY_STRING; };
      std::string idref;
    public:
      static properties PROPS;
    };

  class TextMarkupGap: public AbstractTextMarkup {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( TextMarkupGap, AbstractTextMarkup );
  public:
    static properties PROPS;
  };

  class TextMarkupString: public AbstractTextMarkup {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( TextMarkupString, AbstractTextMarkup );
  public:
    static properties PROPS;
  };

  class TextMarkupCorrection: public AbstractTextMarkup {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( TextMarkupCorrection, AbstractTextMarkup );
    void setAttributes( KWargs& ) override;
    KWargs collectAttributes() const override;
  public:
    static properties PROPS;
  private:
    const UnicodeString private_text( const TextPolicy& ) const override;
    std::string _original;
  };

  class TextMarkupError: public AbstractTextMarkup {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( TextMarkupError, AbstractTextMarkup );
  public:
    static properties PROPS;
  };

  class TextMarkupStyle: public AbstractTextMarkup {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( TextMarkupStyle, AbstractTextMarkup );
  public:
    static properties PROPS;
  };

  class Hyphbreak: public AbstractTextMarkup {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( Hyphbreak, AbstractTextMarkup );
  public:
    static properties PROPS;
  private:
    const UnicodeString private_text( const TextPolicy& ) const override;
  };

  class TextMarkupReference: public AbstractTextMarkup {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( TextMarkupReference, AbstractTextMarkup );
    KWargs collectAttributes() const override;
    void setAttributes( KWargs& ) override;

  private:
    void init() override;
    std::string ref_id;
    std::string ref_type;
    std::string _type;
    std::string _format;

  public:
    static properties PROPS;
  };

  class TextMarkupHSpace: public AbstractTextMarkup {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( TextMarkupHSpace, AbstractTextMarkup );
  private:
    const UnicodeString private_text( const TextPolicy& ) const override;
  public:
    static properties PROPS;
  };

  class TextMarkupWhitespace: public AbstractTextMarkup {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( TextMarkupWhitespace, AbstractTextMarkup );
  public:
    static properties PROPS;
  };

  class TextMarkupLanguage: public AbstractTextMarkup {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( TextMarkupLanguage, AbstractTextMarkup );
  public:
    static properties PROPS;
  };

  class AbstractContentAnnotation:
    public AbstractElement,
    public AllowGenerateID
  {
    friend void static_init();
  protected:
    // DO NOT USE AbstractContentAnnotation as a real node!!
    ADD_PROTECTED_CONSTRUCTORS_INIT( AbstractContentAnnotation, AbstractElement, _offset(0) );
  public:
    void setAttributes( KWargs& ) override;
    KWargs collectAttributes() const override;
    FoliaElement *get_reference( int&, bool=true ) const;
    int offset() const override { return _offset; };
    std::string ref() const { return _ref; };
  private:
    void init() override;
    virtual FoliaElement *find_default_reference() const = 0;
    void set_offset( int o ) const override { _offset = o; };
    mutable int _offset;
    std::string _ref;
  public:
    static properties PROPS;
  };

  class TextContent:
    public AbstractContentAnnotation,
    public AllowXlink
    {
      friend void static_init();
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
      FoliaElement *postappend() override;
    private:
      FoliaElement *find_default_reference() const override;
    public:
      static properties PROPS;
  };

  class PhonContent: public AbstractContentAnnotation {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( PhonContent,
			      AbstractContentAnnotation );
    void setAttributes( KWargs& ) override;
    KWargs collectAttributes() const override;
    const UnicodeString phon( const TextPolicy& ) const override;
    const UnicodeString phon( const std::string& = "current",
			      TEXT_FLAGS = TEXT_FLAGS::NONE ) const override;
    FoliaElement *postappend() override;
  public:
    FoliaElement *find_default_reference() const override;
  public:
    static properties PROPS;

  };

  class FoLiA: public AbstractElement {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( FoLiA, AbstractElement );
    FoliaElement* parseXml( const xmlNode * ) override;
    void setAttributes( KWargs& ) override;
  private:
    const UnicodeString private_text( const TextPolicy& ) const override;
  public:
    static properties PROPS;
  };

  class DCOI: public AbstractElement {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( DCOI, AbstractElement );
  public:
    static properties PROPS;
  };

  class Head: public AbstractStructureElement {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( Head, AbstractStructureElement );

  private:
    static properties PROPS;
  };

  class TableHead: public AbstractStructureElement {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( TableHead, AbstractStructureElement );
    static properties PROPS;
  };

  class Table: public AbstractStructureElement {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( Table, AbstractStructureElement );
    static properties PROPS;
  };

  class Row: public AbstractStructureElement {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( Row, AbstractStructureElement );
    const UnicodeString private_text( const TextPolicy& ) const override;
    static properties PROPS;
  };

  class Cell: public AbstractStructureElement {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( Cell, AbstractStructureElement );
    const UnicodeString private_text( const TextPolicy& ) const override;
    static properties PROPS;
  };

  class Gap: public AbstractElement {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( Gap, AbstractElement );
    const std::string content() const override;
    static properties PROPS;
  };

  class Content: public AbstractElement {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( Content, AbstractElement );

    FoliaElement* parseXml( const xmlNode * ) override;
    xmlNode *xml( bool, bool = false ) const override;
    const std::string content() const override { return value; };
    void setAttributes( KWargs& ) override;
  private:
    std::string value;
  public:
    static properties PROPS;
  };

  class Metric: public AbstractElement {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( Metric, AbstractElement );
    static properties PROPS;
  };

  class Division: public AbstractStructureElement {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( Division, AbstractStructureElement );
    Head *head() const;
    static properties PROPS;
  };

  class Linebreak:
    public AbstractStructureElement,
    public AllowXlink
    {
      friend void static_init();
    public:
      ADD_DEFAULT_CONSTRUCTORS_INIT( Linebreak, AbstractStructureElement, _newpage(false) );
      void setAttributes( KWargs& ) override;
      KWargs collectAttributes() const override;
    private:
      void init() override;
      const UnicodeString private_text( const TextPolicy& ) const override {
	return "\n";
      }
      std::string _pagenr;
      std::string _linenr;
      bool _newpage;
    public:
      static properties PROPS;
    };

  class Whitespace: public AbstractStructureElement {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( Whitespace, AbstractStructureElement );
  private:
    const UnicodeString private_text( const TextPolicy& ) const override {
      return "\n\n";
    }
  public:
    static properties PROPS;
  };

  class Word:
    public AbstractStructureElement,
    public AbstractWord
  {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( Word, AbstractStructureElement );

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
  protected:
    ADD_PROTECTED_CONSTRUCTORS( Word, AbstractStructureElement );
  public:
    static properties PROPS;
  };

  class Hiddenword:
    public AbstractStructureElement,
    public AbstractWord
  {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( Hiddenword, AbstractStructureElement );
    static properties PROPS;
  };

  class Part: public AbstractStructureElement {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( Part, AbstractStructureElement );
    static properties PROPS;
  };

  class String:
    public AbstractElement,
    public AllowInlineAnnotation,
    public AllowGenerateID
    {
      friend void static_init();
    public:
      ADD_DEFAULT_CONSTRUCTORS( String, AbstractElement );
      static properties PROPS;
    };

  class PlaceHolder: public Word {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( PlaceHolder, Word );
    void setAttributes( KWargs& ) override;
    static properties PROPS;
  };

  class Sentence:
    public AbstractStructureElement
    {
      friend void static_init();
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
      static properties PROPS;
    private:
      Correction *correctWords( const std::vector<FoliaElement *>&,
				const std::vector<FoliaElement *>&,
				const KWargs& );
    };

  class Speech: public AbstractStructureElement {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( Speech, AbstractStructureElement );
    static properties PROPS;
  };

  class Text: public AbstractStructureElement {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( Text, AbstractStructureElement );
    static properties PROPS;
  };

  class Utterance: public AbstractStructureElement {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( Utterance, AbstractStructureElement );
    static properties PROPS;
  };

  class Event: public AbstractStructureElement {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( Event, AbstractStructureElement );
    static properties PROPS;
  };

  class Caption: public AbstractStructureElement {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( Caption, AbstractStructureElement );
    static properties PROPS;
  };

  class Label: public AbstractStructureElement {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( Label, AbstractStructureElement );
    static properties PROPS;
  };

  class ListItem: public AbstractStructureElement {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( ListItem, AbstractStructureElement );
    static properties PROPS;
  };

  class List: public AbstractStructureElement {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( List, AbstractStructureElement );
    static properties PROPS;
  };

  class Figure: public AbstractStructureElement {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( Figure, AbstractStructureElement );
    const UnicodeString caption() const;
    static properties PROPS;
  };

  class Paragraph: public AbstractStructureElement {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( Paragraph, AbstractStructureElement );
    static properties PROPS;
  };

  class AbstractSubtokenAnnotation:
    public AbstractStructureElement
  {
    friend void static_init();
  protected:
    // DO NOT USE AbstractSubtokenAnnotation as a real node!!
    ADD_PROTECTED_CONSTRUCTORS( AbstractSubtokenAnnotation,
				     AbstractStructureElement );
  public:
    static properties PROPS;
  };

  class Alternative:
    public AbstractHigherOrderAnnotation,
    public AllowInlineAnnotation,
    public AllowGenerateID
    {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( Alternative, AbstractHigherOrderAnnotation );
    static properties PROPS;
  };


  class AlternativeLayers: public AbstractElement {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( AlternativeLayers, AbstractElement );
    static properties PROPS;
  };

  class PosAnnotation: public AbstractInlineAnnotation {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( PosAnnotation, AbstractInlineAnnotation );
    static properties PROPS;
  };

  class LemmaAnnotation: public AbstractInlineAnnotation {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( LemmaAnnotation, AbstractInlineAnnotation );
    static properties PROPS;
  };

  class LangAnnotation: public AbstractInlineAnnotation {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( LangAnnotation, AbstractInlineAnnotation );
    static properties PROPS;
  };

  class Phoneme:
    public AbstractSubtokenAnnotation,
    public AbstractWord
    {
      friend void static_init();
    public:
      ADD_DEFAULT_CONSTRUCTORS( Phoneme, AbstractSubtokenAnnotation );
      static properties PROPS;
    };

  class DomainAnnotation: public AbstractInlineAnnotation {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( DomainAnnotation, AbstractInlineAnnotation );
    static properties PROPS;
  };

  class SenseAnnotation: public AbstractInlineAnnotation {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( SenseAnnotation, AbstractInlineAnnotation );
    static properties PROPS;
  };

  class EtymologyAnnotation: public AbstractInlineAnnotation {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( EtymologyAnnotation,
			      AbstractInlineAnnotation );
    static properties PROPS;
  };

  class SubjectivityAnnotation: public AbstractInlineAnnotation {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( SubjectivityAnnotation,
				  AbstractInlineAnnotation );
    static properties PROPS;
  };

  class Quote: public AbstractStructureElement {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( Quote, AbstractStructureElement );
    std::vector<Word*> wordParts() const override;
    const std::string& get_delimiter( const TextPolicy& ) const override;
    static properties PROPS;
  };

  class Feature: public AbstractElement {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( Feature, AbstractElement );
    void setAttributes( KWargs& ) override;
    KWargs collectAttributes() const override;
    const std::string subset() const override { return _subset; };

  protected:
    ADD_PROTECTED_CONSTRUCTORS( Feature, AbstractElement );
  public:
    std::string _subset;
    static properties PROPS;
  };

  class BegindatetimeFeature: public Feature {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( BegindatetimeFeature, Feature );
    static properties PROPS;
  };

  class EnddatetimeFeature: public Feature {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( EnddatetimeFeature, Feature );
    static properties PROPS;
  };

  class SynsetFeature: public Feature {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( SynsetFeature, Feature );
    static properties PROPS;
  };

  class ActorFeature: public Feature {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( ActorFeature, Feature );
    static properties PROPS;
  };

  class PolarityFeature: public Feature {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( PolarityFeature, Feature );
    static properties PROPS;
  };

  class StrengthFeature: public Feature {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( StrengthFeature, Feature );
    static properties PROPS;
  };


  class HeadFeature: public Feature {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( HeadFeature, Feature );
    static properties PROPS;
  };

  class ValueFeature: public Feature {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( ValueFeature, Feature );
    static properties PROPS;
  };

  class FunctionFeature: public Feature {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( FunctionFeature, Feature );
    static properties PROPS;
  };

  class TimeFeature: public Feature {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( TimeFeature, Feature );
    static properties PROPS;
  };

  class LevelFeature: public Feature {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( LevelFeature, Feature );
    static properties PROPS;
  };

  class ModalityFeature: public Feature {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( ModalityFeature, Feature );
    static properties PROPS;
  };

  class StyleFeature: public Feature {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( StyleFeature, Feature );
    static properties PROPS;
  };

  class FontFeature: public Feature {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( FontFeature, Feature );
    static properties PROPS;
  };

  class SizeFeature: public Feature {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( SizeFeature, Feature );
    static properties PROPS;
  };

  class WordReference: public AbstractElement {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( WordReference, AbstractElement );
    static properties PROPS;
  private:
    FoliaElement* parseXml( const xmlNode *node ) override;
  };

  class Relation:
    public AbstractHigherOrderAnnotation,
    public AllowXlink {
      friend void static_init();
  public:
      ADD_DEFAULT_CONSTRUCTORS( Relation, AbstractHigherOrderAnnotation );
      std::vector<FoliaElement *>resolve() const override;
      void setAttributes( KWargs& ) override;
      KWargs collectAttributes() const override;
      static properties PROPS;
  private:
      void init() override;
      std::string _format;
    };

  class LinkReference: public AbstractElement {
    friend void static_init();
    friend class Relation;
  public:
    ADD_DEFAULT_CONSTRUCTORS( LinkReference, AbstractElement );

    KWargs collectAttributes() const override;
    void setAttributes( KWargs& ) override;
    const std::string refid() const { return ref_id; };
    const std::string type() const { return ref_type; };
    const std::string t() const { return _t; };
    static properties PROPS;

  private:
    FoliaElement* parseXml( const xmlNode *node ) override;
    FoliaElement *resolve_element( const Relation *ref ) const;
    std::string ref_id;
    std::string ref_type;
    std::string _t;
  };

  class SyntacticUnit: public AbstractSpanAnnotation {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( SyntacticUnit, AbstractSpanAnnotation );
    static properties PROPS;
  };

  class Chunk: public AbstractSpanAnnotation {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( Chunk, AbstractSpanAnnotation );
    static properties PROPS;
  };

  class Entity: public AbstractSpanAnnotation {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( Entity, AbstractSpanAnnotation );
    static properties PROPS;
  };

  class AbstractSpanRole: public AbstractSpanAnnotation {
    friend void static_init();
  protected:
    // DO NOT USE AbstractSpanRole as a real node!!
    ADD_PROTECTED_CONSTRUCTORS( AbstractSpanRole, AbstractSpanAnnotation );
    static properties PROPS;
  };

  class Headspan: public AbstractSpanRole {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( Headspan, AbstractSpanRole );
    static properties PROPS;
  };

  class DependencyDependent: public AbstractSpanRole {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( DependencyDependent, AbstractSpanRole );
    static properties PROPS;
  };

  class Source: public AbstractSpanRole {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( Source, AbstractSpanRole );
    static properties PROPS;
  };

  class Target: public AbstractSpanRole {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( Target, AbstractSpanRole );
    static properties PROPS;
  };

  class Scope: public AbstractSpanRole {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( Scope, AbstractSpanRole );
    static properties PROPS;
  };

  class Cue: public AbstractSpanRole {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( Cue, AbstractSpanRole );
    static properties PROPS;
  };

  class StatementRelation: public AbstractSpanRole {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( StatementRelation, AbstractSpanRole );
    static properties PROPS;
  };

  class Dependency: public AbstractSpanAnnotation {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( Dependency, AbstractSpanAnnotation );
    Headspan *head() const;
    DependencyDependent *dependent() const;
    static properties PROPS;
  };

  class CoreferenceLink: public AbstractSpanRole {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( CoreferenceLink, AbstractSpanRole );
    static properties PROPS;
  };

  class CoreferenceChain: public AbstractSpanAnnotation {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( CoreferenceChain, AbstractSpanAnnotation );
    static properties PROPS;
  };

  class SemanticRole: public AbstractSpanAnnotation {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( SemanticRole, AbstractSpanAnnotation );
    static properties PROPS;
  };

  class Predicate: public AbstractSpanAnnotation {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( Predicate, AbstractSpanAnnotation );
    static properties PROPS;
  };

  class Sentiment: public AbstractSpanAnnotation {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( Sentiment, AbstractSpanAnnotation );
    static properties PROPS;
  };

  class Modality: public AbstractSpanAnnotation {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( Modality, AbstractSpanAnnotation );
    static properties PROPS;
  };

  class Statement: public AbstractSpanAnnotation {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( Statement, AbstractSpanAnnotation );
    static properties PROPS;
  };

  class Observation: public AbstractSpanAnnotation {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( Observation, AbstractSpanAnnotation );
    static properties PROPS;
  };


  class AbstractAnnotationLayer:
    public AbstractElement,
    public AllowGenerateID,
    public AllowInlineAnnotation
    {
      friend void static_init();
    public:
      AbstractSpanAnnotation *findspan( const std::vector<FoliaElement*>& ) const override;
      FoliaElement *append( FoliaElement * ) override;
      KWargs collectAttributes() const override;
      static properties PROPS;
    protected:
      // DO NOT USE AbstractAnnotationLayer as a real node!!
      ADD_PROTECTED_CONSTRUCTORS( AbstractAnnotationLayer, AbstractElement );
    private:
      void assignset( FoliaElement * );
    };

  class AbstractCorrectionChild: public AbstractElement {
    friend void static_init();
  protected:
    // DO NOT USE AbstractCorrectionChild as a real node!!
    ADD_PROTECTED_CONSTRUCTORS( AbstractCorrectionChild, AbstractElement );
    static properties PROPS;
  };

  class New: public AbstractCorrectionChild {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( New, AbstractCorrectionChild );

    Correction *correct( const std::vector<FoliaElement*>&,
			 const std::vector<FoliaElement*>&,
			 const std::vector<FoliaElement*>&,
			 const std::vector<FoliaElement*>&,
			 const KWargs& ) override;
    Correction *correct( const std::string& = "" ) override;
    bool addable( const FoliaElement * ) const override;
    static properties PROPS;
  };

  class Current: public AbstractCorrectionChild {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( Current, AbstractCorrectionChild );
    static properties PROPS;
    bool addable( const FoliaElement * ) const override;
  };

  class Original: public AbstractCorrectionChild {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( Original, AbstractCorrectionChild );
    static properties PROPS;
    bool addable( const FoliaElement * ) const override;
  };

  class Suggestion: public AbstractCorrectionChild {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( Suggestion, AbstractCorrectionChild );
    void setAttributes( KWargs& ) override;
    KWargs collectAttributes() const override;
    static properties PROPS;
  private:
    std::string _split;
    std::string _merge;
  };

  class Description: public AbstractElement {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( Description, AbstractElement );

    const std::string description() const override { return _value; };
    void setAttributes( KWargs& ) override;
    FoliaElement* parseXml( const xmlNode * ) override;
    xmlNode *xml( bool, bool=false ) const override;
    void setvalue( const std::string& s ){ _value = s; };
    static properties PROPS;
  private:
    std::string _value;
  };

  class Comment: public AbstractElement {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( Comment, AbstractElement );

    const std::string comment() const { return _value; };
    void setAttributes( KWargs& ) override;
    FoliaElement* parseXml( const xmlNode * ) override;
    xmlNode *xml( bool, bool=false ) const override;
    void setvalue( const std::string& s ){ _value = s; };
    static properties PROPS;
  private:
    std::string _value;
  };

  class XmlComment: public AbstractElement {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( XmlComment, AbstractElement );
    FoliaElement* parseXml( const xmlNode * ) override;
    xmlNode *xml( bool, bool=false ) const override;
    void setvalue( const std::string& s ){ _value = s; };
    static properties PROPS;
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
    FoliaElement* parseXml( const xmlNode * ) override;
    xmlNode *xml( bool, bool=false ) const override;
    static properties PROPS;
    const std::string target() const { return _target; };
    const std::string content() const override { return _content; };
  private:
    const UnicodeString private_text( const TextPolicy& ) const override {
      return "";
    }
    std::string _target;
    std::string _content;
  };

  class XmlText: public AbstractElement {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( XmlText, AbstractElement );
    FoliaElement* parseXml( const xmlNode * ) override;
    xmlNode *xml( bool, bool=false ) const override;
    void setvalue( const std::string& );
    void setuvalue( const UnicodeString& );
    const std::string& get_delimiter( const TextPolicy& ) const override {
      return EMPTY_STRING; };
    void setAttributes( KWargs& ) override;
    static properties PROPS;
  private:
    const UnicodeString private_text( const TextPolicy& ) const override;
    std::string _value; //UTF8 value
  };

  class External: public AbstractElement {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( External, AbstractElement );

    FoliaElement* parseXml( const xmlNode * ) override;
    void resolve_external();
    static properties PROPS;
  };

  class Note: public AbstractStructureElement {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( Note, AbstractStructureElement );
    void setAttributes( KWargs& ) override;
    static properties PROPS;
  private:
    std::string ref_id;
  };

  class Definition: public AbstractStructureElement {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( Definition, AbstractStructureElement );
    static properties PROPS;
  };

  class Term: public AbstractStructureElement {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( Term, AbstractStructureElement );
    static properties PROPS;
  };

  class Example: public AbstractStructureElement {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( Example, AbstractStructureElement );
    static properties PROPS;
  };

  class Entry: public AbstractStructureElement {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( Entry, AbstractStructureElement );
    static properties PROPS;
  };

  class Reference:
    public AbstractStructureElement,
    public AllowXlink
    {
      friend void static_init();
      friend class Note;
    public:
      ADD_DEFAULT_CONSTRUCTORS( Reference, AbstractStructureElement );

      KWargs collectAttributes() const override;
      void setAttributes( KWargs& ) override;
      static properties PROPS;
    private:
      void init() override;
      std::string ref_id;
      std::string ref_type;
      std::string _format;
    };


  class Correction: public AbstractInlineAnnotation {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( Correction, AbstractInlineAnnotation );
    bool hasNew() const;
    bool hasOriginal() const;
    bool hasCurrent() const;
    bool hasSuggestions() const;
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
    static properties PROPS;
  private:
    const UnicodeString private_text( const TextPolicy& ) const override;
  };

  class ErrorDetection: public AbstractInlineAnnotation  {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( ErrorDetection, AbstractInlineAnnotation );
    static properties PROPS;
  };

  class TimeSegment: public AbstractSpanAnnotation {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( TimeSegment, AbstractSpanAnnotation );
    static properties PROPS;
  };

  class Morpheme:
    public AbstractSubtokenAnnotation,
    public AbstractWord
    {
      friend void static_init();
    public:
      ADD_DEFAULT_CONSTRUCTORS( Morpheme, AbstractSubtokenAnnotation );
      static properties PROPS;
    };

  class SyntaxLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( SyntaxLayer, AbstractAnnotationLayer );
    static properties PROPS;
  };

  class ChunkingLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( ChunkingLayer, AbstractAnnotationLayer );
    static properties PROPS;
  };

  class EntitiesLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( EntitiesLayer, AbstractAnnotationLayer );
    static properties PROPS;
  };

  class TimingLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( TimingLayer, AbstractAnnotationLayer );
    static properties PROPS;
  };

  class MorphologyLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( MorphologyLayer, AbstractAnnotationLayer );
    static properties PROPS;
  };

  class PhonologyLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( PhonologyLayer, AbstractAnnotationLayer );
    static properties PROPS;
  };

  class DependenciesLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( DependenciesLayer, AbstractAnnotationLayer );
    static properties PROPS;
  };

  class CoreferenceLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( CoreferenceLayer, AbstractAnnotationLayer );
    static properties PROPS;
  };

  class SemanticRolesLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( SemanticRolesLayer, AbstractAnnotationLayer );
    static properties PROPS;
  };

  class StatementLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( StatementLayer, AbstractAnnotationLayer );
    static properties PROPS;
  };

  class SentimentLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( SentimentLayer, AbstractAnnotationLayer );
    static properties PROPS;
  };

  class ModalitiesLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( ModalitiesLayer, AbstractAnnotationLayer );
    static properties PROPS;
  };

  class ObservationLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( ObservationLayer, AbstractAnnotationLayer );
    static properties PROPS;
  };

  class SpanRelationLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
    ADD_DEFAULT_CONSTRUCTORS( SpanRelationLayer, AbstractAnnotationLayer );
    static properties PROPS;
  };

  ///
  /// ForeignData is special. It implements it's own destructor
  ///
  class ForeignData: public AbstractElement {
    friend void static_init();
  public:
    explicit ForeignData( Document *d=0 ):
      AbstractElement( PROPS, d ),
      _foreign_data(0)
    {
      classInit();
    };
    explicit ForeignData( FoliaElement *p ):
      AbstractElement( PROPS, p ),
      _foreign_data(0)
    {
      classInit();
    }
    ~ForeignData();
    FoliaElement* parseXml( const xmlNode * ) override;
    xmlNode *xml( bool, bool=false ) const override;
    void set_data( const xmlNode * );
    xmlNode* get_data() const;
  private:
    void init() override;
    xmlNode *_foreign_data;
  public:
    static properties PROPS;
  };

} // namespace folia

#endif // FOLIA_SUBCLASSES_H
