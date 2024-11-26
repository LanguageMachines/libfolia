/*
  Copyright (c) 2006 - 2024
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

#ifndef FOLIA_UTILS_H
#define FOLIA_UTILS_H

#include <map>
#include <set>
#include <string>
#include <iostream>
#include <exception>
#include <ctime>
#include "unicode/unistr.h"
#include <unicode/ustream.h>
#include "libxml/tree.h"

#include "ticcutils/StringOps.h"
#include "ticcutils/XMLtools.h"

namespace folia {

  enum class AnnotationType : int;
  enum class AnnotatorType : int;
  enum ElementType : unsigned int;
  class FoliaElement;
  class KWargs;

  class ArgsError: public std::runtime_error {
  public:
    explicit ArgsError( const std::string& s ): std::runtime_error( "error in argument list: " + s  ){};
  };

  class NotImplementedError: public std::runtime_error {
  public:
    explicit NotImplementedError( const std::string& s ):
    std::runtime_error( "NOT IMPLEMENTED: " + s ){};
  };

  class ValueError: public std::runtime_error {
  public:
    explicit ValueError( const std::string& s ):
      std::runtime_error( s ){};
    explicit ValueError( const FoliaElement *,
			 const std::string& s );
  };

  class XmlError: public std::runtime_error {
  public:
    explicit XmlError( const std::string& s ):
      std::runtime_error( "XML error: " + s ){};
    XmlError( const FoliaElement *elt,
	      const std::string& mess );
  };

  class MetaDataError: public std::runtime_error {
  public:
    explicit MetaDataError( const std::string& s ):
      std::runtime_error( "MetaData problem: " + s ){};
  };

  class NoSuchAnnotation: public std::runtime_error {
  public:
    NoSuchAnnotation( const FoliaElement *,
		      const std::string& );
  };

  class NoSuchText: public std::runtime_error {
  public:
    explicit NoSuchText( const std::string& s ):
      std::runtime_error( "no such text: " + s ){};
    explicit NoSuchText( const FoliaElement *,
			 const std::string& );
  };

  class NoSuchPhon: public std::runtime_error {
  public:
    explicit NoSuchPhon( const std::string& s ):
      std::runtime_error( "no such phoneme: " + s ){};
    explicit NoSuchPhon( const FoliaElement *,
			 const std::string& );
  };

  class DuplicateAnnotationError: public std::runtime_error {
  public:
    explicit DuplicateAnnotationError( const std::string& s ):
      std::runtime_error( s ){};
    DuplicateAnnotationError( const FoliaElement *,
			      const std::string& );
  };

  class DuplicateIDError: public std::runtime_error {
  public:
    explicit DuplicateIDError( const std::string& s ):
      std::runtime_error( "duplicate ID : " + s ){};
    DuplicateIDError( const FoliaElement *,
		      const std::string& );
  };

  class DuplicateAttributeError: public std::runtime_error {
  public:
    explicit DuplicateAttributeError( const std::string& s ):
      std::runtime_error( "duplicate attribute : " + s ){};
    DuplicateAttributeError( const KWargs&,
			     const std::string&,
			     const std::string& );
  };

  class NoDefaultError: public std::runtime_error {
  public:
    explicit NoDefaultError( const std::string& s ):
      std::runtime_error( "No Default found: " + s ){};
  };

  class InconsistentText: public std::runtime_error {
  public:
    explicit InconsistentText( const std::string& s ):
      std::runtime_error( "inconsistent text: " + s ){};
    InconsistentText( const FoliaElement *,
		      const std::string& );
  };

  class UnresolvableTextContent: public std::runtime_error {
  public:
    explicit UnresolvableTextContent( const std::string& s ):
      std::runtime_error( "Unresolvable text: " + s ){};
    explicit UnresolvableTextContent( const FoliaElement *,
				      const std::string& );
  };

  class DeclarationError: public std::runtime_error {
  public:
    explicit DeclarationError( const std::string& s ):
      std::runtime_error( "DeclarationError: " + s ){};
    DeclarationError( const FoliaElement *,
		      const std::string& );
  };

  class DocumentError: public std::runtime_error {
  public:
    DocumentError( const std::string& document,
		   const std::string& message ):
      std::runtime_error( document + ": " + message ){};
    DocumentError( const std::string& document,
		   const std::string& message,
		   long int line ):
      std::runtime_error( document + ": (line " + std::to_string(line) + ") "
			  + message ){};
  };

  ///
  /// KWargs is a class to hold attribute/value entries,
  ///
  /// it is used to pass argument lists to and from functions
  /// including attributes for FoLiA constructs
  ///
  /// KWargs is a map, so it kan be indexed (on attribute), iterated etc.
  ///
  class KWargs : public std::map<std::string, std::string> {
  public:
    explicit KWargs( const std::string& ="" );
    KWargs( const std::string&, const std::string& );
    bool is_present( const std::string& ) const;
    std::string lookup( const std::string& ) const;
    std::string extract( const std::string& );
    std::string toString();
    bool add( const std::string&, const std::string& );
    bool replace( const std::string&, const std::string& );
    void init( const std::string& );
  };

  KWargs getArgs( const std::string& );
  std::string toString( const KWargs& );

  void addAttributes( xmlNode *, const KWargs&, bool=false );
  KWargs getAttributes( const xmlNode * );

  std::string parseDate( const std::string& );
  std::string parseTime( const std::string& );

  /// functcions to test internal sanity
  bool AT_sanity_check();
  bool Attrib_sanity_check();
  bool ET_sanity_check();
  bool annotator_sanity_check();
  bool annotation_sanity_check();
  bool document_sanity_check();
  bool space_sanity_check();
  bool subclass_sanity_check();

  ///
  /// some xml goodies
  ///
  inline const xmlChar *to_xmlChar( const std::string& in ){
    return TiCC::to_xmlChar( in );
  }

  inline const std::string to_string( const xmlChar *in ){
    return TiCC::to_string( in );
  }

  inline const std::string to_string( const xmlChar *in, size_t size ){
    return std::string( reinterpret_cast<const char *>(in), size );
  }

  using TiCC::TextValue;
  using TiCC::isNCName;

  std::string create_NCName( const std::string& );

  bool checkNS( const xmlNode *, const std::string& );
  std::map<std::string,std::string> getNS_definitions( const xmlNode * );

  icu::UnicodeString normalize_spaces( const icu::UnicodeString& );
  bool is_norm_empty( const icu::UnicodeString&  );

  std::string get_ISO_date();
  std::string get_fqdn();
  std::string get_user();

} // namespace folia

namespace TiCC {
  // add a specialization to the TiCC namespace
  inline std::ostream& operator<<( std::ostream& os,
				   const folia::KWargs& ats ){
    os << folia::toString( ats );
    return os;
  }

} // namespace TiCC


#endif // FOLIA_UTILS
