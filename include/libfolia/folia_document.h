/*
  Copyright (c) 2006 - 2023
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

#ifndef FOLIA_DOCUMENT_H
#define FOLIA_DOCUMENT_H

#include <list>
#include <set>
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include "unicode/unistr.h"
#include "unicode/regex.h"
#include "libxml/tree.h"
#include "libxml/xpath.h"
#include "libfolia/folia.h"

using namespace icu;

namespace folia {

  extern const std::string NSFOLIA;

  enum ElementType : unsigned int;

  class Pattern {
    friend std::ostream& operator<<( std::ostream&, const Pattern& );
  public:
    explicit Pattern( const std::vector<std::string>&,
		      const ElementType = BASE,
		      const std::string& = "" );
    Pattern( const std::vector<std::string>&,  const std::string& );

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

  class FoliaElement;
  class Word;
  class Sentence;
  class Paragraph;
  class processor;
  class Provenance;

  class Document {
    friend std::ostream& operator<<( std::ostream& os, const Document *d );
    /// enum Mode determines runtime characteristic of the document
    /*!
      The default settings are CHECKTEXT and AUTODECLARE
     */
    enum Mode {
      NOMODE=0,        //!< no special mode is set.
      PERMISSIVE=1,    //!< be permissive for certain incompatablities
      CHECKTEXT=2,     //!< check text consistency
      FIXTEXT=4,       //!< try to fix text inconsistencies in the fly
      STRIP=8,         //!< on output, strip
      CANONICAL=16,    //!< sort ouput in a reproducable way.
      AUTODECLARE=32,  //!< Automagicly add missing Annotation Declarations
      EXPLICIT=64      //!< add all set information
    };
    friend class Engine;

  public:
    Document();
    explicit Document( const KWargs& );
    explicit Document( const std::string& );
    ~Document();
    void init();
    void init_args( const KWargs& );
    bool read_from_string( const std::string& );
    bool readFromString( const std::string& s ){
      /// backward compatability. read_from_string() is preferred
      return read_from_string( s );
    }
    bool read_from_file( const std::string& );
    bool readFromFile( const std::string& s ){
      /// backward compatability. read_from_file() is preferred
      return read_from_file( s );
    }
    bool save( std::ostream&, const std::string&, bool = false ) const;
    bool save( std::ostream& os, bool canonical = false ) const {
      /// save a Document to a stream without using a namespace name
      return save( os, "", canonical );
    }
    bool save( const std::string&, const std::string&, bool = false ) const ;
    bool save( const std::string& s, bool canonical = false ) const {
      /// save a Document to a file without using a namespace name
      return save( s, "", canonical );
    }
    std::string xmlstring( bool = false ) const;

    FoliaElement* doc() const {
      /// return a pointer to the internal FoLiA tree
      return foliadoc;
    }

    template <typename T>
      T *create_root( const KWargs& ){
      throw std::logic_error( "create_root() only possible for 'Text' and 'Speech'" );
    }
    template <typename T>
      T *create_root(){
      throw std::logic_error( "create_root() only possible for 'Text' and 'Speech'" );
    }

    FoliaElement* append( FoliaElement *t );
    Text* setTextRoot();
    Text* setTextRoot( const KWargs& );
    Speech* setSpeechRoot();
    Speech* setSpeechRoot( const KWargs& );
    FoliaElement *getRoot();
    // backward compatible:
    Text* addText( const KWargs& a ){ return setTextRoot( a ); };
    Text* addText( Text *t ){ return dynamic_cast<Text*>( append(t) ); };
    Speech* addSpeech( const KWargs& a ){ return setSpeechRoot( a ); };
    Speech* addSpeech( Speech *s ){ return dynamic_cast<Speech*>( append(s) ); };

    void set_foreign_metadata( xmlNode * );
    void addStyle( const std::string&, const std::string& );
    void replaceStyle( const std::string&, const std::string& );
    UnicodeString text( const std::string& = "current",
			bool = false,
			bool = false ) const;
    UnicodeString text( const TextPolicy& ) const;
    std::vector<Paragraph*> paragraphs() const;
    std::vector<Sentence*> sentences() const;
    std::vector<Sentence*> sentenceParts() const;
    std::vector<Word*> words() const;
    std::vector<std::vector<Word*> > findwords( const Pattern&,
						const std::string& ="" ) const;
    std::vector<std::vector<Word*> > findwords( std::list<Pattern>&,
						const std::string& = "" ) const;
    Word *words( size_t ) const;
    Word *rwords( size_t ) const;
    Paragraph *paragraphs( size_t ) const;
    Paragraph *rparagraphs( size_t ) const;
    Sentence *sentences( size_t ) const;
    Sentence *rsentences( size_t ) const;
    std::string toXml( const std::string& ="" ) const;
    bool toXml( const std::string&,
		const std::string& ) const;
    std::string metadata_type() const;
    std::string metadata_file() const;
    std::string annotation_type_to_string( AnnotationType ) const;
    void set_metadata( const std::string&, const std::string& );
    const std::string get_metadata( const std::string&) const;
    processor *get_default_processor() const;
    processor *get_processor( const std::string& ) const;
    std::vector<processor*> get_processors_by_name( const std::string& ) const;
    void add_doc_index( FoliaElement * );
    void del_doc_index( const std::string& );

    FoliaElement *index( const std::string& ) const; //retrieve element with specified ID
    FoliaElement* operator []( const std::string& ) const ; //index as operator
    bool declared( const AnnotationType&,
		   const std::string& = "" ) const;
    bool declared( ElementType, const std::string& = "" ) const;
    std::string unalias( AnnotationType,
			 const std::string& ) const;
    std::string alias( AnnotationType,
		       const std::string& ) const;

    processor *add_processor( const KWargs&, processor * =0 );
    std::vector<std::string> get_annotators( AnnotationType,
					    const std::string& ="" ) const;
    std::vector<const processor *> get_processors( AnnotationType,
						   const std::string& ="" ) const;

    std::string default_set( AnnotationType ) const;
    std::string original_default_set( AnnotationType ) const;

    std::string default_annotator( AnnotationType,
				   const std::string& ="" ) const;
    AnnotatorType default_annotatortype( AnnotationType,
					const std::string& ="" ) const;

    std::string default_datetime( AnnotationType,
				  const std::string& ="" ) const;
    std::string default_processor( AnnotationType,
				   const std::string& ="" ) const;
    std::string original_default_processor( AnnotationType ) const;

    FoliaElement* parseXml( );

    std::string id() const {
      /// return the Document id value
      return _id;
    };
    std::string language() const;
    void auto_declare( AnnotationType,
		       const std::string& = "" );
    void declare( AnnotationType,
		  const std::string&,
		  const std::string& = "" );
    void declare( AnnotationType,
		  const std::string&,
		  const KWargs& );
    void un_declare( AnnotationType,
		     const std::string& );
    const xmlDoc *XmlDoc() const {
      /// return a pointer to the internal xmlDoc. handle with care.
      return _xmldoc;
    };
    xmlNs *foliaNs() const {
      /// return a pointer to the output namespace structure
      return _foliaNsOut;
    };
    void keepForDeletion( FoliaElement *p ) {
      /// add FoliaElement \e p to the delSet
      /*!
	\param p the FoliaElement to keep for later annihilation
	the delSet is kept until the destruction of the Document
       */
      delSet.insert( p );
    };
    void addExternal( External *p ) {
      /// add a node to the _externals list
      /*!
	\param p The node to add
      */
      _externals.push_back( p );
    };
    void resolveExternals();
    int debug; //!< the debug level. 0 means NO debugging.

    /// is the PERMISSIVE mode set?
    bool permissive() const { return mode & PERMISSIVE; };
    /// is the CHECKTEXT mode set?
    bool checktext() const { return mode & CHECKTEXT; };
    /// is the FIXTEXT mode set?
    bool fixtext() const { return mode & FIXTEXT; };
    /// is the STRIP mode set?
    bool strip() const { return mode & STRIP; };
    /// is the CANONICAL mode set?
    bool canonical() const { return mode & CANONICAL; };
    /// is the AUTODECLARE mode set?
    bool autodeclare() const { return mode & AUTODECLARE; };
    bool has_explicit() const { return mode & EXPLICIT; };
    bool set_permissive( bool ) const; // defined const, but the mode is mutable!
    bool set_checktext( bool ) const; // defined const, but the mode is mutable!
    bool set_fixtext( bool ) const; // defined const, but the mode is mutable!
    bool set_strip( bool ) const; // defined const, but the mode is mutable!
    bool set_canonical( bool ) const; // defined const, but the mode is mutable!
    bool set_autodeclare( bool ) const; // defined const, but the mode is mutable!
    bool set_explicit( bool ) const; // defined const, but the mode is mutable!
    /// this class holds annotation declaration information
    class annotation_info {
      friend std::ostream& operator<<( std::ostream& os,
				       const annotation_info& at );
    public:
    annotation_info( const std::string& a,
		     const AnnotatorType& t,
		     const std::string& d,
		     const std::string& f,
		     const std::set<std::string>& p ):
      _annotator(a),
      _ann_type(t),
	_date(d),
	_format(f),
	_processors(p){};
      std::string _annotator;   ///< the annotator as a string
      AnnotatorType _ann_type; ///< the annotator type
      std::string _date;   ///< the timestamp as a string
      std::string _format;   ///< the format
      std::set<std::string> _processors; ///< the id's of all associated processors
    };
    annotation_info *lookup_default( AnnotationType,
				     const std::string& );
    annotation_info const *lookup_default( AnnotationType,
					   const std::string& ) const;

    void incrRef( AnnotationType, const std::string& );
    void decrRef( AnnotationType, const std::string& );
    void setmode( const std::string& ) const;
    std::string getmode() const;
    int setdebug( int val ){
      /// set the debug level
      /*!
	\param val the new debug value
	\return the old debug value
      */
      int ret=debug; debug=val; return ret;
    };
    std::multimap<AnnotationType,std::string> unused_declarations( ) const;
    const MetaData *get_submetadata( const std::string& m ){
      /// get the metadata structure with value \e m
      /*!
	\param m the value we search
	\return the found MetaData element, or 0
       */
      const auto& it = submetadata.find( m );
      if ( it == submetadata.end() ){
	return 0;
      }
      else {
	return it->second;
      }
    }
    void cache_textcontent( TextContent *tc ){
      /// add a TextContent to the validation buffer
      /*!
	\param tc the TextContent to add to the buffer
	on a call to validate_offsets() this buffer is used to validate
	all offsets.
      */
      t_offset_validation_buffer.push_back( tc );
    }
    void cache_phoncontent( PhonContent *pc ){
      /// add a PhonContent to the validation buffer
      /*!
	\param pc the PhonContent to add
	on a call to validate_offsets() this buffer is used to validate
	all offsets.
      */
      p_offset_validation_buffer.push_back( pc );
    }
    bool validate_offsets() const;
    int compare_to_build_version() const;
    const std::string& version() const {
      /// return the version string
      return _version_string;
    };
    std::string doc_version() const;
    std::string update_version();
    bool version_below( int, int ) const;
    const std::map<AnnotationType,std::map<std::string,annotation_info>>& annotationdefaults() const { return _annotationdefaults; };
    void parse_metadata( const xmlNode * );
    void setDocumentProps( KWargs& );
    Provenance *provenance() const {
      /// return a pointer to the Provenance data
      return _provenance;
    };
    const std::string& filename() const {
      /// return the filename the Document was created from
      return _source_name;
    };
    void save_orig_ann_defaults();
    void set_incremental( bool b ) {
      /// set/unset the incremental_parse flag
      _incremental_parse = b;
    };
    bool is_incremental() const {
      /// return the value of the incremental_parse flag
      return _incremental_parse;
    };
    void set_preserve_spaces( bool b ) {
      /// set/unset the preserve_spaces flag
      _preserve_spaces = b;
    };
    bool preserve_spaces() const {
      /// return the value of the preserve_spaces flag
      return _preserve_spaces;
    }
    int get_warn_count( ) const {
      /// return the number of warnings
      return _warn_count;
    }
    void reset_warn_count( ) {
      /// reset the number of warnings to 0
      _warn_count = 0;
    }
    void increment_warn_count() const {
      /// increment the warning count
      // NOTE: function is defined const, but the _warn_count is mutable
      ++_warn_count;
    }
    void add_textclass( const std::string& tc ){
      _textclasses.insert( tc );
    }
    const std::set<std::string> textclasses() const {
      return _textclasses;
    }
  private:
    void test_temporary_text_exception( const std::string& ) const;
    void adjustTextMode();
    std::map<AnnotationType,std::map<std::string,annotation_info> > _annotationdefaults;   ///< stores all declared annotations per AnnotationType
    std::map<AnnotationType,std::map<std::string,bool> > _groupannotations; ///<
    ///< register which annotations are GROUP annotations
    std::vector<std::pair<AnnotationType,std::string>> _anno_sort; ///<
    ///< register the original sorting of the annotation declarations in the
    ///< input, so we can use that for output in the same order. (cannonical
    ///< mode
    std::map<AnnotationType,std::map<std::string,int> > _annotationrefs; ///<
    ///< register the number of references to this AnnotationType/setname
    std::map<AnnotationType,std::map<std::string,std::string>> _alias_set; ///<
    ///< register the mapping from aliases to setnames per AnnotationType
    std::map<AnnotationType,std::map<std::string,std::string>> _set_alias; ///<
    ///< register the mapping from setname to aliases per AnnotationType
    std::map<AnnotationType,std::string> _orig_ann_default_sets; ///<
    ///< for folia::Engine we need to register the original mapping from a
    ///< AnnoationType to a setname, because in the process more mappings
    ///< can be added, loosing the default.
    std::map<AnnotationType,std::string> _orig_ann_default_procs;///<
    ///< for folia::Engine we need to register the original mapping from a
    ///< AnnoationType to a processor name, because in the process more mappings
    ///< can be added, loosing the default.
    std::set<std::string> _textclasses; ///<
    /// < we keep track of all textclasses found in the document
    std::vector<TextContent*> t_offset_validation_buffer; ///< we register all
    ///< TextContent nodes here to quickly access them for offset checks
    ///< that check is performed directly after parsing
    std::vector<PhonContent*> p_offset_validation_buffer; ///< we register all
    ///< PhonContent nodes here to quickly access them for offset checks
    ///< that check is performed directly after parsing
    void parse_imdi( const xmlNode * );
    void parse_annotations( const xmlNode * );
    void parse_provenance( const xmlNode * );
    void parse_submeta( const xmlNode * );
    void parse_styles();
    void add_annotations( xmlNode * ) const;
    void add_provenance( xmlNode * ) const;
    void add_metadata( xmlNode * ) const;
    void add_submetadata( xmlNode *) const;
    void add_styles( xmlDoc* ) const;
    void append_processor( xmlNode *, const processor * ) const;
    xmlDoc *to_xmlDoc( const std::string& ="" ) const;
    void add_one_anno( const std::pair<AnnotationType,std::string>&,
		       xmlNode * ) const;
    void internal_declare( AnnotationType,
			   const std::string&,
			   const std::string&, const std::string&,
			   const std::string&, const std::string&,
			   const std::set<std::string>&,
			   const std::string& = "" );
    std::map<std::string, FoliaElement* > sindex; ///< the lookup table
    ///< for FoliaElements by index (xml:id) (not all nodes do have an index)
    //    std::vector<FoliaElement*> data;
    std::vector<External*> _externals;
    std::string _id;
    std::set<FoliaElement *> delSet;
    FoliaElement *foliadoc;
    xmlDoc *_xmldoc;
    const xmlChar* _foliaNsIn_href;
    const xmlChar* _foliaNsIn_prefix;
    mutable xmlNs *_foliaNsOut;
    Provenance *_provenance;
    MetaData *_metadata;
    ForeignMetaData *_foreign_metadata;
    std::map<std::string,MetaData *> submetadata;
    std::multimap<std::string,std::string> styles;
    mutable Mode mode;
    std::string _source_name;
    std::string _version_string;
    int _major_version;
    int _minor_version;
    int _sub_version;
    std::string _patch_version;
    bool _external_document;
    bool _incremental_parse;
    bool _preserve_spaces;
    mutable int _warn_count;
    Document( const Document& ); // inhibit copies
    Document& operator=( const Document& ); // inhibit copies
  };

  template <> inline
    Text *Document::create_root( const KWargs& args ){
    return setTextRoot( args );
  }

  template <> inline
    Speech *Document::create_root( const KWargs& args ){
    return setSpeechRoot( args );
  }

  template <> inline
    Text *Document::create_root(){
    return setTextRoot();
  }
  template <> inline
    Speech *Document::create_root(){
    return setSpeechRoot();
  }

  std::ostream& operator<<( std::ostream& os, const Document *d );
  inline std::ostream& operator<<( std::ostream& os, const Document& d ){
    os << &d;
    return os;
  }

  std::ostream& operator<<( std::ostream& os,
			    const Document::annotation_info& at );

  void expand_version_string( const std::string&,
			      int&, int&, int&, std::string& );

  std::string library_version();
  std::string folia_version();

} // namespace folia

#endif // FOLIA_DOCUMENT_H
