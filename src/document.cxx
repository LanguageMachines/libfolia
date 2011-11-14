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
#include "folia/document.h"
#include "folia/folia.h"
#include "config.h"

using namespace std;

namespace folia {

  const string FOLIAVERSION = "0.7"; // the FoLiA version we implement
  const string NSFOLIA = "http://ilk.uvt.nl/folia";
  const string NSDCOI = "http://lands.let.ru.nl/projects/d-coi/ns/1.0";
  const string NSIMDI = "http://www.mpi.nl/IMDI/Schema/IMDI";

  Document::Document(){
    init();
  }

  Document::Document( const string& args ){
    init();
    setAttributes( getArgs( args ) );
    foliadoc = new FoLiA( this, args );
  }

  void Document::init(){
    metadatatype = NATIVE;
    metadata = 0;
    xmldoc = 0;
    foliadoc = 0;
    _foliaNs = 0;
    debug = 0;
    version = FOLIAVERSION;
  }

  Document::~Document(){
    xmlFreeDoc( xmldoc );
    delete foliadoc;
    set<AbstractElement*>::const_iterator it = delSet.begin();
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
  
  bool operator==( const AbstractElement& a1, const AbstractElement& a2){
    if ( a1._element_id != a2._element_id )
      return false;
    if ( a1._id != a2._id )
      return false;
    if ( a1._set != a2._set )
      return false;
    if ( a1._cls != a2._cls )
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
    if ( it != kwargs.end() )
      version = it->second;
    else
      version.clear();

    it = kwargs.find( "id" );
    if ( it != kwargs.end() ){
      _id = it->second;
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

  void Document::addDocIndex( AbstractElement* el, const string& s ){
    if ( sindex.find( s ) == sindex.end() ){
      sindex[s] = el;
      iindex.push_back( el );
    }
    else
      throw DuplicateIDError( s );
  }

  bool Document::readFromFile( const string& s ){
    if ( xmldoc ){
      throw runtime_error( "Document is aready initialized" );
      return false;
    }
    xmldoc = xmlReadFile( s.c_str(), 0, 0 );
    if ( xmldoc ){
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
    xmldoc = xmlReadMemory( s.c_str(), s.length(), 0, 0, 0 );
    if ( xmldoc ){
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

  bool Document::save( const string& fn, const string& nsLabel ) {
    ofstream os( fn.c_str() );
    if ( os.good() ) {
      string s = toXml( nsLabel );
      os << s << endl;
      return true;
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

  AbstractElement* Document::index( const string& s ) const {
    map<string,AbstractElement*>::const_iterator it = sindex.find( s );
    if ( it == sindex.end() )
      return 0;
    else
      return it->second;
  }

  AbstractElement* Document::operator []( const string& s ) const {
    return index(s);
  }

  AbstractElement* Document::operator []( size_t i ) const {
    if ( i < iindex.size()-1 )
      return iindex[i+1];
    else
      throw range_error( "Document index out of range" );
  }

  vector<AbstractElement*> Document::paragraphs() const {
    return foliadoc->select( Paragraph_t );
  }

  vector<AbstractElement*> Document::sentences() const {
    static set<ElementType> excludeSet;
    if ( excludeSet.empty() ){
      excludeSet.insert( Quote_t );
    }
    return foliadoc->select( Sentence_t, excludeSet );
  }

  Sentence *Document::sentences( size_t index ) const {
    vector<AbstractElement*> v = sentences();
    if ( index < v.size() ){
      return dynamic_cast<Sentence*>(v[index]);
    }
    else
      throw range_error( "sentences() index out of range" );
  }

  Sentence *Document::rsentences( size_t index ) const {
    vector<AbstractElement*> v = sentences();
    if ( index < v.size() ){
      return dynamic_cast<Sentence*>(v[v.size()-1-index]);
    }
    else
      throw range_error( "rsentences() index out of range" );
  }

  vector<AbstractElement*> Document::words() const {
    static set<ElementType> excludeSet;
    if ( excludeSet.empty() ){
      excludeSet.insert( Original_t );
      excludeSet.insert( Suggestion_t );
      excludeSet.insert( Alternative_t );
      excludeSet.insert( Chunk_t );
      excludeSet.insert( SyntacticUnit_t );
      excludeSet.insert( Entity_t );
    }
    return foliadoc->select( Word_t, excludeSet );
  }

  Word *Document::words( size_t index ) const {
    vector<AbstractElement*> v = words();
    if ( index < v.size() ){
      return dynamic_cast<Word*>(v[index]);
    }
    else
      throw range_error( "words() index out of range" );
  }

  Word *Document::rwords( size_t index ) const {
    vector<AbstractElement*> v = words();
    if ( index < v.size() ){
      return dynamic_cast<Word*>(v[v.size()-1-index]);
    }
    else
      throw range_error( "rwords() index out of range" );
  }

  Paragraph *Document::paragraphs( size_t index ) const {
    vector<AbstractElement*> v = paragraphs();
    if ( index < v.size() ){
      return dynamic_cast<Paragraph*>(v[index]);
    }
    else
      throw range_error( "paragraphs() index out of range" );
  }

  Paragraph *Document::rparagraphs( size_t index ) const {
    vector<AbstractElement*> v = paragraphs();
    if ( index < v.size() ){
      return dynamic_cast<Paragraph*>(v[v.size()-1-index]);
    }
    else
      throw range_error( "rparagraphs() index out of range" );
  }

  AbstractElement *Document::append( AbstractElement *txt ){
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
	annotations.push_back( ts_t( type, "" ) );
	annotationdefaults[type].clear();
	KWargs att = getAttributes( n );
	KWargs::const_iterator it = att.find("set" );
	string s;
	string a;
	string t;
	if ( it != att.end() )
	  s = it->second;
	else
	  runtime_error( "what if no set? " );
	it = att.find( "annotator" );
	if ( it != att.end() )
	  a = it->second;
	it = att.find( "annotatortype" );
	if ( it != att.end() ){
	  if ( it->second == "auto" )
	    t = "auto";
	  else if ( it->second == "manual" )
	    t = "manual";
	  else
	    t = it->second;
	}
	annotationdefaults[type].insert( make_pair(s,at_t( a, t )));
	//      cerr << "inserted [" << toString(type) << "][" << s << "](" << a << "," << t << ")" << endl;
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

  AbstractElement* Document::parseFoliaDoc( xmlNode *root ){
    KWargs att = getAttributes( root );
    if ( att["id"] == "" ){
      throw XmlError("FoLiA Document has no ID!");
      return 0;
    }
    AbstractElement *result = AbstractElement::createElement( this, Name(root) );
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
	  xmlNode *m = p->children;
	  while ( m ){
	    if ( Name(m)  == "METATRANSCRIPT" ){
	      if ( !checkNS( m, NSIMDI ) )
		throw runtime_error( "imdi != imdi " );
	      if ( debug > 1 )
		cerr << "found IMDI" << endl;
	      metadatatype = IMDI;
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
	    AbstractElement *t = AbstractElement::createElement( this, tag );
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

  void Document::getstyles(){
    xmlNode *pnt = xmldoc->children;
    while ( pnt ){
      if ( pnt->type == XML_PI_NODE && Name(pnt) == "xml-stylesheet" ){
	addStyle( (const char*)pnt->content );
      }
      pnt = pnt->next;
    }
  }

  AbstractElement* Document::parseXml( ){
    getstyles();
    xmlNode *root = xmlDocGetRootElement( xmldoc );
    if ( debug > 2 ){
      string dum;
      cerr << "root = " << Name( root ) << endl;
      cerr << "in namespace " << getNS( root, dum ) << endl;
      cerr << "namespace list" << getNSlist( root ) << endl;
    }
    AbstractElement *result = 0;
    if ( root  ){
      try {
	if ( Name( root ) == "FoLiA" &&
	     checkNS( root, NSFOLIA ) ){
	  result = parseFoliaDoc( root );
	}
	else if ( Name( root ) == "DCOI" &&
		  checkNS( root, NSDCOI ) ){
	  cerr << " D-coi is not directly supported";
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
			  const string& st, const string& args ){
    bool found = false;
    list<ts_t>::const_iterator it = annotations.begin();
    while( it != annotations.end() && !found ){
      found = ( it->t == type && it->s == st );
      ++it;
    }
    if ( !found ){
      annotations.push_back( ts_t(type,st) );
    }
    KWargs kw = getArgs( args );
    string a = kw["annotator"];
    string t = kw["annotatortype"];
    annotationdefaults[type].insert( make_pair(st, at_t(a,t) ) );
  }

  AbstractElement* Document::addNode( ElementType et, const KWargs& kwargs ){
    AbstractElement *res = AbstractElement::createElement( this, toString(et) );
    res->setAttributes( kwargs );
    foliadoc->append( res );
    return res;
  }

  bool Document::isDeclared( const string& s, 
			     AnnotationType::AnnotationType type ){
    map<AnnotationType::AnnotationType,map<string,at_t> >::const_iterator mit1 = annotationdefaults.find(type);
    if ( mit1 != annotationdefaults.end() ){
      map<string,at_t>::const_iterator mit2 = mit1->second.find(s);
      return mit2 != mit1->second.end();
    }
    return false;
  }

  string Document::defaultset( AnnotationType::AnnotationType type ) const {
    // search a set. it must be unique. Otherwise return ""
    map<AnnotationType::AnnotationType,map<string,at_t> >::const_iterator mit1 = annotationdefaults.find(type);
    string result;
    if ( mit1 != annotationdefaults.end() ){
      if ( mit1->second.size() == 1 )
	result = mit1->second.begin()->first;
    }
    //  cerr << "defaultset ==> " << result << endl;
    return result;
  }

  std::string Document::defaultannotator( AnnotationType::AnnotationType type,
					  const string& st,
					  bool noThrow ) const {
    map<AnnotationType::AnnotationType,map<string,at_t> >::const_iterator mit1 = annotationdefaults.find(type);
    string result;
    if ( mit1 != annotationdefaults.end() ){
      if ( st.empty() )
	result = mit1->second.begin()->second.a;
      else {
	map<string,at_t>::const_iterator mit2 = mit1->second.find( st );
	if ( mit2 != mit1->second.end() ){
	  result = mit2->second.a;
	}
      }
    }
    //  cerr << "get default ==> " << result << endl;
    if ( !noThrow && result.empty() )
      throw NoDefaultError( "defaultannotator" );
    return result;
  }

  std::string Document::defaultannotatortype( AnnotationType::AnnotationType type,
					      const string& st,
					      bool noThrow ) const {
    map<AnnotationType::AnnotationType,map<string,at_t> >::const_iterator mit1 = annotationdefaults.find(type);
    string result;
    if ( mit1 != annotationdefaults.end() ){
      if ( st.empty() )
	result = mit1->second.begin()->second.t;
      else {
	map<string,at_t>::const_iterator mit2 = mit1->second.find( st );
	if ( mit2 != mit1->second.end() ){
	  result = mit2->second.t;
	}
      }
    }
    //  cerr << "get default ==> " << result << endl;
    if ( !noThrow && result.empty() )
      throw NoDefaultError( "defaultannotationtype" );
    return result;
  }

  void Document::setannotations( xmlNode *node ) const{
    list<ts_t>::const_iterator it = annotations.begin();
    while ( it != annotations.end() ){
      // Find the 'label' 
      string label = toString( it->t );
      label += "-annotation";
      xmlNode *n = xmlAddChild( node, newXMLNode( _foliaNs, label ) );
      KWargs args;
      string s = defaultannotator( it->t, "", true );
      if ( !s.empty() )
	args["annotator"] = s;
      s = defaultannotatortype( it->t, "", true );
      if ( !s.empty() )
	args["annotatortype"] = s;
      s = defaultset( it->t );
      if ( !s.empty() )
	args["set"] = s;
      addAttributes( n, args );
      ++it;
    } 
  }

  void Document::setmetadata( xmlNode *node ) const{
    KWargs atts;
    if ( metadatatype == NATIVE )
      atts["type"] = "native";
    else if ( metadatatype == IMDI )
      atts["type"] = "imdi";
    else if ( metadatatype == CMDI )
      atts["type"] = "cmdi";

    if ( metadatatype == NATIVE ){
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
    }
    else if ( metadatatype == IMDI ){
      xmlAddChild( node, xmlCopyNodeList(metadata) );
    }
    else if ( metadatatype == CMDI ){
      // ????
    }
    addAttributes( node, atts );
  }

  void Document::setstyles( xmlDoc* doc ) const {
    for ( size_t i=0; i < styles.size(); ++i ){
      xmlAddChild( (xmlNode*)doc,
		   xmlNewDocPI( doc,
				(const xmlChar*)"xml-stylesheet", 
				(const xmlChar*)styles[i].c_str() ) );
    }
  }

  string Document::toXml( const string& nsLabel ) const {
    string result;
    if ( foliadoc ){
      xmlDoc *outDoc = xmlNewDoc( (const xmlChar*)"1.0" );
      setstyles( outDoc );
      xmlNode *root = xmlNewDocNode( outDoc, 0, (const xmlChar*)"FoLiA", 0 );
      xmlDocSetRootElement( outDoc, root );
      xmlNs *xl = xmlNewNs( root, (const xmlChar *)"http://www.w3.org/1999/xlink", 
			    (const xmlChar *)"xlink" );
      xmlSetNs( root, xl );
      if ( !_foliaNs ){
	if ( nsLabel.empty() )
	  _foliaNs = xmlNewNs( root, (const xmlChar *)NSFOLIA.c_str(), 0 );
	else
	  _foliaNs = xmlNewNs( root,
			       (const xmlChar *)NSFOLIA.c_str(),
			       (const xmlChar*)nsLabel.c_str() );
      }
      xmlSetNs( root, _foliaNs );
      KWargs attribs;
      attribs["_id"] = foliadoc->id(); // sort "id" in front!
      attribs["generator"] = string("libfolia-v") + VERSION;
      if ( !version.empty() )
	attribs["version"] = version;
      addAttributes( root, attribs );

      xmlNode *md = xmlAddChild( root,  newXMLNode( _foliaNs, "metadata" ) );  
      xmlNode *an = xmlAddChild( md,  newXMLNode( _foliaNs, "annotations" ) );
      setannotations( an );
      setmetadata( md );
      vector<AbstractElement*>::const_iterator it= foliadoc->data.begin();
      while ( it != foliadoc->data.end() ){
	xmlAddChild( root, (*it)->xml( true ) );
	++it;
      }
      xmlChar *buf; int size;
      xmlDocDumpFormatMemory( outDoc, &buf, &size, 1 );
      result = string( (const char *)buf, size );
      xmlFree( buf );
      xmlFreeDoc( outDoc );
      _foliaNs = 0;
    }
    else
      throw runtime_error( "can't save, no doc" );
    return result;
  }

  vector<vector<AbstractElement*> > Document::findwords( const Pattern& pat,
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
    vector<vector<AbstractElement*> > result;
    vector<AbstractElement*> matched;
    if ( pat.regexp )
      throw runtime_error( "regexp not supported yet in patterns" );
    vector<AbstractElement*> mywords = words();
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
	  vector<AbstractElement *> v = mywords[i]->select( pat.matchannotation );
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
	    vector<AbstractElement *> keep = matched;
	    //	  cerr << "findnodes() tussenresultaat ==> " << matched << endl;
	    vector<AbstractElement *> tmp1;
	    if ( leftcontext > 0 ){
	      tmp1 = matched[0]->leftcontext(leftcontext);
	      //	    cerr << "findnodes() tmp1 ==> " << tmp1 << endl;
	      copy( matched.begin(), matched.end(), back_inserter(tmp1) );
	      //	    cerr << "findnodes() tmp1 na copy ==> " << tmp1 << endl;
	    }
	    else
	      tmp1 = matched;
	    vector<AbstractElement *> tmp2;
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

  vector<vector<AbstractElement*> > Document::findwords( list<Pattern>& pats,
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
    vector<vector<AbstractElement*> > result;
    it = pats.begin();
    while ( it != pats.end() ){
      vector<vector<AbstractElement*> > res = findwords( *it, args );
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
