/*
  Copyright (c) 2006 - 2021
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
  CLASS( const properties& props, Document *d=0 ):		\
    BASE( props, d ){ classInit(); };				\
  CLASS( const properties& props, FoliaElement *p ):		\
    BASE( props, p ){ classInit(); }

#define ADD_PUBLIC_CONSTRUCTORS( CLASS, BASE )			  \
  CLASS( const KWargs& a, Document *d=0 ):			  \
    BASE( PROPS, d ){ classInit(a); };				  \
  CLASS( Document *d=0 ):					  \
    BASE( PROPS, d ){ classInit(); };				  \
  CLASS( const KWargs& a, FoliaElement *p ):			  \
    BASE( PROPS, p ){ classInit(a); };				  \
  CLASS( FoliaElement *p ):					  \
    BASE( PROPS, p ){ classInit(); }


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
      FoliaElement *append( FoliaElement* );
      std::vector<Paragraph*> paragraphs() const;
      std::vector<Sentence*> sentences() const;
      std::vector<Word*> words( const std::string& ="" ) const;
      Sentence *sentences( size_t ) const;
      Sentence *rsentences( size_t ) const;
      Paragraph *paragraphs( size_t ) const;
      Paragraph *rparagraphs( size_t ) const;
      Word *words( size_t, const std::string& ="" ) const;
      Word *rwords( size_t, const std::string& ="" ) const;
      const Word* resolveword( const std::string& ) const;
    private:
      static properties PROPS;
    };

  class AbstractWord: public virtual FoliaElement {
    /// Interface class that is inherited by word-like (wrefable)
    /// elements (Word, Hiddenword, Morpheme, Phoneme)
  public:
    Sentence *sentence() const;
    Paragraph *paragraph() const;
    Division *division() const;
    const std::string pos( const std::string& = "" ) const;
    const std::string lemma( const std::string& = "" ) const;
    SenseAnnotation *sense( const std::string& = "" ) const;
    DomainAnnotation *domain( const std::string& = "" ) const;
    std::vector<Morpheme *> morphemes( const std::string& ="" ) const;
    std::vector<Phoneme *> phonemes( const std::string& ="" ) const;
    Morpheme *morpheme( size_t, const std::string& ="" ) const;
    Phoneme *phoneme( size_t, const std::string& ="" ) const;
    std::vector<Correction *> getcorrections( const std::string& ="",
					      const std::string& ="" ) const NOT_IMPLEMENTED;
    Correction *getcorrection( const std::string& ="",
			       const std::string& ="" ) const NOT_IMPLEMENTED;
    std::vector<AbstractSpanAnnotation*> findspans( ElementType,
						    const std::string& ="" ) const;
  };

  class AbstractInlineAnnotation:
    public AbstractElement,
    public AllowGenerateID
    {
      friend void static_init();
    protected:
      // DO NOT USE AbstractInlineAnnotation as a real node!!
      ADD_PROTECTED_CONSTRUCTORS( AbstractInlineAnnotation, AbstractElement );
    private:
      static properties PROPS;
    };

  class AbstractHigherOrderAnnotation:
    public AbstractElement
    {
      friend void static_init();
    protected:
      // DO NOT USE AbstractHigherOrderAnnotation as a real node!!
      ADD_PROTECTED_CONSTRUCTORS( AbstractHigherOrderAnnotation, AbstractElement );
    private:
      static properties PROPS;
    };

  class AbstractSpanAnnotation:
    public AbstractElement,
    public AllowGenerateID,
    public AllowInlineAnnotation
    {
      friend void static_init();
    public:
      xmlNode *xml( bool, bool=false ) const;
      FoliaElement *append( FoliaElement* );

      std::vector<FoliaElement*> wrefs() const;
      FoliaElement *wrefs( size_t ) const;
    protected:
      // DO NOT USE AbstractSpanAnnotation as a real node!!
      ADD_PROTECTED_CONSTRUCTORS( AbstractSpanAnnotation, AbstractElement );
    private:
      static properties PROPS;
    };

  class SpanRelation: public AbstractElement {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( SpanRelation, AbstractElement );
  private:
    static properties PROPS;
  };

  class ForeignData: public AbstractElement {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( ForeignData, AbstractElement );
    ~ForeignData();

    FoliaElement* parseXml( const xmlNode * );
    xmlNode *xml( bool, bool=false ) const;
    void set_data( const xmlNode * );
    xmlNode* get_data() const;
  private:
  ForeignData( const properties& props, Document *d=0 ):
    AbstractElement( props, d ){ classInit(); };
    void init();
    static properties PROPS;
    xmlNode *_foreign_data;
  };

  const std::string EMPTY_STRING = "";

  class AbstractTextMarkup:
    public AbstractElement,
    public AllowXlink
    {
      friend void static_init();
    public:
      void setAttributes( KWargs& );
      KWargs collectAttributes() const;
      const FoliaElement* resolveid() const;
    protected:
      // DO NOT USE AbstractTextMarkup as a real node!!
      ADD_PROTECTED_CONSTRUCTORS( AbstractTextMarkup, AbstractElement );
      const std::string& get_delimiter( bool ) const { return EMPTY_STRING; };
      std::string idref;
    private:
      static properties PROPS;
    };

  class TextMarkupGap: public AbstractTextMarkup {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( TextMarkupGap, AbstractTextMarkup );
  private:
    static properties PROPS;
  };

  class TextMarkupString: public AbstractTextMarkup {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( TextMarkupString, AbstractTextMarkup );
  private:
    static properties PROPS;
  };

  class TextMarkupCorrection: public AbstractTextMarkup {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( TextMarkupCorrection, AbstractTextMarkup );
    void setAttributes( KWargs& );
    KWargs collectAttributes() const;
  private:
    const UnicodeString private_text( const TextPolicy& ) const;
    static properties PROPS;
    std::string _original;
  };

  class TextMarkupError: public AbstractTextMarkup {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( TextMarkupError, AbstractTextMarkup );
  private:
    static properties PROPS;
  };

  class TextMarkupStyle: public AbstractTextMarkup {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( TextMarkupStyle, AbstractTextMarkup );
  private:
    static properties PROPS;
  };

  class Hyphbreak: public AbstractTextMarkup {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( Hyphbreak, AbstractTextMarkup );
  private:
    static properties PROPS;
  };

  class TextMarkupReference: public AbstractTextMarkup {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( TextMarkupReference, AbstractTextMarkup );
    KWargs collectAttributes() const;
    void setAttributes( KWargs& );

  private:
    void init();
    static properties PROPS;
    std::string ref_id;
    std::string ref_type;
    std::string _type;
    std::string _format;
  };

  class TextMarkupHSpace: public AbstractTextMarkup {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( TextMarkupHSpace, AbstractTextMarkup );
  private:
    const UnicodeString private_text( const TextPolicy& ) const;
    static properties PROPS;
  };

  class TextMarkupWhitespace: public AbstractTextMarkup {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( TextMarkupWhitespace, AbstractTextMarkup );
  private:
    static properties PROPS;
  };

  class TextMarkupLanguage: public AbstractTextMarkup {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( TextMarkupLanguage, AbstractTextMarkup );
  private:
    static properties PROPS;
  };


  class AbstractContentAnnotation: public AbstractElement {
    friend void static_init();
  protected:
    // DO NOT USE AbstractContentAnnotation as a real node!!
    ADD_PROTECTED_CONSTRUCTORS( AbstractContentAnnotation, AbstractElement );
  private:
    static properties PROPS;
  };


  class TextContent:
    public AbstractContentAnnotation,
    public AllowXlink
    {
      friend void static_init();
    public:
      ADD_PUBLIC_CONSTRUCTORS( TextContent,
				    AbstractContentAnnotation );
      void setAttributes( KWargs& );
      KWargs collectAttributes() const;
      int offset() const { return _offset; };
      std::vector<FoliaElement*> find_replacables( FoliaElement * ) const;
      const std::string set_to_current() { // Don't use without thinking twice!
	std::string res = cls();
	set_cls( "current" );
	return res;
      }
      FoliaElement *postappend();
      FoliaElement *get_reference(bool trim_spaces=true) const;
      std::string ref() const { return _ref; };
    private:
      void init();
      FoliaElement *find_default_reference() const;
      void set_offset( int o ) const { _offset = o; }; // this MUST be const,
      // only used for 'fixing up' invalid offsets. keep it private!
      // therefore _offset  has to be mutable!
      static properties PROPS;
      mutable int _offset;
      std::string _ref;
    };

  class PhonContent: public AbstractContentAnnotation {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( PhonContent,
				  AbstractContentAnnotation );
    void setAttributes( KWargs& );
    KWargs collectAttributes() const;
    const UnicodeString phon( const TextPolicy& ) const;
    const UnicodeString phon( const std::string& = "current",
			      TEXT_FLAGS = TEXT_FLAGS::NONE ) const;
    int offset() const { return _offset; };
    FoliaElement *postappend();
    FoliaElement *get_reference(bool trim_spaces=true) const;
    std::string ref() const { return _ref; };
  private:
    void init();
    FoliaElement *find_default_reference() const;
    void set_offset( int o ) const { _offset = o; }; // this MUST be const,
    // only used for 'fixing up' invalid offsets. keep it private!
    // therefore _offset  has to be mutable!
    static properties PROPS;
    mutable int _offset;
    std::string _ref;
  };

  class FoLiA: public AbstractElement {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( FoLiA, AbstractElement );
    FoliaElement* parseXml( const xmlNode * );
    void setAttributes( KWargs& );
  private:
    const UnicodeString private_text( const TextPolicy& ) const;
    static properties PROPS;
  };

  class DCOI: public AbstractElement {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( DCOI, AbstractElement );
  private:
    static properties PROPS;
  };

  class Head: public AbstractStructureElement {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( Head, AbstractStructureElement );

  private:
    static properties PROPS;
  };

  class TableHead: public AbstractStructureElement {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( TableHead, AbstractStructureElement );
  private:
    static properties PROPS;
  };

  class Table: public AbstractStructureElement {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( Table, AbstractStructureElement );
  private:
    static properties PROPS;
  };

  class Row: public AbstractStructureElement {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( Row, AbstractStructureElement );
  private:
    static properties PROPS;
  };

  class Cell: public AbstractStructureElement {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( Cell, AbstractStructureElement );
  private:
    static properties PROPS;
  };

  class Gap: public AbstractElement {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( Gap, AbstractElement );

    const std::string content() const;
  private:
    static properties PROPS;
  };

  class Content: public AbstractElement {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( Content, AbstractElement );

    FoliaElement* parseXml( const xmlNode * );
    xmlNode *xml( bool, bool = false ) const;
    const std::string content() const { return value; };
    void setAttributes( KWargs& );
  private:
    static properties PROPS;
    std::string value;
  };

  class Metric: public AbstractElement {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( Metric, AbstractElement );

  private:
    static properties PROPS;
  };

  class Division: public AbstractStructureElement {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( Division, AbstractStructureElement );
    Head *head() const;
  private:
    static properties PROPS;
  };

  class Linebreak:
    public AbstractStructureElement,
    public AllowXlink
    {
      friend void static_init();
    public:
      ADD_PUBLIC_CONSTRUCTORS( Linebreak, AbstractStructureElement );
      void setAttributes( KWargs& );
      KWargs collectAttributes() const;
    private:
      void init();
      const UnicodeString private_text( const TextPolicy& ) const {
	return "\n";
      }
      static properties PROPS;
      std::string _pagenr;
      std::string _linenr;
      bool _newpage;
    };

  class Whitespace: public AbstractStructureElement {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( Whitespace, AbstractStructureElement );
  private:
    const UnicodeString private_text( const TextPolicy& ) const {
      return "\n\n";
    }
    static properties PROPS;
  };

  class Word:
    public AbstractStructureElement,
    public AbstractWord
  {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( Word, AbstractStructureElement );

    Correction *split( FoliaElement *, FoliaElement *,
		       const std::string& = "" );
    Correction *incorrection() const;
    Word *previous() const;
    Word *next() const;
    std::vector<Word*> context( size_t,
				const std::string& ="" ) const;
    std::vector<Word*> leftcontext( size_t,
				    const std::string& = "" ) const;
    std::vector<Word*> rightcontext( size_t,
				     const std::string& ="" ) const;
    FoliaElement *append( FoliaElement * );
    const Word* resolveword( const std::string& ) const;
    void setAttributes( KWargs& );
    const std::string& get_delimiter( bool=false) const;
    MorphologyLayer *addMorphologyLayer( const KWargs& );
    MorphologyLayer *getMorphologyLayers( const std::string&,
					  std::vector<MorphologyLayer*>& ) const;
  protected:
    ADD_PROTECTED_CONSTRUCTORS( Word, AbstractStructureElement );
  private:
    static properties PROPS;
  };

  class Hiddenword:
    public AbstractStructureElement,
    public AbstractWord
  {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( Hiddenword, AbstractStructureElement );
  private:
    static properties PROPS;
  };

  class Part: public AbstractStructureElement {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( Part, AbstractStructureElement );

  private:
    static properties PROPS;
  };

  class String:
    public AbstractElement,
    public AllowInlineAnnotation,
    public AllowGenerateID
    {
      friend void static_init();
    public:
      ADD_PUBLIC_CONSTRUCTORS( String, AbstractElement );
    private:
      static properties PROPS;
    };

  class PlaceHolder: public Word {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( PlaceHolder, Word );
    void setAttributes( KWargs& );
  private:
    static properties PROPS;
  };

  class Sentence:
    public AbstractStructureElement
    {
      friend void static_init();
    public:
      ADD_PUBLIC_CONSTRUCTORS( Sentence, AbstractStructureElement );

      Correction *splitWord( FoliaElement *, FoliaElement *,
			     FoliaElement *, const KWargs& );
      Correction *mergewords( FoliaElement *,
			      const std::vector<FoliaElement *>&,
			      const std::string& = "" );
      Correction *deleteword( FoliaElement *, const std::string& args );
      Correction *insertword( FoliaElement *, FoliaElement *,
			      const std::string& args );
      std::vector<Word*> wordParts() const;
    private:
      static properties PROPS;
      Correction *correctWords( const std::vector<FoliaElement *>&,
				const std::vector<FoliaElement *>&,
				const KWargs& );
    };

  class Speech: public AbstractStructureElement {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( Speech, AbstractStructureElement );

  private:
    static properties PROPS;
  };

  class Text: public AbstractStructureElement {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( Text, AbstractStructureElement );
  private:
    static properties PROPS;
  };

  class Utterance: public AbstractStructureElement {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( Utterance, AbstractStructureElement );

  private:
    static properties PROPS;
  };

  class Event: public AbstractStructureElement {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( Event, AbstractStructureElement );
  private:
    static properties PROPS;
  };

  class Caption: public AbstractStructureElement {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( Caption, AbstractStructureElement );
  private:
    static properties PROPS;
  };

  class Label: public AbstractStructureElement {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( Label, AbstractStructureElement );
  private:
    static properties PROPS;
  };

  class ListItem: public AbstractStructureElement {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( ListItem, AbstractStructureElement );
  private:
    static properties PROPS;
  };

  class List: public AbstractStructureElement {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( List, AbstractStructureElement );
  private:
    static properties PROPS;
  };

  class Figure: public AbstractStructureElement {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( Figure, AbstractStructureElement );
    const UnicodeString caption() const;
  private:
    static properties PROPS;
  };

  class Paragraph: public AbstractStructureElement {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( Paragraph, AbstractStructureElement );
  private:
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
  private:
    static properties PROPS;
  };

  class Alternative:
    public AbstractHigherOrderAnnotation,
    public AllowInlineAnnotation,
    public AllowGenerateID
    {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( Alternative, AbstractHigherOrderAnnotation );
  private:
    static properties PROPS;
  };


  class AlternativeLayers: public AbstractElement {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( AlternativeLayers, AbstractElement );

  private:
    static properties PROPS;
  };

  class PosAnnotation: public AbstractInlineAnnotation {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( PosAnnotation, AbstractInlineAnnotation );
  private:
    static properties PROPS;
  };

  class LemmaAnnotation: public AbstractInlineAnnotation {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( LemmaAnnotation, AbstractInlineAnnotation );
  private:
    static properties PROPS;
  };

  class LangAnnotation: public AbstractInlineAnnotation {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( LangAnnotation, AbstractInlineAnnotation );
  private:
    static properties PROPS;
  };

  class Phoneme:
    public AbstractSubtokenAnnotation,
    public AbstractWord
    {
      friend void static_init();
    public:
      ADD_PUBLIC_CONSTRUCTORS( Phoneme, AbstractSubtokenAnnotation );
    private:
      static properties PROPS;
    };

  class DomainAnnotation: public AbstractInlineAnnotation {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( DomainAnnotation, AbstractInlineAnnotation );
  private:
    static properties PROPS;
  };

  class SenseAnnotation: public AbstractInlineAnnotation {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( SenseAnnotation, AbstractInlineAnnotation );
  private:
    static properties PROPS;
  };

  class SubjectivityAnnotation: public AbstractInlineAnnotation {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( SubjectivityAnnotation,
				  AbstractInlineAnnotation );
  private:
    static properties PROPS;
  };

  class Quote: public AbstractStructureElement {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( Quote, AbstractStructureElement );
    std::vector<Word*> wordParts() const;
    const std::string& get_delimiter( bool=false) const;
  private:
    static properties PROPS;
  };

  class Feature: public AbstractElement {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( Feature, AbstractElement );
    void setAttributes( KWargs& );
    KWargs collectAttributes() const;
    const std::string subset() const { return _subset; };

  protected:
    ADD_PROTECTED_CONSTRUCTORS( Feature, AbstractElement );
  private:
    std::string _subset;
    static properties PROPS;
  };

  class BegindatetimeFeature: public Feature {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( BegindatetimeFeature, Feature );
  private:
    static properties PROPS;
  };

  class EnddatetimeFeature: public Feature {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( EnddatetimeFeature, Feature );
  private:
    static properties PROPS;
  };

  class SynsetFeature: public Feature {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( SynsetFeature, Feature );
  private:
    static properties PROPS;
  };

  class ActorFeature: public Feature {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( ActorFeature, Feature );
  private:
    static properties PROPS;
  };

  class PolarityFeature: public Feature {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( PolarityFeature, Feature );
  private:
    static properties PROPS;
  };

  class StrengthFeature: public Feature {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( StrengthFeature, Feature );
  private:
    static properties PROPS;
  };


  class HeadFeature: public Feature {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( HeadFeature, Feature );
  private:
    static properties PROPS;
  };

  class ValueFeature: public Feature {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( ValueFeature, Feature );
  private:
    static properties PROPS;
  };

  class FunctionFeature: public Feature {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( FunctionFeature, Feature );
  private:
    static properties PROPS;
  };

  class TimeFeature: public Feature {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( TimeFeature, Feature );
  private:
    static properties PROPS;
  };

  class LevelFeature: public Feature {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( LevelFeature, Feature );
  private:
    static properties PROPS;
  };

  class ModalityFeature: public Feature {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( ModalityFeature, Feature );
  private:
    static properties PROPS;
  };

  class StyleFeature: public Feature {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( StyleFeature, Feature );
  private:
    static properties PROPS;
  };

  class FontFeature: public Feature {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( FontFeature, Feature );
  private:
    static properties PROPS;
  };

  class SizeFeature: public Feature {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( SizeFeature, Feature );
  private:
    static properties PROPS;
  };

  class WordReference: public AbstractElement {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( WordReference, AbstractElement );
  private:
    static properties PROPS;
    FoliaElement* parseXml( const xmlNode *node );
  };

  class Relation:
    public AbstractHigherOrderAnnotation,
    public AllowXlink {
      friend void static_init();
  public:
      ADD_PUBLIC_CONSTRUCTORS( Relation, AbstractHigherOrderAnnotation );
      std::vector<FoliaElement *>resolve() const;
      void setAttributes( KWargs& );
      KWargs collectAttributes() const;
  private:
      void init();
      static properties PROPS;
      std::string _format;
    };

  class LinkReference: public AbstractElement {
    friend void static_init();
    friend class Relation;
  public:
    ADD_PUBLIC_CONSTRUCTORS( LinkReference, AbstractElement );

    KWargs collectAttributes() const;
    void setAttributes( KWargs& );
    const std::string refid() const { return ref_id; };
    const std::string type() const { return ref_type; };
    const std::string t() const { return _t; };

  private:
    static properties PROPS;
    FoliaElement* parseXml( const xmlNode *node );
    FoliaElement *resolve_element( const Relation *ref ) const;
    std::string ref_id;
    std::string ref_type;
    std::string _t;
  };

  class SyntacticUnit: public AbstractSpanAnnotation {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( SyntacticUnit, AbstractSpanAnnotation );

  private:
    static properties PROPS;
  };

  class Chunk: public AbstractSpanAnnotation {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( Chunk, AbstractSpanAnnotation );

  private:
    static properties PROPS;
  };

  class Entity: public AbstractSpanAnnotation {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( Entity, AbstractSpanAnnotation );

  private:
    static properties PROPS;
  };

  class AbstractSpanRole: public AbstractSpanAnnotation {
    friend void static_init();
  protected:
    // DO NOT USE AbstractSpanRole as a real node!!
    ADD_PROTECTED_CONSTRUCTORS( AbstractSpanRole, AbstractSpanAnnotation );
  private:
    static properties PROPS;
  };

  class Headspan: public AbstractSpanRole {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( Headspan, AbstractSpanRole );
  private:
    static properties PROPS;
  };

  class DependencyDependent: public AbstractSpanRole {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( DependencyDependent, AbstractSpanRole );
  private:
    static properties PROPS;
  };

  class Source: public AbstractSpanRole {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( Source, AbstractSpanRole );
  private:
    static properties PROPS;
  };

  class Target: public AbstractSpanRole {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( Target, AbstractSpanRole );
  private:
    static properties PROPS;
  };

  class Scope: public AbstractSpanRole {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( Scope, AbstractSpanRole );
  private:
    static properties PROPS;
  };

  class Cue: public AbstractSpanRole {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( Cue, AbstractSpanRole );
  private:
    static properties PROPS;
  };

  class StatementRelation: public AbstractSpanRole {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( StatementRelation, AbstractSpanRole );
  private:
    static properties PROPS;
  };

  class Dependency: public AbstractSpanAnnotation {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( Dependency, AbstractSpanAnnotation );
    Headspan *head() const;
    DependencyDependent *dependent() const;
  private:
    static properties PROPS;
  };

  class CoreferenceLink: public AbstractSpanRole {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( CoreferenceLink, AbstractSpanRole );
  private:
    static properties PROPS;
  };

  class CoreferenceChain: public AbstractSpanAnnotation {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( CoreferenceChain, AbstractSpanAnnotation );
  private:
    static properties PROPS;
  };

  class SemanticRole: public AbstractSpanAnnotation {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( SemanticRole, AbstractSpanAnnotation );
  private:
    static properties PROPS;
  };

  class Predicate: public AbstractSpanAnnotation {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( Predicate, AbstractSpanAnnotation );
  private:
    static properties PROPS;
  };

  class Sentiment: public AbstractSpanAnnotation {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( Sentiment, AbstractSpanAnnotation );
  private:
    static properties PROPS;
  };

  class Modality: public AbstractSpanAnnotation {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( Modality, AbstractSpanAnnotation );
  private:
    static properties PROPS;
  };

  class Statement: public AbstractSpanAnnotation {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( Statement, AbstractSpanAnnotation );
  private:
    static properties PROPS;
  };

  class Observation: public AbstractSpanAnnotation {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( Observation, AbstractSpanAnnotation );
  private:
    static properties PROPS;
  };


  class AbstractAnnotationLayer:
    public AbstractElement,
    public AllowGenerateID,
    public AllowInlineAnnotation
    {
      friend void static_init();
    public:
      AbstractSpanAnnotation *findspan( const std::vector<FoliaElement*>& ) const;
      FoliaElement *append( FoliaElement * );
      KWargs collectAttributes() const;
    protected:
      // DO NOT USE AbstractAnnotationLayer as a real node!!
      ADD_PROTECTED_CONSTRUCTORS( AbstractAnnotationLayer, AbstractElement );
    private:
      static properties PROPS;
      void assignset( FoliaElement * );
    };

  class AbstractCorrectionChild: public AbstractElement {
    friend void static_init();
  protected:
    // DO NOT USE AbstractCorrectionChild as a real node!!
    ADD_PROTECTED_CONSTRUCTORS( AbstractCorrectionChild, AbstractElement );
  private:
    static properties PROPS;
  };

  class New: public AbstractCorrectionChild {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( New, AbstractCorrectionChild );

    Correction *correct( const std::vector<FoliaElement*>&,
			 const std::vector<FoliaElement*>&,
			 const std::vector<FoliaElement*>&,
			 const std::vector<FoliaElement*>&,
			 const KWargs& );
    Correction *correct( const std::string& = "" );
  private:
    static properties PROPS;
  };

  class Current: public AbstractCorrectionChild {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( Current, AbstractCorrectionChild );
  private:
    static properties PROPS;
  };

  class Original: public AbstractCorrectionChild {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( Original, AbstractCorrectionChild );
  private:
    static properties PROPS;
  };

  class Suggestion: public AbstractCorrectionChild {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( Suggestion, AbstractCorrectionChild );
    void setAttributes( KWargs& );
    KWargs collectAttributes() const;
  private:
    static properties PROPS;
    std::string _split;
    std::string _merge;
  };

  class Description: public AbstractElement {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( Description, AbstractElement );

    const std::string description() const { return _value; };
    void setAttributes( KWargs& );
    FoliaElement* parseXml( const xmlNode * );
    xmlNode *xml( bool, bool=false ) const;

  private:
    static properties PROPS;
    std::string _value;
  };

  class Comment: public AbstractElement {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( Comment, AbstractElement );

    const std::string comment() const { return _value; };
    void setAttributes( KWargs& );
    FoliaElement* parseXml( const xmlNode * );
    xmlNode *xml( bool, bool=false ) const;

  private:
    static properties PROPS;
    std::string _value;
  };

  class XmlComment: public AbstractElement {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( XmlComment, AbstractElement );

    FoliaElement* parseXml( const xmlNode * );
    xmlNode *xml( bool, bool=false ) const;

  private:
    const UnicodeString private_text( const TextPolicy& ) const {
      return "";
    }
    static properties PROPS;
    std::string _value;
  };

  class XmlText: public AbstractElement {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( XmlText, AbstractElement );

    FoliaElement* parseXml( const xmlNode * );
    xmlNode *xml( bool, bool=false ) const;
    bool setvalue( const std::string& );
    const std::string& get_delimiter( bool ) const { return EMPTY_STRING; };
  private:
    const UnicodeString private_text( const TextPolicy& ) const;
    static properties PROPS;
    std::string _value; //UTF8 value
  };

  class External: public AbstractElement {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( External, AbstractElement );

    FoliaElement* parseXml( const xmlNode * );
    void resolve_external();
  private:
    static properties PROPS;
  };

  class Note: public AbstractStructureElement {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( Note, AbstractStructureElement );
    void setAttributes( KWargs& );
  private:
    static properties PROPS;
    std::string ref_id;
  };

  class Definition: public AbstractStructureElement {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( Definition, AbstractStructureElement );
  private:
    static properties PROPS;
  };

  class Term: public AbstractStructureElement {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( Term, AbstractStructureElement );

  private:
    static properties PROPS;
  };

  class Example: public AbstractStructureElement {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( Example, AbstractStructureElement );

  private:
    static properties PROPS;
  };

  class Entry: public AbstractStructureElement {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( Entry, AbstractStructureElement );

  private:
    static properties PROPS;
  };

  class Reference:
    public AbstractStructureElement,
    public AllowXlink
    {
      friend void static_init();
      friend class Note;
    public:
      ADD_PUBLIC_CONSTRUCTORS( Reference, AbstractStructureElement );

      KWargs collectAttributes() const;
      void setAttributes( KWargs& );
    private:
      void init();
      static properties PROPS;
      std::string ref_id;
      std::string ref_type;
      std::string _format;
    };


  class Correction: public AbstractInlineAnnotation {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( Correction, AbstractInlineAnnotation );
    bool hasNew() const;
    bool hasOriginal() const;
    bool hasCurrent() const;
    bool hasSuggestions() const;
    New *getNew() const;
    FoliaElement *getNew( size_t ) const;
    Original *getOriginal() const;
    FoliaElement *getOriginal( size_t ) const;
    Current *getCurrent() const;
    FoliaElement *getCurrent( size_t ) const;
    std::vector<Suggestion*> suggestions() const;
    Suggestion *suggestions( size_t ) const;
    const TextContent *text_content( const TextPolicy& ) const;
    const TextContent *text_content( const std::string& = "current" ) const;
    const PhonContent *phon_content( const TextPolicy& tp ) const;
    const PhonContent *phon_content( const std::string& = "current" ) const;
    const std::string& get_delimiter( bool=false) const;
    Correction *correct( const std::vector<FoliaElement*>&,
			 const std::vector<FoliaElement*>&,
			 const std::vector<FoliaElement*>&,
			 const std::vector<FoliaElement*>&,
			 const KWargs& );
    Correction *correct( const std::string& = "" );
  private:
    const UnicodeString private_text( const TextPolicy& ) const;
    static properties PROPS;
  };

  class ErrorDetection: public AbstractInlineAnnotation  {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( ErrorDetection, AbstractInlineAnnotation );
  private:
    static properties PROPS;
  };

  class TimeSegment: public AbstractSpanAnnotation {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( TimeSegment, AbstractSpanAnnotation );
  private:
    static properties PROPS;
  };

  class Morpheme:
    public AbstractSubtokenAnnotation,
    public AbstractWord
    {
      friend void static_init();
    public:
      ADD_PUBLIC_CONSTRUCTORS( Morpheme, AbstractSubtokenAnnotation );
    private:
      static properties PROPS;
    };

  class SyntaxLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( SyntaxLayer, AbstractAnnotationLayer );
  private:
    static properties PROPS;
  };

  class ChunkingLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( ChunkingLayer, AbstractAnnotationLayer );
  private:
    static properties PROPS;
  };

  class EntitiesLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( EntitiesLayer, AbstractAnnotationLayer );
  private:
    static properties PROPS;
  };

  class TimingLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( TimingLayer, AbstractAnnotationLayer );
  private:
    static properties PROPS;
  };

  class MorphologyLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( MorphologyLayer, AbstractAnnotationLayer );
  private:
    static properties PROPS;
  };

  class PhonologyLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( PhonologyLayer, AbstractAnnotationLayer );
  private:
    static properties PROPS;
  };

  class DependenciesLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( DependenciesLayer, AbstractAnnotationLayer );
  private:
    static properties PROPS;
  };

  class CoreferenceLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( CoreferenceLayer, AbstractAnnotationLayer );
  private:
    static properties PROPS;
  };

  class SemanticRolesLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( SemanticRolesLayer, AbstractAnnotationLayer );
  private:
    static properties PROPS;
  };

  class StatementLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( StatementLayer, AbstractAnnotationLayer );
  private:
    static properties PROPS;
  };

  class SentimentLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( SentimentLayer, AbstractAnnotationLayer );
  private:
    static properties PROPS;
  };

  class ModalitiesLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( ModalitiesLayer, AbstractAnnotationLayer );
  private:
    static properties PROPS;
  };

  class ObservationLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( ObservationLayer, AbstractAnnotationLayer );
  private:
    static properties PROPS;
  };

  class SpanRelationLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
    ADD_PUBLIC_CONSTRUCTORS( SpanRelationLayer, AbstractAnnotationLayer );
  private:
    static properties PROPS;
  };

} // namespace folia

#endif // FOLIA_SUBCLASSES_H
