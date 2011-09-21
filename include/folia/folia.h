#ifndef FOLIA_H
#define FOLIA_H

#include <list>
#include "unicode/unistr.h"
#include "unicode/regex.h"
#include "libxml/tree.h"
#include "libxml/xpath.h"
#include <ctime>
#include "foliautils.h"

class Document;
class AbstractElement;
class AbstractTokenAnnotation;
class Sentence;
class Word;
class TextContent;
class Alternative;
class Correction;
class Division;
class Paragraph;

class AbstractElement {
  friend std::ostream& operator<<( std::ostream&, const AbstractElement& );
  friend std::ostream& operator<<( std::ostream&, const AbstractElement* );
  friend bool operator==( const AbstractElement&, const AbstractElement& );
  friend class Document;
  friend class Correction;
 public:
  //Constructor  
  AbstractElement( Document* =0 );
  virtual ~AbstractElement();

  static AbstractElement *createElement( Document *, const std::string&  );

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
  
  virtual AbstractElement *append( AbstractElement* );
  virtual AbstractElement *postappend( ) { return this; };
  virtual std::vector<AbstractElement*> findreplacables( AbstractElement * ) const;
  void remove( AbstractElement *, bool = true );
  void replace( AbstractElement * );

  AbstractElement* index( size_t ) const;
  AbstractElement* rindex( size_t ) const;

  virtual const AbstractElement* resolveword( const std::string& ) const { return 0; };

  bool isinstance( ElementType et ) const {
    return et == _element_id;
  }
  bool acceptable( ElementType ) const;
  bool addable( const AbstractElement *, const std::string& = "" ) const;
  
  std::vector<AbstractElement*> select( ElementType elementtype,
					bool = true ) const;
  std::vector<AbstractElement*> select( ElementType elementtype,
					const std::set<ElementType>& ,
					bool = true ) const;
  std::vector<AbstractElement*> select( ElementType elementtype,
					const std::string&,
					bool = true ) const;
  std::vector<AbstractElement*> select( ElementType elementtype,
					const std::string&,
					const std::set<ElementType>& ,
					bool = true ) const;
  //XML (de)serialisation
  std::string xmlstring() const; // serialize to a string (XML fragment)
  virtual xmlNode *xml( bool ) const; //serialize to XML  
  virtual AbstractElement* parseXml( const xmlNode * );
  UnicodeString unicode() const { return text(); };
  virtual std::string str() const;
  bool hastext( const std::string& = "current" ) const ;
  virtual UnicodeString text( const std::string& = "current" ) const;
  virtual AbstractElement *head() const {
    throw NotImplementedError("head()"); }; 
  virtual std::string feat( const std::string& ) const {
    throw NotImplementedError("feat()"); }; 
  virtual AbstractElement *getNew( int = -1 ) const {
    throw NotImplementedError("getNew()"); };
  virtual AbstractElement *getOriginal( int = -1) const {
    throw NotImplementedError("getOriginal()"); };
  virtual AbstractElement *getCurrent() const {
    throw NotImplementedError("getCurrent()"); };
  virtual AbstractElement *getSuggestion( int = -1 ) const
  { throw NotImplementedError("getSuggestion() for " + _xmltag ); };
  virtual AbstractElement *split( AbstractElement *, AbstractElement *, 
				  const std::string& = "" ){
    throw NotImplementedError("split()"); };
  virtual Correction *mergewords( AbstractElement *, 
				  const std::vector<AbstractElement *>&,
				  const std::string& = "" ){
    throw NotImplementedError("mergewords()"); };
  virtual Correction *deleteword( AbstractElement *, 
				  const std::string& = "" ){
    throw NotImplementedError("deleteword()"); };
  virtual Correction *insertword( AbstractElement *, AbstractElement *,
				  const std::string& = "" ){
    throw NotImplementedError("insertword()"); };
  virtual std::vector<AbstractElement *> suggestions() const
    { throw NotImplementedError("suggestions()"); };    
  virtual std::string subset() const 
    { throw NotImplementedError("subset()"); };
  virtual AbstractElement *previous() const {
    throw NotImplementedError("previous()"); };
  virtual AbstractElement *next() const {
    throw NotImplementedError("next()"); };
  virtual std::vector<AbstractElement *> context( size_t, 
						  const std::string& ="" ) const {
    throw NotImplementedError("contect()"); };
  virtual std::vector<AbstractElement *> leftcontext( size_t, 
						      const std::string& ="" ) const {
    throw NotImplementedError("leftcontect()"); 
  };
  virtual std::vector<AbstractElement *> rightcontext( size_t, 
						       const std::string& ="" ) const {
    throw NotImplementedError("rightcontext()"); 
  };

  std::vector<AbstractElement *>annotations( ElementType ) const;
  virtual AbstractElement *annotation( ElementType ) const {
    throw NotImplementedError( "annotation() not allowed on " + classname() );
  }
  virtual AbstractElement *annotation( ElementType, const std::string& ) const {
    throw NotImplementedError( "annotation() not allowed on " + classname() );
  }
  
  std::string pos() const { return annotation( Pos_t )->cls(); };
  std::string lemma() const { return annotation( Lemma_t )->cls(); };

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
  virtual std::vector<AbstractElement*> paragraphs() const {
    throw NotImplementedError("paragraphs() for " + _xmltag );
  };
  virtual std::vector<AbstractElement*> sentences() const {
    throw NotImplementedError("sentences() for " + _xmltag );
  };
  virtual std::vector<AbstractElement*> words() const {
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
  std::vector<AbstractElement *> alternatives( const std::string& s = "" ) const { 
    return alternatives( BASE, s );
  }
  virtual std::vector<AbstractElement *> alternatives( ElementType,
						       const std::string& = ""
						       ) const { 
    throw NotImplementedError("alternatives()"); 
  }
  virtual std::string content() const {
    throw NoSuchAnnotation( "content" );
  }
  virtual Correction *correct( const std::vector<AbstractElement*>,
			       const std::vector<AbstractElement*>,
			       const std::vector<AbstractElement*>,
			       const std::vector<AbstractElement*>,
			       const KWargs& ){
    throw NotImplementedError("correct() for " + _xmltag );
  }
  virtual Correction* correct( AbstractElement*,
			       AbstractElement*,
			       const KWargs& ){
    throw NotImplementedError("correct() for " + _xmltag );
  }
  virtual Correction *correct( const std::string& = "" ){
    throw NotImplementedError("correct()"); };

  void increfcount() { ++refcount; };
  AbstractElement *parent() const { return _parent; };
  void setParent( AbstractElement *p ) { _parent = p ; };
 protected:
  virtual void init()=0;
  virtual KWargs collectAttributes() const;
  virtual std::string getTextDelimiter() const { return TEXTDELIMITER; }
  virtual std::string generateId( const std::string&, const std::string& = "" ){
    throw NotImplementedError( "generateId() not allowed for " + classname() );
  };

  std::vector<AbstractElement*> data;
  AbstractElement *_parent;
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

bool operator==( const AbstractElement&, const AbstractElement& );
inline bool operator!=( const AbstractElement& e1, const AbstractElement& e2 ){
  return !( e1 == e2 );
}

inline size_t len( const AbstractElement *e ) { 
  return e->size(); }
inline size_t len( const std::vector<AbstractElement*>& v ) {
  return v.size(); }
inline std::string str( const AbstractElement *e ) { 
  return e->str(); }
inline UnicodeString text( const AbstractElement *e ) { 
  if ( e )
    return e->text();
  else
    throw ValueError( "text() for empty element" );
}
inline UnicodeString unicode( const AbstractElement *e ) {
  return e->unicode(); }
inline bool isinstance( const AbstractElement *e, ElementType t ) { 
  return e->isinstance( t ); }

class AbstractStructureElement: public AbstractElement {
 public:  
 AbstractStructureElement( Document *d=0 ): AbstractElement( d ) {};
  std::string str() const;
  size_t hasannotation( ElementType, std::set<ElementType>& );
  AbstractElement *annotation( ElementType ) const;
  AbstractElement *annotation( ElementType, const std::string& ) const ;
  std::vector<AbstractElement *> alternatives( ElementType = BASE,
					       const std::string& = "" ) const;
  
  AbstractElement *append( AbstractElement* );
  void setMaxId( AbstractElement * );
  int getMaxId( const std::string& );
  std::string generateId( const std::string&, const std::string& = "" );
  Correction *correct( std::vector<AbstractElement*>,
		       std::vector<AbstractElement*>,
		       std::vector<AbstractElement*>,
		       std::vector<AbstractElement*>,
		       const KWargs& );
  std::vector<AbstractElement*> paragraphs() const;
  std::vector<AbstractElement*> sentences() const;
  std::vector<AbstractElement*> words() const;
  Sentence *sentences( size_t ) const;
  Sentence *rsentences( size_t ) const;
  Paragraph *paragraphs( size_t ) const;
  Paragraph *rparagraphs( size_t ) const;
  Word *words( size_t ) const;
  Word *rwords( size_t ) const;
  const AbstractElement* resolveword( const std::string& ) const;
 private:
  std::map<std::string, int> maxid;
};

class AbstractAnnotation: public AbstractElement {
 public:
 AbstractAnnotation( Document *d=0 ):  AbstractElement( d ){};
  std::string feat( const std::string& ) const;
};

class AbstractTokenAnnotation: public AbstractAnnotation {
 public:
 AbstractTokenAnnotation( Document *d=0 ):  AbstractAnnotation( d ){};
};

class AbstractSubTokenAnnotation: public AbstractAnnotation {
 public:
 AbstractSubTokenAnnotation( Document *d=0 ):  AbstractAnnotation( d ){};
};

class Feature: public AbstractElement {
 public:
 Feature( const std::string& s=""): AbstractElement( ){ classInit( s ); }
 Feature( Document *d, const std::string& s=""): AbstractElement( d ){ classInit( s ); }
  void setAttributes( const KWargs& );
  KWargs collectAttributes() const;  
  std::string subset() const { return _subset; };

 protected:
  std::string _subset;
  
 private:
  void init();
};

class AbstractSpanAnnotation: public AbstractAnnotation {
 public:
 AbstractSpanAnnotation( Document *d=0 ):  AbstractAnnotation( d ){};
  xmlNode *xml( bool ) const;
  AbstractElement *append( AbstractElement* );
};

class TextContent: public AbstractElement {
 public:
 TextContent( const std::string& s="" ):  AbstractElement( ){ classInit( s ); }
 TextContent( Document *d=0, const std::string& s="" ):  AbstractElement( d ){ classInit( s ); }
  AbstractElement* parseXml( const xmlNode * );
  xmlNode *xml( bool ) const;
  void setAttributes( const KWargs& );
  KWargs collectAttributes() const;  
  std::string str() const;
  UnicodeString text( const std::string& = "current" ) const;
  AbstractElement *append( AbstractElement* ){ throw NotImplementedError("TextContent::append()"); };
  AbstractElement *postappend();
  std::vector<AbstractElement*> findreplacables( AbstractElement * ) const;
 private:
  void init();
  int _offset;
  UnicodeString _text;
};

class FoLiA: public AbstractElement {
 public:
 FoLiA( const std::string& s="" ): AbstractElement( ) { classInit( s ); };
 FoLiA( Document *d=0, const std::string& s=""): AbstractElement( d ) { classInit( s ); };
 private:
  void init();
};

class DCOI: public AbstractElement {
 public:
 DCOI( const std::string& s=""): AbstractElement( ) { classInit( s ); };
 DCOI( Document *d=0, const std::string& s=""): AbstractElement( d ) { classInit( s ); };
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

class Gap: public AbstractElement {
 public:
 Gap( const std::string& s=""): AbstractElement( ) { classInit( s ); };
 Gap( Document *d=0, const std::string& s=""): AbstractElement( d ) { classInit( s ); };
  std::string content() const;
 private:
  void init();
};

class Content: public AbstractElement {
 public:
 Content( const std::string& s=""): AbstractElement( ) { classInit( s ); };
 Content( Document *d=0, const std::string& s=""): AbstractElement( d ) { classInit( s ); };
  AbstractElement* parseXml( const xmlNode * );
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
  AbstractElement *head() const;
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
  Correction *correct( AbstractElement*,
		       AbstractElement*,
		       const KWargs& );
  AbstractElement *split( AbstractElement *, AbstractElement *,
			  const std::string& = "" );
  Sentence *sentence() const;
  Paragraph *paragraph() const;
  Division *division() const;
  Correction *incorrection() const;
  AbstractElement *previous() const;
  AbstractElement *next() const;
  std::vector<AbstractElement *> context( size_t, 
					  const std::string& ="" ) const;
  std::vector<AbstractElement *> leftcontext( size_t, 
					  const std::string& = "" ) const;
  std::vector<AbstractElement *> rightcontext( size_t, 
					  const std::string& ="" ) const;
  AbstractElement *append( AbstractElement *);
  const AbstractElement* resolveword( const std::string& ) const;
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
  Correction *splitWord( AbstractElement *, AbstractElement *, 
			 AbstractElement *, const KWargs& );
  Correction *mergewords( AbstractElement *, 
			  const std::vector<AbstractElement *>&,
			  const std::string& = "" );
  Correction *deleteword( AbstractElement *, const std::string& args );
  Correction *insertword( AbstractElement *, AbstractElement *,
			  const std::string& args );
 private:
  Correction *correctWords( const std::vector<AbstractElement *>&,
			    const std::vector<AbstractElement *>&,
			    const std::vector<AbstractElement *>&, 
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
 private:
  void init();
  std::string _url;
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

class WordReference: public AbstractElement {
 public:
 WordReference( const std::string& s="" ): AbstractElement( ){ classInit( s ); };
 WordReference( Document *d=0, const std::string& s="" ): AbstractElement( d ){ classInit( s ); };
 private:
  void init();
  AbstractElement* parseXml( const xmlNode *node );
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

class AbstractAnnotationLayer: public AbstractElement {
 public:
 AbstractAnnotationLayer( const std::string& s=""): AbstractElement( ) { classInit( s ); };
 AbstractAnnotationLayer( Document *d=0, const std::string& s=""): AbstractElement( d ) { classInit( s ); };
 private:
  void init();
};

class NewElement: public AbstractElement {
 public:
 NewElement( const std::string& s=""): AbstractElement( ) { classInit( s ); };
 NewElement( Document *d=0, const std::string& s=""): AbstractElement( d ) { classInit( s ); };
 private:
  void init();
};

class Current: public AbstractElement {
 public:
 Current( const std::string& s=""): AbstractElement( ) { classInit( s ); };
 Current( Document *d=0, const std::string& s=""): AbstractElement( d ) { classInit( s ); };
 private:
  void init();
};

class Original: public AbstractElement {
 public:
 Original( const std::string& s=""): AbstractElement( ) { classInit( s ); };
 Original( Document *d=0, const std::string& s=""): AbstractElement( d ) { classInit( s ); };
 private:
  void init();
};

class Suggestion: public AbstractElement {
 public:
 Suggestion( const std::string& s=""): AbstractElement( ) { classInit( s ); };
 Suggestion( Document *d=0, const std::string& s=""): AbstractElement( d ) { classInit( s ); };
 private:
  void init();
};

class Description: public AbstractElement {
 public:
 Description( const std::string& s=""): AbstractElement( ) { classInit( s ); };
 Description( Document *d=0, const std::string& s="" ): AbstractElement( d ) { classInit( s ); };
  std::string description() const { return _value; };
  void setAttributes( const KWargs& kwargs );
  AbstractElement* parseXml( const xmlNode * );
  xmlNode *xml( bool ) const;
 private:
  void init();
  std::string _value;
};

class Correction: public AbstractElement {
 public:
 Correction( const std::string& s=""): AbstractElement( ){ classInit( s ); }
 Correction( Document *d=0, const std::string& s=""): AbstractElement( d ){ classInit( s ); }
  bool hasNew() const;
  bool hasOriginal() const;
  bool hasCurrent() const;
  bool hasSuggestions() const;
  AbstractElement *getNew( int = -1 ) const;
  AbstractElement *getOriginal( int = -1 ) const;
  AbstractElement *getCurrent( int = -1 ) const;
  AbstractElement *getSuggestion( int = -1 ) const;
  std::vector<AbstractElement *> suggestions() const;
  UnicodeString text( const std::string& = "current" ) const;
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

class AbstractSubtokenAnnotationLayer: public AbstractElement {
 public:
 AbstractSubtokenAnnotationLayer( const std::string& s="" ): AbstractElement( ){ classInit( s ); }
 AbstractSubtokenAnnotationLayer( Document *d=0, const std::string& s="" ): AbstractElement( d ){ classInit( s ); }
 private:
  void init();
};

class AbstractSubtokenAnnotation: public AbstractAnnotation {
 public:
 AbstractSubtokenAnnotation( const std::string& s="" ): AbstractAnnotation( ){ classInit( s ); }
 AbstractSubtokenAnnotation( Document *d=0, const std::string& s="" ): AbstractAnnotation( d ){ classInit( s ); }
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


#endif
