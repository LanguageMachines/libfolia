#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <list>
#include "unicode/unistr.h"
#include "unicode/regex.h"
#include "libxml/tree.h"
#include "libxml/xpath.h"
#include <ctime>
#include "foliautils.h"

extern const std::string NSFOLIA;

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

class AbstractElement;

class Document {
  friend bool operator==( const Document&, const Document& );
  friend std::ostream& operator<<( std::ostream&, const Document& );
 public:
  Document();
  Document( const std::string& );
  ~Document();
  void init();
  bool readFromFile( const std::string& );
  bool readFromString( const std::string& );
  bool save( const std::string&, const std::string& = "" );
  int size() const;
  AbstractElement* doc() const { return foliadoc; }
  AbstractElement* addNode( ElementType, const KWargs& );
  void addStyle( const std::string& st ){
    styles.push_back( st );
  };
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
  std::string toXml( const std::string& ="" ) const;

  AbstractElement *append( AbstractElement* );

  void addDocIndex( AbstractElement* el, const std::string& );

  AbstractElement* operator []( size_t ) const; //select i'th element from data
  
  AbstractElement *index( const std::string& ) const; //retrieve element with specified ID 
  AbstractElement* operator []( const std::string& ) const ; //index as operator 
  bool isDeclared( const std::string&, AnnotationType::AnnotationType );
  std::string defaultset( AnnotationType::AnnotationType ) const;

  std::string defaultannotator( AnnotationType::AnnotationType, 
				const std::string& ="", bool = false ) const;
  std::string defaultannotatortype( AnnotationType::AnnotationType, 
				    const std::string& ="", bool = false ) const;

  AbstractElement* parseXml( );

  std::string id() const { return _id; };
  void declare( AnnotationType::AnnotationType, 
		const std::string&,
		const std::string& = "" );
  xmlDoc *XmlDoc() const { return xmldoc; };
  xmlNs *foliaNs() const { return _foliaNs; };
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

 private:
  std::map<AnnotationType::AnnotationType,std::map<std::string,at_t> > annotationdefaults;

  AbstractElement* parseFoliaDoc( xmlNode * );
  void setimdi( xmlNode * );
  void setAttributes( const KWargs&  );
  void parseannotations( xmlNode * );
  void getstyles();
  void setannotations( xmlNode *) const;
  void setmetadata( xmlNode * ) const;
  void setstyles( xmlDoc* ) const;
  std::map<std::string, AbstractElement* > sindex;
  std::vector<AbstractElement* > iindex;
  std::vector<AbstractElement*> data;
  std::string _id;
  std::list<ts_t> annotations;
  std::set<AbstractElement *> delSet;
  AbstractElement *foliadoc;
  xmlDoc *xmldoc;
  mutable xmlNs *_foliaNs;
  MetaDataType metadatatype;
  xmlNode *metadata;
  std::string _title;
  std::string _date;
  std::string _language;
  std::string _publisher;
  std::string _license;
  std::vector<std::string> styles;
  bool loadall;
  std::string filename;
  std::string version;
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

#endif // DOCUMENT_H
