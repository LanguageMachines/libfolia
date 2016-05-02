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

    virtual void assignDoc( Document* ) = 0;
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
	res = new F( args, doc() );
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
    const std::string& classname() const { return xmltag(); }; //synomym
    virtual const std::string& default_subset() const = 0;
    virtual const std::string subset() const NOT_IMPLEMENTED;
    virtual bool setonly() const = 0;
    virtual bool auto_generate_id() const = 0;
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
    static FoliaElement *createElement( ElementType, Document * =0 );
    static FoliaElement *createElement( const std::string&, Document * =0 );
    virtual ~FoliaImpl();

    void classInit(){
      // we could call 'init()' directly, but this is more esthetic
      // keep in balance with the next function
      init(); // virtual init
    }

    void classInit( const KWargs& a ){
      // this funcion is needed because calling the virtual function
      // setAttributes from the constuctor will NOT call the right version
      // THIS IS BY DESIGN.
      init(); // virtual init
      setAttributes( a ); // also virtual!
    }

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

    void assignDoc( Document* );
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
    size_t occurrences() const;
    size_t occurrences_per_set() const;
    Attrib required_attributes() const;
    Attrib optional_attributes() const;
    const std::string& xmltag() const;
    const std::string& default_subset() const;
    AnnotationType::AnnotationType annotation_type() const;
    const std::set<ElementType>& accepted_data() const;
    const std::set<ElementType>& required_data() const;
    bool printable() const;
    bool speakable() const;
    bool xlink() const;
    bool auth() const;
    bool setonly() const;
    bool auto_generate_id() const;

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
    static FoliaElement *private_createElement( ElementType );
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

  class AbstractTokenAnnotation:
    public FoliaImpl,
    public AllowGenerateID
    {
      friend void static_init();
    protected:
    // DO NOT USE AbstractTokenAnnotation as a real node!!
    AbstractTokenAnnotation( const properties& props, Document *d=0 ):
      FoliaImpl( props, d ){ classInit(); };
    AbstractTokenAnnotation( Document *d=0 ):
      FoliaImpl( PROPS, d ){ classInit(); };

    private:
      static properties PROPS;
    };


  class AbstractSpanAnnotation:
    public FoliaImpl,
    public AllowGenerateID,
    public AllowAnnotation,
    public AllowCorrection
    {
      friend void static_init();
    protected:
      // DO NOT USE AbstractSpanAnnotation as a real node!!
    AbstractSpanAnnotation( const properties& props, Document *d=0 ):
      FoliaImpl( props, d ){ classInit(); };
    AbstractSpanAnnotation( Document *d=0 ):
      FoliaImpl( PROPS, d ){ classInit(); };
    public:
      xmlNode *xml( bool, bool=false ) const;
      FoliaElement *append( FoliaElement* );

      std::vector<FoliaElement*> wrefs() const;
      FoliaElement *wrefs( size_t ) const;

    private:
      static properties PROPS;
    };

  class AbstractExtendedTokenAnnotation: public AbstractTokenAnnotation
    {
      friend void static_init();
    protected:
    // DO NOT USE AbstractExtendedTokenAnnotation as a real node!!
    AbstractExtendedTokenAnnotation( const properties& props, Document *d=0 ):
      AbstractTokenAnnotation( props, d ){ classInit(); };
    AbstractExtendedTokenAnnotation( Document *d=0 ):
      AbstractTokenAnnotation( PROPS, d ){ classInit(); };

    private:
      static properties PROPS;
    };

  class ComplexAlignment: public FoliaImpl {
    friend void static_init();
  public:
  ComplexAlignment( Document *d=0 ):
    FoliaImpl( PROPS, d ){ classInit(); }
  ComplexAlignment( const KWargs& a, Document *d=0 ):
    FoliaImpl( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };


  const std::string EMPTY_STRING = "";

  class AbstractTextMarkup: public FoliaImpl {
    friend void static_init();
  protected:
    // DO NOT USE AbstractTextMarkup as a real node!!
  AbstractTextMarkup( const properties& props, Document *d=0 ):
    FoliaImpl( props, d ){ classInit(); };
  AbstractTextMarkup( Document *d=0 ):
    FoliaImpl( PROPS, d ){ classInit(); };
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
    static properties PROPS;
  };

  class TextMarkupGap: public AbstractTextMarkup {
    friend void static_init();
  public:
  TextMarkupGap( Document *d=0 ):
    AbstractTextMarkup( PROPS, d ) { classInit(); };
  private:
    static properties PROPS;
  };

  class TextMarkupString: public AbstractTextMarkup {
    friend void static_init();
  public:
  TextMarkupString( Document *d=0 ):
    AbstractTextMarkup( PROPS, d ) { classInit(); };
  private:
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
    static properties PROPS;
    std::string _original;
  };

  class TextMarkupError: public AbstractTextMarkup {
    friend void static_init();
  public:
  TextMarkupError( Document *d=0 ):
    AbstractTextMarkup( PROPS, d ){ classInit(); };
  private:
    static properties PROPS;
  };

  class TextMarkupStyle: public AbstractTextMarkup {
    friend void static_init();
  public:
  TextMarkupStyle( Document *d=0 ):
    AbstractTextMarkup( PROPS, d ){ classInit(); };
  private:
    static properties PROPS;
  };

  class TextContent: public FoliaImpl {
    friend void static_init();
  public:
  TextContent( Document *d = 0 ):
    FoliaImpl( PROPS, d ){ classInit(); }
  TextContent( const KWargs& a, Document *d=0 ):
    FoliaImpl( PROPS, d ){ classInit(a); }
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
  PhonContent( const KWargs& a, Document *d = 0 ):
    FoliaImpl(PROPS,d){ classInit( a ); }
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
  FoLiA( Document *d = 0 ):
    FoliaImpl( PROPS,d) { classInit(); }
  FoLiA( const KWargs& a, Document *d = 0 ):
    FoliaImpl( PROPS, d ) { classInit( a ); }

  private:
    static properties PROPS;
  };

  class DCOI: public FoliaImpl {
    friend void static_init();
  public:
  DCOI( Document *d = 0 ):
    FoliaImpl( PROPS, d ) { classInit( ); }
  DCOI( const KWargs& a, Document *d = 0 ):
    FoliaImpl( PROPS, d ) { classInit( a ); }

  private:
    static properties PROPS;
  };

  class Head: public AbstractStructureElement {
    friend void static_init();
  public:
  Head( Document *d = 0 ):
    AbstractStructureElement( PROPS, d ) {  classInit(); }
  Head( const KWargs& a, Document *d = 0 ):
    AbstractStructureElement( PROPS, d ) {  classInit( a ); }

  private:
    static properties PROPS;
  };

  class TableHead: public AbstractStructureElement {
    friend void static_init();
  public:
  TableHead( Document *d = 0 ):
    AbstractStructureElement( PROPS, d ) {  classInit( ); }
  TableHead( const KWargs& a, Document *d = 0 ):
    AbstractStructureElement( PROPS, d ) {  classInit( a ); }

  private:
    static properties PROPS;
  };

  class Table: public AbstractStructureElement {
    friend void static_init();
  public:
  Table( Document *d = 0 ):
    AbstractStructureElement( PROPS, d ) {  classInit(); };
  Table( const KWargs& a, Document *d = 0  ):
    AbstractStructureElement( PROPS, d ) {  classInit( a ); };
  private:
    static properties PROPS;
  };

  class Row: public AbstractStructureElement {
    friend void static_init();
  public:
  Row( Document *d = 0 ):
    AbstractStructureElement( PROPS, d ) {  classInit(); };
  Row( const KWargs& a, Document *d = 0 ):
    AbstractStructureElement( PROPS, d ) {  classInit( a ); };
  private:
    static properties PROPS;
  };

  class Cell: public AbstractStructureElement {
    friend void static_init();
  public:
  Cell( Document *d = 0 ):
    AbstractStructureElement( PROPS, d ) {  classInit(); };
  Cell( const KWargs& a, Document *d ):
    AbstractStructureElement( PROPS, d ) {  classInit( a ); };
  private:
    static properties PROPS;
  };

  class Gap: public FoliaImpl {
    friend void static_init();
  public:
  Gap( Document *d = 0 ):
    FoliaImpl( PROPS, d ) { classInit(); }
  Gap( const KWargs& a, Document *d = 0 ):
    FoliaImpl( PROPS, d ) { classInit( a ); }

    const std::string content() const;
  private:
    static properties PROPS;
  };

  class Content: public FoliaImpl {
    friend void static_init();
  public:
  Content( Document *d = 0 ):
    FoliaImpl( PROPS, d ) { classInit(); }
  Content( const KWargs& a, Document *d = 0 ):
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
  Metric( Document *d = 0 ):
    FoliaImpl( PROPS, d ) { classInit(); }
  Metric( const KWargs& a, Document *d = 0 ):
    FoliaImpl( PROPS, d ) { classInit( a ); }

  private:
    static properties PROPS;
  };

  class Division: public AbstractStructureElement {
    friend void static_init();
  public:
  Division( Document *d=0 ):
    AbstractStructureElement( PROPS, d ) { classInit(); };
  Division( const KWargs& a, Document *d = 0 ):
    AbstractStructureElement( PROPS, d ) { classInit( a ); };
    Head *head() const;
  private:
    static properties PROPS;
  };

  class Linebreak: public AbstractStructureElement {
    friend void static_init();
  public:
  Linebreak( Document *d=0 ):
    AbstractStructureElement( PROPS, d ){ classInit(); };
  Linebreak( const KWargs& a, Document *d = 0 ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); };
    const UnicodeString text( const std::string& = "current",
			      bool = false, bool = false ) const {
      return "";
    }
    void setAttributes( const KWargs& );
    KWargs collectAttributes() const;
  private:
    void init();
    static properties PROPS;
    std::string _pagenr;
    std::string _linenr;
    bool _newpage;
  };

  class Whitespace: public AbstractStructureElement {
    friend void static_init();
  public:
  Whitespace( Document *d=0 ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Whitespace( const KWargs& a, Document *d = 0 ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class Word: public AbstractStructureElement {
    friend void static_init();
  public:
  Word( Document *d = 0 ):
    AbstractStructureElement(PROPS, d ) { classInit(); }
  Word( const properties& p, Document *d=0 ):
    AbstractStructureElement( p, d ) { classInit(); }
  Word( const KWargs& a,  Document *d = 0 ):
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
    void init();
    static properties PROPS;
    bool _space;
  };

  class Part: public AbstractStructureElement {
    friend void static_init();
  public:
  Part( Document *d=0 ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Part( const KWargs& a, Document *d = 0 ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class String:
    public AbstractTokenAnnotation,
    public AllowAnnotation,
    public AllowCorrection {
    friend void static_init();
  public:
  String( Document *d=0 ):
    AbstractTokenAnnotation( PROPS, d ){ classInit(); }
  String( const KWargs& a, Document *d =0 ):
    AbstractTokenAnnotation( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
    };

  class PlaceHolder: public Word {
    friend void static_init();
  public:
  PlaceHolder( Document *d=0 ):
    Word( PROPS, d ){ classInit(); }
  PlaceHolder( const KWargs& a, Document *d = 0 ):
    Word( PROPS, d ){ classInit( a ); }

    void setAttributes( const KWargs& );
  private:
    static properties PROPS;
  };

  class Sentence: public AbstractStructureElement {
    friend void static_init();
  public:
  Sentence( Document *d=0 ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Sentence( const KWargs& a, Document *d =0 ):
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
    static properties PROPS;
    Correction *correctWords( const std::vector<FoliaElement *>&,
			      const std::vector<FoliaElement *>&,
			      const KWargs& );
  };

  class Speech: public AbstractStructureElement {
    friend void static_init();
  public:
  Speech( Document *d=0 ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Speech( const KWargs& a, Document *d = 0 ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class Text: public AbstractStructureElement {
    friend void static_init();
  public:
  Text( Document *d=0 ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Text( const KWargs& a, Document *d = 0 ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class Utterance: public AbstractStructureElement {
    friend void static_init();
  public:
  Utterance( Document *d=0 ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Utterance( const KWargs& a, Document *d = 0 ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class Event: public AbstractStructureElement {
    friend void static_init();
  public:
  Event( Document *d=0 ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Event( const KWargs& a, Document *d = 0 ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class Caption: public AbstractStructureElement {
    friend void static_init();
  public:
  Caption( Document *d=0 ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Caption( const KWargs& a, Document *d = 0 ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class Label: public AbstractStructureElement {
    friend void static_init();
  public:
  Label( Document *d=0 ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Label( const KWargs& a, Document *d = 0 ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }
  private:
    static properties PROPS;
  };

  class ListItem: public AbstractStructureElement {
    friend void static_init();
  public:
  ListItem( Document *d=0 ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  ListItem( const KWargs& a, Document *d = 0 ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class List: public AbstractStructureElement {
    friend void static_init();
  public:
  List( Document *d=0 ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  List( const KWargs& a, Document *d = 0 ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class Figure: public AbstractStructureElement {
    friend void static_init();
  public:
  Figure( Document *d=0 ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Figure( const KWargs& a, Document *d = 0 ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

    const std::string src() const { return _src; };
    const UnicodeString caption() const;
  private:
    static properties PROPS;
  };

  class Paragraph: public AbstractStructureElement {
    friend void static_init();
  public:
  Paragraph( Document *d=0 ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Paragraph( const KWargs& a, Document *d = 0 ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class Alternative: public AbstractStructureElement {
    friend void static_init();
  public:
  Alternative( Document *d=0 ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Alternative( const KWargs& a, Document *d = 0 ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };


  class AlternativeLayers: public FoliaImpl {
    friend void static_init();
  public:
  AlternativeLayers( Document *d = 0 ):
    FoliaImpl( PROPS, d ){ classInit(); }
  AlternativeLayers( const KWargs& a, Document *d = 0 ):
    FoliaImpl( PROPS, d ){ classInit( a ); }

  private:
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
      alt = new Alternative( kw, doc() );
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
  PosAnnotation( Document *d=0 ):
    AbstractTokenAnnotation( PROPS, d ){ classInit(); }
  PosAnnotation( const KWargs& a, Document *d = 0 ):
    AbstractTokenAnnotation( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class LemmaAnnotation: public AbstractTokenAnnotation {
    friend void static_init();
  public:
  LemmaAnnotation( Document *d=0 ):
    AbstractTokenAnnotation( PROPS, d ){ classInit(); }
  LemmaAnnotation( const KWargs& a, Document *d = 0 ):
    AbstractTokenAnnotation( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class LangAnnotation: public AbstractExtendedTokenAnnotation {
    friend void static_init();
  public:
  LangAnnotation( Document *d=0 ):
    AbstractExtendedTokenAnnotation( PROPS, d ){ classInit(); }
  LangAnnotation( const KWargs& a, Document *d = 0 ):
    AbstractExtendedTokenAnnotation( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class Phoneme: public AbstractTokenAnnotation {
    friend void static_init();
  public:
  Phoneme( Document *d=0 ):
    AbstractTokenAnnotation( PROPS, d ){ classInit(); }
  Phoneme( const KWargs& a, Document *d = 0 ):
    AbstractTokenAnnotation( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class DomainAnnotation: public AbstractExtendedTokenAnnotation {
    friend void static_init();
  public:
  DomainAnnotation( Document *d=0 ):
    AbstractExtendedTokenAnnotation( PROPS, d ){ classInit(); }
  DomainAnnotation( const KWargs& a, Document *d = 0 ):
    AbstractExtendedTokenAnnotation( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class SenseAnnotation: public AbstractTokenAnnotation {
    friend void static_init();
  public:
  SenseAnnotation( Document *d=0 ):
    AbstractTokenAnnotation( PROPS, d ){ classInit(); }
  SenseAnnotation( const KWargs& a, Document *d = 0 ):
    AbstractTokenAnnotation( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class SubjectivityAnnotation: public AbstractTokenAnnotation {
    friend void static_init();
  public:
  SubjectivityAnnotation( Document *d=0 ):
    AbstractTokenAnnotation( PROPS, d ){ classInit(); }
  SubjectivityAnnotation( const KWargs& a, Document *d = 0 ):
    AbstractTokenAnnotation( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class Quote: public AbstractStructureElement {
    friend void static_init();
  public:
  Quote( Document *d=0 ):
    AbstractStructureElement( PROPS, d ){ classInit(); };
  Quote( const KWargs& a, Document *d = 0 ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); };
    FoliaElement *append( FoliaElement *);
    std::vector<Word*> wordParts() const;
    const std::string& getTextDelimiter( bool=false) const;
  private:
    static properties PROPS;
  };

  class Feature: public FoliaImpl {
    friend void static_init();
  public:
  Feature( const properties&p, Document *d = 0 ):
    FoliaImpl( p, d ){ classInit(); }
  Feature( Document *d = 0 ):
    FoliaImpl( PROPS, d ){ classInit(); }
  Feature( const KWargs& a, Document *d = 0 ):
    FoliaImpl( PROPS, d ){ classInit( a ); }

    void setAttributes( const KWargs& );
    KWargs collectAttributes() const;
    const std::string subset() const { return _subset; };

  protected:
    std::string _subset;

  private:
    static properties PROPS;
  };

  class BegindatetimeFeature: public Feature {
    friend void static_init();
  public:
  BegindatetimeFeature( Document *d=0 ):
    Feature( PROPS, d ){ classInit(); }
  BegindatetimeFeature( const KWargs& a, Document *d = 0 ):
    Feature( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class EnddatetimeFeature: public Feature {
    friend void static_init();
  public:
  EnddatetimeFeature( Document *d=0 ):
    Feature( PROPS, d ){ classInit(); }
  EnddatetimeFeature( const KWargs& a, Document *d = 0 ):
    Feature( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class SynsetFeature: public Feature {
    friend void static_init();
  public:
  SynsetFeature( Document *d=0 ):
    Feature( PROPS, d ){ classInit(); }
  SynsetFeature( const KWargs& a, Document *d = 0 ):
    Feature( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class ActorFeature: public Feature {
    friend void static_init();
  public:
  ActorFeature( Document *d=0 ):
    Feature( PROPS, d ){ classInit(); }
  ActorFeature( const KWargs& a, Document *d = 0 ):
    Feature( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class HeadFeature: public Feature {
    friend void static_init();
  public:
  HeadFeature( Document *d=0 ):
    Feature( PROPS, d ){ classInit(); }
  HeadFeature( const KWargs& a, Document *d = 0 ):
    Feature( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class ValueFeature: public Feature {
    friend void static_init();
  public:
  ValueFeature( Document *d=0 ):
    Feature( PROPS, d ){ classInit(); }
  ValueFeature( const KWargs& a, Document *d = 0 ):
    Feature( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class FunctionFeature: public Feature {
    friend void static_init();
  public:
  FunctionFeature( Document *d=0 ):
    Feature( PROPS, d ){ classInit(); }
  FunctionFeature( const KWargs& a, Document *d = 0 ):
    Feature( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class TimeFeature: public Feature {
    friend void static_init();
  public:
  TimeFeature( Document *d=0 ):
    Feature( PROPS, d ){ classInit(); }
  TimeFeature( const KWargs& a, Document *d = 0 ):
    Feature( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class LevelFeature: public Feature {
    friend void static_init();
  public:
  LevelFeature( Document *d=0 ):
    Feature( PROPS, d ){ classInit(); }
  LevelFeature( const KWargs& a, Document *d = 0 ):
    Feature( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class ModalityFeature: public Feature {
    friend void static_init();
  public:
  ModalityFeature( Document *d=0 ):
    Feature( PROPS, d ){ classInit(); }
  ModalityFeature( const KWargs& a, Document *d = 0 ):
    Feature( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class StyleFeature: public Feature {
    friend void static_init();
  public:
  StyleFeature( Document *d=0 ):
    Feature( PROPS, d ){ classInit(); }
  StyleFeature( const KWargs& a, Document *d = 0 ):
    Feature( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class WordReference: public FoliaImpl {
    friend void static_init();
  public:
  WordReference( Document *d = 0 ):
    FoliaImpl( PROPS, d ){ classInit(); }
  WordReference( const KWargs& a, Document *d = 0 ):
    FoliaImpl( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
    FoliaElement* parseXml( const xmlNode *node );
  };

  class Alignment: public FoliaImpl {
    friend void static_init();
  public:
  Alignment( Document *d = 0 ):
    FoliaImpl( PROPS, d ){ classInit(); }
  Alignment( const KWargs& a, Document *d = 0 ):
    FoliaImpl( PROPS, d ){ classInit( a ); }

    std::vector<FoliaElement *>resolve() const;
    void setAttributes( const KWargs& );
    KWargs collectAttributes() const;
  private:
    void init();
    static properties PROPS;
    std::string _format;
  };

  class AlignReference: public FoliaImpl {
    friend void static_init();
    friend class Alignment;
  public:
  AlignReference( Document *d = 0 ):
    FoliaImpl( PROPS, d ){ classInit(); }
  AlignReference( const KWargs& a, Document *d = 0 ):
    FoliaImpl( PROPS, d ){ classInit( a ); }

    KWargs collectAttributes() const;
    void setAttributes( const KWargs& );

  private:
    static properties PROPS;
    FoliaElement* parseXml( const xmlNode *node );
    FoliaElement *resolve_element( const Alignment *ref ) const;
    std::string refId;
    std::string ref_type;
    std::string _t;
  };

  class SyntacticUnit: public AbstractSpanAnnotation {
    friend void static_init();
  public:
  SyntacticUnit( Document *d = 0 ):
    AbstractSpanAnnotation( PROPS, d ){ classInit(); }
  SyntacticUnit( const KWargs& a, Document *d = 0 ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class Chunk: public AbstractSpanAnnotation {
    friend void static_init();
  public:
  Chunk( Document *d=0 ):
    AbstractSpanAnnotation( PROPS, d ){ classInit(); }
  Chunk( const KWargs& a, Document *d = 0 ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class Entity: public AbstractSpanAnnotation {
    friend void static_init();
  public:
  Entity( Document *d=0 ):
    AbstractSpanAnnotation( PROPS, d ){ classInit(); }
  Entity( const KWargs& a, Document *d = 0 ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

    class AbstractSpanRole: public AbstractSpanAnnotation {
    friend void static_init();
  public:
  AbstractSpanRole( const properties& props, Document *d=0 ):
    AbstractSpanAnnotation( props, d ){ classInit(); }
  AbstractSpanRole( Document *d=0 ):
    AbstractSpanAnnotation( PROPS, d ){ classInit(); }

  private:
    static properties PROPS;
  };

  class Headspan: public AbstractSpanRole {
    friend void static_init();
  public:
  Headspan( Document *d=0 ):
    AbstractSpanRole( PROPS, d ){ classInit(); }
  Headspan( const KWargs& a, Document *d = 0 ):
    AbstractSpanRole( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class DependencyDependent: public AbstractSpanRole {
    friend void static_init();
  public:
  DependencyDependent( Document *d=0 ):
    AbstractSpanRole( PROPS, d ){ classInit(); }
  DependencyDependent( const KWargs& a, Document *d = 0 ):
    AbstractSpanRole( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class Dependency: public AbstractSpanAnnotation {
    friend void static_init();
  public:
  Dependency( Document *d=0 ):
    AbstractSpanAnnotation( PROPS, d ){ classInit(); }
  Dependency( const KWargs& a, Document *d = 0 ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( a ); }

    Headspan *head() const;
    DependencyDependent *dependent() const;
  private:
    static properties PROPS;
  };

  class CoreferenceLink: public AbstractSpanRole {
    friend void static_init();
  public:
  CoreferenceLink( Document *d=0 ):
    AbstractSpanRole( PROPS, d ){ classInit(); }
  CoreferenceLink( const KWargs& a, Document *d = 0 ):
    AbstractSpanRole( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class CoreferenceChain: public AbstractSpanAnnotation {
    friend void static_init();
  public:
  CoreferenceChain( Document *d=0 ):
    AbstractSpanAnnotation( PROPS, d ){ classInit(); }
  CoreferenceChain( const KWargs& a, Document *d = 0 ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class SemanticRole: public AbstractSpanAnnotation {
    friend void static_init();
  public:
  SemanticRole( Document *d=0 ):
    AbstractSpanAnnotation( PROPS, d ){ classInit(); }
  SemanticRole( const KWargs& a, Document *d = 0 ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( a ); }

  private:
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
    AbstractAnnotationLayer( const properties& props, const KWargs& a, Document *d = 0 ):
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
  New( Document *d=0 ):
    AbstractCorrectionChild( PROPS, d ) { classInit(); }
  New( const KWargs& a, Document *d = 0 ):
    AbstractCorrectionChild( PROPS, d ) { classInit( a ); }

  private:
    static properties PROPS;
  };

  class Current: public AbstractCorrectionChild {
    friend void static_init();
  public:
  Current( Document *d=0 ):
    AbstractCorrectionChild( PROPS, d ) { classInit(); }
  Current( const KWargs& a, Document *d = 0 ):
    AbstractCorrectionChild( PROPS, d ) { classInit( a ); }

  private:
    static properties PROPS;
  };

  class Original: public AbstractCorrectionChild {
    friend void static_init();
  public:
  Original( Document *d=0 ):
    AbstractCorrectionChild( PROPS, d ) { classInit(); }
  Original( const KWargs& a, Document *d = 0 ):
    AbstractCorrectionChild( PROPS, d ) { classInit( a ); }

  private:
    static properties PROPS;
  };

  class Suggestion: public AbstractCorrectionChild {
    friend void static_init();
  public:
  Suggestion( Document *d=0 ):
    AbstractCorrectionChild( PROPS, d ) { classInit(); }
  Suggestion( const KWargs& a, Document *d = 0 ):
    AbstractCorrectionChild( PROPS, d ) { classInit( a ); }

  private:
    static properties PROPS;
  };

  class Description: public FoliaImpl {
    friend void static_init();
  public:
  Description( Document *d=0 ):
    FoliaImpl( PROPS, d ) { classInit(); }
  Description( const KWargs& a, Document *d =0 ):
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
  XmlComment( Document *d=0 ):
    FoliaImpl( PROPS, d ) { classInit(); }
  XmlComment( const KWargs& a, Document *d = 0 ):
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
  XmlText( Document *d = 0 ):
    FoliaImpl( PROPS, d ) { classInit(); }
  XmlText( const KWargs& a, Document *d = 0 ):
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
  External( Document *d = 0 ):
    FoliaImpl( PROPS, d ), _include(false) { classInit(); }
  External( const KWargs& a, Document *d = 0 ):
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
  Note( Document *d=0 ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Note( const KWargs& a, Document *d = 0 ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

    KWargs collectAttributes() const;
    void setAttributes( const KWargs& );
  private:
    static properties PROPS;
    std::string refId;
  };

  class Definition: public AbstractStructureElement {
    friend void static_init();
  public:
  Definition( Document *d=0 ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Definition( const KWargs& a, Document *d = 0 ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class Term: public AbstractStructureElement {
    friend void static_init();
  public:
  Term( Document *d=0 ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Term( const KWargs& a, Document *d = 0 ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class Example: public AbstractStructureElement {
    friend void static_init();
  public:
  Example( Document *d=0 ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Example( const KWargs& a, Document *d = 0 ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class Entry: public AbstractStructureElement {
    friend void static_init();
  public:
  Entry( Document *d=0 ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Entry( const KWargs& a, Document *d = 0 ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class Reference: public AbstractStructureElement {
    friend void static_init();
    friend class Note;
  public:
  Reference( Document *d=0 ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Reference( const KWargs& a, Document *d = 0 ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

    KWargs collectAttributes() const;
    void setAttributes( const KWargs& );
  private:
    static properties PROPS;
    std::string refId;
    std::string ref_type;
  };


  class Correction: public AbstractTokenAnnotation {
    friend void static_init();
  public:
  Correction( Document *d=0 ):
    AbstractTokenAnnotation( PROPS, d ){ classInit(); }
  Correction( const KWargs& a, Document *d = 0 ):
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
    static properties PROPS;
  };

  class ErrorDetection: public AbstractExtendedTokenAnnotation  {
    friend void static_init();
  public:
  ErrorDetection( Document *d=0 ):
    AbstractExtendedTokenAnnotation( PROPS, d ){ classInit(); }
  ErrorDetection( const KWargs& a, Document *d = 0 ):
    AbstractExtendedTokenAnnotation( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class TimeSegment: public AbstractSpanAnnotation {
    friend void static_init();
  public:
  TimeSegment( Document *d=0 ):
    AbstractSpanAnnotation( PROPS, d ){ classInit(); }
  TimeSegment( const KWargs& a, Document *d = 0 ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class Morpheme: public AbstractStructureElement {
    friend void static_init();
  public:
  Morpheme( Document *d=0 ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Morpheme( const KWargs& a, Document *d = 0 ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class SyntaxLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
  SyntaxLayer( Document *d=0 ):
    AbstractAnnotationLayer( PROPS, d ){ classInit(); }
  SyntaxLayer( const KWargs& a, Document *d = 0 ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class ChunkingLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
  ChunkingLayer( Document *d=0 ):
    AbstractAnnotationLayer( PROPS, d ){ classInit(); }
  ChunkingLayer( const KWargs& a, Document *d = 0 ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class EntitiesLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
  EntitiesLayer( Document *d=0 ):
    AbstractAnnotationLayer( PROPS, d ){ classInit(); }
  EntitiesLayer( const KWargs& a, Document *d = 0 ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class TimingLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
  TimingLayer( Document *d=0 ):
    AbstractAnnotationLayer( PROPS, d ){ classInit(); }
  TimingLayer( const KWargs& a, Document *d = 0 ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class MorphologyLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
  MorphologyLayer( Document *d=0 ):
    AbstractAnnotationLayer( PROPS, d ){ classInit(); }
  MorphologyLayer( const KWargs& a, Document *d = 0 ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class PhonologyLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
  PhonologyLayer( Document *d=0 ):
    AbstractAnnotationLayer( PROPS, d ){ classInit(); }
  PhonologyLayer( const KWargs& a, Document *d = 0 ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class DependenciesLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
  DependenciesLayer( Document *d=0 ):
    AbstractAnnotationLayer( PROPS, d ){ classInit(); }
  DependenciesLayer( const KWargs& a, Document *d = 0 ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class CoreferenceLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
  CoreferenceLayer( Document *d=0 ):
    AbstractAnnotationLayer( PROPS, d ){ classInit(); }
  CoreferenceLayer( const KWargs& a, Document *d = 0 ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class SemanticRolesLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
  SemanticRolesLayer( Document *d=0 ):
    AbstractAnnotationLayer( PROPS, d ){ classInit(); }
  SemanticRolesLayer( const KWargs& a, Document *d = 0 ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( a ); }
  private:
    static properties PROPS;
  };

  class ComplexAlignmentLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
  ComplexAlignmentLayer( Document *d=0  ):
    AbstractAnnotationLayer( PROPS, d ){ classInit(); }
  ComplexAlignmentLayer( const KWargs& a, Document *d = 0 ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };


  std::string VersionName();
  std::string Version();

} // namespace folia

#endif
