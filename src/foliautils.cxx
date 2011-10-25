#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <stdexcept>
#include <algorithm> 
#include "folia/document.h"
#include "folia/folia.h"

using namespace std;

namespace folia {

  UnicodeString UTF8ToUnicode( const string& s ){
    return UnicodeString( s.c_str(), s.length(), "UTF-8" );
  }

  string UnicodeToUTF8( const UnicodeString& s ){
    string result;
    int len = s.length();
    if ( len > 0 ){
      char *buf = new char[len*6+1];
      s.extract( 0, len, buf, len*6, "UTF-8" );
      result = buf;
      delete [] buf;
    }
    return result;
  }

  double toDouble( const string& s ){
    char *end;
    double res = strtod( s.c_str(), &end );
    return res;
  }

  string toString( const double d ){
    stringstream dummy;
    if ( !( dummy << d ) ) {
      throw( runtime_error( "conversion to string failed" ) );
    }
    return dummy.str();
  }

  string toString( const AnnotationType::AnnotationType& at ){
    string result;
    switch ( at ) {
    case AnnotationType::NO_ANN:
      result = "NoNe";
      break; 
    case AnnotationType::TOKEN:
      result = "token";
      break; 
    case AnnotationType::DIVISION:
      result = "division";
      break; 
    case AnnotationType::POS: 
      result = "pos";
      break; 
    case AnnotationType::LEMMA:
      result = "lemma";
      break; 
    case AnnotationType::EVENT:
      result = "event";
      break; 
    case AnnotationType::DOMEIN:
      result = "domain";
      break; 
    case AnnotationType::SENSE: 
      result = "sense";
      break; 
    case AnnotationType::SYNTAX: 
      result = "syntax";
      break;
    case AnnotationType::CHUNKING: 
      result = "chunking";
      break; 
    case AnnotationType::ENTITY: 
      result = "entity";
      break;
    case AnnotationType::SUBENTITY: 
      result = "subentity";
      break;
    case AnnotationType::CORRECTION: 
      result = "correction";
      break;
    case AnnotationType::ERRORDETECTION: 
      result = "errordetection";
      break;
    case AnnotationType::ALTERNATIVE: 
      result = "alternative";
      break; 
    case AnnotationType::PHON:
      result = "phon";
      break;
    case AnnotationType::SUBJECTIVITY:
      result = "subjectivity";
      break;
    case AnnotationType::MORPHOLOGICAL:
      result = "morphological";
      break;
    default:
      throw ValueError( " unknown translation for annotation" + 
			folia::toString(int(at)) );
    };
    return result;
  }

  int to_lower( const int& i ){ return tolower(i); }
  int to_upper( const int& i ){ return toupper(i); }

  string lowercase( const string& in ){
    string s = in;
    transform( s.begin(), s.end(), s.begin(), to_lower );
    return s;
  }

  string uppercase( const string& in ){
    string s = in;
    transform( s.begin(), s.end(), s.begin(), to_upper );
    return s;
  }

  AnnotationType::AnnotationType stringToAT( const string& at ){
    if ( at == "token" )
      return AnnotationType::TOKEN;
    if ( at == "division" )
      return AnnotationType::DIVISION;
    if ( at == "pos" )
      return AnnotationType::POS;
    if ( at == "lemma" )
      return AnnotationType::LEMMA;
    if ( at == "event" )
      return AnnotationType::EVENT;
    if ( at == "domain" )
      return AnnotationType::DOMEIN;
    if ( at == "sense" )
      return AnnotationType::SENSE;
    if ( at == "syntax" )
      return AnnotationType::SYNTAX;
    if ( at == "chunking" )
      return AnnotationType::CHUNKING;
    if ( at == "entity" )
      return AnnotationType::ENTITY;
    if ( at == "subentity" )
      return AnnotationType::SUBENTITY;
    if ( at == "correction" )
      return AnnotationType::CORRECTION; 
    if ( at == "errordetection" )
      return AnnotationType::ERRORDETECTION; 
    if ( at == "alternative" )
      return AnnotationType::ALTERNATIVE;
    if ( at == "phon" )
      return AnnotationType::PHON;
    if ( at == "subjectivity" )
      return AnnotationType::SUBJECTIVITY;
    if ( at == "morphological" )
      return AnnotationType::MORPHOLOGICAL;
    throw ValueError( " unknown translation for attribute: " + at );
  }

  string toString( const ElementType& et ) {
    string result;
    switch( et ){
    case BASE: result = "BASE"; break;
    case Text_t: result = "text"; break;
    case Event_t: result = "event"; break;
    case TextContent_t: result = "t"; break;
    case LineBreak_t: result = "br"; break;
    case WhiteSpace_t: result = "whitespace"; break;
    case Word_t: result = "word"; break;
    case PlaceHolder_t: result = "placeholder"; break;
    case Sentence_t: result = "s"; break;
    case Paragraph_t: result = "p"; break;
    case Division_t: result = "div"; break;
    case Head_t: result = "head"; break;
    case Description_t: result = "desc"; break;
    case Gap_t: result = "gap"; break;
    case Content_t: result = "content"; break;
    case List_t: result = "list"; break;
    case ListItem_t: result = "listitem"; break;
    case Figure_t: result = "figure"; break;
    case Caption_t: result = "caption"; break;
    case Quote_t: result = "quote"; break;
    case Pos_t: result = "pos"; break;
    case Current_t: result = "current"; break;
    case New_t: result = "new"; break;
    case Original_t: result = "original"; break;
    case Suggestion_t: result = "suggestion"; break;
    case Lemma_t: result = "lemma"; break;
    case Phon_t: result = "phon"; break; 
    case Domain_t: result = "domain"; break; 
    case Sense_t: result = "sense"; break; 
    case Morphology_t: result = "morphology"; break;
    case Morpheme_t: result = "morpheme"; break;
    case Correction_t: result = "correction"; break;
    case ErrorDetection_t: result = "errordetection"; break;
    case Annolay_t: result = "annotationlayer"; break; 
    case SyntaxLayer_t: result = "syntax"; break; 
    case SyntacticUnit_t: result = "su"; break; 
    case WordReference_t: result = "wref"; break; 
    case Chunk_t: result = "chunk"; break; 
    case Chunking_t: result = "chunking"; break; 
    case Entity_t: result = "entity"; break;
    case Entities_t: result = "entities"; break;
    case Subentity_t: result = "subentity"; break;
    case Subentities_t: result = "subentities"; break;
    case Alternative_t: result = "alternative"; break; 
    case AltLayers_t: result = "altlayers"; break;
    case Feature_t: result = "feature"; break;
    case SynsetFeature_t: result = "synset"; break;
    case ActorFeature_t: result = "actor"; break;
    default:
      result = "Unknown Elementtype " + folia::toString( int(et) );
    }
    return result;
  }

  AbstractElement *AbstractElement::createElement( Document *doc, 
						   const string& tag ){
    //factory;
    if ( tag == "FoLiA" ){
      return new FoLiA( doc );
    }
    if ( tag == "DCOI" ){
      return new DCOI( doc );
    }
    if ( tag == "text" ){
      return new Text( doc );
    }
    if ( tag == "event" ){
      return new Event( doc );
    }
    if ( tag == "s" ){
      return new Sentence( doc );
    }
    if ( tag == "t" ){
      return new TextContent( doc );
    }
    if ( tag == "br" ){
      return new LineBreak( doc );
    }
    if ( tag == "whitespace" ){
      return new WhiteSpace( doc );
    }
    if ( tag == "w" ){
      return new Word( doc );
    }
    if ( tag == "figure" ){
      return new Figure( doc );
    }
    if ( tag == "caption" ){
      return new Caption( doc );
    }
    if ( tag == "listitem" ){
      return new ListItem( doc );
    }
    if ( tag == "list" ){
      return new List( doc );
    }
    if ( tag == "p" ){
      return new Paragraph( doc );
    }
    if ( tag == "new" ){
      return new NewElement( doc );
    }
    if ( tag == "original" ){
      return new Original( doc );
    }
    if ( tag == "suggestion" ){
      return new Suggestion( doc );
    }
    if ( tag == "head" ){
      return new Head( doc );
    }
    if ( tag == "desc" ){
      return new Description( doc );
    }
    if ( tag == "gap" ){
      return new Gap( doc );
    }
    if ( tag == "content" ){
      return new Content( doc );
    }
    if ( tag == "div" ){
      return new Division( doc );
    }
    if ( tag == "pos" ){
      return new PosAnnotation( doc );
    }
    if ( tag == "lemma" ){
      return new LemmaAnnotation( doc );
    }
    if ( tag == "phon" ){
      return new PhonAnnotation( doc );
    }
    if ( tag == "domain" ){
      return new DomainAnnotation( doc );
    }
    if ( tag == "sense" ){
      return new SenseAnnotation( doc );
    }
    if ( tag == "syntax" ){
      return new SyntaxLayer( doc );
    }
    if ( tag == "subjectivity" ){
      return new SubjectivityAnnotation( doc );
    }
    if ( tag == "chunk" ){
      return new Chunk( doc );
    }
    if ( tag == "chunking" ){
      return new ChunkingLayer( doc );
    }
    if ( tag == "entity" ){
      return new Entity( doc );
    }
    if ( tag == "entities" ){
      return new EntitiesLayer( doc );
    }
    if ( tag == "subentity" ){
      return new Subentity( doc );
    }
    if ( tag == "subentities" ){
      return new SubentitiesLayer( doc );
    }
    if ( tag == "su" ){
      return new SyntacticUnit( doc );
    }
    if ( tag == "wref" ){
      return new WordReference( doc );
    }
    if ( tag == "correction" ){
      return new Correction( doc );
    }
    if ( tag == "errordetection" ){
      return new ErrorDetection( doc );
    }
    if ( tag == "morphology" ){
      return new MorphologyLayer( doc );
    }
    if ( tag == "morpheme" ){
      return new Morpheme( doc );
    }
    if ( tag == "feat" ){
      return new Feature( doc );
    }
    if ( tag == "synset" ){
      return new SynsetFeature( doc );
    }
    if ( tag == "actor" ){
      return new ActorFeature( doc );
    }
    if ( tag == "quote" ){
      return new Quote( doc );
    }
    else {
      //    throw runtime_error( "unknown tag <" + tag + ">" );
      cerr << "unknown tag <" << tag << ">" << endl;
    }
    return 0;
  }


  string compress( const string& s ){
    // remove leading and trailing spaces from a string
    string result;
    if ( !s.empty() ){
      string::const_iterator b_it = s.begin();
      while ( b_it != s.end() && isspace( *b_it ) ) ++b_it;
      string::const_iterator e_it = s.end();
      --e_it;
      while ( e_it != s.begin() && isspace( *e_it ) ) --e_it;
      if ( b_it <= e_it )
	result = string( b_it, e_it+1 );
    }
    return result;
  }

  size_t split_at( const string& src, vector<string>& results, 
		   const string& sep ){
    // split a string into substrings, using seps as seperator
    // silently skip empty entries (e.g. when two or more seperators co-incide)
    // also purges trailing and leading spaces
    results.clear();
    string::size_type pos = 0;
    string res;
    while ( pos != string::npos ){
      string::size_type p = src.find( sep, pos );
      if ( p == string::npos ){
	res = src.substr( pos );
	pos = p;
      }
      else {
	res = src.substr( pos, p - pos );
	pos = p + sep.length();
      }
      res = folia::compress( res );
      if ( !res.empty() )
	results.push_back( res );
    }
    return results.size();
  }

  KWargs getArgs( const std::string& s ){
    KWargs result;
    bool quoted = false;
    bool parseatt = true;
    vector<string> parts;
    string att;
    string val;
    //  cerr << "getArgs \\" << s << "\\" << endl;

    for ( size_t i=0; i < s.size(); ++i ){
      //       cerr << "bekijk " << s[i] << endl;
      //       cerr << "quoted = " << (quoted?"YES":"NO") << " parseatt = " << (parseatt?"YES":"NO") << endl;
      if ( s[i] == '\'' ){
	if ( quoted ){
	  if ( att.empty() || val.empty() )
	    throw ArgsError( s + ", (''?)" );
	  result[att] = val;
	  //	cerr << "added " << att << "='" << val << "'" << endl;
	  att.clear();
	  val.clear();
	  quoted = false;
	}
	else {
	  quoted = true;
	}
      }
      else if ( s[i] == '=' ) {
	if ( parseatt ){
	  parseatt = false;
	}
	else if ( quoted )
	  val += s[i];
	else
	  throw ArgsError( s + ", stray '='?" );
      }
      else if ( s[i] == ',' ){
	if ( quoted )
	  val += s[i];
	else if ( !parseatt ){
	  parseatt = true;
	}
	else
	  throw ArgsError( s + ", stray '='?" );
      }
      else if ( s[i] == ' ' ){
	if ( quoted )
	  val += s[i];
      }
      else if ( parseatt )
	att += s[i];
      else if ( quoted )
	val += s[i];
      else
	throw ArgsError( s + ", unquoted value?" );
      //    cerr << "att = '" << att << "'" << endl;
      //    cerr << "val = '" << val << "'" << endl;
    }
    if ( quoted )
      throw ArgsError( s + ", unbalanced '?" );
    return result;
  }

  std::string toString( const KWargs& args ){
    string result;
    KWargs::const_iterator it = args.begin();
    while ( it != args.end() ){
      result += it->first + "='" + it->second + "'";
      ++it;
      if ( it != args.end() )
	result += ",";
    }
    return result;
  }

  xmlNode *newXMLNode( xmlNs *ns,  const string& elem ){
    return xmlNewNode( ns, (const xmlChar*)elem.c_str() );
  }

  KWargs getAttributes( const xmlNode *node ){
    KWargs atts;
    if ( node ){
      xmlAttr *a = node->properties;
      while ( a ){
	atts[(char*)a->name] = (char *)a->children->content;
	a = a->next;
      }
    }
    return atts;
  }

  void addAttributes( xmlNode *node, const KWargs& attribs ){
    KWargs::const_iterator it = attribs.begin();
    while ( it != attribs.end() ){
      //    cerr << "addAttributes(" << it->first << ", " << it->second << ")" << endl;
      if ( it->first == "_id" ){ // id is special
	xmlNewNsProp( node, 0, XML_XML_ID,  (const xmlChar *)it->second.c_str() );
      }
      else {
	xmlNewNsProp( node, 0, 
		      (const xmlChar*)it->first.c_str(), 
		      (const xmlChar*)it->second.c_str() );
      }
      ++it;
    }
  }

  string Name( const xmlNode *node ){
    string result;
    if ( node ){
      result = (char *)node->name;
    }
    return result;
  }

  string XmlContent( const xmlNode *node ){
    string result;
    if ( node ){
      xmlChar *tmp = xmlNodeListGetString( node->doc, node->children, 1 );
      if ( tmp ){
	result = string( (char *)tmp );
	xmlFree( tmp );
      }
    }
    return result;
  }

  string getNS( const xmlNode *node, string& prefix ){
    string result;
    prefix = "";
    xmlNs *p = node->ns;
    if ( p ){
      if ( p->prefix ){
	prefix = (char *)p->prefix;
      }
      result = (char *)p->href;
    }
    return result;
  }

  map<string,string> getNSlist( const xmlNode *node ){
    map<string,string> result;
    xmlNs *p = node->ns;
    while ( p ){
      string pre;
      string val;
      if ( p->prefix ){
	pre = (char *)p->prefix;
      }
      val = (char *)p->href;
      result[pre] = val;
      p = p->next;
    }
    return result;
  }

  list<xmlNode*> FindLocal( xmlXPathContext* ctxt, 
			    const string& xpath ){
    list<xmlNode*> nodes;
    xmlXPathObject* result = xmlXPathEval((xmlChar*)xpath.c_str(), ctxt);
    if ( result ){
      if (result->type != XPATH_NODESET) {
	xmlXPathFreeObject(result);
	throw runtime_error( "sorry, only nodeset result types supported for now." );
	return nodes;
      }
      xmlNodeSet* nodeset = result->nodesetval;
      if ( nodeset ){
	for (int i = 0; i != nodeset->nodeNr; ++i)
	  nodes.push_back(nodeset->nodeTab[i]);
      }
      else {
	throw( runtime_error( "FindLocal: Missing nodeset" ) );
      }
      xmlXPathFreeObject(result);
    }
    else {
      throw runtime_error( "Invalid Xpath: '" + xpath + "'" );
    }
    return nodes;
  }

  list<xmlNode*> FindNodes( xmlNode* node,
			    const string& xpath ){
    xmlXPathContext* ctxt = xmlXPathNewContext( node->doc );
    ctxt->node = node;
    ctxt->namespaces = xmlGetNsList( node->doc, ctxt->node );
    ctxt->nsNr = 0;
    if (ctxt->namespaces != 0 ) {
      while (ctxt->namespaces[ctxt->nsNr] != 0 ){
	ctxt->nsNr++;
      }
    }
    list<xmlNode*> nodes = FindLocal( ctxt, xpath );
    if (ctxt->namespaces != NULL)
      xmlFree(ctxt->namespaces);
    xmlXPathFreeContext(ctxt);
    return nodes;
  }

  xmlNode *xPath( xmlNode *node, const string& xpath ){
    // try to find a path, but it may not be there...
    // if there are more, just return the first

    list<xmlNode*> srch = FindNodes( node, xpath );
    xmlNode *result = 0;
    if ( srch.size() != 0 ){
      result = srch.front();
    }
    return result;
  }

  int toMonth( const string& ms ){
    int result = 0;
    try {
      result = stringTo<int>( ms );
      return result - 1;
    }
    catch( exception ){
      string m = lowercase( ms );
      if ( m == "jan" )
	return 0;
      else if ( m == "feb" )
	return 1;
      else if ( m == "mar" )
	return 2;
      else if ( m == "apr" )
	return 3;
      else if ( m == "may" )
	return 4;
      else if ( m == "jun" )
	return 5;
      else if ( m == "jul" )
	return 6;
      else if ( m == "aug" )
	return 7;
      else if ( m == "sep" )
	return 8;
      else if ( m == "oct" )
	return 9;
      else if ( m == "nov" )
	return 10;
      else if ( m == "dec" )
	return 11;
      else
	throw runtime_error( "invalid month: " + m );
    }
  }

  tm *parseDate( const string& s ){
    //  cerr << "try to read a date-time " << s << endl;
    vector<string> date_time;
    size_t num = split_at( s, date_time, "T");
    if ( num == 0 ){
      num = split_at( s, date_time, " ");
      if ( num == 0 ){
	cerr << "failed to read a date-time " << s << endl;
	return 0;
      }
    }
    //  cerr << "found " << num << " parts" << endl;
    tm *time = new tm();
    if ( num == 1 || num == 2 ){
      //    cerr << "parse date " << date_time[0] << endl;
      vector<string> date_parts;
      size_t dnum = split_at( date_time[0], date_parts, "-" );
      switch ( dnum ){
      case 3: {
	int year = stringTo<int>( date_parts[0] );
	time->tm_year = year-1900;
      }
      case 2: {
	int mon = toMonth( date_parts[1] );
	time->tm_mon = mon;
      }
      case 1: {
	int mday = stringTo<int>( date_parts[2] );
	time->tm_mday = mday;
      }
	break;
      default:
	cerr << "failed to read a date from " << date_time[0] << endl;
	return 0;
      }
    }
    if ( num == 2 ){
      //    cerr << "parse time " << date_time[1] << endl;
      vector<string> date_parts;
      num = split_at( date_time[1], date_parts, ":" );
      switch ( num ){
      case 4:
	// ignore
      case 3: {
	int hour = stringTo<int>( date_parts[0] );
	time->tm_hour = hour;
      }
      case 2: {
	int min = stringTo<int>( date_parts[1] );
	time->tm_min = min;
      }
      case 1: {
	int sec = stringTo<int>( date_parts[2] );
	time->tm_sec = sec;
      }
	break;
      default:
	cerr << "failed to read a time from " << date_time[1] << endl;
	return 0;
      }
    }
    //  cerr << "read _date time = ";
    // char buf[100];
    // strftime( buf, 100, "%Y-%b-%d %X", time );
    // cerr << buf << endl;
    return time;
  }

} //namespace folia
