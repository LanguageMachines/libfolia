#ifndef FOLIA_H
#define FOLIA_H

#include "unicode/unistr.h"
#include "unicode/regex.h"
#include "libxml/tree.h"
#include "libxml/xpath.h"
#include "boost/date_time/posix_time/posix_time.hpp"
#include "foliautils.h"

extern const std::string NSFOLIA;
extern const std::string NSDCOI;

class AbstractElement;
class AbstractTokenAnnotation;
class Sentence;
class Word;
class TextContent;
class Alternative;
class Correction;
class Pattern;

class Document {
  friend bool operator==( const Document&, const Document& );
 public:
  Document();
  Document( const std::string& );
  ~Document();
  void init();
  bool readFromFile( const std::string& );
  bool readFromString( const std::string& );
  bool save( const std::string& );
  int size() const;
  AbstractElement* doc() const { return foliadoc; }
  AbstractElement* addNode( ElementType, const KWargs& );
  std::vector<AbstractElement*> paragraphs() const;
  std::vector<AbstractElement*> sentences() const;
  std::vector<AbstractElement*> words() const;
  std::vector<std::vector<AbstractElement*> > findwords( const Pattern&,
							 const std::string& ="" ) const;
  std::vector<std::vector<AbstractElement*> > findwords( std::list<Pattern>&,
							 const std::string& = "" ) const;
  AbstractElement* words( size_t ) const;
  AbstractElement* rwords( size_t ) const;
  AbstractElement* rparagraphs( size_t ) const;
  AbstractElement* sentences( size_t ) const;
  std::string toXml();

  AbstractElement *append( AbstractElement* );
        
  void addDocIndex( AbstractElement* el, const std::string& );
  
  AbstractElement* operator []( size_t ) const; //select i'th element from data
  
  AbstractElement *index( const std::string& ) const; //retrieve element with specified ID 
  AbstractElement* operator []( const std::string& ) const ; //index as operator 
  bool isDeclared( const std::string&, AnnotationType::AnnotationType );
  std::string defaultset( AnnotationType::AnnotationType,
			  bool = false ) const;
  std::string defaultannotator( AnnotationType::AnnotationType, 
				const std::string& ="", bool = false ) const;
  std::string defaultannotatortype( AnnotationType::AnnotationType, 
				    const std::string& ="", bool = false ) const;

  AbstractElement* parseXml( );
  AbstractElement* parseFoliaDoc( xmlNode * );

  std::string id() const { return _id; };
  xmlNs *foliaNs() { return _foliaNs; };
  void setimdi( xmlNode * );
  void declare( AnnotationType::AnnotationType, 
		const std::string&,
		const std::string& = "" );
  void parseannotations( xmlNode * );
  void setannotations( xmlNode *);
  void setmetadata( xmlNode * );
  xmlDoc *XmlDoc() const { return xmldoc; };
  void keepForDeletion( AbstractElement *p ) { delSet.insert( p ); };
  int debug;

  class at_t {
    friend std::ostream& operator<<( std::ostream&, const at_t& );
  public:
  at_t( const std::string& _a, const std::string& _t ): a(_a),t(_t){}; 
    std::string a;
    std::string t;
  };
  class ts_t {
    friend std::ostream& operator<<( std::ostream&, const ts_t& );
  public:
  ts_t( AnnotationType::AnnotationType _t, const std::string& _s): t(_t),s(_s){};
    AnnotationType::AnnotationType t;
    std::string s;
  };
  std::map<AnnotationType::AnnotationType,std::map<std::string,at_t> > annotationdefaults;

 private:
  void setAttributes( const KWargs&  );
  std::map<std::string, AbstractElement* > sindex;
  std::vector<AbstractElement* > iindex;
  std::vector<AbstractElement*> data;
  std::string _id;
  std::list<ts_t> annotations;
  std::set<AbstractElement *> delSet;
  AbstractElement *foliadoc;
  xmlDoc *xmldoc;
  xmlNs *_foliaNs;
  MetaDataType metadatatype;
  xmlNode *metadata;
  std::string _title;
  std::string _date;
  std::string _language;
  std::string _publisher;
  std::string _license;
  bool loadall;
  std::string filename;
};

bool operator==( const Document&, const Document& );
inline bool operator!=( const Document& d1, const Document& d2 ){ 
  return !( d1==d2 );
};

inline std::ostream& operator<<( std::ostream& os, const Document::at_t& at ){
  os << "<" << at.a << "," << at.t << ">";
  return os;
}

inline std::ostream& operator<<( std::ostream& os, const Document::ts_t& ts ){
  os << "<" << ts.t << "," << ts.s << ">";
  return os;
}

class Pattern {
  friend std::ostream& operator<<( std::ostream&, const Pattern& );
 public:
  Pattern( const std::vector<std::string>&, 
	   const ElementType = BASE,
	   const std::string& = "" );
  ~Pattern();
  bool match( const UnicodeString& , size_t&, int&, bool&, bool& ) const;
  size_t size() const { return sequence.size(); };
  void unsetwild();
  bool variablesize() const;
  std::set<int> variablewildcards() const;
  ElementType matchannotation;
  bool regexp;
 private:
  bool case_sensitive;
  int maxgapsize;
  std::vector<UnicodeString> sequence;
  std::vector<RegexMatcher*> matchers;
  std::string matchannotationset;
};

inline std::ostream& operator<<( std::ostream& os, const Pattern& p ){
  os << "pattern: " << p.sequence;
  return os;
}

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
  std::vector<AbstractElement *>annotations( ElementType );

  virtual void setAttributes( const KWargs& );

  void setDateTime( const std::string& );

  //functions regarding contained data
  virtual size_t size() const { return data.size(); };
  
  virtual AbstractElement *append( AbstractElement* );
  virtual std::vector<AbstractElement*> findreplacables( AbstractElement * );
  void remove( AbstractElement *, bool = true );

  void replace( AbstractElement * );
  
  AbstractElement* index( size_t ) const;
  AbstractElement* rindex( size_t ) const;

  std::vector<AbstractElement*> words() const;
  AbstractElement* words( size_t ) const;
  AbstractElement* rwords( size_t ) const;

  bool isinstance( ElementType et ) const {
    return et == _element_id;
  }
  bool acceptable( ElementType ) const;
  bool addable( const AbstractElement *, const std::string& = "" ) const;
  virtual bool contains( const AbstractElement * ) const;
  
  std::vector<AbstractElement*> select( ElementType elementtype,
					bool = TRUE);
  std::vector<AbstractElement*> select( ElementType elementtype,
					std::set<ElementType>& ,
					bool = TRUE);
  std::vector<AbstractElement*> select( ElementType elementtype,
					const std::string&,
					bool = TRUE);
  std::vector<AbstractElement*> select( ElementType elementtype,
					const std::string&,
					std::set<ElementType>& ,
					bool = TRUE);
  KWargs collectAttributes() const;  
  //XML (de)serialisation
  std::string xmlstring() const; // serialize to a string (XML fragment)
  virtual xmlNode *xml( Document *, bool ) const; //serialize to XML  
  virtual AbstractElement* parseXml( xmlNode * );
  UnicodeString unicode() const { return text(); };
  virtual std::string str() const;
  bool hastext( TextCorrectionLevel ) const ;
  virtual UnicodeString text( TextCorrectionLevel=NOCORR ) const;
  virtual AbstractElement *head() const {
    throw NotImplementedError("head()"); }; 
  virtual std::string feat( const std::string& ) const {
    throw NotImplementedError("feat()"); }; 
  virtual AbstractElement *getNew( int = -1 ){
    throw NotImplementedError("getNew()"); };
  virtual AbstractElement *getOriginal( int = -1)
  { throw NotImplementedError("getOriginal()"); };
  virtual AbstractElement *getCurrent()
  { throw NotImplementedError("getCurrent()"); };
  virtual AbstractElement *getSuggestion( int = -1 )
  { throw NotImplementedError("getSuggestion() for " + _xmltag ); };
  virtual AbstractElement *split( AbstractElement *, AbstractElement *, 
				  const std::string& = "" ){
    throw NotImplementedError("split()"); };
  virtual Correction *mergewords( AbstractElement *, 
				  std::vector<AbstractElement *>&,
				  const std::string& = "" ){
    throw NotImplementedError("mergewords()"); };
  virtual Correction *deleteword( AbstractElement *, 
				  const std::string& = "" ){
    throw NotImplementedError("deleteword()"); };
  virtual Correction *insertword( AbstractElement *, AbstractElement *,
				  const std::string& = "" ){
    throw NotImplementedError("insertword()"); };
  virtual std::vector<AbstractElement *> suggestions()
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
    throw NotImplementedError("leftcontect()"); };
  virtual std::vector<AbstractElement *> rightcontext( size_t, 
						       const std::string& ="" ) const {
    throw NotImplementedError("rightcontext()"); };
  virtual AbstractElement *annotation( ElementType );
  virtual AbstractElement *annotation( ElementType, const std::string& );
  virtual std::string generateId( const std::string&, const std::string& = "" );
  std::string pos();
  std::string lemma();
  virtual TextCorrectionLevel corrected() const { return NOCORR; };
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
  Document *doc() { return mydoc; };
  virtual Sentence *sentence() const {
    throw NotImplementedError("sentence() for " + _xmltag );
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
  TextContent *addText( const std::string&, TextCorrectionLevel );
  Alternative *addAlternative( ElementType, const KWargs& );
  AbstractTokenAnnotation *addAnnotation( ElementType, const KWargs& );
  AbstractTokenAnnotation *addPosAnnotation( const KWargs& );
  AbstractTokenAnnotation *addLemmaAnnotation( const KWargs& );
  virtual std::vector<AbstractElement *> alternatives( const std::string& = "",
						       AnnotationType::AnnotationType=AnnotationType::NO_ANN ) { 
    throw NotImplementedError("alternatives()"); 
  }
  virtual std::string content(){
    throw NoSuchAnnotation( "content" );
  }
  virtual Correction *correct( std::vector<AbstractElement*>,
			       std::vector<AbstractElement*>,
			       std::vector<AbstractElement*>,
			       std::vector<AbstractElement*>,
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
  std::vector<AbstractElement*> data;
  AbstractElement *_parent;
  UnicodeString _text;
  Document *mydoc;
  std::string _xmltag;
  ElementType _element_id;
  std::set<ElementType> _accepted_data;
  AnnotationType::AnnotationType _annotation_type;

  Attrib _required_attributes;
  Attrib _optional_attributes;
  size_t occurrences;
  size_t occurrences_per_set;
  TextCorrectionLevel MINTEXTCORRECTIONLEVEL;
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
  boost::posix_time::ptime _datetime;
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
  AbstractElement *annotation( ElementType );
  AbstractElement *annotation( ElementType, const std::string& );
  std::vector<AbstractElement *> alternatives( const std::string& = "",
					       AnnotationType::AnnotationType=AnnotationType::NO_ANN );
  AbstractElement *append( AbstractElement* );
  void setMaxId( AbstractElement * );
  int getMaxId( const std::string& );
  std::string generateId( const std::string&, const std::string& = "" );
  Correction *correct( std::vector<AbstractElement*>,
		       std::vector<AbstractElement*>,
		       std::vector<AbstractElement*>,
		       std::vector<AbstractElement*>,
		       const KWargs& );
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

class TextContent: public AbstractElement {
 public:
 TextContent( const std::string& s="" ):  AbstractElement( ){ classInit( s ); }
 TextContent( Document *d=0, const std::string& s="" ):  AbstractElement( d ){ classInit( s ); }
  AbstractElement* parseXml( xmlNode * );
  xmlNode *xml( Document *, bool ) const;
  void setAttributes( const KWargs& );
  std::string str() const;
  UnicodeString text( TextCorrectionLevel ) const;
  AbstractElement *append( AbstractElement* ){ throw NotImplementedError("TextContent::append()"); };
  std::vector<AbstractElement*> findreplacables( AbstractElement * );
  TextCorrectionLevel corrected() const { return _corrected; };
 private:
  void init();
  TextCorrectionLevel _corrected;
  int offset;
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
  std::string content();
 private:
  void init();
};

class Content: public AbstractElement {
 public:
 Content( const std::string& s=""): AbstractElement( ) { classInit( s ); };
 Content( Document *d=0, const std::string& s=""): AbstractElement( d ) { classInit( s ); };
  AbstractElement* parseXml( xmlNode * );
  xmlNode *xml( Document *, bool ) const;
  std::string content();
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


class Word: public AbstractStructureElement {
 public:
 Word( const std::string& s=""):  AbstractStructureElement(){ classInit( s ); };
 Word( Document *d=0,  const std::string& s=""):  AbstractStructureElement( d ){ classInit( s ); };
  void setAttributes( const KWargs& );
  Correction *correct( const std::string& = "" );
  Correction *correct( AbstractElement*,
		       AbstractElement*,
		       const KWargs& );
  AbstractElement *split( AbstractElement *, AbstractElement *,
			  const std::string& = "" );
  Sentence *sentence() const;
  AbstractElement *previous() const;
  AbstractElement *next() const;
  std::vector<AbstractElement *> context( size_t, 
					  const std::string& ="" ) const;
  std::vector<AbstractElement *> leftcontext( size_t, 
					  const std::string& = "" ) const;
  std::vector<AbstractElement *> rightcontext( size_t, 
					  const std::string& ="" ) const;
  AbstractElement *append( AbstractElement *);
 private:
  void init();
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
  Correction *mergewords( AbstractElement *, std::vector<AbstractElement *>&,
			  const std::string& = "" );
  Correction *deleteword( AbstractElement *, const std::string& args );
  Correction *insertword( AbstractElement *, AbstractElement *,
			  const std::string& args );
 private:
  Correction *correctWords( std::vector<AbstractElement *>&,
			    std::vector<AbstractElement *>&,
			    std::vector<AbstractElement *>&, 
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

class SenseAnnotation: public AbstractTokenAnnotation {
 public:
 SenseAnnotation( const std::string& s="" ): AbstractTokenAnnotation(){ classInit( s ); };
 SenseAnnotation( Document *d=0, const std::string& s="" ): AbstractTokenAnnotation( d ){ classInit( s ); };
 private:
  void init();
};

class WordReference: public AbstractElement {
 public:
 WordReference( const std::string& s="" ): AbstractElement( ){ classInit( s ); };
 WordReference( Document *d=0, const std::string& s="" ): AbstractElement( d ){ classInit( s ); };
 private:
  void init();
  AbstractElement* parseXml( xmlNode *node );
};  

class SyntacticUnit: public AbstractElement {
 public:
 SyntacticUnit( const std::string& s ): AbstractElement( ){ classInit( s ); }
 SyntacticUnit( Document *d=0, const std::string& s="" ): AbstractElement( d ){ classInit( s ); }
  xmlNode *xml( Document *, bool ) const;
  AbstractElement *append( AbstractElement* );
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

class SyntaxLayer: public AbstractAnnotationLayer {
 public:
 SyntaxLayer( const std::string& s=""): AbstractAnnotationLayer( s ){ classInit( s ); }
 SyntaxLayer( Document *d=0, const std::string& s=""): AbstractAnnotationLayer( d ){ classInit( s ); }
 private:
  void init();
};

class NewElement: public AbstractElement {
 public:
 NewElement( const std::string& s=""): AbstractElement( ) { classInit( s ); };
 NewElement( Document *d=0, const std::string& s=""): AbstractElement( d ) { classInit( s ); };
  void setAttributes( const KWargs& );
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
  void setAttributes( const KWargs& );
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
  std::string description() const { return value; };
  AbstractElement* parseXml( xmlNode * );
  xmlNode *xml( Document *, bool ) const;
 private:
  void init();
  std::string value;
};

class Correction: public AbstractElement {
 public:
 Correction( const std::string& s=""): AbstractElement( ){ classInit( s ); }
 Correction( Document *d=0, const std::string& s=""): AbstractElement( d ){ classInit( s ); }
  bool hasNew();
  bool hasOriginal();
  bool hasCurrent();
  bool hasSuggestions();
  AbstractElement *getNew( int = -1 );
  AbstractElement *getOriginal( int = -1 );
  AbstractElement *getCurrent( int = -1 );
  AbstractElement *getSuggestion( int = -1 );
  std::vector<AbstractElement *> suggestions();
  UnicodeString text( TextCorrectionLevel=NOCORR) const;
 private:
  void init();
};

class ErrorDetection: public AbstractTokenAnnotation  {
 public: 
 ErrorDetection( const std::string& s=""): AbstractTokenAnnotation(){ classInit( s ); }
 ErrorDetection( Document *d=0, const std::string& s=""): AbstractTokenAnnotation( d ){ classInit( s ); }
 void setAttributes( const KWargs& );
  xmlNode* xml(Document*, bool) const;
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

class MorphologyLayer: public AbstractSubtokenAnnotationLayer {
 public:
 MorphologyLayer( const std::string& s="" ): AbstractSubtokenAnnotationLayer( s ){ classInit( s ); }
 MorphologyLayer( Document *d=0, const std::string& s="" ): AbstractSubtokenAnnotationLayer( d ){ classInit( s ); }
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

class Feature: public AbstractElement {
 public:
 Feature( const std::string& s=""): AbstractElement( ){ classInit( s ); }
 Feature( Document *d=0, const std::string& s=""): AbstractElement( d ){ classInit( s ); }
  void setAttributes( const KWargs& );
  xmlNode* xml(Document*, bool) const;
  std::string subset() const { return _subset; };
 private:
  void init();
  std::string _subset;
};

#endif
