/*
  $Id$
  $URL$

  Copyright (c) 1998 - 2015
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
#include "libxml/tree.h"
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
  class Correction;
  class NewElement;
  class Original;
  class Current;
  class Suggestion;
  class Division;
  class DependencyDependent;
  class Paragraph;
  class Morpheme;

  extern std::set<ElementType> default_ignore;
  extern std::set<ElementType> default_ignore_annotations;
  extern std::set<ElementType> default_ignore_structure;

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

    virtual std::string str() const = 0;
    UnicodeString unicode() const { return text(); };
    UnicodeString toktext( const std::string& cls = "current" ) const {
      return deeptext( cls, true );
    }
    virtual UnicodeString text( const std::string& = "current", bool = false ) const = 0;
    UnicodeString stricttext( const std::string& = "current" ) const;
    virtual UnicodeString deeptext( const std::string& = "current", bool = false ) const = 0;

    virtual bool printable() const = 0;

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
    virtual Original *getOriginal() const NOT_IMPLEMENTED;
    virtual Current *getCurrent() const NOT_IMPLEMENTED;
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
    FoliaImpl( Document* = 0 );
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

    std::string str() const;
    UnicodeString text( const std::string& = "current", bool = false ) const;

    UnicodeString deeptext( const std::string& = "current", bool = false ) const;
    bool printable() const { return PRINTABLE; };

    // Word
    const Word* resolveword( const std::string& ) const { return 0; };
    Word *addWord( const KWargs& );

    // TextContent
    TextContent *textcontent( const std::string& = "current" ) const;

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
    double confidence() const { return _confidence; };
    void confidence( double d ) { _confidence = d; };
    ElementType element_id() const { return _element_id; };
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
    Attrib _required_attributes;
    Attrib _optional_attributes;
    std::string _id;
    std::string _class;
    std::string _set;

  private:
    void addFeatureNodes( const KWargs& args );
    std::string _annotator;
    std::string _n;
    std::string _datetime;
    AnnotatorType _annotator_type;
    double _confidence;
    int _refcount;
  };

  bool isSubClass( const ElementType e1, const ElementType e2 );

  inline bool isSubClass( const FoliaElement *e1, const FoliaElement *e2 ){
    return isSubClass( e1->element_id(), e2->element_id() );
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
  AbstractStructureElement( Document *d=0 ): FoliaImpl( d ) { classInit(); };

    std::string str() const;
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
  AbstractAnnotation( Document *d=0 ): FoliaImpl( d ){};
  };

  class AbstractTokenAnnotation:
    public AbstractAnnotation,
    public AllowGenerateID {
  public:
  AbstractTokenAnnotation( Document *d=0 ): AbstractAnnotation( d ){ classInit(); };
  private:
    void init();
  };

  class Feature: public FoliaImpl {
  public:
  Feature( const std::string& s=""): FoliaImpl(){ classInit( s ); }
  Feature( const KWargs& a ): FoliaImpl(){ classInit( a ); }
  Feature( Document *d, const std::string& s=""): FoliaImpl(d){ classInit( s ); }
  Feature( Document *d, const KWargs& a ): FoliaImpl(d){ classInit( a ); }
    void setAttributes( const KWargs& );
    KWargs collectAttributes() const;
    std::string subset() const { return _subset; };

  protected:
    std::string _subset;

  private:
    void init();
  };

  class AbstractSpanAnnotation: public AbstractAnnotation,
    public AllowGenerateID,
    public AllowAnnotation,
    public AllowCorrection
    {
  public:
  AbstractSpanAnnotation( Document *d=0 ): AbstractAnnotation( d ){ classInit(); };
    xmlNode *xml( bool, bool=false ) const;
    FoliaElement *append( FoliaElement* );

    std::vector<FoliaElement*> wrefs() const;

  private:
    void init();
  };

  class AbstractTextMarkup: public AbstractAnnotation {
  public:
  AbstractTextMarkup( Document *d=0 ): AbstractAnnotation( d ){ classInit(); };
    void setAttributes( const KWargs& );
    KWargs collectAttributes() const;
    const FoliaElement* resolveid() const;
    UnicodeString text( const std::string& = "current", bool = false ) const;
  protected:
    std::string getTextDelimiter( bool ) const { return ""; };
    std::string idref;
  private:
    void init();
  };

  class TextMarkupGap: public AbstractTextMarkup {
  public:
  TextMarkupGap( Document *d=0 ): AbstractTextMarkup( d ){ classInit(); };
  private:
    void init();
  };

  class TextMarkupString: public AbstractTextMarkup {
  public:
  TextMarkupString( Document *d=0 ): AbstractTextMarkup( d ){ classInit(); };
  private:
    void init();
  };

  class TextMarkupCorrection: public AbstractTextMarkup {
  public:
  TextMarkupCorrection( Document *d=0 ): AbstractTextMarkup( d ){ classInit(); };
    void setAttributes( const KWargs& );
    KWargs collectAttributes() const;
    UnicodeString text( const std::string& = "current", bool = false ) const;
  private:
    void init();
    std::string _original;
  };

  class TextMarkupError: public AbstractTextMarkup {
  public:
  TextMarkupError( Document *d=0 ): AbstractTextMarkup( d ){ classInit(); };
  private:
    void init();
  };

  class TextMarkupStyle: public AbstractTextMarkup {
  public:
  TextMarkupStyle( Document *d=0 ): AbstractTextMarkup( d ){ classInit(); };
  private:
    void init();
  };

  class TextContent: public FoliaImpl {
  public:
  TextContent( const std::string& s="" ): FoliaImpl(){ classInit( s ); }
  TextContent( const KWargs& a ): FoliaImpl(){ classInit( a ); }
  TextContent( Document *d, const std::string& s="" ): FoliaImpl(d){ classInit( s ); }
  TextContent( Document *d, const KWargs& a ): FoliaImpl(d){ classInit( a ); }
    void setAttributes( const KWargs& );
    KWargs collectAttributes() const;
    std::string str() const;
    UnicodeString text( const std::string& = "current", bool = false ) const;
    int offset() const { return _offset; };
    TextContent *postappend();
    std::vector<FoliaElement*> findreplacables( FoliaElement * ) const;
  private:
    void init();
    int _offset;
    std::string _lang; // UNUSED!
    UnicodeString _text;
  };

  class FoLiA: public FoliaImpl {
  public:
  FoLiA( const std::string& s="" ): FoliaImpl() { classInit( s ); };
  FoLiA( const KWargs& a ): FoliaImpl() { classInit( a ); };
  FoLiA( Document *d, const std::string& s=""): FoliaImpl(d) { classInit( s ); };
  FoLiA( Document *d, const KWargs& a ): FoliaImpl(d) { classInit( a ); };
  private:
    void init();
  };

  class DCOI: public FoliaImpl {
  public:
  DCOI( const std::string& s=""): FoliaImpl() { classInit( s ); };
  DCOI( const KWargs& a ): FoliaImpl() { classInit( a ); };
  DCOI( Document *d, const std::string& s=""): FoliaImpl(d) { classInit( s ); };
  DCOI( Document *d, const KWargs& a ): FoliaImpl(d) { classInit( a ); };
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

  class Gap: public FoliaImpl {
  public:
  Gap( const std::string& s=""): FoliaImpl() { classInit( s ); };
  Gap( const KWargs& a ): FoliaImpl() { classInit( a ); };
  Gap( Document *d, const std::string& s=""): FoliaImpl(d) { classInit( s ); };
  Gap( Document *d, const KWargs& a ): FoliaImpl(d) { classInit( a ); };
    std::string content() const;
  private:
    void init();
  };

  class Content: public FoliaImpl {
  public:
  Content( const std::string& s=""): FoliaImpl() { classInit( s ); };
  Content( const KWargs& a ): FoliaImpl() { classInit( a ); };
  Content( Document *d, const std::string& s=""): FoliaImpl(d) { classInit( s ); };
  Content( Document *d, const KWargs& a ): FoliaImpl(d) { classInit( a ); };
    FoliaElement* parseXml( const xmlNode * );
    xmlNode *xml( bool, bool = false ) const;
    std::string content() const { return value; };
  private:
    void init();
    std::string value;
  };

  class MetricAnnotation: public FoliaImpl {
  public:
  MetricAnnotation( const std::string& s=""): FoliaImpl() { classInit( s ); };
  MetricAnnotation( const KWargs& a ): FoliaImpl() { classInit( a ); };
  MetricAnnotation( Document *d, const std::string& s=""): FoliaImpl(d) { classInit( s ); };
  MetricAnnotation( Document *d, const KWargs& a ): FoliaImpl(d) { classInit( a ); };
  private:
    void init();
  };

  class Division: public AbstractStructureElement {
  public:
  Division( const std::string& s=""): AbstractStructureElement() { classInit( s ); };
  Division( const KWargs& a ): AbstractStructureElement() { classInit( a ); };
  Division( Document *d, const std::string& s=""): AbstractStructureElement( d ) { classInit( s ); };
  Division( Document *d, const KWargs& a ): AbstractStructureElement( d ) { classInit( a ); };
    Head *head() const;
  private:
    void init();
  };

  class LineBreak: public AbstractStructureElement {
  public:
  LineBreak( const std::string& s=""): AbstractStructureElement(){ classInit( s ); };
  LineBreak( const KWargs& a ): AbstractStructureElement(){ classInit( a ); };
  LineBreak( Document *d,  const std::string& s=""): AbstractStructureElement( d ){ classInit( s ); };
  LineBreak( Document *d,  const KWargs& a ): AbstractStructureElement( d ){ classInit( a ); };
    UnicodeString text( const std::string& = "current", bool = false ) const {
      return "";
    }
  private:
    void init();
  };

  class WhiteSpace: public AbstractStructureElement {
  public:
  WhiteSpace( const std::string& s=""): AbstractStructureElement(){ classInit( s ); };
  WhiteSpace( const KWargs& a ): AbstractStructureElement(){ classInit( a ); };
  WhiteSpace( Document *d,  const std::string& s=""): AbstractStructureElement( d ){ classInit( s ); };
  WhiteSpace( Document *d,  const KWargs& a ): AbstractStructureElement( d ){ classInit( a ); };
  private:
    void init();
  };

  class Word: public AbstractStructureElement {
  public:
  Word( const std::string& s="" ): AbstractStructureElement(){ classInit( s ); };
  Word( const KWargs& a ): AbstractStructureElement(){ classInit( a ); };
  Word( Document *d, const std::string& s=""): AbstractStructureElement( d ){ classInit( s ); };
  Word( Document *d, const KWargs& a ): AbstractStructureElement( d ){ classInit( a ); };
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
    void init();
    bool space;
  };

  class Part: public AbstractStructureElement {
  public:
  Part( const std::string& s="" ): AbstractStructureElement(){ classInit( s ); };
  Part( const KWargs& a ): AbstractStructureElement(){ classInit( a ); };
  Part( Document *d, const std::string& s=""): AbstractStructureElement( d ){ classInit( s ); };
  Part( Document *d, const KWargs& a ): AbstractStructureElement( d ){ classInit( a ); };
  private:
    void init();
  };

  class String:
    public AbstractTokenAnnotation,
    public AllowAnnotation,
    public AllowCorrection {
  public:
  String( const std::string& s="" ): AbstractTokenAnnotation(){ classInit( s ); };
  String( const KWargs& a ): AbstractTokenAnnotation(){ classInit( a ); };
  String( Document *d, const std::string& s=""): AbstractTokenAnnotation( d ){ classInit( s ); };
  String( Document *d, const KWargs& a ): AbstractTokenAnnotation( d ){ classInit( a ); };
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
  Sentence( const std::string& s=""): AbstractStructureElement(){ classInit( s ); }
  Sentence( const KWargs& a ): AbstractStructureElement(){ classInit( a ); }
  Sentence( Document *d, const std::string& s=""): AbstractStructureElement( d ){ classInit( s ); }
  Sentence( Document *d, const KWargs& a ): AbstractStructureElement( d ){ classInit( a ); }
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
			      const KWargs& );
    void init();
  };

  class Text: public AbstractStructureElement {
  public:
  Text( const std::string& s=""): AbstractStructureElement(){ classInit( s ); };
  Text( const KWargs& a ): AbstractStructureElement(){ classInit( a ); };
  Text( Document *d, const std::string& s=""): AbstractStructureElement( d ){ classInit( s ); };
  Text( Document *d, const KWargs& a ): AbstractStructureElement( d ){ classInit( a ); };
  private:
    void init();
  };

  class Event: public AbstractStructureElement {
  public:
  Event( const std::string& s=""): AbstractStructureElement(){ classInit( s ); };
  Event( const KWargs& a ): AbstractStructureElement(){ classInit( a ); };
  Event( Document *d, const std::string& s=""): AbstractStructureElement( d ){ classInit( s ); };
  Event( Document *d, const KWargs& a ): AbstractStructureElement( d ){ classInit( a ); };
  private:
    void init();
  };

  class Caption: public AbstractStructureElement {
  public:
  Caption( const std::string& s="" ): AbstractStructureElement(){ classInit( s ); };
  Caption( const KWargs& a ): AbstractStructureElement(){ classInit( a ); };
  Caption( Document *d, const std::string& s=""): AbstractStructureElement( d ){ classInit( s ); };
  Caption( Document *d, const KWargs& a ): AbstractStructureElement( d ){ classInit( a ); };
  private:
    void init();
  };

  class Label: public AbstractStructureElement {
  public:
  Label( const std::string& s=""): AbstractStructureElement(){ classInit( s ); };
  Label( const KWargs& a ): AbstractStructureElement(){ classInit( a ); };
  Label( Document *d, const std::string& s="" ): AbstractStructureElement( d ){ classInit( s ); };
  Label( Document *d, const KWargs& a ): AbstractStructureElement( d ){ classInit( a ); };
  private:
    void init();
  };

  class Item: public AbstractStructureElement {
  public:
  Item( const std::string& s=""): AbstractStructureElement(){ classInit( s ); };
  Item( const KWargs& a ): AbstractStructureElement(){ classInit( a ); };
  Item( Document *d, const std::string& s=""): AbstractStructureElement( d ){ classInit( s ); };
  Item( Document *d, const KWargs& a ): AbstractStructureElement( d ){ classInit( a ); };
  private:
    void init();
  };

  class List: public AbstractStructureElement {
  public:
  List( const std::string& s=""): AbstractStructureElement(){ classInit( s ); };
  List( const KWargs& a ): AbstractStructureElement(){ classInit( a ); };
  List( Document *d, const std::string& s=""): AbstractStructureElement( d ){ classInit( s ); };
  List( Document *d, const KWargs& a ): AbstractStructureElement( d ){ classInit( a ); };
  private:
    void init();
  };

  class Figure: public AbstractStructureElement {
  public:
  Figure( const std::string& s=""): AbstractStructureElement(){ classInit( s ); };
  Figure( const KWargs& a ): AbstractStructureElement(){ classInit( a ); };
  Figure( Document *d, const std::string& s="" ): AbstractStructureElement( d ){ classInit( s ); };
  Figure( Document *d, const KWargs& a ): AbstractStructureElement( d ){ classInit( a ); };
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
  Paragraph( const std::string& s=""): AbstractStructureElement(){ classInit( s ); };
  Paragraph( const KWargs& a ): AbstractStructureElement(){ classInit( a ); };
  Paragraph( Document *d, const std::string& s=""): AbstractStructureElement( d ){ classInit( s ); };
  Paragraph( Document *d, const KWargs& a ): AbstractStructureElement( d ){ classInit( a ); };
  private:
    void init();
  };

  class Alternative: public AbstractStructureElement {
  public:
  Alternative( const std::string& s=""): AbstractStructureElement(){ classInit( s ); };
  Alternative( const KWargs& a ): AbstractStructureElement(){ classInit( a ); };
  Alternative( Document *d, const std::string& s=""): AbstractStructureElement( d ){ classInit( s ); };
  Alternative( Document *d, const KWargs& a ): AbstractStructureElement( d ){ classInit( a ); };
  private:
    void init();
  };


  class AlternativeLayers: public FoliaImpl {
  public:
  AlternativeLayers( const std::string& s=""): FoliaImpl(){ classInit( s ); };
  AlternativeLayers( const KWargs& a ): FoliaImpl(){ classInit( a ); };
  AlternativeLayers( Document *d, const std::string& s=""): FoliaImpl(d){ classInit( s ); };
  AlternativeLayers( Document *d, const KWargs& a ): FoliaImpl(d){ classInit( a ); };
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
    std::string getTextDelimiter( bool=false) const;
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

  class WordReference: public FoliaImpl {
  public:
  WordReference( const std::string& s="" ): FoliaImpl(){ classInit( s ); };
  WordReference( const KWargs& a ): FoliaImpl(){ classInit( a ); };
  WordReference( Document *d, const std::string& s="" ): FoliaImpl(d){ classInit( s ); };
  WordReference( Document *d, const KWargs& a ): FoliaImpl(d){ classInit( a ); };
  private:
    void init();
    FoliaElement* parseXml( const xmlNode *node );
  };

  class Alignment: public FoliaImpl {
  public:
  Alignment( const std::string& s="" ): FoliaImpl(){ classInit( s ); };
  Alignment( const KWargs& a ): FoliaImpl(){ classInit( a ); };
  Alignment( Document *d, const std::string& s="" ): FoliaImpl(d){ classInit( s ); };
  Alignment( Document *d, const KWargs& a ): FoliaImpl(d){ classInit( a ); };
    std::string href() const { return _href; };
    std::vector<FoliaElement *>resolve() const;
    KWargs collectAttributes() const;
    void setAttributes( const KWargs& );
  private:
    void init();
    std::string _href;
    std::string all_type;
  };

  class AlignReference: public FoliaImpl {
    friend class Alignment;
  public:
  AlignReference( const std::string& s="" ): FoliaImpl(){ classInit( s ); };
  AlignReference( const KWargs& a ): FoliaImpl(){ classInit( a ); };
  AlignReference( Document *d, const std::string& s="" ): FoliaImpl(d){ classInit( s ); };
  AlignReference( Document *d, const KWargs& a ): FoliaImpl(d){ classInit( a ); };
    KWargs collectAttributes() const;
    void setAttributes( const KWargs& );
  private:
    void init();
    FoliaElement* parseXml( const xmlNode *node );
    FoliaElement *resolve( const Alignment *ref ) const;
    std::string refId;
    std::string ref_type;
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

  class AbstractAnnotationLayer: public FoliaImpl,
    public AllowGenerateID,
    public AllowAnnotation,
    public AllowCorrection {
  public:
  AbstractAnnotationLayer( const std::string& s=""): FoliaImpl() { classInit( s ); };
  AbstractAnnotationLayer( const KWargs& a ): FoliaImpl() { classInit( a ); };
  AbstractAnnotationLayer( Document *d, const std::string& s=""): FoliaImpl(d) { classInit( s ); };
  AbstractAnnotationLayer( Document *d, const KWargs& a ): FoliaImpl(d) { classInit( a ); };
    AbstractSpanAnnotation *findspan( const std::vector<FoliaElement*>& ) const;
  private:
    void init();
  };

  class AbstractCorrectionChild: public FoliaImpl {
  public:
  AbstractCorrectionChild( Document *d=0 ): FoliaImpl(d){ classInit(); };
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

  class Description: public FoliaImpl {
  public:
  Description( const std::string& s=""): FoliaImpl() { classInit( s ); };
  Description( const KWargs& a ): FoliaImpl() { classInit( a ); };
  Description( Document *d, const std::string& s="" ): FoliaImpl(d) { classInit( s ); };
  Description( Document *d, const KWargs& a ): FoliaImpl(d) { classInit( a ); };
    std::string description() const { return _value; };
    void setAttributes( const KWargs& kwargs );
    FoliaElement* parseXml( const xmlNode * );
    xmlNode *xml( bool, bool=false ) const;
  private:
    void init();
    std::string _value;
  };

  class XmlComment: public FoliaImpl {
  public:
  XmlComment( const std::string& s=""): FoliaImpl() { classInit( s ); };
  XmlComment( const KWargs& a ): FoliaImpl() { classInit( a ); };
  XmlComment( Document *d, const std::string& s="" ): FoliaImpl(d) { classInit( s ); };
  XmlComment( Document *d, const KWargs& a ): FoliaImpl(d) { classInit( a ); };
    FoliaElement* parseXml( const xmlNode * );
    xmlNode *xml( bool, bool=false ) const;
  private:
    void init();
    std::string _value;
  };

  class XmlText: public FoliaImpl {
  public:
  XmlText( const std::string& s=""): FoliaImpl() { classInit( s ); };
  XmlText( const KWargs& a ): FoliaImpl() { classInit( a ); };
  XmlText( Document *d, const std::string& s="" ): FoliaImpl(d) { classInit( s ); };
  XmlText( Document *d, const KWargs& a ): FoliaImpl( d ) { classInit( a ); };
    FoliaElement* parseXml( const xmlNode * );
    xmlNode *xml( bool, bool=false ) const;
    bool setvalue( const std::string& s ) { _value = s; return true; };
    std::string getTextDelimiter( bool ) const { return ""; };
    UnicodeString text( const std::string&, bool ) const;
  private:
    void init();
    std::string _value; //UTF8 value
  };

  class External: public FoliaImpl {
  public:
  External( const std::string& s=""): FoliaImpl( ) { classInit( s ); };
  External( const KWargs& a ): FoliaImpl( ) { classInit( a ); };
  External( Document *d, const std::string& s="" ): FoliaImpl(d) { classInit( s ); };
  External( Document *d, const KWargs& a ): FoliaImpl(d) { classInit( a ); };
    FoliaElement* parseXml( const xmlNode * );
    void resolve();
    void setAttributes( const KWargs& );
    KWargs collectAttributes() const;
  private:
    void init();
    std::string _src;
    bool _include;
  };


  class Note: public AbstractStructureElement {
  public:
  Note( const std::string& s=""): AbstractStructureElement(){ classInit( s ); };
  Note( const KWargs& a ): AbstractStructureElement(){ classInit( a ); };
  Note( Document *d, const std::string& s=""): AbstractStructureElement( d ){ classInit( s ); };
  Note( Document *d, const KWargs& a ): AbstractStructureElement( d ){ classInit( a ); };
    KWargs collectAttributes() const;
    void setAttributes( const KWargs& );
  private:
    void init();
    std::string refId;
  };

  class Reference: public AbstractStructureElement {
    friend class Note;
  public:
  Reference( const std::string& s="" ): AbstractStructureElement( ){ classInit( s ); };
  Reference( const KWargs& a ): AbstractStructureElement( ){ classInit( a ); };
  Reference( Document *d, const std::string& s="" ): AbstractStructureElement( d ){ classInit( s ); };
  Reference( Document *d, const KWargs& a ): AbstractStructureElement( d ){ classInit( a ); };
    KWargs collectAttributes() const;
    void setAttributes( const KWargs& );
  private:
    void init();
    std::string refId;
    std::string ref_type;
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
    std::string getTextDelimiter( bool=false) const;
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
