/*
  $Id$
  $URL$

  Copyright (c) 1998 - 2012
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
#include "foliautils.h"

namespace folia {
  class Document;
  class FoliaElement;
  class AbstractTokenAnnotation;
  class Alternative;
  class PosAnnotation;
  class LemmaAnnotation;
  class Sentence;
  class Word;
  class TextContent;
  class Alternative;
  class Correction;
  class Suggestion;
  class Division;
  class DependencyDependent;
  class Paragraph;

  class FoliaElement {
    friend std::ostream& operator<<( std::ostream&, const FoliaElement& );
    friend std::ostream& operator<<( std::ostream&, const FoliaElement* );
    friend bool operator==( const FoliaElement&, const FoliaElement& );
    friend class Document;
    friend class Correction;
  public:
    //Constructor  
    FoliaElement( Document* =0 );
    virtual ~FoliaElement();

    static FoliaElement *createElement( Document *, const std::string&  );

    void classInit( const std::string& s ){
      init(); // virtual init
      if ( !s.empty() ){
	// virtual is only called when s != ""
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
      std::vector<F*> annotations( ) const {
      if ( allowannotations() ){
	std::vector<F*> v = select<F>();
	if ( v.size() >= 1 )
	  return v;
	else {
	  F obj("");
	  throw NoSuchAnnotation( obj.classname() );
	}
      }
      else {
	throw NotImplementedError( "annotations() for " + _xmltag ); 
      };
    }
    
    template <typename F> 
      F *annotation() const {
      std::vector<F*>v = annotations<F>();
      return v[0]; // always exist, otherwise annotations would throw()
    }

    template <typename F>
      F *annotation( const std::string& val ) const {
      if ( allowannotations() ){
	// Will return a SINGLE annotation (even if there are multiple). 
	// Raises a NoSuchAnnotation exception if none was found
	std::vector<F*>v = select<F>( val );
	if ( v.size() >= 1 )
	  return v[0];
	else {
	  F obj("");
	  throw NoSuchAnnotation( obj.classname() );
	}
      }
      else {
	throw NotImplementedError( "annotation() for " + _xmltag ); 
      }
    }

    std::string feat( const std::string& ) const;
    //XML (de)serialisation
    std::string xmlstring() const; // serialize to a string (XML fragment)
    virtual xmlNode *xml( bool ) const; //serialize to XML  
    virtual FoliaElement* parseXml( const xmlNode * );
    virtual std::string str() const;
    UnicodeString unicode() const { return text(); };
    virtual UnicodeString text( const std::string& = "current" ) const;
    virtual TextContent *textcontent( const std::string& = "current" ) const;
    virtual UnicodeString stricttext( const std::string& = "current" ) const;
    bool hastext( const std::string& = "current" ) const ;
    virtual FoliaElement *head() const {
      throw NotImplementedError("head() for " + _xmltag );
    }
    virtual FoliaElement *getNew() const {
      throw NotImplementedError("getNew()"); };
    virtual FoliaElement *getOriginal() const {
      throw NotImplementedError("getOriginal()"); };
    virtual FoliaElement *getCurrent() const {
      throw NotImplementedError("getCurrent()"); };
    virtual FoliaElement *split( FoliaElement *, FoliaElement *, 
				    const std::string& = "" ){
      throw NotImplementedError("split()"); };
    virtual Correction *mergewords( FoliaElement *, 
				    const std::vector<FoliaElement *>&,
				    const std::string& = "" ){
      throw NotImplementedError("mergewords()"); };
    virtual Correction *deleteword( FoliaElement *, 
				    const std::string& = "" ){
      throw NotImplementedError("deleteword()"); };
    virtual Correction *insertword( FoliaElement *, FoliaElement *,
				    const std::string& = "" ){
      throw NotImplementedError("insertword()"); };
    virtual std::vector<Suggestion*> suggestions() const
      { throw NotImplementedError("suggestions()"); };    
    virtual Suggestion *suggestions( size_t ) const
      { throw NotImplementedError("suggestions()"); };    
    virtual std::string subset() const 
      { throw NotImplementedError("subset()"); };
    virtual FoliaElement *previous() const {
      throw NotImplementedError("previous()"); };
    virtual FoliaElement *next() const {
      throw NotImplementedError("next()"); };
    virtual std::vector<Word*> context( size_t, 
					const std::string& ="" ) const {
      throw NotImplementedError("contect()"); };
    virtual std::vector<Word*> leftcontext( size_t, 
					    const std::string& ="" ) const {
      throw NotImplementedError("leftcontect()"); 
    };
    virtual std::vector<Word*> rightcontext( size_t, 
					     const std::string& ="" ) const {
      throw NotImplementedError("rightcontext()"); 
    };
    virtual int offset() const {
      throw NotImplementedError("offset()"); 
    };

    std::string pos() const;
    std::string lemma() const;
    std::string cls() const { return _class; };
    std::string sett() const { return _set; };
    std::string annotator() const { return _annotator; };
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

    template <typename F>
      Alternative *addAlternative( const KWargs& );
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
      throw NotImplementedError("alternatives()"); 
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
      throw NotImplementedError("correct()"); };

    virtual std::string src() const {
      throw NotImplementedError("src()"); };

    virtual UnicodeString caption() const {
      throw NotImplementedError("caption() for " + classname()); };

    void increfcount() { ++refcount; };
    FoliaElement *parent() const { return _parent; };
    void setParent( FoliaElement *p ) { _parent = p ; };
    void setAuth( bool b ){ _auth = b; };
  protected:
    virtual void init()=0;
    virtual KWargs collectAttributes() const;
    virtual std::string getTextDelimiter() const { return TEXTDELIMITER; }
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
    bool addable( const FoliaElement *, const std::string& = "" ) const;

    std::vector<FoliaElement*> data;
    FoliaElement *_parent;
    bool _auth;
    //  UnicodeString _text;
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
    std::tm *_datetime;
    AnnotatorType _annotator_type;
    double _confidence;
    int refcount;
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

  class AbstractStructureElement: public FoliaElement, AllowGenerateID {
  public:  
  AbstractStructureElement( Document *d=0 ): FoliaElement( d ) {};
    std::string str() const;
    bool allowannotations() const { return true; };
    std::vector<Alternative *> alternatives( ElementType = BASE,
					     const std::string& = "" ) const;
  
    FoliaElement *append( FoliaElement* );
    Correction *correct( std::vector<FoliaElement*>,
			 std::vector<FoliaElement*>,
			 std::vector<FoliaElement*>,
			 std::vector<FoliaElement*>,
			 const KWargs& );
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
  };

  class AbstractAnnotation: public FoliaElement {
  public:
  AbstractAnnotation( Document *d=0 ):  FoliaElement( d ){};
  };

  class AbstractTokenAnnotation: public AbstractAnnotation, AllowGenerateID {
  public:
  AbstractTokenAnnotation( Document *d=0 ):  AbstractAnnotation( d ){};
    std::string generateId( const std::string& tag ){
      return IGgen( tag, _id ); 
    }
  };

  class AbstractSubTokenAnnotation: public AbstractAnnotation {
  public:
  AbstractSubTokenAnnotation( Document *d=0 ):  AbstractAnnotation( d ){};
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
  AbstractSpanAnnotation( Document *d=0 ):  AbstractAnnotation( d ){};
    xmlNode *xml( bool ) const;
    FoliaElement *append( FoliaElement* );
    std::string generateId( const std::string& tag ){
      return IGgen( tag, _id ); 
    }
  };

  class TextContent: public FoliaElement {
  public:
  TextContent( const std::string& s="" ):  FoliaElement( ){ classInit( s ); }
  TextContent( const KWargs& a ):  FoliaElement( ){ classInit( a ); }
  TextContent( Document *d, const std::string& s="" ):  FoliaElement( d ){ classInit( s ); }
  TextContent( Document *d, const KWargs& a ):  FoliaElement( d ){ classInit( a ); }
    FoliaElement* parseXml( const xmlNode * );
    xmlNode *xml( bool ) const;
    void setAttributes( const KWargs& );
    KWargs collectAttributes() const;  
    std::string str() const;
    UnicodeString text( const std::string& = "current" ) const;
    int offset() const { return _offset; };
    FoliaElement *append( FoliaElement* ){ throw NotImplementedError("TextContent::append()"); };
    TextContent *postappend();
    std::vector<FoliaElement*> findreplacables( FoliaElement * ) const;
  private:
    void init();
    int _offset;
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
    xmlNode *xml( bool ) const;
    std::string content() const { return value; };
  private:
    void init();
    std::string value;
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
    Correction *incorrection() const;
    Word *previous() const;
    Word *next() const;
    std::vector<Word*> context( size_t, 
				const std::string& ="" ) const;
    std::vector<Word*> leftcontext( size_t, 
				    const std::string& = "" ) const;
    std::vector<Word*> rightcontext( size_t, 
				     const std::string& ="" ) const;
    FoliaElement *append( FoliaElement *);
    const Word* resolveword( const std::string& ) const;
    void setAttributes( const KWargs& );
    KWargs collectAttributes() const;  
    std::string getTextDelimiter() const { 
      if ( space )
	return TEXTDELIMITER;
      else {
	return "";
      }
    }
  private:
    void init();
    bool space;
  };

  class PlaceHolder: public Word {
  public:
  PlaceHolder( const std::string& s=""): Word(){ classInit( s ); };
  PlaceHolder( const KWargs& a ): Word(){ classInit( a ); };
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

  class Alternative: public AbstractStructureElement{
  public:
  Alternative( const std::string& s=""):  AbstractStructureElement(){ classInit( s ); };
  Alternative( const KWargs& a ):  AbstractStructureElement(){ classInit( a ); };
  Alternative( Document *d, const std::string& s=""):  AbstractStructureElement( d ){ classInit( s ); };
  Alternative( Document *d, const KWargs& a ):  AbstractStructureElement( d ){ classInit( a ); };
    bool allowannotations() const { return true; };
  private:
    void init();
  };

  template <typename F>
    Alternative *FoliaElement::addAlternative( const KWargs& args ){
    KWargs kw;
    std::string id = generateId( "alt" );
    kw["id"] = id;
    Alternative *res = 0;
    try {
      res = new Alternative( mydoc, kw );
      res->addAnnotation<F>( args );
    }
    catch( std::exception& ){
      delete res;
      throw;
    }
    append( res );
    return res;
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
  private:
    void init();
  };

  class BegindatetimeFeature: public Feature {
  public:
  BegindatetimeFeature( const std::string& s="" ): Feature( ){ classInit( s ); }
  BegindatetimeFeature( const KWargs& a ): Feature( ){ classInit( a ); }
  BegindatetimeFeature( Document *d, const std::string& s="" ): Feature( d ){ classInit( s ); }
  BegindatetimeFeature( Document *d, const KWargs& a ): Feature( d ){ classInit( a ); }
  private:
    void init();
  };

  class EnddatetimeFeature: public Feature {
  public:
  EnddatetimeFeature( const std::string& s="" ): Feature( ){ classInit( s ); }
  EnddatetimeFeature( const KWargs& a ): Feature( ){ classInit( a ); }
  EnddatetimeFeature( Document *d, const std::string& s="" ): Feature( d ){ classInit( s ); }
  EnddatetimeFeature( Document *d, const KWargs& a ): Feature( d ){ classInit( a ); }
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

  class DependencyHead: public AbstractSpanAnnotation {
  public:
  DependencyHead( const std::string& s="" ): AbstractSpanAnnotation( ){ classInit( s ); }
  DependencyHead( const KWargs& a ): AbstractSpanAnnotation( ){ classInit( a ); }
  DependencyHead( Document *d, const std::string& s="" ): AbstractSpanAnnotation( d ){ classInit( s ); }
  DependencyHead( Document *d, const KWargs& a ): AbstractSpanAnnotation( d ){ classInit( a ); }
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
    DependencyHead *head() const;
    DependencyDependent *dependent() const;
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
  private:
    void init();
  };

  class NewElement: public FoliaElement {
  public:
  NewElement( const std::string& s=""): FoliaElement( ) { classInit( s ); };
  NewElement( const KWargs& a ): FoliaElement( ) { classInit( a ); };
  NewElement( Document *d, const std::string& s=""): FoliaElement( d ) { classInit( s ); };
  NewElement( Document *d, const KWargs& a ): FoliaElement( d ) { classInit( a ); };
  private:
    void init();
  };

  class Current: public FoliaElement {
  public:
  Current( const std::string& s=""): FoliaElement( ) { classInit( s ); };
  Current( const KWargs& a ): FoliaElement( ) { classInit( a ); };
  Current( Document *d, const std::string& s=""): FoliaElement( d ) { classInit( s ); };
  Current( Document *d, const KWargs& a ): FoliaElement( d ) { classInit( a ); };
  private:
    void init();
  };

  class Original: public FoliaElement {
  public:
  Original( const std::string& s=""): FoliaElement( ) { classInit( s ); };
  Original( const KWargs& a ): FoliaElement( ) { classInit( a ); };
  Original( Document *d, const std::string& s=""): FoliaElement( d ) { classInit( s ); };
  Original( Document *d, const KWargs& a ): FoliaElement( d ) { classInit( a ); };
  private:
    void init();
  };

  class Suggestion: public FoliaElement {
  public:
  Suggestion( const std::string& s=""): FoliaElement( ) { classInit( s ); };
  Suggestion( const KWargs& a ): FoliaElement( ) { classInit( a ); };
  Suggestion( Document *d, const std::string& s=""): FoliaElement( d ) { classInit( s ); };
  Suggestion( Document *d, const KWargs& a ): FoliaElement( d ) { classInit( a ); };
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
    xmlNode *xml( bool ) const;
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
    UnicodeString text( const std::string& = "current" ) const;
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
    void setAttributes( const KWargs& );
    KWargs collectAttributes() const;  
  private:
    void init();
    bool error;
  };

  class AbstractSubtokenAnnotationLayer: public FoliaElement, AllowGenerateID {
  public:
  AbstractSubtokenAnnotationLayer( const std::string& s="" ): FoliaElement( ){ classInit( s ); }
  AbstractSubtokenAnnotationLayer( const KWargs& a ): FoliaElement( ){ classInit( a ); }
  AbstractSubtokenAnnotationLayer( Document *d, const std::string& s="" ): FoliaElement( d ){ classInit( s ); }
  AbstractSubtokenAnnotationLayer( Document *d, const KWargs& a ): FoliaElement( d ){ classInit( a ); }
    std::string generateId( const std::string& tag ){
      return IGgen( tag, _id ); 
    }

  private:
    void init();
  };

  class AbstractSubtokenAnnotation: public AbstractAnnotation, AllowGenerateID {
  public:
  AbstractSubtokenAnnotation( const std::string& s="" ): AbstractAnnotation( ){ classInit( s ); }
  AbstractSubtokenAnnotation( const KWargs& a ): AbstractAnnotation( ){ classInit( a ); }
  AbstractSubtokenAnnotation( Document *d, const std::string& s="" ): AbstractAnnotation( d ){ classInit( s ); }
  AbstractSubtokenAnnotation( Document *d, const KWargs& a ): AbstractAnnotation( d ){ classInit( a ); }
    std::string generateId( const std::string& tag ){
      return IGgen( tag, _id ); 
    }

  private:
    void init();
  };

  class TimedEvent: public AbstractSpanAnnotation {
  public:
  TimedEvent( const std::string& s="" ): AbstractSpanAnnotation(){ classInit( s ); }
  TimedEvent( const KWargs& a ): AbstractSpanAnnotation(){ classInit( a ); }
  TimedEvent( Document *d, const std::string& s="" ): AbstractSpanAnnotation( d ){ classInit( s ); }
  TimedEvent( Document *d, const KWargs& a ): AbstractSpanAnnotation( d ){ classInit( a ); }
  private:
    void init();
  };

  class Morpheme: public AbstractSubtokenAnnotation {
  public:
  Morpheme( const std::string& s="" ): AbstractSubtokenAnnotation(){ classInit( s ); }
  Morpheme( const KWargs& a ): AbstractSubtokenAnnotation(){ classInit( a ); }
  Morpheme( Document *d, const std::string& s="" ): AbstractSubtokenAnnotation( d ){ classInit( s ); }
  Morpheme( Document *d, const KWargs& a ): AbstractSubtokenAnnotation( d ){ classInit( a ); }
  private:
    void init();
  };

  class Subentity: public AbstractSubtokenAnnotation {
  public:
  Subentity( const std::string& s="" ): AbstractSubtokenAnnotation(){ classInit( s ); }
  Subentity( const KWargs& a ): AbstractSubtokenAnnotation(){ classInit( a ); }
  Subentity( Document *d, const std::string& s="" ): AbstractSubtokenAnnotation( d ){ classInit( s ); }
  Subentity( Document *d, const KWargs& a ): AbstractSubtokenAnnotation( d ){ classInit( a ); }
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

  class MorphologyLayer: public AbstractSubtokenAnnotationLayer {
  public:
  MorphologyLayer( const std::string& s="" ): AbstractSubtokenAnnotationLayer(){ classInit( s ); }
  MorphologyLayer( const KWargs& a ): AbstractSubtokenAnnotationLayer(){ classInit( a ); }
  MorphologyLayer( Document *d, const std::string& s="" ): AbstractSubtokenAnnotationLayer( d ){ classInit( s ); }
  MorphologyLayer( Document *d, const KWargs& a ): AbstractSubtokenAnnotationLayer( d ){ classInit( a ); }
  private:
    void init();
  };

  class SubentitiesLayer: public AbstractSubtokenAnnotationLayer {
  public:
  SubentitiesLayer( const std::string& s="" ): AbstractSubtokenAnnotationLayer(){ classInit( s ); }
  SubentitiesLayer( const KWargs& a ): AbstractSubtokenAnnotationLayer(){ classInit( a ); }
  SubentitiesLayer( Document *d, const std::string& s="" ): AbstractSubtokenAnnotationLayer( d ){ classInit( s ); }
  SubentitiesLayer( Document *d, const KWargs& a ): AbstractSubtokenAnnotationLayer( d ){ classInit( a ); }
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

  std::string VersionName();
  std::string Version();

} // namespace folia

#endif
