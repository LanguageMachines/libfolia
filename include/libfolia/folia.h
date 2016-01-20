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

#ifndef FOLIA_H
#define FOLIA_H

#include <set>
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <exception>
#include "unicode/unistr.h"
#include "libxml/tree.h"
//
//the following are duplicate headers, they are also in cxx files but included here as well to facilitate use from Cython
#include "libfolia/properties.h"
#include "libfolia/foliautils.h"

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
  class NewElement;
  class Original;
  class Current;
  class Suggestion;
  class Division;
  class DependencyDependent;
  class Paragraph;
  class Morpheme;

  extern const std::set<ElementType> default_ignore;
  extern const std::set<ElementType> default_ignore_annotations;
  extern const std::set<ElementType> default_ignore_structure;

#define NOT_IMPLEMENTED {						\
    throw NotImplementedError( xmltag() + "::" + std::string(__func__) ); \
  }

  class FoliaElement {
    friend std::ostream& operator<<( std::ostream&, const FoliaElement& );
    friend std::ostream& operator<<( std::ostream&, const FoliaElement* );
    friend bool operator==( const FoliaElement&, const FoliaElement& );
  public:
    virtual ~FoliaElement(){};
    virtual void init()=0;

    virtual size_t size() const = 0;
    virtual FoliaElement* index( size_t ) const = 0;
    virtual FoliaElement* rindex( size_t ) const = 0;
    FoliaElement* operator[]( size_t i ) const {
      return index(i);
    }

    virtual bool isinstance( ElementType et ) const = 0;
    template <typename F>
      bool isinstance() const {
      F obj("");
      return element_id() == obj.element_id();
    }
    bool isSubClass( ElementType ) const;
    bool isSubClass( const FoliaElement *c ) const {
      return isSubClass( c->element_id() );
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
      F obj(0);
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
      F obj(0);
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
      F obj(0);
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
      F obj(0);
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
      F obj(0);
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
    virtual std::string annotator( ) const = 0;
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
	  F obj(0);
	  if ( s.empty() )
	    throw NoSuchAnnotation( obj.classname() );
	  else
	    throw NoSuchAnnotation( obj.classname() + " for set='" + s + "'" );
	}
      }
      else NOT_IMPLEMENTED;
    }

    template <typename F>
      bool hasannotation( const std::string& st = "" ) const {
      try {
	std::vector<F*> v = annotations<F>( st );
	return true;
      }
      catch ( NoSuchAnnotation& e ){
	return false;
      }
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
      F obj("");
      return findspans( obj.element_id(), st );
    }
    virtual AbstractSpanAnnotation *findspan( const std::vector<FoliaElement*>& ) const NOT_IMPLEMENTED;

    // features
    virtual std::vector<std::string> feats( const std::string& ) const = 0;
    virtual std::string feat( const std::string& ) const = 0;

    //XML (de)serialisation
    virtual FoliaElement* parseXml( const xmlNode * ) = 0;
    std::string xmlstring() const; // serialize to a string (XML fragment)
    virtual xmlNode *xml( bool, bool = false ) const = 0; //serialize to XML

    // text/string content
    bool hastext( const std::string& = "current" ) const;

    virtual std::string str( const std::string& = "current" ) const = 0;
    UnicodeString unicode( const std::string& cls = "current" ) const { return text( cls ); };
    virtual UnicodeString text( const std::string& = "current",
				bool = false, bool = false ) const = 0;
    UnicodeString stricttext( const std::string& = "current" ) const;
    UnicodeString toktext( const std::string& = "current" ) const;
    virtual UnicodeString phon( const std::string& = "current",
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
    virtual NewElement *getNew() const NOT_IMPLEMENTED;
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
    virtual std::string getTextDelimiter( bool retaintok=false ) const = 0;
    virtual void setDateTime( const std::string& ) = 0;
    virtual std::string getDateTime() const = 0;
    virtual std::string pos( const std::string& = "" ) const = 0;
    virtual std::string lemma( const std::string& = "" ) const = 0;
    virtual std::string cls() const = 0;
    virtual std::string sett() const = 0;
    virtual std::string classname() const = 0;
    virtual std::string n() const = 0;
    virtual std::string id() const = 0;
    virtual std::string begintime() const = 0;
    virtual std::string endtime() const = 0;
    virtual std::string speech_src() const = 0;
    virtual std::string speech_speaker() const = 0;
    virtual double confidence() const = 0;
    virtual void confidence( double ) = 0;
    virtual ElementType element_id() const = 0;
    virtual size_t occurrences() const = 0;
    virtual size_t occurrences_per_set() const = 0;
    virtual Attrib required_attributes() const = 0;
    virtual std::string xmltag() const = 0;
    virtual std::string subset() const NOT_IMPLEMENTED;

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

    virtual std::string description() const;

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

    virtual std::string content() const {
      throw NoSuchAnnotation( "content" );
    }
    virtual std::string src() const NOT_IMPLEMENTED;
    virtual UnicodeString caption() const NOT_IMPLEMENTED;
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
    virtual std::string generateId( const std::string& ) NOT_IMPLEMENTED;
  };

  class FoliaImpl: public virtual FoliaElement {
  public:
    //Constructor
    FoliaImpl( const properties& p, Document* = 0 );
    // static element Constructor
    static FoliaElement *createElement( Document *, const std::string&  );
    virtual ~FoliaImpl();

    void classInit( const std::string& s="" ){
      init(); // virtual init
      if ( !s.empty() ){
	// this enables the init of empty classes, which hopefully get their
	// attributes in a later state
	setAttributes(  getArgs( s ) );
      }
    }

    void classInit( const KWargs& a ){
      init(); // virtual init
      setAttributes( a ); // also virtual!
    }

    //functions regarding contained data
    size_t size() const { return data.size(); };
    FoliaElement* index( size_t ) const;
    FoliaElement* rindex( size_t ) const;

    bool isinstance( ElementType et ) const {
      return et == _element_id;
    }
    template <typename F>
      bool isinstance() const {
      F obj("");
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

    std::string annotator( ) const { return _annotator; };
    void annotator( const std::string& a ) { _annotator = a; };
    AnnotatorType annotatortype() const { return _annotator_type; };
    void annotatortype( AnnotatorType t ) { _annotator_type =  t; };
    AnnotationType::AnnotationType annotation_type() const { return _annotation_type; };

    template <typename F>
      F *addAnnotation( const KWargs& args ) {
      return FoliaElement::addAnnotation<F>( args );
    }

    template <typename F>
      std::vector<F*> annotations( const std::string& s = "" ) const {
      return FoliaElement::annotations<F>( s );
    }

    template <typename F>
      bool hasannotation( const std::string& st = "" ) const {
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
    std::string feat( const std::string& ) const;

    //XML (de)serialisation
    FoliaElement* parseXml( const xmlNode * );
    xmlNode *xml( bool, bool = false ) const;

    // text/string content

    std::string str( const std::string& = "current" ) const;
    UnicodeString text( const std::string& = "current",
			bool = false, bool = false ) const;

    UnicodeString phon( const std::string& = "current",
			bool = false ) const;
    UnicodeString deeptext( const std::string& = "current",
			    bool = false ) const;
    UnicodeString deepphon( const std::string& = "current" ) const;

    bool printable() const { return PRINTABLE; };
    bool speakable() const { return SPEAKABLE; };

    // Word
    const Word* resolveword( const std::string& ) const { return 0; };
    Word *addWord( const KWargs& );

    // TextContent
    TextContent *textcontent( const std::string& = "current" ) const;
    // PhonContent
    PhonContent *phoncontent( const std::string& = "current" ) const;

    // properties
    std::string getTextDelimiter( bool retaintok=false ) const;
    void setDateTime( const std::string& );
    std::string getDateTime() const;
    std::string pos( const std::string& = "" ) const ;
    std::string lemma( const std::string& = "" ) const;
    std::string cls() const { return _class; };
    std::string sett() const { return _set; };
    std::string classname() const { return toString(_element_id); };
    std::string n() const { return _n; };
    std::string id() const { return _id; };
    std::string href() const { return _href; };
    std::string begintime() const { return _begintime; };
    std::string endtime() const { return _endtime; };
    std::string speech_src() const;
    std::string speech_speaker() const;

    double confidence() const { return _confidence; };
    void confidence( double d ) { _confidence = d; };
    ElementType element_id() const { return _props._element_id; };
    size_t occurrences() const { return _occurrences; };
    size_t occurrences_per_set() const { return _occurrences_per_set; };
    Attrib required_attributes() const { return _required_attributes; };
    std::string xmltag() const { return _xmltag; };

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
    std::string _xmltag;
    ElementType _element_id;
    std::set<ElementType> _accepted_data;
    AnnotationType::AnnotationType _annotation_type;
    std::string TEXTDELIMITER;
    size_t _occurrences;
    size_t _occurrences_per_set;
    bool PRINTABLE;
    bool SPEAKABLE;
    bool XLINK;
    Attrib _required_attributes;
    Attrib _optional_attributes;
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

  inline bool isSubClass( const FoliaElement *e1, const FoliaElement *e2 ){
    return isSubClass( e1->element_id(), e2->element_id() );
  }

  template <typename T1, typename T2>
    bool isSubClass(){
    T1 t1(0);
    T2 t2(0);
    return t1.isSubClass( &t2 );
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

  inline std::string str( const FoliaElement *e,
			  const std::string& cls = "current" ) {
    return e->str( cls ); }

  inline UnicodeString text( const FoliaElement *e,
			     const std::string& cls = "current" ) {
    if ( e )
      return e->text( cls );
    else
      throw ValueError( "text() for empty element" );
  }

  inline UnicodeString unicode( const FoliaElement *e ) {
    return e->unicode(); }

  inline bool isinstance( const FoliaElement *e, ElementType t ) {
    return e->isinstance( t ); }

  class AllowGenerateID: public virtual FoliaElement {
  public:
    void setMaxId( FoliaElement * );
    int getMaxId( const std::string& );
    std::string generateId( const std::string& tag ){
      return IDgen( tag, this );
    }
  private:
    std::string IDgen( const std::string&, const FoliaElement* );
    std::map<std::string, int> maxid;
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

  class AbstractStructureElement: public FoliaImpl,
    public AllowGenerateID,
    public AllowAnnotation,
    public AllowCorrection {
  public:
    AbstractStructureElement( const properties& props, Document *d=0 ):
      FoliaImpl( props, d ){ classInit(); };


      std::string str( const std::string& = "current" ) const;
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
      void init();
    };

  class AbstractAnnotation: public FoliaImpl {
  public:
  AbstractAnnotation( const properties& props, Document *d=0 ):
    FoliaImpl( props, d ){};
  };

  class AbstractTokenAnnotation:
    public AbstractAnnotation,
    public AllowGenerateID {
  public:
  AbstractTokenAnnotation( const properties& props, Document *d=0 ):
      AbstractAnnotation( props, d ){ classInit(); };
  private:
      void init();
    };

  class Feature: public FoliaImpl {
    friend void static_init();
  public:
  Feature( const std::string& s=""): FoliaImpl(PROPS){ classInit( s ); }
  Feature( const properties& p, const std::string& s=""): FoliaImpl(p){ classInit( s ); }
  Feature( const KWargs& a ): FoliaImpl(PROPS){ classInit( a ); }
  Feature( Document *d, const std::string& s=""): FoliaImpl(PROPS,d){ classInit( s ); }
  Feature( const properties&p, Document *d, const std::string& s=""): FoliaImpl(p,d){ classInit( s ); }
  Feature( Document *d, const KWargs& a ): FoliaImpl(PROPS,d){ classInit( a ); }
    void setAttributes( const KWargs& );
    KWargs collectAttributes() const;
    std::string subset() const { return _subset; };
  protected:
    std::string _subset;

  private:
    static properties PROPS;
    void init();
  };

  class AbstractSpanAnnotation: public AbstractAnnotation,
    public AllowGenerateID,
    public AllowAnnotation,
    public AllowCorrection
    {
  public:
    AbstractSpanAnnotation( const properties& props, Document *d=0 ):
      AbstractAnnotation( props, d ){ classInit(); };
      xmlNode *xml( bool, bool=false ) const;
      FoliaElement *append( FoliaElement* );

      std::vector<FoliaElement*> wrefs() const;
      FoliaElement *wrefs( size_t ) const;

    private:
      void init();
    };

  class AbstractTextMarkup: public AbstractAnnotation {
  public:
  AbstractTextMarkup( const properties& props, Document *d=0 ):
    AbstractAnnotation( props, d ){ classInit(); };
    void setAttributes( const KWargs& );
    KWargs collectAttributes() const;
    const FoliaElement* resolveid() const;
    UnicodeString text( const std::string& = "current",
			bool = false, bool = false ) const;
  protected:
    std::string getTextDelimiter( bool ) const { return ""; };
    std::string idref;
  private:
    void init();
  };

  class TextMarkupGap: public AbstractTextMarkup {
    friend void static_init();
  public:
  TextMarkupGap( Document *d=0 ):
    AbstractTextMarkup( PROPS, d ) { classInit(); };
  private:
    static properties PROPS;
    void init();
  };

  class TextMarkupString: public AbstractTextMarkup {
    friend void static_init();
  public:
  TextMarkupString( Document *d=0 ):
    AbstractTextMarkup( PROPS, d ) { classInit(); };
  private:
    static properties PROPS;
    void init();
  };

  class TextMarkupCorrection: public AbstractTextMarkup {
    friend void static_init();
  public:
  TextMarkupCorrection( Document *d=0 ):
    AbstractTextMarkup( PROPS, d ){ classInit(); };
    void setAttributes( const KWargs& );
    KWargs collectAttributes() const;
    UnicodeString text( const std::string& = "current",
			bool = false, bool = false ) const;
  private:
    static properties PROPS;
    void init();
    std::string _original;
  };

  class TextMarkupError: public AbstractTextMarkup {
    friend void static_init();
  public:
  TextMarkupError( Document *d=0 ):
    AbstractTextMarkup( PROPS, d ){ classInit(); };
  private:
    static properties PROPS;
    void init();
  };

  class TextMarkupStyle: public AbstractTextMarkup {
    friend void static_init();
  public:
  TextMarkupStyle( Document *d=0 ):
    AbstractTextMarkup( PROPS, d ){ classInit(); };
  private:
    static properties PROPS;
    void init();
  };

  class TextContent: public FoliaImpl {
    friend void static_init();
  public:
  TextContent( const std::string& s="" ): FoliaImpl(PROPS){ classInit( s ); }
  TextContent( const KWargs& a ): FoliaImpl(PROPS){ classInit( a ); }
  TextContent( Document *d, const std::string& s="" ): FoliaImpl(PROPS,d){ classInit( s ); }
  TextContent( Document *d, const KWargs& a ): FoliaImpl(PROPS,d){ classInit( a ); }
    void setAttributes( const KWargs& );
    KWargs collectAttributes() const;
    std::string str( const std::string& = "current" ) const;
    UnicodeString text( const std::string& = "current",
			bool = false, bool = false ) const;
    int offset() const { return _offset; };
    TextContent *postappend();
    std::vector<FoliaElement*> findreplacables( FoliaElement * ) const;
  private:
    static properties PROPS;
    void init();
    int _offset;
  };

  class PhonContent: public FoliaImpl {
    friend void static_init();
  public:
  PhonContent( const std::string& s="" ): FoliaImpl(PROPS){ classInit( s ); }
  PhonContent( const KWargs& a ): FoliaImpl(PROPS){ classInit( a ); }
  PhonContent( Document *d, const std::string& s="" ): FoliaImpl(PROPS,d){ classInit( s ); }
  PhonContent( Document *d, const KWargs& a ): FoliaImpl(PROPS,d){ classInit( a ); }
    void setAttributes( const KWargs& );
    KWargs collectAttributes() const;
    UnicodeString phon( const std::string& = "current",
			bool = false ) const;
    int offset() const { return _offset; };
    /* TextContent *postappend(); */
    /* std::vector<FoliaElement*> findreplacables( FoliaElement * ) const; */
  private:
    static properties PROPS;
    void init();
    int _offset;
  };

  class FoLiA: public FoliaImpl {
    friend void static_init();
  public:
  FoLiA( const std::string& s="" ): FoliaImpl(PROPS) { classInit( s ); };
  FoLiA( const KWargs& a ): FoliaImpl(PROPS) { classInit( a ); };
  FoLiA( Document *d, const std::string& s=""): FoliaImpl(PROPS,d) { classInit( s ); };
  FoLiA( Document *d, const KWargs& a ): FoliaImpl(PROPS, d)
      { classInit( a ); };
  private:
    static properties PROPS;
    void init();
  };

  class DCOI: public FoliaImpl {
    friend void static_init();
  public:
  DCOI( const std::string& s=""): FoliaImpl(PROPS) { classInit( s ); };
  DCOI( const KWargs& a ): FoliaImpl(PROPS) { classInit( a ); };
  DCOI( Document *d, const std::string& s=""): FoliaImpl(PROPS, d) { classInit( s ); };
  DCOI( Document *d, const KWargs& a ): FoliaImpl(PROPS, d) { classInit( a ); };
  private:
    static properties PROPS;
    void init();
  };

  class Head: public AbstractStructureElement {
    friend void static_init();
  public:
  Head( const std::string& s=""):
    AbstractStructureElement( PROPS ) {  classInit( s ); };
  Head( const KWargs& a ):
    AbstractStructureElement( PROPS ) {  classInit( a ); };
  Head( Document *d, const std::string& s=""):
    AbstractStructureElement( PROPS, d ) {  classInit( s ); };
  Head( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ) {  classInit( a ); };
  private:
    static properties PROPS;
    void init();
  };

  class TableHead: public AbstractStructureElement {
    friend void static_init();
  public:
  TableHead( const std::string& s=""):
    AbstractStructureElement( PROPS ) {  classInit( s ); };
  TableHead( const KWargs& a ):
    AbstractStructureElement( PROPS ) {  classInit( a ); };
  TableHead( Document *d, const std::string& s=""):
    AbstractStructureElement( PROPS, d ) {  classInit( s ); };
  TableHead( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ) {  classInit( a ); };
  private:
    static properties PROPS;
    void init();
  };

  class Table: public AbstractStructureElement {
    friend void static_init();
  public:
  Table( const std::string& s=""):
    AbstractStructureElement( PROPS ) {  classInit( s ); };
  Table( const KWargs& a ):
    AbstractStructureElement( PROPS ) {  classInit( a ); };
  Table( Document *d, const std::string& s=""):
    AbstractStructureElement( PROPS, d ) {  classInit( s ); };
  Table( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ) {  classInit( a ); };
  private:
    static properties PROPS;
    void init();
  };

  class Row: public AbstractStructureElement {
    friend void static_init();
  public:
  Row( const std::string& s=""):
    AbstractStructureElement( PROPS ) {  classInit( s ); };
  Row( const KWargs& a ):
    AbstractStructureElement( PROPS ) {  classInit( a ); };
  Row( Document *d, const std::string& s=""):
    AbstractStructureElement( PROPS, d ) {  classInit( s ); };
  Row( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ) {  classInit( a ); };
  private:
    static properties PROPS;
    void init();
  };

  class Cell: public AbstractStructureElement {
    friend void static_init();
  public:
  Cell( const std::string& s=""):
    AbstractStructureElement( PROPS ) {  classInit( s ); };
  Cell( const KWargs& a ):
    AbstractStructureElement( PROPS ) {  classInit( a ); };
  Cell( Document *d, const std::string& s=""):
    AbstractStructureElement( PROPS, d ) {  classInit( s ); };
  Cell( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ) {  classInit( a ); };
  private:
    static properties PROPS;
    void init();
  };

  class Gap: public FoliaImpl {
    friend void static_init();
  public:
  Gap( const std::string& s=""):
    FoliaImpl(PROPS) { classInit( s ); };
  Gap( const KWargs& a ):
    FoliaImpl(PROPS) { classInit( a ); };
  Gap( Document *d, const std::string& s=""):
    FoliaImpl(PROPS,d) { classInit( s ); };
  Gap( Document *d, const KWargs& a ):
    FoliaImpl(PROPS,d) { classInit( a ); };
    std::string content() const;
  private:
    static properties PROPS;
    void init();
  };

  class Content: public FoliaImpl {
    friend void static_init();
  public:
  Content( const std::string& s=""):
    FoliaImpl(PROPS) { classInit( s ); };
  Content( const KWargs& a ):
    FoliaImpl(PROPS) { classInit( a ); };
  Content( Document *d, const std::string& s=""):
    FoliaImpl(PROPS,d) { classInit( s ); };
  Content( Document *d, const KWargs& a ):
    FoliaImpl(PROPS,d) { classInit( a ); };
    FoliaElement* parseXml( const xmlNode * );
    xmlNode *xml( bool, bool = false ) const;
    std::string content() const { return value; };
  private:
    static properties PROPS;
    void init();
    std::string value;
  };

  class MetricAnnotation: public FoliaImpl {
    friend void static_init();
  public:
  MetricAnnotation( const std::string& s=""):
    FoliaImpl(PROPS) { classInit( s ); };
  MetricAnnotation( const KWargs& a ):
    FoliaImpl(PROPS) { classInit( a ); };
  MetricAnnotation( Document *d, const std::string& s=""):
    FoliaImpl(PROPS,d) { classInit( s ); };
  MetricAnnotation( Document *d, const KWargs& a ):
    FoliaImpl(PROPS,d) { classInit( a ); };
  private:
    static properties PROPS;
    void init();
  };

  class Division: public AbstractStructureElement {
    friend void static_init();
  public:
  Division( const std::string& s=""):
    AbstractStructureElement( PROPS ) { classInit( s ); };
  Division( const KWargs& a ):
    AbstractStructureElement( PROPS ) { classInit( a ); };
  Division( Document *d, const std::string& s=""):
    AbstractStructureElement( PROPS, d ) { classInit( s ); };
  Division( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ) { classInit( a ); };
    Head *head() const;
  private:
    static properties PROPS;
    void init();
  };

  class LineBreak: public AbstractStructureElement {
    friend void static_init();
  public:
  LineBreak( const std::string& s=""):
    AbstractStructureElement( PROPS ){ classInit( s ); };
  LineBreak( const KWargs& a ):
    AbstractStructureElement( PROPS ){ classInit( a ); };
  LineBreak( Document *d,  const std::string& s=""):
    AbstractStructureElement( PROPS, d ){ classInit( s ); };
  LineBreak( Document *d,  const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); };
    UnicodeString text( const std::string& = "current",
			bool = false, bool = false ) const {
      return "";
    }
  private:
    static properties PROPS;
    void init();
  };

  class WhiteSpace: public AbstractStructureElement {
    friend void static_init();
  public:
  WhiteSpace( const std::string& s=""):
    AbstractStructureElement( PROPS ){ classInit( s ); };
  WhiteSpace( const KWargs& a ):
    AbstractStructureElement( PROPS ){ classInit( a ); };
  WhiteSpace( Document *d,  const std::string& s=""):
    AbstractStructureElement( PROPS, d ){ classInit( s ); };
  WhiteSpace( Document *d,  const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); };
  private:
    static properties PROPS;
    void init();
  };

  class Word: public AbstractStructureElement {
    friend void static_init();
  public:
  Word( const std::string& s="" ):
    AbstractStructureElement( PROPS ){ classInit( s ); };
  Word( const properties& p, const std::string& s="" ):
    AbstractStructureElement( p ){ classInit( s ); };
  Word( const KWargs& a ):
    AbstractStructureElement( PROPS ){ classInit( a ); };
  Word( Document *d, const std::string& s=""):
    AbstractStructureElement( PROPS, d ){ classInit( s ); };
  Word( const properties& p, Document *d, const std::string& s=""):
    AbstractStructureElement( p, d ){ classInit( s ); };
  Word( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); };
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
    std::string getTextDelimiter( bool=false) const;
    MorphologyLayer *addMorphologyLayer( const KWargs& );
    MorphologyLayer *getMorphologyLayers( const std::string&,
					  std::vector<MorphologyLayer*>& ) const;
  private:
    static properties PROPS;
    void init();
    bool space;
  };

  class Part: public AbstractStructureElement {
    friend void static_init();
  public:
  Part( const std::string& s="" ):
    AbstractStructureElement( PROPS ){ classInit( s ); };
  Part( const KWargs& a ):
    AbstractStructureElement( PROPS ){ classInit( a ); };
  Part( Document *d, const std::string& s=""):
    AbstractStructureElement( PROPS, d ){ classInit( s ); };
  Part( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); };
  private:
    static properties PROPS;
    void init();
  };

  class String:
    public AbstractTokenAnnotation,
    public AllowAnnotation,
    public AllowCorrection {
    friend void static_init();
  public:
  String( const std::string& s="" ):
      AbstractTokenAnnotation( PROPS ){ classInit( s ); };
  String( const KWargs& a ):
      AbstractTokenAnnotation( PROPS ){ classInit( a ); };
  String( Document *d, const std::string& s=""):
      AbstractTokenAnnotation( PROPS, d ){ classInit( s ); };
  String( Document *d, const KWargs& a ):
      AbstractTokenAnnotation( PROPS, d ){ classInit( a ); };
  private:
      static properties PROPS;
      void init();
    };

  class PlaceHolder: public Word {
    friend void static_init();
  public:
  PlaceHolder( const std::string& s=""):
    Word( PROPS ){ classInit( s ); };
  PlaceHolder( const KWargs& a ):
    Word( PROPS ){ classInit( a ); };
  PlaceHolder( Document *d, const std::string& s=""):
    Word( PROPS, d ){ classInit( s ); };
  PlaceHolder( Document *d, const KWargs& a ):
    Word( PROPS, d ){ classInit( a ); };
    void setAttributes( const KWargs& );
  private:
    static properties PROPS;
    void init();
  };

  class Sentence: public AbstractStructureElement {
    friend void static_init();
  public:
  Sentence( const std::string& s=""):
    AbstractStructureElement( PROPS ){ classInit( s ); }
  Sentence( const KWargs& a ):
    AbstractStructureElement( PROPS ){ classInit( a ); }
  Sentence( Document *d, const std::string& s=""):
    AbstractStructureElement( PROPS, d ){ classInit( s ); }
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
    static properties PROPS;
    Correction *correctWords( const std::vector<FoliaElement *>&,
			      const std::vector<FoliaElement *>&,
			      const KWargs& );
    void init();
  };

  class Speech: public AbstractStructureElement {
    friend void static_init();
  public:
  Speech( const std::string& s=""):
    AbstractStructureElement( PROPS ){ classInit( s ); };
  Speech( const KWargs& a ):
    AbstractStructureElement( PROPS ){ classInit( a ); };
  Speech( Document *d, const std::string& s=""):
    AbstractStructureElement( PROPS, d ){ classInit( s ); };
  Speech( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); };
  private:
    static properties PROPS;
    void init();
  };

  class Text: public AbstractStructureElement {
    friend void static_init();
  public:
  Text( const std::string& s=""):
    AbstractStructureElement( PROPS ){ classInit( s ); };
  Text( const KWargs& a ):
    AbstractStructureElement( PROPS ){ classInit( a ); };
  Text( Document *d, const std::string& s=""):
    AbstractStructureElement( PROPS, d ){ classInit( s ); };
  Text( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); };
  private:
    static properties PROPS;
    void init();
  };

  class Utterance: public AbstractStructureElement {
    friend void static_init();
  public:
  Utterance( const std::string& s=""):
    AbstractStructureElement( PROPS ){ classInit( s ); };
  Utterance( const KWargs& a ):
    AbstractStructureElement( PROPS ){ classInit( a ); };
  Utterance( Document *d, const std::string& s=""):
    AbstractStructureElement( PROPS, d ){ classInit( s ); };
  Utterance( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); };
  private:
    static properties PROPS;
    void init();
  };

  class Event: public AbstractStructureElement {
    friend void static_init();
  public:
  Event( const std::string& s=""):
    AbstractStructureElement( PROPS ){ classInit( s ); };
  Event( const KWargs& a ):
    AbstractStructureElement( PROPS ){ classInit( a ); };
  Event( Document *d, const std::string& s=""):
    AbstractStructureElement( PROPS, d ){ classInit( s ); };
  Event( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); };
  private:
    static properties PROPS;
    void init();
  };

  class Caption: public AbstractStructureElement {
    friend void static_init();
  public:
  Caption( const std::string& s="" ):
    AbstractStructureElement( PROPS ){ classInit( s ); };
  Caption( const KWargs& a ):
    AbstractStructureElement( PROPS ){ classInit( a ); };
  Caption( Document *d, const std::string& s=""):
    AbstractStructureElement( PROPS, d ){ classInit( s ); };
  Caption( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); };
  private:
    static properties PROPS;
    void init();
  };

  class Label: public AbstractStructureElement {
    friend void static_init();
  public:
  Label( const std::string& s=""):
    AbstractStructureElement( PROPS ){ classInit( s ); };
  Label( const KWargs& a ):
    AbstractStructureElement( PROPS ){ classInit( a ); };
  Label( Document *d, const std::string& s="" ):
    AbstractStructureElement( PROPS, d ){ classInit( s ); };
  Label( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); };
  private:
    static properties PROPS;
    void init();
  };

  class Item: public AbstractStructureElement {
    friend void static_init();
  public:
  Item( const std::string& s=""):
    AbstractStructureElement( PROPS ){ classInit( s ); };
  Item( const KWargs& a ):
    AbstractStructureElement( PROPS ){ classInit( a ); };
  Item( Document *d, const std::string& s=""):
    AbstractStructureElement( PROPS, d ){ classInit( s ); };
  Item( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); };
  private:
    static properties PROPS;
    void init();
  };

  class List: public AbstractStructureElement {
    friend void static_init();
  public:
  List( const std::string& s=""):
    AbstractStructureElement( PROPS ){ classInit( s ); };
  List( const KWargs& a ):
    AbstractStructureElement( PROPS ){ classInit( a ); };
  List( Document *d, const std::string& s=""):
    AbstractStructureElement( PROPS, d ){ classInit( s ); };
  List( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); };
  private:
    static properties PROPS;
    void init();
  };

  class Figure: public AbstractStructureElement {
    friend void static_init();
  public:
  Figure( const std::string& s=""):
    AbstractStructureElement( PROPS ){ classInit( s ); };
  Figure( const KWargs& a ):
    AbstractStructureElement( PROPS ){ classInit( a ); };
  Figure( Document *d, const std::string& s="" ):
    AbstractStructureElement( PROPS, d ){ classInit( s ); };
  Figure( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); };
    std::string src() const { return _src; };
    UnicodeString caption() const;
  private:
    static properties PROPS;
    void init();
  };

  class Paragraph: public AbstractStructureElement {
    friend void static_init();
  public:
  Paragraph( const std::string& s=""):
    AbstractStructureElement( PROPS ){ classInit( s ); };
  Paragraph( const KWargs& a ):
    AbstractStructureElement( PROPS ){ classInit( a ); };
  Paragraph( Document *d, const std::string& s=""):
    AbstractStructureElement( PROPS, d ){ classInit( s ); };
  Paragraph( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); };
  private:
    static properties PROPS;
    void init();
  };

  class Alternative: public AbstractStructureElement {
    friend void static_init();
  public:
  Alternative( const std::string& s=""):
    AbstractStructureElement( PROPS ){ classInit( s ); };
  Alternative( const KWargs& a ):
    AbstractStructureElement( PROPS ){ classInit( a ); };
  Alternative( Document *d, const std::string& s=""):
    AbstractStructureElement( PROPS, d ){ classInit( s ); };
  Alternative( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); };
  private:
    static properties PROPS;
    void init();
  };


  class AlternativeLayers: public FoliaImpl {
    friend void static_init();
  public:
  AlternativeLayers( const std::string& s=""):
    FoliaImpl(PROPS){ classInit( s ); };
  AlternativeLayers( const KWargs& a ):
    FoliaImpl(PROPS){ classInit( a ); };
  AlternativeLayers( Document *d, const std::string& s=""):
    FoliaImpl(PROPS,d){ classInit( s ); };
  AlternativeLayers( Document *d, const KWargs& a ):
    FoliaImpl(PROPS,d){ classInit( a ); };
  private:
    static properties PROPS;
    void init();
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
  PosAnnotation( const std::string& s=""):
    AbstractTokenAnnotation( PROPS ){ classInit( s ); };
  PosAnnotation( const KWargs& a ):
    AbstractTokenAnnotation( PROPS ){ classInit( a ); };
  PosAnnotation( Document *d, const std::string& s=""):
    AbstractTokenAnnotation( PROPS, d ){ classInit( s ); };
  PosAnnotation( Document *d, const KWargs& a ):
    AbstractTokenAnnotation( PROPS, d ){ classInit( a ); };
  private:
    static properties PROPS;
    void init();
  };

  class LemmaAnnotation: public AbstractTokenAnnotation {
    friend void static_init();
  public:
  LemmaAnnotation( const std::string& s=""):
    AbstractTokenAnnotation( PROPS ){ classInit( s ); };
  LemmaAnnotation( const KWargs& a ):
    AbstractTokenAnnotation( PROPS ){ classInit( a ); };
  LemmaAnnotation( Document *d, const std::string& s=""):
    AbstractTokenAnnotation( PROPS, d ){ classInit( s ); };
  LemmaAnnotation( Document *d, const KWargs& a ):
    AbstractTokenAnnotation( PROPS, d ){ classInit( a ); };
  private:
    static properties PROPS;
    void init();
  };

  class LangAnnotation: public AbstractTokenAnnotation {
    friend void static_init();
  public:
  LangAnnotation( const std::string& s=""):
    AbstractTokenAnnotation( PROPS ){ classInit( s ); };
  LangAnnotation( const KWargs& a ):
    AbstractTokenAnnotation( PROPS ){ classInit( a ); };
  LangAnnotation( Document *d, const std::string& s=""):
    AbstractTokenAnnotation( PROPS, d ){ classInit( s ); };
  LangAnnotation( Document *d, const KWargs& a ):
    AbstractTokenAnnotation( PROPS, d ){ classInit( a ); };
  private:
    static properties PROPS;
    void init();
  };

  class Phoneme: public AbstractTokenAnnotation {
    friend void static_init();
  public:
  Phoneme( const std::string& s=""):
    AbstractTokenAnnotation( PROPS ){ classInit( s ); };
  Phoneme( const KWargs& a ):
    AbstractTokenAnnotation( PROPS ){ classInit( a ); };
  Phoneme( Document *d, const std::string& s=""):
    AbstractTokenAnnotation( PROPS, d ){ classInit( s ); };
  Phoneme( Document *d, const KWargs& a ):
    AbstractTokenAnnotation( PROPS, d ){ classInit( a ); };
  private:
    static properties PROPS;
    void init();
  };

  class DomainAnnotation: public AbstractTokenAnnotation {
    friend void static_init();
  public:
  DomainAnnotation(  const std::string& s=""):
    AbstractTokenAnnotation( PROPS ){ classInit( s ); };
  DomainAnnotation(  const KWargs& a ):
    AbstractTokenAnnotation( PROPS ){ classInit( a ); };
  DomainAnnotation( Document *d, const std::string& s=""):
    AbstractTokenAnnotation( PROPS, d ){ classInit( s ); };
  DomainAnnotation( Document *d, const KWargs& a ):
    AbstractTokenAnnotation( PROPS, d ){ classInit( a ); };
  private:
    static properties PROPS;
    void init();
  };

  class SenseAnnotation: public AbstractTokenAnnotation {
    friend void static_init();
  public:
  SenseAnnotation( const std::string& s="" ):
    AbstractTokenAnnotation( PROPS ){ classInit( s ); };
  SenseAnnotation( const KWargs& a ):
    AbstractTokenAnnotation( PROPS ){ classInit( a ); };
  SenseAnnotation( Document *d, const std::string& s="" ):
    AbstractTokenAnnotation( PROPS, d ){ classInit( s ); };
  SenseAnnotation( Document *d, const KWargs& a ):
    AbstractTokenAnnotation( PROPS, d ){ classInit( a ); };
  private:
    static properties PROPS;
    void init();
  };

  class SubjectivityAnnotation: public AbstractTokenAnnotation {
    friend void static_init();
  public:
  SubjectivityAnnotation( const std::string& s="" ):
    AbstractTokenAnnotation( PROPS ){ classInit( s ); };
  SubjectivityAnnotation( const KWargs& a ):
    AbstractTokenAnnotation( PROPS ){ classInit( a ); };
  SubjectivityAnnotation( Document *d, const std::string& s="" ):
    AbstractTokenAnnotation( PROPS, d ){ classInit( s ); };
  SubjectivityAnnotation( Document *d, const KWargs& a ):
    AbstractTokenAnnotation( PROPS, d ){ classInit( a ); };
  private:
    static properties PROPS;
    void init();
  };

  class Quote: public AbstractStructureElement {
    friend void static_init();
  public:
  Quote( const std::string& s="" ):
    AbstractStructureElement( PROPS ){ classInit( s ); };
  Quote( const KWargs& a ):
    AbstractStructureElement( PROPS ){ classInit( a ); };
  Quote( Document *d, const std::string& s="" ):
    AbstractStructureElement( PROPS, d ){ classInit( s ); };
  Quote( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); };
    FoliaElement *append( FoliaElement *);
    std::vector<Word*> wordParts() const;
    std::string getTextDelimiter( bool=false) const;
  private:
    static properties PROPS;
    void init();
  };

  class BeginDateTimeFeature: public Feature {
    friend void static_init();
  public:
  BeginDateTimeFeature( const std::string& s="" ):
    Feature( PROPS ){ classInit( s ); }
  BeginDateTimeFeature( const KWargs& a ):
    Feature( PROPS ){ classInit( a ); }
  BeginDateTimeFeature( Document *d, const std::string& s="" ):
    Feature( PROPS, d ){ classInit( s ); }
  BeginDateTimeFeature( Document *d, const KWargs& a ):
    Feature( PROPS, d ){ classInit( a ); }
  private:
    static properties PROPS;
    void init();
  };

  class EndDateTimeFeature: public Feature {
    friend void static_init();
  public:
  EndDateTimeFeature( const std::string& s="" ):
    Feature( PROPS ){ classInit( s ); }
  EndDateTimeFeature( const KWargs& a ):
    Feature( PROPS ){ classInit( a ); }
  EndDateTimeFeature( Document *d, const std::string& s="" ):
    Feature( PROPS, d ){ classInit( s ); }
  EndDateTimeFeature( Document *d, const KWargs& a ):
    Feature( PROPS, d ){ classInit( a ); }
  private:
    static properties PROPS;
    void init();
  };

  class SynsetFeature: public Feature {
    friend void static_init();
  public:
  SynsetFeature( const std::string& s="" ):
    Feature( PROPS ){ classInit( s ); }
  SynsetFeature( const KWargs& a ):
    Feature( PROPS ){ classInit( a ); }
  SynsetFeature( Document *d, const std::string& s="" ):
    Feature( PROPS, d ){ classInit( s ); }
  SynsetFeature( Document *d, const KWargs& a ):
    Feature( PROPS, d ){ classInit( a ); }
  private:
    static properties PROPS;
    void init();
  };

  class ActorFeature: public Feature {
    friend void static_init();
  public:
  ActorFeature( const std::string& s="" ):
    Feature( PROPS ){ classInit( s ); }
  ActorFeature( const KWargs& a ):
    Feature( PROPS ){ classInit( a ); }
  ActorFeature( Document *d, const std::string& s="" ):
    Feature( PROPS, d ){ classInit( s ); }
  ActorFeature( Document *d, const KWargs& a ):
    Feature( PROPS, d ){ classInit( a ); }
  private:
    static properties PROPS;
    void init();
  };

  class HeadFeature: public Feature {
    friend void static_init();
  public:
  HeadFeature( const std::string& s="" ):
    Feature( PROPS ){ classInit( s ); }
  HeadFeature( const KWargs& a ):
    Feature( PROPS ){ classInit( a ); }
  HeadFeature( Document *d, const std::string& s="" ):
    Feature( PROPS, d ){ classInit( s ); }
  HeadFeature( Document *d, const KWargs& a ):
    Feature( PROPS, d ){ classInit( a ); }
  private:
    static properties PROPS;
    void init();
  };

  class ValueFeature: public Feature {
    friend void static_init();
  public:
  ValueFeature( const std::string& s="" ):
    Feature( PROPS ){ classInit( s ); }
  ValueFeature( const KWargs& a ):
    Feature( PROPS ){ classInit( a ); }
  ValueFeature( Document *d, const std::string& s="" ):
    Feature( PROPS, d ){ classInit( s ); }
  ValueFeature( Document *d, const KWargs& a ):
    Feature( PROPS, d ){ classInit( a ); }
  private:
    static properties PROPS;
    void init();
  };

  class FunctionFeature: public Feature {
    friend void static_init();
  public:
  FunctionFeature( const std::string& s="" ):
    Feature( PROPS ){ classInit( s ); }
  FunctionFeature( const KWargs& a ):
    Feature( PROPS ){ classInit( a ); }
  FunctionFeature( Document *d, const std::string& s="" ):
    Feature( PROPS, d ){ classInit( s ); }
  FunctionFeature( Document *d, const KWargs& a ):
    Feature( PROPS, d ){ classInit( a ); }
  private:
    static properties PROPS;
    void init();
  };

  class TimeFeature: public Feature {
    friend void static_init();
  public:
  TimeFeature( const std::string& s="" ):
    Feature( PROPS ){ classInit( s ); }
  TimeFeature( const KWargs& a ):
    Feature( PROPS ){ classInit( a ); }
  TimeFeature( Document *d, const std::string& s="" ):
    Feature( PROPS, d ){ classInit( s ); }
  TimeFeature( Document *d, const KWargs& a ):
    Feature( PROPS, d ){ classInit( a ); }
  private:
    static properties PROPS;
    void init();
  };

  class LevelFeature: public Feature {
    friend void static_init();
  public:
  LevelFeature( const std::string& s="" ):
    Feature( PROPS ){ classInit( s ); }
  LevelFeature( const KWargs& a ):
    Feature( PROPS ){ classInit( a ); }
  LevelFeature( Document *d, const std::string& s="" ):
    Feature( PROPS, d ){ classInit( s ); }
  LevelFeature( Document *d, const KWargs& a ):
    Feature( PROPS, d ){ classInit( a ); }
  private:
    static properties PROPS;
    void init();
  };

  class ModalityFeature: public Feature {
    friend void static_init();
  public:
  ModalityFeature( const std::string& s="" ):
    Feature( PROPS ){ classInit( s ); }
  ModalityFeature( const KWargs& a ):
    Feature( PROPS ){ classInit( a ); }
  ModalityFeature( Document *d, const std::string& s="" ):
    Feature( PROPS, d ){ classInit( s ); }
  ModalityFeature( Document *d, const KWargs& a ):
    Feature( PROPS, d ){ classInit( a ); }
  private:
    static properties PROPS;
    void init();
  };

  class WordReference: public FoliaImpl {
    friend void static_init();
  public:
  WordReference( const std::string& s="" ):
    FoliaImpl(PROPS){ classInit( s ); };
  WordReference( const KWargs& a ):
    FoliaImpl(PROPS){ classInit( a ); };
  WordReference( Document *d, const std::string& s="" ):
    FoliaImpl(PROPS,d){ classInit( s ); };
  WordReference( Document *d, const KWargs& a ):
    FoliaImpl(PROPS,d){ classInit( a ); };
  private:
    static properties PROPS;
    void init();
    FoliaElement* parseXml( const xmlNode *node );
  };

  class Alignment: public FoliaImpl {
    friend void static_init();
  public:
  Alignment( const std::string& s="" ):
    FoliaImpl(PROPS){ classInit( s ); };
  Alignment( const KWargs& a ):
    FoliaImpl(PROPS){ classInit( a ); };
  Alignment( Document *d, const std::string& s="" ):
    FoliaImpl(PROPS,d){ classInit( s ); };
  Alignment( Document *d, const KWargs& a ):
    FoliaImpl(PROPS,d){ classInit( a ); };
    std::vector<FoliaElement *>resolve() const;
  private:
    static properties PROPS;
    void init();
  };

  class AlignReference: public FoliaImpl {
    friend void static_init();
    friend class Alignment;
  public:
  AlignReference( const std::string& s="" ):
    FoliaImpl(PROPS){ classInit( s ); };
  AlignReference( const KWargs& a ):
    FoliaImpl(PROPS){ classInit( a ); };
  AlignReference( Document *d, const std::string& s="" ):
    FoliaImpl(PROPS,d){ classInit( s ); };
  AlignReference( Document *d, const KWargs& a ):
    FoliaImpl(PROPS,d){ classInit( a ); };
    KWargs collectAttributes() const;
    void setAttributes( const KWargs& );
  private:
    static properties PROPS;
    void init();
    FoliaElement* parseXml( const xmlNode *node );
    FoliaElement *resolve( const Alignment *ref ) const;
    std::string refId;
    std::string ref_type;
    std::string _t;
  };

  class SyntacticUnit: public AbstractSpanAnnotation {
    friend void static_init();
  public:
  SyntacticUnit( const std::string& s="" ):
    AbstractSpanAnnotation( PROPS ){ classInit( s ); }
  SyntacticUnit( const KWargs& a ):
    AbstractSpanAnnotation( PROPS ){ classInit( a ); }
  SyntacticUnit( Document *d, const std::string& s="" ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( s ); }
  SyntacticUnit( Document *d, const KWargs& a ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( a ); }
  private:
    static properties PROPS;
    void init();
  };

  class Chunk: public AbstractSpanAnnotation {
    friend void static_init();
  public:
  Chunk( const std::string& s="" ):
    AbstractSpanAnnotation( PROPS ){ classInit( s ); }
  Chunk( const KWargs& a ):
    AbstractSpanAnnotation( PROPS ){ classInit( a ); }
  Chunk( Document *d, const std::string& s="" ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( s ); }
  Chunk( Document *d, const KWargs& a ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( a ); }
  private:
    static properties PROPS;
    void init();
  };

  class Entity: public AbstractSpanAnnotation {
    friend void static_init();
  public:
  Entity( const std::string& s="" ):
    AbstractSpanAnnotation( PROPS ){ classInit( s ); }
  Entity( const KWargs& a ):
    AbstractSpanAnnotation( PROPS ){ classInit( a ); }
  Entity( Document *d, const std::string& s="" ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( s ); }
  Entity( Document *d, const KWargs& a ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( a ); }
  private:
    static properties PROPS;
    void init();
  };

  class Headwords: public AbstractSpanAnnotation {
    friend void static_init();
  public:
  Headwords( const std::string& s="" ):
    AbstractSpanAnnotation( PROPS ){ classInit( s ); }
  Headwords( const KWargs& a ):
    AbstractSpanAnnotation( PROPS ){ classInit( a ); }
  Headwords( Document *d, const std::string& s="" ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( s ); }
  Headwords( Document *d, const KWargs& a ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( a ); }
  private:
    static properties PROPS;
    void init();
  };

  class DependencyDependent: public AbstractSpanAnnotation {
    friend void static_init();
  public:
  DependencyDependent( const std::string& s="" ):
    AbstractSpanAnnotation( PROPS ){ classInit( s ); }
  DependencyDependent( const KWargs& a ):
    AbstractSpanAnnotation( PROPS ){ classInit( a ); }
  DependencyDependent( Document *d, const std::string& s="" ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( s ); }
  DependencyDependent( Document *d, const KWargs& a ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( a ); }
  private:
    static properties PROPS;
    void init();
  };

  class Dependency: public AbstractSpanAnnotation {
    friend void static_init();
  public:
  Dependency( const std::string& s="" ):
    AbstractSpanAnnotation( PROPS ){ classInit( s ); }
  Dependency( const KWargs& a ):
    AbstractSpanAnnotation( PROPS ){ classInit( a ); }
  Dependency( Document *d, const std::string& s="" ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( s ); }
  Dependency( Document *d, const KWargs& a ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( a ); }
    Headwords *head() const;
    DependencyDependent *dependent() const;
  private:
    static properties PROPS;
    void init();
  };

  class CoreferenceLink: public AbstractSpanAnnotation {
    friend void static_init();
  public:
  CoreferenceLink( const std::string& s="" ):
    AbstractSpanAnnotation( PROPS ){ classInit( s ); }
  CoreferenceLink( const KWargs& a ):
    AbstractSpanAnnotation( PROPS ){ classInit( a ); }
  CoreferenceLink( Document *d, const std::string& s="" ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( s ); }
  CoreferenceLink( Document *d, const KWargs& a ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( a ); }
  private:
    static properties PROPS;
    void init();
  };

  class CoreferenceChain: public AbstractSpanAnnotation {
    friend void static_init();
  public:
  CoreferenceChain( const std::string& s="" ):
    AbstractSpanAnnotation( PROPS ){ classInit( s ); }
  CoreferenceChain( const KWargs& a ):
    AbstractSpanAnnotation( PROPS ){ classInit( a ); }
  CoreferenceChain( Document *d, const std::string& s="" ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( s ); }
  CoreferenceChain( Document *d, const KWargs& a ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( a ); }
  private:
    static properties PROPS;
    void init();
  };

  class SemanticRole: public AbstractSpanAnnotation {
    friend void static_init();
  public:
  SemanticRole( const std::string& s="" ):
    AbstractSpanAnnotation( PROPS ){ classInit( s ); }
  SemanticRole( const KWargs& a ):
    AbstractSpanAnnotation( PROPS ){ classInit( a ); }
  SemanticRole( Document *d, const std::string& s="" ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( s ); }
  SemanticRole( Document *d, const KWargs& a ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( a ); }
  private:
    static properties PROPS;
    void init();
  };

  class AbstractAnnotationLayer: public FoliaImpl,
    public AllowGenerateID,
    public AllowAnnotation,
    public AllowCorrection {
  public:
  AbstractAnnotationLayer( const properties& props, const std::string& s=""):
      FoliaImpl( props ) { classInit( s ); };
  AbstractAnnotationLayer( const properties& props, const KWargs& a ):
      FoliaImpl( props ) { classInit( a ); };
  AbstractAnnotationLayer( const properties& props, Document *d, const std::string& s=""):
      FoliaImpl( props, d ) { classInit( s ); };
  AbstractAnnotationLayer( const properties& props, Document *d, const KWargs& a ):
      FoliaImpl( props, d ) { classInit( a ); };
    AbstractSpanAnnotation *findspan( const std::vector<FoliaElement*>& ) const;
    FoliaElement *append( FoliaElement * );
  private:
    void init();
    void assignset( FoliaElement * );
  };

  class AbstractCorrectionChild: public FoliaImpl {
  public:
  AbstractCorrectionChild( const properties& props, Document *d=0 ):
    FoliaImpl( props, d ){ classInit(); };
  private:
    void init();
  };

  class NewElement: public AbstractCorrectionChild {
    friend void static_init();
  public:
  NewElement( const std::string& s=""):
    AbstractCorrectionChild( PROPS ) { classInit( s ); };
  NewElement( const KWargs& a ):
    AbstractCorrectionChild( PROPS ) { classInit( a ); };
  NewElement( Document *d, const std::string& s=""):
    AbstractCorrectionChild( PROPS, d ) { classInit( s ); };
  NewElement( Document *d, const KWargs& a ):
    AbstractCorrectionChild( PROPS, d ) { classInit( a ); };
  private:
    static properties PROPS;
    void init();
  };

  class Current: public AbstractCorrectionChild {
    friend void static_init();
  public:
  Current( const std::string& s=""):
    AbstractCorrectionChild( PROPS ) { classInit( s ); };
  Current( const KWargs& a ):
    AbstractCorrectionChild( PROPS ) { classInit( a ); };
  Current( Document *d, const std::string& s=""):
    AbstractCorrectionChild( PROPS, d ) { classInit( s ); };
  Current( Document *d, const KWargs& a ):
    AbstractCorrectionChild( PROPS, d ) { classInit( a ); };
  private:
    static properties PROPS;
    void init();
  };

  class Original: public AbstractCorrectionChild {
    friend void static_init();
  public:
  Original( const std::string& s=""):
    AbstractCorrectionChild( PROPS ) { classInit( s ); };
  Original( const KWargs& a ):
    AbstractCorrectionChild( PROPS ) { classInit( a ); };
  Original( Document *d, const std::string& s=""):
    AbstractCorrectionChild( PROPS, d ) { classInit( s ); };
  Original( Document *d, const KWargs& a ):
    AbstractCorrectionChild( PROPS, d ) { classInit( a ); };
  private:
    static properties PROPS;
    void init();
  };

  class Suggestion: public AbstractCorrectionChild {
    friend void static_init();
  public:
  Suggestion( const std::string& s=""):
    AbstractCorrectionChild( PROPS ) { classInit( s ); };
  Suggestion( const KWargs& a ):
    AbstractCorrectionChild( PROPS ) { classInit( a ); };
  Suggestion( Document *d, const std::string& s=""):
    AbstractCorrectionChild( PROPS, d ) { classInit( s ); };
  Suggestion( Document *d, const KWargs& a ):
    AbstractCorrectionChild( PROPS, d ) { classInit( a ); };
  private:
    static properties PROPS;
    void init();
  };

  class Description: public FoliaImpl {
    friend void static_init();
  public:
  Description( const std::string& s=""):
    FoliaImpl(PROPS) { classInit( s ); };
  Description( const KWargs& a ):
    FoliaImpl(PROPS) { classInit( a ); };
  Description( Document *d, const std::string& s="" ):
    FoliaImpl(PROPS,d) { classInit( s ); };
  Description( Document *d, const KWargs& a ):
    FoliaImpl(PROPS,d) { classInit( a ); };
    std::string description() const { return _value; };
    void setAttributes( const KWargs& kwargs );
    FoliaElement* parseXml( const xmlNode * );
    xmlNode *xml( bool, bool=false ) const;
  private:
    static properties PROPS;
    void init();
    std::string _value;
  };

  class XmlComment: public FoliaImpl {
    friend void static_init();
  public:
  XmlComment( const std::string& s=""):
    FoliaImpl(PROPS) { classInit( s ); };
  XmlComment( const KWargs& a ):
    FoliaImpl(PROPS) { classInit( a ); };
  XmlComment( Document *d, const std::string& s="" ):
    FoliaImpl(PROPS,d) { classInit( s ); };
  XmlComment( Document *d, const KWargs& a ):
    FoliaImpl(PROPS,d) { classInit( a ); };
    FoliaElement* parseXml( const xmlNode * );
    xmlNode *xml( bool, bool=false ) const;
  private:
    static properties PROPS;
    void init();
    std::string _value;
  };

  class XmlText: public FoliaImpl {
    friend void static_init();
  public:
  XmlText( const std::string& s=""):
    FoliaImpl(PROPS) { classInit( s ); };
  XmlText( const KWargs& a ):
    FoliaImpl(PROPS) { classInit( a ); };
  XmlText( Document *d, const std::string& s="" ):
    FoliaImpl(PROPS,d) { classInit( s ); };
  XmlText( Document *d, const KWargs& a ):
    FoliaImpl(PROPS, d ) { classInit( a ); };
    FoliaElement* parseXml( const xmlNode * );
    xmlNode *xml( bool, bool=false ) const;
    bool setvalue( const std::string& s ) { _value = s; return true; };
    std::string getTextDelimiter( bool ) const { return ""; };
    UnicodeString text( const std::string& = "current",
			bool = false, bool = false ) const;
  private:
    static properties PROPS;
    void init();
    std::string _value; //UTF8 value
  };

  class External: public FoliaImpl {
    friend void static_init();
  public:
  External( const std::string& s=""):
    FoliaImpl( PROPS ) { classInit( s ); };
  External( const KWargs& a ):
    FoliaImpl( PROPS ) { classInit( a ); };
  External( Document *d, const std::string& s="" ):
    FoliaImpl( PROPS, d ) { classInit( s ); };
  External( Document *d, const KWargs& a ):
    FoliaImpl( PROPS, d ) { classInit( a ); };
    FoliaElement* parseXml( const xmlNode * );
    void resolve();
    void setAttributes( const KWargs& );
    KWargs collectAttributes() const;
  private:
    static properties PROPS;
    void init();
    bool _include;
  };

  class Note: public AbstractStructureElement {
    friend void static_init();
  public:
  Note( const std::string& s=""):
    AbstractStructureElement( PROPS ){ classInit( s ); };
  Note( const KWargs& a ):
    AbstractStructureElement( PROPS ){ classInit( a ); };
  Note( Document *d, const std::string& s=""):
    AbstractStructureElement( PROPS, d ){ classInit( s ); };
  Note( Document *d, const KWargs& a ): AbstractStructureElement( PROPS, d ){ classInit( a ); };
    KWargs collectAttributes() const;
    void setAttributes( const KWargs& );
  private:
    static properties PROPS;
    void init();
    std::string refId;
  };

  class Definition: public AbstractStructureElement {
    friend void static_init();
  public:
  Definition( const std::string& s=""):
    AbstractStructureElement( PROPS ){ classInit( s ); };
  Definition( const KWargs& a ):
    AbstractStructureElement( PROPS ){ classInit( a ); };
  Definition( Document *d, const std::string& s=""):
    AbstractStructureElement( PROPS, d ){ classInit( s ); };
  Definition( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); };
  private:
    static properties PROPS;
    void init();
  };

  class Term: public AbstractStructureElement {
    friend void static_init();
  public:
  Term( const std::string& s=""):
    AbstractStructureElement( PROPS ){ classInit( s ); };
  Term( const KWargs& a ):
    AbstractStructureElement( PROPS ){ classInit( a ); };
  Term( Document *d, const std::string& s=""):
    AbstractStructureElement( PROPS, d ){ classInit( s ); };
  Term( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); };
  private:
    static properties PROPS;
    void init();
  };

  class Example: public AbstractStructureElement {
    friend void static_init();
  public:
  Example( const std::string& s=""):
    AbstractStructureElement( PROPS ){ classInit( s ); };
  Example( const KWargs& a ):
    AbstractStructureElement( PROPS ){ classInit( a ); };
  Example( Document *d, const std::string& s=""):
    AbstractStructureElement( PROPS, d ){ classInit( s ); };
  Example( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); };
  private:
    static properties PROPS;
    void init();
  };

  class Entry: public AbstractStructureElement {
    friend void static_init();
  public:
  Entry( const std::string& s=""):
    AbstractStructureElement( PROPS ){ classInit( s ); };
  Entry( const KWargs& a ):
    AbstractStructureElement( PROPS ){ classInit( a ); };
  Entry( Document *d, const std::string& s=""):
    AbstractStructureElement( PROPS, d ){ classInit( s ); };
  Entry( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); };
  private:
    static properties PROPS;
    void init();
  };

  class Reference: public AbstractStructureElement {
    friend void static_init();
    friend class Note;
  public:
  Reference( const std::string& s="" ):
    AbstractStructureElement( PROPS ){ classInit( s ); };
  Reference( const KWargs& a ):
    AbstractStructureElement( PROPS ){ classInit( a ); };
  Reference( Document *d, const std::string& s="" ):
    AbstractStructureElement( PROPS, d ){ classInit( s ); };
  Reference( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); };
    KWargs collectAttributes() const;
    void setAttributes( const KWargs& );
  private:
    static properties PROPS;
    void init();
    std::string refId;
    std::string ref_type;
  };


  class Correction: public AbstractTokenAnnotation {
    friend void static_init();
  public:
  Correction( const std::string& s=""):
    AbstractTokenAnnotation( PROPS ){ classInit( s ); }
  Correction( const KWargs& a ):
    AbstractTokenAnnotation( PROPS ){ classInit( a ); }
  Correction( Document *d, const std::string& s=""):
    AbstractTokenAnnotation( PROPS, d ){ classInit( s ); }
  Correction( Document *d, const KWargs& a ):
    AbstractTokenAnnotation( PROPS, d ){ classInit( a ); }
    bool hasNew() const;
    bool hasOriginal() const;
    bool hasCurrent() const;
    bool hasSuggestions() const;
    NewElement *getNew() const;
    FoliaElement *getNew( size_t ) const;
    Original *getOriginal() const;
    FoliaElement *getOriginal( size_t ) const;
    Current *getCurrent() const;
    FoliaElement *getCurrent( size_t ) const;
    std::vector<Suggestion*> suggestions() const;
    Suggestion *suggestions( size_t ) const;
    UnicodeString text( const std::string& = "current",
			bool = false, bool = false ) const;
    TextContent *textcontent( const std::string& = "current" ) const;
    PhonContent *phoncontent( const std::string& = "current" ) const;
    std::string getTextDelimiter( bool=false) const;
  private:
    static properties PROPS;
    void init();
  };

  class ErrorDetection: public AbstractTokenAnnotation  {
    friend void static_init();
  public:
  ErrorDetection( const std::string& s=""):
    AbstractTokenAnnotation( PROPS ){ classInit( s ); }
  ErrorDetection( const KWargs& a ):
    AbstractTokenAnnotation( PROPS ){ classInit( a ); }
  ErrorDetection( Document *d, const std::string& s=""):
    AbstractTokenAnnotation( PROPS, d ){ classInit( s ); }
  ErrorDetection( Document *d, const KWargs& a ):
    AbstractTokenAnnotation( PROPS, d ){ classInit( a ); }
  private:
    static properties PROPS;
    void init();
  };

  class TimeSegment: public AbstractSpanAnnotation {
    friend void static_init();
  public:
  TimeSegment( const std::string& s="" ):
    AbstractSpanAnnotation( PROPS ){ classInit( s ); }
  TimeSegment( const KWargs& a ):
    AbstractSpanAnnotation( PROPS ){ classInit( a ); }
  TimeSegment( Document *d, const std::string& s="" ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( s ); }
  TimeSegment( Document *d, const KWargs& a ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( a ); }
  private:
    static properties PROPS;
    void init();
  };

  class Morpheme: public AbstractStructureElement {
    friend void static_init();
  public:
  Morpheme( const std::string& s="" ):
    AbstractStructureElement( PROPS ){ classInit( s ); }
  Morpheme( const KWargs& a ):
    AbstractStructureElement( PROPS ){ classInit( a ); }
  Morpheme( Document *d, const std::string& s="" ):
    AbstractStructureElement( PROPS, d ){ classInit( s ); }
  Morpheme( Document *d, const KWargs& a ): AbstractStructureElement( PROPS, d ){ classInit( a ); }
  private:
    static properties PROPS;
    void init();
  };

  class SyntaxLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
  SyntaxLayer( const std::string& s=""):
    AbstractAnnotationLayer( PROPS ){ classInit( s ); }
  SyntaxLayer( const KWargs& a ):
    AbstractAnnotationLayer( PROPS ){ classInit( a ); }
  SyntaxLayer( Document *d, const std::string& s=""):
    AbstractAnnotationLayer( PROPS, d ){ classInit( s ); }
  SyntaxLayer( Document *d, const KWargs& a ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( a ); }
  private:
    static properties PROPS;
    void init();
  };

  class ChunkingLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
  ChunkingLayer( const std::string& s=""):
    AbstractAnnotationLayer( PROPS ){ classInit( s ); }
  ChunkingLayer( const KWargs& a ):
    AbstractAnnotationLayer( PROPS ){ classInit( a ); }
  ChunkingLayer( Document *d, const std::string& s=""):
    AbstractAnnotationLayer( PROPS, d ){ classInit( s ); }
  ChunkingLayer( Document *d, const KWargs& a ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( a ); }
  private:
    static properties PROPS;
    void init();
  };

  class EntitiesLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
  EntitiesLayer( const std::string& s=""):
    AbstractAnnotationLayer( PROPS ){ classInit( s ); }
  EntitiesLayer( const KWargs& a ):
    AbstractAnnotationLayer( PROPS ){ classInit( a ); }
  EntitiesLayer( Document *d, const std::string& s=""):
    AbstractAnnotationLayer( PROPS, d ){ classInit( s ); }
  EntitiesLayer( Document *d, const KWargs& a ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( a ); }
  private:
    static properties PROPS;
    void init();
  };

  class TimingLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
  TimingLayer( const std::string& s=""):
    AbstractAnnotationLayer( PROPS ){ classInit( s ); }
  TimingLayer( const KWargs& a ):
    AbstractAnnotationLayer( PROPS ){ classInit( a ); }
  TimingLayer( Document *d, const std::string& s=""):
    AbstractAnnotationLayer( PROPS, d ){ classInit( s ); }
  TimingLayer( Document *d, const KWargs& a ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( a ); }
  private:
    static properties PROPS;
    void init();
  };

  class MorphologyLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
  MorphologyLayer( const std::string& s="" ):
    AbstractAnnotationLayer( PROPS ){ classInit( s ); }
  MorphologyLayer( const KWargs& a ):
    AbstractAnnotationLayer( PROPS ){ classInit( a ); }
  MorphologyLayer( Document *d, const std::string& s="" ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( s ); }
  MorphologyLayer( Document *d, const KWargs& a ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( a ); }
  private:
    static properties PROPS;
    void init();
  };

  class PhonologyLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
  PhonologyLayer( const std::string& s="" ):
    AbstractAnnotationLayer( PROPS ){ classInit( s ); }
  PhonologyLayer( const KWargs& a ):
    AbstractAnnotationLayer( PROPS ){ classInit( a ); }
  PhonologyLayer( Document *d, const std::string& s="" ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( s ); }
  PhonologyLayer( Document *d, const KWargs& a ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( a ); }
  private:
    static properties PROPS;
    void init();
  };

  class DependenciesLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
  DependenciesLayer( const std::string& s=""):
    AbstractAnnotationLayer( PROPS ){ classInit( s ); }
  DependenciesLayer( const KWargs& a ):
    AbstractAnnotationLayer( PROPS ){ classInit( a ); }
  DependenciesLayer( Document *d, const std::string& s=""):
    AbstractAnnotationLayer( PROPS, d ){ classInit( s ); }
  DependenciesLayer( Document *d, const KWargs& a ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( a ); }
  private:
    static properties PROPS;
    void init();
  };

  class CoreferenceLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
  CoreferenceLayer( const std::string& s=""):
    AbstractAnnotationLayer( PROPS ){ classInit( s ); }
  CoreferenceLayer( const KWargs& a ):
    AbstractAnnotationLayer( PROPS ){ classInit( a ); }
  CoreferenceLayer( Document *d, const std::string& s=""):
    AbstractAnnotationLayer( PROPS, d ){ classInit( s ); }
  CoreferenceLayer( Document *d, const KWargs& a ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( a ); }
  private:
    static properties PROPS;
    void init();
  };

  class SemanticRolesLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
  SemanticRolesLayer( const std::string& s=""):
    AbstractAnnotationLayer( PROPS ){ classInit( s ); }
  SemanticRolesLayer( const KWargs& a ):
    AbstractAnnotationLayer( PROPS ){ classInit( a ); }
  SemanticRolesLayer( Document *d, const std::string& s=""):
    AbstractAnnotationLayer( PROPS, d ){ classInit( s ); }
  SemanticRolesLayer( Document *d, const KWargs& a ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( a ); }
  private:
    static properties PROPS;
    void init();
  };

  std::string VersionName();
  std::string Version();

} // namespace folia

#endif
