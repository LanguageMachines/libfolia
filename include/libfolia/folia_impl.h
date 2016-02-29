/*
  Copyright (c) 2006 - 2016
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

#ifndef FOLIA_IMPL_H
#define FOLIA_IMPL_H

#include <set>
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <exception>
#include "unicode/unistr.h"
#include "libxml/tree.h"

namespace folia {
  class Document;
  class AbstractSpanAnnotation;
  class Alternative;
  class PosAnnotation;
  class LemmaAnnotation;
  class MorphologyLayer;
  class Sentence;
  class Word;
  class TextContent;
  class PhonContent;
  class Correction;
  class New;
  class Original;
  class Current;
  class Suggestion;
  class Division;
  class DependencyDependent;
  class Paragraph;
  class Morpheme;

  class properties;
  extern const std::set<ElementType> default_ignore_annotations;

#define NOT_IMPLEMENTED {						\
    throw NotImplementedError( xmltag() + "::" + std::string(__func__) ); \
  }

  class FoliaElement {
    friend std::ostream& operator<<( std::ostream&, const FoliaElement& );
    friend std::ostream& operator<<( std::ostream&, const FoliaElement* );
    friend bool operator==( const FoliaElement&, const FoliaElement& );
  public:
    virtual ~FoliaElement(){};
    virtual void init() {};
    virtual bool has_base( ElementType ) const = 0;
    virtual size_t size() const = 0;
    virtual FoliaElement* index( size_t ) const = 0;
    virtual FoliaElement* rindex( size_t ) const = 0;
    FoliaElement* operator[]( size_t i ) const {
      return index(i);
    }

    virtual bool isinstance( ElementType et ) const = 0;
    template <typename F>
      bool isinstance() const {
      F obj((Document*)0);
      return element_id() == obj.element_id();
    }
    bool isSubClass( ElementType ) const;
    bool isSubClass( const FoliaElement *c ) const {
      return dynamic_cast<decltype(c)>(this) != 0;
    };

    virtual void fixupDoc( Document* ) = 0;
    virtual FoliaElement *parent() const = 0;
    virtual void setParent( FoliaElement *p ) = 0;
    virtual bool acceptable( ElementType ) const = 0;
    virtual bool addable( const FoliaElement * ) const = 0;
    virtual FoliaElement *append( FoliaElement* ) = 0;
    virtual FoliaElement *postappend( ) = 0;
    virtual void remove( size_t, bool = true ) = 0;
    virtual void remove( FoliaElement *, bool = true ) = 0;
    virtual std::vector<FoliaElement*> findreplacables( FoliaElement * ) const = 0;
    virtual void replace( FoliaElement * ) = 0;
    virtual FoliaElement* replace( FoliaElement *, FoliaElement* ) = 0;


    virtual FoliaElement *head() const NOT_IMPLEMENTED;

    // Sentences
    virtual Sentence *addSentence( const KWargs& ) = 0;
    Sentence *addSentence( const std::string& s ="" ){
      return addSentence( getArgs(s) );
    };

    // Selections
    template <typename F>
      std::vector<F*> select( const std::string& st,
			      const std::set<ElementType>& exclude,
			      bool recurse = true ) const {
      F obj((Document*)0);
      std::vector<F*> res;
      std::vector<FoliaElement*> tmp = select( obj.element_id(),
					       st,
					       exclude,
					       recurse );
      for ( size_t i = 0; i < tmp.size(); ++i ){
	res.push_back( dynamic_cast<F*>( tmp[i]) );
      }
      return res;
    }

    template <typename F>
      std::vector<F*> select( const std::string& st,
			      bool recurse = true ) const {
      F obj((Document*)0);
      std::vector<F*> res;
      std::vector<FoliaElement*> tmp = select( obj.element_id(),
					       st,
					       recurse );
      for ( size_t i = 0; i < tmp.size(); ++i ){
	res.push_back( dynamic_cast<F*>( tmp[i]) );
      }
      return res;
    }

    template <typename F>
      std::vector<F*> select( const char* st,
			      bool recurse = true ) const {
      F obj((Document*)0);
      std::vector<F*> res;
      std::vector<FoliaElement*> tmp = select( obj.element_id(),
					       std::string(st),
					       recurse );
      for ( size_t i = 0; i < tmp.size(); ++i ){
	res.push_back( dynamic_cast<F*>( tmp[i]) );
      }
      return res;
    }

    template <typename F>
      std::vector<F*> select( const std::set<ElementType>& exclude,
			      bool recurse = true ) const {
      F obj((Document*)0);
      std::vector<F*> res;
      std::vector<FoliaElement*> tmp = select( obj.element_id(),
					       exclude,
					       recurse );
      for ( size_t i = 0; i < tmp.size(); ++i ){
	res.push_back( dynamic_cast<F*>( tmp[i]) );
      }
      return res;
    }

    template <typename F>
      std::vector<F*> select( bool recurse = true ) const {
      F obj((Document*)0);
      std::vector<F*> res;
      std::vector<FoliaElement*> tmp = select( obj.element_id(),
					       recurse );
      for ( size_t i = 0; i < tmp.size(); ++i ){
	res.push_back( dynamic_cast<F*>( tmp[i]) );
      }
      return res;
    }

    // annotations

    virtual bool allowannotations() const { return false; };
    virtual const std::string annotator( ) const = 0;
    virtual void annotator( const std::string& ) = 0;
    virtual AnnotatorType annotatortype() const = 0;
    virtual void annotatortype( AnnotatorType t ) =  0;
    virtual AnnotationType::AnnotationType annotation_type() const = 0;
    virtual PosAnnotation *addPosAnnotation( const KWargs& ) NOT_IMPLEMENTED;
    virtual LemmaAnnotation *addLemmaAnnotation( const KWargs& ) NOT_IMPLEMENTED;
    virtual MorphologyLayer *addMorphologyLayer( const KWargs& ) NOT_IMPLEMENTED;

    virtual PosAnnotation *getPosAnnotations( const std::string&,
					      std::vector<PosAnnotation*>& ) const NOT_IMPLEMENTED;
    virtual LemmaAnnotation *getLemmaAnnotations( const std::string&,
						  std::vector<LemmaAnnotation*>& ) const NOT_IMPLEMENTED;
    virtual MorphologyLayer *getMorphologyLayers( const std::string&,
						  std::vector<MorphologyLayer*>& ) const NOT_IMPLEMENTED;

    template <typename F>
      std::vector<F*> annotations( const std::string& s = "" ) const {
      if ( allowannotations() ){
	std::vector<F*> v = select<F>( s, default_ignore_annotations );
	if ( v.size() >= 1 )
	  return v;
	else {
	  F obj((Document*)0);
	  if ( s.empty() )
	    throw NoSuchAnnotation( obj.classname() );
	  else
	    throw NoSuchAnnotation( obj.classname() + " for set='" + s + "'" );
	}
      }
      else NOT_IMPLEMENTED;
    }

    template <typename F>
      int hasannotation( const std::string& st = "" ) const {
      try {
	std::vector<F*> v = annotations<F>( st );
	return v.size();
      }
      catch ( NoSuchAnnotation& e ){
      }
      return 0;
    }

    template <typename F>
      F *annotation( const std::string& st = "" ) const {
      std::vector<F*>v = annotations<F>( st );
      return v[0]; // always exist, otherwise annotations would throw()
    }

    template <typename F>
      F *addAnnotation( const KWargs& args ) {
      F *res = 0;
      try {
	res = new F( doc(), args);
      }
      catch( std::exception& ){
	delete res;
	throw;
      }
      append( res );
      return res;
    }
    // span annotation
    virtual std::vector<AbstractSpanAnnotation*> selectSpan() const = 0;
    virtual std::vector<AbstractSpanAnnotation*> findspans( ElementType,
							    const std::string& = "" ) const NOT_IMPLEMENTED;
    template <typename F>
      std::vector<AbstractSpanAnnotation*> findspans( const std::string& st = "" ) const {
      F obj((Document*)0);
      return findspans( obj.element_id(), st );
    }
    virtual AbstractSpanAnnotation *findspan( const std::vector<FoliaElement*>& ) const NOT_IMPLEMENTED;

    // features
    virtual std::vector<std::string> feats( const std::string& ) const = 0;
    virtual const std::string feat( const std::string& ) const = 0;

    //XML (de)serialisation
    virtual FoliaElement* parseXml( const xmlNode * ) = 0;
    const std::string xmlstring() const; // serialize to a string (XML fragment)
    virtual xmlNode *xml( bool, bool = false ) const = 0; //serialize to XML

    // text/string content
    bool hastext( const std::string& = "current" ) const;

    virtual const std::string str( const std::string& = "current" ) const = 0;
    const UnicodeString unicode( const std::string& cls = "current" ) const { return text( cls ); };
    virtual const UnicodeString text( const std::string& = "current",
				bool = false, bool = false ) const = 0;
    const UnicodeString stricttext( const std::string& = "current" ) const;
    const UnicodeString toktext( const std::string& = "current" ) const;
    virtual const UnicodeString phon( const std::string& = "current",
				bool = false ) const = 0;
    virtual bool printable() const = 0;
    virtual bool speakable() const = 0;

    // Word
    virtual Word *previous() const NOT_IMPLEMENTED;
    virtual Word *next() const NOT_IMPLEMENTED;
    virtual const Word* resolveword( const std::string& ) const = 0;
    virtual std::vector<Word*> context( size_t,
					const std::string& ="" ) const NOT_IMPLEMENTED;
    virtual std::vector<Word*> leftcontext( size_t,
					    const std::string& ="" ) const NOT_IMPLEMENTED;
    virtual std::vector<Word*> rightcontext( size_t,
					     const std::string& ="" ) const NOT_IMPLEMENTED;
    virtual Word *addWord( const KWargs& ) = 0;
    Word *addWord( const std::string& s ){
      return addWord( getArgs(s) );
    }

    // corrections
    virtual New *getNew() const NOT_IMPLEMENTED;
    virtual FoliaElement *getNew( size_t ) const NOT_IMPLEMENTED;
    virtual Original *getOriginal() const NOT_IMPLEMENTED;
    virtual FoliaElement *getOriginal( size_t ) const NOT_IMPLEMENTED;
    virtual Current *getCurrent() const NOT_IMPLEMENTED;
    virtual FoliaElement *getCurrent( size_t ) const NOT_IMPLEMENTED;
    virtual Correction *incorrection() const NOT_IMPLEMENTED;
    virtual Correction *split( FoliaElement *, FoliaElement *,
			       const std::string& = "" ) NOT_IMPLEMENTED;

    virtual Correction *mergewords( FoliaElement *,
				    const std::vector<FoliaElement *>&,
				    const std::string& = "" ) NOT_IMPLEMENTED;

    virtual Correction *deleteword( FoliaElement *,
				    const std::string& = "" ) NOT_IMPLEMENTED;
    virtual Correction *insertword( FoliaElement *, FoliaElement *,
				    const std::string& = "" ) NOT_IMPLEMENTED;
    virtual std::vector<Suggestion*> suggestions() const NOT_IMPLEMENTED;
    virtual Suggestion *suggestions( size_t ) const NOT_IMPLEMENTED;

    virtual Correction *correct( const std::vector<FoliaElement*>&,
				 const std::vector<FoliaElement*>&,
				 const std::vector<FoliaElement*>&,
				 const std::vector<FoliaElement*>&,
				 const KWargs& ) NOT_IMPLEMENTED;
    virtual Correction* correct( FoliaElement*,
				 FoliaElement*,
				 const KWargs& ) NOT_IMPLEMENTED;
    virtual Correction* correct( FoliaElement*,
				 FoliaElement*,
				 const std::vector<FoliaElement*>&,
				 const KWargs& ) NOT_IMPLEMENTED;
    virtual Correction *correct( const std::string& = "" ) NOT_IMPLEMENTED;

    // TextContent
    virtual TextContent *textcontent( const std::string& = "current" ) const = 0;
    TextContent *settext( const std::string&, const std::string& = "current" );
    TextContent *settext( const std::string&, int , const std::string& = "current" );
    TextContent *setutext( const UnicodeString&, const std::string& = "current" );
    TextContent *setutext( const UnicodeString&, int , const std::string& = "current" );
    virtual int offset() const NOT_IMPLEMENTED;

    // PhonContent
    virtual PhonContent *phoncontent( const std::string& = "current" ) const = 0;

    // properties
    virtual const std::string& getTextDelimiter( bool retaintok=false ) const = 0;
    virtual void setDateTime( const std::string& ) = 0;
    virtual const std::string getDateTime() const = 0;
    virtual const std::string pos( const std::string& = "" ) const = 0;
    virtual const std::string lemma( const std::string& = "" ) const = 0;
    virtual const std::string cls() const = 0;
    virtual const std::string sett() const = 0;
    virtual const std::string classname() const = 0;
    virtual const std::string n() const = 0;
    virtual const std::string id() const = 0;
    virtual const std::string begintime() const = 0;
    virtual const std::string endtime() const = 0;
    virtual const std::string speech_src() const = 0;
    virtual const std::string speech_speaker() const = 0;
    virtual double confidence() const = 0;
    virtual void confidence( double ) = 0;
    virtual ElementType element_id() const = 0;
    virtual size_t occurrences() const = 0;
    virtual size_t occurrences_per_set() const = 0;
    virtual Attrib required_attributes() const = 0;
    virtual Attrib optional_attributes() const = 0;
    virtual const std::string& xmltag() const = 0;
    virtual const std::string subset() const NOT_IMPLEMENTED;

    virtual Document *doc() const = 0;
    virtual Sentence *sentence() const NOT_IMPLEMENTED;
    virtual Paragraph *paragraph() const NOT_IMPLEMENTED;
    virtual Division *division() const NOT_IMPLEMENTED;
    virtual std::vector<Paragraph*> paragraphs() const NOT_IMPLEMENTED;
    virtual std::vector<Sentence*> sentences() const NOT_IMPLEMENTED;
    virtual std::vector<Word*> words() const NOT_IMPLEMENTED;
    virtual std::vector<FoliaElement*> wrefs() const NOT_IMPLEMENTED;
    virtual FoliaElement* wrefs( size_t ) const NOT_IMPLEMENTED;

    virtual std::vector<Morpheme*> morphemes( const std::string& ="" ) const NOT_IMPLEMENTED;
    virtual Morpheme* morpheme( size_t, const std::string& ="" ) const NOT_IMPLEMENTED;
    virtual Sentence *sentences( size_t ) const NOT_IMPLEMENTED;
    virtual Sentence *rsentences( size_t ) const NOT_IMPLEMENTED;
    virtual Paragraph *paragraphs( size_t ) const NOT_IMPLEMENTED;
    virtual Paragraph *rparagraphs( size_t ) const NOT_IMPLEMENTED;
    virtual Word *words( size_t ) const NOT_IMPLEMENTED;
    virtual std::vector<Word *> wordParts() const NOT_IMPLEMENTED;
    virtual Word *rwords( size_t ) const NOT_IMPLEMENTED;

    virtual DependencyDependent *dependent() const NOT_IMPLEMENTED;

    virtual const std::string description() const;

    // alternatives
    template <typename F>
      F *addAlternative();
    template <typename F>
      F *addAlternative( const KWargs& );
    virtual std::vector<Alternative *> alternatives( ElementType,
						     const std::string& = ""
						     ) const NOT_IMPLEMENTED;
    std::vector<Alternative*> alternatives( const std::string& s = "" ) const {
      return alternatives( BASE, s );
    }

    virtual const std::string content() const NOT_IMPLEMENTED;
    virtual const std::string src() const NOT_IMPLEMENTED;
    virtual const UnicodeString caption() const NOT_IMPLEMENTED;
    virtual std::vector<FoliaElement *> resolve() const NOT_IMPLEMENTED;
    virtual const FoliaElement* resolveid() const NOT_IMPLEMENTED;
    virtual bool checkAtts() = 0;


    virtual std::vector<FoliaElement*> select( ElementType elementtype,
					       bool = true ) const = 0;
    virtual std::vector<FoliaElement*> select( ElementType elementtype,
					       const std::set<ElementType>& ,
					       bool = true ) const = 0;
    virtual std::vector<FoliaElement*> select( ElementType elementtype,
					       const std::string&,
					       bool = true ) const = 0;
    virtual std::vector<FoliaElement*> select( ElementType elementtype,
					       const std::string&,
					       const std::set<ElementType>& ,
					       bool = true ) const = 0;
    // some 'internal stuff
    virtual int refcount() const = 0;
    virtual void increfcount() = 0;
    virtual void setAttributes( const KWargs& ) = 0;
    virtual KWargs collectAttributes() const = 0;
    virtual void setAuth( bool b ) = 0;
    virtual const std::string generateId( const std::string& ) NOT_IMPLEMENTED;
  };

  class FoliaImpl: public virtual FoliaElement {
  private:
    FoliaImpl( const FoliaImpl& ); // inhibit copies
    FoliaImpl& operator=( const FoliaImpl& ); // inhibit copies
  public:
    //Constructor
    FoliaImpl( const properties& p, Document* = 0 );
    // static element Constructor
    static FoliaElement *createElement( Document *, ElementType );
    static FoliaElement *createElement( Document *, const std::string&  );
    virtual ~FoliaImpl();

    void classInit(){
      init(); // virtual init
    }

    void classInit( const KWargs& a ){
      init(); // virtual init
      setAttributes( a ); // also virtual!
    }

    bool has_base( ElementType ) const { return false; };
    //functions regarding contained data
    size_t size() const { return data.size(); };
    FoliaElement* index( size_t ) const;
    FoliaElement* rindex( size_t ) const;

    bool isinstance( ElementType et ) const {
      return et == element_id();
    }
    template <typename F>
      bool isinstance() const {
      F obj((Document*)0);
      return element_id() == obj.element_id();
    }

    void fixupDoc( Document* );
    FoliaElement *parent() const { return _parent; };
    void setParent( FoliaElement *p ) { _parent = p ; };
    bool acceptable( ElementType ) const;
    bool addable( const FoliaElement * ) const;
    FoliaElement *append( FoliaElement* );
    FoliaElement *postappend( ) { return this; };
    void remove( size_t, bool = true );
    void remove( FoliaElement *, bool = true );
    std::vector<FoliaElement*> findreplacables( FoliaElement * ) const;
    void replace( FoliaElement * );
    FoliaElement* replace( FoliaElement *, FoliaElement* );

    // Sentences
    Sentence *addSentence( const KWargs& );

    // Selections
    template <typename F>
      std::vector<F*> select( bool recurse = true ) const {
      return FoliaElement::select<F>(recurse);
    }

    template <typename F>
      std::vector<F*> select( const std::string& st,
			      const std::set<ElementType>& exclude,
			      bool recurse = true ) const {
      return FoliaElement::select<F>( st, exclude, recurse );
    }

    template <typename F>
      std::vector<F*> select( const std::string& st,
			      bool recurse = true ) const {
      return FoliaElement::select<F>( st, recurse );
    }

    template <typename F>
      std::vector<F*> select( const char* st,
			      bool recurse = true ) const {
      return FoliaElement::select<F>( st, recurse );
    }

    template <typename F>
      std::vector<F*> select( const std::set<ElementType>& exclude,
			      bool recurse = true ) const {
      return FoliaElement::select<F>( exclude, recurse );
    }

    const std::string annotator( ) const { return _annotator; };
    void annotator( const std::string& a ) { _annotator = a; };
    AnnotatorType annotatortype() const { return _annotator_type; };
    void annotatortype( AnnotatorType t ) { _annotator_type =  t; };

    template <typename F>
      F *addAnnotation( const KWargs& args ) {
      return FoliaElement::addAnnotation<F>( args );
    }

    template <typename F>
      std::vector<F*> annotations( const std::string& s = "" ) const {
      return FoliaElement::annotations<F>( s );
    }

    template <typename F>
      int hasannotation( const std::string& st = "" ) const {
      return FoliaElement::hasannotation<F>(st);
    }

    template <typename F>
      F *annotation( const std::string& st = "" ) const {
      return FoliaElement::annotation<F>(st);
    }

    // Span annotations
    std::vector<AbstractSpanAnnotation*> selectSpan() const;

    // features
    std::vector<std::string> feats( const std::string& ) const;
    const std::string feat( const std::string& ) const;

    //XML (de)serialisation
    FoliaElement* parseXml( const xmlNode * );
    xmlNode *xml( bool, bool = false ) const;

    // text/string content

    const std::string str( const std::string& = "current" ) const;
    const UnicodeString text( const std::string& = "current",
			      bool = false, bool = false ) const;

    const UnicodeString phon( const std::string& = "current",
			      bool = false ) const;
    const UnicodeString deeptext( const std::string& = "current",
				  bool = false ) const;
    const UnicodeString deepphon( const std::string& = "current" ) const;

    // Word
    const Word* resolveword( const std::string& ) const { return 0; };
    Word *addWord( const KWargs& );

    // TextContent
    TextContent *textcontent( const std::string& = "current" ) const;
    // PhonContent
    PhonContent *phoncontent( const std::string& = "current" ) const;

    // properties
    const std::string& getTextDelimiter( bool retaintok=false ) const;
    void setDateTime( const std::string& );
    const std::string getDateTime() const;
    const std::string pos( const std::string& = "" ) const ;
    const std::string lemma( const std::string& = "" ) const;
    const std::string cls() const { return _class; };
    const std::string sett() const { return _set; };
    const std::string n() const { return _n; };
    const std::string id() const { return _id; };
    const std::string href() const { return _href; };
    const std::string begintime() const { return _begintime; };
    const std::string endtime() const { return _endtime; };
    const std::string speech_src() const;
    const std::string speech_speaker() const;

    double confidence() const { return _confidence; };
    void confidence( double d ) { _confidence = d; };

    // generic properties
    ElementType element_id() const;
    const std::string classname() const;
    size_t occurrences() const;
    size_t occurrences_per_set() const;
    Attrib required_attributes() const;
    Attrib optional_attributes() const;
    const std::string& xmltag() const;
    AnnotationType::AnnotationType annotation_type() const;
    const std::set<ElementType>& accepted_data() const;
    bool printable() const;
    bool speakable() const;
    bool xlink() const;


    Document *doc() const { return mydoc; };

    bool checkAtts();

    std::vector<FoliaElement*> select( ElementType elementtype,
				       bool = true ) const;
    std::vector<FoliaElement*> select( ElementType elementtype,
				       const std::set<ElementType>& ,
				       bool = true ) const;
    std::vector<FoliaElement*> select( ElementType elementtype,
				       const std::string&,
				       bool = true ) const;
    std::vector<FoliaElement*> select( ElementType elementtype,
				       const std::string&,
				       const std::set<ElementType>& ,
				       bool = true ) const;
    int refcount() const { return _refcount; };
    void increfcount() { ++_refcount; };
    void setAttributes( const KWargs& );
    KWargs collectAttributes() const;
    void setAuth( bool b ){ _auth = b; };

  protected:
    xmlNs *foliaNs() const;
    std::vector<FoliaElement*> data;
    FoliaElement *_parent;
    bool _auth;
    Document *mydoc;
    std::string _id;
    std::string _class;
    std::string _set;
    std::string _href;
    std::string _src;
  private:
    void addFeatureNodes( const KWargs& args );
    std::string _annotator;
    std::string _n;
    std::string _datetime;
    std::string _begintime;
    std::string _endtime;
    std::string _speaker;
    AnnotatorType _annotator_type;
    double _confidence;
    int _refcount;
    const properties& _props;
  };

  bool isSubClass( const ElementType e1, const ElementType e2 );

  bool isSubClass( const FoliaElement *e1, const FoliaElement *e2 );

  template <typename T1, typename T2>
    bool isSubClass(){
    T1 t1((Document*)0);
    return dynamic_cast<T2*>(&t1) != 0;
  }

  bool operator==( const FoliaElement&, const FoliaElement& );
  inline bool operator!=( const FoliaElement& e1, const FoliaElement& e2 ){
    return !( e1 == e2 );
  }

  inline size_t len( const FoliaElement *e ) {
    return e->size(); }

  template <typename T>
    inline size_t len( const std::vector<T>& v ) {
    return v.size(); }

  inline const std::string str( const FoliaElement *e,
				const std::string& cls = "current" ) {
    return e->str( cls ); }

  inline const UnicodeString text( const FoliaElement *e,
				   const std::string& cls = "current" ) {
    if ( e )
      return e->text( cls );
    else
      throw ValueError( "text() for empty element" );
  }

  inline const UnicodeString unicode( const FoliaElement *e ) {
    return e->unicode(); }

  inline bool isinstance( const FoliaElement *e, ElementType t ) {
    return e->isinstance( t ); }

  class AllowGenerateID: public virtual FoliaElement {
  public:
    void setMaxId( FoliaElement * );
    int getMaxId( const std::string& );
    const std::string generateId( const std::string& tag ){
      return IDgen( tag, this );
    }
  private:
    std::string IDgen( const std::string&, const FoliaElement* );
    std::map<std::string, int> id_map;
  };

  class AllowCorrection: public virtual FoliaElement {
  public:
    Correction *correct( const std::vector<FoliaElement*>& v1,
			 const std::vector<FoliaElement*>& v2,
 			 const std::vector<FoliaElement*>& v3,
			 const std::vector<FoliaElement*>& v4,
			 const KWargs& args );
    Correction *correct( FoliaElement* o,
			 FoliaElement* n,
			 const KWargs& args ) {
      const std::vector<FoliaElement*> v;
      return correct( o, n, v, args );
    }
    Correction* correct( FoliaElement*,
			 FoliaElement*,
			 const std::vector<FoliaElement*>&,
			 const KWargs& );
    Correction *correct( const std::string& = "" );
  };

  class AllowAnnotation: public virtual FoliaElement {
  public:
    bool allowannotations() const { return true; };
    PosAnnotation *addPosAnnotation( const KWargs& );
    PosAnnotation *getPosAnnotations( const std::string&,
				      std::vector<PosAnnotation*>& ) const;
    LemmaAnnotation *addLemmaAnnotation( const KWargs& );
    LemmaAnnotation *getLemmaAnnotations( const std::string&,
					  std::vector<LemmaAnnotation*>& ) const;
  };

  class AbstractStructureElement:
    public FoliaImpl,
    public AllowGenerateID,
    public AllowAnnotation,
    public AllowCorrection
    {
      friend void static_init();
    protected:
      // DO NOT USE AbstractStructureElement as a real node!!
    AbstractStructureElement( const properties& props, Document *d=0 ):
      FoliaImpl( props, d ){ classInit(); };
    AbstractStructureElement( Document *d=0 ):
      FoliaImpl( PROPS, d ) { classInit(); };
    public:
      const std::string str( const std::string& = "current" ) const;
      std::vector<Alternative *> alternatives( ElementType = BASE,
					       const std::string& = "" ) const;

      FoliaElement *append( FoliaElement* );
      std::vector<Paragraph*> paragraphs() const;
      std::vector<Sentence*> sentences() const;
      std::vector<Word*> words() const;
      Sentence *sentences( size_t ) const;
      Sentence *rsentences( size_t ) const;
      Paragraph *paragraphs( size_t ) const;
      Paragraph *rparagraphs( size_t ) const;
      Word *words( size_t ) const;
      Word *rwords( size_t ) const;
      const Word* resolveword( const std::string& ) const;
  private:
      static properties PROPS;
    };

  class AbstractAnnotation: public FoliaImpl {
    friend void static_init();
  protected:
    // DO NOT USE AbstractAnnotation as a real node!!
  AbstractAnnotation( const properties& props, Document *d=0 ):
    FoliaImpl( props, d ){};
  AbstractAnnotation( Document *d=0 ):
    FoliaImpl( PROPS, d ){};
  private:
    static properties PROPS;
  };

  class AbstractTokenAnnotation:
    public AbstractAnnotation,
    public AllowGenerateID
    {
      friend void static_init();
    protected:
    // DO NOT USE AbstractTokenAnnotation as a real node!!
    AbstractTokenAnnotation( const properties& props, Document *d=0 ):
      AbstractAnnotation( props, d ){ classInit(); };
    AbstractTokenAnnotation( Document *d=0 ):
      AbstractAnnotation( PROPS, d ){ classInit(); };

    private:
      bool has_base( ElementType e ) const {
	return e == AbstractAnnotation_t;
      }
      static properties PROPS;
    };

  class AbstractSpanAnnotation:
    public AbstractAnnotation,
    public AllowGenerateID,
    public AllowAnnotation,
    public AllowCorrection
    {
      friend void static_init();
    protected:
      // DO NOT USE AbstractSpanAnnotation as a real node!!
    AbstractSpanAnnotation( const properties& props, Document *d=0 ):
      AbstractAnnotation( props, d ){ classInit(); };
    AbstractSpanAnnotation( Document *d=0 ):
      AbstractAnnotation( PROPS, d ){ classInit(); };
    public:
      xmlNode *xml( bool, bool=false ) const;
      FoliaElement *append( FoliaElement* );

      std::vector<FoliaElement*> wrefs() const;
      FoliaElement *wrefs( size_t ) const;

    private:
      bool has_base( ElementType e ) const {
	return e == AbstractAnnotation_t;
      }
      static properties PROPS;
    };

  class ComplexAlignment: public AbstractAnnotation {
    friend void static_init();
  public:
  ComplexAlignment( Document *d ):
    AbstractAnnotation( PROPS, d ){ classInit(); }
  ComplexAlignment( Document *d, const KWargs& a ):
    AbstractAnnotation( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };


  const std::string EMPTY_STRING = "";

  class AbstractTextMarkup: public AbstractAnnotation {
    friend void static_init();
  protected:
    // DO NOT USE AbstractTextMarkup as a real node!!
  AbstractTextMarkup( const properties& props, Document *d=0 ):
    AbstractAnnotation( props, d ){ classInit(); };
  AbstractTextMarkup( Document *d=0 ):
    AbstractAnnotation( PROPS, d ){ classInit(); };
  public:
    void setAttributes( const KWargs& );
    KWargs collectAttributes() const;
    const FoliaElement* resolveid() const;
    const UnicodeString text( const std::string& = "current",
			      bool = false, bool = false ) const;
  protected:
    const std::string& getTextDelimiter( bool ) const { return EMPTY_STRING; };
    std::string idref;
  private:
    bool has_base( ElementType e ) const {
      return e == AbstractAnnotation_t;
    }
    static properties PROPS;
  };

  class TextMarkupGap: public AbstractTextMarkup {
    friend void static_init();
  public:
  TextMarkupGap( Document *d=0 ):
    AbstractTextMarkup( PROPS, d ) { classInit(); };
  private:
    bool has_base( ElementType e ) const {
      return e == AbstractTextMarkup_t
	|| e == AbstractAnnotation_t;
    }
    static properties PROPS;
  };

  class TextMarkupString: public AbstractTextMarkup {
    friend void static_init();
  public:
  TextMarkupString( Document *d=0 ):
    AbstractTextMarkup( PROPS, d ) { classInit(); };
  private:
    bool has_base( ElementType e ) const {
      return e == AbstractTextMarkup_t
	|| e == AbstractAnnotation_t;
    }
    static properties PROPS;
  };

  class TextMarkupCorrection: public AbstractTextMarkup {
    friend void static_init();
  public:
  TextMarkupCorrection( Document *d=0 ):
    AbstractTextMarkup( PROPS, d ){ classInit(); };
    void setAttributes( const KWargs& );
    KWargs collectAttributes() const;
    const UnicodeString text( const std::string& = "current",
			      bool = false, bool = false ) const;
  private:
    bool has_base( ElementType e ) const {
      return e == AbstractTextMarkup_t
	|| e == AbstractAnnotation_t;
    }
    static properties PROPS;
    std::string _original;
  };

  class TextMarkupError: public AbstractTextMarkup {
    friend void static_init();
  public:
  TextMarkupError( Document *d=0 ):
    AbstractTextMarkup( PROPS, d ){ classInit(); };
  private:
    bool has_base( ElementType e ) const {
      return e == AbstractTextMarkup_t
	|| e == AbstractAnnotation_t;
    }
    static properties PROPS;
  };

  class TextMarkupStyle: public AbstractTextMarkup {
    friend void static_init();
  public:
  TextMarkupStyle( Document *d=0 ):
    AbstractTextMarkup( PROPS, d ){ classInit(); };
  private:
    bool has_base( ElementType e ) const {
      return e == AbstractTextMarkup_t
	|| e == AbstractAnnotation_t;
    }
    static properties PROPS;
  };

  class TextContent: public FoliaImpl {
    friend void static_init();
  public:
  TextContent( Document *d ):
    FoliaImpl(PROPS,d){ classInit(); }
  TextContent( ):
    FoliaImpl(PROPS,0){ classInit(); }
  TextContent( Document *d, const KWargs& a ):
    FoliaImpl(PROPS,d){ classInit( a ); }
  TextContent( const KWargs& a ):
    FoliaImpl(PROPS,0){ classInit( a ); }
    void setAttributes( const KWargs& );
    KWargs collectAttributes() const;
    const std::string str( const std::string& = "current" ) const;
    const UnicodeString text( const std::string& = "current",
			      bool = false, bool = false ) const;
    int offset() const { return _offset; };
    TextContent *postappend();
    std::vector<FoliaElement*> findreplacables( FoliaElement * ) const;
  private:
    void init();
    static properties PROPS;
    int _offset;
  };

  class PhonContent: public FoliaImpl {
    friend void static_init();
  public:
  PhonContent( Document *d = 0 ):
    FoliaImpl(PROPS,d){ classInit(); }
  PhonContent( Document *d, const KWargs& a ):
    FoliaImpl(PROPS,d){ classInit( a ); }
  PhonContent( const KWargs& a ):
    FoliaImpl(PROPS,0){ classInit( a ); }
    void setAttributes( const KWargs& );
    KWargs collectAttributes() const;
    const UnicodeString phon( const std::string& = "current",
			      bool = false ) const;
    int offset() const { return _offset; };
  private:
    void init();
    static properties PROPS;
    int _offset;
  };

  class FoLiA: public FoliaImpl {
    friend void static_init();
  public:
  FoLiA( Document *d ):
    FoliaImpl( PROPS,d) { classInit(); }
  FoLiA( Document *d, const KWargs& a ):
    FoliaImpl( PROPS, d ) { classInit( a ); }
  FoLiA( ):
    FoliaImpl( PROPS, 0 ) { classInit( ); }
  FoLiA( const KWargs& a ):
    FoliaImpl( PROPS, 0 ) { classInit( a ); }

  private:
    static properties PROPS;
  };

  class DCOI: public FoliaImpl {
    friend void static_init();
  public:
  DCOI( Document *d ):
    FoliaImpl( PROPS, d ) { classInit( ); }
  DCOI( Document *d, const KWargs& a ):
    FoliaImpl( PROPS, d ) { classInit( a ); }
  DCOI( ):
    FoliaImpl( PROPS, 0 ) { classInit( ); }
  DCOI( const KWargs& a ):
    FoliaImpl( PROPS, 0 ) { classInit( a ); }

  private:
    static properties PROPS;
  };

  class Head: public AbstractStructureElement {
    friend void static_init();
  public:
  Head( Document *d ):
    AbstractStructureElement( PROPS, d ) {  classInit(); }
  Head( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ) {  classInit( a ); }
  Head():
    AbstractStructureElement( PROPS, 0 ) {  classInit(); }
  Head( const KWargs& a ):
    AbstractStructureElement( PROPS, 0 ) {  classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
  };

  class TableHead: public AbstractStructureElement {
    friend void static_init();
  public:
  TableHead():
    AbstractStructureElement( PROPS, 0 ) { classInit(); }
  TableHead( const KWargs& a ):
    AbstractStructureElement( PROPS, 0 ) {  classInit( a ); }
  TableHead( Document *d ):
    AbstractStructureElement( PROPS, d ) {  classInit( ); }
  TableHead( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ) {  classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
  };

  class Table: public AbstractStructureElement {
    friend void static_init();
  public:
  Table():
    AbstractStructureElement( PROPS, 0 ) {  classInit(); };
  Table( const KWargs& a ):
    AbstractStructureElement( PROPS, 0 ) {  classInit( a ); };
  Table( Document *d ):
    AbstractStructureElement( PROPS, d ) {  classInit(); };
  Table( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ) {  classInit( a ); };
  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
  };

  class Row: public AbstractStructureElement {
    friend void static_init();
  public:
  Row( ):
    AbstractStructureElement( PROPS, 0 ) {  classInit(); };
  Row( const KWargs& a ):
    AbstractStructureElement( PROPS, 0 ) {  classInit( a ); };
  Row( Document *d ):
    AbstractStructureElement( PROPS, d ) {  classInit(); };
  Row( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ) {  classInit( a ); };
  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
  };

  class Cell: public AbstractStructureElement {
    friend void static_init();
  public:
  Cell( ):
    AbstractStructureElement( PROPS, 0 ) {  classInit(); };
  Cell( const KWargs& a ):
    AbstractStructureElement( PROPS, 0 ) {  classInit( a ); };
  Cell( Document *d ):
    AbstractStructureElement( PROPS, d ) {  classInit(); };
  Cell( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ) {  classInit( a ); };
  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
  };

  class Gap: public FoliaImpl {
    friend void static_init();
  public:
  Gap( ):
    FoliaImpl( PROPS, 0 ) { classInit(); }
  Gap( const KWargs& a ):
    FoliaImpl( PROPS, 0 ) { classInit( a ); }
  Gap( Document *d ):
    FoliaImpl( PROPS, d ) { classInit(); }
  Gap( Document *d, const KWargs& a ):
    FoliaImpl( PROPS, d ) { classInit( a ); }

    const std::string content() const;
  private:
    static properties PROPS;
  };

  class Content: public FoliaImpl {
    friend void static_init();
  public:
  Content( ):
    FoliaImpl( PROPS, 0 ) { classInit(); }
  Content( const KWargs& a ):
    FoliaImpl( PROPS, 0 ) { classInit( a ); }
  Content( Document *d ):
    FoliaImpl( PROPS, d ) { classInit(); }
  Content( Document *d, const KWargs& a ):
    FoliaImpl( PROPS, d ) { classInit( a ); }

    FoliaElement* parseXml( const xmlNode * );
    xmlNode *xml( bool, bool = false ) const;
    const std::string content() const { return value; };

  private:
    static properties PROPS;
    std::string value;
  };

  class Metric: public FoliaImpl {
    friend void static_init();
  public:
  Metric( ):
    FoliaImpl( PROPS, 0 ) { classInit(); }
  Metric( const KWargs& a ):
    FoliaImpl( PROPS, 0 ) { classInit( a ); }
  Metric( Document *d ):
    FoliaImpl( PROPS, d ) { classInit(); }
  Metric( Document *d, const KWargs& a ):
    FoliaImpl( PROPS, d ) { classInit( a ); }

  private:
    static properties PROPS;
  };

  class Division: public AbstractStructureElement {
    friend void static_init();
  public:
  Division( ):
    AbstractStructureElement( PROPS, 0 ) { classInit(); };
  Division( const KWargs& a ):
    AbstractStructureElement( PROPS, 0 ) { classInit( a ); };
  Division( Document *d ):
    AbstractStructureElement( PROPS, d ) { classInit(); };
  Division( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ) { classInit( a ); };
    Head *head() const;
  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
  };

  class LineBreak: public AbstractStructureElement {
    friend void static_init();
  public:
  LineBreak( ):
    AbstractStructureElement( PROPS, 0 ){ classInit(); };
  LineBreak( const KWargs& a ):
    AbstractStructureElement( PROPS, 0 ){ classInit( a ); };
  LineBreak( Document *d ):
    AbstractStructureElement( PROPS, d ){ classInit(); };
  LineBreak( Document *d,  const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); };
    const UnicodeString text( const std::string& = "current",
			      bool = false, bool = false ) const {
      return "";
    }
  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
  };

  class WhiteSpace: public AbstractStructureElement {
    friend void static_init();
  public:
  WhiteSpace( ):
    AbstractStructureElement( PROPS, 0 ){ classInit(); }
  WhiteSpace( const KWargs& a ):
    AbstractStructureElement( PROPS, 0 ){ classInit( a ); }
  WhiteSpace( Document *d ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  WhiteSpace( Document *d,  const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
  };

  class Word: public AbstractStructureElement {
    friend void static_init();
  public:
  Word( Document *d = 0 ):
    AbstractStructureElement(PROPS, d ) { classInit(); }
  Word( const KWargs& a ):
    AbstractStructureElement( PROPS, 0 ) { classInit( a ); }
  Word( const properties& p, Document *d=0 ):
    AbstractStructureElement( p, d ) { classInit(); }
  Word( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ) { classInit( a ); };

    Correction *split( FoliaElement *, FoliaElement *,
		       const std::string& = "" );
    Sentence *sentence() const;
    Paragraph *paragraph() const;
    Division *division() const;
    std::vector<Morpheme *> morphemes( const std::string& ="" ) const;
    Morpheme* morpheme( size_t, const std::string& ="" ) const;
    Correction *incorrection() const;
    Word *previous() const;
    Word *next() const;
    std::vector<Word*> context( size_t,
				const std::string& ="" ) const;
    std::vector<Word*> leftcontext( size_t,
				    const std::string& = "" ) const;
    std::vector<Word*> rightcontext( size_t,
				     const std::string& ="" ) const;
    std::vector<AbstractSpanAnnotation*> findspans( ElementType,
					  const std::string& = "" ) const;
    FoliaElement *append( FoliaElement *);
    const Word* resolveword( const std::string& ) const;
    void setAttributes( const KWargs& );
    KWargs collectAttributes() const;
    const std::string& getTextDelimiter( bool=false) const;
    MorphologyLayer *addMorphologyLayer( const KWargs& );
    MorphologyLayer *getMorphologyLayers( const std::string&,
					  std::vector<MorphologyLayer*>& ) const;
  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    void init();
    static properties PROPS;
    bool _space;
  };

  class Part: public AbstractStructureElement {
    friend void static_init();
  public:
  Part(  ):
    AbstractStructureElement( PROPS, 0 ){ classInit(); }
  Part( const KWargs& a ):
    AbstractStructureElement( PROPS, 0 ){ classInit( a ); }
  Part( Document *d ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Part( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
  };

  class String:
    public AbstractTokenAnnotation,
    public AllowAnnotation,
    public AllowCorrection {
    friend void static_init();
  public:
  String(  ):
    AbstractTokenAnnotation( PROPS, 0 ){ classInit(); }
  String( const KWargs& a ):
    AbstractTokenAnnotation( PROPS, 0 ){ classInit( a ); }
  String( Document *d ):
    AbstractTokenAnnotation( PROPS, d ){ classInit(); }
  String( Document *d, const KWargs& a ):
    AbstractTokenAnnotation( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractTokenAnnotation_t
	|| e == AbstractAnnotation_t;

    }
    static properties PROPS;
    };

  class PlaceHolder: public Word {
    friend void static_init();
  public:
  PlaceHolder( ):
    Word( PROPS, 0 ){ classInit(); }
  PlaceHolder( const KWargs& a ):
    Word( PROPS, 0 ){ classInit( a ); }
  PlaceHolder( Document *d ):
    Word( PROPS, d ){ classInit(); }
  PlaceHolder( Document *d, const KWargs& a ):
    Word( PROPS, d ){ classInit( a ); }

    void setAttributes( const KWargs& );
  private:
    bool has_base( ElementType e ) const {
      return e == Word_t || e == AbstractStructureElement_t;
      }
    static properties PROPS;
  };

  class Sentence: public AbstractStructureElement {
    friend void static_init();
  public:
  Sentence( ):
    AbstractStructureElement( PROPS, 0 ){ classInit(); }
  Sentence( const KWargs& a ):
    AbstractStructureElement( PROPS, 0 ){ classInit( a ); }
  Sentence( Document *d ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Sentence( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

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
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
    Correction *correctWords( const std::vector<FoliaElement *>&,
			      const std::vector<FoliaElement *>&,
			      const KWargs& );
  };

  class Speech: public AbstractStructureElement {
    friend void static_init();
  public:
  Speech( ):
    AbstractStructureElement( PROPS, 0 ){ classInit(); }
  Speech( const KWargs& a ):
    AbstractStructureElement( PROPS, 0 ){ classInit( a ); }
  Speech( Document *d ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Speech( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
  };

  class Text: public AbstractStructureElement {
    friend void static_init();
  public:
  Text( ):
    AbstractStructureElement( PROPS, 0 ){ classInit(); }
  Text( const KWargs& a ):
    AbstractStructureElement( PROPS, 0 ){ classInit( a ); }
  Text( Document *d ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Text( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
  };

  class Utterance: public AbstractStructureElement {
    friend void static_init();
  public:
  Utterance( ):
    AbstractStructureElement( PROPS, 0 ){ classInit(); }
  Utterance( const KWargs& a ):
    AbstractStructureElement( PROPS, 0 ){ classInit( a ); }
  Utterance( Document *d ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Utterance( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
  };

  class Event: public AbstractStructureElement {
    friend void static_init();
  public:
  Event( ):
    AbstractStructureElement( PROPS, 0 ){ classInit(); }
  Event( const KWargs& a ):
    AbstractStructureElement( PROPS, 0 ){ classInit( a ); }
  Event( Document *d ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Event( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
  };

  class Caption: public AbstractStructureElement {
    friend void static_init();
  public:
  Caption(  ):
    AbstractStructureElement( PROPS, 0 ){ classInit(); }
  Caption( const KWargs& a ):
    AbstractStructureElement( PROPS, 0 ){ classInit( a ); }
  Caption( Document *d ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Caption( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
  };

  class Label: public AbstractStructureElement {
    friend void static_init();
  public:
  Label( ):
    AbstractStructureElement( PROPS, 0 ){ classInit(); }
  Label( const KWargs& a ):
    AbstractStructureElement( PROPS, 0 ){ classInit( a ); }
  Label( Document *d ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Label( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }
  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
  };

  class Item: public AbstractStructureElement {
    friend void static_init();
  public:
  Item( ):
    AbstractStructureElement( PROPS, 0 ){ classInit(); }
  Item( const KWargs& a ):
    AbstractStructureElement( PROPS, 0 ){ classInit( a ); }
  Item( Document *d ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Item( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
  };

  class List: public AbstractStructureElement {
    friend void static_init();
  public:
  List( ):
    AbstractStructureElement( PROPS, 0 ){ classInit(); }
  List( const KWargs& a ):
    AbstractStructureElement( PROPS, 0 ){ classInit( a ); }
  List( Document *d ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  List( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
  };

  class Figure: public AbstractStructureElement {
    friend void static_init();
  public:
  Figure( ):
    AbstractStructureElement( PROPS, 0 ){ classInit(); }
  Figure( const KWargs& a ):
    AbstractStructureElement( PROPS, 0 ){ classInit( a ); }
  Figure( Document *d ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Figure( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

    const std::string src() const { return _src; };
    const UnicodeString caption() const;
  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
  };

  class Paragraph: public AbstractStructureElement {
    friend void static_init();
  public:
  Paragraph( ):
    AbstractStructureElement( PROPS, 0 ){ classInit(); }
  Paragraph( const KWargs& a ):
    AbstractStructureElement( PROPS, 0 ){ classInit( a ); }
  Paragraph( Document *d ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Paragraph( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
  };

  class Alternative: public AbstractStructureElement {
    friend void static_init();
  public:
  Alternative( ):
    AbstractStructureElement( PROPS, 0 ){ classInit(); }
  Alternative( const KWargs& a ):
    AbstractStructureElement( PROPS, 0 ){ classInit( a ); }
  Alternative( Document *d ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Alternative( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    void init();
    static properties PROPS;
  };


  class AlternativeLayers: public FoliaImpl {
    friend void static_init();
  public:
  AlternativeLayers( ):
    FoliaImpl( PROPS, 0 ){ classInit(); }
  AlternativeLayers( const KWargs& a ):
    FoliaImpl( PROPS, 0 ){ classInit( a ); }
  AlternativeLayers( Document *d ):
    FoliaImpl( PROPS, d ){ classInit(); }
  AlternativeLayers( Document *d, const KWargs& a ):
    FoliaImpl( PROPS, d ){ classInit( a ); }

  private:
    void init();
    static properties PROPS;
  };

  template <typename F>
    F *FoliaElement::addAlternative( const KWargs& args ){
    KWargs kw;
    std::string id = generateId( "alt" );
    kw["id"] = id;
    F *res = 0;
    Alternative *alt = 0;
    try {
      alt = new Alternative( doc(), kw );
      res = alt->addAnnotation<F>( args );
    }
    catch( std::exception& ){
      delete alt;
      throw;
    }
    append( alt );
    return res;
  }

  template <typename F>
    F *FoliaElement::addAlternative(){
    KWargs numb;
    return addAlternative<F>( numb );
  }

  class PosAnnotation: public AbstractTokenAnnotation {
    friend void static_init();
  public:
  PosAnnotation( ):
    AbstractTokenAnnotation( PROPS, 0 ){ classInit(); }
  PosAnnotation( const KWargs& a ):
    AbstractTokenAnnotation( PROPS, 0 ){ classInit( a ); }
  PosAnnotation( Document *d ):
    AbstractTokenAnnotation( PROPS, d ){ classInit(); }
  PosAnnotation( Document *d, const KWargs& a ):
    AbstractTokenAnnotation( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractTokenAnnotation_t
	|| e == AbstractAnnotation_t;
    }
    static properties PROPS;
  };

  class LemmaAnnotation: public AbstractTokenAnnotation {
    friend void static_init();
  public:
  LemmaAnnotation( ):
    AbstractTokenAnnotation( PROPS, 0 ){ classInit(); }
  LemmaAnnotation( const KWargs& a ):
    AbstractTokenAnnotation( PROPS, 0 ){ classInit( a ); }
  LemmaAnnotation( Document *d ):
    AbstractTokenAnnotation( PROPS, d ){ classInit(); }
  LemmaAnnotation( Document *d, const KWargs& a ):
    AbstractTokenAnnotation( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractTokenAnnotation_t
      || e == AbstractAnnotation_t;
    }
    static properties PROPS;
  };

  class LangAnnotation: public AbstractTokenAnnotation {
    friend void static_init();
  public:
  LangAnnotation( ):
    AbstractTokenAnnotation( PROPS, 0 ){ classInit(); }
  LangAnnotation( const KWargs& a ):
    AbstractTokenAnnotation( PROPS, 0 ){ classInit( a ); }
  LangAnnotation( Document *d ):
    AbstractTokenAnnotation( PROPS, d ){ classInit(); }
  LangAnnotation( Document *d, const KWargs& a ):
    AbstractTokenAnnotation( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractTokenAnnotation_t
	|| e == AbstractAnnotation_t;
    }
    static properties PROPS;
  };

  class Phoneme: public AbstractTokenAnnotation {
    friend void static_init();
  public:
  Phoneme( ):
    AbstractTokenAnnotation( PROPS, 0 ){ classInit(); }
  Phoneme( const KWargs& a ):
    AbstractTokenAnnotation( PROPS, 0 ){ classInit( a ); }
  Phoneme( Document *d ):
    AbstractTokenAnnotation( PROPS, d ){ classInit(); }
  Phoneme( Document *d, const KWargs& a ):
    AbstractTokenAnnotation( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractTokenAnnotation_t
	|| e == AbstractAnnotation_t;
    }
    static properties PROPS;
  };

  class DomainAnnotation: public AbstractTokenAnnotation {
    friend void static_init();
  public:
  DomainAnnotation(   ):
    AbstractTokenAnnotation( PROPS, 0 ){ classInit(); }
  DomainAnnotation(  const KWargs& a ):
    AbstractTokenAnnotation( PROPS, 0 ){ classInit( a ); }
  DomainAnnotation( Document *d ):
    AbstractTokenAnnotation( PROPS, d ){ classInit(); }
  DomainAnnotation( Document *d, const KWargs& a ):
    AbstractTokenAnnotation( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractTokenAnnotation_t
	|| e == AbstractAnnotation_t;
    }
    static properties PROPS;
  };

  class SenseAnnotation: public AbstractTokenAnnotation {
    friend void static_init();
  public:
  SenseAnnotation(  ):
    AbstractTokenAnnotation( PROPS, 0 ){ classInit(); }
  SenseAnnotation( const KWargs& a ):
    AbstractTokenAnnotation( PROPS, 0 ){ classInit( a ); }
  SenseAnnotation( Document *d ):
    AbstractTokenAnnotation( PROPS, d ){ classInit(); }
  SenseAnnotation( Document *d, const KWargs& a ):
    AbstractTokenAnnotation( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractTokenAnnotation_t
	|| e == AbstractAnnotation_t;
    }
    static properties PROPS;
  };

  class SubjectivityAnnotation: public AbstractTokenAnnotation {
    friend void static_init();
  public:
  SubjectivityAnnotation(  ):
    AbstractTokenAnnotation( PROPS, 0 ){ classInit(); }
  SubjectivityAnnotation( const KWargs& a ):
    AbstractTokenAnnotation( PROPS, 0 ){ classInit( a ); }
  SubjectivityAnnotation( Document *d ):
    AbstractTokenAnnotation( PROPS, d ){ classInit(); }
  SubjectivityAnnotation( Document *d, const KWargs& a ):
    AbstractTokenAnnotation( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractTokenAnnotation_t
	|| e == AbstractAnnotation_t;
    }
    static properties PROPS;
  };

  class Quote: public AbstractStructureElement {
    friend void static_init();
  public:
  Quote(  ):
    AbstractStructureElement( PROPS, 0 ){ classInit(); };
  Quote( const KWargs& a ):
    AbstractStructureElement( PROPS, 0 ){ classInit( a ); };
  Quote( Document *d ):
    AbstractStructureElement( PROPS, d ){ classInit(); };
  Quote( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); };
    FoliaElement *append( FoliaElement *);
    std::vector<Word*> wordParts() const;
    const std::string& getTextDelimiter( bool=false) const;
  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
  };

  class Feature: public FoliaImpl {
    friend void static_init();
  public:
  Feature( const properties&p, Document *d = 0 ):
    FoliaImpl( p, d ){ classInit(); }
  Feature( Document *d = 0 ):
    FoliaImpl( PROPS, d ){ classInit(); }
  Feature( Document *d, const KWargs& a ):
    FoliaImpl( PROPS, d ){ classInit( a ); }
  Feature( const KWargs& a ):
    FoliaImpl( PROPS, 0 ){ classInit( a ); }

    void setAttributes( const KWargs& );
    KWargs collectAttributes() const;
    const std::string subset() const { return _subset; };
  protected:
    std::string _subset;

  private:
    static properties PROPS;
  };

  class BeginDateTimeFeature: public Feature {
    friend void static_init();
  public:
  BeginDateTimeFeature(  ):
    Feature( PROPS, 0 ){ classInit(); }
  BeginDateTimeFeature( const KWargs& a ):
    Feature( PROPS, 0 ){ classInit( a ); }
  BeginDateTimeFeature( Document *d ):
    Feature( PROPS, d ){ classInit(); }
  BeginDateTimeFeature( Document *d, const KWargs& a ):
    Feature( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == Feature_t;
    }
    static properties PROPS;
    void init();
  };

  class EndDateTimeFeature: public Feature {
    friend void static_init();
  public:
  EndDateTimeFeature(  ):
    Feature( PROPS, 0 ){ classInit(); }
  EndDateTimeFeature( const KWargs& a ):
    Feature( PROPS, 0 ){ classInit( a ); }
  EndDateTimeFeature( Document *d ):
    Feature( PROPS, d ){ classInit(); }
  EndDateTimeFeature( Document *d, const KWargs& a ):
    Feature( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == Feature_t;
    }
    static properties PROPS;
    void init();
  };

  class SynsetFeature: public Feature {
    friend void static_init();
  public:
  SynsetFeature(  ):
    Feature( PROPS, 0 ){ classInit(); }
  SynsetFeature( const KWargs& a ):
    Feature( PROPS, 0 ){ classInit( a ); }
  SynsetFeature( Document *d ):
    Feature( PROPS, d ){ classInit(); }
  SynsetFeature( Document *d, const KWargs& a ):
    Feature( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == Feature_t;
    }
    static properties PROPS;
    void init();
  };

  class ActorFeature: public Feature {
    friend void static_init();
  public:
  ActorFeature(  ):
    Feature( PROPS, 0 ){ classInit(); }
  ActorFeature( const KWargs& a ):
    Feature( PROPS, 0 ){ classInit( a ); }
  ActorFeature( Document *d ):
    Feature( PROPS, d ){ classInit(); }
  ActorFeature( Document *d, const KWargs& a ):
    Feature( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == Feature_t;
    }
    static properties PROPS;
    void init();
  };

  class HeadFeature: public Feature {
    friend void static_init();
  public:
  HeadFeature(  ):
    Feature( PROPS, 0 ){ classInit(); }
  HeadFeature( const KWargs& a ):
    Feature( PROPS, 0 ){ classInit( a ); }
  HeadFeature( Document *d ):
    Feature( PROPS, d ){ classInit(); }
  HeadFeature( Document *d, const KWargs& a ):
    Feature( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == Feature_t;
    }
    static properties PROPS;
    void init();
  };

  class ValueFeature: public Feature {
    friend void static_init();
  public:
  ValueFeature(  ):
    Feature( PROPS, 0 ){ classInit(); }
  ValueFeature( const KWargs& a ):
    Feature( PROPS, 0 ){ classInit( a ); }
  ValueFeature( Document *d ):
    Feature( PROPS, d ){ classInit(); }
  ValueFeature( Document *d, const KWargs& a ):
    Feature( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == Feature_t;
    }
    static properties PROPS;
    void init();
  };

  class FunctionFeature: public Feature {
    friend void static_init();
  public:
  FunctionFeature(  ):
    Feature( PROPS, 0 ){ classInit(); }
  FunctionFeature( const KWargs& a ):
    Feature( PROPS, 0 ){ classInit( a ); }
  FunctionFeature( Document *d ):
    Feature( PROPS, d ){ classInit(); }
  FunctionFeature( Document *d, const KWargs& a ):
    Feature( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == Feature_t;
    }
    static properties PROPS;
    void init();
  };

  class TimeFeature: public Feature {
    friend void static_init();
  public:
  TimeFeature(  ):
    Feature( PROPS, 0 ){ classInit(); }
  TimeFeature( const KWargs& a ):
    Feature( PROPS, 0 ){ classInit( a ); }
  TimeFeature( Document *d ):
    Feature( PROPS, d ){ classInit(); }
  TimeFeature( Document *d, const KWargs& a ):
    Feature( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == Feature_t;
    }
    static properties PROPS;
    void init();
  };

  class LevelFeature: public Feature {
    friend void static_init();
  public:
  LevelFeature(  ):
    Feature( PROPS, 0 ){ classInit(); }
  LevelFeature( const KWargs& a ):
    Feature( PROPS, 0 ){ classInit( a ); }
  LevelFeature( Document *d ):
    Feature( PROPS, d ){ classInit(); }
  LevelFeature( Document *d, const KWargs& a ):
    Feature( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == Feature_t;
    }
    static properties PROPS;
    void init();
  };

  class ModalityFeature: public Feature {
    friend void static_init();
  public:
  ModalityFeature(  ):
    Feature( PROPS, 0 ){ classInit(); }
  ModalityFeature( const KWargs& a ):
    Feature( PROPS, 0 ){ classInit( a ); }
  ModalityFeature( Document *d ):
    Feature( PROPS, d ){ classInit(); }
  ModalityFeature( Document *d, const KWargs& a ):
    Feature( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == Feature_t;
    }
    static properties PROPS;
    void init();
  };

  class WordReference: public FoliaImpl {
    friend void static_init();
  public:
  WordReference(  ):
    FoliaImpl( PROPS, 0 ){ classInit(); }
  WordReference( const KWargs& a ):
    FoliaImpl( PROPS, 0 ){ classInit( a ); }
  WordReference( Document *d ):
    FoliaImpl( PROPS, d ){ classInit(); }
  WordReference( Document *d, const KWargs& a ):
    FoliaImpl( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
    void init();
    FoliaElement* parseXml( const xmlNode *node );
  };

  class Alignment: public FoliaImpl {
    friend void static_init();
  public:
  Alignment(  ):
    FoliaImpl( PROPS, 0 ){ classInit(); }
  Alignment( const KWargs& a ):
    FoliaImpl( PROPS, 0 ){ classInit( a ); }
  Alignment( Document *d ):
    FoliaImpl( PROPS, d ){ classInit(); }
  Alignment( Document *d, const KWargs& a ):
    FoliaImpl( PROPS, d ){ classInit( a ); }

    std::vector<FoliaElement *>resolve() const;

  private:
    static properties PROPS;
  };

  class AlignReference: public FoliaImpl {
    friend void static_init();
    friend class Alignment;
  public:
  AlignReference(  ):
    FoliaImpl( PROPS, 0 ){ classInit(); }
  AlignReference( const KWargs& a ):
    FoliaImpl( PROPS, 0 ){ classInit( a ); }
  AlignReference( Document *d ):
    FoliaImpl( PROPS, d ){ classInit(); }
  AlignReference( Document *d, const KWargs& a ):
    FoliaImpl( PROPS, d ){ classInit( a ); }

    KWargs collectAttributes() const;
    void setAttributes( const KWargs& );

  private:
    static properties PROPS;
    FoliaElement* parseXml( const xmlNode *node );
    FoliaElement *resolve( const Alignment *ref ) const;
    std::string refId;
    std::string ref_type;
    std::string _t;
  };

  class SyntacticUnit: public AbstractSpanAnnotation {
    friend void static_init();
  public:
  SyntacticUnit(  ):
    AbstractSpanAnnotation( PROPS, 0 ){ classInit(); }
  SyntacticUnit( const KWargs& a ):
    AbstractSpanAnnotation( PROPS, 0 ){ classInit( a ); }
  SyntacticUnit( Document *d ):
    AbstractSpanAnnotation( PROPS, d ){ classInit(); }
  SyntacticUnit( Document *d, const KWargs& a ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractSpanAnnotation_t
	|| e == AbstractAnnotation_t;
    }
    static properties PROPS;
  };

  class Chunk: public AbstractSpanAnnotation {
    friend void static_init();
  public:
  Chunk(  ):
    AbstractSpanAnnotation( PROPS, 0 ){ classInit(); }
  Chunk( const KWargs& a ):
    AbstractSpanAnnotation( PROPS, 0 ){ classInit( a ); }
  Chunk( Document *d ):
    AbstractSpanAnnotation( PROPS, d ){ classInit(); }
  Chunk( Document *d, const KWargs& a ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractSpanAnnotation_t
	|| e == AbstractAnnotation_t;
    }
    static properties PROPS;
  };

  class Entity: public AbstractSpanAnnotation {
    friend void static_init();
  public:
  Entity(  ):
    AbstractSpanAnnotation( PROPS, 0 ){ classInit(); }
  Entity( const KWargs& a ):
    AbstractSpanAnnotation( PROPS, 0 ){ classInit( a ); }
  Entity( Document *d ):
    AbstractSpanAnnotation( PROPS, d ){ classInit(); }
  Entity( Document *d, const KWargs& a ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractSpanAnnotation_t
	|| e == AbstractAnnotation_t;
    }
    static properties PROPS;
  };

  class Headwords: public AbstractSpanAnnotation {
    friend void static_init();
  public:
  Headwords(  ):
    AbstractSpanAnnotation( PROPS, 0 ){ classInit(); }
  Headwords( const KWargs& a ):
    AbstractSpanAnnotation( PROPS, 0 ){ classInit( a ); }
  Headwords( Document *d ):
    AbstractSpanAnnotation( PROPS, d ){ classInit(); }
  Headwords( Document *d, const KWargs& a ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractSpanAnnotation_t
	|| e == AbstractAnnotation_t;
    }
    static properties PROPS;
  };

  class DependencyDependent: public AbstractSpanAnnotation {
    friend void static_init();
  public:
  DependencyDependent(  ):
    AbstractSpanAnnotation( PROPS, 0 ){ classInit(); }
  DependencyDependent( const KWargs& a ):
    AbstractSpanAnnotation( PROPS, 0 ){ classInit( a ); }
  DependencyDependent( Document *d ):
    AbstractSpanAnnotation( PROPS, d ){ classInit(); }
  DependencyDependent( Document *d, const KWargs& a ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractSpanAnnotation_t
	|| e == AbstractAnnotation_t;
    }
    static properties PROPS;
  };

  class Dependency: public AbstractSpanAnnotation {
    friend void static_init();
  public:
  Dependency(  ):
    AbstractSpanAnnotation( PROPS, 0 ){ classInit(); }
  Dependency( const KWargs& a ):
    AbstractSpanAnnotation( PROPS, 0 ){ classInit( a ); }
  Dependency( Document *d ):
    AbstractSpanAnnotation( PROPS, d ){ classInit(); }
  Dependency( Document *d, const KWargs& a ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( a ); }

    Headwords *head() const;
    DependencyDependent *dependent() const;
  private:
    bool has_base( ElementType e ) const {
      return e == AbstractSpanAnnotation_t
	|| e == AbstractAnnotation_t;
    }
    static properties PROPS;
  };

  class CoreferenceLink: public AbstractSpanAnnotation {
    friend void static_init();
  public:
  CoreferenceLink(  ):
    AbstractSpanAnnotation( PROPS, 0 ){ classInit(); }
  CoreferenceLink( const KWargs& a ):
    AbstractSpanAnnotation( PROPS, 0 ){ classInit( a ); }
  CoreferenceLink( Document *d ):
    AbstractSpanAnnotation( PROPS, d ){ classInit(); }
  CoreferenceLink( Document *d, const KWargs& a ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractSpanAnnotation_t;
    }
    static properties PROPS;
  };

  class CoreferenceChain: public AbstractSpanAnnotation {
    friend void static_init();
  public:
  CoreferenceChain(  ):
    AbstractSpanAnnotation( PROPS, 0 ){ classInit(); }
  CoreferenceChain( const KWargs& a ):
    AbstractSpanAnnotation( PROPS, 0 ){ classInit( a ); }
  CoreferenceChain( Document *d ):
    AbstractSpanAnnotation( PROPS, d ){ classInit(); }
  CoreferenceChain( Document *d, const KWargs& a ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( a ); }
  private:
    bool has_base( ElementType e ) const {
      return e == AbstractSpanAnnotation_t
	|| e == AbstractAnnotation_t;
    }
    static properties PROPS;
  };

  class SemanticRole: public AbstractSpanAnnotation {
    friend void static_init();
  public:
  SemanticRole(  ):
    AbstractSpanAnnotation( PROPS, 0 ){ classInit(); }
  SemanticRole( const KWargs& a ):
    AbstractSpanAnnotation( PROPS, 0 ){ classInit( a ); }
  SemanticRole( Document *d ):
    AbstractSpanAnnotation( PROPS, d ){ classInit(); }
  SemanticRole( Document *d, const KWargs& a ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractSpanAnnotation_t
	|| e == AbstractAnnotation_t;
    }
    static properties PROPS;
  };

  class AbstractAnnotationLayer:
    public FoliaImpl,
    public AllowGenerateID,
    public AllowAnnotation,
    public AllowCorrection
    {
      friend void static_init();
    protected:
      // DO NOT USE AbstractAnnotationLayer as a real node!!
    AbstractAnnotationLayer( Document *d = 0 ):
      FoliaImpl( PROPS, d ) { classInit(); };
    AbstractAnnotationLayer( const properties& props, Document *d = 0 ):
      FoliaImpl( props, d ) { classInit(); };
    AbstractAnnotationLayer( const properties& props, const KWargs& a ):
      FoliaImpl( props, 0 ) { classInit( a ); };
    AbstractAnnotationLayer( const properties& props, Document *d, const KWargs& a ):
      FoliaImpl( props, d ) { classInit( a ); };
  public:
    AbstractSpanAnnotation *findspan( const std::vector<FoliaElement*>& ) const;
    FoliaElement *append( FoliaElement * );
  private:
    static properties PROPS;
    void assignset( FoliaElement * );
  };

  class AbstractCorrectionChild: public FoliaImpl {
    friend void static_init();
  protected:
    // DO NOT USE AbstractCorrectionChild as a real node!!
  AbstractCorrectionChild( const properties& props, Document *d=0 ):
    FoliaImpl( props, d ){ classInit(); };
  AbstractCorrectionChild( Document *d=0 ):
    FoliaImpl( PROPS, d ){ classInit(); };
  private:
    static properties PROPS;
  };

  class New: public AbstractCorrectionChild {
    friend void static_init();
  public:
  New( ):
    AbstractCorrectionChild( PROPS, 0 ) { classInit(); }
  New( const KWargs& a ):
    AbstractCorrectionChild( PROPS, 0 ) { classInit( a ); }
  New( Document *d ):
    AbstractCorrectionChild( PROPS, d ) { classInit(); }
  New( Document *d, const KWargs& a ):
    AbstractCorrectionChild( PROPS, d ) { classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractCorrectionChild_t;
    }
    static properties PROPS;
  };

  class Current: public AbstractCorrectionChild {
    friend void static_init();
  public:
  Current( ):
    AbstractCorrectionChild( PROPS, 0 ) { classInit(); }
  Current( const KWargs& a ):
    AbstractCorrectionChild( PROPS, 0 ) { classInit( a ); }
  Current( Document *d ):
    AbstractCorrectionChild( PROPS, d ) { classInit(); }
  Current( Document *d, const KWargs& a ):
    AbstractCorrectionChild( PROPS, d ) { classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractCorrectionChild_t;
    }
    static properties PROPS;
  };

  class Original: public AbstractCorrectionChild {
    friend void static_init();
  public:
  Original( ):
    AbstractCorrectionChild( PROPS, 0 ) { classInit(); }
  Original( const KWargs& a ):
    AbstractCorrectionChild( PROPS, 0 ) { classInit( a ); }
  Original( Document *d ):
    AbstractCorrectionChild( PROPS, d ) { classInit(); }
  Original( Document *d, const KWargs& a ):
    AbstractCorrectionChild( PROPS, d ) { classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractCorrectionChild_t;
    }
    static properties PROPS;
    void init();
  };

  class Suggestion: public AbstractCorrectionChild {
    friend void static_init();
  public:
  Suggestion( ):
    AbstractCorrectionChild( PROPS, 0 ) { classInit(); }
  Suggestion( const KWargs& a ):
    AbstractCorrectionChild( PROPS, 0 ) { classInit( a ); }
  Suggestion( Document *d ):
    AbstractCorrectionChild( PROPS, d ) { classInit(); }
  Suggestion( Document *d, const KWargs& a ):
    AbstractCorrectionChild( PROPS, d ) { classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractCorrectionChild_t;
    }
    static properties PROPS;
    void init();
  };

  class Description: public FoliaImpl {
    friend void static_init();
  public:
  Description( ):
    FoliaImpl( PROPS, 0 ) { classInit(); }
  Description( const KWargs& a ):
    FoliaImpl( PROPS, 0 ) { classInit( a ); }
  Description( Document *d ):
    FoliaImpl( PROPS, d ) { classInit(); }
  Description( Document *d, const KWargs& a ):
    FoliaImpl( PROPS, d ) { classInit( a ); }

    const std::string description() const { return _value; };
    void setAttributes( const KWargs& kwargs );
    FoliaElement* parseXml( const xmlNode * );
    xmlNode *xml( bool, bool=false ) const;

  private:
    static properties PROPS;
    std::string _value;
  };

  class XmlComment: public FoliaImpl {
    friend void static_init();
  public:
  XmlComment( ):
    FoliaImpl( PROPS, 0 ) { classInit(); }
  XmlComment( const KWargs& a ):
    FoliaImpl( PROPS, 0 ) { classInit( a ); }
  XmlComment( Document *d ):
    FoliaImpl( PROPS, d ) { classInit(); }
  XmlComment( Document *d, const KWargs& a ):
    FoliaImpl( PROPS, d ) { classInit( a ); }

    FoliaElement* parseXml( const xmlNode * );
    xmlNode *xml( bool, bool=false ) const;
  private:
    static properties PROPS;
    std::string _value;
  };

  class XmlText: public FoliaImpl {
    friend void static_init();
  public:
  XmlText( ):
    FoliaImpl( PROPS, 0 ) { classInit(); }
  XmlText( const KWargs& a ):
    FoliaImpl( PROPS, 0 ) { classInit( a ); }
  XmlText( Document *d ):
    FoliaImpl( PROPS, d ) { classInit(); }
  XmlText( Document *d, const KWargs& a ):
    FoliaImpl(PROPS, d ) { classInit( a ); }

    FoliaElement* parseXml( const xmlNode * );
    xmlNode *xml( bool, bool=false ) const;
    bool setvalue( const std::string& s ) { _value = s; return true; };
    const std::string& getTextDelimiter( bool ) const { return EMPTY_STRING; };
    const UnicodeString text( const std::string& = "current",
			      bool = false, bool = false ) const;
  private:
    static properties PROPS;
    std::string _value; //UTF8 value
  };

  class External: public FoliaImpl {
    friend void static_init();
  public:
  External( ):
    FoliaImpl( PROPS, 0 ), _include(false) { classInit(); }
  External( const KWargs& a ):
    FoliaImpl( PROPS, 0 ), _include(false) { classInit( a ); }
  External( Document *d ):
    FoliaImpl( PROPS, d ), _include(false) { classInit(); }
  External( Document *d, const KWargs& a ):
    FoliaImpl( PROPS, d ), _include(false) { classInit( a ); }

    FoliaElement* parseXml( const xmlNode * );
    void resolve();
    void setAttributes( const KWargs& );
    KWargs collectAttributes() const;
  private:
    static properties PROPS;
    bool _include;
  };

  class Note: public AbstractStructureElement {
    friend void static_init();
  public:
  Note( ):
    AbstractStructureElement( PROPS, 0 ){ classInit(); }
  Note( const KWargs& a ):
    AbstractStructureElement( PROPS, 0 ){ classInit( a ); }
  Note( Document *d ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Note( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

    KWargs collectAttributes() const;
    void setAttributes( const KWargs& );
  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
    std::string refId;
  };

  class Definition: public AbstractStructureElement {
    friend void static_init();
  public:
  Definition( ):
    AbstractStructureElement( PROPS, 0 ){ classInit(); }
  Definition( const KWargs& a ):
    AbstractStructureElement( PROPS, 0 ){ classInit( a ); }
  Definition( Document *d ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Definition( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
  };

  class Term: public AbstractStructureElement {
    friend void static_init();
  public:
  Term( ):
    AbstractStructureElement( PROPS, 0 ){ classInit(); }
  Term( const KWargs& a ):
    AbstractStructureElement( PROPS, 0 ){ classInit( a ); }
  Term( Document *d ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Term( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
  };

  class Example: public AbstractStructureElement {
    friend void static_init();
  public:
  Example( ):
    AbstractStructureElement( PROPS, 0 ){ classInit(); }
  Example( const KWargs& a ):
    AbstractStructureElement( PROPS, 0 ){ classInit( a ); }
  Example( Document *d ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Example( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
  };

  class Entry: public AbstractStructureElement {
    friend void static_init();
  public:
  Entry( ):
    AbstractStructureElement( PROPS, 0 ){ classInit(); }
  Entry( const KWargs& a ):
    AbstractStructureElement( PROPS, 0 ){ classInit( a ); }
  Entry( Document *d ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Entry( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
  };

  class Reference: public AbstractStructureElement {
    friend void static_init();
    friend class Note;
  public:
  Reference(  ):
    AbstractStructureElement( PROPS, 0 ){ classInit(); }
  Reference( const KWargs& a ):
    AbstractStructureElement( PROPS, 0 ){ classInit( a ); }
  Reference( Document *d ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Reference( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

    KWargs collectAttributes() const;
    void setAttributes( const KWargs& );
  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
    std::string refId;
    std::string ref_type;
  };


  class Correction: public AbstractTokenAnnotation {
    friend void static_init();
  public:
  Correction( ):
    AbstractTokenAnnotation( PROPS, 0 ){ classInit(); }
  Correction( const KWargs& a ):
    AbstractTokenAnnotation( PROPS, 0 ){ classInit( a ); }
  Correction( Document *d ):
    AbstractTokenAnnotation( PROPS, d ){ classInit(); }
  Correction( Document *d, const KWargs& a ):
    AbstractTokenAnnotation( PROPS, d ){ classInit( a ); }

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
    const UnicodeString text( const std::string& = "current",
			      bool = false, bool = false ) const;
    TextContent *textcontent( const std::string& = "current" ) const;
    PhonContent *phoncontent( const std::string& = "current" ) const;
    const std::string& getTextDelimiter( bool=false) const;
  private:
    bool has_base( ElementType e ) const {
      return e == AbstractTokenAnnotation_t
	|| e == AbstractAnnotation_t;
    }
    static properties PROPS;
  };

  class ErrorDetection: public AbstractTokenAnnotation  {
    friend void static_init();
  public:
  ErrorDetection( ):
    AbstractTokenAnnotation( PROPS, 0 ){ classInit(); }
  ErrorDetection( const KWargs& a ):
    AbstractTokenAnnotation( PROPS, 0 ){ classInit( a ); }
  ErrorDetection( Document *d ):
    AbstractTokenAnnotation( PROPS, d ){ classInit(); }
  ErrorDetection( Document *d, const KWargs& a ):
    AbstractTokenAnnotation( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractTokenAnnotation_t
	|| e == AbstractAnnotation_t;
    }
    static properties PROPS;
  };

  class TimeSegment: public AbstractSpanAnnotation {
    friend void static_init();
  public:
  TimeSegment(  ):
    AbstractSpanAnnotation( PROPS, 0 ){ classInit(); }
  TimeSegment( const KWargs& a ):
    AbstractSpanAnnotation( PROPS, 0 ){ classInit( a ); }
  TimeSegment( Document *d ):
    AbstractSpanAnnotation( PROPS, d ){ classInit(); }
  TimeSegment( Document *d, const KWargs& a ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractSpanAnnotation_t
	|| e == AbstractAnnotation_t;
    }
    static properties PROPS;
  };

  class Morpheme: public AbstractStructureElement {
    friend void static_init();
  public:
  Morpheme(  ):
    AbstractStructureElement( PROPS, 0 ){ classInit(); }
  Morpheme( const KWargs& a ):
    AbstractStructureElement( PROPS, 0 ){ classInit( a ); }
  Morpheme( Document *d ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Morpheme( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
  };

  class SyntaxLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
  SyntaxLayer( ):
    AbstractAnnotationLayer( PROPS, 0 ){ classInit(); }
  SyntaxLayer( const KWargs& a ):
    AbstractAnnotationLayer( PROPS, 0 ){ classInit( a ); }
  SyntaxLayer( Document *d ):
    AbstractAnnotationLayer( PROPS, d ){ classInit(); }
  SyntaxLayer( Document *d, const KWargs& a ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractAnnotationLayer_t;
    }
    static properties PROPS;
  };

  class ChunkingLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
  ChunkingLayer(  ):
    AbstractAnnotationLayer( PROPS, 0 ){ classInit(); }
  ChunkingLayer( const KWargs& a ):
    AbstractAnnotationLayer( PROPS, 0 ){ classInit( a ); }
  ChunkingLayer( Document *d ):
    AbstractAnnotationLayer( PROPS, d ){ classInit(); }
  ChunkingLayer( Document *d, const KWargs& a ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractAnnotationLayer_t;
    }
    static properties PROPS;
  };

  class EntitiesLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
  EntitiesLayer( ):
    AbstractAnnotationLayer( PROPS, 0 ){ classInit(); }
  EntitiesLayer( const KWargs& a ):
    AbstractAnnotationLayer( PROPS, 0 ){ classInit( a ); }
  EntitiesLayer( Document *d ):
    AbstractAnnotationLayer( PROPS, d ){ classInit(); }
  EntitiesLayer( Document *d, const KWargs& a ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractAnnotationLayer_t;
    }
    static properties PROPS;
  };

  class TimingLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
  TimingLayer( ):
    AbstractAnnotationLayer( PROPS, 0 ){ classInit(); }
  TimingLayer( const KWargs& a ):
    AbstractAnnotationLayer( PROPS, 0 ){ classInit( a ); }
  TimingLayer( Document *d ):
    AbstractAnnotationLayer( PROPS, d ){ classInit(); }
  TimingLayer( Document *d, const KWargs& a ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractAnnotationLayer_t;
    }
    static properties PROPS;
  };

  class MorphologyLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
  MorphologyLayer(  ):
    AbstractAnnotationLayer( PROPS, 0 ){ classInit(); }
  MorphologyLayer( const KWargs& a ):
    AbstractAnnotationLayer( PROPS, 0 ){ classInit( a ); }
  MorphologyLayer( Document *d ):
    AbstractAnnotationLayer( PROPS, d ){ classInit(); }
  MorphologyLayer( Document *d, const KWargs& a ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractAnnotationLayer_t;
    }
    static properties PROPS;
  };

  class PhonologyLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
  PhonologyLayer(  ):
    AbstractAnnotationLayer( PROPS, 0 ){ classInit(); }
  PhonologyLayer( const KWargs& a ):
    AbstractAnnotationLayer( PROPS, 0 ){ classInit( a ); }
  PhonologyLayer( Document *d ):
    AbstractAnnotationLayer( PROPS, d ){ classInit(); }
  PhonologyLayer( Document *d, const KWargs& a ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractAnnotationLayer_t;
    }
    static properties PROPS;
  };

  class DependenciesLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
  DependenciesLayer( ):
    AbstractAnnotationLayer( PROPS, 0 ){ classInit(); }
  DependenciesLayer( const KWargs& a ):
    AbstractAnnotationLayer( PROPS, 0 ){ classInit( a ); }
  DependenciesLayer( Document *d ):
    AbstractAnnotationLayer( PROPS, d ){ classInit(); }
  DependenciesLayer( Document *d, const KWargs& a ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractAnnotationLayer_t;
    }
    static properties PROPS;
  };

  class CoreferenceLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
  CoreferenceLayer( ):
    AbstractAnnotationLayer( PROPS, 0 ){ classInit(); }
  CoreferenceLayer( const KWargs& a ):
    AbstractAnnotationLayer( PROPS, 0 ){ classInit( a ); }
  CoreferenceLayer( Document *d ):
    AbstractAnnotationLayer( PROPS, d ){ classInit(); }
  CoreferenceLayer( Document *d, const KWargs& a ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractAnnotationLayer_t;
    }
    static properties PROPS;
  };

  class SemanticRolesLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
  SemanticRolesLayer( ):
    AbstractAnnotationLayer( PROPS, 0 ){ classInit(); }
  SemanticRolesLayer( const KWargs& a ):
    AbstractAnnotationLayer( PROPS, 0 ){ classInit( a ); }
  SemanticRolesLayer( Document *d ):
    AbstractAnnotationLayer( PROPS, d ){ classInit(); }
  SemanticRolesLayer( Document *d, const KWargs& a ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( a ); }
  private:
    bool has_base( ElementType e ) const {
      return e == AbstractAnnotationLayer_t;
    }
    static properties PROPS;
  };

  class ComplexAlignmentLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
  ComplexAlignmentLayer( ):
    AbstractAnnotationLayer( PROPS, 0 ){ classInit(); }
  ComplexAlignmentLayer( const KWargs& a ):
    AbstractAnnotationLayer( PROPS, 0 ){ classInit( a ); }
  ComplexAlignmentLayer( Document *d  ):
    AbstractAnnotationLayer( PROPS, d ){ classInit(); }
  ComplexAlignmentLayer( Document *d, const KWargs& a ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractAnnotationLayer_t;
    }
    static properties PROPS;
  };


  std::string VersionName();
  std::string Version();

} // namespace folia

#endif
