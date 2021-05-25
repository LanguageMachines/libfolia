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
#include "libfolia/folia_metadata.h"
#include "libfolia/folia_textpolicy.h"

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


#define NOT_IMPLEMENTED {						\
    throw NotImplementedError( xmltag() + "::" + std::string(__func__) ); \
  }

  class FoliaElement {
    friend std::ostream& operator<<( std::ostream&, const FoliaElement& );
    friend std::ostream& operator<<( std::ostream&, const FoliaElement* );
    friend bool operator==( const FoliaElement&, const FoliaElement& );
    friend void destroy( FoliaElement * );
  protected:
    virtual ~FoliaElement(){};
  public:
    virtual void destroy() = 0;
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
      return element_id() == F::PROPS.ELEMENT_ID;
    }

    template <typename T>
      inline T *add_child( KWargs& args ){
      /// create a new FoliaElement of type T as child of this
      /*!
	\param args an attribute-value list of arguments to use
	\return a new FoliaElement
	may throw if a Document is required, but not available
      */
      return new T( args, this );
    }

    template <typename T>
      inline T *add_child(){
      /// create a new FoliaElement of type T as child of this
      /*!
	\return a new FoliaElement
	may throw if a Document is required, but not available
      */
      return new T( this );
    }

    template <typename T>
      inline T *add_child( const std::string& txt ){
      /// create a new XmlText as child of 'this'
      /*!
	\param txt an value to be assigned as a "text" attribute
	\return a new FoliaElement
	this will not complie for any class that has NO IMPLEMENTATION for
	setvalue(). (which ar most classes)
      */
      T *result = new T(this);
      result->setvalue( txt );
      return result;
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
      std::vector<FoliaElement*> tmp = select( F::PROPS.ELEMENT_ID,
					       st,
					       exclude,
					       (recurse?SELECT_FLAGS::RECURSE : SELECT_FLAGS::LOCAL) );
      std::vector<F*> res;
      for ( size_t i = 0; i < tmp.size(); ++i ){
	res.push_back( dynamic_cast<F*>( tmp[i]) );
      }
      return res;
    }

    template <typename F>
      std::vector<F*> select( const std::string& st,
			      bool recurse = true ) const {
      std::vector<FoliaElement*> tmp = select( F::PROPS.ELEMENT_ID,
					       st,
					       (recurse?SELECT_FLAGS::RECURSE : SELECT_FLAGS::LOCAL) );
      std::vector<F*> res;
      for ( size_t i = 0; i < tmp.size(); ++i ){
	res.push_back( dynamic_cast<F*>( tmp[i]) );
      }
      return res;
    }

    template <typename F>
      std::vector<F*> select( const char* st,
			      bool recurse = true ) const {
      std::vector<FoliaElement*> tmp = select( F::PROPS.ELEMENT_ID,
					       std::string(st),
					       (recurse?SELECT_FLAGS::RECURSE : SELECT_FLAGS::LOCAL) );
      std::vector<F*> res;
      for ( size_t i = 0; i < tmp.size(); ++i ){
	res.push_back( dynamic_cast<F*>( tmp[i]) );
      }
      return res;
    }

    template <typename F>
      std::vector<F*> select( const std::set<ElementType>& exclude,
			      bool recurse = true ) const {
      std::vector<FoliaElement*> tmp = select( F::PROPS.ELEMENT_ID,
					       exclude,
					       (recurse?SELECT_FLAGS::RECURSE : SELECT_FLAGS::LOCAL) );
      std::vector<F*> res;
      for ( size_t i = 0; i < tmp.size(); ++i ){
	res.push_back( dynamic_cast<F*>( tmp[i]) );
      }
      return res;
    }

    template <typename F>
      std::vector<F*> select( bool recurse = true ) const {
      std::vector<FoliaElement*> tmp = select( F::PROPS.ELEMENT_ID,
					       (recurse?SELECT_FLAGS::RECURSE : SELECT_FLAGS::LOCAL) );
      std::vector<F*> res;
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
	if ( res ){
	  res->destroy();
	}
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
      return findspans( F::PROPS.ELEMENT_ID, st );
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
    virtual const UnicodeString phon( const TextPolicy& ) const = 0;
    virtual const UnicodeString phon( const std::string&,
				      TEXT_FLAGS = TEXT_FLAGS::NONE ) const = 0;
    virtual const UnicodeString phon( TEXT_FLAGS = TEXT_FLAGS::NONE ) const = 0;
    virtual bool printable() const = 0;
    virtual bool speakable() const = 0;
    virtual bool referable() const = 0;
    virtual bool is_textcontainer() const = 0;
    virtual bool is_phoncontainer() const = 0;
    virtual const std::string& text_delimiter() const = 0;
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
    virtual const PhonContent *phon_content( const TextPolicy& ) const = 0;
    virtual const PhonContent *phon_content( const std::string& = "current" ) const = 0;

    // properties
    virtual const std::string& get_delimiter( bool=false ) const = 0;
    virtual void setDateTime( const std::string& ) = 0;
    virtual const std::string getDateTime() const = 0;
    virtual const std::string pos( const std::string& = "" ) const NOT_IMPLEMENTED;
    virtual const std::string lemma( const std::string& = "" ) const NOT_IMPLEMENTED;
    virtual const std::string cls() const = 0;
    virtual void set_cls( const std::string& ) = 0;
    virtual const std::string sett() const = 0;
    virtual void set_set( const std::string& ) = 0;
    virtual const std::string n() const = 0;
    virtual void set_n( const std::string& ) = 0;
    virtual const std::string tag() const = 0;
    virtual const std::string set_tag( const std::string& ) = 0;
    virtual const std::string id() const = 0;
    virtual const std::string begintime() const = 0;
    virtual void set_begintime( const std::string& ) = 0;
    virtual const std::string endtime() const = 0;
    virtual void set_endtime( const std::string& ) = 0;
    virtual const std::string speech_src() const = 0;
    virtual void set_speech_src( const std::string& ) = 0;
    virtual const std::string speech_speaker() const = 0;
    virtual void set_speech_speaker( const std::string& ) = 0;
    virtual const std::string language( const std::string& = "" ) const = 0;
    virtual const std::string set_to_current() NOT_IMPLEMENTED;
    virtual double confidence() const = 0;
    virtual void set_confidence( double ) = 0;
    virtual void confidence( double ) = 0; // deprecated
    virtual bool space() const = 0;
    virtual bool set_space( bool ) = 0;
    virtual SPACE_FLAGS spaces_flag() const = 0;
    virtual void set_spaces_flag( SPACE_FLAGS ) = 0;
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
    virtual const UnicodeString deepphon( const TextPolicy& ) const NOT_IMPLEMENTED;


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
    friend void destroy( FoliaElement * );
  private:
    //Constructor
    AbstractElement( const AbstractElement& ); // inhibit copies
    AbstractElement& operator=( const AbstractElement& ); // inhibit copies
  protected:
    AbstractElement( const properties& p, Document* = 0 );
    AbstractElement( const properties& p, FoliaElement * );
    virtual ~AbstractElement();
  public:
    void destroy();
    void classInit();
    void classInit( const KWargs& );

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

    const UnicodeString private_text( const TextPolicy& ) const;
    const UnicodeString text( const TextPolicy & ) const;
    const UnicodeString text( const std::string&,
			      TEXT_FLAGS = TEXT_FLAGS::NONE ) const;
    const UnicodeString text( TEXT_FLAGS flags = TEXT_FLAGS::NONE ) const {
      return text( "current", flags );
    }

    const UnicodeString phon( const TextPolicy& ) const;
    const UnicodeString phon( const std::string&,
			      TEXT_FLAGS = TEXT_FLAGS::NONE ) const;
    const UnicodeString phon( TEXT_FLAGS flags = TEXT_FLAGS::NONE ) const {
      return phon( "current", flags );
    }

    const UnicodeString deeptext( const TextPolicy& ) const;
    const UnicodeString deepphon( const TextPolicy& ) const;

    // Word
    const Word* resolveword( const std::string& ) const { return 0; };
    Word *addWord( const KWargs& );
    Word *addWord( const std::string& ="" );
    // TextContent
    const TextContent *text_content( const TextPolicy& ) const;
    const TextContent *text_content( const std::string& = "current" ) const;
    // PhonContent
    const PhonContent *phon_content( const TextPolicy& tp ) const;
    const PhonContent *phon_content( const std::string& = "current" ) const;

    // properties
    const std::string& get_delimiter( bool=false ) const;

    // attributes
    const std::string cls() const { return _class; };
    void set_cls( const std::string& cls ){ _class = cls; };
    void update_cls( const std::string& c ) { set_cls( c ); } // deprecated

    const std::string sett() const { return _set; };
    void set_set( const std::string& st ){ _set = st; };

    const std::string tag() const { return _tags; };
    const std::string set_tag( const std::string&  );
    const std::string settag( const std::string& t ){
      return set_tag(t); };                              //deprecated

    const std::string n() const { return _n; };
    void set_n( const std::string& n ){ _n = n; };

    const std::string id() const { return _id; };

    const std::string begintime() const { return _begintime; };
    void set_begintime( const std::string& bt ){ _begintime = bt; };

    const std::string endtime() const { return _endtime; };
    void set_endtime( const std::string& bt ){ _endtime = bt; };

    const std::string textclass() const { return _textclass; };
    void textclass( const std::string& tc ){ _textclass = tc; };

    const std::string speech_src() const;
    void set_speech_src( const std::string& ) NOT_IMPLEMENTED;

    const std::string speech_speaker() const;
    void set_speech_speaker( const std::string& ) NOT_IMPLEMENTED;

    bool space() const { return _space; };
    bool set_space( bool b ) { bool s =_space; _space =  b; return s; };

    SPACE_FLAGS spaces_flag() const { return _preserve_spaces; };
    void set_spaces_flag( SPACE_FLAGS f ) { _preserve_spaces = f; };

    double confidence() const { return _confidence; };
    void confidence( double d ) { _confidence = d; };
    void set_confidence( double d ) { _confidence = d; };

    const std::string language( const std::string& = "" ) const;
    const std::string src() const { return _src; };
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
    const std::string& text_delimiter() const;
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
    KWargs collectAttributes() const;
    xmlNs *foliaNs() const;

  private:
    int refcount() const { return _refcount; };
    void increfcount() { ++_refcount; };
    void decrefcount() { --_refcount; };
    void resetrefcount() { _refcount = 0; };
    void setAuth( bool b ){ _auth = b; };
    void setDateTime( const std::string& );
    const std::string getDateTime() const;
    bool checkAtts();
    void set_typegroup( KWargs& ) const;
    bool acceptable( ElementType ) const;
    bool addable( const FoliaElement * ) const;
    UnicodeString text_container_text( const TextPolicy& ) const;
    void check_text_consistency(bool = true) const;
    void check_text_consistency_while_parsing(bool = true); //can't we merge these two somehow?
    void check_append_text_consistency( const FoliaElement * ) const;
    void check_set_declaration();
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
    return isSubClass( T1::PROPS.ELEMENT_ID, T2::PROPS.ELEMENT_ID );
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
  std::string VersionName();
  std::string Version();

  UnicodeString trim_space( const UnicodeString& in );
  UnicodeString postprocess_spaces( const UnicodeString& in );
  std::string tagToAtt( const FoliaElement* );
  void destroy( FoliaElement *el );

} // namespace folia

#endif // FOLIA_IMPL_H
