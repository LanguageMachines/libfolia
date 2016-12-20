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
#include "libfolia/folia_properties.h"

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
    size_t dim = s_ant_map.size();
    if ( dim != ant_s_map.size() ){
      cerr << "s_ant_map and ant_s_map are different in size!" << endl;
      return false;
    }
    for ( auto const& it : ant_s_map ){
      string s;
      try {
	s = toString( it.first );
      }
      catch (...){
	cerr << "no string translation for AnnotationType(" << int(it.first) << ")" << endl;
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
    for ( auto const& it : s_et_map ){
      ElementType et = it.second;
      if ( et_s_map.find(et) == et_s_map.end() ){
	cerr << "no et_s found for ElementType(" << int(et) << ")" << endl;
	return false;
      }
    }
    for ( auto const& it : et_s_map ){
      ElementType et = it.first;
      if ( s_et_map.find(it.second) == s_et_map.end() ){
	cerr << "no string found for ElementType(" << int(it.first) << ")" << endl;
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
	  et2 = stringToET( s );
	}
	catch ( ValueError& e ){
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
	  tmp1 = FoliaImpl::createElement( s );
	}
	catch( ValueError &e ){
	  string err = e.what();
	  if ( err.find("abstract") ==string::npos ){
	    cerr << "createElement(" << s << ") failed! :" << err << endl;
	    sane = false;
	  }
	}
	if ( sane && tmp1 ){
	  FoliaElement *tmp2 = 0;
	  try {
	    tmp2 = FoliaImpl::createElement( et );
	  }
	  catch( ValueError &e ){
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
	  }
	}
      }
    }
    return sane;
  }

  bool isNCName( const string& s ){
    int test = xmlValidateNCName( (const xmlChar*)s.c_str(), 0 );
    if ( test != 0 ){
      return false;
    }
    return true;
  }

} //namespace folia
