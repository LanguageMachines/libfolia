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
      setAttributes( s );
    }
    virtual void setAttributes( const std::string& s ){
      if ( !s.empty() )
	setAttributes(  getArgs( s ) );
    };

    virtual void setAttributes( const KWargs& );

    void setDateTime( const std::string& );
    std::string getDateTime() const;

    //functions regarding contained data
    virtual size_t size() const { return data.size(); };
  
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
      std::vector<F*> select( const std::string& set,
			      const std::set<ElementType>& exclude, 
			      bool recurse = true ) const {
      F obj("");
      std::vector<F*> res;
      std::vector<FoliaElement*> tmp = select( obj._element_id, 
					       set,
					       exclude,
					       recurse );
      for ( size_t i = 0; i < tmp.size(); ++i ){
	res.push_back( dynamic_cast<F*>( tmp[i]) );
      }
      return res;
    }
    
    template <typename F>
      std::vector<F*> select( const std::string& set,
			      bool recurse = true ) const {
      F obj("");
      std::vector<F*> res;
      std::vector<FoliaElement*> tmp = select( obj._element_id, 
					       set,
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
    virtual TextContent *textcontent( const std::string& = "current" ) const;
    virtual xmlNode *xml( bool ) const; //serialize to XML  
    virtual FoliaElement* parseXml( const xmlNode * );
    UnicodeString unicode() const { return text(); };
    virtual std::string str() const;
    bool hastext( const std::string& = "current" ) const ;
    virtual UnicodeString text( const std::string& = "current" ) const;
    virtual UnicodeString stricttext( const std::string& = "current" ) const;
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
    std::string cls() const { return _cls; };
    std::string st() const { return _set; };
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
    Alternative *addAlternative( ElementType, const KWargs& );
    AbstractTokenAnnotation *addAnnotation( ElementType, const KWargs& );
    AbstractTokenAnnotation *addPosAnnotation( const KWargs& );
    AbstractTokenAnnotation *addLemmaAnnotation( const KWargs& );
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
    std::string _cls;
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
    std::string IGgen( const std::string&, const std::string& );
  private:
    std::map<std::string, int> maxid;
  };

  class AbstractStructureElement: public FoliaElement, AllowGenerateID {
  public:  
  AbstractStructureElement( Document *d=0 ): FoliaElement( d ) {};
    std::string str() const;
    bool allowannotations() const { return true; };
    size_t hasannotation( ElementType, std::set<ElementType>& );
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
  Feature( Document *d, const std::string& s=""): FoliaElement( d ){ classInit( s ); }
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
  TextContent( Document *d=0, const std::string& s="" ):  FoliaElement( d ){ classInit( s ); }
    FoliaElement* parseXml( const xmlNode * );
    xmlNode *xml( bool ) const;
    void setAttributes( const KWargs& );
    KWargs collectAttributes() const;  
    std::string str() const;
    UnicodeString text( const std::string& = "current" ) const;
    int offset() const { return _offset; };
    FoliaElement *append( FoliaElement* ){ throw NotImplementedError("TextContent::append()"); };
    FoliaElement *postappend();
    std::vector<FoliaElement*> findreplacables( FoliaElement * ) const;
  private:
    void init();
    int _offset;
    UnicodeString _text;
  };

  class FoLiA: public FoliaElement {
  public:
  FoLiA( const std::string& s="" ): FoliaElement( ) { classInit( s ); };
  FoLiA( Document *d=0, const std::string& s=""): FoliaElement( d ) { classInit( s ); };
  private:
    void init();
  };

  class DCOI: public FoliaElement {
  public:
  DCOI( const std::string& s=""): FoliaElement( ) { classInit( s ); };
  DCOI( Document *d=0, const std::string& s=""): FoliaElement( d ) { classInit( s ); };
  private:
    void init();
  };

  class Head: public AbstractStructureElement {
  public:
  Head( const std::string& s=""): AbstractStructureElement() {  classInit( s ); };
  Head( Document *d=0, const std::string& s=""): AbstractStructureElement( d ) {  classInit( s ); };
  private:
    void init();
  };

  class Gap: public FoliaElement {
  public:
  Gap( const std::string& s=""): FoliaElement( ) { classInit( s ); };
  Gap( Document *d=0, const std::string& s=""): FoliaElement( d ) { classInit( s ); };
    std::string content() const;
  private:
    void init();
  };

  class Content: public FoliaElement {
  public:
  Content( const std::string& s=""): FoliaElement( ) { classInit( s ); };
  Content( Document *d=0, const std::string& s=""): FoliaElement( d ) { classInit( s ); };
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
  Division( Document *d=0, const std::string& s=""):  AbstractStructureElement( d ) { classInit( s ); };
    Head *head() const;
  private:
    void init();
  };

  class LineBreak: public AbstractStructureElement {
  public:
  LineBreak( const std::string& s=""):  AbstractStructureElement(){ classInit( s ); };
  LineBreak( Document *d=0,  const std::string& s=""):  AbstractStructureElement( d ){ classInit( s ); };
  private:
    void init();
  };

  class WhiteSpace: public AbstractStructureElement {
  public:
  WhiteSpace( const std::string& s=""):  AbstractStructureElement(){ classInit( s ); };
  WhiteSpace( Document *d=0,  const std::string& s=""):  AbstractStructureElement( d ){ classInit( s ); };
  private:
    void init();
  };

  class Word: public AbstractStructureElement {
  public:
  Word( const std::string& s=""):  AbstractStructureElement(){ classInit( s ); };
  Word( Document *d=0,  const std::string& s=""):  AbstractStructureElement( d ){ classInit( s ); };
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
  PlaceHolder( const std::string& s=""): Word(""){ classInit( s ); };
    void setAttributes( const std::string& );
  private:
    void init();
  };

  class Sentence: public AbstractStructureElement {
  public:
  Sentence( const std::string& s=""):  AbstractStructureElement(){ classInit( s ); }
  Sentence( Document *d=0, const std::string& s=""):  AbstractStructureElement( d ){ classInit( s ); }
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
  Text( Document *d=0, const std::string& s=""):  AbstractStructureElement( d ){ classInit( s ); };
  private:
    void init();
  };

  class Event: public AbstractStructureElement {
  public:
  Event( const std::string& s=""):  AbstractStructureElement(){ classInit( s ); };
  Event( Document *d=0, const std::string& s=""):  AbstractStructureElement( d ){ classInit( s ); };
  private:
    void init();
  };

  class Caption: public AbstractStructureElement {
  public:
  Caption( const std::string& s=""):  AbstractStructureElement(){ classInit( s ); };
  Caption( Document *d=0, const std::string& s=""):  AbstractStructureElement( d ){ classInit( s ); };
  private:
    void init();
  };

  class Label: public AbstractStructureElement {
  public:
  Label( const std::string& s=""):  AbstractStructureElement(){ classInit( s ); };
  Label( Document *d=0, const std::string& s=""):  AbstractStructureElement( d ){ classInit( s ); };
  private:
    void init();
  };

  class ListItem: public AbstractStructureElement {
  public:
  ListItem( const std::string& s=""):  AbstractStructureElement(){ classInit( s ); };
  ListItem( Document *d=0, const std::string& s=""):  AbstractStructureElement( d ){ classInit( s ); };
  private:
    void init();
  };

  class List: public AbstractStructureElement {
  public:
  List( const std::string& s=""):  AbstractStructureElement(){ classInit( s ); };
  List( Document *d=0, const std::string& s=""):  AbstractStructureElement( d ){ classInit( s ); };
  private:
    void init();
  };

  class Figure: public AbstractStructureElement {
  public:
  Figure( const std::string& s=""):  AbstractStructureElement(){ classInit( s ); };
  Figure( Document *d=0, const std::string& s=""):  AbstractStructureElement( d ){ classInit( s ); };
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
  Paragraph( Document *d=0, const std::string& s=""):  AbstractStructureElement( d ){ classInit( s ); };
  private:
    void init();
  };

  class Alternative: public AbstractStructureElement{
  public:
  Alternative( const std::string& s=""):  AbstractStructureElement(){ classInit( s ); };
  Alternative( Document *d=0, const std::string& s=""):  AbstractStructureElement( d ){ classInit( s ); };
    bool allowannotations() const { return true; };
  private:
    void init();
  };


  class PosAnnotation: public AbstractTokenAnnotation {
  public:
  PosAnnotation( const std::string& s=""): AbstractTokenAnnotation(){ classInit( s ); };
  PosAnnotation( Document *d=0, const std::string& s=""): AbstractTokenAnnotation( d ){ classInit( s ); };
  private:
    void init();
  };

  class LemmaAnnotation: public AbstractTokenAnnotation {
  public:
  LemmaAnnotation(  const std::string& s=""): AbstractTokenAnnotation(){ classInit( s ); };
  LemmaAnnotation( Document *d=0, const std::string& s=""): AbstractTokenAnnotation( d ){ classInit( s ); };
  private:
    void init();
  };

  class PhonAnnotation: public AbstractTokenAnnotation {
  public:
  PhonAnnotation(  const std::string& s=""): AbstractTokenAnnotation(){ classInit( s ); };
  PhonAnnotation( Document *d=0, const std::string& s=""): AbstractTokenAnnotation( d ){ classInit( s ); };
  private:
    void init();
  };

  class DomainAnnotation: public AbstractTokenAnnotation {
  public:
  DomainAnnotation(  const std::string& s=""): AbstractTokenAnnotation(){ classInit( s ); };
  DomainAnnotation( Document *d=0, const std::string& s=""): AbstractTokenAnnotation( d ){ classInit( s ); };
  private:
    void init();
  };

  class SenseAnnotation: public AbstractTokenAnnotation {
  public:
  SenseAnnotation( const std::string& s="" ): AbstractTokenAnnotation(){ classInit( s ); };
  SenseAnnotation( Document *d=0, const std::string& s="" ): AbstractTokenAnnotation( d ){ classInit( s ); };
  private:
    void init();
  };

  class SubjectivityAnnotation: public AbstractTokenAnnotation {
  public:
  SubjectivityAnnotation( const std::string& s="" ): AbstractTokenAnnotation(){ classInit( s ); };
  SubjectivityAnnotation( Document *d=0, const std::string& s="" ): AbstractTokenAnnotation( d ){ classInit( s ); };
  private:
    void init();
  };

  class Quote: public AbstractStructureElement {
  public:
  Quote( const std::string& s="" ): AbstractStructureElement(){ classInit( s ); };
  Quote( Document *d=0, const std::string& s="" ): AbstractStructureElement( d ){ classInit( s ); };
    FoliaElement *append( FoliaElement *);
  private:
    void init();
  };

  class BegindatetimeFeature: public Feature {
  public:
  BegindatetimeFeature( const std::string& s ): Feature( ){ classInit( s ); }
  BegindatetimeFeature( Document *d=0, const std::string& s="" ): Feature( d ){ classInit( s ); }
  private:
    void init();
  };

  class EnddatetimeFeature: public Feature {
  public:
  EnddatetimeFeature( const std::string& s ): Feature( ){ classInit( s ); }
  EnddatetimeFeature( Document *d=0, const std::string& s="" ): Feature( d ){ classInit( s ); }
  private:
    void init();
  };

  class SynsetFeature: public Feature {
  public:
  SynsetFeature( const std::string& s ): Feature( ){ classInit( s ); }
  SynsetFeature( Document *d=0, const std::string& s="" ): Feature( d ){ classInit( s ); }
  private:
    void init();
  };

  class ActorFeature: public Feature {
  public:
  ActorFeature( const std::string& s ): Feature( ){ classInit( s ); }
  ActorFeature( Document *d=0, const std::string& s="" ): Feature( d ){ classInit( s ); }
  private:
    void init();
  };

  class HeadFeature: public Feature {
  public:
  HeadFeature( const std::string& s ): Feature( ){ classInit( s ); }
  HeadFeature( Document *d=0, const std::string& s="" ): Feature( d ){ classInit( s ); }
  private:
    void init();
  };

  class WordReference: public FoliaElement {
  public:
  WordReference( const std::string& s="" ): FoliaElement( ){ classInit( s ); };
  WordReference( Document *d=0, const std::string& s="" ): FoliaElement( d ){ classInit( s ); };
  private:
    void init();
    FoliaElement* parseXml( const xmlNode *node );
  };  

  class SyntacticUnit: public AbstractSpanAnnotation {
  public:
  SyntacticUnit( const std::string& s ): AbstractSpanAnnotation( ){ classInit( s ); }
  SyntacticUnit( Document *d=0, const std::string& s="" ): AbstractSpanAnnotation( d ){ classInit( s ); }
  private:
    void init();
  };

  class Chunk: public AbstractSpanAnnotation {
  public:
  Chunk( const std::string& s ): AbstractSpanAnnotation( ){ classInit( s ); }
  Chunk( Document *d=0, const std::string& s="" ): AbstractSpanAnnotation( d ){ classInit( s ); }
  private:
    void init();
  };

  class Entity: public AbstractSpanAnnotation {
  public:
  Entity( const std::string& s ): AbstractSpanAnnotation( ){ classInit( s ); }
  Entity( Document *d=0, const std::string& s="" ): AbstractSpanAnnotation( d ){ classInit( s ); }
  private:
    void init();
  };

  class DependencyHead: public AbstractSpanAnnotation {
  public:
  DependencyHead( const std::string& s ): AbstractSpanAnnotation( ){ classInit( s ); }
  DependencyHead( Document *d=0, const std::string& s="" ): AbstractSpanAnnotation( d ){ classInit( s ); }
  private:
    void init();
  };

  class DependencyDependent: public AbstractSpanAnnotation {
  public:
  DependencyDependent( const std::string& s ): AbstractSpanAnnotation( ){ classInit( s ); }
  DependencyDependent( Document *d=0, const std::string& s="" ): AbstractSpanAnnotation( d ){ classInit( s ); }
  private:
    void init();
  };

  class Dependency: public AbstractSpanAnnotation {
  public:
  Dependency( const std::string& s ): AbstractSpanAnnotation( ){ classInit( s ); }

  Dependency( Document *d=0, const std::string& s="" ): AbstractSpanAnnotation( d ){ classInit( s ); }
    DependencyHead *head() const;
    DependencyDependent *dependent() const;
  private:
    void init();
  };

  class AbstractAnnotationLayer: public FoliaElement, AllowGenerateID {
  public:
  AbstractAnnotationLayer( const std::string& s=""): FoliaElement( ) { classInit( s ); };
  AbstractAnnotationLayer( Document *d=0, const std::string& s=""): FoliaElement( d ) { classInit( s ); };
    std::string generateId( const std::string& tag ){
      return IGgen( tag, _id ); 
    }
  private:
    void init();
  };

  class NewElement: public FoliaElement {
  public:
  NewElement( const std::string& s=""): FoliaElement( ) { classInit( s ); };
  NewElement( Document *d=0, const std::string& s=""): FoliaElement( d ) { classInit( s ); };
  private:
    void init();
  };

  class Current: public FoliaElement {
  public:
  Current( const std::string& s=""): FoliaElement( ) { classInit( s ); };
  Current( Document *d=0, const std::string& s=""): FoliaElement( d ) { classInit( s ); };
  private:
    void init();
  };

  class Original: public FoliaElement {
  public:
  Original( const std::string& s=""): FoliaElement( ) { classInit( s ); };
  Original( Document *d=0, const std::string& s=""): FoliaElement( d ) { classInit( s ); };
  private:
    void init();
  };

  class Suggestion: public FoliaElement {
  public:
  Suggestion( const std::string& s=""): FoliaElement( ) { classInit( s ); };
  Suggestion( Document *d=0, const std::string& s=""): FoliaElement( d ) { classInit( s ); };
  private:
    void init();
  };

  class Description: public FoliaElement {
  public:
  Description( const std::string& s=""): FoliaElement( ) { classInit( s ); };
  Description( Document *d=0, const std::string& s="" ): FoliaElement( d ) { classInit( s ); };
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
  Correction( Document *d=0, const std::string& s=""): AbstractTokenAnnotation( d ){ classInit( s ); }
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
  ErrorDetection( Document *d=0, const std::string& s=""): AbstractTokenAnnotation( d ){ classInit( s ); }
    void setAttributes( const KWargs& );
    KWargs collectAttributes() const;  
  private:
    void init();
    bool error;
  };

  class AbstractSubtokenAnnotationLayer: public FoliaElement, AllowGenerateID {
  public:
  AbstractSubtokenAnnotationLayer( const std::string& s="" ): FoliaElement( ){ classInit( s ); }
  AbstractSubtokenAnnotationLayer( Document *d=0, const std::string& s="" ): FoliaElement( d ){ classInit( s ); }
    std::string generateId( const std::string& tag ){
      return IGgen( tag, _id ); 
    }

  private:
    void init();
  };

  class AbstractSubtokenAnnotation: public AbstractAnnotation, AllowGenerateID {
  public:
  AbstractSubtokenAnnotation( const std::string& s="" ): AbstractAnnotation( ){ classInit( s ); }
  AbstractSubtokenAnnotation( Document *d=0, const std::string& s="" ): AbstractAnnotation( d ){ classInit( s ); }
    std::string generateId( const std::string& tag ){
      return IGgen( tag, _id ); 
    }

  private:
    void init();
  };

  class TimedEvent: public AbstractSpanAnnotation {
  public:
  TimedEvent( const std::string& s="" ): AbstractSpanAnnotation(){ classInit( s ); }
  TimedEvent( Document *d=0, const std::string& s="" ): AbstractSpanAnnotation( d ){ classInit( s ); }
  private:
    void init();
  };

  class Morpheme: public AbstractSubtokenAnnotation {
  public:
  Morpheme( const std::string& s="" ): AbstractSubtokenAnnotation( s ){ classInit( s ); }
  Morpheme( Document *d=0, const std::string& s="" ): AbstractSubtokenAnnotation( d ){ classInit( s ); }
  private:
    void init();
  };

  class Subentity: public AbstractSubtokenAnnotation {
  public:
  Subentity( const std::string& s="" ): AbstractSubtokenAnnotation( s ){ classInit( s ); }
  Subentity( Document *d=0, const std::string& s="" ): AbstractSubtokenAnnotation( d ){ classInit( s ); }
  private:
    void init();
  };

  class SyntaxLayer: public AbstractAnnotationLayer {
  public:
  SyntaxLayer( const std::string& s=""): AbstractAnnotationLayer( s ){ classInit( s ); }
  SyntaxLayer( Document *d=0, const std::string& s=""): AbstractAnnotationLayer( d ){ classInit( s ); }
  private:
    void init();
  };

  class ChunkingLayer: public AbstractAnnotationLayer {
  public:
  ChunkingLayer( const std::string& s=""): AbstractAnnotationLayer( s ){ classInit( s ); }
  ChunkingLayer( Document *d=0, const std::string& s=""): AbstractAnnotationLayer( d ){ classInit( s ); }
  private:
    void init();
  };

  class EntitiesLayer: public AbstractAnnotationLayer {
  public:
  EntitiesLayer( const std::string& s=""): AbstractAnnotationLayer( s ){ classInit( s ); }
  EntitiesLayer( Document *d=0, const std::string& s=""): AbstractAnnotationLayer( d ){ classInit( s ); }
  private:
    void init();
  };

  class TimingLayer: public AbstractAnnotationLayer {
  public:
  TimingLayer( const std::string& s=""): AbstractAnnotationLayer( s ){ classInit( s ); }
  TimingLayer( Document *d=0, const std::string& s=""): AbstractAnnotationLayer( d ){ classInit( s ); }
  private:
    void init();
  };

  class MorphologyLayer: public AbstractSubtokenAnnotationLayer {
  public:
  MorphologyLayer( const std::string& s="" ): AbstractSubtokenAnnotationLayer( s ){ classInit( s ); }
  MorphologyLayer( Document *d=0, const std::string& s="" ): AbstractSubtokenAnnotationLayer( d ){ classInit( s ); }
  private:
    void init();
  };

  class SubentitiesLayer: public AbstractSubtokenAnnotationLayer {
  public:
  SubentitiesLayer( const std::string& s="" ): AbstractSubtokenAnnotationLayer( s ){ classInit( s ); }
  SubentitiesLayer( Document *d=0, const std::string& s="" ): AbstractSubtokenAnnotationLayer( d ){ classInit( s ); }
  private:
    void init();
  };

  class DependenciesLayer: public AbstractAnnotationLayer {
  public:
  DependenciesLayer( const std::string& s=""): AbstractAnnotationLayer( s ){ classInit( s ); }
  DependenciesLayer( Document *d=0, const std::string& s=""): AbstractAnnotationLayer( d ){ classInit( s ); }
  private:
    void init();
  };

} // namespace folia

#endif
