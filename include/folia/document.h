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

#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <list>
#include "unicode/unistr.h"
#include "unicode/regex.h"
#include "libxml/tree.h"
#include "libxml/xpath.h"
#include <ctime>
#include "foliautils.h"

namespace folia {
  extern const std::string NSFOLIA;

  class Pattern {
    friend std::ostream& operator<<( std::ostream&, const Pattern& );
  public:
    Pattern( const std::vector<std::string>&, 
	     const ElementType = BASE,
	     const std::string& = "" );
    Pattern( const std::vector<std::string>&, 
	     const std::string& );
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

  class FoliaElement;
  class Word;
  class Sentence;
  class Paragraph;

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
    bool save( std::ostream&, const std::string& = "" );
    bool save( const std::string&, const std::string& = "" );
    int size() const;
    FoliaElement* doc() const { return foliadoc; }
    FoliaElement* addNode( ElementType, const KWargs& );
    void addStyle( const std::string& st ){
      styles.push_back( st );
    };
    std::vector<Paragraph*> paragraphs() const;
    std::vector<Sentence*> sentences() const;
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
    MetaDataType metadatatype() const { return _metadatatype; };
    std::string metadatafile() const { return _metadatafile; };

    FoliaElement *append( FoliaElement* );

    void addDocIndex( FoliaElement* el, const std::string& );

    FoliaElement* operator []( size_t ) const; //select i'th element from data
  
    FoliaElement *index( const std::string& ) const; //retrieve element with specified ID 
    FoliaElement* operator []( const std::string& ) const ; //index as operator 
    bool isDeclared( AnnotationType::AnnotationType, const std::string&, 
		     const std::string&, const std::string& );
    bool isDeclared( AnnotationType::AnnotationType, const std::string& = "" );
    std::string defaultset( AnnotationType::AnnotationType ) const;

    std::string defaultannotator( AnnotationType::AnnotationType, 
				  const std::string& ="" ) const;
    std::string defaultannotatortype( AnnotationType::AnnotationType, 
				      const std::string& ="" ) const;

    FoliaElement* parseXml( );

    std::string id() const { return _id; };
    void declare( AnnotationType::AnnotationType, 
		  const std::string&,
		  const std::string& = "" );
    void declare( AnnotationType::AnnotationType, 
		  const std::string&, const std::string&, const std::string& );
    xmlDoc *XmlDoc() const { return xmldoc; };
    xmlNs *foliaNs() const { return _foliaNs; };
    void keepForDeletion( FoliaElement *p ) { delSet.insert( p ); };
    int debug;

    class at_t {
      friend std::ostream& operator<<( std::ostream&, const at_t& );
    public:
    at_t( const std::string& _a, const std::string& _t ): a(_a),t(_t){}; 
      std::string a;
      std::string t;
    };
  private:
    std::map<AnnotationType::AnnotationType,std::multimap<std::string,at_t> > annotationdefaults;

    FoliaElement* parseFoliaDoc( xmlNode * );
    void setimdi( xmlNode * );
    void setAttributes( const KWargs&  );
    void parseannotations( xmlNode * );
    void getstyles();
    void setannotations( xmlNode *) const;
    void setmetadata( xmlNode * ) const;
    void setstyles( xmlDoc* ) const;
    std::map<std::string, FoliaElement* > sindex;
    std::vector<FoliaElement* > iindex;
    std::vector<FoliaElement*> data;
    std::string _id;
    std::set<FoliaElement *> delSet;
    FoliaElement *foliadoc;
    xmlDoc *xmldoc;
    mutable xmlNs *_foliaNs;
    MetaDataType _metadatatype;
    xmlNode *metadata;
    std::string _metadatafile;
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
  }

  inline std::ostream& operator<<( std::ostream& os, const Document::at_t& at ){
    os << "<" << at.a << "," << at.t << ">";
    return os;
  }

} // namespace folia

#endif // DOCUMENT_H
