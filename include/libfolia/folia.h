/*
  $Id$
  $URL$

  Copyright (c) 1998 - 2013
  ILK   - Tilburg University
  CLiPS - University of Antwerp
 
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
      http://ilk.uvt.nl/software.html
  or send mail to:
      timbl@uvt.nl
*/

#ifndef FOLIA_H
#define FOLIA_H

#include <list>
#include "unicode/unistr.h"
#include "unicode/regex.h"
#include "libxml/tree.h"
#include "libxml/xpath.h"
#include <ctime>
#include "libfolia/foliautils.h"

namespace folia {
  class Document;
  class FoliaElement;
  class AbstractTokenAnnotation;
  class AbstractSpanAnnotation;
  class Alternative;
  class PosAnnotation;
  class LemmaAnnotation;
  class Sentence;
  class Word;
  class TextContent;
  class AllowCorrection;
  class Correction;
  class Suggestion;
  class Division;
  class DependencyDependent;
  class Paragraph;
  class Morpheme;

  extern std::set<ElementType> default_ignore;  
  extern std::set<ElementType> default_ignore_annotations;  
  extern std::set<ElementType> default_ignore_structure;  

  class FoliaElement {
    friend std::ostream& operator<<( std::ostream&, const FoliaElement& );
    friend std::ostream& operator<<( std::ostream&, const FoliaElement* );
    friend bool operator==( const FoliaElement&, const FoliaElement& );
    friend class Document;
    friend class Word;
    friend class Correction;
    friend class AllowCorrection;
  public:
    //Constructor  
    FoliaElement( Document* =0 );
    virtual ~FoliaElement();

    static FoliaElement *createElement( Document *, const ElementType  );
    static FoliaElement *createElement( Document *, const std::string&  );

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
    
    virtual void setAttributes( const KWargs& );

    void setDateTime( const std::string& );
    std::string getDateTime() const;

    //functions regarding contained data
    size_t size() const { return data.size(); };
  
    void fixupDoc( Document* );
    virtual FoliaElement *append( FoliaElement* );
    virtual FoliaElement *postappend( ) { return this; };
    virtual std::vector<FoliaElement*> findreplacables( FoliaElement * ) const;
    void remove( FoliaElement *, bool = true );
    void replace( FoliaElement * );

    FoliaElement* index( size_t ) const;

    FoliaElement* operator[]( size_t i ) const {
      return index(i);
    }

    FoliaElement* rindex( size_t ) const;

    virtual const Word* resolveword( const std::string& ) const { return 0; };

    bool isinstance( ElementType et ) const {
      return et == _element_id;
    }

    std::vector<AbstractSpanAnnotation*> selectSpan() const;

    template <typename F>
      bool isinstance() const {
      F obj("");
      return _element_id == obj._element_id;
    }
      
    template <typename F>
      std::vector<F*> select( const std::string& st,
			      const std::set<ElementType>& exclude, 
			      bool recurse = true ) const {
      F obj("");
      std::vector<F*> res;
      std::vector<FoliaElement*> tmp = select( obj._element_id, 
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
      F obj("");
      std::vector<F*> res;
      std::vector<FoliaElement*> tmp = select( obj._element_id, 
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
      F obj("");
      std::vector<F*> res;
      std::vector<FoliaElement*> tmp = select( obj._element_id, 
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
      F obj("");
      std::vector<F*> res;
      std::vector<FoliaElement*> tmp = select( obj._element_id, 
					       exclude,
					       recurse );
      for ( size_t i = 0; i < tmp.size(); ++i ){
	res.push_back( dynamic_cast<F*>( tmp[i]) );
      }
      return res;
    }
    
    template <typename F>
      std::vector<F*> select( bool recurse = true ) const {
      F obj("");
      std::vector<F*> res;
      std::vector<FoliaElement*> tmp = select( obj._element_id,
					       recurse );
      for ( size_t i = 0; i < tmp.size(); ++i ){
	res.push_back( dynamic_cast<F*>( tmp[i]) );
      }
      return res;
    }
    
    template <typename F> 
      std::vector<F*> annotations( const std::string& s = "" ) const {
      if ( allowannotations() ){
	std::vector<F*> v = select<F>( s, default_ignore_annotations );
	if ( v.size() >= 1 )
	  return v;
	else {
	  F obj("");
	  if ( s.empty() )
	    throw NoSuchAnnotation( obj.classname() );
	  else
	    throw NoSuchAnnotation( obj.classname() + " for set='" + s + "'" );
	}
      }
      else {
	throw NotImplementedError( "annotations() for " + _xmltag ); 
      };
    }
    
    template <typename F> 
      F *annotation( const std::string& st = "" ) const {
      std::vector<F*>v = annotations<F>( st );
      return v[0]; // always exist, otherwise annotations would throw()
    }

    virtual std::vector<FoliaElement*> findspans( ElementType,
						  const std::string& = "" ) const {
      throw NotImplementedError( "findspans() for " + _xmltag ); 
    }

    template <typename F> 
      std::vector<FoliaElement*> findspans( const std::string& st = "" ) const {
      F obj("");
      return findspans( obj._element_id, st );
    }

    std::vector<std::string> feats( const std::string& ) const;
    std::vector<Sentence *> sentencePart() const;
    std::string feat( const std::string& ) const;
    //XML (de)serialisation
    std::string xmlstring() const; // serialize to a string (XML fragment)
    virtual xmlNode *xml( bool, bool = false ) const; //serialize to XML  
    virtual FoliaElement* parseXml( const xmlNode * );
    virtual std::string str() const;
    UnicodeString unicode() const { return text(); };
    UnicodeString toktext( const std::string& cls = "current" ) const {
      return text( cls, true );
    }
    virtual UnicodeString text( const std::string& = "current", bool = false ) const;
    virtual TextContent *textcontent( const std::string& = "current" ) const;
    virtual UnicodeString stricttext( const std::string& = "current" ) const;
    bool hastext( const std::string& = "current" ) const ;
    virtual FoliaElement *head() const {
      throw NotImplementedError("head() for " + _xmltag );
    }
    virtual FoliaElement *getNew() const {
      throw NotImplementedError("getNew() for " + _xmltag ); };
    virtual FoliaElement *getOriginal() const {
      throw NotImplementedError("getOriginal() for " + _xmltag ); };
    virtual FoliaElement *getCurrent() const {
      throw NotImplementedError("getCurrent() for " + _xmltag ); };
    virtual FoliaElement *split( FoliaElement *, FoliaElement *, 
				    const std::string& = "" ){
      throw NotImplementedError("split() for " + _xmltag ); };
    virtual Correction *mergewords( FoliaElement *, 
				    const std::vector<FoliaElement *>&,
				    const std::string& = "" ){
      throw NotImplementedError("mergewords() for " + _xmltag ); };
    virtual Correction *deleteword( FoliaElement *, 
				    const std::string& = "" ){
      throw NotImplementedError("deleteword() for " + _xmltag ); };
    virtual Correction *insertword( FoliaElement *, FoliaElement *,
				    const std::string& = "" ){
      throw NotImplementedError("insertword() for " + _xmltag ); };
    virtual std::vector<Suggestion*> suggestions() const
      { throw NotImplementedError("suggestions() for " + _xmltag ); };
    virtual Suggestion *suggestions( size_t ) const
      { throw NotImplementedError("suggestions() for " + _xmltag ); };    
    virtual std::string subset() const 
      { throw NotImplementedError("subset() for " + _xmltag ); };
    virtual FoliaElement *previous() const {
      throw NotImplementedError("previous() for " + _xmltag ); };
    virtual FoliaElement *next() const {
      throw NotImplementedError("next() for " + _xmltag ); };
    virtual std::vector<Word*> context( size_t, 
					const std::string& ="" ) const {
      throw NotImplementedError("contect() for " + _xmltag ); };
    virtual std::vector<Word*> leftcontext( size_t, 
					    const std::string& ="" ) const {
      throw NotImplementedError("leftcontect() for " + _xmltag ); 
    };
    virtual std::vector<Word*> rightcontext( size_t, 
					     const std::string& ="" ) const {
      throw NotImplementedError("rightcontext() for " + _xmltag ); 
    };
    virtual FoliaElement *findspan( const std::vector<FoliaElement*>& ) const {
      throw NotImplementedError("findspan() for " + _xmltag ); 
    };
    virtual int offset() const {
      throw NotImplementedError("offset() for " + _xmltag ); 
    };
    virtual std::string getlang() const {
      throw NotImplementedError("offset() for " + _xmltag ); 
    };
    virtual std::string setlang( const std::string& ) {
      throw NotImplementedError("offset() for " + _xmltag ); 
    };

    std::string pos( const std::string& = "" ) const;
    std::string lemma( const std::string& = "" ) const;
    std::string cls() const { return _class; };
    std::string sett() const { return _set; };
    std::string annotator( ) const { return _annotator; };
    void annotator( const std::string& a ) { _annotator = a; };
    AnnotatorType annotatortype() const { return _annotator_type; };
    void annotatortype( AnnotatorType t ) { _annotator_type =  t; };
    double confidence() const { return _confidence; };
    void confidence( double d ) { _confidence = d; };
    AnnotationType::AnnotationType annotation_type() const { return _annotation_type; };
    std::string classname() const { return toString(_element_id); };
    std::string n() const { return _n; };
    std::string id() const { return _id; };
    ElementType element_id() const { return _element_id; };
    std::string xmltag() const { return _xmltag; };
    Document *doc() const { return mydoc; };
    xmlNs *foliaNs() const;
    virtual Sentence *sentence() const {
      throw NotImplementedError("sentence() for " + _xmltag );
    };
    virtual Paragraph *paragraph() const {
      throw NotImplementedError("paragraph() for " + _xmltag );
    };
    virtual Division *division() const {
      throw NotImplementedError("division() for " + _xmltag );
    };
    virtual Correction *incorrection() const {
      throw NotImplementedError("incorrection() for " + _xmltag );
    };
    virtual std::vector<Paragraph*> paragraphs() const {
      throw NotImplementedError("paragraphs() for " + _xmltag );
    };
    virtual std::vector<Sentence*> sentences() const {
      throw NotImplementedError("sentences() for " + _xmltag );
    };
    virtual std::vector<Word*> words() const {
      throw NotImplementedError("words() for " + _xmltag );
    };
    virtual std::vector<FoliaElement*> wrefs() const {
      throw NotImplementedError("wrefs() for " + _xmltag );
    };

    virtual std::vector<Morpheme*> morphemes( const std::string& ="" ) const {
      throw NotImplementedError("morphemes() for " + _xmltag );
    };
    virtual Morpheme* morpheme( size_t, const std::string& ="" ) const {
      throw NotImplementedError("morpheme() for " + _xmltag );
    };
    virtual Sentence *sentences( size_t ) const {
      throw NotImplementedError("sentences() for " + _xmltag );
    };
    virtual Sentence *rsentences( size_t ) const {
      throw NotImplementedError("rsentences() for " + _xmltag );
    };
    virtual Paragraph *paragraphs( size_t ) const {
      throw NotImplementedError("paragraphs() for " + _xmltag );
    };
    virtual Paragraph *rparagraphs( size_t ) const {
      throw NotImplementedError("rparagraphs() for " + _xmltag );
    };
    virtual Word *words( size_t ) const {
      throw NotImplementedError("words() for " + _xmltag );
    };
    virtual std::vector<Word *> wordParts() const {
      throw NotImplementedError("words() for " + _xmltag );
    };
    virtual Word *rwords( size_t ) const {
      throw NotImplementedError("rwords() for " + _xmltag );
    };
    virtual DependencyDependent *dependent() const {
      throw NotImplementedError("dependent() for " + _xmltag );
    };

    virtual std::string description() const;
    Sentence *addSentence( const std::string& s ="" ){
      return addSentence( getArgs(s) );
    };
    Sentence *addSentence( const KWargs& );
    Word *addWord( const std::string& s ){
      return addWord( getArgs(s) );
    }
    Word *addWord( const KWargs& );
    TextContent *settext( const std::string&, const std::string& = "current" );
    TextContent *settext( const std::string&, int , const std::string& = "current" );

    template <typename F>
      F *addAlternative();
    template <typename F>
      F *addAlternative( const KWargs& );

    template <typename F>
      F *addAnnotation( const KWargs& args ) {
      F *res = 0;
      try {
	res = new F( mydoc, args);
      }
      catch( std::exception& ){
	delete res;
	throw;
      }
      append( res );
      return res;
    }
    PosAnnotation *addPosAnnotation( const KWargs& );
    LemmaAnnotation *addLemmaAnnotation( const KWargs& );
    std::vector<Alternative*> alternatives( const std::string& s = "" ) const { 
      return alternatives( BASE, s );
    }
    virtual std::vector<Alternative *> alternatives( ElementType,
						     const std::string& = ""
						     ) const { 
      throw NotImplementedError("alternatives() for " + _xmltag ); 
    }
    virtual std::string content() const {
      throw NoSuchAnnotation( "content" );
    }
    virtual Correction *correct( const std::vector<FoliaElement*>,
				 const std::vector<FoliaElement*>,
				 const std::vector<FoliaElement*>,
				 const std::vector<FoliaElement*>,
				 const KWargs& ){
      throw NotImplementedError("correct() for " + _xmltag );
    }
    virtual Correction* correct( FoliaElement*,
				 FoliaElement*,
				 const KWargs& ){
      throw NotImplementedError("correct() for " + _xmltag );
    }
    virtual Correction *correct( const std::string& = "" ){
      throw NotImplementedError("correct() for " + _xmltag ); };

    virtual std::string src() const {
      throw NotImplementedError("src() for " + _xmltag ); };

    virtual UnicodeString caption() const {
      throw NotImplementedError("caption() for " + _xmltag ); };

    void increfcount() { ++refcount; };
    FoliaElement *parent() const { return _parent; };
    void setParent( FoliaElement *p ) { _parent = p ; };
    void setAuth( bool b ){ _auth = b; };
    virtual std::vector<FoliaElement *> resolve() const {
      throw NotImplementedError("resolve() for " + _xmltag );
    };
    bool printable() const { return PRINTABLE; };
    bool isSubClass( ElementType ) const;
    bool isSubClass( const FoliaElement *c ) const {
      return isSubClass( c->element_id() );
    };
  protected:
    virtual void init()=0;
    virtual KWargs collectAttributes() const;
    virtual std::string getTextDelimiter( bool=false ) const { return TEXTDELIMITER; }
    virtual std::string generateId( const std::string& ){
      throw NotImplementedError( "generateId() not allowed for " + classname() );
    };
    virtual bool allowannotations() const { return false; };
    
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
    bool acceptable( ElementType ) const;
    bool checkAtts();
    bool addable( const FoliaElement * ) const;

    std::vector<FoliaElement*> data;
    FoliaElement *_parent;
    bool _auth;
    Document *mydoc;
    std::string _xmltag;
    ElementType _element_id;
    std::set<ElementType> _accepted_data;
    AnnotationType::AnnotationType _annotation_type;

    Attrib _required_attributes;
    Attrib _optional_attributes;
    size_t occurrences;
    size_t occurrences_per_set;
    std::string TEXTDELIMITER;
    bool PRINTABLE;
    bool AUTH;

    //common FoLiA attributes
    //not applicable to all subtypes, but taking this slight extra memory 
    //loss for granted. (Perhaps make them into pointers?)
    std::string _id;
    std::string _set;
    std::string _class;
    std::string _annotator;
    std::string _n;
    std::string _datetime;
    AnnotatorType _annotator_type;
    double _confidence;
    int refcount;
  private:
    void addFeatureNodes( const KWargs& args );
  };

  bool isSubClass( const ElementType e1, const ElementType e2 );

  inline bool isSubClass( const FoliaElement *e1, const FoliaElement *e2 ){
    return isSubClass( e1->element_id(), e2->element_id() );
  };

  bool operator==( const FoliaElement&, const FoliaElement& );
  inline bool operator!=( const FoliaElement& e1, const FoliaElement& e2 ){
    return !( e1 == e2 );
  }

  inline size_t len( const FoliaElement *e ) { 
    return e->size(); }

  template <typename T>
    inline size_t len( const std::vector<T>& v ) {
    return v.size(); }
  inline std::string str( const FoliaElement *e ) { 
    return e->str(); }
  inline UnicodeString text( const FoliaElement *e ) { 
    if ( e )
      return e->text();
    else
      throw ValueError( "text() for empty element" );
  }
  inline UnicodeString unicode( const FoliaElement *e ) {
    return e->unicode(); }
  inline bool isinstance( const FoliaElement *e, ElementType t ) { 
    return e->isinstance( t ); }

  class AllowGenerateID {
  public:
    void setMaxId( FoliaElement * );
    int getMaxId( const std::string& );
  protected:
    std::string IGgen( const std::string&, const std::string& );
  private:
    std::map<std::string, int> maxid;
  };

  class AllowCorrection {
  public:
    Correction *correctBase( FoliaElement *,
			     std::vector<FoliaElement*>,
			     std::vector<FoliaElement*>,
			     std::vector<FoliaElement*>,
			     std::vector<FoliaElement*>,
			     const KWargs& );
  };

  class AbstractStructureElement: public FoliaElement, AllowGenerateID, AllowCorrection {
  public:  
  AbstractStructureElement( Document *d=0 ): FoliaElement( d ) { classInit(); };
    
    std::string str() const;
    bool allowannotations() const { return true; };
    std::vector<Alternative *> alternatives( ElementType = BASE,
					     const std::string& = "" ) const;
  
    FoliaElement *append( FoliaElement* );
    Correction *correct( std::vector<FoliaElement*> v1,
			 std::vector<FoliaElement*> v2,
 			 std::vector<FoliaElement*> v3,
			 std::vector<FoliaElement*> v4,
			 const KWargs& args ) {
      return correctBase( this, v1, v2, v3, v4, args );
    }
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
    std::string generateId( const std::string& tag ){
      return IGgen( tag, _id ); 
    }
  private:
    void init();
  };

  class AbstractAnnotation: public FoliaElement {
  public:
  AbstractAnnotation( Document *d=0 ):  FoliaElement( d ){};
  };

  class AbstractTokenAnnotation: public AbstractAnnotation, AllowGenerateID {
  public:
  AbstractTokenAnnotation( Document *d=0 ):  AbstractAnnotation( d ){ classInit(); };
    std::string generateId( const std::string& tag ){
      return IGgen( tag, _id ); 
    }
  private:
    void init();
  };

  class Feature: public FoliaElement {
  public:
  Feature( const std::string& s=""): FoliaElement( ){ classInit( s ); }
  Feature( const KWargs& a ): FoliaElement( ){ classInit( a ); }
  Feature( Document *d, const std::string& s=""): FoliaElement( d ){ classInit( s ); }
  Feature( Document *d, const KWargs& a ): FoliaElement( d ){ classInit( a ); }
    void setAttributes( const KWargs& );
    KWargs collectAttributes() const;  
    std::string subset() const { return _subset; };

  protected:
    std::string _subset;
  
  private:
    void init();
  };

  class AbstractSpanAnnotation: public AbstractAnnotation, AllowGenerateID {
  public:
  AbstractSpanAnnotation( Document *d=0 ):  AbstractAnnotation( d ){ classInit(); };
    xmlNode *xml( bool, bool=false ) const;
    FoliaElement *append( FoliaElement* );
    std::string generateId( const std::string& tag ){
      return IGgen( tag, _id ); 
    }
    bool allowannotations() const { return true; };
    std::vector<FoliaElement*> wrefs() const;

  private:
    void init();
  };

  class TextContent: public FoliaElement {
  public:
  TextContent( const std::string& s="" ):  FoliaElement( ){ classInit( s ); }
  TextContent( const KWargs& a ):  FoliaElement( ){ classInit( a ); }
  TextContent( Document *d, const std::string& s="" ):  FoliaElement( d ){ classInit( s ); }
  TextContent( Document *d, const KWargs& a ):  FoliaElement( d ){ classInit( a ); }
    FoliaElement* parseXml( const xmlNode * );
    xmlNode *xml( bool, bool=false ) const;
    void setAttributes( const KWargs& );
    KWargs collectAttributes() const;  
    std::string str() const;
    UnicodeString text( const std::string& = "current", bool = false ) const;
    int offset() const { return _offset; };
    std::string getlang() const { 
      throw NotImplementedError("TextContent::getlang() is obsolete" ); 
    };
    std::string setlang( const std::string& l ) { 
      throw NotImplementedError("TextContent::setlang() is obsolete" ); 
    };
    FoliaElement *append( FoliaElement* ){ 
      throw NotImplementedError("TextContent::append() for " + _xmltag ); };
    TextContent *postappend();
    std::vector<FoliaElement*> findreplacables( FoliaElement * ) const;
  private:
    void init();
    int _offset;
    std::string _lang; // UNUSED!
    UnicodeString _text;
  };

  class FoLiA: public FoliaElement {
  public:
  FoLiA( const std::string& s="" ): FoliaElement( ) { classInit( s ); };
  FoLiA( const KWargs& a ): FoliaElement( ) { classInit( a ); };
  FoLiA( Document *d, const std::string& s=""): FoliaElement( d ) { classInit( s ); };
  FoLiA( Document *d, const KWargs& a ): FoliaElement( d ) { classInit( a ); };
  private:
    void init();
  };

  class DCOI: public FoliaElement {
  public:
  DCOI( const std::string& s=""): FoliaElement( ) { classInit( s ); };
  DCOI( const KWargs& a ): FoliaElement( ) { classInit( a ); };
  DCOI( Document *d, const std::string& s=""): FoliaElement( d ) { classInit( s ); };
  DCOI( Document *d, const KWargs& a ): FoliaElement( d ) { classInit( a ); };
  private:
    void init();
  };

  class Head: public AbstractStructureElement {
  public:
  Head( const std::string& s=""): AbstractStructureElement() {  classInit( s ); };
  Head( const KWargs& a ): AbstractStructureElement() {  classInit( a ); };
  Head( Document *d, const std::string& s=""): AbstractStructureElement( d ) {  classInit( s ); };
  Head( Document *d, const KWargs& a ): AbstractStructureElement( d ) {  classInit( a ); };
  private:
    void init();
  };

  class TableHead: public AbstractStructureElement {
  public:
  TableHead( const std::string& s=""): AbstractStructureElement() {  classInit( s ); };
  TableHead( const KWargs& a ): AbstractStructureElement() {  classInit( a ); };
  TableHead( Document *d, const std::string& s=""): AbstractStructureElement( d ) {  classInit( s ); };
  TableHead( Document *d, const KWargs& a ): AbstractStructureElement( d ) {  classInit( a ); };
  private:
    void init();
  };

  class Table: public AbstractStructureElement {
  public:
  Table( const std::string& s=""): AbstractStructureElement() {  classInit( s ); };
  Table( const KWargs& a ): AbstractStructureElement() {  classInit( a ); };
  Table( Document *d, const std::string& s=""): AbstractStructureElement( d ) {  classInit( s ); };
  Table( Document *d, const KWargs& a ): AbstractStructureElement( d ) {  classInit( a ); };
  private:
    void init();
  };

  class Row: public AbstractStructureElement {
  public:
  Row( const std::string& s=""): AbstractStructureElement() {  classInit( s ); };
  Row( const KWargs& a ): AbstractStructureElement() {  classInit( a ); };
  Row( Document *d, const std::string& s=""): AbstractStructureElement( d ) {  classInit( s ); };
  Row( Document *d, const KWargs& a ): AbstractStructureElement( d ) {  classInit( a ); };
  private:
    void init();
  };

  class Cell: public AbstractStructureElement {
  public:
  Cell( const std::string& s=""): AbstractStructureElement() {  classInit( s ); };
  Cell( const KWargs& a ): AbstractStructureElement() {  classInit( a ); };
  Cell( Document *d, const std::string& s=""): AbstractStructureElement( d ) {  classInit( s ); };
  Cell( Document *d, const KWargs& a ): AbstractStructureElement( d ) {  classInit( a ); };
  private:
    void init();
  };

  class Gap: public FoliaElement {
  public:
  Gap( const std::string& s=""): FoliaElement( ) { classInit( s ); };
  Gap( const KWargs& a ): FoliaElement( ) { classInit( a ); };
  Gap( Document *d, const std::string& s=""): FoliaElement( d ) { classInit( s ); };
  Gap( Document *d, const KWargs& a ): FoliaElement( d ) { classInit( a ); };
    std::string content() const;
  private:
    void init();
  };

  class Content: public FoliaElement {
  public:
  Content( const std::string& s=""): FoliaElement( ) { classInit( s ); };
  Content( const KWargs& a ): FoliaElement( ) { classInit( a ); };
  Content( Document *d, const std::string& s=""): FoliaElement( d ) { classInit( s ); };
  Content( Document *d, const KWargs& a ): FoliaElement( d ) { classInit( a ); };
    FoliaElement* parseXml( const xmlNode * );
    xmlNode *xml( bool, bool = false ) const;
    std::string content() const { return value; };
  private:
    void init();
    std::string value;
  };
  
  class MetricAnnotation: public FoliaElement {
  public:
  MetricAnnotation( const std::string& s=""): FoliaElement( ) { classInit( s ); };
  MetricAnnotation( const KWargs& a ): FoliaElement( ) { classInit( a ); };
  MetricAnnotation( Document *d, const std::string& s=""): FoliaElement( d ) { classInit( s ); };
  MetricAnnotation( Document *d, const KWargs& a ): FoliaElement( d ) { classInit( a ); };
  private:
    void init();
  };

  class Division: public AbstractStructureElement {
  public:
  Division( const std::string& s=""):  AbstractStructureElement() { classInit( s ); };
  Division( const KWargs& a ):  AbstractStructureElement() { classInit( a ); };
  Division( Document *d, const std::string& s=""):  AbstractStructureElement( d ) { classInit( s ); };
  Division( Document *d, const KWargs& a ):  AbstractStructureElement( d ) { classInit( a ); };
    Head *head() const;
  private:
    void init();
  };

  class LineBreak: public AbstractStructureElement {
  public:
  LineBreak( const std::string& s=""):  AbstractStructureElement(){ classInit( s ); };
  LineBreak( const KWargs& a ):  AbstractStructureElement(){ classInit( a ); };
  LineBreak( Document *d,  const std::string& s=""):  AbstractStructureElement( d ){ classInit( s ); };
  LineBreak( Document *d,  const KWargs& a ):  AbstractStructureElement( d ){ classInit( a ); };
  private:
    void init();
  };

  class WhiteSpace: public AbstractStructureElement {
  public:
  WhiteSpace( const std::string& s=""):  AbstractStructureElement(){ classInit( s ); };
  WhiteSpace( const KWargs& a ):  AbstractStructureElement(){ classInit( a ); };
  WhiteSpace( Document *d,  const std::string& s=""):  AbstractStructureElement( d ){ classInit( s ); };
  WhiteSpace( Document *d,  const KWargs& a ):  AbstractStructureElement( d ){ classInit( a ); };
  private:
    void init();
  };

  class Word: public AbstractStructureElement {
  public:
  Word( const std::string& s="" ): AbstractStructureElement(){ classInit( s ); };
  Word( const KWargs& a ):  AbstractStructureElement(){ classInit( a ); };
  Word( Document *d, const std::string& s=""):  AbstractStructureElement( d ){ classInit( s ); };
  Word( Document *d, const KWargs& a ):  AbstractStructureElement( d ){ classInit( a ); };
    Correction *correct( const std::string& = "" );
    Correction *correct( FoliaElement*,
			 FoliaElement*,
			 const KWargs& );
    FoliaElement *split( FoliaElement *, FoliaElement *,
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
    std::vector<FoliaElement*> findspans( ElementType, 
					  const std::string& = "" ) const;
    FoliaElement *append( FoliaElement *);
    const Word* resolveword( const std::string& ) const;
    void setAttributes( const KWargs& );
    KWargs collectAttributes() const;  
    std::string getTextDelimiter( bool retaintok=false) const { 
      if ( space || retaintok )
	return TEXTDELIMITER;
      else {
	return "";
      }
    }
  private:
    void init();
    bool space;
  };

  class String: public AbstractTokenAnnotation, AllowCorrection {
  public:
  String( const std::string& s="" ): AbstractTokenAnnotation(){ classInit( s ); };
  String( const KWargs& a ):  AbstractTokenAnnotation(){ classInit( a ); };
  String( Document *d, const std::string& s=""):  AbstractTokenAnnotation( d ){ classInit( s ); };
  String( Document *d, const KWargs& a ):  AbstractTokenAnnotation( d ){ classInit( a ); };
    Correction *correct( std::vector<FoliaElement*> v1,
			 std::vector<FoliaElement*> v2,
 			 std::vector<FoliaElement*> v3,
			 std::vector<FoliaElement*> v4,
			 const KWargs& args ) {
      return correctBase( this, v1, v2, v3, v4, args );
    }
  private:
    void init();
  };

  class PlaceHolder: public Word {
  public:
  PlaceHolder( const std::string& s=""): Word(){ classInit( s ); };
  PlaceHolder( const KWargs& a ): Word(){ classInit( a ); };
  PlaceHolder( Document *d, const std::string& s=""): Word( d ){ classInit( s ); };
  PlaceHolder( Document *d, const KWargs& a ): Word( d ){ classInit( a ); };
    void setAttributes( const KWargs& );
  private:
    void init();
  };

  class Sentence: public AbstractStructureElement {
  public:
  Sentence( const std::string& s=""):  AbstractStructureElement(){ classInit( s ); }
  Sentence( const KWargs& a ):  AbstractStructureElement(){ classInit( a ); }
  Sentence( Document *d, const std::string& s=""):  AbstractStructureElement( d ){ classInit( s ); }
  Sentence( Document *d, const KWargs& a ):  AbstractStructureElement( d ){ classInit( a ); }
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
    Correction *correctWords( const std::vector<FoliaElement *>&,
			      const std::vector<FoliaElement *>&,
			      const std::vector<FoliaElement *>&, 
			      const KWargs& );
    void init();
  };

  class Text: public AbstractStructureElement {
  public:
  Text( const std::string& s=""):  AbstractStructureElement(){ classInit( s ); };
  Text( const KWargs& a ):  AbstractStructureElement(){ classInit( a ); };
  Text( Document *d, const std::string& s=""):  AbstractStructureElement( d ){ classInit( s ); };
  Text( Document *d, const KWargs& a ):  AbstractStructureElement( d ){ classInit( a ); };
  private:
    void init();
  };

  class Event: public AbstractStructureElement {
  public:
  Event( const std::string& s=""):  AbstractStructureElement(){ classInit( s ); };
  Event( const KWargs& a ):  AbstractStructureElement(){ classInit( a ); };
  Event( Document *d, const std::string& s=""):  AbstractStructureElement( d ){ classInit( s ); };
  Event( Document *d, const KWargs& a ):  AbstractStructureElement( d ){ classInit( a ); };
  private:
    void init();
  };

  class Caption: public AbstractStructureElement {
  public:
  Caption( const std::string& s="" ): AbstractStructureElement(){ classInit( s ); };
  Caption( const KWargs& a ): AbstractStructureElement(){ classInit( a ); };
  Caption( Document *d, const std::string& s=""):  AbstractStructureElement( d ){ classInit( s ); };
  Caption( Document *d, const KWargs& a ):  AbstractStructureElement( d ){ classInit( a ); };
  private:
    void init();
  };

  class Label: public AbstractStructureElement {
  public:
  Label( const std::string& s=""):  AbstractStructureElement(){ classInit( s ); };
  Label( const KWargs& a ): AbstractStructureElement(){ classInit( a ); };
  Label( Document *d, const std::string& s="" ):  AbstractStructureElement( d ){ classInit( s ); };
  Label( Document *d, const KWargs& a ): AbstractStructureElement( d ){ classInit( a ); };
  private:
    void init();
  };

  class ListItem: public AbstractStructureElement {
  public:
  ListItem( const std::string& s=""):  AbstractStructureElement(){ classInit( s ); };
  ListItem( const KWargs& a ):  AbstractStructureElement(){ classInit( a ); };
  ListItem( Document *d, const std::string& s=""):  AbstractStructureElement( d ){ classInit( s ); };
  ListItem( Document *d, const KWargs& a ):  AbstractStructureElement( d ){ classInit( a ); };
  private:
    void init();
  };

  class List: public AbstractStructureElement {
  public:
  List( const std::string& s=""):  AbstractStructureElement(){ classInit( s ); };
  List( const KWargs& a ):  AbstractStructureElement(){ classInit( a ); };
  List( Document *d, const std::string& s=""):  AbstractStructureElement( d ){ classInit( s ); };
  List( Document *d, const KWargs& a ):  AbstractStructureElement( d ){ classInit( a ); };
  private:
    void init();
  };

  class Figure: public AbstractStructureElement {
  public:
  Figure( const std::string& s=""):  AbstractStructureElement(){ classInit( s ); };
  Figure( const KWargs& a ):  AbstractStructureElement(){ classInit( a ); };
  Figure( Document *d, const std::string& s="" ):  AbstractStructureElement( d ){ classInit( s ); };
  Figure( Document *d, const KWargs& a ):  AbstractStructureElement( d ){ classInit( a ); };
    void setAttributes( const KWargs& );
    KWargs collectAttributes() const;  
    std::string src() const { return _src; };
    UnicodeString caption() const;
  private:
    void init();
    std::string _src;
  };

  class Paragraph: public AbstractStructureElement {
  public:
  Paragraph( const std::string& s=""):  AbstractStructureElement(){ classInit( s ); };
  Paragraph( const KWargs& a ):  AbstractStructureElement(){ classInit( a ); };
  Paragraph( Document *d, const std::string& s=""):  AbstractStructureElement( d ){ classInit( s ); };
  Paragraph( Document *d, const KWargs& a ):  AbstractStructureElement( d ){ classInit( a ); };
  private:
    void init();
  };

  class Alternative: public AbstractStructureElement {
  public:
  Alternative( const std::string& s=""):  AbstractStructureElement(){ classInit( s ); };
  Alternative( const KWargs& a ):  AbstractStructureElement(){ classInit( a ); };
  Alternative( Document *d, const std::string& s=""):  AbstractStructureElement( d ){ classInit( s ); };
  Alternative( Document *d, const KWargs& a ):  AbstractStructureElement( d ){ classInit( a ); };
    bool allowannotations() const { return true; };
  private:
    void init();
  };

  
  class AlternativeLayers: public FoliaElement {
  public:
  AlternativeLayers( const std::string& s=""): FoliaElement(){ classInit( s ); };
  AlternativeLayers( const KWargs& a ): FoliaElement(){ classInit( a ); };
  AlternativeLayers( Document *d, const std::string& s=""): FoliaElement( d ){ classInit( s ); };
  AlternativeLayers( Document *d, const KWargs& a ): FoliaElement( d ){ classInit( a ); };
  private:
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
      alt = new Alternative( mydoc, kw );
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
  public:
  PosAnnotation( const std::string& s=""): AbstractTokenAnnotation(){ classInit( s ); };
  PosAnnotation( const KWargs& a ): AbstractTokenAnnotation(){ classInit( a ); };
  PosAnnotation( Document *d, const std::string& s=""): AbstractTokenAnnotation( d ){ classInit( s ); };
  PosAnnotation( Document *d, const KWargs& a ): AbstractTokenAnnotation( d ){ classInit( a ); };
  private:
    void init();
  };

  class LemmaAnnotation: public AbstractTokenAnnotation {
  public:
  LemmaAnnotation( const std::string& s=""): AbstractTokenAnnotation(){ classInit( s ); };
  LemmaAnnotation( const KWargs& a ): AbstractTokenAnnotation(){ classInit( a ); };
  LemmaAnnotation( Document *d, const std::string& s=""): AbstractTokenAnnotation( d ){ classInit( s ); };
  LemmaAnnotation( Document *d, const KWargs& a ): AbstractTokenAnnotation( d ){ classInit( a ); };
  private:
    void init();
  };

  class LangAnnotation: public AbstractTokenAnnotation {
  public:
  LangAnnotation( const std::string& s=""): AbstractTokenAnnotation(){ classInit( s ); };
  LangAnnotation( const KWargs& a ): AbstractTokenAnnotation(){ classInit( a ); };
  LangAnnotation( Document *d, const std::string& s=""): AbstractTokenAnnotation( d ){ classInit( s ); };
  LangAnnotation( Document *d, const KWargs& a ): AbstractTokenAnnotation( d ){ classInit( a ); };
  private:
    void init();
  };

  class PhonAnnotation: public AbstractTokenAnnotation {
  public:
  PhonAnnotation( const std::string& s=""): AbstractTokenAnnotation(){ classInit( s ); };
  PhonAnnotation( const KWargs& a ): AbstractTokenAnnotation(){ classInit( a ); };
  PhonAnnotation( Document *d, const std::string& s=""): AbstractTokenAnnotation( d ){ classInit( s ); };
  PhonAnnotation( Document *d, const KWargs& a ): AbstractTokenAnnotation( d ){ classInit( a ); };
  private:
    void init();
  };

  class DomainAnnotation: public AbstractTokenAnnotation {
  public:
  DomainAnnotation(  const std::string& s=""): AbstractTokenAnnotation(){ classInit( s ); };
  DomainAnnotation(  const KWargs& a ): AbstractTokenAnnotation(){ classInit( a ); };
  DomainAnnotation( Document *d, const std::string& s=""): AbstractTokenAnnotation( d ){ classInit( s ); };
  DomainAnnotation( Document *d, const KWargs& a ): AbstractTokenAnnotation( d ){ classInit( a ); };
  private:
    void init();
  };

  class SenseAnnotation: public AbstractTokenAnnotation {
  public:
  SenseAnnotation( const std::string& s="" ): AbstractTokenAnnotation(){ classInit( s ); };
  SenseAnnotation( const KWargs& a ): AbstractTokenAnnotation(){ classInit( a ); };
  SenseAnnotation( Document *d, const std::string& s="" ): AbstractTokenAnnotation( d ){ classInit( s ); };
  SenseAnnotation( Document *d, const KWargs& a ): AbstractTokenAnnotation( d ){ classInit( a ); };
  private:
    void init();
  };

  class SubjectivityAnnotation: public AbstractTokenAnnotation {
  public:
  SubjectivityAnnotation( const std::string& s="" ): AbstractTokenAnnotation(){ classInit( s ); };
  SubjectivityAnnotation( const KWargs& a ): AbstractTokenAnnotation(){ classInit( a ); };
  SubjectivityAnnotation( Document *d, const std::string& s="" ): AbstractTokenAnnotation( d ){ classInit( s ); };
  SubjectivityAnnotation( Document *d, const KWargs& a ): AbstractTokenAnnotation( d ){ classInit( a ); };
  private:
    void init();
  };

  class Quote: public AbstractStructureElement {
  public:
  Quote( const std::string& s="" ): AbstractStructureElement(){ classInit( s ); };
  Quote( const KWargs& a ): AbstractStructureElement(){ classInit( a ); };
  Quote( Document *d, const std::string& s="" ): AbstractStructureElement( d ){ classInit( s ); };
  Quote( Document *d, const KWargs& a ): AbstractStructureElement( d ){ classInit( a ); };
    FoliaElement *append( FoliaElement *);
    std::vector<Word*> wordParts() const;
  private:
    void init();
  };

  class BeginDateTimeFeature: public Feature {
  public:
  BeginDateTimeFeature( const std::string& s="" ): Feature( ){ classInit( s ); }
  BeginDateTimeFeature( const KWargs& a ): Feature( ){ classInit( a ); }
  BeginDateTimeFeature( Document *d, const std::string& s="" ): Feature( d ){ classInit( s ); }
  BeginDateTimeFeature( Document *d, const KWargs& a ): Feature( d ){ classInit( a ); }
  private:
    void init();
  };

  class EndDateTimeFeature: public Feature {
  public:
  EndDateTimeFeature( const std::string& s="" ): Feature( ){ classInit( s ); }
  EndDateTimeFeature( const KWargs& a ): Feature( ){ classInit( a ); }
  EndDateTimeFeature( Document *d, const std::string& s="" ): Feature( d ){ classInit( s ); }
  EndDateTimeFeature( Document *d, const KWargs& a ): Feature( d ){ classInit( a ); }
  private:
    void init();
  };

  class SynsetFeature: public Feature {
  public:
  SynsetFeature( const std::string& s="" ): Feature( ){ classInit( s ); }
  SynsetFeature( const KWargs& a ): Feature( ){ classInit( a ); }
  SynsetFeature( Document *d, const std::string& s="" ): Feature( d ){ classInit( s ); }
  SynsetFeature( Document *d, const KWargs& a ): Feature( d ){ classInit( a ); }
  private:
    void init();
  };

  class ActorFeature: public Feature {
  public:
  ActorFeature( const std::string& s="" ): Feature( ){ classInit( s ); }
  ActorFeature( const KWargs& a ): Feature( ){ classInit( a ); }
  ActorFeature( Document *d, const std::string& s="" ): Feature( d ){ classInit( s ); }
  ActorFeature( Document *d, const KWargs& a ): Feature( d ){ classInit( a ); }
  private:
    void init();
  };

  class HeadFeature: public Feature {
  public:
  HeadFeature( const std::string& s="" ): Feature( ){ classInit( s ); }
  HeadFeature( const KWargs& a ): Feature( ){ classInit( a ); }
  HeadFeature( Document *d, const std::string& s="" ): Feature( d ){ classInit( s ); }
  HeadFeature( Document *d, const KWargs& a ): Feature( d ){ classInit( a ); }
  private:
    void init();
  };

  class ValueFeature: public Feature {
  public:
  ValueFeature( const std::string& s="" ): Feature( ){ classInit( s ); }
  ValueFeature( const KWargs& a ): Feature( ){ classInit( a ); }
  ValueFeature( Document *d, const std::string& s="" ): Feature( d ){ classInit( s ); }
  ValueFeature( Document *d, const KWargs& a ): Feature( d ){ classInit( a ); }
  private:
    void init();
  };

  class FunctionFeature: public Feature {
  public:
  FunctionFeature( const std::string& s="" ): Feature( ){ classInit( s ); }
  FunctionFeature( const KWargs& a ): Feature( ){ classInit( a ); }
  FunctionFeature( Document *d, const std::string& s="" ): Feature( d ){ classInit( s ); }
  FunctionFeature( Document *d, const KWargs& a ): Feature( d ){ classInit( a ); }
  private:
    void init();
  };

  class TimeFeature: public Feature {
  public:
  TimeFeature( const std::string& s="" ): Feature( ){ classInit( s ); }
  TimeFeature( const KWargs& a ): Feature( ){ classInit( a ); }
  TimeFeature( Document *d, const std::string& s="" ): Feature( d ){ classInit( s ); }
  TimeFeature( Document *d, const KWargs& a ): Feature( d ){ classInit( a ); }
  private:
    void init();
  };

  class LevelFeature: public Feature {
  public:
  LevelFeature( const std::string& s="" ): Feature( ){ classInit( s ); }
  LevelFeature( const KWargs& a ): Feature( ){ classInit( a ); }
  LevelFeature( Document *d, const std::string& s="" ): Feature( d ){ classInit( s ); }
  LevelFeature( Document *d, const KWargs& a ): Feature( d ){ classInit( a ); }
  private:
    void init();
  };

  class ModalityFeature: public Feature {
  public:
  ModalityFeature( const std::string& s="" ): Feature( ){ classInit( s ); }
  ModalityFeature( const KWargs& a ): Feature( ){ classInit( a ); }
  ModalityFeature( Document *d, const std::string& s="" ): Feature( d ){ classInit( s ); }
  ModalityFeature( Document *d, const KWargs& a ): Feature( d ){ classInit( a ); }
  private:
    void init();
  };

  class WordReference: public FoliaElement {
  public:
  WordReference( const std::string& s="" ): FoliaElement( ){ classInit( s ); };
  WordReference( const KWargs& a ): FoliaElement( ){ classInit( a ); };
  WordReference( Document *d, const std::string& s="" ): FoliaElement( d ){ classInit( s ); };
  WordReference( Document *d, const KWargs& a ): FoliaElement( d ){ classInit( a ); };
  private:
    void init();
    FoliaElement* parseXml( const xmlNode *node );
  };  

  class Alignment: public FoliaElement {
  public:
  Alignment( const std::string& s="" ): FoliaElement( ){ classInit( s ); };
  Alignment( const KWargs& a ): FoliaElement( ){ classInit( a ); };
  Alignment( Document *d, const std::string& s="" ): FoliaElement( d ){ classInit( s ); };
  Alignment( Document *d, const KWargs& a ): FoliaElement( d ){ classInit( a ); };
    std::string href() const { return _href; };
    std::vector<FoliaElement *>resolve() const;
    KWargs collectAttributes() const;  
    void setAttributes( const KWargs& );
  private:
    void init();
    std::string _href;
    std::string _type;
  };  

  class AlignReference: public FoliaElement {
    friend class Alignment;
  public:
  AlignReference( const std::string& s="" ): FoliaElement( ){ classInit( s ); };
  AlignReference( const KWargs& a ): FoliaElement( ){ classInit( a ); };
  AlignReference( Document *d, const std::string& s="" ): FoliaElement( d ){ classInit( s ); };
  AlignReference( Document *d, const KWargs& a ): FoliaElement( d ){ classInit( a ); };
    KWargs collectAttributes() const;  
    void setAttributes( const KWargs& );
  private:
    void init();
    FoliaElement* parseXml( const xmlNode *node );
    FoliaElement *resolve( const Alignment *ref ) const;
    std::string refId;
    std::string _type;
    std::string _t;
  };  

  class SyntacticUnit: public AbstractSpanAnnotation {
  public:
  SyntacticUnit( const std::string& s="" ): AbstractSpanAnnotation( ){ classInit( s ); }
  SyntacticUnit( const KWargs& a ): AbstractSpanAnnotation( ){ classInit( a ); }
  SyntacticUnit( Document *d, const std::string& s="" ): AbstractSpanAnnotation( d ){ classInit( s ); }
  SyntacticUnit( Document *d, const KWargs& a ): AbstractSpanAnnotation( d ){ classInit( a ); }
  private:
    void init();
  };

  class Chunk: public AbstractSpanAnnotation {
  public:
  Chunk( const std::string& s="" ): AbstractSpanAnnotation( ){ classInit( s ); }
  Chunk( const KWargs& a ): AbstractSpanAnnotation( ){ classInit( a ); }
  Chunk( Document *d, const std::string& s="" ): AbstractSpanAnnotation( d ){ classInit( s ); }
  Chunk( Document *d, const KWargs& a ): AbstractSpanAnnotation( d ){ classInit( a ); }
  private:
    void init();
  };

  class Entity: public AbstractSpanAnnotation {
  public:
  Entity( const std::string& s="" ): AbstractSpanAnnotation( ){ classInit( s ); }
  Entity( const KWargs& a ): AbstractSpanAnnotation( ){ classInit( a ); }
  Entity( Document *d, const std::string& s="" ): AbstractSpanAnnotation( d ){ classInit( s ); }
  Entity( Document *d, const KWargs& a ): AbstractSpanAnnotation( d ){ classInit( a ); }
  private:
    void init();
  };

  class Headwords: public AbstractSpanAnnotation {
  public:
  Headwords( const std::string& s="" ): AbstractSpanAnnotation( ){ classInit( s ); }
  Headwords( const KWargs& a ): AbstractSpanAnnotation( ){ classInit( a ); }
  Headwords( Document *d, const std::string& s="" ): AbstractSpanAnnotation( d ){ classInit( s ); }
  Headwords( Document *d, const KWargs& a ): AbstractSpanAnnotation( d ){ classInit( a ); }
  private:
    void init();
  };

  class DependencyDependent: public AbstractSpanAnnotation {
  public:
  DependencyDependent( const std::string& s="" ): AbstractSpanAnnotation( ){ classInit( s ); }
  DependencyDependent( const KWargs& a ): AbstractSpanAnnotation( ){ classInit( a ); }
  DependencyDependent( Document *d, const std::string& s="" ): AbstractSpanAnnotation( d ){ classInit( s ); }
  DependencyDependent( Document *d, const KWargs& a ): AbstractSpanAnnotation( d ){ classInit( a ); }
  private:
    void init();
  };

  class Dependency: public AbstractSpanAnnotation {
  public:
  Dependency( const std::string& s="" ): AbstractSpanAnnotation( ){ classInit( s ); }
  Dependency( const KWargs& a ): AbstractSpanAnnotation( ){ classInit( a ); }
  Dependency( Document *d, const std::string& s="" ): AbstractSpanAnnotation( d ){ classInit( s ); }
  Dependency( Document *d, const KWargs& a ): AbstractSpanAnnotation( d ){ classInit( a ); }
    Headwords *head() const;
    DependencyDependent *dependent() const;
  private:
    void init();
  };

  class CoreferenceLink: public AbstractSpanAnnotation {
  public:
  CoreferenceLink( const std::string& s="" ): AbstractSpanAnnotation(){ classInit( s ); }
  CoreferenceLink( const KWargs& a ): AbstractSpanAnnotation(){ classInit( a ); }
  CoreferenceLink( Document *d, const std::string& s="" ): AbstractSpanAnnotation( d ){ classInit( s ); }
  CoreferenceLink( Document *d, const KWargs& a ): AbstractSpanAnnotation( d ){ classInit( a ); }
  private:
    void init();
  };

  class CoreferenceChain: public AbstractSpanAnnotation {
  public:
  CoreferenceChain( const std::string& s="" ): AbstractSpanAnnotation(){ classInit( s ); }
  CoreferenceChain( const KWargs& a ): AbstractSpanAnnotation(){ classInit( a ); }
  CoreferenceChain( Document *d, const std::string& s="" ): AbstractSpanAnnotation( d ){ classInit( s ); }
  CoreferenceChain( Document *d, const KWargs& a ): AbstractSpanAnnotation( d ){ classInit( a ); }
  private:
    void init();
  };

  class SemanticRole: public AbstractSpanAnnotation {
  public:
  SemanticRole( const std::string& s="" ): AbstractSpanAnnotation(){ classInit( s ); }
  SemanticRole( const KWargs& a ): AbstractSpanAnnotation(){ classInit( a ); }
  SemanticRole( Document *d, const std::string& s="" ): AbstractSpanAnnotation( d ){ classInit( s ); }
  SemanticRole( Document *d, const KWargs& a ): AbstractSpanAnnotation( d ){ classInit( a ); }
  private:
    void init();
  };

  class AbstractAnnotationLayer: public FoliaElement, AllowGenerateID {
  public:
  AbstractAnnotationLayer( const std::string& s=""): FoliaElement( ) { classInit( s ); };
  AbstractAnnotationLayer( const KWargs& a ): FoliaElement( ) { classInit( a ); };
  AbstractAnnotationLayer( Document *d, const std::string& s=""): FoliaElement( d ) { classInit( s ); };
  AbstractAnnotationLayer( Document *d, const KWargs& a ): FoliaElement( d ) { classInit( a ); };
    std::string generateId( const std::string& tag ){
      return IGgen( tag, _id ); 
    }
    bool allowannotations() const { return true; };
    FoliaElement *findspan( const std::vector<FoliaElement*>& ) const;
  private:
    void init();
  };

  class AbstractCorrectionChild: public FoliaElement {
  public:
  AbstractCorrectionChild( Document *d=0 ):  FoliaElement( d ){ classInit(); };
  private:
    void init();
  };

  class NewElement: public AbstractCorrectionChild {
  public:
  NewElement( const std::string& s=""): AbstractCorrectionChild( ) { classInit( s ); };
  NewElement( const KWargs& a ): AbstractCorrectionChild( ) { classInit( a ); };
  NewElement( Document *d, const std::string& s=""): AbstractCorrectionChild( d ) { classInit( s ); };
  NewElement( Document *d, const KWargs& a ): AbstractCorrectionChild( d ) { classInit( a ); };
  private:
    void init();
  };

  class Current: public AbstractCorrectionChild {
  public:
  Current( const std::string& s=""): AbstractCorrectionChild( ) { classInit( s ); };
  Current( const KWargs& a ): AbstractCorrectionChild( ) { classInit( a ); };
  Current( Document *d, const std::string& s=""): AbstractCorrectionChild( d ) { classInit( s ); };
  Current( Document *d, const KWargs& a ): AbstractCorrectionChild( d ) { classInit( a ); };
  private:
    void init();
  };

  class Original: public AbstractCorrectionChild {
  public:
  Original( const std::string& s=""): AbstractCorrectionChild( ) { classInit( s ); };
  Original( const KWargs& a ): AbstractCorrectionChild( ) { classInit( a ); };
  Original( Document *d, const std::string& s=""): AbstractCorrectionChild( d ) { classInit( s ); };
  Original( Document *d, const KWargs& a ): AbstractCorrectionChild( d ) { classInit( a ); };
  private:
    void init();
  };

  class Suggestion: public AbstractCorrectionChild {
  public:
  Suggestion( const std::string& s=""): AbstractCorrectionChild( ) { classInit( s ); };
  Suggestion( const KWargs& a ): AbstractCorrectionChild( ) { classInit( a ); };
  Suggestion( Document *d, const std::string& s=""): AbstractCorrectionChild( d ) { classInit( s ); };
  Suggestion( Document *d, const KWargs& a ): AbstractCorrectionChild( d ) { classInit( a ); };
  private:
    void init();
  };

  class Description: public FoliaElement {
  public:
  Description( const std::string& s=""): FoliaElement( ) { classInit( s ); };
  Description( const KWargs& a ): FoliaElement( ) { classInit( a ); };
  Description( Document *d, const std::string& s="" ): FoliaElement( d ) { classInit( s ); };
  Description( Document *d, const KWargs& a ): FoliaElement( d ) { classInit( a ); };
    std::string description() const { return _value; };
    void setAttributes( const KWargs& kwargs );
    FoliaElement* parseXml( const xmlNode * );
    xmlNode *xml( bool, bool=false ) const;
  private:
    void init();
    std::string _value;
  };

  class XmlComment: public FoliaElement {
  public:
  XmlComment( const std::string& s=""): FoliaElement( ) { classInit( s ); };
  XmlComment( const KWargs& a ): FoliaElement( ) { classInit( a ); };
  XmlComment( Document *d, const std::string& s="" ): FoliaElement( d ) { classInit( s ); };
  XmlComment( Document *d, const KWargs& a ): FoliaElement( d ) { classInit( a ); };
    FoliaElement* parseXml( const xmlNode * );
    xmlNode *xml( bool, bool=false ) const;
  private:
    void init();
    std::string _value;
  };

  class Correction: public AbstractTokenAnnotation {
  public:
  Correction( const std::string& s=""): AbstractTokenAnnotation( ){ classInit( s ); }
  Correction( const KWargs& a ): AbstractTokenAnnotation( ){ classInit( a ); }
  Correction( Document *d, const std::string& s=""): AbstractTokenAnnotation( d ){ classInit( s ); }
  Correction( Document *d, const KWargs& a ): AbstractTokenAnnotation( d ){ classInit( a ); }
    bool hasNew() const;
    bool hasOriginal() const;
    bool hasCurrent() const;
    bool hasSuggestions() const;
    NewElement *getNew() const;
    Original *getOriginal() const;
    Current *getCurrent() const;
    std::vector<Suggestion*> suggestions() const;
    Suggestion *suggestions( size_t ) const;
    UnicodeString text( const std::string& = "current", bool = false ) const;
    TextContent *textcontent( const std::string& = "current" ) const;
  private:
    void init();
  };

  class ErrorDetection: public AbstractTokenAnnotation  {
  public: 
  ErrorDetection( const std::string& s=""): AbstractTokenAnnotation(){ classInit( s ); }
  ErrorDetection( const KWargs& a ): AbstractTokenAnnotation(){ classInit( a ); }
  ErrorDetection( Document *d, const std::string& s=""): AbstractTokenAnnotation( d ){ classInit( s ); }
  ErrorDetection( Document *d, const KWargs& a ): AbstractTokenAnnotation( d ){ classInit( a ); }
  private:
    void init();
  };

  class TimeSegment: public AbstractSpanAnnotation {
  public:
  TimeSegment( const std::string& s="" ): AbstractSpanAnnotation(){ classInit( s ); }
  TimeSegment( const KWargs& a ): AbstractSpanAnnotation(){ classInit( a ); }
  TimeSegment( Document *d, const std::string& s="" ): AbstractSpanAnnotation( d ){ classInit( s ); }
  TimeSegment( Document *d, const KWargs& a ): AbstractSpanAnnotation( d ){ classInit( a ); }
  private:
    void init();
  };

  class Morpheme: public AbstractStructureElement {
  public:
  Morpheme( const std::string& s="" ): AbstractStructureElement(){ classInit( s ); }
  Morpheme( const KWargs& a ): AbstractStructureElement(){ classInit( a ); }
  Morpheme( Document *d, const std::string& s="" ): AbstractStructureElement( d ){ classInit( s ); }
  Morpheme( Document *d, const KWargs& a ): AbstractStructureElement( d ){ classInit( a ); }
  private:
    void init();
  };

  class SyntaxLayer: public AbstractAnnotationLayer {
  public:
  SyntaxLayer( const std::string& s=""): AbstractAnnotationLayer(){ classInit( s ); }
  SyntaxLayer( const KWargs& a ): AbstractAnnotationLayer(){ classInit( a ); }
  SyntaxLayer( Document *d, const std::string& s=""): AbstractAnnotationLayer( d ){ classInit( s ); }
  SyntaxLayer( Document *d, const KWargs& a ): AbstractAnnotationLayer( d ){ classInit( a ); }
  private:
    void init();
  };

  class ChunkingLayer: public AbstractAnnotationLayer {
  public:
  ChunkingLayer( const std::string& s=""): AbstractAnnotationLayer(){ classInit( s ); }
  ChunkingLayer( const KWargs& a ): AbstractAnnotationLayer(){ classInit( a ); }
  ChunkingLayer( Document *d, const std::string& s=""): AbstractAnnotationLayer( d ){ classInit( s ); }
  ChunkingLayer( Document *d, const KWargs& a ): AbstractAnnotationLayer( d ){ classInit( a ); }
  private:
    void init();
  };

  class EntitiesLayer: public AbstractAnnotationLayer {
  public:
  EntitiesLayer( const std::string& s=""): AbstractAnnotationLayer(){ classInit( s ); }
  EntitiesLayer( const KWargs& a ): AbstractAnnotationLayer(){ classInit( a ); }
  EntitiesLayer( Document *d, const std::string& s=""): AbstractAnnotationLayer( d ){ classInit( s ); }
  EntitiesLayer( Document *d, const KWargs& a ): AbstractAnnotationLayer( d ){ classInit( a ); }
  private:
    void init();
  };

  class TimingLayer: public AbstractAnnotationLayer {
  public:
  TimingLayer( const std::string& s=""): AbstractAnnotationLayer(){ classInit( s ); }
  TimingLayer( const KWargs& a ): AbstractAnnotationLayer(){ classInit( a ); }
  TimingLayer( Document *d, const std::string& s=""): AbstractAnnotationLayer( d ){ classInit( s ); }
  TimingLayer( Document *d, const KWargs& a ): AbstractAnnotationLayer( d ){ classInit( a ); }
  private:
    void init();
  };

  class MorphologyLayer: public AbstractAnnotationLayer {
  public:
  MorphologyLayer( const std::string& s="" ): AbstractAnnotationLayer(){ classInit( s ); }
  MorphologyLayer( const KWargs& a ): AbstractAnnotationLayer(){ classInit( a ); }
  MorphologyLayer( Document *d, const std::string& s="" ): AbstractAnnotationLayer( d ){ classInit( s ); }
  MorphologyLayer( Document *d, const KWargs& a ): AbstractAnnotationLayer( d ){ classInit( a ); }
  private:
    void init();
  };

  class DependenciesLayer: public AbstractAnnotationLayer {
  public:
  DependenciesLayer( const std::string& s=""): AbstractAnnotationLayer(){ classInit( s ); }
  DependenciesLayer( const KWargs& a ): AbstractAnnotationLayer(){ classInit( a ); }
  DependenciesLayer( Document *d, const std::string& s=""): AbstractAnnotationLayer( d ){ classInit( s ); }
  DependenciesLayer( Document *d, const KWargs& a ): AbstractAnnotationLayer( d ){ classInit( a ); }
  private:
    void init();
  };

  class CoreferenceLayer: public AbstractAnnotationLayer {
  public:
  CoreferenceLayer( const std::string& s=""): AbstractAnnotationLayer(){ classInit( s ); }
  CoreferenceLayer( const KWargs& a ): AbstractAnnotationLayer(){ classInit( a ); }
  CoreferenceLayer( Document *d, const std::string& s=""): AbstractAnnotationLayer( d ){ classInit( s ); }
  CoreferenceLayer( Document *d, const KWargs& a ): AbstractAnnotationLayer( d ){ classInit( a ); }
  private:
    void init();
  };

  class SemanticRolesLayer: public AbstractAnnotationLayer {
  public:
  SemanticRolesLayer( const std::string& s=""): AbstractAnnotationLayer(){ classInit( s ); }
  SemanticRolesLayer( const KWargs& a ): AbstractAnnotationLayer(){ classInit( a ); }
  SemanticRolesLayer( Document *d, const std::string& s=""): AbstractAnnotationLayer( d ){ classInit( s ); }
  SemanticRolesLayer( Document *d, const KWargs& a ): AbstractAnnotationLayer( d ){ classInit( a ); }
  private:
    void init();
  };

  std::string VersionName();
  std::string Version();

} // namespace folia

#endif
