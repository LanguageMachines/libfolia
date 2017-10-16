/*
  Copyright (c) 2006 - 2017
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

namespace folia {
  void initMT();

  extern const std::string NSFOLIA;

  enum ElementType : unsigned int;

  class Pattern {
    friend std::ostream& operator<<( std::ostream&, const Pattern& );
  public:
    Pattern( const std::vector<std::string>&,
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

  class Document {
    friend bool operator==( const Document&, const Document& );
    friend std::ostream& operator<<( std::ostream&, const Document * );
    enum Mode { NOMODE=0, PERMISSIVE=1, CHECKTEXT=2, FIXTEXT=4, STRIP=8 };

  public:
    Document();
    explicit Document( const std::string& );
    ~Document();
    void init();
    bool readFromFile( const std::string& );
    bool readFromString( const std::string& );
    bool save( std::ostream&, const std::string&, bool = false ) const;
    bool save( std::ostream& os, bool kanon = false ) const {
      return save( os, "", kanon );
    }
    bool save( const std::string&, const std::string&, bool = false ) const ;
    bool save( const std::string& s, bool kanon = false ) const {
      return save( s, "", kanon );
    }
    std::string xmlstring( bool k = false ) const;
    int size() const;
    FoliaElement* doc() const { return foliadoc; }
    Text* addText( const KWargs& );
    Text* addText( Text * );
    FoliaElement* append( Text *t ){
      // almost backward compatible
      return addText(t);
    };
    void set_foreign_metadata( xmlNode * );
    void addStyle( const std::string&, const std::string& );
    void replaceStyle( const std::string&, const std::string& );
    UnicodeString text( const std::string& = "current",
			bool = false,
			bool = false ) const;
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
    std::string toXml( const std::string& ="",
		       bool = false ) const;
    bool toXml( const std::string&,
		const std::string& ="",
		bool = false ) const;
    std::string metadatatype() const;
    std::string metadatafile() const;
    void set_metadata( const std::string& type, const std::string& value );
    const std::string get_metadata( const std::string& type ) const;

    void addDocIndex( FoliaElement*, const std::string& );
    void delDocIndex( const FoliaElement*, const std::string& );

    FoliaElement* operator []( size_t ) const; //select i'th element from data

    FoliaElement *index( const std::string& ) const; //retrieve element with specified ID
    FoliaElement* operator []( const std::string& ) const ; //index as operator
    bool isDeclared( AnnotationType::AnnotationType,
		     const std::string&,
		     const std::string&,
		     const std::string& );
    bool isDeclared( AnnotationType::AnnotationType, const std::string& = "" );
    std::string unalias( AnnotationType::AnnotationType,
			 const std::string& ) const;
    std::string alias( AnnotationType::AnnotationType,
		       const std::string& ) const;
    std::string defaultset( AnnotationType::AnnotationType ) const;

    std::string defaultannotator( AnnotationType::AnnotationType,
				  const std::string& ="" ) const;
    std::string defaultannotatortype( AnnotationType::AnnotationType,
				      const std::string& ="" ) const;

    std::string defaultdatetime( AnnotationType::AnnotationType,
				 const std::string& ="" ) const;

    FoliaElement* parseXml( );

    std::string id() const { return _id; };
    std::string language() const;
    void declare( AnnotationType::AnnotationType,
		  const std::string&,
		  const std::string& = "" );
    void declare( AnnotationType::AnnotationType,
		  const std::string&, const std::string&,
		  const std::string&, const std::string&,
		  const std::string& = "" );
    void un_declare( AnnotationType::AnnotationType,
		     const std::string& );
    xmlDoc *XmlDoc() const { return _xmldoc; };
    xmlNs *foliaNs() const { return _foliaNsOut; };
    void keepForDeletion( FoliaElement *p ) { delSet.insert( p ); };
    void addExternal( External *p ) { externals.push_back( p ); };
    FoliaElement *resolveExternals( FoliaElement* );
    int debug;
    bool permissive() const { return mode & PERMISSIVE; };
    bool checktext() const {
      return mode & CHECKTEXT;
    };
    bool fixtext() const {
      return mode & FIXTEXT;
    };
    bool strip() const { return mode & STRIP; };
    class at_t {
      friend std::ostream& operator<<( std::ostream&, const at_t& );
    public:
    at_t( const std::string& _a, const std::string& _t, const std::string& _d ): a(_a),t(_t),d(_d){};
      std::string a;
      std::string t;
      std::string d;
    };
    void incrRef( AnnotationType::AnnotationType, const std::string& );
    void decrRef( AnnotationType::AnnotationType, const std::string& );
    void setmode( const std::string& );
    std::string getmode() const;
    std::multimap<AnnotationType::AnnotationType,std::string> unused_declarations( ) const;
      const MetaData *get_submetadata( const std::string& m ){
      const auto& it = submetadata.find( m );
      if ( it == submetadata.end() ){
	return 0;
      } else {
	return it->second;
      }
    }
    void cache_textcontent( TextContent *tc ){
      t_offset_validation_buffer.push_back( tc );
    }
    void cache_phoncontent( PhonContent *tc ){
      p_offset_validation_buffer.push_back( tc );
    }
    bool validate_offsets() const;
    static int compare_to_lib_version( const std::string& );
    static std::string library_version();
    std::string version() const { return _version; };
    std::string update_version();
  private:
    std::map<AnnotationType::AnnotationType,std::multimap<std::string,at_t> > annotationdefaults;
    std::vector<std::pair<AnnotationType::AnnotationType,std::string>> anno_sort;
    std::map<AnnotationType::AnnotationType,std::map<std::string,int> > annotationrefs;
    std::map<AnnotationType::AnnotationType,std::map<std::string,std::string>> alias_set;
    std::map<AnnotationType::AnnotationType,std::map<std::string,std::string>> set_alias;
    std::vector<TextContent*> t_offset_validation_buffer;
    std::vector<PhonContent*> p_offset_validation_buffer;

    FoliaElement* parseFoliaDoc( xmlNode * );
    void parsemeta( xmlNode * );
    void setimdi( xmlNode * );
    void setDocumentProps( KWargs& );
    void parseannotations( xmlNode * );
    void parsesubmeta( xmlNode * );
    void getstyles();
    void setannotations( xmlNode *) const;
    void setmetadata( xmlNode * ) const;
    void addsubmetadata( xmlNode *) const;
    void setstyles( xmlDoc* ) const;
    xmlDoc *to_xmlDoc( const std::string& ="", bool=false ) const;
    std::map<std::string, FoliaElement* > sindex;
    std::vector<FoliaElement* > iindex;
    std::vector<FoliaElement*> data;
    std::vector<External*> externals;
    std::string _id;
    std::set<FoliaElement *> delSet;
    FoliaElement *foliadoc;
    xmlDoc *_xmldoc;
    const xmlChar* _foliaNsIn_href;
    const xmlChar* _foliaNsIn_prefix;
    mutable xmlNs *_foliaNsOut;
    MetaData *_metadata;
    std::map<std::string,MetaData *> submetadata;
    std::multimap<std::string,std::string> styles;
    Mode mode;
    std::string filename;
    std::string _version;
    bool external;
    Document( const Document& ); // inhibit copies
    Document& operator=( const Document& ); // inhibit copies
  };

  bool operator==( const Document&, const Document& );
  inline bool operator!=( const Document& d1, const Document& d2 ){
    return !( d1==d2 );
  }

  std::ostream& operator<<( std::ostream&, const Document * );
  inline std::ostream& operator<<( std::ostream& os, const Document& d ){
    os << &d;
    return os;
  }

  inline std::ostream& operator<<( std::ostream& os, const Document::at_t& at ){
    os << "<" << at.a << "," << at.t << "," << at.d << ">";
    return os;
  }

} // namespace folia

#endif // FOLIA_DOCUMENT_H
