/*
  Copyright (c) 2006 - 2021
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

#include "libfolia/folia_properties.h"

using namespace icu;

namespace folia {
  class Document;
  class AbstractSpanAnnotation;
  class Alternative;
  class PosAnnotation;
  class LemmaAnnotation;
  class SenseAnnotation;
  class DomainAnnotation;
  class MorphologyLayer;
  class Morpheme;
  class Phoneme;
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
  class MetaData;

  /// class used to steer 'text()' search behaviour
  /// the values may be logically 'or'-ed, like RETAIN|HIDDEN
  enum class TEXT_FLAGS {
    NONE=0,     //!< None of the flags is set. This is the default.
      RETAIN=1, //!< When returning text, keep al tokenization.
      STRICT=2, /*!< return only text from the current textcontent sibling.
		  The default is NOT STRICT, meaning to get text from deeper
		  textcontent nodes too. (stopping at the first that HAS text)
		*/
      HIDDEN=4,  //!< Include text from 'hidden' nodes.
      NO_TRIM_SPACES=8 /*!< Do not trim leading and trailing spaces (was the default
                          prior to FoLiA v2.4.1, see
                          https://github.com/proycon/folia/issues/92
                        */

      };

  inline TEXT_FLAGS operator&( TEXT_FLAGS f1, TEXT_FLAGS f2 ){
    return (TEXT_FLAGS)((int)f1&(int)f2);
  }

  inline TEXT_FLAGS operator|( TEXT_FLAGS f1, TEXT_FLAGS f2 ){
    return (TEXT_FLAGS) ((int)f1|(int)f2);
  }

  inline TEXT_FLAGS& operator|=( TEXT_FLAGS& f1, TEXT_FLAGS f2 ){
    f1 = (f1 | f2);
    return f1;
  }

  inline TEXT_FLAGS& operator&=( TEXT_FLAGS& f1, TEXT_FLAGS f2 ){
    f1 = (f1 & f2);
    return f1;
  }

  inline TEXT_FLAGS operator~( TEXT_FLAGS f1 ){
    return TEXT_FLAGS( ~(int)f1 );
  }

  /// class used to steer 'select()' behaviour
  enum class SELECT_FLAGS {
    RECURSE=0,  /*!< recurse the whole FoLia from the given node downwards
		  returning all matching nodes, even within matches.
		  This is the default.
		*/
      LOCAL=1,  //!< only just look in the direct sibblings of the given node
      TOP_HIT=2 //!< like recurse, but do NOT recurse into sibblings of matching nodes
      };

  /// class used to steer 'xml:space' behaviour
  enum class SPACE_FLAGS {
    UNSET=-1,  //!< not yet known
      DEFAULT=0,  //!< the default behaviour
      PRESERVE=1 //!< spaces should be preserved
      };


  class FoliaElement;
  class TextPolicy;

  typedef UnicodeString (*stringFunctionPointer)( const FoliaElement*,
						  const TextPolicy& );

  class TextPolicy {
  public:
    TextPolicy( const std::string& = "current",
		const TEXT_FLAGS=TEXT_FLAGS::NONE );
    TextPolicy( const TEXT_FLAGS );
    bool is_set( TEXT_FLAGS ) const;
    void set( TEXT_FLAGS );
    void clear( TEXT_FLAGS );
    void add_handler( const std::string&, const stringFunctionPointer& );
    stringFunctionPointer get_handler( const std::string& ) const;
    std::string get_class() const { return _class; };
    void set_class( const std::string& c ) { _class = c; };
  private:
    std::string _class;
    TEXT_FLAGS _text_flags;
    std::map<std::string, stringFunctionPointer> _tag_handlers;
  };

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

    template <typename T>
      inline T *create_child( KWargs& args ){
      /// create a new FoliaElement of type T as child of this
      /*!
	\param args an attribute-value list of arguments to use
	\return a new FoliaElement
	may throw if a Document is required, but not available
      */
      T *nt = new T(args, doc());
      append( nt );
      return nt;
    }

    template <typename T>
      inline T *create_child(){
      /// create a new FoliaElement of type T as child of this
      /*!
	\return a new FoliaElement
	may throw if a Document is required, but not available
      */
      T *nt = new T(doc());
      append( nt );
      return nt;
    }

    bool isSubClass( ElementType ) const;
    bool isSubClass( const FoliaElement *c ) const {
      /// check if the object is a subclass of the class of \e c
      /*!
	\param c the FoliaElement we would like to compare to
	\return true if the object is a SubClass of c.
	This is about C++ class inheritance: is our class a derivative of c's
	class?
      */
      return dynamic_cast<decltype(c)>(this) != 0;
    };

    virtual void assignDoc( Document* ) = 0;
    virtual FoliaElement *parent() const = 0;
    virtual void set_parent( FoliaElement *p ) = 0;
    virtual bool acceptable( ElementType ) const = 0;
    virtual bool addable( const FoliaElement * ) const = 0;
    virtual FoliaElement *append( FoliaElement* ) = 0;
    virtual FoliaElement *postappend( ) = 0;
    virtual void remove( size_t, bool = true ) = 0;
    virtual void remove( FoliaElement *, bool = true ) = 0;
    virtual std::vector<FoliaElement*> find_replacables( FoliaElement * ) const = 0;
    virtual void replace( FoliaElement * ) = 0;
    virtual FoliaElement* replace( FoliaElement *, FoliaElement* ) = 0;
    virtual void insert_after( FoliaElement *, FoliaElement * ) = 0;
    virtual const std::vector<FoliaElement*>& data() const = 0;
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
					       (recurse?SELECT_FLAGS::RECURSE : SELECT_FLAGS::LOCAL) );
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
					       (recurse?SELECT_FLAGS::RECURSE : SELECT_FLAGS::LOCAL) );
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
					       (recurse?SELECT_FLAGS::RECURSE : SELECT_FLAGS::LOCAL) );
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
					       (recurse?SELECT_FLAGS::RECURSE : SELECT_FLAGS::LOCAL) );
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
					       (recurse?SELECT_FLAGS::RECURSE : SELECT_FLAGS::LOCAL) );
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
    virtual const std::string processor() const = 0;
    virtual void processor( const std::string& ) = 0;
    virtual void annotatortype( AnnotatorType t ) =  0;
    virtual AnnotationType annotation_type() const = 0;
    virtual PosAnnotation *addPosAnnotation( const KWargs& ) NOT_IMPLEMENTED;
    virtual LemmaAnnotation *addLemmaAnnotation( const KWargs& ) NOT_IMPLEMENTED;
    virtual MorphologyLayer *addMorphologyLayer( const KWargs& ) NOT_IMPLEMENTED;

    virtual PosAnnotation *getPosAnnotations( const std::string&,
					      std::vector<PosAnnotation*>& ) const NOT_IMPLEMENTED;
    virtual LemmaAnnotation *getLemmaAnnotations( const std::string&,
						  std::vector<LemmaAnnotation*>& ) const NOT_IMPLEMENTED;
    virtual MorphologyLayer *getMorphologyLayers( const std::string&,
						  std::vector<MorphologyLayer*>& ) const NOT_IMPLEMENTED;

    virtual const MetaData *get_metadata() const = 0;
    virtual const std::string get_metadata( const std::string& ) const = 0;

    template <typename F>
      std::vector<F*> annotations( const std::string& s = "" ) const {
      if ( allowannotations() ){
	return select<F>( s, default_ignore_annotations );
      }
      else NOT_IMPLEMENTED;
    }

    template <typename F>
      bool has_annotation( const std::string& st = "" ) const {
      std::vector<F*> v = annotations<F>( st );
      return v.size() > 0;
    }

    template <typename F>
      F *annotation( const std::string& st = "" ) const {
      std::vector<F*>v = annotations<F>( st );
      if ( v.size() > 0 ){
	return v[0];
      }
      else {
	return 0;
      }
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
    const std::string xmlstring( bool=true ) const; // serialize to a string (XML fragment)
    const std::string xmlstring( bool, int=0, bool=true ) const; // serialize to a string (XML fragment)
    virtual xmlNode *xml( bool, bool = false ) const = 0; //serialize to XML

    // text/string content
    bool hastext( const std::string& = "current" ) const;
    bool hasphon( const std::string& = "current" ) const;
    virtual void check_text_consistency(bool = true) const = 0;
    virtual void check_text_consistency_while_parsing(bool = true) = 0; //can't we merge these two somehow?
    virtual void check_append_text_consistency( const FoliaElement * ) const = 0;

    virtual const std::string str( const std::string& = "current" ) const = 0;
    virtual const std::string str( const TextPolicy& ) const = 0;

    const UnicodeString unicode( const std::string& cls = "current" ) const {
      return text( cls, TEXT_FLAGS::NONE ); };

    virtual UnicodeString text_container_text( const TextPolicy& ) const = 0;
    virtual const UnicodeString private_text( const TextPolicy& ) const = 0;
    virtual const UnicodeString text( const TextPolicy & ) const = 0;
    virtual const UnicodeString text( const std::string&,
				      TEXT_FLAGS = TEXT_FLAGS::NONE ) const = 0;
    virtual const UnicodeString text( TEXT_FLAGS = TEXT_FLAGS::NONE ) const = 0;
    const UnicodeString stricttext( const std::string& = "current" ) const;
    const UnicodeString toktext( const std::string& = "current" ) const;
    virtual const UnicodeString phon( const std::string&,
				      TEXT_FLAGS = TEXT_FLAGS::NONE ) const = 0;
    virtual const UnicodeString phon( TEXT_FLAGS = TEXT_FLAGS::NONE ) const = 0;
    virtual bool printable() const = 0;
    virtual bool speakable() const = 0;
    virtual bool referable() const = 0;
    virtual bool is_textcontainer() const = 0;
    virtual bool is_phoncontainer() const = 0;

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
    virtual Word *addWord( const std::string& ="" ) = 0;

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
    virtual const TextContent *text_content( const TextPolicy& ) const = 0;
    virtual const TextContent *text_content( const std::string& = "current" ) const = 0;
    TextContent *settext( const std::string&,
			  const std::string& = "current" );
    TextContent *settext( const std::string&,
			  int,
			  const std::string& = "current" );
    TextContent *setutext( const UnicodeString&,
			   const std::string& = "current" );
    TextContent *setutext( const UnicodeString&,
			   int ,
			   const std::string& = "current" );
    virtual int offset() const NOT_IMPLEMENTED;
    virtual void set_offset( int ) const NOT_IMPLEMENTED;

    void clear_textcontent( const std::string& = "current" );
    // PhonContent
    virtual const PhonContent *phon_content( const std::string& = "current",
					     bool = false ) const = 0;

    // properties
    virtual const std::string& get_delimiter( bool=false ) const = 0;
    virtual void setDateTime( const std::string& ) = 0;
    virtual const std::string getDateTime() const = 0;
    virtual const std::string pos( const std::string& = "" ) const NOT_IMPLEMENTED;
    virtual const std::string lemma( const std::string& = "" ) const NOT_IMPLEMENTED;
    virtual const std::string cls() const = 0;
    virtual const std::string sett() const = 0;
    virtual void update_cls( const std::string& ) = 0;
    virtual void update_set( const std::string& ) = 0;
    virtual const std::string n() const = 0;
    virtual const std::string tag() const = 0;
    virtual const std::string settag( const std::string& ) = 0;
    virtual const std::string id() const = 0;
    virtual const std::string begintime() const = 0;
    virtual const std::string endtime() const = 0;
    virtual const std::string speech_src() const = 0;
    virtual const std::string speech_speaker() const = 0;
    virtual const std::string language( const std::string& = "" ) const = 0;
    virtual const std::string set_to_current() NOT_IMPLEMENTED;
    virtual double confidence() const = 0;
    virtual void confidence( double ) = 0;
    virtual bool space() const = 0;
    virtual bool setspace( bool ) = 0;
    virtual SPACE_FLAGS preserve_spaces() const = 0;
    virtual void preserve_spaces( SPACE_FLAGS ) = 0;
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
    virtual std::vector<Word*> words( const std::string& ="" ) const NOT_IMPLEMENTED;
    virtual std::vector<FoliaElement*> wrefs() const NOT_IMPLEMENTED;
    virtual FoliaElement* wrefs( size_t ) const NOT_IMPLEMENTED;

    virtual std::vector<Morpheme*> morphemes( const std::string& ="" ) const NOT_IMPLEMENTED;
    virtual Morpheme* morpheme( size_t, const std::string& ="" ) const NOT_IMPLEMENTED;
    virtual Sentence *sentences( size_t ) const NOT_IMPLEMENTED;
    virtual Sentence *rsentences( size_t ) const NOT_IMPLEMENTED;
    virtual Paragraph *paragraphs( size_t ) const NOT_IMPLEMENTED;
    virtual Paragraph *rparagraphs( size_t ) const NOT_IMPLEMENTED;
    virtual Word *words( size_t, const std::string& ="" ) const NOT_IMPLEMENTED;
    virtual std::vector<Word *> wordParts() const NOT_IMPLEMENTED;
    virtual Word *rwords( size_t, const std::string& ="" ) const NOT_IMPLEMENTED;

    virtual DependencyDependent *dependent() const NOT_IMPLEMENTED;

    virtual const std::string description() const;

    // alternatives
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
    virtual const UnicodeString deeptext( const TextPolicy& ) const NOT_IMPLEMENTED;
    virtual const UnicodeString deepphon( const std::string& = "current",
					  TEXT_FLAGS = TEXT_FLAGS::NONE ) const NOT_IMPLEMENTED;


    virtual std::vector<FoliaElement*> select( ElementType,
					       SELECT_FLAGS = SELECT_FLAGS::RECURSE ) const = 0;
    virtual std::vector<FoliaElement*> select( ElementType,
					       const std::set<ElementType>& ,
					       SELECT_FLAGS = SELECT_FLAGS::RECURSE ) const = 0;
    virtual std::vector<FoliaElement*> select( ElementType,
					       const std::string&,
					       SELECT_FLAGS = SELECT_FLAGS::RECURSE ) const = 0;
    virtual std::vector<FoliaElement*> select( ElementType,
					       const std::string&,
					       const std::set<ElementType>& ,
					       SELECT_FLAGS = SELECT_FLAGS::RECURSE ) const = 0;
    // some 'internal stuff
    virtual int refcount() const = 0;
    virtual void increfcount() = 0;
    virtual void decrefcount() = 0;
    virtual void resetrefcount() = 0;
    virtual void setAttributes( KWargs& ) = 0;
    virtual KWargs collectAttributes() const = 0;
    virtual void setAuth( bool b ) = 0;
    virtual bool auth( ) const = 0;
    virtual bool xlink() const = 0;
    virtual const std::string href() const NOT_IMPLEMENTED;
    virtual const std::string generateId( const std::string& ) NOT_IMPLEMENTED;
    virtual const std::string textclass() const NOT_IMPLEMENTED;
    virtual void unravel( std::set<FoliaElement*>& ) NOT_IMPLEMENTED;
    static FoliaElement *private_createElement( ElementType );
  public:
    static FoliaElement *createElement( ElementType, Document * =0 );
    static FoliaElement *createElement( const std::string&, Document * =0 );

  };

  class AbstractElement: public virtual FoliaElement {
  private:
    AbstractElement( const AbstractElement& ); // inhibit copies
    AbstractElement& operator=( const AbstractElement& ); // inhibit copies
    //Constructor
  protected:
    AbstractElement( const properties& p, Document* = 0 );
  public:
    // expose static element Constructor
    virtual ~AbstractElement();

    void classInit(){
      // we could call 'init()' directly, but this is more esthetic
      // keep in balance with the next function
      init(); // virtual init
    }

    void classInit( const KWargs& a ){
      // this funcion is needed because calling the virtual function
      // setAttributes from the constructor will NOT call the right version
      // THIS IS BY DESIGN in C++
      init(); // virtual init
      KWargs a1 = a;
      setAttributes( a1 ); // also virtual!
    }

    //functions regarding contained data
    size_t size() const { return _data.size(); };
    FoliaElement* index( size_t ) const;
    FoliaElement* rindex( size_t ) const;

    bool isinstance( ElementType et ) const {
      /// return true when the object is an instance of the type parameter
      /*!
      \param et the type to check against
    */
      return et == element_id();
    }

    void assignDoc( Document* );
    FoliaElement *parent() const { return _parent; };
    void set_parent( FoliaElement *p ) { _parent = p ; };

    // modify the internal data
    FoliaElement *append( FoliaElement* );
    FoliaElement *postappend( );
    void remove( size_t, bool = true );
    void remove( FoliaElement *, bool = true );
    std::vector<FoliaElement*> find_replacables( FoliaElement * ) const;
    void replace( FoliaElement * );
    FoliaElement* replace( FoliaElement *, FoliaElement* );
    void insert_after( FoliaElement *, FoliaElement * );
    const std::vector<FoliaElement*>& data() const { return _data; };

    // Sentences
    Sentence *addSentence( const KWargs& );

    // MetaData
    const MetaData *get_metadata() const;
    const std::string get_metadata( const std::string&  ) const;

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
    const std::string processor( ) const { return _processor; };
    void processor( const std::string& p ) { _processor = p; };
    AnnotatorType annotatortype() const { return _annotator_type; };
    void annotatortype( AnnotatorType t ) { _annotator_type =  t; };

    template <typename F>
      F *addAnnotation( const KWargs& args ) {
      return FoliaElement::addAnnotation<F>( args );
    }

    // Span annotations
    std::vector<AbstractSpanAnnotation*> selectSpan() const;

    // features
    std::vector<std::string> feats( const std::string& ) const;
    const std::string feat( const std::string& ) const;

    //XML parsing
    FoliaElement* parseXml( const xmlNode * );

    // text/string content

    const std::string str( const std::string& = "current" ) const;
    const std::string str( const TextPolicy& ) const;

    UnicodeString text_container_text( const TextPolicy& ) const;
    const UnicodeString private_text( const TextPolicy& ) const;
    const UnicodeString text( const TextPolicy & ) const;
    const UnicodeString text( const std::string&,
			      TEXT_FLAGS = TEXT_FLAGS::NONE ) const;
    const UnicodeString text( TEXT_FLAGS flags = TEXT_FLAGS::NONE ) const {
      return text( "current", flags );
    }

    const UnicodeString phon( const std::string&,
			      TEXT_FLAGS = TEXT_FLAGS::NONE ) const;
    const UnicodeString phon( TEXT_FLAGS flags = TEXT_FLAGS::NONE ) const {
      return phon( "current", flags );
    }

    const UnicodeString deeptext( const TextPolicy& ) const;
    const UnicodeString deepphon( const std::string& = "current",
				  TEXT_FLAGS = TEXT_FLAGS::NONE ) const;

    // Word
    const Word* resolveword( const std::string& ) const { return 0; };
    Word *addWord( const KWargs& );
    Word *addWord( const std::string& ="" );
    // TextContent
    const TextContent *text_content( const TextPolicy& ) const;
    const TextContent *text_content( const std::string& = "current" ) const;
    // PhonContent
    const PhonContent *phon_content( const std::string& = "current",
				     bool = false ) const;

    // properties
    const std::string& get_delimiter( bool=false ) const;

    // attributes
    const std::string cls() const { return _class; };
    const std::string sett() const { return _set; };
    void update_cls( const std::string& cls ) { _class = cls; };
    void update_set( const std::string& st ) { _set = st; };
    const std::string tag() const { return _tags; };
    const std::string settag( const std::string&  );
    const std::string n() const { return _n; };
    const std::string id() const { return _id; };
    const std::string begintime() const { return _begintime; };
    const std::string endtime() const { return _endtime; };
    const std::string textclass() const { return _textclass; };
    const std::string speech_src() const;
    const std::string speech_speaker() const;
    const std::string language( const std::string& = "" ) const;
    const std::string src() const { return _src; };
    bool space() const { return _space; };
    bool setspace( bool b ) { bool s =_space; _space =  b; return s; };
    SPACE_FLAGS preserve_spaces() const { return _preserve_spaces; };
    void preserve_spaces( SPACE_FLAGS f ) { _preserve_spaces = f; };
    double confidence() const { return _confidence; };
    void confidence( double d ) { _confidence = d; };

    // generic properties
    ElementType element_id() const;
    size_t occurrences() const;
    size_t occurrences_per_set() const;
    Attrib required_attributes() const;
    Attrib optional_attributes() const;
    bool hidden() const;
    const std::string& xmltag() const;
    const std::string& default_subset() const;
    AnnotationType annotation_type() const;
    const std::set<ElementType>& accepted_data() const;
    const std::set<ElementType>& required_data() const;
    bool printable() const;
    bool speakable() const;
    bool referable() const;
    bool is_textcontainer() const;
    bool is_phoncontainer() const;
    bool auth() const;
    bool xlink() const;
    bool setonly() const;
    bool auto_generate_id() const;

    Document *doc() const { return _mydoc; };


    std::vector<FoliaElement*> select( ElementType,
				       SELECT_FLAGS = SELECT_FLAGS::RECURSE ) const;
    std::vector<FoliaElement*> select( ElementType,
				       const std::set<ElementType>& ,
				       SELECT_FLAGS = SELECT_FLAGS::RECURSE ) const;
    std::vector<FoliaElement*> select( ElementType,
				       const std::string&,
				       SELECT_FLAGS = SELECT_FLAGS::RECURSE ) const;
    std::vector<FoliaElement*> select( ElementType,
				       const std::string&,
				       const std::set<ElementType>& ,
				       SELECT_FLAGS = SELECT_FLAGS::RECURSE ) const;

    void unravel( std::set<FoliaElement*>& );

  protected:
    xmlNode *xml( bool, bool = false ) const;
    void setAttributes( KWargs& );
    bool checkAtts();
    void set_typegroup( KWargs& ) const;
    KWargs collectAttributes() const;
    int refcount() const { return _refcount; };
    void increfcount() { ++_refcount; };
    void decrefcount() { --_refcount; };
    void resetrefcount() { _refcount = 0; };
    void setAuth( bool b ){ _auth = b; };
    xmlNs *foliaNs() const;
    bool acceptable( ElementType ) const;
    bool addable( const FoliaElement * ) const;
    void setDateTime( const std::string& );
    const std::string getDateTime() const;
    void check_text_consistency(bool = true) const;
    void check_text_consistency_while_parsing(bool = true); //can't we merge these two somehow?
    void check_append_text_consistency( const FoliaElement * ) const;
    void check_declaration();
  private:
    void addFeatureNodes( const KWargs& args );
    Document *_mydoc;
    FoliaElement *_parent;
    bool _auth;
    bool _space;
    AnnotatorType _annotator_type;
    int _refcount;
    double _confidence;
    std::string _annotator;
    std::string _n;
    std::string _datetime;
    std::string _begintime;
    std::string _endtime;
    std::string _speaker;
    std::string _textclass;
    std::string _metadata;
    std::string _processor;
    std::string _set;
    std::string _class;
    std::string _id;
    std::string _src;
    std::string _tags;
    SPACE_FLAGS _preserve_spaces;
    std::vector<FoliaElement*> _data;
    const properties& _props;
  };

  bool isSubClass( const ElementType e1, const ElementType e2 );

  bool isSubClass( const FoliaElement *e1, const FoliaElement *e2 );

  template <typename T1, typename T2>
    bool isSubClass(){
    /// templated check if Type T1 is a subclass of Type T2
    /*!
      \return true if T1 is a SubClass of T2.
      This is about C++ class inheritance: is our class a derivative of c's
      class?
    */
    T1 t1((Document*)0);
    return dynamic_cast<T2*>(&t1) != 0;
  }

  bool operator==( const FoliaElement&, const FoliaElement& );
  inline bool operator!=( const FoliaElement& e1, const FoliaElement& e2 ){
    return !( e1 == e2 );
  }

  inline size_t len( const FoliaElement *e ) {
    /// return the number of FoliaElement children of '\e e
    return e->size();
  }

  template <typename T>
    inline size_t len( const std::vector<T>& v ) {
    /// return the size of the given vector
    return v.size(); }

  template <typename T>
    T *create( KWargs& args ){
    /// create a new FoliaElement
    /*!
      \param args an attribute-value list of arguments to use
      \return a new FoliaElement
    */
    return new T(args);
  }

  template <typename T>
    T *create( ){
    /// create a new FoliaElement
    /*!
      \return a new FoliaElement
    */
    return new T();
  }

  template <typename T>
    T *create( KWargs& args, FoliaElement* p ){
    /// create a new FoliaElement as a child of \e p
    /*!
      \param p the FoliaElement to connect to
      \param args an attribute-value list of arguments to use
      \return a new FoliaElement
    */
    T *nt = new T(args,p->doc());
    p->append( nt );
    return nt;
  }

  template <typename T>
    T *create( FoliaElement* p ){
    /// create a new FoliaElement as a child of \e p
    /*!
      \param p the FoliaElement to connect to
      \return a new FoliaElement
    */
    T *nt = new T(p->doc());
    p->append( nt );
    return nt;
  }

  inline const std::string str( const FoliaElement *e,
				const std::string& cls = "current" ) {
    /// return the string value contained in \e e
    /*!
      \param e The FoliaElement
      \param cls the textclass we want
      \return the (UTF8) string value
    */
    return e->str( cls ); }

  inline const std::string str( const FoliaElement *e,
				const TextPolicy& tp ){
    /// return the string value contained in \e e
    /*!
      \param e The FoliaElement
      \param tp the TextPolicy to use
      \return the (UTF8) string value
    */
    return e->str( tp );
  }

  inline const UnicodeString text( const FoliaElement *e,
				   const TextPolicy& tp ){
    /// return the Unicode value contained in \e e
    /*!
      \param e The FoliaElement
      \param tp the TextPolicy to use
      \return the Unicode string value
    */
    if ( e ){
      return e->text( tp );
    }
    else {
      throw ValueError( "text() for empty element" );
    }
  }

  inline const UnicodeString text( const FoliaElement *e,
				   const std::string& cls = "current" ) {
    /// return the Unicode value contained in \e e
    /*!
      \param e The FoliaElement
      \param cls the textclass we want
      \return the Unicode string value
    */
    if ( e ){
      return e->text( cls, TEXT_FLAGS::NONE );
    }
    else {
      throw ValueError( "text() for empty element" );
    }
  }

  inline const UnicodeString unicode( const FoliaElement *e ) {
    /// return the Unicode value contained in \e e
    /*!
      \param e The FoliaElement
      \return the Unicode string value
    */
    return e->unicode(); }

  inline bool isinstance( const FoliaElement *e, ElementType t ) {
    /// return true when the first parameter is an instance of the type
    /// given by the second parameter
    /*!
      \param e the FoliaElement to test
      \param t the type to check against
    */
    return e->isinstance( t ); }

  class AllowGenerateID: public virtual FoliaElement {
  public:
    void setMaxId( FoliaElement * );
    const std::string generateId( const std::string& tag );
  private:
    std::map<std::string, int> id_map;
  };

  class AllowCorrections: public virtual FoliaElement {
    /// Interface class that enables corrections on Elements
  public:
    Correction *correct( const std::vector<FoliaElement*>&,
			 const std::vector<FoliaElement*>&,
 			 const std::vector<FoliaElement*>&,
			 const std::vector<FoliaElement*>&,
			 const KWargs& );
    Correction *correct( FoliaElement*,
			 FoliaElement*,
			 const KWargs& );
    Correction* correct( FoliaElement*,
			 FoliaElement*,
			 const std::vector<FoliaElement*>&,
			 const KWargs& );
    Correction *correct( const std::string& = "" );
  };

  class AllowXlink: public virtual FoliaElement {
  public:
    const std::string href() const;
    void setAttributes( KWargs& );
    KWargs collectAttributes() const;
  protected:
    std::map<std::string,std::string> _xlink;
  };

  class AllowInlineAnnotation: public AllowCorrections {
  public:
    bool allowannotations() const { return true; };
    template <typename F>
      std::vector<F*> annotations( const std::string& s = "" ) const {
      return FoliaElement::annotations<F>( s );
    }

    template <typename F>
      int has_annotation( const std::string& st = "" ) const {
      return FoliaElement::has_annotation<F>(st);
    }

    template <typename F>
      F *annotation( const std::string& st = "" ) const {
      return FoliaElement::annotation<F>(st);
    }

    std::vector<Alternative *> alternatives( ElementType = BASE,
					     const std::string& = "" ) const;


    PosAnnotation *addPosAnnotation( const KWargs& );
    PosAnnotation *getPosAnnotations( const std::string&,
				      std::vector<PosAnnotation*>& ) const;
    LemmaAnnotation *addLemmaAnnotation( const KWargs& );
    LemmaAnnotation *getLemmaAnnotations( const std::string&,
					  std::vector<LemmaAnnotation*>& ) const;

  };

  class AbstractStructureElement:
    public AbstractElement,
    public AllowGenerateID,
    public AllowInlineAnnotation
    {
      friend void static_init();
    protected:
      // DO NOT USE AbstractStructureElement as a real node!!
    AbstractStructureElement( const properties& props, Document *d=0 ):
      AbstractElement( props, d ){ classInit(); };
      explicit AbstractStructureElement( Document *d=0 ):
      AbstractStructureElement( PROPS, d ){};
    public:
      FoliaElement *append( FoliaElement* );
      std::vector<Paragraph*> paragraphs() const;
      std::vector<Sentence*> sentences() const;
      std::vector<Word*> words( const std::string& ="" ) const;
      Sentence *sentences( size_t ) const;
      Sentence *rsentences( size_t ) const;
      Paragraph *paragraphs( size_t ) const;
      Paragraph *rparagraphs( size_t ) const;
      Word *words( size_t, const std::string& ="" ) const;
      Word *rwords( size_t, const std::string& ="" ) const;
      const Word* resolveword( const std::string& ) const;
    private:
      static properties PROPS;
    };

  class AbstractWord: public virtual FoliaElement {
    /// Interface class that is inherited by word-like (wrefable)
    /// elements (Word, Hiddenword, Morpheme, Phoneme)
  public:
    Sentence *sentence() const;
    Paragraph *paragraph() const;
    Division *division() const;
    const std::string pos( const std::string& = "" ) const;
    const std::string lemma( const std::string& = "" ) const;
    SenseAnnotation *sense( const std::string& = "" ) const;
    DomainAnnotation *domain( const std::string& = "" ) const;
    std::vector<Morpheme *> morphemes( const std::string& ="" ) const;
    std::vector<Phoneme *> phonemes( const std::string& ="" ) const;
    Morpheme *morpheme( size_t, const std::string& ="" ) const;
    Phoneme *phoneme( size_t, const std::string& ="" ) const;
    std::vector<Correction *> getcorrections( const std::string& ="",
					      const std::string& ="" ) const NOT_IMPLEMENTED;
    Correction *getcorrection( const std::string& ="",
			       const std::string& ="" ) const NOT_IMPLEMENTED;
    std::vector<AbstractSpanAnnotation*> findspans( ElementType,
						    const std::string& ="" ) const;
  };

  class AbstractInlineAnnotation:
    public AbstractElement,
    public AllowGenerateID
    {
      friend void static_init();
    protected:
      // DO NOT USE AbstractInlineAnnotation as a real node!!
    AbstractInlineAnnotation( const properties& props, Document *d=0 ):
      AbstractElement( props, d ){ classInit(); };
      explicit AbstractInlineAnnotation( Document *d=0 ):
      AbstractInlineAnnotation( PROPS, d ){};

    private:
      static properties PROPS;
    };

  class AbstractHigherOrderAnnotation:
    public AbstractElement
    {
      friend void static_init();
    protected:
      // DO NOT USE AbstractHigherOrderAnnotation as a real node!!
    AbstractHigherOrderAnnotation( const properties& props, Document *d=0 ):
      AbstractElement( props, d ){ classInit(); };
      explicit AbstractHigherOrderAnnotation( Document *d=0 ):
      AbstractHigherOrderAnnotation( PROPS, d ){};
    private:
      static properties PROPS;
    };


  class AbstractSpanAnnotation:
    public AbstractElement,
    public AllowGenerateID,
    public AllowInlineAnnotation
    {
      friend void static_init();
    protected:
      // DO NOT USE AbstractSpanAnnotation as a real node!!
    AbstractSpanAnnotation( const properties& props, Document *d=0 ):
      AbstractElement( props, d ){ classInit(); };
      explicit AbstractSpanAnnotation( Document *d=0 ):
      AbstractSpanAnnotation( PROPS, d ){};
    public:
      xmlNode *xml( bool, bool=false ) const;
      FoliaElement *append( FoliaElement* );

      std::vector<FoliaElement*> wrefs() const;
      FoliaElement *wrefs( size_t ) const;

    private:
      static properties PROPS;
    };

  class SpanRelation: public AbstractElement {
    friend void static_init();
  public:
    explicit SpanRelation( Document *d=0 ):
    AbstractElement( PROPS, d ){ classInit(); }
  SpanRelation( const KWargs& a, Document *d=0 ):
    AbstractElement( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class ForeignData: public AbstractElement {
    friend void static_init();
  public:
    explicit ForeignData( Document *d=0 ):
    ForeignData( PROPS, d ){};
    ~ForeignData();

    FoliaElement* parseXml( const xmlNode * );
    xmlNode *xml( bool, bool=false ) const;
    void set_data( const xmlNode * );
    xmlNode* get_data() const;
  private:
  ForeignData( const properties& props, Document *d=0 ):
    AbstractElement( props, d ){ classInit(); };
    void init();
    static properties PROPS;
    xmlNode *_foreign_data;
  };

#define META_NOT_IMPLEMENTED {						\
    throw NotImplementedError( "MetaTags::" + std::string(__func__) ); \
  }

  class MetaData {
  public:
  MetaData( const std::string& type ): _type(type){};
    virtual ~MetaData(){};
    virtual void add_av( const std::string&, const std::string& ) META_NOT_IMPLEMENTED;
    virtual const KWargs& get_avs() const META_NOT_IMPLEMENTED;
    virtual const std::string get_val( const std::string& ) const {
      return "";
    }
    virtual void add_foreign( const xmlNode * ) META_NOT_IMPLEMENTED;
    virtual std::string datatype() const { return "BaseMetaData"; };
    std::string type() const { return _type; };
    virtual std::string src() const META_NOT_IMPLEMENTED;
    virtual const std::vector<FoliaElement*> get_foreigners() const META_NOT_IMPLEMENTED;
  private:
    std::string _type;
  };

  class NativeMetaData: public MetaData {
  public:
    explicit NativeMetaData( const std::string& t ): MetaData(t) {};
    void add_av( const std::string& a, const std::string& v )
    { _attribs[a] = v; };
    const KWargs& get_avs() const {
      return _attribs;
    }
    const std::string get_val( const std::string& at ) const {
      auto const& it = _attribs.find( at );
      if ( it != _attribs.end() ){
	return it->second;
      }
      return "";
    }
    std::string datatype() const { return "NativeMetaData"; };
  private:
    KWargs _attribs;
  };

  class ForeignMetaData: public MetaData {
  public:
  ForeignMetaData( const std::string& t ): MetaData(t) {};
    ~ForeignMetaData();
    void add_foreign( const xmlNode * );
    std::string datatype() const { return "ForeignMetaData"; };
    const std::vector<FoliaElement*> get_foreigners() const { return foreigners;};
  private:
    std::vector<FoliaElement*> foreigners;
  };

  class ExternalMetaData: public MetaData {
  public:
  ExternalMetaData( const std::string& t,
		    const std::string& src ): MetaData(t) { _src = src; };
    std::string datatype() const { return "ExternalMetaData"; };
    std::string src() const { return _src; };
  private:
    std::string _src;
  };

  const std::string EMPTY_STRING = "";

  class AbstractTextMarkup:
    public AbstractElement,
    public AllowXlink
    {
      friend void static_init();
  protected:
      // DO NOT USE AbstractTextMarkup as a real node!!
  AbstractTextMarkup( const properties& props, Document *d=0 ):
      AbstractElement( props, d ){ classInit(); };
      explicit AbstractTextMarkup( Document *d=0 ):
    AbstractTextMarkup( PROPS, d ){};
  public:
    void setAttributes( KWargs& );
    KWargs collectAttributes() const;
    const FoliaElement* resolveid() const;
  protected:
    const std::string& get_delimiter( bool ) const { return EMPTY_STRING; };
    std::string idref;
  private:
    static properties PROPS;
  };

  class TextMarkupGap: public AbstractTextMarkup {
    friend void static_init();
  public:
    explicit TextMarkupGap( Document *d=0 ):
    AbstractTextMarkup( PROPS, d ) { classInit(); };
  TextMarkupGap(  const KWargs& a, Document *d=0 ):
    AbstractTextMarkup( PROPS, d ) { classInit(a); };
  private:
    static properties PROPS;
  };

  class TextMarkupString: public AbstractTextMarkup {
    friend void static_init();
  public:
  explicit TextMarkupString( Document *d=0 ):
    AbstractTextMarkup( PROPS, d ) { classInit(); };
  TextMarkupString( const KWargs& a, Document *d=0 ):
    AbstractTextMarkup( PROPS, d ) { classInit(a); };
  private:
    static properties PROPS;
  };

  class TextMarkupCorrection: public AbstractTextMarkup {
    friend void static_init();
  public:
    explicit TextMarkupCorrection( Document *d=0 ):
    AbstractTextMarkup( PROPS, d ){ classInit(); };
  TextMarkupCorrection( const KWargs& a, Document *d=0 ):
    AbstractTextMarkup( PROPS, d ) { classInit(a); };

    void setAttributes( KWargs& );
    KWargs collectAttributes() const;
  private:
    const UnicodeString private_text( const TextPolicy& ) const;
    static properties PROPS;
    std::string _original;
  };

  class TextMarkupError: public AbstractTextMarkup {
    friend void static_init();
  public:
    explicit TextMarkupError( Document *d=0 ):
    AbstractTextMarkup( PROPS, d ){ classInit(); };
  TextMarkupError( const KWargs& a, Document *d=0 ):
    AbstractTextMarkup( PROPS, d ) { classInit(a); };

  private:
    static properties PROPS;
  };

  class TextMarkupStyle: public AbstractTextMarkup {
    friend void static_init();
  public:
    explicit TextMarkupStyle( Document *d=0 ):
    AbstractTextMarkup( PROPS, d ){ classInit(); };
  TextMarkupStyle( const KWargs& a, Document *d=0 ):
    AbstractTextMarkup( PROPS, d ) { classInit(a); };

  private:
    static properties PROPS;
  };

  class Hyphbreak: public AbstractTextMarkup {
    friend void static_init();
  public:
    explicit Hyphbreak( Document *d=0 ):
    AbstractTextMarkup( PROPS, d ){ classInit(); };
  Hyphbreak( const KWargs& a, Document *d=0 ):
    AbstractTextMarkup( PROPS, d ) { classInit(a); };

  private:
    static properties PROPS;
  };

  class TextMarkupReference: public AbstractTextMarkup {
    friend void static_init();
  public:
    explicit TextMarkupReference( Document *d=0 ):
    AbstractTextMarkup( PROPS, d ){ classInit(); };
  TextMarkupReference( const KWargs& a, Document *d=0 ):
    AbstractTextMarkup( PROPS, d ) { classInit(a); };

    KWargs collectAttributes() const;
    void setAttributes( KWargs& );

  private:
    void init();
    static properties PROPS;
    std::string ref_id;
    std::string ref_type;
    std::string _type;
    std::string _format;
  };

  class TextMarkupHSpace: public AbstractTextMarkup {
    friend void static_init();
  public:
    explicit TextMarkupHSpace( Document *d=0 ):
    AbstractTextMarkup( PROPS, d ) { classInit(); };
  TextMarkupHSpace(  const KWargs& a, Document *d=0 ):
    AbstractTextMarkup( PROPS, d ) { classInit(a); };
  private:
    const UnicodeString private_text( const TextPolicy& ) const;
    static properties PROPS;
  };

  class TextMarkupWhitespace: public AbstractTextMarkup {
    friend void static_init();
  public:
    explicit TextMarkupWhitespace( Document *d=0 ):
    AbstractTextMarkup( PROPS, d ) { classInit(); };
  TextMarkupWhitespace(  const KWargs& a, Document *d=0 ):
    AbstractTextMarkup( PROPS, d ) { classInit(a); };
  private:
    static properties PROPS;
  };

  class TextMarkupLanguage: public AbstractTextMarkup {
    friend void static_init();
  public:
    explicit TextMarkupLanguage( Document *d=0 ):
    AbstractTextMarkup( PROPS, d ) { classInit(); };
  TextMarkupLanguage(  const KWargs& a, Document *d=0 ):
    AbstractTextMarkup( PROPS, d ) { classInit(a); };
  private:
    static properties PROPS;
  };


  class AbstractContentAnnotation: public AbstractElement {
    friend void static_init();
  protected:
  AbstractContentAnnotation( const properties& props, Document *d=0 ):
    AbstractElement( props, d ){ classInit(); };
    explicit AbstractContentAnnotation( Document *d = 0 ):
    AbstractContentAnnotation( PROPS, d ) {};
  AbstractContentAnnotation( const KWargs& a, Document *d = 0 ):
    AbstractElement( PROPS, d ) {  classInit( a ); }
  private:
    static properties PROPS;
  };


  class TextContent:
    public AbstractContentAnnotation,
    public AllowXlink
    {
      friend void static_init();
    public:
      explicit TextContent( Document *d = 0 ):
      AbstractContentAnnotation( PROPS, d ){ classInit(); }
    TextContent( const KWargs& a, Document *d=0 ):
      AbstractContentAnnotation( PROPS, d ){ classInit(a); }
      void setAttributes( KWargs& );
      KWargs collectAttributes() const;
      int offset() const { return _offset; };
      std::vector<FoliaElement*> find_replacables( FoliaElement * ) const;
      const std::string set_to_current() { // Don't use without thinking twice!
	std::string res = cls();
	update_cls( "current" );
	return res;
      }
      FoliaElement *postappend();
      FoliaElement *get_reference(bool trim_spaces=true) const;
      std::string ref() const { return _ref; };
    private:
      void init();
      FoliaElement *find_default_reference() const;
      void set_offset( int o ) const { _offset = o; }; // this MUST be const,
      // only used for 'fixing up' invalid offsets. keep it private!
      // therefore _offset  has to be mutable!
      static properties PROPS;
      mutable int _offset;
      std::string _ref;
    };

  class PhonContent: public AbstractContentAnnotation {
    friend void static_init();
  public:
  explicit PhonContent( Document *d = 0 ):
    AbstractContentAnnotation(PROPS,d){ classInit(); }
  PhonContent( const KWargs& a, Document *d = 0 ):
    AbstractContentAnnotation(PROPS,d){ classInit( a ); }
    void setAttributes( KWargs& );
    KWargs collectAttributes() const;
    const UnicodeString phon( const std::string& = "current",
			      TEXT_FLAGS = TEXT_FLAGS::NONE ) const;
    int offset() const { return _offset; };
    FoliaElement *postappend();
    FoliaElement *get_reference(bool trim_spaces=true) const;
    std::string ref() const { return _ref; };
  private:
    void init();
    FoliaElement *find_default_reference() const;
    void set_offset( int o ) const { _offset = o; }; // this MUST be const,
    // only used for 'fixing up' invalid offsets. keep it private!
    // therefore _offset  has to be mutable!
    static properties PROPS;
    mutable int _offset;
    std::string _ref;
  };

  class FoLiA: public AbstractElement {
    friend void static_init();
  public:
    explicit FoLiA( Document *d = 0 ):
    AbstractElement( PROPS,d) { classInit(); }
  FoLiA( const KWargs& a, Document *d = 0 ):
    AbstractElement( PROPS, d ) { classInit( a ); }

    FoliaElement* parseXml( const xmlNode * );
    void setAttributes( KWargs& );
  private:
    const UnicodeString private_text( const TextPolicy& ) const;
    static properties PROPS;
  };

  class DCOI: public AbstractElement {
    friend void static_init();
  public:
    explicit DCOI( Document *d = 0 ):
    AbstractElement( PROPS, d ) { classInit( ); }
  DCOI( const KWargs& a, Document *d = 0 ):
    AbstractElement( PROPS, d ) { classInit( a ); }

  private:
    static properties PROPS;
  };

  class Head: public AbstractStructureElement {
    friend void static_init();
  public:
  explicit Head( Document *d = 0 ):
    AbstractStructureElement( PROPS, d ) {  classInit(); }
  Head( const KWargs& a, Document *d = 0 ):
    AbstractStructureElement( PROPS, d ) {  classInit( a ); }

  private:
    static properties PROPS;
  };

  class TableHead: public AbstractStructureElement {
    friend void static_init();
  public:
  explicit TableHead( Document *d = 0 ):
    AbstractStructureElement( PROPS, d ) {  classInit( ); }
  TableHead( const KWargs& a, Document *d = 0 ):
    AbstractStructureElement( PROPS, d ) {  classInit( a ); }

  private:
    static properties PROPS;
  };

  class Table: public AbstractStructureElement {
    friend void static_init();
  public:
  explicit Table( Document *d = 0 ):
    AbstractStructureElement( PROPS, d ) {  classInit(); };
  Table( const KWargs& a, Document *d = 0  ):
    AbstractStructureElement( PROPS, d ) {  classInit( a ); };
  private:
    static properties PROPS;
  };

  class Row: public AbstractStructureElement {
    friend void static_init();
  public:
  explicit Row( Document *d = 0 ):
    AbstractStructureElement( PROPS, d ) {  classInit(); };
  Row( const KWargs& a, Document *d = 0 ):
    AbstractStructureElement( PROPS, d ) {  classInit( a ); };
  private:
    static properties PROPS;
  };

  class Cell: public AbstractStructureElement {
    friend void static_init();
  public:
    explicit Cell( Document *d = 0 ):
    AbstractStructureElement( PROPS, d ) {  classInit(); };
  Cell( const KWargs& a, Document *d ):
    AbstractStructureElement( PROPS, d ) {  classInit( a ); };
  private:
    static properties PROPS;
  };

  class Gap: public AbstractElement {
    friend void static_init();
  public:
    explicit Gap( Document *d = 0 ):
    AbstractElement( PROPS, d ) { classInit(); }
  Gap( const KWargs& a, Document *d = 0 ):
    AbstractElement( PROPS, d ) { classInit( a ); }

    const std::string content() const;
  private:
    static properties PROPS;
  };

  class Content: public AbstractElement {
    friend void static_init();
  public:
    explicit Content( Document *d = 0 ):
    AbstractElement( PROPS, d ) { classInit(); }
  Content( const KWargs& a, Document *d = 0 ):
    AbstractElement( PROPS, d ) { classInit( a ); }

    FoliaElement* parseXml( const xmlNode * );
    xmlNode *xml( bool, bool = false ) const;
    const std::string content() const { return value; };
    void setAttributes( KWargs& );
  private:
    static properties PROPS;
    std::string value;
  };

  class Metric: public AbstractElement {
    friend void static_init();
  public:
  explicit Metric( Document *d = 0 ):
    AbstractElement( PROPS, d ) { classInit(); }
  Metric( const KWargs& a, Document *d = 0 ):
    AbstractElement( PROPS, d ) { classInit( a ); }

  private:
    static properties PROPS;
  };

  class Division: public AbstractStructureElement {
    friend void static_init();
  public:
    explicit Division( Document *d=0 ):
    AbstractStructureElement( PROPS, d ) { classInit(); };
  Division( const KWargs& a, Document *d = 0 ):
    AbstractStructureElement( PROPS, d ) { classInit( a ); };
    Head *head() const;
  private:
    static properties PROPS;
  };

  class Linebreak:
    public AbstractStructureElement,
    public AllowXlink
    {
      friend void static_init();
    public:
      explicit Linebreak( Document *d=0 ):
      AbstractStructureElement( PROPS, d ){ classInit(); };
    Linebreak( const KWargs& a, Document *d = 0 ):
      AbstractStructureElement( PROPS, d ){ classInit( a ); };
      void setAttributes( KWargs& );
      KWargs collectAttributes() const;
    private:
      void init();
      const UnicodeString private_text( const TextPolicy& ) const {
	return "\n";
      }
      static properties PROPS;
      std::string _pagenr;
      std::string _linenr;
      bool _newpage;
    };

  class Whitespace: public AbstractStructureElement {
    friend void static_init();
  public:
    explicit Whitespace( Document *d=0 ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Whitespace( const KWargs& a, Document *d = 0 ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }


  private:
    const UnicodeString private_text( const TextPolicy& ) const {
      return "\n\n";
    }
    static properties PROPS;
  };

  class Word:
    public AbstractStructureElement,
    public AbstractWord
  {
    friend void static_init();
  public:
    explicit Word( Document *d = 0 ):
    Word(PROPS, d ) {};
  Word( const KWargs& a,  Document *d = 0 ):
    AbstractStructureElement( PROPS, d ) { classInit( a ); };

    Correction *split( FoliaElement *, FoliaElement *,
		       const std::string& = "" );
    Correction *incorrection() const;
    Word *previous() const;
    Word *next() const;
    std::vector<Word*> context( size_t,
				const std::string& ="" ) const;
    std::vector<Word*> leftcontext( size_t,
				    const std::string& = "" ) const;
    std::vector<Word*> rightcontext( size_t,
				     const std::string& ="" ) const;
    FoliaElement *append( FoliaElement * );
    const Word* resolveword( const std::string& ) const;
    void setAttributes( KWargs& );
    const std::string& get_delimiter( bool=false) const;
    MorphologyLayer *addMorphologyLayer( const KWargs& );
    MorphologyLayer *getMorphologyLayers( const std::string&,
					  std::vector<MorphologyLayer*>& ) const;
  protected:
  Word( const properties& p, Document *d=0 ):
    AbstractStructureElement( p, d ) { classInit(); }

  private:
    static properties PROPS;
  };

  class Hiddenword:
    public AbstractStructureElement,
    public AbstractWord
  {
    friend void static_init();
  public:
    explicit Hiddenword( Document *d=0 ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Hiddenword( const KWargs& a, Document *d = 0 ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class Part: public AbstractStructureElement {
    friend void static_init();
  public:
    explicit Part( Document *d=0 ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Part( const KWargs& a, Document *d = 0 ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class String:
    public AbstractElement,
    public AllowInlineAnnotation,
    public AllowGenerateID
    {
      friend void static_init();
  public:
      explicit String( Document *d=0 ):
      AbstractElement( PROPS, d ){ classInit(); }
    String( const KWargs& a, Document *d =0 ):
      AbstractElement( PROPS, d ){ classInit( a ); }
    private:
      static properties PROPS;
    };

  class PlaceHolder: public Word {
    friend void static_init();
  public:
    explicit PlaceHolder( Document *d=0 ):
    Word( PROPS, d ){ classInit(); }
  PlaceHolder( const KWargs& a, Document *d = 0 ):
    Word( PROPS, d ){ classInit( a ); }

    void setAttributes( KWargs& );
  private:
    static properties PROPS;
  };

  class Sentence:
    public AbstractStructureElement
    {
      friend void static_init();
    public:
      explicit Sentence( Document *d=0 ):
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
    explicit Speech( Document *d=0 ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Speech( const KWargs& a, Document *d = 0 ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class Text: public AbstractStructureElement {
    friend void static_init();
  public:
    explicit Text( Document *d=0 ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Text( const KWargs& a, Document *d = 0 ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }
  private:
    static properties PROPS;
  };

  class Utterance: public AbstractStructureElement {
    friend void static_init();
  public:
    explicit Utterance( Document *d=0 ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Utterance( const KWargs& a, Document *d = 0 ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class Event: public AbstractStructureElement {
    friend void static_init();
  public:
    explicit Event( Document *d=0 ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Event( const KWargs& a, Document *d = 0 ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class Caption: public AbstractStructureElement {
    friend void static_init();
  public:
    explicit Caption( Document *d=0 ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Caption( const KWargs& a, Document *d = 0 ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class Label: public AbstractStructureElement {
    friend void static_init();
  public:
   explicit Label( Document *d=0 ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Label( const KWargs& a, Document *d = 0 ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }
  private:
    static properties PROPS;
  };

  class ListItem: public AbstractStructureElement {
    friend void static_init();
  public:
    explicit ListItem( Document *d=0 ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  ListItem( const KWargs& a, Document *d = 0 ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class List: public AbstractStructureElement {
    friend void static_init();
  public:
    explicit List( Document *d=0 ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  List( const KWargs& a, Document *d = 0 ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class Figure: public AbstractStructureElement {
    friend void static_init();
  public:
    explicit Figure( Document *d=0 ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Figure( const KWargs& a, Document *d = 0 ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }
    const UnicodeString caption() const;
  private:
    static properties PROPS;
  };

  class Paragraph: public AbstractStructureElement {
    friend void static_init();
  public:
    explicit Paragraph( Document *d=0 ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Paragraph( const KWargs& a, Document *d = 0 ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class AbstractSubtokenAnnotation:
    public AbstractStructureElement
  {
    friend void static_init();
  public:
    explicit AbstractSubtokenAnnotation( Document *d = 0 ):
    AbstractSubtokenAnnotation( PROPS, d ) {  classInit(); }
  AbstractSubtokenAnnotation( const KWargs& a, Document *d = 0 ):
    AbstractStructureElement( PROPS, d ) {  classInit( a ); }

  protected:
  AbstractSubtokenAnnotation( const properties& props, Document *d=0 ):
    AbstractStructureElement( props, d ){ classInit(); };

  private:
    static properties PROPS;
  };

  class Alternative:
    public AbstractHigherOrderAnnotation,
    public AllowInlineAnnotation,
    public AllowGenerateID
    {
    friend void static_init();
  public:
    explicit Alternative( Document *d=0 ):
    AbstractHigherOrderAnnotation( PROPS, d ){ classInit(); }
  Alternative( const KWargs& a, Document *d = 0 ):
    AbstractHigherOrderAnnotation( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };


  class AlternativeLayers: public AbstractElement {
    friend void static_init();
  public:
    explicit AlternativeLayers( Document *d = 0 ):
    AbstractElement( PROPS, d ){ classInit(); }
  AlternativeLayers( const KWargs& a, Document *d = 0 ):
    AbstractElement( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class PosAnnotation: public AbstractInlineAnnotation {
    friend void static_init();
  public:
    explicit PosAnnotation( Document *d=0 ):
    AbstractInlineAnnotation( PROPS, d ){ classInit(); }
  PosAnnotation( const KWargs& a, Document *d = 0 ):
    AbstractInlineAnnotation( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class LemmaAnnotation: public AbstractInlineAnnotation {
    friend void static_init();
  public:
    explicit LemmaAnnotation( Document *d=0 ):
    AbstractInlineAnnotation( PROPS, d ){ classInit(); }
  LemmaAnnotation( const KWargs& a, Document *d = 0 ):
    AbstractInlineAnnotation( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class LangAnnotation: public AbstractInlineAnnotation {
    friend void static_init();
  public:
    explicit LangAnnotation( Document *d=0 ):
    AbstractInlineAnnotation( PROPS, d ){ classInit(); }
  LangAnnotation( const KWargs& a, Document *d = 0 ):
    AbstractInlineAnnotation( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class Phoneme:
    public AbstractSubtokenAnnotation,
    public AbstractWord
    {
      friend void static_init();
    public:
      explicit Phoneme( Document *d=0 ):
      AbstractSubtokenAnnotation( PROPS, d ){ classInit(); }
    Phoneme( const KWargs& a, Document *d = 0 ):
      AbstractSubtokenAnnotation( PROPS, d ){ classInit( a ); }

    private:
      static properties PROPS;
    };

  class DomainAnnotation: public AbstractInlineAnnotation {
    friend void static_init();
  public:
    explicit DomainAnnotation( Document *d=0 ):
    AbstractInlineAnnotation( PROPS, d ){ classInit(); }
  DomainAnnotation( const KWargs& a, Document *d = 0 ):
    AbstractInlineAnnotation( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class SenseAnnotation: public AbstractInlineAnnotation {
    friend void static_init();
  public:
    explicit SenseAnnotation( Document *d=0 ):
    AbstractInlineAnnotation( PROPS, d ){ classInit(); }
  SenseAnnotation( const KWargs& a, Document *d = 0 ):
    AbstractInlineAnnotation( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class SubjectivityAnnotation: public AbstractInlineAnnotation {
    friend void static_init();
  public:
    explicit SubjectivityAnnotation( Document *d=0 ):
    AbstractInlineAnnotation( PROPS, d ){ classInit(); }
  SubjectivityAnnotation( const KWargs& a, Document *d = 0 ):
    AbstractInlineAnnotation( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class Quote: public AbstractStructureElement {
    friend void static_init();
  public:
    explicit Quote( Document *d=0 ):
    AbstractStructureElement( PROPS, d ){ classInit(); };
  Quote( const KWargs& a, Document *d = 0 ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); };
    std::vector<Word*> wordParts() const;
    const std::string& get_delimiter( bool=false) const;
  private:
    static properties PROPS;
  };

  class Feature: public AbstractElement {
    friend void static_init();
  public:
    explicit Feature( Document *d = 0 ):
    Feature( PROPS, d ){ classInit(); }
  Feature( const KWargs& a, Document *d = 0 ):
    AbstractElement( PROPS, d ){ classInit( a ); }

    void setAttributes( KWargs& );
    KWargs collectAttributes() const;
    const std::string subset() const { return _subset; };

  protected:
  Feature( const properties&p, Document *d = 0 ):
    AbstractElement( p, d ){ classInit(); }

  private:
    std::string _subset;
    static properties PROPS;
  };

  class BegindatetimeFeature: public Feature {
    friend void static_init();
  public:
    explicit BegindatetimeFeature( Document *d=0 ):
    Feature( PROPS, d ){ classInit(); }
  BegindatetimeFeature( const KWargs& a, Document *d = 0 ):
    Feature( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class EnddatetimeFeature: public Feature {
    friend void static_init();
  public:
  explicit EnddatetimeFeature( Document *d=0 ):
    Feature( PROPS, d ){ classInit(); }
  EnddatetimeFeature( const KWargs& a, Document *d = 0 ):
    Feature( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class SynsetFeature: public Feature {
    friend void static_init();
  public:
  explicit SynsetFeature( Document *d=0 ):
    Feature( PROPS, d ){ classInit(); }
  SynsetFeature( const KWargs& a, Document *d = 0 ):
    Feature( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class ActorFeature: public Feature {
    friend void static_init();
  public:
    explicit ActorFeature( Document *d=0 ):
    Feature( PROPS, d ){ classInit(); }
  ActorFeature( const KWargs& a, Document *d = 0 ):
    Feature( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class PolarityFeature: public Feature {
    friend void static_init();
  public:
    explicit PolarityFeature( Document *d=0 ):
    Feature( PROPS, d ){ classInit(); }
  PolarityFeature( const KWargs& a, Document *d = 0 ):
    Feature( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class StrengthFeature: public Feature {
    friend void static_init();
  public:
    explicit StrengthFeature( Document *d=0 ):
    Feature( PROPS, d ){ classInit(); }
  StrengthFeature( const KWargs& a, Document *d = 0 ):
    Feature( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };


  class HeadFeature: public Feature {
    friend void static_init();
  public:
    explicit HeadFeature( Document *d=0 ):
    Feature( PROPS, d ){ classInit(); }
  HeadFeature( const KWargs& a, Document *d = 0 ):
    Feature( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class ValueFeature: public Feature {
    friend void static_init();
  public:
    explicit ValueFeature( Document *d=0 ):
    Feature( PROPS, d ){ classInit(); }
  ValueFeature( const KWargs& a, Document *d = 0 ):
    Feature( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class FunctionFeature: public Feature {
    friend void static_init();
  public:
    explicit FunctionFeature( Document *d=0 ):
    Feature( PROPS, d ){ classInit(); }
  FunctionFeature( const KWargs& a, Document *d = 0 ):
    Feature( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class TimeFeature: public Feature {
    friend void static_init();
  public:
    explicit TimeFeature( Document *d=0 ):
    Feature( PROPS, d ){ classInit(); }
  TimeFeature( const KWargs& a, Document *d = 0 ):
    Feature( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class LevelFeature: public Feature {
    friend void static_init();
  public:
    explicit LevelFeature( Document *d=0 ):
    Feature( PROPS, d ){ classInit(); }
  LevelFeature( const KWargs& a, Document *d = 0 ):
    Feature( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class ModalityFeature: public Feature {
    friend void static_init();
  public:
    explicit ModalityFeature( Document *d=0 ):
    Feature( PROPS, d ){ classInit(); }
  ModalityFeature( const KWargs& a, Document *d = 0 ):
    Feature( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class StyleFeature: public Feature {
    friend void static_init();
  public:
    explicit StyleFeature( Document *d=0 ):
    Feature( PROPS, d ){ classInit(); }
  StyleFeature( const KWargs& a, Document *d = 0 ):
    Feature( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class FontFeature: public Feature {
    friend void static_init();
  public:
    explicit FontFeature( Document *d=0 ):
    Feature( PROPS, d ){ classInit(); }
  FontFeature( const KWargs& a, Document *d = 0 ):
    Feature( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class SizeFeature: public Feature {
    friend void static_init();
  public:
    explicit SizeFeature( Document *d=0 ):
    Feature( PROPS, d ){ classInit(); }
  SizeFeature( const KWargs& a, Document *d = 0 ):
    Feature( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class WordReference: public AbstractElement {
    friend void static_init();
  public:
    explicit WordReference( Document *d = 0 ):
    AbstractElement( PROPS, d ){ classInit(); }
  WordReference( const KWargs& a, Document *d = 0 ):
    AbstractElement( PROPS, d ){ classInit( a ); }
  private:
    static properties PROPS;
    FoliaElement* parseXml( const xmlNode *node );
  };

  class Relation:
    public AbstractHigherOrderAnnotation,
    public AllowXlink
    {
      friend void static_init();
    public:
      explicit Relation( Document *d = 0 ):
      AbstractHigherOrderAnnotation( PROPS, d ){ classInit(); }
    Relation( const KWargs& a, Document *d = 0 ):
      AbstractHigherOrderAnnotation( PROPS, d ){ classInit( a ); }

      std::vector<FoliaElement *>resolve() const;
      void setAttributes( KWargs& );
      KWargs collectAttributes() const;
    private:
      void init();
      static properties PROPS;
      std::string _format;
    };

  class LinkReference: public AbstractElement {
    friend void static_init();
    friend class Relation;
  public:
    explicit LinkReference( Document *d = 0 ):
    AbstractElement( PROPS, d ){ classInit(); }
  LinkReference( const KWargs& a, Document *d = 0 ):
    AbstractElement( PROPS, d ){ classInit( a ); }

    KWargs collectAttributes() const;
    void setAttributes( KWargs& );
    const std::string refid() const { return ref_id; };
    const std::string type() const { return ref_type; };
    const std::string t() const { return _t; };

  private:
    static properties PROPS;
    FoliaElement* parseXml( const xmlNode *node );
    FoliaElement *resolve_element( const Relation *ref ) const;
    std::string ref_id;
    std::string ref_type;
    std::string _t;
  };

  class SyntacticUnit: public AbstractSpanAnnotation {
    friend void static_init();
  public:
    explicit SyntacticUnit( Document *d = 0 ):
    AbstractSpanAnnotation( PROPS, d ){ classInit(); }
  SyntacticUnit( const KWargs& a, Document *d = 0 ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class Chunk: public AbstractSpanAnnotation {
    friend void static_init();
  public:
    explicit Chunk( Document *d=0 ):
    AbstractSpanAnnotation( PROPS, d ){ classInit(); }
  Chunk( const KWargs& a, Document *d = 0 ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class Entity: public AbstractSpanAnnotation {
    friend void static_init();
  public:
    explicit Entity( Document *d=0 ):
    AbstractSpanAnnotation( PROPS, d ){ classInit(); }
  Entity( const KWargs& a, Document *d = 0 ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class AbstractSpanRole: public AbstractSpanAnnotation {
    friend void static_init();
  public:
    explicit AbstractSpanRole( Document *d=0 ):
    AbstractSpanRole( PROPS, d ){};
  protected:
  AbstractSpanRole( const properties& props, Document *d=0 ):
    AbstractSpanAnnotation( props, d ){ classInit(); }

  private:
    static properties PROPS;
  };

  class Headspan: public AbstractSpanRole {
    friend void static_init();
  public:
    explicit Headspan( Document *d=0 ):
    AbstractSpanRole( PROPS, d ){ classInit(); }
  Headspan( const KWargs& a, Document *d = 0 ):
    AbstractSpanRole( PROPS, d ){ classInit( a ); }
  private:
    static properties PROPS;
  };

  class DependencyDependent: public AbstractSpanRole {
    friend void static_init();
  public:
    explicit DependencyDependent( Document *d=0 ):
    AbstractSpanRole( PROPS, d ){ classInit(); }
  DependencyDependent( const KWargs& a, Document *d = 0 ):
    AbstractSpanRole( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class Source: public AbstractSpanRole {
    friend void static_init();
  public:
    explicit Source( Document *d=0 ):
    AbstractSpanRole( PROPS, d ){ classInit(); }
  Source( const KWargs& a, Document *d = 0 ):
    AbstractSpanRole( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class Target: public AbstractSpanRole {
    friend void static_init();
  public:
    explicit Target( Document *d=0 ):
    AbstractSpanRole( PROPS, d ){ classInit(); }
  Target( const KWargs& a, Document *d = 0 ):
    AbstractSpanRole( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class Scope: public AbstractSpanRole {
    friend void static_init();
  public:
    explicit Scope( Document *d=0 ):
    AbstractSpanRole( PROPS, d ){ classInit(); }
  Scope( const KWargs& a, Document *d = 0 ):
    AbstractSpanRole( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class Cue: public AbstractSpanRole {
    friend void static_init();
  public:
    explicit Cue( Document *d=0 ):
    AbstractSpanRole( PROPS, d ){ classInit(); }
  Cue( const KWargs& a, Document *d = 0 ):
    AbstractSpanRole( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class StatementRelation: public AbstractSpanRole {
    friend void static_init();
  public:
    explicit StatementRelation( Document *d=0 ):
    AbstractSpanRole( PROPS, d ){ classInit(); }
  StatementRelation( const KWargs& a, Document *d = 0 ):
    AbstractSpanRole( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class Dependency: public AbstractSpanAnnotation {
    friend void static_init();
  public:
    explicit Dependency( Document *d=0 ):
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
    explicit CoreferenceLink( Document *d=0 ):
    AbstractSpanRole( PROPS, d ){ classInit(); }
  CoreferenceLink( const KWargs& a, Document *d = 0 ):
    AbstractSpanRole( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class CoreferenceChain: public AbstractSpanAnnotation {
    friend void static_init();
  public:
    explicit CoreferenceChain( Document *d=0 ):
    AbstractSpanAnnotation( PROPS, d ){ classInit(); }
  CoreferenceChain( const KWargs& a, Document *d = 0 ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class SemanticRole: public AbstractSpanAnnotation {
    friend void static_init();
  public:
    explicit SemanticRole( Document *d=0 ):
    AbstractSpanAnnotation( PROPS, d ){ classInit(); }
  SemanticRole( const KWargs& a, Document *d = 0 ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class Predicate: public AbstractSpanAnnotation {
    friend void static_init();
  public:
    explicit Predicate( Document *d=0 ):
    AbstractSpanAnnotation( PROPS, d ){ classInit(); }
  Predicate( const KWargs& a, Document *d = 0 ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class Sentiment: public AbstractSpanAnnotation {
    friend void static_init();
  public:
    explicit Sentiment( Document *d=0 ):
    AbstractSpanAnnotation( PROPS, d ){ classInit(); }
  Sentiment( const KWargs& a, Document *d = 0 ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class Modality: public AbstractSpanAnnotation {
    friend void static_init();
  public:
    explicit Modality( Document *d=0 ):
    AbstractSpanAnnotation( PROPS, d ){ classInit(); }
  Modality( const KWargs& a, Document *d = 0 ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class Statement: public AbstractSpanAnnotation {
    friend void static_init();
  public:
    explicit Statement( Document *d=0 ):
    AbstractSpanAnnotation( PROPS, d ){ classInit(); }
  Statement( const KWargs& a, Document *d = 0 ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class Observation: public AbstractSpanAnnotation {
    friend void static_init();
  public:
    explicit Observation( Document *d=0 ):
    AbstractSpanAnnotation( PROPS, d ){ classInit(); }
  Observation( const KWargs& a, Document *d = 0 ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };


  class AbstractAnnotationLayer:
    public AbstractElement,
    public AllowGenerateID,
    public AllowInlineAnnotation
    {
      friend void static_init();
    protected:
      // DO NOT USE AbstractAnnotationLayer as a real node!!
      explicit AbstractAnnotationLayer( Document *d = 0 ):
      AbstractAnnotationLayer( PROPS, d ){};
    AbstractAnnotationLayer( const properties& props, Document *d = 0 ):
      AbstractElement( props, d ) { classInit(); };
    AbstractAnnotationLayer( const properties& props, const KWargs& a, Document *d = 0 ):
      AbstractElement( props, d ) { classInit( a ); };
  public:
    AbstractSpanAnnotation *findspan( const std::vector<FoliaElement*>& ) const;
    FoliaElement *append( FoliaElement * );
    KWargs collectAttributes() const;
  private:
    static properties PROPS;
    void assignset( FoliaElement * );
  };

  class AbstractCorrectionChild: public AbstractElement {
    friend void static_init();
  protected:
    // DO NOT USE AbstractCorrectionChild as a real node!!
  AbstractCorrectionChild( const properties& props, Document *d=0 ):
    AbstractElement( props, d ){ classInit(); };
    explicit AbstractCorrectionChild( Document *d=0 ):
    AbstractCorrectionChild( PROPS, d ){};
  private:
    static properties PROPS;
  };

  class New: public AbstractCorrectionChild {
    friend void static_init();
  public:
  explicit New( Document *d=0 ):
    AbstractCorrectionChild( PROPS, d ) { classInit(); }
  New( const KWargs& a, Document *d = 0 ):
    AbstractCorrectionChild( PROPS, d ) { classInit( a ); }
    Correction *correct( const std::vector<FoliaElement*>&,
			 const std::vector<FoliaElement*>&,
			 const std::vector<FoliaElement*>&,
			 const std::vector<FoliaElement*>&,
			 const KWargs& );
    Correction *correct( const std::string& = "" );
  private:
    static properties PROPS;
  };

  class Current: public AbstractCorrectionChild {
    friend void static_init();
  public:
  explicit Current( Document *d=0 ):
    AbstractCorrectionChild( PROPS, d ) { classInit(); }
  Current( const KWargs& a, Document *d = 0 ):
    AbstractCorrectionChild( PROPS, d ) { classInit( a ); }

  private:
    static properties PROPS;
  };

  class Original: public AbstractCorrectionChild {
    friend void static_init();
  public:
  explicit Original( Document *d=0 ):
    AbstractCorrectionChild( PROPS, d ) { classInit(); }
  Original( const KWargs& a, Document *d = 0 ):
    AbstractCorrectionChild( PROPS, d ) { classInit( a ); }

  private:
    static properties PROPS;
  };

  class Suggestion: public AbstractCorrectionChild {
    friend void static_init();
  public:
  explicit Suggestion( Document *d=0 ):
    AbstractCorrectionChild( PROPS, d ) { classInit(); }
  Suggestion( const KWargs& a, Document *d = 0 ):
    AbstractCorrectionChild( PROPS, d ) { classInit( a ); }
    void setAttributes( KWargs& );
    KWargs collectAttributes() const;
  private:
    static properties PROPS;
    std::string _split;
    std::string _merge;
  };

  class Description: public AbstractElement {
    friend void static_init();
  public:
  explicit Description( Document *d=0 ):
    AbstractElement( PROPS, d ) { classInit(); }
  Description( const KWargs& a, Document *d =0 ):
    AbstractElement( PROPS, d ) { classInit( a ); }

    const std::string description() const { return _value; };
    void setAttributes( KWargs& );
    FoliaElement* parseXml( const xmlNode * );
    xmlNode *xml( bool, bool=false ) const;

  private:
    static properties PROPS;
    std::string _value;
  };

  class Comment: public AbstractElement {
    friend void static_init();
  public:
  explicit Comment( Document *d=0 ):
    AbstractElement( PROPS, d ) { classInit(); }
  Comment( const KWargs& a, Document *d =0 ):
    AbstractElement( PROPS, d ) { classInit( a ); }

    const std::string comment() const { return _value; };
    void setAttributes( KWargs& );
    FoliaElement* parseXml( const xmlNode * );
    xmlNode *xml( bool, bool=false ) const;

  private:
    static properties PROPS;
    std::string _value;
  };

  class XmlComment: public AbstractElement {
    friend void static_init();
  public:
  explicit XmlComment( Document *d=0 ):
    AbstractElement( PROPS, d ) { classInit(); }
  XmlComment( const KWargs& a, Document *d = 0 ):
    AbstractElement( PROPS, d ) { classInit( a ); }

    FoliaElement* parseXml( const xmlNode * );
    xmlNode *xml( bool, bool=false ) const;

  private:
    const UnicodeString private_text( const TextPolicy& ) const {
      return "";
    }
    static properties PROPS;
    std::string _value;
  };

  class XmlText: public AbstractElement {
    friend void static_init();
  public:
  explicit XmlText( Document *d = 0 ):
    AbstractElement( PROPS, d ) { classInit(); }
  XmlText( const KWargs& a, Document *d = 0 ):
    AbstractElement(PROPS, d ) { classInit( a ); }

    FoliaElement* parseXml( const xmlNode * );
    xmlNode *xml( bool, bool=false ) const;
    bool setvalue( const std::string& );
    const std::string& get_delimiter( bool ) const { return EMPTY_STRING; };
  private:
    const UnicodeString private_text( const TextPolicy& ) const;
    static properties PROPS;
    std::string _value; //UTF8 value
  };

  class External: public AbstractElement {
    friend void static_init();
  public:
  explicit External( Document *d = 0 ):
    AbstractElement( PROPS, d ) { classInit(); }
  External( const KWargs& a, Document *d = 0 ):
    AbstractElement( PROPS, d ) { classInit( a ); }

    FoliaElement* parseXml( const xmlNode * );
    void resolve_external();
  private:
    static properties PROPS;
  };

  class Note: public AbstractStructureElement {
    friend void static_init();
  public:
  explicit Note( Document *d=0 ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Note( const KWargs& a, Document *d = 0 ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

    void setAttributes( KWargs& );
  private:
    static properties PROPS;
    std::string ref_id;
  };

  class Definition: public AbstractStructureElement {
    friend void static_init();
  public:
  explicit Definition( Document *d=0 ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Definition( const KWargs& a, Document *d = 0 ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class Term: public AbstractStructureElement {
    friend void static_init();
  public:
  explicit Term( Document *d=0 ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Term( const KWargs& a, Document *d = 0 ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class Example: public AbstractStructureElement {
    friend void static_init();
  public:
  explicit Example( Document *d=0 ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Example( const KWargs& a, Document *d = 0 ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class Entry: public AbstractStructureElement {
    friend void static_init();
  public:
  explicit Entry( Document *d=0 ):
    AbstractStructureElement( PROPS, d ){ classInit(); }
  Entry( const KWargs& a, Document *d = 0 ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class Reference:
    public AbstractStructureElement,
    public AllowXlink
    {
      friend void static_init();
      friend class Note;
    public:
      explicit Reference( Document *d=0 ):
      AbstractStructureElement( PROPS, d ){ classInit(); }
    Reference( const KWargs& a, Document *d = 0 ):
      AbstractStructureElement( PROPS, d ){ classInit( a ); }

      KWargs collectAttributes() const;
      void setAttributes( KWargs& );
    private:
      void init();
      static properties PROPS;
      std::string ref_id;
      std::string ref_type;
      std::string _format;
    };


  class Correction: public AbstractInlineAnnotation {
    friend void static_init();
  public:
  explicit Correction( Document *d=0 ):
    AbstractInlineAnnotation( PROPS, d ){ classInit(); }
  Correction( const KWargs& a, Document *d = 0 ):
    AbstractInlineAnnotation( PROPS, d ){ classInit( a ); }

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
    const TextContent *text_content( const TextPolicy& ) const;
    const TextContent *text_content( const std::string& = "current" ) const;
    const PhonContent *phon_content( const std::string& = "current",
				     bool = false ) const;
    const std::string& get_delimiter( bool=false) const;
    Correction *correct( const std::vector<FoliaElement*>&,
			 const std::vector<FoliaElement*>&,
			 const std::vector<FoliaElement*>&,
			 const std::vector<FoliaElement*>&,
			 const KWargs& );
    Correction *correct( const std::string& = "" );
  private:
    const UnicodeString private_text( const TextPolicy& ) const;
    static properties PROPS;
  };

  class ErrorDetection: public AbstractInlineAnnotation  {
    friend void static_init();
  public:
  explicit ErrorDetection( Document *d=0 ):
    AbstractInlineAnnotation( PROPS, d ){ classInit(); }
  ErrorDetection( const KWargs& a, Document *d = 0 ):
    AbstractInlineAnnotation( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class TimeSegment: public AbstractSpanAnnotation {
    friend void static_init();
  public:
  explicit TimeSegment( Document *d=0 ):
    AbstractSpanAnnotation( PROPS, d ){ classInit(); }
  TimeSegment( const KWargs& a, Document *d = 0 ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class Morpheme:
    public AbstractSubtokenAnnotation,
    public AbstractWord
    {
      friend void static_init();
    public:
      explicit Morpheme( Document *d=0 ):
      AbstractSubtokenAnnotation( PROPS, d ){ classInit(); }
    Morpheme( const KWargs& a, Document *d = 0 ):
      AbstractSubtokenAnnotation( PROPS, d ){ classInit( a ); }

    private:
      static properties PROPS;
    };

  class SyntaxLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
    explicit SyntaxLayer( Document *d=0 ):
    AbstractAnnotationLayer( PROPS, d ){ classInit(); }
  SyntaxLayer( const KWargs& a, Document *d = 0 ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class ChunkingLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
    explicit ChunkingLayer( Document *d=0 ):
    AbstractAnnotationLayer( PROPS, d ){ classInit(); }
  ChunkingLayer( const KWargs& a, Document *d = 0 ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class EntitiesLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
    explicit EntitiesLayer( Document *d=0 ):
    AbstractAnnotationLayer( PROPS, d ){ classInit(); }
  EntitiesLayer( const KWargs& a, Document *d = 0 ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class TimingLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
    explicit TimingLayer( Document *d=0 ):
    AbstractAnnotationLayer( PROPS, d ){ classInit(); }
  TimingLayer( const KWargs& a, Document *d = 0 ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class MorphologyLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
    explicit MorphologyLayer( Document *d=0 ):
    AbstractAnnotationLayer( PROPS, d ){ classInit(); }
  MorphologyLayer( const KWargs& a, Document *d = 0 ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class PhonologyLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
    explicit PhonologyLayer( Document *d=0 ):
    AbstractAnnotationLayer( PROPS, d ){ classInit(); }
  PhonologyLayer( const KWargs& a, Document *d = 0 ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class DependenciesLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
    explicit DependenciesLayer( Document *d=0 ):
    AbstractAnnotationLayer( PROPS, d ){ classInit(); }
  DependenciesLayer( const KWargs& a, Document *d = 0 ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class CoreferenceLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
    explicit CoreferenceLayer( Document *d=0 ):
    AbstractAnnotationLayer( PROPS, d ){ classInit(); }
  CoreferenceLayer( const KWargs& a, Document *d = 0 ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( a ); }

  private:
    static properties PROPS;
  };

  class SemanticRolesLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
    explicit SemanticRolesLayer( Document *d=0 ):
    AbstractAnnotationLayer( PROPS, d ){ classInit(); }
  SemanticRolesLayer( const KWargs& a, Document *d = 0 ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( a ); }
  private:
    static properties PROPS;
  };

  class StatementLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
    explicit StatementLayer( Document *d=0 ):
    AbstractAnnotationLayer( PROPS, d ){ classInit(); }
  StatementLayer( const KWargs& a, Document *d = 0 ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( a ); }
  private:
    static properties PROPS;
  };

  class SentimentLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
    explicit SentimentLayer( Document *d=0 ):
    AbstractAnnotationLayer( PROPS, d ){ classInit(); }
  SentimentLayer( const KWargs& a, Document *d = 0 ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( a ); }
  private:
    static properties PROPS;
  };

  class ModalitiesLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
    explicit ModalitiesLayer( Document *d=0 ):
    AbstractAnnotationLayer( PROPS, d ){ classInit(); }
  ModalitiesLayer( const KWargs& a, Document *d = 0 ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( a ); }
  private:
    static properties PROPS;
  };

  class ObservationLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
    explicit ObservationLayer( Document *d=0  ):
    AbstractAnnotationLayer( PROPS, d ){ classInit(); }
  ObservationLayer( const KWargs& a, Document *d = 0 ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( a ); }
  private:
    static properties PROPS;
  };

  class SpanRelationLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
    explicit SpanRelationLayer( Document *d=0  ):
    AbstractAnnotationLayer( PROPS, d ){ classInit(); }
  SpanRelationLayer( const KWargs& a, Document *d = 0 ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( a ); }
  private:
    static properties PROPS;
  };

  std::string VersionName();
  std::string Version();

  UnicodeString trim_space( const UnicodeString& in );
  UnicodeString postprocess_spaces( const UnicodeString& in );

} // namespace folia

#endif
