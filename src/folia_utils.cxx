/*
  Copyright (c) 2006 - 2016
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

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <stdexcept>
#include <algorithm>
#include "ticcutils/StringOps.h"
#include "ticcutils/XMLtools.h"
#include "ticcutils/PrettyPrint.h"
#include "libfolia/folia.h"

using namespace std;
using namespace TiCC;

namespace folia {

  UnicodeString UTF8ToUnicode( const string& s ){
    return UnicodeString::fromUTF8( s );
  }

  string UnicodeToUTF8( const UnicodeString& s ){
    string result;
    s.toUTF8String(result);
    return result;
  }

  FoliaElement *FoliaImpl::private_createElement( ElementType et ){
    switch ( et ){
    case BASE:
      return new FoLiA();
    case Text_t:
      return new Text();
    case Speech_t:
      return new Speech();
    case Utterance_t:
      return new Utterance();
    case ForeignData_t:
      return new ForeignData();
    case Entry_t:
      return new Entry();
    case Example_t:
      return new Example();
    case Term_t:
      return new Term();
    case Definition_t:
      return new Definition();
    case PhonContent_t:
      return new PhonContent();
    case Word_t:
      return new Word();
    case String_t:
      return new String();
    case Event_t:
      return new Event();
    case TimeSegment_t:
      return new TimeSegment();
    case TimingLayer_t:
      return new TimingLayer();
    case Sentence_t:
      return new Sentence();
    case TextContent_t:
      return new TextContent();
    case Linebreak_t:
      return new Linebreak();
    case Whitespace_t:
      return new Whitespace();
    case Figure_t:
      return new Figure();
    case Caption_t:
      return new Caption();
    case Label_t:
      return new Label();
    case List_t:
      return new List();
    case ListItem_t:
      return new ListItem();
    case Paragraph_t:
      return new Paragraph();
    case New_t:
      return new New();
    case Original_t:
      return new Original();
    case Current_t:
      return new Current();
    case Suggestion_t:
      return new Suggestion();
    case Head_t:
      return new Head();
    case Table_t:
      return new Table();
    case TableHead_t:
      return new TableHead();
    case Cell_t:
      return new Cell();
    case Row_t:
      return new Row();
    case LangAnnotation_t:
      return new LangAnnotation();
    case XmlComment_t:
      return new XmlComment();
    case XmlText_t:
      return new XmlText();
    case External_t:
      return new External();
    case Note_t:
      return new Note();
    case Reference_t:
      return new Reference();
    case Description_t:
      return new Description();
    case Gap_t:
      return new Gap();
    case Content_t:
      return new Content();
    case Metric_t:
      return new Metric();
    case Division_t:
      return new Division();
    case PosAnnotation_t:
      return new PosAnnotation();
    case LemmaAnnotation_t:
      return new LemmaAnnotation();
    case PhonologyLayer_t:
      return new PhonologyLayer();
    case Phoneme_t:
      return new Phoneme();
    case DomainAnnotation_t:
      return new DomainAnnotation();
    case SenseAnnotation_t:
      return new SenseAnnotation();
    case SyntaxLayer_t:
      return new SyntaxLayer();
    case SubjectivityAnnotation_t:
      return new SubjectivityAnnotation();
    case Chunk_t:
      return new Chunk();
    case ChunkingLayer_t:
      return new ChunkingLayer();
    case Entity_t:
      return new Entity();
    case EntitiesLayer_t:
      return new EntitiesLayer();
    case SemanticRolesLayer_t:
      return new SemanticRolesLayer();
    case SemanticRole_t:
      return new SemanticRole();
    case CoreferenceLayer_t:
      return new CoreferenceLayer();
    case CoreferenceLink_t:
      return new CoreferenceLink();
    case CoreferenceChain_t:
      return new CoreferenceChain();
    case Alternative_t:
      return new Alternative();
    case PlaceHolder_t:
      return new PlaceHolder();
    case AlternativeLayers_t:
      return new AlternativeLayers();
    case SyntacticUnit_t:
      return new SyntacticUnit();
    case WordReference_t:
      return new WordReference();
    case Correction_t:
      return new Correction();
    case ErrorDetection_t:
      return new ErrorDetection();
    case MorphologyLayer_t:
      return new MorphologyLayer();
    case Morpheme_t:
      return new Morpheme();
    case Feature_t:
      return new Feature();
    case BegindatetimeFeature_t:
      return new BegindatetimeFeature();
    case EnddatetimeFeature_t:
      return new EnddatetimeFeature();
    case SynsetFeature_t:
      return new SynsetFeature();
    case ActorFeature_t:
      return new ActorFeature();
    case HeadFeature_t:
      return new HeadFeature();
    case ValueFeature_t:
      return new ValueFeature();
    case TimeFeature_t:
      return new TimeFeature();
    case ModalityFeature_t:
      return new ModalityFeature();
    case FunctionFeature_t:
      return new FunctionFeature();
    case LevelFeature_t:
      return new LevelFeature();
    case Quote_t:
      return new Quote();
    case DependenciesLayer_t:
      return new DependenciesLayer();
    case Dependency_t:
      return new Dependency();
    case DependencyDependent_t:
      return new DependencyDependent();
    case Headspan_t:
      return new Headspan();
    case ComplexAlignmentLayer_t:
      return new ComplexAlignmentLayer();
    case ComplexAlignment_t:
      return new ComplexAlignment();
    case Alignment_t:
      return new Alignment();
    case AlignReference_t:
      return new AlignReference();
    case TextMarkupString_t:
      return new TextMarkupString();
    case TextMarkupGap_t:
      return new TextMarkupGap();
    case TextMarkupCorrection_t:
      return new TextMarkupCorrection();
    case TextMarkupError_t:
      return new TextMarkupError();
    case TextMarkupStyle_t:
      return new TextMarkupStyle();
    case Part_t:
      return new Part();
    case AbstractSpanAnnotation_t:
    case AbstractSpanRole_t:
    case AbstractAnnotationLayer_t:
    case AbstractTextMarkup_t:
    case AbstractTokenAnnotation_t:
    case AbstractStructureElement_t:
    case AbstractCorrectionChild_t:
      throw ValueError( "you may not create an abstract node of type "
			+ TiCC::toString(int(et)) + ")" );
    default:
      throw ValueError( "createElement: unknown elementtype("
			+ TiCC::toString(int(et)) + ")" );
    }
    return 0;
  }

  FoliaElement *FoliaImpl::createElement( const string& tag,
					  Document *doc ){

    ElementType et = BASE;
    try {
      et = stringToET( tag );
    }
    catch ( ValueError& e ){
      cerr << e.what() << endl;
      return 0;
    }
    FoliaElement *el = private_createElement( et );
    if ( doc ){
      el->assignDoc( doc );
    }
    return el;
  }

  FoliaElement *FoliaImpl::createElement( ElementType et,
					  Document *doc ){
    FoliaElement *el = private_createElement( et );
    if ( doc ){
      el->assignDoc( doc );
    }
    return el;
  }

  KWargs getArgs( const string& s ){
    KWargs result;
    bool quoted = false;
    bool parseatt = true;
    bool escaped = false;
    vector<string> parts;
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
	    result[att] = val;
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
	if ( quoted )
	  val += let;
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
    if ( quoted )
      throw ArgsError( s + ", unbalanced '?" );
    return result;
  }

  string toString( const KWargs& args ){
    string result;
    auto it = args.begin();
    while ( it != args.end() ){
      result += it->first + "='" + it->second + "'";
      ++it;
      if ( it != args.end() )
	result += ",";
    }
    return result;
  }

  KWargs getAttributes( const xmlNode *node ){
    KWargs atts;
    if ( node ){
      xmlAttr *a = node->properties;
      while ( a ){
	if ( a->atype == XML_ATTRIBUTE_ID && string((char*)a->name) == "id" ){
	  atts["_id"] = (char *)a->children->content;
	}
	else if ( a->ns == 0 || a->ns->prefix == 0 ){
	  atts[(char*)a->name] = (char *)a->children->content;
	}
	else {
	  string pref = string( (const char*)a->ns->prefix);
	  if ( pref == "xlink" ){ // are there others?
	    atts[(char*)a->name] = (char *)a->children->content;
	  }
	  // else {
	  //   cerr << "attribute PREF=" << pref << endl;
	  // }
	}
	a = a->next;
      }
    }
    return atts;
  }

  void addAttributes( xmlNode *node, const KWargs& atts ){
    KWargs attribs = atts;
    auto it = attribs.find("_id");
    if ( it != attribs.end() ){ // _id is special for xml:id
      xmlSetProp( node, XML_XML_ID, (const xmlChar *)it->second.c_str() );
      attribs.erase(it);
    }
    it = attribs.find("lang");
    if ( it != attribs.end() ){ // lang is special too
      xmlNodeSetLang( node, (const xmlChar*)it->second.c_str() );
      attribs.erase(it);
    }
    it = attribs.find("id");
    if ( it != attribs.end() ){ // id is te be sorted before the rest
      xmlSetProp( node, (const xmlChar*)("id"), (const xmlChar *)it->second.c_str() );
      attribs.erase(it);
    }
    // and now the rest
    it = attribs.begin();
    while ( it != attribs.end() ){
      xmlSetProp( node,
		  (const xmlChar*)it->first.c_str(),
		  (const xmlChar*)it->second.c_str() );
      ++it;
    }
  }

  int toMonth( const string& ms ){
    int result = 0;
    try {
      result = stringTo<int>( ms );
      return result - 1;
    }
    catch( exception ){
      string m = TiCC::lowercase( ms );
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

  string parseDate( const string& s ){
    if ( s.empty() ){
      return "";
    }
    //    cerr << "try to read a date-time " << s << endl;
    vector<string> date_time;
    size_t num = TiCC::split_at( s, date_time, "T");
    if ( num == 0 ){
      num = TiCC::split_at( s, date_time, " ");
      if ( num == 0 ){
	cerr << "failed to read a date-time " << s << endl;
	return "";
      }
    }
    //    cerr << "found " << num << " parts" << endl;
    tm *time = new tm();
    if ( num == 1 || num == 2 ){
      //      cerr << "parse date " << date_time[0] << endl;
      vector<string> date_parts;
      size_t dnum = TiCC::split_at( date_time[0], date_parts, "-" );
      switch ( dnum ){
      case 3: {
	int mday = stringTo<int>( date_parts[2] );
	time->tm_mday = mday;
      }
      case 2: {
	int mon = toMonth( date_parts[1] );
	time->tm_mon = mon;
      }
      case 1: {
	int year = stringTo<int>( date_parts[0] );
	time->tm_year = year-1900;
      }
	break;
      default:
	cerr << "failed to read a date from " << date_time[0] << endl;
	return "";
      }
    }
    if ( num == 2 ){
      //      cerr << "parse time " << date_time[1] << endl;
      vector<string> date_parts;
      num = TiCC::split_at( date_time[1], date_parts, ":" );
      //      cerr << "parts " << date_parts << endl;
      switch ( num ){
      case 4:
	// ignore
      case 3: {
	int sec = stringTo<int>( date_parts[2] );
	time->tm_sec = sec;
      }
      case 2: {
	int min = stringTo<int>( date_parts[1] );
	time->tm_min = min;
      }
      case 1: {
	int hour = stringTo<int>( date_parts[0] );
	time->tm_hour = hour;
      }
	break;
      default:
	cerr << "failed to read a time from " << date_time[1] << endl;
	return "";
      }
    }
    // cerr << "read _date time = " << toString(time) << endl;
    char buf[100];
    strftime( buf, 100, "%Y-%m-%dT%X", time );
    delete time;
    return buf;
  }

  string parseTime( const string& s ){
    if ( s.empty() ){
      return "";
    }
    //    cerr << "try to read a time " << s << endl;
    vector<string> time_parts;
    string mil = "000";
    tm *time = new tm();
    int num = TiCC::split_at( s, time_parts, ":" );
    if ( num != 3 ){
      cerr << "failed to read a time " << s << endl;
      return "";
    }
    time->tm_min = stringTo<int>( time_parts[1] );
    time->tm_hour = stringTo<int>( time_parts[0] );
    string secs = time_parts[2];
    num = TiCC::split_at( secs, time_parts, "." );
    time->tm_sec = stringTo<int>( time_parts[0] );
    string mil_sec = "000";
    if ( num == 2 ){
      mil_sec = time_parts[1];
    }
    char buf[100];
    strftime( buf, 100, "%X", time );
    delete time;
    string result = buf;
    result += "." + mil_sec;
    //    cerr << "formatted time = " << result << endl;
    return result;
  }

  bool AT_sanity_check(){
    bool sane = true;
    AnnotationType::AnnotationType at = AnnotationType::NO_ANN;
    while ( ++at != AnnotationType::LAST_ANN ){
      string s;
      try {
	s = toString( at );
      }
      catch (...){
	cerr << "no string translation for AnnotationType(" << int(at) << ")" << endl;
	sane = false;
      }
      if ( !s.empty() ){
	try {
	  stringTo<AnnotationType::AnnotationType>( s );
	}
	catch ( ValueError& e ){
	  cerr << "no AnnotationType found for string '" << s << "'" << endl;
	  sane = false;
	}
      }
    }
    return sane;
  }

  bool ET_sanity_check(){
    bool sane = true;
    ElementType et = BASE;
    while ( ++et != LastElement ){
      string s;
      try {
	toString( et );
      }
      catch (...){
	cerr << "no string translation for ElementType(" << int(et) << ")" << endl;
	sane = false;
      }
      if( !s.empty() ){
	ElementType et2;
	try {
	  et2 = stringToET( s );
	}
	catch ( ValueError& e ){
	  cerr << "no element type found for string '" << s << "'" << endl;
	  sane = false;
	  continue;
	}
	if ( et != et2 ){
	  cerr << "Argl: toString(ET) doesn't match original:" << s
	       << " vs " << toString(et2) << endl;
	  sane = false;
	  continue;
	}
	FoliaElement *tmp = 0;
	try {
	  tmp = FoliaImpl::createElement( s );
	}
	catch( ValueError &e ){
	  string err = e.what();
	  if ( !err.find("abstract" ) ){
	    sane = false;
	  }
	}
	if ( tmp != 0 ) {
	  if ( et != tmp->element_id() ){
	    cerr << "the element type " << tmp->element_id() << " of " << tmp
		 << " != " << et << " (" << toString(tmp->element_id())
		 << " != " << toString(et) << ")" << endl;
	    sane = false;
	  }
	  if ( s != tmp->xmltag() ){
	    cerr << "the xmltag " << tmp->xmltag() << " != " << s << endl;
	    sane = false;
	  }
	}
      }
    }
    return sane;
  }

  bool isNCName( const string& s ){
    const string extra=".-_";
    if ( s.empty() ){
      throw XmlError( "an empty string is not a valid NCName." );
    }
    else if ( !isalpha(s[0]) ){
      throw XmlError( "'"
		      + s
		      + "' is not a valid NCName. (must start with character)." );
    }
    else {
      for ( const auto& let : s ){
	if ( !isalnum(let) &&
	     extra.find(let) == string::npos ){
	  throw XmlError( "'" + s
			  + "' is not a valid NCName.(invalid '"
			  + char(let) + "' found" );
	}
      }
    }
    return true;
  }

} //namespace folia
