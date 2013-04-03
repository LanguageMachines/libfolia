/*
  $Id$
  $URL$

  Copyright (c) 1998 - 2013
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
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <set>
#include <list>
#include <vector>
#include <map>
#include <algorithm>
#include <stdexcept>
#include "ticcutils/XMLtools.h"
#include "folia/document.h"
#include "folia/folia.h"
#include "config.h"

using namespace std;
using namespace TiCC;

namespace folia {

  const string FOLIAVERSION = "0.8.2"; // the FoLiA version we implement
  const string NSFOLIA = "http://ilk.uvt.nl/folia";
  const string NSDCOI = "http://lands.let.ru.nl/projects/d-coi/ns/1.0";
  const string NSIMDI = "http://www.mpi.nl/IMDI/Schema/IMDI";

  Document::Document(){
    init();
  }

  Document::Document( const string& args ){
    init();
    setAttributes( getArgs( args ) );
    if ( ! foliadoc )
      foliadoc = new FoLiA( this, args );
  }

  void Document::init(){
    _metadatatype = NATIVE;
    metadata = 0;
    xmldoc = 0;
    foliadoc = 0;
    _foliaNsIn = 0;
    _foliaNsOut = 0;
    debug = 0;
    version = FOLIAVERSION;
  }

  Document::~Document(){
    xmlFreeDoc( xmldoc );
    delete foliadoc;
    set<FoliaElement*>::const_iterator it = delSet.begin();
    while ( it != delSet.end() ){
      delete *it;
      ++it;
    }
  }

  bool operator==( const Document& d1, const Document& d2 ){
    if ( d1.data.size() != d2.data.size() )
      return false;
    for ( size_t i = 0; i < d1.data.size(); ++i ){
      if ( *d1.data[i] != *d2.data[i] )
	return false;
    }
    return true;
  }
  
  bool operator==( const FoliaElement& a1, const FoliaElement& a2){
    if ( a1._element_id != a2._element_id )
      return false;
    if ( a1._id != a2._id )
      return false;
    if ( a1._set != a2._set )
      return false;
    if ( a1._class != a2._class )
      return false;
    if ( a1._annotator != a2._annotator )
      return false;
    if ( a1._annotator_type != a2._annotator_type )
      return false;
    if ( a1.data.size() == a2.data.size() ) {
      for ( size_t i = 0; i < a1.data.size(); ++i ){
	if ( *a1.data[i] != *a2.data[i] )
	  return false;
      }
    }
    return true;
  }

  void Document::setAttributes( const KWargs& kwargs ){
    bool happy = false;
    KWargs::const_iterator it = kwargs.find( "debug" );
    if ( it != kwargs.end() )
      debug = stringTo<int>( it->second );
    else
      debug = 0;
    it = kwargs.find( "load" );
    if ( it != kwargs.end() )
      loadall = stringTo<bool>( it->second );
    else
      loadall = true;
    it = kwargs.find( "version" );
    if ( it != kwargs.end() ){
      version = it->second;
    }
    else
      version.clear();
    
    it = kwargs.find( "id" );
    if ( it != kwargs.end() ){
      if ( isNCName( it->second ) ){
	_id = it->second;
      }
      else {
	// isNCName throws
      }
      happy = true;
    }
    else {
      it = kwargs.find( "file" );
      if ( it != kwargs.end() ){
	filename = it->second;
	happy = readFromFile(filename);
      }
      else {
	it = kwargs.find( "string" );
	if ( it != kwargs.end() ){
	  string s = it->second;
	  happy = readFromString( s );
	}
      }
    }
    if ( !happy )
      throw runtime_error( "No ID, valid filename or string specified" );
  }

  void Document::addDocIndex( FoliaElement* el, const string& s ){
    if ( sindex.find( s ) == sindex.end() ){
      sindex[s] = el;
      iindex.push_back( el );
    }
    else
      throw DuplicateIDError( s );
  }

  static int error_sink(void *mydata, xmlError *error ){
    int *cnt = (int*)mydata;
    if ( *cnt == 0 ){
      cerr << "\nXML-error: " << error->message << endl;
    }
    (*cnt)++;
    return 1;
  }

  bool Document::readFromFile( const string& s ){
    if ( xmldoc ){
      throw runtime_error( "Document is aready initialized" );
      return false;
    }
    int cnt = 0;
    xmlSetStructuredErrorFunc( &cnt, (xmlStructuredErrorFunc)error_sink );
    xmldoc = xmlReadFile( s.c_str(), 0, XML_PARSE_NOBLANKS );
    if ( xmldoc ){
      if ( cnt > 0 ){
	throw XmlError( "document is invalid" );
      }
      if ( debug )
	cout << "read a doc from " << s << endl;
      foliadoc = parseXml();
      if ( debug ){
	if ( foliadoc ){
	  cout << "succesful parsed the doc" << endl;
	}
	else
	  cout << "failed to parse the doc" << endl;
      }
      return foliadoc != 0;
    }
    if ( debug )
      cout << "Failed to read a doc from " << s << endl;
    return false;
  }

  bool Document::readFromString( const string& s ){
    if ( xmldoc ){
      throw runtime_error( "Document is aready initialized" );
      return false;
    }
    int cnt = 0;
    xmlSetStructuredErrorFunc( &cnt, (xmlStructuredErrorFunc)error_sink );
    xmldoc = xmlReadMemory( s.c_str(), s.length(), 0, 0, XML_PARSE_NOBLANKS );
    if ( xmldoc ){
      if ( cnt > 0 ){
	throw XmlError( "document is invalid" );
      }
      if ( debug )
	cout << "read a doc from string" << endl;
      foliadoc = parseXml();
      if ( debug ){
	if ( foliadoc ){
	  cout << "succesful parsed the doc" << endl;
	}
	else
	  cout << "failed to parse the doc" << endl;
      }
      return foliadoc != 0;
    }
    if ( debug )
      cout << "Failed to read a doc from a string" << endl;
    return false;
  }

  ostream& operator<<( ostream& os, const Document& d ){
    string s = d.toXml();
    os << s << endl;
    return os;
  }

  bool Document::save( ostream& os, const string& nsLabel, bool kanon ) {
    string s = toXml( nsLabel, kanon );
    os << s << endl;
    return os;
  }

  bool Document::save( const string& fn, const string& nsLabel, bool kanon ) {
    ofstream os( fn.c_str() );
    if ( os.good() ) {
      return save( os, nsLabel, kanon );
    }
    throw runtime_error( "saving to file " + fn + " failed" );
    return false;
  }

  int Document::size() const {
    if ( foliadoc )
      return foliadoc->size(); 
    else
      return 0;
  }

  FoliaElement* Document::index( const string& s ) const {
    map<string,FoliaElement*>::const_iterator it = sindex.find( s );
    if ( it == sindex.end() )
      return 0;
    else
      return it->second;
  }

  FoliaElement* Document::operator []( const string& s ) const {
    return index(s);
  }

  FoliaElement* Document::operator []( size_t i ) const {
    if ( i < iindex.size()-1 )
      return iindex[i+1];
    else
      throw range_error( "Document index out of range" );
  }

  vector<Paragraph*> Document::paragraphs() const {
    return foliadoc->select<Paragraph>();
  }

  vector<Sentence*> Document::sentences() const {
    static set<ElementType> excludeSet;
    if ( excludeSet.empty() ){
      excludeSet.insert( Quote_t );
    }
    return foliadoc->select<Sentence>( excludeSet );
  }
  
  vector<Sentence*> Document::sentenceParts() const {
    static set<ElementType> excludeSet;
    vector<Sentence*> sents = foliadoc->select<Sentence>( excludeSet );
    return sents;
  }
  
  Sentence *Document::sentences( size_t index ) const {
    vector<Sentence*> v = sentences();
    if ( index < v.size() ){
      return v[index];
    }
    else
      throw range_error( "sentences() index out of range" );
  }

  Sentence *Document::rsentences( size_t index ) const {
    vector<Sentence*> v = sentences();
    if ( index < v.size() ){
      return v[v.size()-1-index];
    }
    else
      throw range_error( "rsentences() index out of range" );
  }

  vector<Word*> Document::words() const {
    static set<ElementType> excludeSet;
    if ( excludeSet.empty() ){
      excludeSet.insert( Original_t );
      excludeSet.insert( Suggestion_t );
      excludeSet.insert( Alternative_t );
      excludeSet.insert( Chunk_t );
      excludeSet.insert( SyntacticUnit_t );
      excludeSet.insert( Coreferences_t );
      excludeSet.insert( Semroles_t );
      excludeSet.insert( Entity_t );
      excludeSet.insert( Headwords_t );
      excludeSet.insert( TimingLayer_t );
      excludeSet.insert( DependencyDependent_t );
      excludeSet.insert( TimeSegment_t );
    }
    return foliadoc->select<Word>( excludeSet );
  }
  
  Word *Document::words( size_t index ) const {
    vector<Word*> v = words();
    if ( index < v.size() ){
      return v[index];
    }
    else
      throw range_error( "words() index out of range" );
  }

  Word *Document::rwords( size_t index ) const {
    vector<Word*> v = words();
    if ( index < v.size() ){
      return v[v.size()-1-index];
    }
    else
      throw range_error( "rwords() index out of range" );
  }

  Paragraph *Document::paragraphs( size_t index ) const {
    vector<Paragraph*> v = paragraphs();
    if ( index < v.size() ){
      return v[index];
    }
    else
      throw range_error( "paragraphs() index out of range" );
  }

  Paragraph *Document::rparagraphs( size_t index ) const {
    vector<Paragraph*> v = paragraphs();
    if ( index < v.size() ){
      return v[v.size()-1-index];
    }
    else
      throw range_error( "rparagraphs() index out of range" );
  }

  FoliaElement *Document::append( FoliaElement *txt ){
    if ( !isinstance( txt, Text_t ) )
      throw runtime_error( "can only append() a Text_t element to a Document" );
    else {
      return foliadoc->append( txt );
    }
  }

  void Document::setimdi( xmlNode *node ){
    xmlNode *n = xPath( node, "//imdi:Session/imdi:Title" );
    if ( n ){
      _title = XmlContent( n );
    }
    n = xPath( node, "//imdi:Session/imdi:Date" );
    if ( n ){
      _date = XmlContent( n);
    }
  
    n = xPath( node, "//imdi:Source/imdi:Access/imdi:Publisher" );
    if ( n ){
      _publisher = XmlContent( n );
    }
  
    n = xPath( node, "//imdi:Source/imdi:Access/imdi:Availability" );
    if ( n ){
      _license = XmlContent( n );
    }
    n = xPath( node, "//imdi:Languages/imdi:Language/imdi:ID" );
    if ( n )
      _language = XmlContent( n );
  }

  void Document::parseannotations( xmlNode *node ){
    xmlNode *n = node->children;
    while ( n ){
      string tag = Name( n );
      if ( tag.length() > 11 && tag.substr( tag.length() - 11 ) == "-annotation" ){
	string prefix = tag.substr( 0,  tag.length() - 11 );
	AnnotationType::AnnotationType type = stringToAT( prefix );
	KWargs att = getAttributes( n );
	string s;
	string a;
	string t;
	string d;
	KWargs::const_iterator it = att.find("set" );
	if ( it != att.end() ){
	  s = it->second;
	}
	else {
	  s = "undefined"; // default value
	}
	it = att.find( "annotator" );
	if ( it != att.end() )
	  a = it->second;
	it = att.find( "annotatortype" );
	if ( it != att.end() ){
	  t = it->second;
	}
	it = att.find( "datetime" );
	if ( it != att.end() ){
	  d = parseDate( it->second );
	}
	declare( type, s, a, t, d );
      }
      n = n->next;
    }
  }

  bool checkNS( xmlNode *n, const string& ns ){
    if ( getNS(n) == ns )
      return true;
    else
      throw runtime_error( "namespace conflict for tag:" + Name(n)
			   + ", wanted:" + ns
			   + " got:" + getNS(n) );
    return false;
  }

  FoliaElement* Document::parseFoliaDoc( xmlNode *root ){
    KWargs att = getAttributes( root );
    if ( att["id"] == "" ){
      throw XmlError("FoLiA Document has no ID!");
      return 0;
    }
    setAttributes( att );
    FoliaElement *result = FoliaElement::createElement( this, Name(root) );
    if ( debug > 2 )
      cerr << "created " << root << endl;
    result->setAttributes( att );
    xmlNode *p = root->children;
    while ( p ){
      if ( p->type == XML_ELEMENT_NODE ){
	if ( Name(p) == "metadata" &&
	     checkNS( p, NSFOLIA ) ){
	  if ( debug > 1 )
	    cerr << "Found metadata" << endl;
	  KWargs atts = getAttributes( p );
	  string val = lowercase(atts["type"]);
	  if ( !val.empty() ){
	    if ( val == "native" ){
	      _metadatatype = NATIVE;
	    }
	    else if ( val == "imdi" ){
	      _metadatatype = IMDI;
	      _metadatafile = atts["src"];
	    }
	    else if ( val == "cmdi" ){
	      _metadatatype = CMDI;
	      _metadatafile = atts["src"];
	    }
	  }
	  xmlNode *m = p->children;
	  while ( m ){
	    if ( Name(m)  == "METATRANSCRIPT" ){
	      if ( !checkNS( m, NSIMDI ) || _metadatatype != IMDI )
		throw runtime_error( "imdi != imdi " );
	      if ( debug > 1 )
		cerr << "found IMDI" << endl;
	      metadata = m;
	      setimdi(m);
	    }
	    else if ( Name( m ) == "annotations" &&
		      checkNS( m, NSFOLIA ) ){
	      if ( debug > 1 )
		cerr << "found annotations" << endl;
	      parseannotations( m );
	    }
	    m = m->next;
	  }
	}
	else {
	  if ( p && getNS(p) == NSFOLIA ){
	    string tag = Name( p );
	    FoliaElement *t = FoliaElement::createElement( this, tag );
	    if ( t ){
	      if ( debug > 2 )
		cerr << "created " << t << endl;
	      t = t->parseXml( p );
	      if ( t ){
		if ( debug > 2 )
		  cerr << "extend " << result << " met " << tag << endl;
		result->append( t );
	      }
	    }
	  }
	}
      }
      p = p->next;
    }
    return result;
  }
  
  void Document::addStyle( const std::string& type, const std::string& href ){
    if ( type == "text/xsl" ){
      multimap<string,string>::iterator it = styles.find( type );
      if ( it != styles.end() )
	throw XmlError( "multiple 'text/xsl' style-sheets defined." );
    }
    styles.insert( make_pair( type, href ) );
  }
  
  void Document::replaceStyle( const std::string& type, 
			       const std::string& href ){
    multimap<string,string>::iterator it = styles.find( type );
    if ( it != styles.end() ){
      it->second = href;
    }
    else {
      styles.insert( make_pair( type, href ) );
    }
  }

  void Document::getstyles(){
    xmlNode *pnt = xmldoc->children;
    while ( pnt ){
      if ( pnt->type == XML_PI_NODE && Name(pnt) == "xml-stylesheet" ){
	string content = (const char*)pnt->content;
	string type;
	string href;
	vector<string> v;
	split( content, v );
	if ( v.size() == 2 ){
	  vector<string> w;
	  split_at( v[0], w, "=" );
	  if ( w.size() == 2 && w[0] == "type" ){
	    type = w[1].substr(1,w[1].length()-2);
	  }
	  w.clear();
	  split_at( v[1], w, "=" );
	  if ( w.size() == 2 && w[0] == "href" ){
	    href = w[1].substr(1,w[1].length()-2);
	  }
	}
	if ( !type.empty() && !href.empty() ){
	  addStyle( type, href );
	}
	else {
	  throw XmlError( "problem parsing line: " + content );
	}
      }
      pnt = pnt->next;
    }
  }

  FoliaElement* Document::parseXml( ){
    getstyles();
    xmlNode *root = xmlDocGetRootElement( xmldoc );
    _foliaNsIn = root->ns;
    if ( debug > 2 ){
      using TiCC::operator <<;
      string dum;
      cerr << "root = " << Name( root ) << endl;
      cerr << "in namespace " << getNS( root, dum ) << endl;
      cerr << "namespace list" << getNSvalues( root ) << endl;
    }
    FoliaElement *result = 0;
    if ( root  ){
      try {
	if ( Name( root ) == "FoLiA" &&
	     checkNS( root, NSFOLIA ) ){
	  result = parseFoliaDoc( root );
	  if ( result ){
	    if ( isNCName( result->id() ) )
	      _id = result->id();
	    else {
	      // isNCName throws
	    }
	  }
	}
	else if ( Name( root ) == "DCOI" &&
		  checkNS( root, NSDCOI ) ){
	  throw runtime_error( "DCOI format not supported" );
	}
	else {
	  throw runtime_error("root node must be FoLiA" );
	}
      }
      catch ( exception& e ){
	throw XmlError( e.what() );
      }
    }
    return result;
  }

  void Document::declare( AnnotationType::AnnotationType type, 
			  const string& setname, const string& args ){
    string st = setname;
    if ( st.empty() )
      st = "undefined";
    KWargs kw = getArgs( args );
    string a = kw["annotator"];
    string t = kw["annotatortype"];
    string d = kw["datetime"];
    kw.erase("annotator");
    kw.erase("annotatortype");
    kw.erase("datetime");
    if ( kw.size() != 0 ){
      throw XmlError( "declaration: expected 'annotator', 'annotatortype' or 'datetime', got '" + kw.begin()->first + "'" );
    }
    declare( type, st, a, t, d );
  }

  string getNow() {
    time_t Time;
    time(&Time);
    tm curtime;
    localtime_r(&Time,&curtime);
    char buf[256];
    strftime( buf, 100, "%Y-%m-%dT%X", &curtime );
    string res = buf;
    return res;
  }
  
  void Document::declare( AnnotationType::AnnotationType type, 
			  const string& s, const string& a, 
			  const string& t, const string& ds ){
    string d = ds;
    if ( d == "now()" ){
      d = getNow();
    }
    if ( !isDeclared( type, s, a, t, d ) ){
      annotationdefaults[type].insert( make_pair(s, at_t(a,t,d) ) );
      //    cerr << "inserted [" << type << "][" << st << "](" << a << "," << t << "," << d ")" << endl;
      //    cerr << "annotation defaults now: " <<  annotationdefaults << endl;
      
    }
  }

  FoliaElement* Document::addNode( ElementType et, const KWargs& kwargs ){
    FoliaElement *res = FoliaElement::createElement( this, et );
    res->setAttributes( kwargs );
    foliadoc->append( res );
    return res;
  }

  bool Document::isDeclared( AnnotationType::AnnotationType type,
			     const string& s, 
			     const string& a,
			     const string& t,
			     const string& d ){
    if ( type == AnnotationType::NO_ANN ){
      return true;
    }
    map<AnnotationType::AnnotationType,multimap<string,at_t> >::const_iterator mit1 = annotationdefaults.find(type);
    if ( mit1 != annotationdefaults.end() ){
      if ( s.empty() )
	throw runtime_error("isDeclared with empty set.");
      multimap<string,at_t>::const_iterator mit2 = mit1->second.lower_bound(s);
      while ( mit2 != mit1->second.upper_bound(s) ){
	if ( mit2->second.a == a && mit2->second.t == t && mit2->second.d == d )
	  return true;
	++mit2;
      }
    }
    return false;
  }

  bool Document::isDeclared( AnnotationType::AnnotationType type,
			     const string& s ){
    if ( type == AnnotationType::NO_ANN ){
      return true;
    }
    map<AnnotationType::AnnotationType,multimap<string,at_t> >::const_iterator mit1 = annotationdefaults.find(type);
    if ( mit1 != annotationdefaults.end() ){
      if ( s.empty() )
	return true;
      multimap<string,at_t>::const_iterator mit2 = mit1->second.find(s);
      return mit2 != mit1->second.end();
    }
    return false;
  }

  string Document::defaultset( AnnotationType::AnnotationType type ) const {
    if ( type == AnnotationType::NO_ANN )
      return "";
    // search a set. it must be unique. Otherwise return ""
    //    cerr << "zoek '" << type << "' default set " <<  annotationdefaults << endl;
    map<AnnotationType::AnnotationType,multimap<string,at_t> >::const_iterator mit1 = annotationdefaults.find(type);
    string result;
    if ( mit1 != annotationdefaults.end() ){
      //      cerr << "vind tussen " <<  mit1->second << endl;
      if ( mit1->second.size() == 1 )
	result = mit1->second.begin()->first;
    }
    //    cerr << "defaultset ==> " << result << endl;
    return result;
  }

  std::string Document::defaultannotator( AnnotationType::AnnotationType type,
					  const string& st ) const {
    if ( type == AnnotationType::NO_ANN ){
      return "";
    }
    // if ( !st.empty() ){
    //   cerr << "zoek '" << st << "' default annotator " <<  annotationdefaults << endl;
    // }
    map<AnnotationType::AnnotationType,multimap<string,at_t> >::const_iterator mit1 = annotationdefaults.find(type);
    string result;
    if ( mit1 != annotationdefaults.end() ){
      //      cerr << "vind tussen " <<  mit1->second << endl;
      if ( st.empty() ){
	if ( mit1->second.size() == 1 ){
	  result = mit1->second.begin()->second.a;
	  return result;
	}
      }
      else {
	if ( mit1->second.count( st ) == 1 ){
	  map<string,at_t>::const_iterator mit2 = mit1->second.find( st );
	  result = mit2->second.a;
	}
      }
    }
    //    cerr << "get default ==> " << result << endl;
    return result;
  }

  std::string Document::defaultannotatortype( AnnotationType::AnnotationType type,
					      const string& st ) const {
    if ( type == AnnotationType::NO_ANN ){
      return "";
    }
    map<AnnotationType::AnnotationType,multimap<string,at_t> >::const_iterator mit1 = annotationdefaults.find(type);
    string result;
    if ( mit1 != annotationdefaults.end() ){
      if ( st.empty() ){
	if ( mit1->second.size() == 1 )
	  result = mit1->second.begin()->second.t;
	return result;
      }
      else {
	if ( mit1->second.count( st ) == 1 ){
	  map<string,at_t>::const_iterator mit2 = mit1->second.find( st );
	  result = mit2->second.t;
	}
      }
    }
    //  cerr << "get default ==> " << result << endl;
    return result;
  }

  std::string Document::defaultdatetime( AnnotationType::AnnotationType type,
					 const string& st ) const {
    map<AnnotationType::AnnotationType,multimap<string,at_t> >::const_iterator mit1 = annotationdefaults.find(type);
    string result;
    if ( mit1 != annotationdefaults.end() ){
      if ( st.empty() ){
	if ( mit1->second.size() == 1 )
	  result = mit1->second.begin()->second.d;
	return result;
      }
      else {
	if ( mit1->second.count( st ) == 1 ){
	  map<string,at_t>::const_iterator mit2 = mit1->second.find( st );
	  result = mit2->second.d;
	}
      }
    }
    //  cerr << "get default ==> " << result << endl;
    return result;
  }

  void Document::setannotations( xmlNode *node ) const {
    map<AnnotationType::AnnotationType,multimap<string,at_t> >::const_iterator mit = annotationdefaults.begin();
    while ( mit != annotationdefaults.end() ){
      map<string,at_t>::const_iterator it = mit->second.begin();
      while ( it != mit->second.end() ){
	// Find the 'label' 
	string label = toString( mit->first );
	label += "-annotation";
	xmlNode *n = XmlNewNode( foliaNs(), label );
	KWargs args;
	string s = it->second.a;
	if ( !s.empty() )
	  args["annotator"] = s;
	s = it->second.t;
	if ( !s.empty() )
	  args["annotatortype"] = s;
	s = it->second.d;
	if ( !s.empty() )
	  args["datetime"] = s;
	s = it->first;
	if ( s != "undefined" ) // the default
	  args["set"] = s;
	addAttributes( n, args );
	xmlAddChild( node, n );
	++it;
      }
      ++mit;
    }
  }

  void Document::setmetadata( xmlNode *node ) const{
    KWargs atts;
    if ( _metadatatype == NATIVE )
      atts["type"] = "native";
    else if ( _metadatatype == IMDI )
      atts["type"] = "imdi";
    else if ( _metadatatype == CMDI )
      atts["type"] = "cmdi";

    if ( _metadatatype == NATIVE ){
      if ( !_title.empty() )
	atts["title"] = _title;
      if ( !_date.empty() )
	atts["date"] = _date;
      if ( !_language.empty() )
	atts["language"] = _language;
      if ( !_license.empty() )
	atts["license"] = _license;
      if ( !_publisher.empty() )
	atts["publisher"] = _publisher;
      addAttributes( node, atts );
    }
    else if ( _metadatatype == IMDI  ||
	      _metadatatype == CMDI ){
      if ( !_metadatafile.empty() )
	atts["src"] = _metadatafile;
      addAttributes( node, atts );
      xmlAddChild( node, xmlCopyNodeList(metadata) );
    }
  }

  void Document::setstyles( xmlDoc* doc ) const {
    multimap<string,string>::const_iterator it = styles.begin();
    while ( it != styles.end() ){
      string content = "type=\"" + it->first 
	+ "\" href=\"" + it->second + "\"";
      xmlAddChild( (xmlNode*)doc,
		   xmlNewDocPI( doc,
				(const xmlChar*)"xml-stylesheet", 
				(const xmlChar*)content.c_str() ) );
      ++it;
    }
  }

  string Document::toXml( const string& nsLabel, bool kanon ) const {
    string result;
    if ( foliadoc ){
      xmlDoc *outDoc = xmlNewDoc( (const xmlChar*)"1.0" );
      setstyles( outDoc );
      xmlNode *root = xmlNewDocNode( outDoc, 0, (const xmlChar*)"FoLiA", 0 );
      xmlDocSetRootElement( outDoc, root );
      xmlNs *xl = xmlNewNs( root, (const xmlChar *)"http://www.w3.org/1999/xlink", 
			    (const xmlChar *)"xlink" );
      xmlSetNs( root, xl );
      if ( !_foliaNsIn ){
	if ( nsLabel.empty() )
	  _foliaNsOut = xmlNewNs( root, (const xmlChar *)NSFOLIA.c_str(), 0 );
	else
	  _foliaNsOut = xmlNewNs( root,
				  (const xmlChar *)NSFOLIA.c_str(),
				  (const xmlChar*)nsLabel.c_str() );
      }
      else {
	_foliaNsOut = xmlNewNs( root, _foliaNsIn->href, _foliaNsIn->prefix );
      }
      xmlSetNs( root, _foliaNsOut );
      KWargs attribs;
      attribs["_id"] = foliadoc->id(); // sort "id" in front!
      attribs["generator"] = string("libfolia-v") + VERSION;
      if ( !version.empty() )
	attribs["version"] = version;
      addAttributes( root, attribs );

      xmlNode *md = xmlAddChild( root, XmlNewNode( foliaNs(), "metadata" ) );  
      xmlNode *an = xmlAddChild( md, XmlNewNode( foliaNs(), "annotations" ) );
      setannotations( an );
      setmetadata( md );
      vector<FoliaElement*>::const_iterator it= foliadoc->data.begin();
      while ( it != foliadoc->data.end() ){
	xmlAddChild( root, (*it)->xml( true, kanon ) );
	++it;
      }
      xmlChar *buf; int size;
      xmlDocDumpFormatMemoryEnc( outDoc, &buf, &size, "UTF-8", 1 );
      result = string( (const char *)buf, size );
      xmlFree( buf );
      xmlFreeDoc( outDoc );
      _foliaNsOut = 0;
    }
    else
      throw runtime_error( "can't save, no doc" );
    return result;
  }

  vector<vector<Word*> > Document::findwords( const Pattern& pat,
					      const string& args ) const {
    size_t leftcontext = 0;
    size_t rightcontext = 0;
    KWargs kw = getArgs( args );
    string val = kw["leftcontext"];
    if ( !val.empty() )
      leftcontext = stringTo<size_t>(val);
    val = kw["rightcontext"];
    if ( !val.empty() )
      rightcontext = stringTo<size_t>(val);
    vector<vector<Word*> > result;
    vector<Word*> matched;
    if ( pat.regexp )
      throw runtime_error( "regexp not supported yet in patterns" );
    vector<Word*> mywords = words();
    for ( size_t startpos =0; startpos < mywords.size(); ++startpos ){
      // loop over all words
      //    cerr << "outer loop STARTPOS = " << startpos << endl;
      size_t cursor = 0;
      int gap = 0;
      bool goon = true;
      for ( size_t i = startpos; i < mywords.size() && goon ; ++i ){
	//      cerr << "inner LOOP I = " << i << " myword=" << mywords[i] << endl;
	UnicodeString value;
	if ( pat.matchannotation == BASE )
	  value = mywords[i]->text();
	else {
	  vector<FoliaElement *> v = mywords[i]->select( pat.matchannotation );
	  if ( v.size() != 1 ){
	    continue;
	  }
	  value = UTF8ToUnicode(v[0]->cls());
	}
	bool done = false;
	bool flag = false;
	if ( pat.match( value, cursor, gap, done, flag ) ){
	  // cerr << "matched, " << (done?"done":"not done")
	  //      << (flag?" Flagged!":":{") << endl;
	  matched.push_back(mywords[i]);
	  if ( cursor == 0 )
	    startpos = i; // restart search here
	  if ( done ){
	    vector<Word*> keep = matched;
	    //	  cerr << "findnodes() tussenresultaat ==> " << matched << endl;
	    vector<Word*> tmp1;
	    if ( leftcontext > 0 ){
	      tmp1 = matched[0]->leftcontext(leftcontext);
	      //	    cerr << "findnodes() tmp1 ==> " << tmp1 << endl;
	      copy( matched.begin(), matched.end(), back_inserter(tmp1) );
	      //	    cerr << "findnodes() tmp1 na copy ==> " << tmp1 << endl;
	    }
	    else
	      tmp1 = matched;
	    vector<Word*> tmp2;
	    if ( rightcontext > 0 ){
	      tmp2 = matched[matched.size()-1]->rightcontext(rightcontext);
	      //	    cerr << "findnodes() tmp2 ==> " << tmp2 << endl;
	      copy( tmp2.begin(), tmp2.end(), back_inserter(tmp1) );
	      //	    cerr << "findnodes() tmp2 na copy ==> " << tmp2 << endl;
	    }
	    result.push_back(tmp1);
	    //	  cerr << "findnodes() tussenresultaat 2 ==> " << tmp1 << endl;
	    if ( flag ){
	      matched = keep;
	    }
	    else {
	      cursor = 0;
	      matched.clear();
	      goon = false;
	    }
	  }
	}
	else {
	  cursor = 0;
	  matched.clear();
	  goon = false;
	}
      }
    }
    //  cerr << "findnodes() result ==> " << result << endl;
    return result;
  }

  vector<vector<Word*> > Document::findwords( list<Pattern>& pats,
						      const string& args ) const {
    size_t prevsize = 0;
    bool start = true;
    bool unsetwildcards = false;
    set<int> variablewildcards;
    list<Pattern>::const_iterator it = pats.begin();
    int index = 0;
    while ( it != pats.end() ){
      //    cerr << "bekijk patroon : " << *it << endl;
      if ( start ){
	prevsize = it->size();
	start = false;
      }
      else if ( it->size() != prevsize ){
	throw runtime_error( "findnodes(): If multiple patterns are provided, they must all have the same length!" );
      }
      if ( it->variablesize() ){
	if ( index > 0 && variablewildcards.empty() )
	  unsetwildcards = true;
	else {
	  if ( !variablewildcards.empty() && 
	       variablewildcards != it->variablewildcards() ){
	    throw runtime_error("If multiple patterns are provided with variable wildcards, then these wildcards must all be in the same positions!");
	  }
	  variablewildcards = it->variablewildcards();
	}
      }
      else if ( !variablewildcards.empty() )
	unsetwildcards = true;
      ++index;
      ++it;
    }
    if ( unsetwildcards ){
      list<Pattern>::iterator it1 = pats.begin();
      while ( it1 != pats.end() ){
	it1->unsetwild();
	++it1;
      }
    }
    vector<vector<Word*> > result;
    it = pats.begin();
    while ( it != pats.end() ){
      vector<vector<Word*> > res = findwords( *it, args );
      if ( result.empty() )
	result = res;
      else if ( res != result ){
	result.clear();
	break;
      }
      ++it;
    }
    return result;
  }

  Pattern::Pattern( const std::vector<std::string>&v, 
		    const ElementType at,
		    const string& args ):matchannotation(at) {
    regexp = false;
    case_sensitive = false;
    KWargs kw = getArgs( args );
    matchannotationset = kw["matchannotationset"];
    if (kw["regexp"] != "" )
      regexp = stringTo<bool>( kw["regexp"] );
    if (kw["maxgapsize"] != "" )
      maxgapsize = stringTo<int>( kw["maxgapsize"] );
    else
      maxgapsize = 10;
    if ( kw["casesensitive"] != "" )
      case_sensitive = stringTo<bool>( kw["casesensitive"] );
    for ( size_t i=0; i < v.size(); ++i ){
      if ( v[i].find( "regexp('" ) == 0 &&
	   v[i].rfind( "')" ) == v[i].length()-2 ){
	string tmp = v[i].substr( 8, v[i].length() - 10 );
	UnicodeString us = UTF8ToUnicode( tmp );
	UErrorCode u_stat = U_ZERO_ERROR;
	RegexMatcher *mat = new RegexMatcher(us, 0, u_stat);
	if ( U_FAILURE(u_stat) ){
	  throw runtime_error( "failed to create a regexp matcher with '" + tmp + "'" );
	}
	matchers.push_back( mat );
	sequence.push_back( "" );
      }
      else {
	sequence.push_back( UTF8ToUnicode(v[i]) );
	matchers.push_back( 0 );
	if ( !case_sensitive ){
	  sequence[i].toLower();
	}
      }
    }
  }

  Pattern::Pattern( const std::vector<std::string>&v, 
		    const string& args ):matchannotation(BASE) {

    // code duplication 
    // BAD BAD BAD
    //
    regexp = false;
    case_sensitive = false;
    KWargs kw = getArgs( args );
    matchannotationset = kw["matchannotationset"];
    if (kw["regexp"] != "" )
      regexp = stringTo<bool>( kw["regexp"] );
    if (kw["maxgapsize"] != "" )
      maxgapsize = stringTo<int>( kw["maxgapsize"] );
    else
      maxgapsize = 10;
    if ( kw["casesensitive"] != "" )
      case_sensitive = stringTo<bool>( kw["casesensitive"] );
    for ( size_t i=0; i < v.size(); ++i ){
      if ( v[i].find( "regexp('" ) == 0 &&
	   v[i].rfind( "')" ) == v[i].length()-2 ){
	string tmp = v[i].substr( 8, v[i].length() - 10 );
	UnicodeString us = UTF8ToUnicode( tmp );
	UErrorCode u_stat = U_ZERO_ERROR;
	RegexMatcher *mat = new RegexMatcher(us, 0, u_stat);
	if ( U_FAILURE(u_stat) ){
	  throw runtime_error( "failed to create a regexp matcher with '" + tmp + "'" );
	}
	matchers.push_back( mat );
	sequence.push_back( "" );
      }
      else {
	sequence.push_back( UTF8ToUnicode(v[i]) );
	matchers.push_back( 0 );
	if ( !case_sensitive ){
	  sequence[i].toLower();
	}
      }
    }
  }

  Pattern::~Pattern(){
    for ( size_t i=0; i < matchers.size(); ++i ){  
      delete matchers[i];
    }
  }

  inline std::ostream& operator<<( std::ostream& os, const Pattern& p ){
    using TiCC::operator <<;
    os << "pattern: " << p.sequence;
    return os;
  }

  bool Pattern::match( const UnicodeString& us, size_t& pos, int& gap, 
		       bool& done, bool& flag ) const {
    UnicodeString s = us;
    //  cerr << "gap = " << gap << "cursor=" << pos << " vergelijk '" <<  sequence[pos] << "' met '" << us << "'" << endl;
    if ( matchers[pos] ){
      matchers[pos]->reset( s );
      UErrorCode u_stat = U_ZERO_ERROR;
      if ( matchers[pos]->matches( u_stat ) ){
	done = ( ++pos >= sequence.size() );
	return true;
      }
      else {
	++pos;
	return false;
      }
    }
    else {
      if ( !case_sensitive )
	s.toLower();
      if ( sequence[pos] == s || sequence[pos] == "*:1" ){
	done = ( ++pos >= sequence.size() );
	return true;
      }
      else if ( sequence[pos] == "*" ){
	if ( (pos + 1 ) >= sequence.size() ){
	  done = true;
	}
	else if ( sequence[pos+1] == s ){
	  //	cerr << "    but next matched!" << endl;
	  flag = ( ++gap < maxgapsize );
	  if ( !flag ){
	    pos = pos + gap;
	    done = ( ++pos >= sequence.size() );
	  }
	  else {
	    done = true;
	  }
	}
	else if ( ++gap == maxgapsize )
	  ++pos;
	else
	  flag = true;
	return true;
      }
      else {
	++pos;
	return false;
      }
    }
  }

  bool Pattern::variablesize() const {
    for ( size_t i=0; i < sequence.size(); ++i ){
      if ( sequence[i] == "*" ){
	return true;
      }
    }
    return false;
  }

  void Pattern::unsetwild() {
    for ( size_t i=0; i < sequence.size(); ++i ){
      if ( sequence[i] == "*" )
	sequence[i] = "*:1";
    }
  }

  set<int> Pattern::variablewildcards() const {
    set<int> result;
    for ( size_t i=0; i < sequence.size(); ++i ){
      if ( sequence[i] == "*" )
	result.insert( i );
    }
    //  cerr << "variablewildcards() ==> " << result << endl;
    return result;
  }

} // namespace folia
