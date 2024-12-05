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
/** @file folia_utils.cxx */

#include <cassert>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <stdexcept>
#include <algorithm>
#include "ticcutils/StringOps.h"
#include "ticcutils/XMLtools.h"
#include "ticcutils/PrettyPrint.h"
#include "ticcutils/Unicode.h"
#include "libfolia/folia.h"
#include "libfolia/folia_properties.h"

using namespace std;
using namespace icu;
using namespace TiCC;

namespace folia {
  using TiCC::operator<<;

  string output_elem( const FoliaElement *elt ){
    string result = (elt->doc()?elt->doc()->filename():"")
      + ( elt->line_number()>=0?
	  ": (line="
	  + std::to_string( elt->line_number() ) + ")"
	  :"" );
    return result;
  }

  ValueError::ValueError( const FoliaElement *elt,
			  const std::string& mess ):
    std::runtime_error( output_elem( elt)
			+ ": Value error: " + mess ){};

  DeclarationError::DeclarationError( const FoliaElement *elt,
				      const std::string& mess ):
    std::runtime_error( output_elem( elt)
			+ ": Declaration error: " + mess ){};

  DuplicateIDError::DuplicateIDError( const FoliaElement *elt,
				      const std::string& mess ):
    std::runtime_error( output_elem( elt)
			+ ": Duplicate ID: " + mess ){};

  DuplicateAttributeError::DuplicateAttributeError( const KWargs& args,
						    const std::string& att,
						    const std::string& val ):
    std::runtime_error( "Duplicate attribute: '" + att + "' with val="
			+ val + ", current value: " + args.lookup(att) ){};

  DuplicateAnnotationError::DuplicateAnnotationError( const FoliaElement *elt,
						      const std::string& mess ):
    std::runtime_error( output_elem( elt)
			+ ": Duplicate Annotation: " + mess ){};

  NoSuchAnnotation::NoSuchAnnotation( const FoliaElement *elt,
				      const std::string& mess ):
    std::runtime_error( output_elem( elt)
			+ ": no such annotation: " + mess ){};

  XmlError::XmlError( const FoliaElement *elt,
		      const std::string& mess ):
    std::runtime_error( output_elem( elt)
			+ ": XML error: " + mess ){};

  InconsistentText::InconsistentText( const FoliaElement *elt,
				      const std::string& mess ):
    std::runtime_error( output_elem( elt)
			+ ": inconsistent text: " + mess ){};

  UnresolvableTextContent::UnresolvableTextContent( const FoliaElement *elt,
						    const std::string& mess ):
    std::runtime_error( output_elem( elt)
			+ ": unresolvable text: " + mess ){};

  NoSuchText::NoSuchText( const FoliaElement *elt,
			  const std::string& mess ):
    std::runtime_error(	output_elem( elt)
			+ ": NO text content: " + mess ){};

  NoSuchPhon::NoSuchPhon( const FoliaElement *elt,
			  const std::string& mess ):
    std::runtime_error( output_elem( elt)
			+ ": NO phon content: " + mess ){};

  FoliaElement *FoliaElement::createElement( const string& tag,
					     Document *doc ){
    /// create a new FoliaElement
    /*!
      \param tag the folia tagname of the desired element
      \param doc the Document the new element will be part of. May be 0
      \return a new FoliaElement
      throws if the tag is unknown
    */
    ElementType et = stringToElementType( tag );
    return createElement( et, doc );
  }

  FoliaElement *FoliaElement::createElement( ElementType et,
					     Document *doc ){
    /// create a new FoliaElement
    /*!
      \param et the ElementType the desired node
      \param doc the Document the new element will be part of. May be 0
      \return a new FoliaElement
    */
    FoliaElement *el = private_createElement( et );
    if ( doc ){
      el->assignDoc( doc );
    }
    return el;
  }

  KWargs::KWargs( const string& s ){
    /// create a KWargs from an input string
    /*!
      \param s The input string, in the following format:
      "att1='val1', att2='val2', ..., attn='valn'"
    */
    init( s );
  }

  KWargs::KWargs( const string& att, const string& val ){
    /// create a KWargs with one att-val pair
    /*!
      \param att the attribute
      \param val the attribute
    */
    add( att, val );
  }

  void KWargs::init( const string& s ){
    /// initialize a KWargs from an input string
    /*!
      \param s The input string, in the following format:
      "att1='val1', att2='val2', ..., attn='valn'"
      all existing values are cleared
    */
    clear();
    bool quoted = false;
    bool parseatt = true;
    bool escaped = false;
    string att;
    string val;
    for ( const auto& let : s ){
      if ( let == '\\' ){
	if ( quoted ){
	  if ( escaped ){
	    val += let;
	    escaped = false;
	  }
	  else {
	    escaped = true;
	    continue;
	  }
	}
	else {
	  throw ArgsError( s + ", stray \\" );
	}
      }
      else if ( let == '\'' ){
	if ( quoted ){
	  if ( escaped ){
	    val += let;
	    escaped = false;
	  }
	  else {
	    if ( att.empty() || val.empty() ){
	      throw ArgsError( s + ", (''?)" );
	    }
	    if ( !add(att,val) ){
	      throw ArgsError( "Duplicate attribute '" + att + "' in"
			       + " arguments list " + s );
	    }
	    att.clear();
	    val.clear();
	    quoted = false;
	  }
	}
	else {
	  quoted = true;
	}
      }
      else if ( let == '=' ) {
	if ( parseatt ){
	  parseatt = false;
	}
	else if ( quoted ) {
	  val += let;
	}
	else {
	  throw ArgsError( s + ", stray '='?" );
	}
      }
      else if ( let == ',' ){
	if ( quoted ){
	  val += let;
	}
	else if ( !parseatt ){
	  parseatt = true;
	}
	else {
	  throw ArgsError( s + ", stray '='?" );
	}
      }
      else if ( let == ' ' ){
	if ( quoted ){
	  val += let;
	}
      }
      else if ( parseatt ){
	att += let;
      }
      else if ( quoted ){
	if ( escaped ){
	  val += "\\";
	  escaped = false;
	}
	val += let;
      }
      else {
	throw ArgsError( s + ", unquoted value or missing , ?" );
      }
    }
    if ( quoted ){
      throw ArgsError( s + ", unbalanced '?" );
    }
  }

  bool KWargs::add( const std::string& att, const std::string& val ){
    /// insert an attribute/value pair into KWargs
    /*!
      \param att name of the attribute
      \param val the value of the attribute
      \return false if nothing is inserted.
              so when att or val empty. or att already present
    */
    if ( att.empty() || val.empty() ){
      return false;
    }
    else if ( is_present( att ) ){
      throw DuplicateAttributeError( *this, att, val );
    }
    else {
      insert( make_pair(att,val) );
      return true;
    }
  }

  bool KWargs::replace( const std::string& att, const std::string& val ){
    /// replace an attribute/values pair with a new value
    /// just insert when the attribute is not present yet
    /*!
      \param att name of the attribute
      \param val the value of the attribute
      \return false when att or val empty
    */
    if ( att.empty() || val.empty() ){
      return false;
    }
    else {
      auto it = find( att );
      if ( it != end() ){
	// already present. remove
	erase(it);
      }
      // and add
      return add( att, val );
    }
  }

  bool KWargs::is_present( const string& att ) const {
    /// check if an attribute is present in the KWargs
    /*!
      \param att The attribute to check
      \return true is present, otherwise false
    */
    return find(att) != end();
  }

  string KWargs::lookup( const string& att ) const {
    /// lookup an attribute
    /*!
      \param att The attribute to check
      \return the value if present, otherwise ""
    */
    string result;
    auto it = find(att);
    if ( it != end() ){
      result = it->second;
    }
    return result;
  }

  string KWargs::extract( const string& att ){
    /// lookup and remove an attribute
    /*!
      \param att The attribute to check
      \return the value if present, otherwise ""
      the attribute is cleared from the KWargs
    */
    string result;
    auto it = find(att);
    if ( it != end() ){
      result = it->second;
      erase(it);
    }
    return result;
  }

  KWargs getArgs( const string& s ){
  ///
  /// explicitely get a KWargs from a string
  /// \param s The input string, in the following format:
  /// "att1='val1', att2='val2', ..., attn='valn'"
  ///
    KWargs result( s );
    return result;
  }

  string toString( const KWargs& args ){
  ///
  /// Convert a KWargs to a string
  /// \param args the KWargs to convert
  /// returns a string, in the following format:
  /// "att1='val1', att2='val2', ..., attn='valn'"
  ///
    string result;
    for ( const auto& [arg,val] : args ){
      result += arg + "='" + val + "'";
      result += ",";
    }
    if ( !result.empty() ){
      result.pop_back(); // remove last ','
    }
    return result;
  }

  string KWargs::toString(){
    return folia::toString( *this );
  }

  inline string att_name( const xmlAttr *node ){
    return to_string( node->name );
  }

  inline string att_content( const xmlAttr *node ){
    return TextValue( node->children );
  }

  KWargs getAttributes( const xmlNode *node ){
    /// extract a KWargs list from an xmlNode
    /*!
      \param node The xmlNode to examine
      \return a KWargs object with all attributes from 'node'

      special care is taken for xml:id and xlink:* attributes
    */
    KWargs atts;
    if ( node ){
      const xmlAttr *a = node->properties;
      while ( a ){
	if ( a->atype == XML_ATTRIBUTE_ID && att_name(a) == "id" ){
	  atts.add("xml:id",att_content(a));
	}
	else if ( a->ns == 0 || a->ns->prefix == 0 ){
	  atts.add(att_name(a), att_content(a));
	}
	else {
	  string pref = to_string( a->ns->prefix );
	  string att  = att_name(a);
	  if ( pref == "xlink" ){
	    atts.add("xlink:"+att, att_content(a));
	  }
	  // all other namespaced attributes are ignored
	}
	a = a->next;
      }
    }
    return atts;
  }

  void addAttributes( xmlNode *node,
		      const KWargs& atts,
		      bool att_dbg ){
    /// add all attributes from 'atts' as attribute nodes to 'node`
    /*!
      \param _node The xmlNode to add to
      \param atts The list of attribute/value pairs
      \param att_dbg do we want to debug? (default false)
      some special care is taken for attributes 'xml:id', 'id' and 'lang'
    */
    KWargs attribs = atts;
    string xid = attribs.extract("xml:id");
    if ( !xid.empty() ){ // xml:id is special
      if ( att_dbg ){
	cerr << "set xml:id " << xid << endl;
      }
      xmlSetProp( node,
		  XML_XML_ID,
		  to_xmlChar(xid) );
    }
    string lang = attribs.extract("lang");
    if ( !lang.empty() ){ // lang is special too
      if ( att_dbg ){
	cerr << "set lang " << lang << endl;
      }
      xmlNodeSetLang( node,
		      to_xmlChar(lang) );
    }
    string id = attribs.extract("id");
    if ( !id.empty() ){
      if ( att_dbg ){
	cerr << "set id " << id << endl;
      }
      xmlSetProp( node,
		  to_xmlChar("id"),
		  to_xmlChar(id) );
    }
    // and now the rest
    for ( const auto& [at,val] : attribs ){
      if ( att_dbg ){
	cerr << "add attribute: [" << at << "," << val << "]" << endl;
      }
      xmlSetProp( node,
		  to_xmlChar(at),
		  to_xmlChar(val) );
    }
  }

  int toMonth( const string& ms ){
    static const map<string,int> m_n_map
      = { { "jan", 0 },
	  { "feb", 1 },
	  { "mar", 2 },
	  { "apr", 3 },
	  { "may", 4 },
	  { "jun", 5 },
	  { "jul", 6 },
	  { "aug", 7 },
	  { "sep", 8 },
	  { "oct", 9 },
	  { "nov", 10 },
	  { "dec", 11 },
    };
    try {
      int result = stringTo<int>( ms );
      return result - 1;
    }
    catch( const exception& ){
      string m = TiCC::lowercase( ms );
      auto it = m_n_map.find( m );
      if ( it == m_n_map.end() ){
	throw runtime_error( "invalid month: " + m );
      }
      else {
	return it->second;
      }
    }
  }

  string parseDate( const string& s ){
    if ( s.empty() ){
      return "";
    }
    //    cerr << "try to read a date-time " << s << endl;
    vector<string> date_time = TiCC::split_at( s, "T" );
    if ( date_time.size() == 0 ){
      date_time = TiCC::split_at( s, " " );
      if ( date_time.size() == 0 ){
	cerr << "failed to read a date-time " << s << endl;
	return "";
      }
    }
    //    cerr << "found " << num << " parts" << endl;
    size_t num = date_time.size();
    tm time = tm();
    if ( num == 1 || num == 2 ){
      //      cerr << "parse date " << date_time[0] << endl;
      vector<string> date_parts = TiCC::split_at( date_time[0], "-" );
      switch ( date_parts.size() ){
      case 3: {
	int mday = stringTo<int>( date_parts[2] );
	time.tm_mday = mday;
      }
	// fall through
      case 2: {
	int mon = toMonth( date_parts[1] );
	time.tm_mon = mon;
      }
	// fall through
      case 1: {
	int year = stringTo<int>( date_parts[0] );
	time.tm_year = year-1900;
      }
	break;
      default:
	cerr << "failed to read a date from " << date_time[0] << endl;
	return "";
      }
    }
    if ( num == 2 ){
      //      cerr << "parse time " << date_time[1] << endl;
      vector<string> date_parts = TiCC::split_at( date_time[1], ":" );
      //      cerr << "parts " << date_parts << endl;
      switch ( date_parts.size() ){
      case 4:
	// ignore
      case 3: {
	int sec = stringTo<int>( date_parts[2] );
	time.tm_sec = sec;
      }
	// fall through
      case 2: {
	int min = stringTo<int>( date_parts[1] );
	time.tm_min = min;
      }
	// fall through
      case 1: {
	int hour = stringTo<int>( date_parts[0] );
	time.tm_hour = hour;
      }
	break;
      default:
	cerr << "failed to read a time from " << date_time[1] << endl;
	return "";
      }
    }
    //  cerr << "read _date time = " << toString(time) << endl;
    char buf[100];
    strftime( buf, 100, "%Y-%m-%dT%X", &time );
    return buf;
  }

  string parseTime( const string& s ){
    if ( s.empty() ){
      return "";
    }
    //    cerr << "try to read a time " << s << endl;
    tm time = tm();
    vector<string> time_parts = TiCC::split_at( s, ":" );
    if ( time_parts.size() != 3 ){
      cerr << "failed to read a time " << s << endl;
      return "";
    }
    time.tm_min = stringTo<int>( time_parts[1] );
    time.tm_hour = stringTo<int>( time_parts[0] );
    string secs = time_parts[2];
    time_parts = TiCC::split_at( secs, "." );
    string mil_sec = "000";
    if ( time_parts.size() == 2 ){
      time.tm_sec = stringTo<int>( time_parts[0] );
      mil_sec = time_parts[1];
    }
    if ( time_parts.size() == 1 ){
      time.tm_sec = stringTo<int>( time_parts[0] );
    }
    char buf[100];
    strftime( buf, 100, "%X", &time );
    string result = buf;
    result += "." + mil_sec;
    //    cerr << "formatted time = " << result << endl;
    return result;
  }

    string create_NCName( const string& s ){
    /// create a valid NCName
    /*!
      \param s a string to be used as template
      \return a string that is a valid NCname

      Make sure these prerequisites are met:
      An xsd:NCName value must start with either a letter or underscore ( _ )
      and may contain only letters, digits, underscores ( _ ), hyphens ( - ),
      and periods ( . ).
    */
    string result;
    try {
      result = TiCC::create_NCName( s );
    }
    catch ( const exception& e ){
      throw XmlError( e.what() );
    }
    return result;
  }

  bool checkNS( const xmlNode *n, const string& ns ){
    string tns = TiCC::getNS(n);
    if ( tns == ns ){
      return true;
    }
    else {
      throw runtime_error( "namespace conflict for tag:" + TiCC::Name(n)
			   + ", wanted:" + ns
			   + " got:" + tns );
    }
    return false;
  }

  UnicodeString normalize_spaces( const UnicodeString& input ){
    /// substitute all spaces and control characters by spaces
    /// AND all multiple spaces by 1, also trims at back and front.
    /*!
      \param input the UnicodeString to normalize
     */
    UnicodeString result;
    UChar32 shy = 0x00ad;   // soft hyphen
    bool is_space = false;
    for ( int i=0; i < input.length(); ++i ){
      if ( input[i] != shy ){
	if ( u_isspace( input[i] ) ){
	  if ( is_space ){
	    // already a space added, skip this one
	    continue;
	  }
	  is_space = true;
	  result += " ";
	  continue;
	}
	else if ( u_iscntrl( input[i] ) ){
	  // ignore
	  continue;
	}
      }
      // normal character, keep it
      is_space = false;
      result += input[i];
    }
    result.trim(); // remove leading and trailing whitespace;
    return result;
  }

  bool is_norm_empty( const UnicodeString& s) {
      /// checks if the string is empty after normalization (strips all control characters), strings with only spaces/newslines etc are considered empty too
      return normalize_spaces(s).isEmpty();
  }

  string get_ISO_date() {
    /// get the current date in ISO 8601 format
    time_t Time;
    time(&Time);
    tm curtime;
    localtime_r(&Time,&curtime);
    char buf[256];
    strftime( buf, 100, "%Y-%m-%dT%X", &curtime );
    string res = buf;
    return res;
  }

  string get_fqdn( ){
    /// function to get the hostname of the machine we are running on
    /*!
      \return a string with the hostname
    */
    string result = "unknown";
    struct addrinfo hints, *info;
    int gai_result;

    char hostname[1024];
    hostname[1023] = '\0';
    if ( gethostname(hostname, 1023) != 0 ){
      cerr << "gethostname failed, using 'unknown'" << endl;
      return result;
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; /*either IPV4 or IPV6*/
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_CANONNAME;

    if ((gai_result = getaddrinfo(hostname, "http", &hints, &info)) != 0) {
      //      cerr << "getaddrinfo failed: " << gai_strerror(gai_result)
      //	   << ", using 'unknown' as hostname" << endl;
      freeaddrinfo(info);
      result = hostname;
      return result;
    }
    const struct addrinfo *p;
    for ( p = info; p != NULL; p = p->ai_next ) {
      result = p->ai_canonname;
      break;
    }
    freeaddrinfo(info);
    return result;
  }

  string get_user(){
    /// function to get the username of the program
    /*!
      \return a string with the username
    */
    string result;
    const char *env = getenv( "USER" );
    if ( env ){
      result = env;
    }
    else {
      result = "unknown";
    }
    return result;
  }

  bool AT_sanity_check(){
    bool sane = true;
    size_t dim = s_ant_map.size();
    if ( dim != ant_s_map.size() ){
      cerr << "s_ant_map and ant_s_map are different in size!" << endl;
      return false;
    }
    for ( auto const& [a_type,dummy] : ant_s_map ){
      string s;
      try {
	s = toString( a_type );
      }
      catch (...){
	cerr << "no string translation for AnnotationType("
	     << int(a_type) << ")" << endl;
	sane = false;
      }
      if ( !s.empty() ){
	try {
	  stringTo<AnnotationType>( s );
	}
	catch ( const ValueError& e ){
	  cerr << "no AnnotationType found for string '" << s << "'" << endl;
	  sane = false;
	}
      }
    }
    return sane;
  }

  bool ET_sanity_check(){
    bool sane = true;
    for ( auto const& [sett,et] : s_et_map ){
      if ( et_s_map.find(et) == et_s_map.end() ){
	cerr << "for set=" << sett
	     << ", no et_s found, ElementType(" << int(et) << ")" << endl;
	return false;
      }
    }
    for ( auto const& [et,sett] : et_s_map ){
      if ( s_et_map.find(sett) == s_et_map.end() ){
	cerr << "no string found for ElementType(" << int(et) << ")" << endl;
	return false;
      }
      string s;
      try {
	s = toString( et );
      }
      catch (...){
	cerr << "toString failed for ElementType(" << int(et) << ")" << endl;
	sane = false;
      }
      if ( !s.empty() ){
	ElementType et2;
	try {
	  et2 = stringToElementType( s );
	}
	catch ( const ValueError& e ){
	  cerr << "no element type found for string '" << s << "'" << endl;
	  sane = false;
	  continue;
	}
	if ( et != et2 ){
	  cerr << "Argl: toString(ET) doesn't match original: " << s
	       << " vs " << toString(et2) << endl;
	  sane = false;
	  continue;
	}
	FoliaElement *tmp1 = 0;
	try {
	  tmp1 = AbstractElement::createElement( s );
	}
	catch( const ValueError &e ){
	  string err = e.what();
	  if ( err.find("abstract") ==string::npos ){
	    cerr << "createElement(" << s << ") failed! :" << err << endl;
	    sane = false;
	  }
	}
	if ( sane && tmp1 ){
	  destroy( tmp1 );
	  FoliaElement *tmp2 = 0;
	  try {
	    tmp2 = AbstractElement::createElement( et );
	  }
	  catch( const ValueError &e ){
	    string err = e.what();
	    if ( err.find("abstract") == string::npos ){
	      cerr << "createElement(" << int(et) << ") failed! :" << err << endl;
	      sane = false;
	    }
	  }
	  if ( tmp2 != 0 ) {
	    if ( et != tmp2->element_id() ){
	      cerr << "the element type " << tmp2->element_id() << " of " << s
		   << " != " << et << " (" << toString(tmp2->element_id())
		   << " != " << toString(et) << ")" << endl;
	      sane = false;
	    }
	    if ( s != tmp2->xmltag() && s != "headfeature" ){
	      cerr << "the xmltag " << tmp2->xmltag() << " != " << s << endl;
	      sane = false;
	    }
	    destroy(tmp2);
	  }
	}
      }
    }
    return sane;
  }

  bool document_sanity_check(){
    cerr << " Creating a document from scratch: ";
    Document d( "xml:id='example'" );
    d.declare( AnnotationType::TOKEN, "adhocset", "annotator='proycon'" );
    if ( d.default_set(AnnotationType::TOKEN) != "adhocset"
	 ||
	 d.default_annotator(AnnotationType::TOKEN) != "proycon" ){
      cerr << " Defaultset or defaultannotator does not match" << endl;
      return EXIT_FAILURE;
    }
    string id = d.id() + ".text.1";
    KWargs kw = getArgs( "xml:id='" + id + "'" );
    FoliaElement *txt = d.addText( kw );
    kw.clear();
    kw = getArgs( "generate_id='" + txt->id() + "'" );
    FoliaElement *s = new Sentence( kw, &d );
    txt->append( s );
    kw.clear();
    kw.add("text","De");
    s->addWord( kw );
    kw.replace("text","site");
    s->addWord( kw );
    kw.replace("text","staat");
    s->addWord( kw );
    kw.replace("text","online");
    s->addWord( kw );
    kw.replace("text", ".");
    s->addWord( kw );
    if ( d[id+".s.1"]->size() != 5 ) {
      cerr << " Unexpected sentence size, " <<  d[id+".s.1"]->size() << ", expected 5" << endl;
      return EXIT_FAILURE;
    }
    UnicodeString txt_val = s->text();
    if ( txt_val != "De site staat online ." ) {
      cerr << " Text does not match reference: '" << txt_val << "' vs reference: 'De site staat online .'" << endl;
      return EXIT_FAILURE;
    }
    cerr << s->text() << endl;
    d.setdebug( "ANNOTATIONS|SERIALIZE" );
    assert( toString(d.debug) == "ANNOTATIONS|SERIALIZE" );
    return true;
  }

  bool annotator_sanity_check() {
    bool result = true;
    set<string> as = { "auto", "manual", "generator", "datasource", "UNDEFINED" };
    for ( const auto& in_ans : as ){
      AnnotatorType ann = stringToAnnotatorType( in_ans );
      string out_ans = toString( ann );
      if ( out_ans != in_ans ){
	cout << "insane AnnotatorType: " << in_ans << " !=" << out_ans << endl;
	result = false;
      }
    }
    return result;
  }

  bool annotation_sanity_check(){
    bool result = true;
    for ( const auto& [a_type,a_string] : ant_s_map ){
      AnnotationType ant = stringToAnnotationType( a_string );
      string out_ans = folia::toString( ant );
      if ( out_ans != a_string ){
	cout << "insane AnnotationType: " << a_string << " !=" << out_ans << endl;
	result = false;
      }
      if ( ant != a_type ){
	cout << "insane AnnotationType: " << ant << " !=" << a_type << endl;
	result = false;
      }
      if ( !result ){
	break;
      }
    }
    return result;
  }

  bool space_sanity_check() {
    UnicodeString dirty = "    A    dir\ty \n  string\r.\n   ";
    UnicodeString clean = normalize_spaces( dirty );
    UnicodeString wanted = "A dir y string .";
    if ( clean != wanted ){
      cerr << "normalize_space() test 1 failed: got:'" << clean << "'"
	   << "                 but expected:'" << wanted << "'" << endl;
      return false;
    }
    dirty = "\n";
    clean = normalize_spaces( dirty );
    wanted = "";
    if ( clean != wanted ){
      cerr << "normalize_space() test 2 failed: got:'" << clean << "'"
	   << "                 but expected:'" << wanted << "'" << endl;
      return false;
    }
    dirty = "\r x   ";
    clean = normalize_spaces( dirty );
    wanted = "x";
    if ( clean != wanted ){
      cerr << "normalize_space() test 3 failed: got:'" << clean << "'"
	   << "                 but expected:'" << wanted << "'" << endl;
      return false;
    }
    dirty = u"\u001B"; // ESC
    clean = normalize_spaces( dirty );
    wanted = "";
    if ( clean != wanted ){
      cerr << "normalize_space() test 4 failed: got:'" << clean << "'"
	   << "                 but expected:'" << wanted << "'" << endl;
      return false;
    }
    if ( !is_norm_empty( dirty ) ){
      cerr << "is_norm_empty() failed." << endl;
      return false;
    }
    return true;
  }

  bool subclass_sanity_check(){
    if ( isSubClass<AbstractWord,Word>() ){
      cerr << "isSubClass<AbstractWord,Word>() failed" << endl;
      return false;
    }
    if ( !isSubClass<Word,AbstractWord>() ){
      cerr << "isSubClass<Word,AbstractWord>() failed" << endl;
      return false;
    }
    if ( isSubClass<AbstractStructureElement,Word>() ){
      cerr << "isSubClass<AbstractStructureElement,Word>() failed" << endl;
      return false;
    }
    if ( !isSubClass<Word,AbstractStructureElement>() ){
      cerr << "isSubClass<Word,AbstractStructureElement>() failed" << endl;
      return false;
    }
    if ( isSubClass<AbstractElement, AbstractWord>() ){
      cerr << "isSubClass<AbstractElement, AbstractWord>() failed" << endl;
      return false;
    }
    if ( !isSubClass<AbstractWord, FoliaElement>() ) {
      cerr << "isSubClass<AbstractWord, FoliaElement>() failed" << endl;
      return false;
    }
    if ( !isSubClass<AbstractWord, AbstractStructureElement>() ){
      cerr << "isSubClass<AbstractWord, AbstractStructureElement>() failed" << endl;
      return false;
    }
    if ( isSubClass<AbstractElement,Word>() ){
      cerr << "isSubClass<AbstractElement,Word>() failed" << endl;
      return false;
    }
    if ( !isSubClass<Word,AbstractElement>() ){
      cerr << "isSubClass<Word,AbstractElement>() failed" << endl;
      return false;
    }
    if ( !isSubClass<Word,Word>() ){
      cerr << "isSubClass<Word,Word>() failed" << endl;
      return false;
    }
    if ( !isSubClass<AllowXlink,FoliaElement>() ){
      cerr << "isSubClass<AllowXlink,FoliaElement>() failed" << endl;
      return false;
    }
    {
      Word *w = new Word();
      if ( !w->isSubClass<Word>() ){
	cerr << "Word::isSubClass<Word>() failed" << endl;
	return false;
      }
      if ( !w->isSubClass<FoliaElement>() ){
	cerr << "Word::isSubClass<FoliaElement() failed" << endl;
	return false;
      }
      if ( !w->isSubClass<AbstractWord>() ){
	cerr << "Word::isSubClass<AbstractWord>() failed" << endl;
	return false;
      }
      if ( !w->isSubClass<AbstractElement>() ){
	cerr << "Word::isSubClass<AbstractElement() failed" << endl;
	return false;
      }
      if ( !w->isSubClass<AbstractStructureElement>() ){
	cerr << "Word::isSubClass<AbstractStructureElement() failed" << endl;
	return false;
      }
      if ( w->isSubClass<Feature>() ){
	cerr << "Word::isSubClass<Feature>() failed" << endl;
	return false;
      }
    }
    try {
      vector<FoliaElement*> ve;
      vector<Paragraph*> vp;
      merge( ve, vp );
    }
    catch ( const exception& e ){
      cerr << "Unexpected fail: " << e.what() << endl;
      return false;
    }
    return true;
  }

} //namespace folia
