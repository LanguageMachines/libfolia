/*
  $Id$
  $URL$

  Copyright (c) 1998 - 2015
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

#include <cassert>
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
#include "ticcutils/PrettyPrint.h"
#include "ticcutils/StringOps.h"
#include "ticcutils/XMLtools.h"
#include "libfolia/document.h"
#include "config.h"

using namespace std;
using namespace TiCC;

namespace folia {

  string VersionName() { return PACKAGE_STRING; }
  string Version() { return VERSION; }

  ostream& operator<<( ostream& os, const FoliaElement& ae ){
    os << " <" << ae.classname();
    KWargs ats = ae.collectAttributes();
    if ( !ae.id().empty() )
      ats["id"] = ae.id();

    for ( const auto& it: ats ){
      os << " " << it.first << "='" << it.second << "'";
    }
    os << " > {";
    for( size_t i=0; i < ae.size(); ++i ){
      os << "<" << ae.index(i)->classname() << ">,";
    }
    os << "}";
    return os;
  }

  ostream& operator<<( ostream&os, const FoliaElement *ae ){
    if ( !ae )
      os << "nil";
    else
      os << *ae;
    return os;
  }

  FoliaImpl::FoliaImpl( Document *d ){
    mydoc = d;
    _confidence = -1;
    _element_id = BASE;
    _refcount = 0;
    _parent = 0;
    _auth = true;
    _required_attributes = NO_ATT;
    _optional_attributes = NO_ATT;
    _annotation_type = AnnotationType::NO_ANN;
    _annotator_type = UNDEFINED;
    _xmltag = "ThIsIsSoWrOnG";
    _occurrences = 0;  //#Number of times this element may occur in its parent (0=unlimited, default=0)
    _occurrences_per_set = 1; // #Number of times this element may occur per set (0=unlimited, default=1)
    TEXTDELIMITER = "NONE" ;
    PRINTABLE = false;
    SPEAKABLE = false;
    XLINK = false;
  }

  FoliaImpl::~FoliaImpl( ){
    // cerr << "delete element id=" << _id << " tag = " << _xmltag << " *= "
    //  	 << (void*)this << " datasize= " << data.size() << endl;
    for ( size_t i=0; i < data.size(); ++i ){
      if ( data[i]->refcount() == 0 ) {
	// probably only for words
	delete data[i];
      }
      else if ( mydoc ){
	mydoc->keepForDeletion( data[i] );
      }
    }
    //    cerr << "\t\tdelete element id=" << _id << " tag = " << _xmltag << " *= "
    //	 << (void*)this << " datasize= " << data.size() << endl;
    if ( mydoc ){
      mydoc->delDocIndex( this, _id );
    }
  }

  xmlNs *FoliaImpl::foliaNs() const {
    if ( mydoc )
      return mydoc->foliaNs();
    else
      return 0;
  }

  void FoliaImpl::setAttributes( const KWargs& kwargs_in ){
    KWargs kwargs = kwargs_in;
    Attrib supported = _required_attributes | _optional_attributes;
    // if ( _element_id == Feature_t ){
    //   using TiCC::operator <<;
    //   cerr << "set attributes: " << kwargs << " on " << toString(_element_id) << endl;
    //   cerr << "required = " <<  _required_attributes << endl;
    //   cerr << "optional = " <<  _optional_attributes << endl;
    //   cerr << "supported = " << supported << endl;
    //   cerr << "ID & supported = " << (ID & supported) << endl;
    //   cerr << "ID & _required = " << (ID & _required_attributes ) << endl;
    // }
    if ( mydoc && mydoc->debug > 2 ){
      using TiCC::operator <<;
      cerr << "set attributes: " << kwargs << " on " << toString(_element_id) << endl;
    }

    KWargs::const_iterator it = kwargs.find( "generate_id" );
    if ( it != kwargs.end() ) {
      if ( !mydoc ){
	throw runtime_error( "can't generate an ID without a doc" );
      }
      FoliaElement *e = (*mydoc)[it->second];
      if ( e ){
	_id = e->generateId( _xmltag );
      }
      else
	throw ValueError("Unable to generate an id from ID= " + it->second );
      kwargs.erase( it );
    }
    else {
      it = kwargs.find( "_id" );
      KWargs::const_iterator it2 = kwargs.find( "id" );
      if ( it == kwargs.end() )
	it = it2;
      else if ( it2 != kwargs.end() ){
	throw ValueError("Both 'id' and 'xml:id found for " + classname() );
      }
      if ( it != kwargs.end() ) {
	if ( !ID & supported )
	  throw ValueError("ID is not supported for " + classname() );
	else {
	  isNCName( it->second );
	  _id = it->second;
	  kwargs.erase( it );
	}
      }
      else
	_id = "";
    }

    it = kwargs.find( "set" );
    string def;
    if ( it != kwargs.end() ) {
      if ( !( (CLASS|SETONLY) & supported ) ){
	throw ValueError("Set is not supported for " + classname());
      }
      else {
	_set = it->second;
      }
      if ( !mydoc ){
	throw ValueError( "Set=" + _set + " is used on a node without a document." );
      }
      if ( !mydoc->isDeclared( _annotation_type, _set ) )
	throw ValueError( "Set " + _set + " is used but has no declaration " +
			  "for " + toString( _annotation_type ) + "-annotation" );
      kwargs.erase( it );
    }
    else if ( mydoc && ( def = mydoc->defaultset( _annotation_type )) != "" ){
      _set = def;
    }
    else
      _set = "";
    it = kwargs.find( "class" );
    if ( it == kwargs.end() )
      it = kwargs.find( "cls" );
    if ( it != kwargs.end() ) {
      if ( !( CLASS & supported ) )
	throw ValueError("Class is not supported for " + classname() );
      _class = it->second;
      if ( _element_id != TextContent_t ){
	if ( !mydoc ){
	  throw ValueError( "Class=" + _class + " is used on a node without a document." );
	}
	else if ( _set == "" &&
		  mydoc->defaultset( _annotation_type ) == "" &&
		  mydoc->isDeclared( _annotation_type ) ){
	  throw ValueError( "Class " + _class + " is used but has no default declaration " +
			    "for " + toString( _annotation_type ) + "-annotation" );
	}
      }
      kwargs.erase( it );
    }
    else
      _class = "";

    if ( _element_id != TextContent_t && _element_id != PhonContent_t ){
      if ( !_class.empty() && _set.empty() )
	throw ValueError("Set is required for <" + classname() +
			 " class=\"" + _class + "\"> assigned without set."  );
    }
    it = kwargs.find( "annotator" );
    if ( it != kwargs.end() ) {
      if ( !(ANNOTATOR & supported) )
	throw ValueError("Annotator is not supported for " + classname() );
      else {
	_annotator = it->second;
      }
      kwargs.erase( it );
    }
    else if ( mydoc &&
	      (def = mydoc->defaultannotator( _annotation_type, _set )) != "" ){
      _annotator = def;
    }
    else
      _annotator = "";

    it = kwargs.find( "annotatortype" );
    if ( it != kwargs.end() ) {
      if ( ! (ANNOTATOR & supported) )
	throw ValueError("Annotatortype is not supported for " + classname() );
      else {
	_annotator_type = stringTo<AnnotatorType>( it->second );
	if ( _annotator_type == UNDEFINED )
	  throw ValueError( "annotatortype must be 'auto' or 'manual', got '"
			    + it->second + "'" );
      }
      kwargs.erase( it );
    }
    else if ( mydoc &&
	      (def = mydoc->defaultannotatortype( _annotation_type, _set ) ) != ""  ){
      _annotator_type = stringTo<AnnotatorType>( def );
      if ( _annotator_type == UNDEFINED )
	throw ValueError("annotatortype must be 'auto' or 'manual'");
    }
    else
      _annotator_type = UNDEFINED;


    it = kwargs.find( "confidence" );
    if ( it != kwargs.end() ) {
      if ( !(CONFIDENCE & supported) )
	throw ValueError("Confidence is not supported for " + classname() );
      else {
	try {
	  _confidence = stringTo<double>(it->second);
	  if ( _confidence < 0 || _confidence > 1.0 )
	    throw ValueError("Confidence must be a floating point number between 0 and 1, got " + TiCC::toString(_confidence) );
	}
	catch (...){
	  throw ValueError("invalid Confidence value, (not a number?)");
	}
      }
      kwargs.erase( it );
    }
    else
      _confidence = -1;

    it = kwargs.find( "n" );
    if ( it != kwargs.end() ) {
      if ( !(N & supported) )
	throw ValueError("N is not supported for " + classname() );
      else {
	_n = it->second;
      }
      kwargs.erase( it );
    }
    else
      _n = "";

    if ( XLINK ){
      it = kwargs.find( "href" );
      if ( it != kwargs.end() ){
	_href = it->second;
	kwargs.erase( it );
      }
      it = kwargs.find( "type" );
      if ( it != kwargs.end() ){
	string type = it->second;
	if ( type != "simple" ){
	  throw XmlError( "only xlink:type=\"simple\" is supported!" );
	}
	kwargs.erase( it );
      }
    }

    it = kwargs.find( "datetime" );
    if ( it != kwargs.end() ) {
      if ( !(DATETIME & supported) )
	throw ValueError("datetime is not supported for " + classname() );
      else {
	string time = parseDate( it->second );
	if ( time.empty() )
	  throw ValueError( "invalid datetime, must be in YYYY-MM-DDThh:mm:ss format: " + it->second );
	_datetime = time;
      }
      kwargs.erase( it );
    }
    else if ( mydoc &&
	      (def = mydoc->defaultdatetime( _annotation_type, _set )) != "" ){
      _datetime = def;
    }
    else
      _datetime.clear();

    it = kwargs.find( "begintime" );
    if ( it != kwargs.end() ) {
      if ( !(BEGINTIME & supported) )
	throw ValueError( "begintime is not supported for " + classname() );
      else {
	string time = parseTime( it->second );
	if ( time.empty() )
	  throw ValueError( "invalid begintime, must be in HH:MM:SS:mmmm format: " + it->second );
	_begintime = time;
      }
      kwargs.erase( it );
    }
    else
      _begintime.clear();

    it = kwargs.find( "endtime" );
    if ( it != kwargs.end() ) {
      if ( !(ENDTIME & supported) )
	throw ValueError( "endtime is not supported for " + classname() );
      else {
	string time = parseTime( it->second );
	if ( time.empty() )
	  throw ValueError( "invalid endtime, must be in HH:MM:SS:mmmm format: " + it->second );
	_endtime = time;
      }
      kwargs.erase( it );
    }
    else
      _endtime.clear();

    it = kwargs.find( "src" );
    if ( it != kwargs.end() ) {
      if ( !(SRC & supported) )
	throw ValueError( "src is not supported for " + classname() );
      else {
	_src = it->second;
      }
      kwargs.erase( it );
    }
    else
      _src.clear();

    it = kwargs.find( "speaker" );
    if ( it != kwargs.end() ) {
      if ( !(SPEAKER & supported) )
	throw ValueError( "speaker is not supported for " + classname() );
      else {
	_speaker = it->second;
      }
      kwargs.erase( it );
    }
    else
      _speaker.clear();

    it = kwargs.find( "auth" );
    if ( it != kwargs.end() ){
      _auth = stringTo<bool>( it->second );
      kwargs.erase( it );
    }

    if ( mydoc && !_id.empty() )
      mydoc->addDocIndex( this, _id );

    addFeatureNodes( kwargs );
  }

  void FoliaImpl::addFeatureNodes( const KWargs& kwargs ){
    for ( const auto& it: kwargs ){
      string tag = it.first;
      if ( tag == "head" ){
	// "head" is special because the tag is "headfeature"
	// this to avoid conflicts withe the "head" tag!
	KWargs newa;
	newa["class"] = it.second;
	FoliaElement *tmp = new HeadFeature();
	tmp->setAttributes( newa );
	append( tmp );
      }
      else if ( tag == "actor" || tag == "value"|| tag == "function" ||
		tag == "level" || tag == "time" || tag == "modality" ||
		tag == "synset" || tag == "begindatetime" ||
		tag == "enddatetime" ){
	KWargs newa;
	newa["class"] = it.second;
	FoliaElement *tmp = createElement( mydoc, tag );
	tmp->setAttributes( newa );
	append( tmp );
      }
      else {
	string message = "unsupported attribute: " + tag + "='" + it.second
	  + "' for node with tag '" + classname() + "'";
	if ( mydoc && mydoc->permissive() ){
	  cerr << message << endl;
	}
	else {
	  throw XmlError( message );
	}
      }
    }
  }

  KWargs FoliaImpl::collectAttributes() const {
    KWargs attribs;
    bool isDefaultSet = true;
    bool isDefaultAnn = true;

    if ( !_id.empty() ){
      attribs["_id"] = _id; // sort "id" as first!
    }
    if ( !_set.empty() &&
	 _set != mydoc->defaultset( _annotation_type ) ){
      isDefaultSet = false;
      attribs["set"] = _set;
    }
    if ( !_class.empty() )
      attribs["class"] = _class;

    if ( !_annotator.empty() &&
	 _annotator != mydoc->defaultannotator( _annotation_type, _set ) ){
      isDefaultAnn = false;
      attribs["annotator"] = _annotator;
    }
    if ( XLINK ){
      if ( !_href.empty() ){
	attribs["xlink:href"] = _href;
	attribs["xlink:type"] = "simple";
      }
    }
    if ( !_datetime.empty() &&
	 _datetime != mydoc->defaultdatetime( _annotation_type, _set ) ){
      attribs["datetime"] = _datetime;
    }
    if ( !_begintime.empty() ){
      attribs["begintime"] = _begintime;
    }
    if ( !_endtime.empty() ){
      attribs["endtime"] = _endtime;
    }
    if ( !_src.empty() ){
      attribs["src"] = _src;
    }
    if ( !_speaker.empty() ){
      attribs["speaker"] = _speaker;
    }
    if ( _annotator_type != UNDEFINED ){
      AnnotatorType at = stringTo<AnnotatorType>( mydoc->defaultannotatortype( _annotation_type, _set ) );
      if ( (!isDefaultSet || !isDefaultAnn) && _annotator_type != at ){
	if ( _annotator_type == AUTO )
	  attribs["annotatortype"] = "auto";
	else if ( _annotator_type == MANUAL )
	  attribs["annotatortype"] = "manual";
      }
    }

    if ( _confidence >= 0 )
      attribs["confidence"] = TiCC::toString(_confidence);

    if ( !_n.empty() )
      attribs["n"] = _n;

    if ( !_auth )
      attribs["auth"] = "no";

    return attribs;
  }

  string FoliaElement::xmlstring() const{
    // serialize to a string (XML fragment)
    xmlNode *n = xml( true, false );
    xmlSetNs( n, xmlNewNs( n, (const xmlChar *)NSFOLIA.c_str(), 0 ) );
    xmlBuffer *buf = xmlBufferCreate();
    xmlNodeDump( buf, 0, n, 0, 0 );
    string result = (const char*)xmlBufferContent( buf );
    xmlBufferFree( buf );
    xmlFreeNode( n );
    return result;
  }

  string tagToAtt( const FoliaElement* c ){
    string att;
    if ( c->isSubClass( Feature_t ) ){
      att = c->xmltag();
      if ( att == "feat" ){
	// "feat" is a Feature_t too. exclude!
	att = "";
      }
      else if ( att == "headfeature" )
	// "head" is special
	att = "head";
    }
    return att;
  }

  xmlNode *FoliaImpl::xml( bool recursive, bool kanon ) const {
    xmlNode *e = XmlNewNode( foliaNs(), _xmltag );
    KWargs attribs = collectAttributes();
    set<FoliaElement *> attribute_elements;
    // nodes that can be represented as attributes are converted to atributes
    // and excluded of 'normal' output.
    for ( const auto el : data ){
      string at = tagToAtt( el );
      if ( !at.empty() ){
	attribs[at] = el->cls();
	attribute_elements.insert( el );
      }
    }
    addAttributes( e, attribs );
    if ( recursive ){
      // append children:
      // we want make sure that text elements are in the right order,
      // in front and the 'current' class first
      list<FoliaElement *> textelements;
      list<FoliaElement *> otherelements;
      list<FoliaElement *> commentelements;
      multimap<ElementType, FoliaElement *> otherelementsMap;
      for ( const auto el : data ){
	if ( attribute_elements.find(el) == attribute_elements.end() ){
	  if ( el->isinstance(TextContent_t) ){
	    if ( el->cls() == "current" )
	      textelements.push_front( el );
	    else
	      textelements.push_back( el );
	  }
	  else {
	    if ( kanon )
	      otherelementsMap.insert( make_pair( el->element_id(), el ) );
	    else {
	      if ( el->isinstance(XmlComment_t) && textelements.empty() ){
		commentelements.push_back( el );
	      }
	      else
		otherelements.push_back( el );
	    }
	  }
	}
      }
      for( const auto cel : commentelements ){
	xmlAddChild( e, cel->xml( recursive, kanon ) );
      }
      for ( const auto tel : textelements ){
	xmlAddChild( e, tel->xml( recursive, kanon ) );
      }
      if ( !kanon ){
	for ( const auto oel : otherelements ){
	  xmlAddChild( e, oel->xml( recursive, kanon ) );
	}
      }
      else {
	for ( const auto oem : otherelementsMap ){
	  xmlAddChild( e, oem.second->xml( recursive, kanon ) );
	}
      }
    }
    return e;
  }

  string FoliaImpl::str( const std::string& ) const {
    cerr << "Impl::str()" << endl;
    return _xmltag;
  }

  string FoliaImpl::speech_src() const {
    if ( !_src.empty() )
      return _src;
    if ( _parent )
      return _parent->speech_src();
    return "";
  }

  string FoliaImpl::speech_speaker() const {
    if ( !_speaker.empty() )
      return _speaker;
    if ( _parent )
      return _parent->speech_speaker();
    return "";
  }

  bool FoliaElement::hastext( const string& cls ) const {
    // does this element have a TextContent with class 'cls'
    // Default is class="current"
    try {
      this->textcontent(cls);
      return true;
    } catch (NoSuchText& e ) {
      return false;
    }
  }

  //#define DEBUG_TEXT
  //#define DEBUG_TEXT_DEL

  string FoliaImpl::getTextDelimiter( bool retaintok ) const {
#ifdef DEBUG_TEXT_DEL
    cerr << "IN " << xmltag() << "::gettextdelimiter (" << retaintok << ")" << endl;
#endif
    if ( TEXTDELIMITER == "NONE" ){
      if ( data.size() > 0 ){
	// attempt to get a delimiter form the last child
	string det = data[data.size()-1]->getTextDelimiter( retaintok );
#ifdef DEBUG_TEXT_DEL
	cerr << "out" << xmltag() << "::gettextdelimiter ==> '" << det << "'" << endl;
#endif
	return det;
      }
      else
#ifdef DEBUG_TEXT_DEL
	cerr << "out" << xmltag() << "::gettextdelimiter ==> ''" << endl;
#endif
	return "";
    }
    return TEXTDELIMITER;
  }

  UnicodeString FoliaImpl::text( const string& cls,
				 bool retaintok,
				 bool strict ) const {
    // get the UnicodeString value of underlying elements
    // default cls="current"
#ifdef DEBUG_TEXT
    cerr << "TEXT(" << cls << ") op node : " << xmltag() << " id ( " << id() << ")" << endl;
#endif
    if ( strict ){
      return textcontent(cls)->text();
    }
    else if ( !PRINTABLE ){
      throw NoSuchText( "NON printable element: " + xmltag() );
    }
    else {
      UnicodeString result = deeptext( cls, retaintok );
      if ( result.isEmpty() )
	result = stricttext( cls );
      if ( result.isEmpty() )
	throw NoSuchText( "on tag " +_xmltag + " nor it's children" );
      return result;
    }
  }

  UnicodeString FoliaImpl::deeptext( const string& cls,
				     bool retaintok ) const {
    // get the UnicodeString value of underlying elements
    // default cls="current"
#ifdef DEBUG_TEXT
    cerr << "deepTEXT(" << cls << ") op node : " << _xmltag << " id(" << id() << ")" << endl;
#endif
#ifdef DEBUG_TEXT
    cerr << "deeptext: node has " << data.size() << " children." << endl;
#endif
    vector<UnicodeString> parts;
    vector<UnicodeString> seps;
    for( size_t i=0; i < data.size(); ++i ){
      // try to get text dynamically from children
      // skip TextContent elements
#ifdef DEBUG_TEXT
      if ( !data[i]->printable() ){
	cerr << "deeptext: node[" << i << "] " << data[i]->xmltag() << " NOT PRINTABLE! " << endl;
      }
#endif
      if ( data[i]->printable() && !data[i]->isinstance( TextContent_t ) ){
#ifdef DEBUG_TEXT
	cerr << "deeptext:bekijk node[" << i << "] " << data[i]->xmltag() << endl;
#endif
	try {
	  UnicodeString tmp = data[i]->text( cls, retaintok, false );
#ifdef DEBUG_TEXT
	  cerr << "deeptext found '" << tmp << "'" << endl;
#endif
	  if ( !isSubClass(AbstractTextMarkup_t) )
	    tmp.trim();
	  parts.push_back(tmp);
	  // get the delimiter
	  string delim = data[i]->getTextDelimiter( retaintok );
#ifdef DEBUG_TEXT
	  cerr << "deeptext:delimiter van "<< data[i]->xmltag() << " ='" << delim << "'" << endl;
#endif
	  seps.push_back(UTF8ToUnicode(delim));
	} catch ( NoSuchText& e ){
#ifdef DEBUG_TEXT
	  cerr << "HELAAS" << endl;
#endif
	}
      }
    }

    // now construct the result;
    UnicodeString result;
    for( size_t i=0; i < parts.size(); ++i ){
      result += parts[i];
      if ( i < parts.size()-1)
	result += seps[i];
    }
#ifdef DEBUG_TEXT
    cerr << "deeptext() for " << _xmltag << " step 3 " << endl;
#endif
    if ( result.isEmpty() ){
      result = textcontent(cls)->text();
    }
#ifdef DEBUG_TEXT
    cerr << "deeptext() for " << _xmltag << " result= '" << result << "'" << endl;
#endif
    if ( result.isEmpty() ){
      throw NoSuchText( _xmltag + ":(class=" + cls +"): empty!" );
    }
    return result;
  }

  UnicodeString FoliaElement::stricttext( const string& cls ) const {
    // get UnicodeString content of TextContent children only
    // default cls="current"
    return this->text(cls, false, true );
  }

  UnicodeString FoliaElement::toktext( const string& cls ) const {
    // get UnicodeString content of TextContent children only
    // default cls="current"
    return this->text(cls, true, false );
  }

  TextContent *FoliaImpl::textcontent( const string& cls ) const {
    // Get the text explicitly associated with this element
    // (of the specified class) the default class is 'current'
    // Returns the TextContent instance rather than the actual text.
    // Does not recurse into children
    // with sole exception of Correction
    // Raises NoSuchText exception if not found.

    if ( !PRINTABLE )
      throw NoSuchText( "non-printable element: " +  _xmltag );

    for( size_t i=0; i < data.size(); ++i ){
      if ( data[i]->isinstance(TextContent_t) && (data[i]->cls() == cls) ){
	return dynamic_cast<TextContent*>(data[i]);
      }
      else if ( data[i]->element_id() == Correction_t) {
	try {
	  return data[i]->textcontent(cls);
	} catch ( NoSuchText& e ){
	  // continue search for other Corrections or a TextContent
	}
      }
    }
    throw NoSuchText( _xmltag + "::textcontent()" );
  }

  PhonContent *FoliaImpl::phoncontent( const string& cls ) const {
    // Get the phon explicitly associated with this element
    // (of the specified class) the default class is 'current'
    // Returns the PhonContent instance rather than the actual phoneme.
    // Does not recurse into children
    // with sole exception of Correction
    // Raises NoSuchPhon exception if not found.

    if ( !SPEAKABLE )
      throw NoSuchPhon( "non-speakable element: " +  _xmltag );

    for( size_t i=0; i < data.size(); ++i ){
      if ( data[i]->isinstance(PhonContent_t) && (data[i]->cls() == cls) ){
	return dynamic_cast<PhonContent*>(data[i]);
      }
      else if ( data[i]->element_id() == Correction_t) {
	try {
	  return data[i]->phoncontent(cls);
	} catch ( NoSuchPhon& e ){
	  // continue search for other Corrections or a TextContent
	}
      }
    }
    throw NoSuchPhon( _xmltag + "::phoncontent()" );
  }

  //#define DEBUG_PHON

  UnicodeString FoliaImpl::phon( const string& cls,
				 bool strict ) const {
    // get the UnicodeString value of underlying elements
    // default cls="current"
#ifdef DEBUG_PHON
    cerr << "PHON(" << cls << ") op node : " << xmltag() << " id ( " << id() << ")" << endl;
#endif
    if ( strict ){
      return phoncontent(cls)->phon();
    }
    else if ( !SPEAKABLE ){
      throw NoSuchText( "NON speakable element: " + xmltag() );
    }
    else {
      UnicodeString result = deepphon( cls );
      if ( result.isEmpty() )
	result = phoncontent(cls)->phon();
      if ( result.isEmpty() )
	throw NoSuchPhon( "on tag " +_xmltag + " nor it's children" );
      return result;
    }
  }

  UnicodeString FoliaImpl::deepphon( const string& cls ) const {
    // get the UnicodeString value of underlying elements
    // default cls="current"
#ifdef DEBUG_PHON
    cerr << "deepPHON(" << cls << ") op node : " << _xmltag << " id(" << id() << ")" << endl;
#endif
#ifdef DEBUG_PHON
    cerr << "deepphon: node has " << data.size() << " children." << endl;
#endif
    vector<UnicodeString> parts;
    vector<UnicodeString> seps;
    for( size_t i=0; i < data.size(); ++i ){
      // try to get text dynamically from children
      // skip TextContent elements
#ifdef DEBUG_TEXT
      if ( !data[i]->speakable() ){
	cerr << "deepphon: node[" << i << "] " << data[i]->xmltag() << " NOT SPEAKABLE! " << endl;
      }
#endif
      if ( data[i]->speakable() && !data[i]->isinstance( PhonContent_t ) ){
#ifdef DEBUG_TEXT
	cerr << "deepphon:bekijk node[" << i << "] " << data[i]->xmltag() << endl;
#endif
	try {
	  UnicodeString tmp = data[i]->phon( cls, false );
#ifdef DEBUG_TEXT
	  cerr << "deepphon found '" << tmp << "'" << endl;
#endif
	  parts.push_back(tmp);
	  // get the delimiter
	  string delim = data[i]->getTextDelimiter();
#ifdef DEBUG_TEXT
	  cerr << "deepphon:delimiter van "<< data[i]->xmltag() << " ='" << delim << "'" << endl;
#endif
	  seps.push_back(UTF8ToUnicode(delim));
	} catch ( NoSuchPhon& e ){
#ifdef DEBUG_TEXT
	  cerr << "HELAAS" << endl;
#endif
	}
      }
    }

    // now construct the result;
    UnicodeString result;
    for( size_t i=0; i < parts.size(); ++i ){
      result += parts[i];
      if ( i < parts.size()-1)
	result += seps[i];
    }
#ifdef DEBUG_TEXT
    cerr << "deepphon() for " << _xmltag << " step 3 " << endl;
#endif
    if ( result.isEmpty() ){
      result = phoncontent(cls)->phon();
    }
#ifdef DEBUG_TEXT
    cerr << "deepphontext() for " << _xmltag << " result= '" << result << "'" << endl;
#endif
    if ( result.isEmpty() ){
      throw NoSuchPhon( _xmltag + ":(class=" + cls +"): empty!" );
    }
    return result;
  }


  vector<FoliaElement *>FoliaImpl::findreplacables( FoliaElement *par ) const {
    return par->select( element_id(), sett(), false );
  }

  void FoliaImpl::replace( FoliaElement *child ){
    // Appends a child element like append(), but replaces any existing child
    // element of the same type and set.
    // If no such child element exists, this will act the same as append()

    vector<FoliaElement*> replace = child->findreplacables( this );
    if ( replace.empty() ){
      // nothing to replace, simply call append
      append( child );
    }
    else if ( replace.size() > 1 ){
      throw runtime_error( "Unable to replace. Multiple candidates found, unable to choose." );
    }
    else {
      remove( replace[0], true );
      append( child );
    }
  }

  FoliaElement* FoliaImpl::replace( FoliaElement *old,
				    FoliaElement* _new ){
    // replaced old by _new
    // returns old
    // when not found does nothing and returns 0;
    for( size_t i=0; i < data.size(); ++i ){
      if ( data[i] == old ){
	data[i] = _new;
	_new->setParent( this );
	return old;
      }
    }
    return 0;
  }

  TextContent *FoliaElement::settext( const string& txt,
				      const string& cls ){
    // create a TextContent child of class 'cls'
    // Default cls="current"
    KWargs args;
    args["value"] = txt;
    args["class"] = cls;
    TextContent *node = new TextContent( doc() );
    node->setAttributes( args );
    replace( node );
    return node;
  }

  TextContent *FoliaElement::setutext( const UnicodeString& txt,
				       const string& cls ){
    // create a TextContent child of class 'cls'
    // Default cls="current"
    string utf8 = UnicodeToUTF8(txt);
    return settext( utf8, cls );
  }

  TextContent *FoliaElement::settext( const string& txt,
				      int offset,
				      const string& cls ){
    // create a TextContent child of class 'cls'
    // Default cls="current"
    // sets the offset attribute.
    KWargs args;
    args["value"] = txt;
    args["class"] = cls;
    args["offset"] = TiCC::toString(offset);
    TextContent *node = new TextContent( doc() );
    node->setAttributes( args );
    replace( node );
    return node;
  }

  TextContent *FoliaElement::setutext( const UnicodeString& txt,
				       int offset,
				       const string& cls ){
    // create a TextContent child of class 'cls'
    // Default cls="current"
    string utf8 = UnicodeToUTF8(txt);
    return settext( utf8, offset, cls );
  }

  string FoliaElement::description() const {
    vector<FoliaElement *> v =  select( Description_t, false );
    if ( v.size() == 0 )
      throw NoSuchAnnotation( "description" );
    else
      return v[0]->description();
  }

  bool isSubClass( const ElementType e1, const ElementType e2 ){
    static map<ElementType,set<ElementType> > sm;
    static bool filled = false;
    if ( !filled ){
      sm[Structure_t] = { Head_t, Division_t,
			  TableHead_t, Table_t,
			  Row_t, Cell_t,
			  LineBreak_t, WhiteSpace_t,
			  Word_t, WordReference_t,
			  Sentence_t, Paragraph_t,
			  Quote_t, Morpheme_t,
			  Text_t, Event_t, Reference_t,
			  External_t,
			  Caption_t, Label_t,
			  Item_t, List_t,
			  Figure_t, Alternative_t, Note_t,
			  Part_t };
      sm[Feature_t] = { SynsetFeature_t,
			ActorFeature_t, HeadFeature_t,
			ValueFeature_t, TimeFeature_t,
			ModalityFeature_t, LevelFeature_t,
			BeginDateTimeFeature_t,
			EndDateTimeFeature_t,
			FunctionFeature_t };
      sm[TokenAnnotation_t] = { Pos_t, Lemma_t, MorphologyLayer_t,
				Sense_t, Phoneme_t, String_t, Lang_t,
				Correction_t, Subjectivity_t,
				ErrorDetection_t };
      sm[SpanAnnotation_t] = { SyntacticUnit_t,
			       Chunk_t, Entity_t,
			       Headwords_t,
			       DependencyDependent_t,Dependency_t,
			       CoreferenceLink_t, CoreferenceChain_t,
			       Semrole_t, TimeSegment_t };
      sm[AnnotationLayer_t] = { SyntaxLayer_t,
				Chunking_t, Entities_t,
				TimingLayer_t, MorphologyLayer_t,
				Dependencies_t,
				Coreferences_t, Semroles_t };
      sm[AbstractTextMarkup_t] = { TextMarkupString_t, TextMarkupGap_t,
				   TextMarkupCorrection_t,
				   TextMarkupError_t, TextMarkupStyle_t };
      filled = true;
    }
    if ( e1 == e2 )
      return true;
    map<ElementType,set<ElementType> >::const_iterator it = sm.find( e2 );
    if ( it != sm.end() ){
      return it->second.find( e1 ) != it->second.end();
    }
    return false;
  }

  bool FoliaElement::isSubClass( ElementType t ) const {
    return folia::isSubClass( element_id(), t );
  }

  bool FoliaImpl::acceptable( ElementType t ) const {
    if ( t == XmlComment_t )
      return true;
    else {
      set<ElementType>::const_iterator it = _accepted_data.find( t );
      if ( it == _accepted_data.end() ){
	it = _accepted_data.begin();
	while ( it != _accepted_data.end() ){
	  if ( folia::isSubClass( t, *it ) )
	    return true;
	  ++it;
	}
	return false;
      }
      return true;
    }
  }

  bool FoliaImpl::addable( const FoliaElement *c ) const {
    if ( !acceptable( c->element_id() ) ){
      throw ValueError( "Unable to append object of type " + c->classname()
			+ " to a " + classname() );
    }
    if ( c->occurrences() > 0 ){
      vector<FoliaElement*> v = select( c->element_id(), false );
      size_t count = v.size();
      if ( count >= c->occurrences() ){
	throw DuplicateAnnotationError( "Unable to add another object of type " + c->classname() + " to " + classname() + ". There are already " + TiCC::toString(count) + " instances of this class, which is the maximum." );
      }
    }
    if ( c->occurrences_per_set() > 0 &&
	 ( (CLASS|SETONLY) & c->required_attributes() ) ){
      vector<FoliaElement*> v = select( c->element_id(), c->sett(), false );
      size_t count = v.size();
      if ( count >= c->occurrences_per_set() ){
	throw DuplicateAnnotationError( "Unable to add another object of type " + c->classname() + " to " + classname() + ". There are already " + TiCC::toString(count) + " instances of this class, which is the maximum." );
      }
    }
    if ( c->parent() &&
	 !( c->element_id() == Word_t
	    || c->element_id() == Morpheme_t
	    || c->element_id() == Phoneme_t ) ){
      // Only for WordRef i hope
      throw XmlError( "attempt to reconnect node " + c->classname()
		      + " to a " + classname() + " node, id=" + _id
		      + ", it was already connected to a "
		      +  c->parent()->classname() + " id=" + c->parent()->id() );
    }
    if ( c->element_id() == TextContent_t && _element_id == Word_t ){
      string val = c->str();
      val = trim( val );
      if ( val.empty() ){
	throw ValueError( "attempt to add an empty <t> to word: " + _id );
      }
    }
    return true;
  }

  void FoliaImpl::fixupDoc( Document* doc ) {
    if ( !mydoc ){
      mydoc = doc;
      string myid = id();
      if ( !_set.empty()
	   && (CLASS & _required_attributes )
	   && !mydoc->isDeclared( _annotation_type, _set ) )
	throw ValueError( "Set " + _set + " is used in " + _xmltag
			  + "element: " + myid + " but has no declaration " +
			  "for " + toString( _annotation_type ) + "-annotation" );
      if ( !myid.empty() )
	doc->addDocIndex( this, myid );
      // assume that children also might be doc-less
      for( size_t i=0; i < data.size(); ++i )
	data[i]->fixupDoc( doc );
    }
  }

  bool FoliaImpl::checkAtts(){
    if ( _id.empty() && (ID & _required_attributes ) ){
      throw ValueError( "attribute 'ID' is required for " + classname() );
    }
    if ( _set.empty() && (CLASS & _required_attributes ) ){
      throw ValueError( "attribute 'set' is required for " + classname() );
    }
    if ( _class.empty() && ( CLASS & _required_attributes ) ){
      throw ValueError( "attribute 'class' is required for " + classname() );
    }
    if ( _annotator.empty() && ( ANNOTATOR & _required_attributes ) ){
      throw ValueError( "attribute 'annotator' is required for " + classname() );
    }
    if ( _annotator_type == UNDEFINED && ( ANNOTATOR & _required_attributes ) ){
      throw ValueError( "attribute 'Annotatortype' is required for " + classname() );
    }
    if ( _confidence == -1 && ( CONFIDENCE & _required_attributes ) ){
      throw ValueError( "attribute 'confidence' is required for " + classname() );
    }
    if ( _n.empty() && ( N & _required_attributes ) ){
      throw ValueError( "attribute 'n' is required for " + classname() );
    }
    if ( DATETIME & _required_attributes ){
      throw ValueError( "attribute 'datetime' is required for " + classname() );
    }
    if ( BEGINTIME & _required_attributes ){
      throw ValueError( "attribute 'begintime' is required for " + classname() );
    }
    if ( ENDTIME & _required_attributes ){
      throw ValueError( "attribute 'endtime' is required for " + classname() );
    }
    if ( SRC & _required_attributes ){
      throw ValueError( "attribute 'src' is required for " + classname() );
    }
    if ( SPEAKER & _required_attributes ){
      throw ValueError( "attribute 'speaker' is required for " + classname() );
    }
    return true;
  }

  FoliaElement *FoliaImpl::append( FoliaElement *child ){
    bool ok = false;
    try {
      ok = child->checkAtts();
      ok = addable( child );
    }
    catch ( XmlError& ){
      // don't delete the offending child in case of illegal reconnection
      // it will be deleted by the true parent
      throw;
    }
    catch ( exception& ){
      delete child;
      throw;
    }
    if ( ok ){
      child->fixupDoc( mydoc );
      data.push_back(child);
      if ( !child->parent() ) // Only for WordRef and Morpheme
	child->setParent(this);
      else
	child->increfcount();
      return child->postappend();
    }
    return 0;
  }


  void FoliaImpl::remove( FoliaElement *child, bool del ){
    vector<FoliaElement*>::iterator it = std::remove( data.begin(), data.end(), child );
    data.erase( it, data.end() );
    if ( del )
      delete child;
  }

  void FoliaImpl::remove( size_t pos, bool del ){
    if ( pos < data.size() ){
      vector<FoliaElement*>::iterator it = data.begin();
      while ( pos > 0 ){
	++it;
	--pos;
      }
      if ( del )
	delete *it;
      data.erase(it);
    }
  }

  FoliaElement* FoliaImpl::index( size_t i ) const {
    if ( i < data.size() )
      return data[i];
    else
      throw range_error( "[] index out of range" );
  }

  FoliaElement* FoliaImpl::rindex( size_t i ) const {
    if ( i < data.size() )
      return data[data.size()-1-i];
    else
      throw range_error( "[] rindex out of range" );
  }

  vector<FoliaElement*> FoliaImpl::select( ElementType et,
					   const string& st,
					   const set<ElementType>& exclude,
					   bool recurse ) const {
    vector<FoliaElement*> res;
    for ( size_t i = 0; i < data.size(); ++i ){
      if ( data[i]->element_id() == et &&
	   ( st.empty() || data[i]->sett() == st ) ){
	res.push_back( data[i] );
      }
      if ( recurse ){
	if ( exclude.find( data[i]->element_id() ) == exclude.end() ){
	  vector<FoliaElement*> tmp = data[i]->select( et, st, exclude, recurse );
	  res.insert( res.end(), tmp.begin(), tmp.end() );
	}
      }
    }
    return res;
  }

  set<ElementType> default_ignore =  { Original_t,
				       Suggestion_t,
				       Alternative_t };
  set<ElementType> default_ignore_annotations = { Original_t,
						  Suggestion_t,
						  Alternative_t,
						  MorphologyLayer_t };

  set<ElementType> default_ignore_structure = { Original_t,
						Suggestion_t,
						Alternative_t,
						Chunk_t,
						SyntacticUnit_t,
						Coreferences_t,
						Semroles_t,
						Entity_t,
						Headwords_t,
						TimingLayer_t,
						DependencyDependent_t,
						TimeSegment_t };

  vector<FoliaElement*> FoliaImpl::select( ElementType et,
					   const string& st,
					   bool recurse ) const {
    return select( et, st, default_ignore, recurse );
  }

  vector<FoliaElement*> FoliaImpl::select( ElementType et,
					   const set<ElementType>& exclude,
					   bool recurse ) const {
    vector<FoliaElement*> res;
    for ( size_t i = 0; i < data.size(); ++i ){
      if ( data[i]->element_id() == et ){
	res.push_back( data[i] );
      }
      if ( recurse ){
	if ( exclude.find( data[i]->element_id() ) == exclude.end() ){
	  vector<FoliaElement*> tmp = data[i]->select( et, exclude, recurse );
	  res.insert( res.end(), tmp.begin(), tmp.end() );
	}
      }
    }
    return res;
  }

  vector<FoliaElement*> FoliaImpl::select( ElementType et,
					      bool recurse ) const {
    return select( et, default_ignore, recurse );
  }

  FoliaElement* FoliaImpl::parseXml( const xmlNode *node ){
    KWargs att = getAttributes( node );
    setAttributes( att );
    xmlNode *p = node->children;
    while ( p ){
      if ( p->type == XML_ELEMENT_NODE ){
	string tag = Name( p );
	FoliaElement *t = createElement( doc(), tag );
	if ( t ){
	  if ( doc() && doc()->debug > 2 )
	    cerr << "created " << t << endl;
	  t = t->parseXml( p );
	  if ( t ){
	    if ( doc() && doc()->debug > 2 )
	      cerr << "extend " << this << " met " << t << endl;
	    append( t );
	  }
	}
      }
      else if ( p->type == XML_COMMENT_NODE ){
	string tag = "xml-comment";
	FoliaElement *t = createElement( doc(), tag );
	if ( t ){
	  if ( doc() && doc()->debug > 2 )
	    cerr << "created " << t << endl;
	  t = t->parseXml( p );
	  if ( t ){
	    if ( doc() && doc()->debug > 2 )
	      cerr << "extend " << this << " met " << t << endl;
	    append( t );
	  }
	}
      }
      else if ( p->type == XML_TEXT_NODE ){
	string tag = "xml-text";
	FoliaElement *t = createElement( doc(), tag );
	if ( t ){
	  if ( doc() && doc()->debug > 2 )
	    cerr << "created " << t << endl;
	  t = t->parseXml( p );
	  if ( t ){
	    if ( doc() && doc()->debug > 2 )
	      cerr << "extend " << this << " met " << t << endl;
	    append( t );
	  }
	}
      }
      p = p->next;
    }
    return this;
  }

  void FoliaImpl::setDateTime( const string& s ){
    Attrib supported = _required_attributes | _optional_attributes;
    if ( !(DATETIME & supported) )
      throw ValueError("datetime is not supported for " + classname() );
    else {
      string time = parseDate( s );
      if ( time.empty() )
	throw ValueError( "invalid datetime, must be in YYYY-MM-DDThh:mm:ss format: " + s );
      _datetime = time;
    }
  }

  string FoliaImpl::getDateTime() const {
    return _datetime;
  }

  string FoliaImpl::pos( const string& st ) const {
    return annotation<PosAnnotation>( st )->cls();
  }

  string FoliaImpl::lemma( const string& st ) const {
    return annotation<LemmaAnnotation>( st )->cls();
  }

  PosAnnotation *AllowAnnotation::addPosAnnotation( const KWargs& inargs ){
    KWargs args = inargs;
    string st;
    KWargs::const_iterator it = args.find("set" );
    if ( it != args.end() ){
      st = it->second;
    }
    string newId = "alt-pos";
    it = args.find("generate_id" );
    if ( it != args.end() ){
      newId = it->second;
      args.erase("generate_id");
    }
    if ( hasannotation<PosAnnotation>( st ) ){
      // ok, there is already one, so create an Alternative
      KWargs kw;
      kw["id"] = generateId( newId );
      Alternative *alt = new Alternative( doc(), kw );
      append( alt );
      return alt->addAnnotation<PosAnnotation>( args );
    }
    else {
      return addAnnotation<PosAnnotation>( args );
    }
  }

  PosAnnotation* AllowAnnotation::getPosAnnotations( const string& st,
					  vector<PosAnnotation*>& vec ) const {
    PosAnnotation *res = 0;
    vec.clear();
    try {
      res = annotation<PosAnnotation>( st );
    }
    catch( NoSuchAnnotation& e ){
      res = 0;
    }
    // now search for alternatives
    static set<ElementType> excludeSet;
    if ( excludeSet.empty() ){
      excludeSet.insert( Original_t );
      excludeSet.insert( Suggestion_t );
    }
    vector<Alternative *> alts = select<Alternative>( excludeSet );
    for ( size_t i=0; i < alts.size(); ++i ){
      if ( alts[i]->size() > 0 ) { // child elements?
	for ( size_t j =0; j < alts[i]->size(); ++j ){
	  if ( alts[i]->index(j)->element_id() == Pos_t &&
	       ( st.empty() || alts[i]->index(j)->sett() == st ) ){
	    vec.push_back( dynamic_cast<PosAnnotation*>(alts[i]->index(j)) );
	  }
	}
      }
    }
    return res;
  }

  LemmaAnnotation *AllowAnnotation::addLemmaAnnotation( const KWargs& inargs ){
    KWargs args = inargs;
    string st;
    KWargs::const_iterator it = args.find("set" );
    if ( it != args.end() ){
      st = it->second;
    }
    string newId = "alt-lem";
    it = args.find("generate_id" );
    if ( it != args.end() ){
      newId = it->second;
      args.erase("generate_id");
    }
    if ( hasannotation<LemmaAnnotation>( st ) ){
      // ok, there is already one, so create an Alternative
      KWargs kw;
      kw["id"] = generateId( newId );
      Alternative *alt = new Alternative( doc(), kw );
      append( alt );
      return alt->addAnnotation<LemmaAnnotation>( args );
    }
    else {
      return addAnnotation<LemmaAnnotation>( args );
    }
  }

  LemmaAnnotation* AllowAnnotation::getLemmaAnnotations( const string& st,
					      vector<LemmaAnnotation*>& vec ) const {
    LemmaAnnotation *res = 0;
    vec.clear();
    try {
      res = annotation<LemmaAnnotation>( st );
    }
    catch( NoSuchAnnotation& e ){
      // ok
      res = 0;
    }
    // now search for alternatives
    static set<ElementType> excludeSet;
    if ( excludeSet.empty() ){
      excludeSet.insert( Original_t );
      excludeSet.insert( Suggestion_t );
    }
    vector<Alternative *> alts = select<Alternative>( excludeSet );
    for ( size_t i=0; i < alts.size(); ++i ){
      if ( alts[i]->size() > 0 ) { // child elements?
	for ( size_t j =0; j < alts[i]->size(); ++j ){
	  if ( alts[i]->index(j)->element_id() == Lemma_t &&
	       ( st.empty() || alts[i]->index(j)->sett() == st ) ){
	    vec.push_back( dynamic_cast<LemmaAnnotation*>(alts[i]->index(j)) );
	  }
	}
      }
    }
    return res;
  }

  MorphologyLayer *Word::addMorphologyLayer( const KWargs& inargs ){
    KWargs args = inargs;
    string st;
    KWargs::const_iterator it = args.find("set" );
    if ( it != args.end() ){
      st = it->second;
    }
    string newId = "alt-mor";
    it = args.find("generate_id" );
    if ( it != args.end() ){
      newId = it->second;
      args.erase("generate_id");
    }
    if ( hasannotation<MorphologyLayer>( st ) ){
      // ok, there is already one, so create an Alternative
      KWargs kw;
      kw["id"] = generateId( newId );
      Alternative *alt = new Alternative( doc(), kw );
      append( alt );
      return alt->addAnnotation<MorphologyLayer>( args );
    }
    else {
      return addAnnotation<MorphologyLayer>( args );
    }
  }

  MorphologyLayer *Word::getMorphologyLayers( const string& st,
					      vector<MorphologyLayer*>& vec ) const {
    MorphologyLayer *res = 0;
    vec.clear();
    try {
      res = annotation<MorphologyLayer>( st );
    }
    catch( NoSuchAnnotation& e ){
      // ok
      res = 0;
    }
    // now search for alternatives
    static set<ElementType> excludeSet;
    if ( excludeSet.empty() ){
      excludeSet.insert( Original_t );
      excludeSet.insert( Suggestion_t );
    }
    vector<Alternative *> alts = select<Alternative>( excludeSet );
    for ( size_t i=0; i < alts.size(); ++i ){
      if ( alts[i]->size() > 0 ) { // child elements?
	for ( size_t j =0; j < alts[i]->size(); ++j ){
	  if ( alts[i]->index(j)->element_id() == MorphologyLayer_t &&
	       ( st.empty() || alts[i]->index(j)->sett() == st ) ){
	    vec.push_back( dynamic_cast<MorphologyLayer*>(alts[i]->index(j)) );
	  }
	}
      }
    }
    return res;
  }

  Sentence *FoliaImpl::addSentence( const KWargs& args ){
    Sentence *res = new Sentence( mydoc );
    KWargs kw = args;
    if ( kw["id"].empty() ){
      string id = generateId( "s" );
      kw["id"] = id;
    }
    try {
      res->setAttributes( kw );
    }
    catch( DuplicateIDError& e ){
      delete res;
      throw e;
    }
    append( res );
    return res;
  }

  Word *FoliaImpl::addWord( const KWargs& args ){
    Word *res = new Word( mydoc );
    KWargs kw = args;
    if ( kw["id"].empty() ){
      string id = generateId( "w" );
      kw["id"] = id;
    }
    try {
      res->setAttributes( kw );
    }
    catch( DuplicateIDError& e ){
      delete res;
      throw e;
    }
    append( res );
    return res;
  }

  std::string Quote::getTextDelimiter( bool retaintok ) const {
#ifdef DEBUG_TEXT_DEL
    cerr << "IN " << xmltag() << "::gettextdelimiter (" << retaintok << ")" << endl;
#endif
    std::vector<FoliaElement*>::const_reverse_iterator it = data.rbegin();
    while ( it != data.rend() ){
      if ( (*it)->isinstance( Sentence_t ) ){
	// if a quote ends in a sentence, we don't want any delimiter
#ifdef DEBUG_TEXT_DEL
	cerr << "OUT " << xmltag() << "::gettextdelimiter ==>''" << endl;
#endif
	return "";
      }
      else {
	string res = (*it)->getTextDelimiter( retaintok );
#ifdef DEBUG_TEXT_DEL
	cerr << "OUT " << xmltag() << "::gettextdelimiter ==> '"
	     << res << "'" << endl;
#endif
	return res;
      }
      ++it;
    }
    return " ";
  }

  vector<Word*> Quote::wordParts() const {
    vector<Word*> result;
    for ( size_t i=0; i < data.size(); ++i ){
      FoliaElement *pnt = data[i];
      if ( pnt->isinstance( Word_t ) )
	result.push_back( dynamic_cast<Word*>(pnt) );
      else if ( pnt->isinstance( Sentence_t ) ){
	PlaceHolder *p = new PlaceHolder( mydoc, "text='" +
					  pnt->id() + "'");
	mydoc->keepForDeletion( p );
	result.push_back( p );
      }
      else if ( pnt->isinstance( Quote_t ) ){
	vector<Word*> tmp = pnt->wordParts();
	result.insert( result.end(), tmp.begin(), tmp.end() );
      }
      else if ( pnt->isinstance( Description_t ) ){
	// ignore
      }
      else {
	throw XmlError( "Word or Sentence expected in Quote. got: "
			+ pnt->classname() );
      }
    }
    return result;
  }

  vector<Word*> Sentence::wordParts() const {
    vector<Word*> result;
    for ( size_t i=0; i < data.size(); ++i ){
      FoliaElement *pnt = data[i];
      if ( pnt->isinstance( Word_t ) )
	result.push_back( dynamic_cast<Word*>(pnt) );
      else if ( pnt->isinstance( Quote_t ) ){
	vector<Word*> v = pnt->wordParts();
	result.insert( result.end(), v.begin(),v.end() );
      }
      else {
	// skip all other stuff. Is there any?
      }
    }
    return result;
  }

  Correction *Sentence::splitWord( FoliaElement *orig, FoliaElement *p1, FoliaElement *p2, const KWargs& args ){
    vector<FoliaElement*> ov;
    ov.push_back( orig );
    vector<FoliaElement*> nv;
    nv.push_back( p1 );
    nv.push_back( p2 );
    return correctWords( ov, nv, args );
  }

  Correction *Sentence::mergewords( FoliaElement *nw,
				    const vector<FoliaElement *>& orig,
				    const string& args ){
    vector<FoliaElement*> nv;
    nv.push_back( nw );
    return correctWords( orig, nv, getArgs(args) );
  }

  Correction *Sentence::deleteword( FoliaElement *w,
				    const string& args ){
    vector<FoliaElement*> ov;
    ov.push_back( w );
    vector<FoliaElement*> nil1;
    return correctWords( ov, nil1, getArgs(args) );
  }

  Correction *Sentence::insertword( FoliaElement *w,
				    FoliaElement *p,
				    const string& args ){
    if ( !p || !p->isinstance( Word_t ) )
      throw runtime_error( "insertword(): previous is not a Word " );
    if ( !w || !w->isinstance( Word_t ) )
      throw runtime_error( "insertword(): new word is not a Word " );
    Word *tmp = new Word( "text='dummy', id='dummy'" );
    tmp->setParent( this ); // we create a dummy Word as member of the
    // Sentence. This makes correctWords() happy
    vector<FoliaElement *>::iterator it = data.begin();
    while ( it != data.end() ){
      if ( *it == p ){
	it = data.insert( ++it, tmp );
	break;
      }
      ++it;
    }
    if ( it == data.end() )
      throw runtime_error( "insertword(): previous not found" );
    vector<FoliaElement *> ov;
    ov.push_back( *it );
    vector<FoliaElement *> nv;
    nv.push_back( w );
    return correctWords( ov, nv, getArgs(args) );
  }

  Correction *Sentence::correctWords( const vector<FoliaElement *>& orig,
				      const vector<FoliaElement *>& _new,
				      const KWargs& argsin ){
    // Generic correction method for words. You most likely want to use the helper functions
    //      splitword() , mergewords(), deleteword(), insertword() instead

    // sanity check:
    for ( const auto org : orig ){
      if ( !org || !org->isinstance( Word_t) )
	throw runtime_error("Original word is not a Word instance" );
      else if ( org->sentence() != this )
	throw runtime_error( "Original not found as member of sentence!");
    }
    for ( const auto nw : _new ){
      if ( ! nw->isinstance( Word_t) )
	throw runtime_error("new word is not a Word instance" );
    }
    KWargs::const_iterator ait = argsin.find("suggest");
    if ( ait != argsin.end() && ait->second == "true" ){
      FoliaElement *sugg = new Suggestion();
      for ( const auto nw : _new ){
	sugg->append( nw );
      }
      vector<FoliaElement *> nil1;
      vector<FoliaElement *> nil2;
      vector<FoliaElement *> sv;
      vector<FoliaElement *> tmp = orig;
      sv.push_back( sugg );
      KWargs args = argsin;
      args.erase("suggest");
      return correct( nil1, tmp, nil2, sv, args );
    }
    else {
      vector<FoliaElement *> nil1;
      vector<FoliaElement *> nil2;
      vector<FoliaElement *> o_tmp = orig;
      vector<FoliaElement *> n_tmp = _new;
      return correct( o_tmp, nil1, n_tmp, nil2, argsin );
    }
  }

  void TextContent::setAttributes( const KWargs& args ){
    KWargs kwargs = args; // need to copy
    KWargs::const_iterator it = kwargs.find( "value" );
    if ( it != kwargs.end() ) {
      XmlText *t = new XmlText();
      string value = it->second;
      if ( value.empty() ){
	// can this ever happen?
	throw ValueError( "TextContent: 'value' attribute may not be empty." );
      }
      t->setvalue( value );
      append( t );
      kwargs.erase(it);
    }
    it = kwargs.find( "offset" );
    if ( it != kwargs.end() ) {
      _offset = stringTo<int>(it->second);
      kwargs.erase(it);
    }
    else
      _offset = -1;
    it = kwargs.find( "ref" );
    if ( it != kwargs.end() ) {
      throw NotImplementedError( "ref attribute in TextContent" );
    }
    it = kwargs.find( "cls" );
    if ( it == kwargs.end() )
      it = kwargs.find( "class" );
    if ( it == kwargs.end() ) {
      kwargs["class"] = "current";
    }
    FoliaImpl::setAttributes(kwargs);
  }

  void PhonContent::setAttributes( const KWargs& args ){
    KWargs kwargs = args; // need to copy
    KWargs::const_iterator it = kwargs.find( "offset" );
    if ( it != kwargs.end() ) {
      _offset = stringTo<int>(it->second);
      kwargs.erase(it);
    }
    else
      _offset = -1;
    it = kwargs.find( "ref" );
    if ( it != kwargs.end() ) {
      throw NotImplementedError( "ref attribute in PhonContent" );
    }
    it = kwargs.find( "cls" );
    if ( it == kwargs.end() )
      it = kwargs.find( "class" );
    if ( it == kwargs.end() ) {
      kwargs["class"] = "current";
    }
    FoliaImpl::setAttributes(kwargs);
  }

  KWargs TextContent::collectAttributes() const {
    KWargs attribs = FoliaImpl::collectAttributes();
    if ( _class == "current" )
      attribs.erase( "class" );
    else if ( _class == "original" && parent() && parent()->isinstance( Original_t ) )
      attribs.erase( "class" );

    if ( _offset >= 0 ){
      attribs["offset"] = TiCC::toString( _offset );
    }
    return attribs;
  }

  KWargs PhonContent::collectAttributes() const {
    KWargs attribs = FoliaImpl::collectAttributes();
    if ( _class == "current" )
      attribs.erase( "class" );
    if ( _offset >= 0 ){
      attribs["offset"] = TiCC::toString( _offset );
    }
    return attribs;
  }

  TextContent *TextContent::postappend(){
    if ( _parent->isinstance( Original_t ) ){
      if ( _class == "current" )
	_class = "original";
    }
    return this;
  }

  vector<FoliaElement *>TextContent::findreplacables( FoliaElement *par ) const {
    vector<FoliaElement *> result;
    vector<TextContent*> v = par->FoliaElement::select<TextContent>( _set, false );
    // cerr << "TextContent::findreplacable found " << v << endl;
    for( const auto el:v ){
      // cerr << "TextContent::findreplacable bekijkt " << el << " ("
      if ( el->cls() == _class )
	result.push_back( el );
    }
    //  cerr << "TextContent::findreplacable resultaat " << v << endl;
    return result;
  }


  string TextContent::str( const string& cls ) const{
#ifdef DEBUG_TEXT
    cerr << "textContent::str(" << cls << ") this=" << this << endl;
#endif
    return UnicodeToUTF8(text(cls));
  }

  UnicodeString TextContent::text( const string& cls,
				   bool retaintok,
				   bool ) const {
    // get the UnicodeString value of underlying elements
    // default cls="current"
#ifdef DEBUG_TEXT
    cerr << "TextContent::TEXT(" << cls << ") " << endl;
#endif
    UnicodeString result;
    for( size_t i=0; i < data.size(); ++i ){
      // try to get text dynamically from children
#ifdef DEBUG_TEXT
      cerr << "TextContent: bekijk node[" << i+1 << "] " << data[i]->str(cls) << endl;
#endif
      try {
#ifdef DEBUG_TEXT
	cerr << "roep text(" << cls << ") aan op " << data[i] << endl;
#endif
	UnicodeString tmp = data[i]->text( cls, retaintok );
#ifdef DEBUG_TEXT
	cerr << "TextContent found '" << tmp << "'" << endl;
#endif
	result += tmp;
      } catch ( NoSuchText& e ){
#ifdef DEBUG_TEXT
	cerr << "TextContent::HELAAS" << endl;
#endif
      }
    }
    result.trim();
#ifdef DEBUG_TEXT
    cerr << "TextContent return " << result << endl;
#endif
    return result;
  }

  UnicodeString PhonContent::phon( const string& cls,
				   bool ) const {
    // get the UnicodeString value of underlying elements
    // default cls="current"
#ifdef DEBUG_PHON
    cerr << "PhonContent::PHON(" << cls << ") " << endl;
#endif
    UnicodeString result;
    for( size_t i=0; i < data.size(); ++i ){
      // try to get text dynamically from children
#ifdef DEBUG_PHON
      cerr << "PhonContent: bekijk node[" << i+1 << "] " << data[i]->str(cls) << endl;
#endif
      try {
#ifdef DEBUG_PHON
	cerr << "roep text(" << cls << ") aan op " << data[i] << endl;
#endif
	UnicodeString tmp = data[i]->text( cls );
#ifdef DEBUG_PHON
	cerr << "PhonContent found '" << tmp << "'" << endl;
#endif
	result += tmp;
      } catch ( NoSuchPhon& e ){
#ifdef DEBUG_TEXT
	cerr << "PhonContent::HELAAS" << endl;
#endif
      }
    }
    result.trim();
#ifdef DEBUG_PHON
    cerr << "PhonContent return " << result << endl;
#endif
    return result;
  }

  string AllowGenerateID::IDgen( const string& tag,
				 const FoliaElement* parent ){
    string nodeId = parent->id();
    if ( nodeId.empty() ){
      // search nearest parent WITH an id
      const FoliaElement *p = parent;
      while ( p && p->id().empty() )
	p = p->parent();
      nodeId = p->id();
    }
    //    cerr << "generateId," << tag << " nodeId = " << nodeId << endl;
    int max = getMaxId(tag);
    //    cerr << "MAX = " << max << endl;
    string id = nodeId + '.' + tag + '.' +  TiCC::toString( max + 1 );
    //    cerr << "new id = " << id << endl;
    return id;
  }

  void AllowGenerateID::setMaxId( FoliaElement *child ) {
    if ( !child->id().empty() && !child->xmltag().empty() ){
      vector<string> parts;
      size_t num = TiCC::split_at( child->id(), parts, "." );
      if ( num > 0 ){
	string val = parts[num-1];
	int i;
	try {
	  i = stringTo<int>( val );
	}
	catch ( exception ){
	  // no number, so assume so user defined id
	  return;
	}
	map<string,int>::iterator it = maxid.find( child->xmltag() );
	if ( it == maxid.end() ){
	  maxid[child->xmltag()] = i;
	}
	else {
	  if ( it->second < i ){
	    it->second = i;
	  }
	}
      }
    }
  }

  int AllowGenerateID::getMaxId( const string& xmltag ) {
    int res = 0;
    if ( !xmltag.empty() ){
      res = maxid[xmltag];
      ++maxid[xmltag];
    }
    return res;
  }

  Correction * AllowCorrection::correct( const vector<FoliaElement*>& _original,
					 const vector<FoliaElement*>& current,
					 const vector<FoliaElement*>& _newv,
					 const vector<FoliaElement*>& _suggestions,
					 const KWargs& args_in ){
    // cerr << "correct " << this << endl;
    // cerr << "original= " << original << endl;
    // cerr << "current = " << current << endl;
    // cerr << "new     = " << _new << endl;
    // cerr << "suggestions     = " << suggestions << endl;
    //  cerr << "args in     = " << args_in << endl;
    // Apply a correction
    Document *mydoc = doc();
    Correction *c = 0;
    bool suggestionsonly = false;
    bool hooked = false;
    FoliaElement * addnew = 0;
    KWargs args = args_in;
    vector<FoliaElement*> original = _original;
    vector<FoliaElement*> _new = _newv;
    vector<FoliaElement*> suggestions = _suggestions;
    KWargs::const_iterator it = args.find("new");
    if ( it != args.end() ){
      TextContent *t = new TextContent( mydoc, "value='" +  it->second + "'" );
      _new.push_back( t );
      args.erase( it );
    }
    it = args.find("suggestion");
    if ( it != args.end() ){
      TextContent *t = new TextContent( mydoc, "value='" +  it->second + "'" );
      suggestions.push_back( t );
      args.erase( it );
    }
    it = args.find("reuse");
    if ( it != args.end() ){
      // reuse an existing correction instead of making a new one
      try {
	c = dynamic_cast<Correction*>(mydoc->index(it->second));
      }
      catch ( exception& e ){
	throw ValueError("reuse= must point to an existing correction id!");
      }
      if ( !c->isinstance( Correction_t ) ){
	throw ValueError("reuse= must point to an existing correction id!");
      }
      hooked = true;
      suggestionsonly = (!c->hasNew() && !c->hasOriginal() && c->hasSuggestions() );
      if ( !_new.empty() && c->hasCurrent() ){
	// can't add new if there's current, so first set original to current, and then delete current

	if ( !current.empty() )
	  throw runtime_error( "Can't set both new= and current= !");
	if ( original.empty() ){
	  FoliaElement *cur = c->getCurrent();
	  original.push_back( cur );
	  c->remove( cur, false );
	}
      }
    }
    else {
      KWargs args2 = args;
      args2.erase("suggestion" );
      args2.erase("suggestions" );
      string id = generateId( "correction" );
      args2["id"] = id;
      c = new Correction(mydoc );
      c->setAttributes( args2 );
    }

    if ( !current.empty() ){
      if ( !original.empty() || !_new.empty() )
	throw runtime_error("When setting current=, original= and new= can not be set!");
      for ( const auto cur : current ){
	FoliaElement *add = new Current( mydoc );
	cur->setParent(0);
	add->append( cur );
	c->replace( add );
	if ( !hooked ) {
	  for ( size_t i=0; i < size(); ++i ){
	    if ( index(i) == cur ){
	      replace( index(i), c );
	      hooked = true;
	    }
	  }
	}
      }
    }
    if ( !_new.empty() ){
      //    cerr << "there is new! " << endl;
      addnew = new NewElement( mydoc );
      c->append(addnew);
      for ( const auto nw : _new ){
	nw->setParent(0);
	addnew->append( nw );
      }
      //    cerr << "after adding " << c << endl;
      vector<Current*> v = c->FoliaElement::select<Current>();
      //delete current if present
      for ( const auto cur:v ){
	c->remove( cur, false );
      }
    }
    if ( !original.empty() ){
      FoliaElement *add = new Original( mydoc );
      c->replace(add);
      for( const auto org: original ){
	bool dummyNode = ( org->id() == "dummy" );
	if ( !dummyNode ){
	  org->setParent(0);
	  add->append( org );
	}
	for ( size_t i=0; i < size(); ++i ){
	  if ( index(i) == org ){
	    if ( !hooked ) {
	      FoliaElement *tmp = replace( index(i), c );
	      if ( dummyNode )
		delete tmp;
	      hooked = true;
	    }
	    else
	      remove( org, false );
	  }
	}
      }
    }
    else if ( addnew ){
      // original not specified, find automagically:
      vector<FoliaElement *> orig;
      //    cerr << "start to look for original " << endl;
      for ( size_t i=0; i < len(addnew); ++ i ){
	FoliaElement *p = addnew->index(i);
	//      cerr << "bekijk " << p << endl;
	vector<FoliaElement*> v = p->findreplacables( this );
	for ( const auto el: v ){
	  orig.push_back( el );
	}
      }
      if ( orig.empty() ){
	throw runtime_error( "No original= specified and unable to automatically infer");
      }
      else {
	//      cerr << "we seem to have some originals! " << endl;
	FoliaElement *add = new Original( mydoc );
	c->replace(add);
	for ( const auto org: orig ){
	  //	cerr << " an original is : " << *oit << endl;
	  org->setParent( 0 );
	  add->append( org );
	  for ( size_t i=0; i < size(); ++i ){
	    if ( index(i) == org ){
	      if ( !hooked ) {
		replace( index(i), c );
		hooked = true;
	      }
	      else
		remove( org, false );
	    }
	  }
	}
	vector<Current*> v = c->FoliaElement::select<Current>();
	//delete current if present
	for( const auto cur: v ){
	  remove( cur, false );
	}
      }
    }

    if ( addnew ){
      for( const auto org : original ){
	c->remove( org, false );
      }
    }

    if ( !suggestions.empty() ){
      if ( !hooked )
	append(c);
      for ( const auto sug : suggestions ){
	if ( sug->isinstance( Suggestion_t ) ){
	  sug->setParent(0);
	  c->append( sug );
	}
	else {
	  FoliaElement *add = new Suggestion( mydoc );
	  sug->setParent(0);
	  add->append( sug );
	  c->append( add );
	}
      }
    }

    it = args.find("reuse");
    if ( it != args.end() ){
      if ( addnew && suggestionsonly ){
	vector<Suggestion*> sv = c->suggestions();
	for ( size_t i=0; i < sv.size(); ++i ){
	  if ( !c->annotator().empty() && sv[i]->annotator().empty() )
	    sv[i]->annotator( c->annotator() );
	  if ( !(c->annotatortype() == UNDEFINED) &&
	       (sv[i]->annotatortype() == UNDEFINED ) )
	    sv[i]->annotatortype( c->annotatortype() );
	}
      }
      it = args.find("annotator");
      if ( it != args.end() )
	c->annotator( it->second );
      it = args.find("annotatortype");
      if ( it != args.end() )
	c->annotatortype( stringTo<AnnotatorType>(it->second) );
      it = args.find("confidence");
      if ( it != args.end() )
	c->confidence( stringTo<double>(it->second) );

    }
    return c;
  }

  Correction *AllowCorrection::correct( const string& s ){
    vector<FoliaElement*> nil1;
    vector<FoliaElement*> nil2;
    vector<FoliaElement*> nil3;
    vector<FoliaElement*> nil4;
    KWargs args = getArgs( s );
    //    cerr << xmltag() << "::correct() <== " << this << endl;
    Correction *tmp = correct( nil1, nil2, nil3, nil4, args );
    //    cerr << xmltag() << "::correct() ==> " << this << endl;
    return tmp;
  }

  Correction *AllowCorrection::correct( FoliaElement *_old,
					FoliaElement *_new,
					const vector<FoliaElement*>& sugg,
					const KWargs& args ){
    vector<FoliaElement *> nv;
    nv.push_back( _new );
    vector<FoliaElement *> ov;
    ov.push_back( _old );
    vector<FoliaElement *> nil;
    //    cerr << xmltag() << "::correct() <== " << this << endl;
    Correction *tmp = correct( ov, nil, nv, sugg, args );
    //    cerr << xmltag() << "::correct() ==> " << this << endl;
    return tmp;
  }


  string AbstractStructureElement::str( const string& cls ) const{
    UnicodeString result = text( cls );
    return UnicodeToUTF8(result);
  }

  FoliaElement *AbstractStructureElement::append( FoliaElement *child ){
    FoliaImpl::append( child );
    setMaxId( child );
    return child;
  }

  vector<Paragraph*> AbstractStructureElement::paragraphs() const{
    return FoliaElement::select<Paragraph>( default_ignore_structure );
  }

  vector<Sentence*> AbstractStructureElement::sentences() const{
    return FoliaElement::select<Sentence>( default_ignore_structure );
  }

  vector<Word*> AbstractStructureElement::words() const{
    return FoliaElement::select<Word>( default_ignore_structure );
  }

  Sentence *AbstractStructureElement::sentences( size_t index ) const {
    vector<Sentence*> v = sentences();
    if ( index < v.size() )
      return v[index];
    else
      throw range_error( "sentences(): index out of range" );
  }

  Sentence *AbstractStructureElement::rsentences( size_t index ) const {
    vector<Sentence*> v = sentences();
    if ( index < v.size() )
      return v[v.size()-1-index];
    else
      throw range_error( "rsentences(): index out of range" );
  }

  Paragraph *AbstractStructureElement::paragraphs( size_t index ) const {
    vector<Paragraph*> v = paragraphs();
    if ( index < v.size() )
      return v[index];
    else
      throw range_error( "paragraphs(): index out of range" );
  }

  Paragraph *AbstractStructureElement::rparagraphs( size_t index ) const {
    vector<Paragraph*> v = paragraphs();
    if ( index < v.size() )
      return v[v.size()-1-index];
    else
      throw range_error( "rparagraphs(): index out of range" );
  }

  Word *AbstractStructureElement::words( size_t index ) const {
    vector<Word*> v = words();
    if ( index < v.size() )
      return v[index];
    else
      throw range_error( "words(): index out of range" );
  }

  Word *AbstractStructureElement::rwords( size_t index ) const {
    vector<Word*> v = words();
    if ( index < v.size() )
      return v[v.size()-1-index];
    else
      throw range_error( "rwords(): index out of range" );
  }

  const Word* AbstractStructureElement::resolveword( const string& id ) const{
    const Word *result = 0;
    for ( size_t i=0; i < data.size(); ++i ){
      result = data[i]->resolveword( id );
      if ( result )
	return result;
    }
    return result;
  }

  vector<Alternative *> AbstractStructureElement::alternatives( ElementType elt,
								const string& st ) const {
    // Return a list of alternatives, either all or only of a specific type, restrained by set
    static set<ElementType> excludeSet;
    if ( excludeSet.empty() ){
      excludeSet.insert( Original_t );
      excludeSet.insert( Suggestion_t );
    }
    vector<Alternative *> alts = FoliaElement::select<Alternative>( excludeSet );
    if ( elt == BASE ){
      return alts;
    }
    else {
      vector<Alternative*> res;
      for ( size_t i=0; i < alts.size(); ++i ){
	if ( alts[i]->size() > 0 ) { // child elements?
	  for ( size_t j =0; j < alts[i]->size(); ++j ){
	    if ( alts[i]->index(j)->element_id() == elt &&
		 ( alts[i]->sett().empty() || alts[i]->sett() == st ) ){
	      res.push_back( alts[i] ); // not the child!
	    }
	  }
	}
      }
      return res;
    }
  }

  KWargs AlignReference::collectAttributes() const {
    KWargs atts;
    atts["id"] = refId;
    atts["type"] = ref_type;
    if ( !_t.empty() )
      atts["t"] = _t;
    return atts;
  }

  void AlignReference::setAttributes( const KWargs& args ){
    KWargs::const_iterator it = args.find( "id" );
    if ( it != args.end() ){
      refId = it->second;
    }
    it = args.find( "type" );
    if ( it != args.end() ){
      try {
	stringTo<ElementType>( it->second );
      }
      catch (...){
	throw XmlError( "attribute 'type' must be an Element Type" );
      }
      ref_type = it->second;
    }
    else {
      throw XmlError( "attribute 'type' required for AlignReference" );
    }
    it = args.find( "t" );
    if ( it != args.end() ){
      _t = it->second;
    }
  }

  void Word::setAttributes( const KWargs& args_in ){
    KWargs args = args_in;
    KWargs::iterator it = args.find( "space" );
    if ( it != args.end() ){
      if ( it->second == "no" ){
	space = false;
      }
      args.erase( it );
    }
    it = args.find( "text" );
    if ( it != args.end() ) {
      settext( it->second );
      args.erase( it );
    }
    FoliaImpl::setAttributes( args );
  }

  KWargs Word::collectAttributes() const {
    KWargs atts = FoliaImpl::collectAttributes();
    if ( !space ){
      atts["space"] = "no";
    }
    return atts;
  }

  string Word::getTextDelimiter( bool retaintok ) const {
    if ( space || retaintok )
      return TEXTDELIMITER;
    else {
      return "";
    }
  }

  Correction *Word::split( FoliaElement *part1, FoliaElement *part2,
			   const string& args ){
    return sentence()->splitWord( this, part1, part2, getArgs(args) );
  }

  FoliaElement *Word::append( FoliaElement *child ) {
    if ( child->isSubClass( TokenAnnotation_t ) ){
      // sanity check, there may be no other child within the same set
      vector<FoliaElement*> v = select( child->element_id(), child->sett() );
      if ( v.empty() ) {
    	// OK!
    	return FoliaImpl::append( child );
      }
      delete child;
      throw DuplicateAnnotationError( "Word::append" );
    }
    return FoliaImpl::append( child );
  }

  Sentence *Word::sentence( ) const {
    // return the sentence this word is a part of, otherwise return null
    FoliaElement *p = _parent;
    while( p ){
      if ( p->isinstance( Sentence_t ) )
	return dynamic_cast<Sentence*>(p);
      p = p->parent();
    }
    return 0;
  }

  Paragraph *Word::paragraph( ) const {
    // return the sentence this word is a part of, otherwise return null
    FoliaElement *p = _parent;
    while( p ){
      if ( p->isinstance( Paragraph_t ) )
	return dynamic_cast<Paragraph*>(p);
      p = p->parent();
    }
    return 0;
  }

  Division *Word::division() const {
    // return the <div> this word is a part of, otherwise return null
    FoliaElement *p = _parent;
    while( p ){
      if ( p->isinstance( Division_t ) )
	return dynamic_cast<Division*>(p);
      p = p->parent();
    }
    return 0;
  }

  vector<Morpheme *> Word::morphemes( const string& set ) const {
    vector<Morpheme *> result;
    vector<MorphologyLayer*> mv = FoliaElement::select<MorphologyLayer>();
    for( size_t i=0; i < mv.size(); ++i ){
      vector<Morpheme*> tmp = mv[i]->FoliaElement::select<Morpheme>( set );
      result.insert( result.end(), tmp.begin(), tmp.end() );
    }
    return result;
  }

  Morpheme * Word::morpheme( size_t pos, const string& set ) const {
    vector<Morpheme *> tmp = morphemes( set );
    if ( pos < tmp.size() )
      return tmp[pos];
    else
      throw range_error( "morpheme() index out of range" );
  }

  Correction *Word::incorrection( ) const {
    // Is the Word part of a correction? If it is, it returns the Correction element, otherwise it returns 0;
    FoliaElement *p = _parent;
    while( p ){
      if ( p->isinstance( Correction_t ) )
	return dynamic_cast<Correction*>(p);
      else if ( p->isinstance( Sentence_t ) )
	break;
      p = p->parent();
    }
    return 0;
  }

  Word *Word::previous() const{
    Sentence *s = sentence();
    vector<Word*> words = s->words();
    for( size_t i=0; i < words.size(); ++i ){
      if ( words[i] == this ){
	if ( i > 0 )
	  return words[i-1];
	else
	  return 0;
	break;
      }
    }
    return 0;
  }

  Word *Word::next() const{
    Sentence *s = sentence();
    vector<Word*> words = s->words();
    for( size_t i=0; i < words.size(); ++i ){
      if ( words[i] == this ){
	if ( i+1 < words.size() )
	  return words[i+1];
	else
	  return 0;
	break;
      }
    }
    return 0;
  }

  vector<Word*> Word::context( size_t size,
			       const string& val ) const {
    vector<Word*> result;
    if ( size > 0 ){
      vector<Word*> words = mydoc->words();
      for( size_t i=0; i < words.size(); ++i ){
	if ( words[i] == this ){
	  size_t miss = 0;
	  if ( i < size ){
	    miss = size - i;
	  }
	  for ( size_t index=0; index < miss; ++index ){
	    if ( val.empty() )
	      result.push_back( 0 );
	    else {
	      PlaceHolder *p = new PlaceHolder( "text='" + val + "'" );
	      mydoc->keepForDeletion( p );
	      result.push_back( p );
	    }
	  }
	  for ( size_t index=i-size+miss; index < i + size + 1; ++index ){
	    if ( index < words.size() ){
	      result.push_back( words[index] );
	    }
	    else {
	      if ( val.empty() )
		result.push_back( 0 );
	      else {
		PlaceHolder *p = new PlaceHolder( "text='" + val + "'" );
		mydoc->keepForDeletion( p );
		result.push_back( p );
	      }
	    }
	  }
	  break;
	}
      }
    }
    return result;
  }


  vector<Word*> Word::leftcontext( size_t size,
				   const string& val ) const {
    //  cerr << "leftcontext : " << size << endl;
    vector<Word*> result;
    if ( size > 0 ){
      vector<Word*> words = mydoc->words();
      for( size_t i=0; i < words.size(); ++i ){
	if ( words[i] == this ){
	  size_t miss = 0;
	  if ( i < size ){
	    miss = size - i;
	  }
	  for ( size_t index=0; index < miss; ++index ){
	    if ( val.empty() )
	      result.push_back( 0 );
	    else {
	      PlaceHolder *p = new PlaceHolder( "text='" + val + "'" );
	      mydoc->keepForDeletion( p );
	      result.push_back( p );
	    }
	  }
	  for ( size_t index=i-size+miss; index < i; ++index ){
	    result.push_back( words[index] );
	  }
	  break;
	}
      }
    }
    return result;
  }

  vector<Word*> Word::rightcontext( size_t size,
				    const string& val ) const {
    vector<Word*> result;
    //  cerr << "rightcontext : " << size << endl;
    if ( size > 0 ){
      vector<Word*> words = mydoc->words();
      size_t begin;
      size_t end;
      for( size_t i=0; i < words.size(); ++i ){
	if ( words[i] == this ){
	  begin = i + 1;
	  end = begin + size;
	  for ( ; begin < end; ++begin ){
	    if ( begin >= words.size() ){
	      if ( val.empty() )
		result.push_back( 0 );
	      else {
		PlaceHolder *p = new PlaceHolder( "text='" + val + "'" );
		mydoc->keepForDeletion( p );
		result.push_back( p );
	      }
	    }
	    else
	      result.push_back( words[begin] );
	  }
	  break;
	}
      }
    }
    return result;
  }

  const Word* Word::resolveword( const string& id ) const {
    if ( _id == id )
      return this;
    return 0;
  }

  ElementType layertypeof( ElementType et ){
    switch( et ){
    case Entity_t:
    case Entities_t:
      return Entities_t;
    case Chunk_t:
    case Chunking_t:
      return Chunking_t;
    case SyntacticUnit_t:
    case SyntaxLayer_t:
      return SyntaxLayer_t;
    case TimeSegment_t:
    case TimingLayer_t:
      return TimingLayer_t;
    case Morpheme_t:
    case MorphologyLayer_t:
      return MorphologyLayer_t;
    case Phoneme_t:
    case PhonologyLayer_t:
      return PhonologyLayer_t;
    case CoreferenceChain_t:
    case Coreferences_t:
      return Coreferences_t;
    case Semroles_t:
    case Semrole_t:
      return Semroles_t;
    case Dependencies_t:
    case Dependency_t:
      return Dependencies_t;
    default:
      return BASE;
    }
  }

  vector<AbstractSpanAnnotation*> Word::findspans( ElementType et,
						   const string& st ) const {
    ElementType layertype = layertypeof( et );
    vector<AbstractSpanAnnotation *> result;
    if ( layertype != BASE ){
      const FoliaElement *e = parent();
      if ( e ){
	vector<FoliaElement*> v = e->select( layertype, st, false );
	for ( size_t i=0; i < v.size(); ++i ){
	  for ( size_t k=0; k < v[i]->size(); ++k ){
	    FoliaElement *f = v[i]->index(k);
	    AbstractSpanAnnotation *as = dynamic_cast<AbstractSpanAnnotation*>(f);
	    if ( as ){
	      vector<FoliaElement*> wv = f->wrefs();
	      for ( size_t j=0; j < wv.size(); ++j ){
		if ( wv[j] == this ){
		  result.push_back(as);
		}
	      }
	    }
	  }
	}
      }
    }
    return result;
  }

  FoliaElement* WordReference::parseXml( const xmlNode *node ){
    KWargs att = getAttributes( node );
    string id = att["id"];
    if ( id.empty() )
      throw XmlError( "empty id in WordReference" );
    if ( mydoc->debug )
      cerr << "Found word reference" << id << endl;
    FoliaElement *res = (*mydoc)[id];
    if ( res ){
      // To DO: check type. Word_t and Morpheme_t
      res->increfcount();
    }
    else {
      if ( mydoc->debug )
	cerr << "...Unresolvable!" << endl;
      throw XmlError( "Unresolvable id " + id + "in WordReference" );
    }
    delete this;
    return res;
  }

  FoliaElement* AlignReference::parseXml( const xmlNode *node ){
    KWargs att = getAttributes( node );
    string val = att["id"];
    if ( val.empty() )
      throw XmlError( "ID required for AlignReference" );
    refId = val;
    if ( mydoc->debug )
      cerr << "Found AlignReference ID " << refId << endl;
    val = att["type"];
    if ( val.empty() )
      throw XmlError( "type required for AlignReference" );
    try {
      stringTo<ElementType>( val );
    }
    catch (...){
      throw XmlError( "AlignReference:type must be an Element Type ("
		      + val + ")" );
    }
    ref_type = val;
    val = att["t"];
    if ( !val.empty() ){
      _t = val;
    }
    return this;
  }

  FoliaElement *AlignReference::resolve( const Alignment *ref ) const {
    if ( ref->href().empty() )
      return (*mydoc)[refId];
    else
      throw NotImplementedError( "AlignReference::resolve() for external doc" );
  }

  vector<FoliaElement *> Alignment::resolve() const {
    vector<AlignReference*> v = FoliaElement::select<AlignReference>();
    vector<FoliaElement*> result;
    for ( size_t i=0; i < v.size(); ++i ){
      result.push_back( v[i]->resolve( this ) );
    }
    return result;
  }

  void PlaceHolder::setAttributes( const KWargs& args ){
    KWargs::const_iterator it = args.find( "text" );
    if ( it == args.end() ){
      throw ValueError("text attribute is required for " + classname() );
    }
    else if ( args.size() != 1 ){
      throw ValueError("only the text attribute is supported for " + classname() );
    }
    Word::setAttributes( args );
  }

  KWargs Figure::collectAttributes() const {
    KWargs atts = FoliaImpl::collectAttributes();
    if ( !_src.empty() ){
      atts["src"] = _src;
    }
    return atts;
  }

  void Figure::setAttributes( const KWargs& kwargsin ){
    KWargs kwargs = kwargsin;
    KWargs::const_iterator it;
    it = kwargs.find( "src" );
    if ( it != kwargs.end() ) {
      _src = it->second;
      kwargs.erase( it );
    }
    FoliaImpl::setAttributes(kwargs);
  }

  UnicodeString Figure::caption() const {
    vector<FoliaElement *> v = select(Caption_t);
    if ( v.empty() )
      throw NoSuchText("caption");
    else
      return v[0]->text();
  }

  void Description::setAttributes( const KWargs& kwargs ){
    KWargs::const_iterator it;
    it = kwargs.find( "value" );
    if ( it == kwargs.end() ) {
      throw ValueError("value attribute is required for " + classname() );
    }
    _value = it->second;
  }

  xmlNode *Description::xml( bool, bool ) const {
    xmlNode *e = FoliaImpl::xml( false, false );
    xmlAddChild( e, xmlNewText( (const xmlChar*)_value.c_str()) );
    return e;
  }

  FoliaElement* Description::parseXml( const xmlNode *node ){
    KWargs att = getAttributes( node );
    KWargs::const_iterator it = att.find("value" );
    if ( it == att.end() ){
      att["value"] = XmlContent( node );
    }
    setAttributes( att );
    return this;
  }

  FoliaElement *AbstractSpanAnnotation::append( FoliaElement *child ){
    FoliaImpl::append( child );
    if ( child->isinstance(PlaceHolder_t) ){
      child->increfcount();
    }
    return child;
  }

  void AbstractAnnotationLayer::assignset( FoliaElement *child ){
    // If there is no set (yet), try to get the set form the child
    // but not if it is the default set.
    // for a Correction child, we look deeper.
    if ( _set.empty() ){
      if ( child->isSubClass( SpanAnnotation_t ) ){
	string st = child->sett();
	if ( !st.empty()
	     && mydoc->defaultset( child->annotation_type() ) != st ){
	  _set = st;
	}
      }
      else if ( child->isinstance(Correction_t) ){
	Original *org = child->getOriginal();
	if ( org ){
	  for ( size_t i=0; i < org->size(); ++i ){
	    FoliaElement *el = org->index(i);
	    if ( el->isSubClass( SpanAnnotation_t ) ){
	      string st = el->sett();
	      if ( !st.empty()
		   && mydoc->defaultset( el->annotation_type() ) != st ){
		_set = st;
		return;
	      }
	    }
	  }
	}
	NewElement *nw = child->getNew();
	if ( nw ){
	  for ( size_t i=0; i < nw->size(); ++i ){
	    FoliaElement *el = nw->index(i);
	    if ( el->isSubClass( SpanAnnotation_t ) ){
	      string st = el->sett();
	      if ( !st.empty()
		   && mydoc->defaultset( el->annotation_type() ) != st ){
		_set = st;
		return;
	      }
	    }
	  }
	}
	auto v = child->suggestions();
	for ( auto el : v ){
	  if ( el->isSubClass( SpanAnnotation_t ) ){
	    string st = el->sett();
	    if ( !st.empty()
		 && mydoc->defaultset( el->annotation_type() ) != st ){
	      _set = st;
	      return;
	    }
	  }
	}
      }
    }
  }

  FoliaElement *AbstractAnnotationLayer::append( FoliaElement *child ){
    FoliaImpl::append( child );
    assignset( child );
    return child;
  }

  xmlNode *AbstractSpanAnnotation::xml( bool recursive, bool kanon ) const {
    xmlNode *e = FoliaImpl::xml( false, false );
    // append Word and Morpheme children as WREFS
    vector<FoliaElement*>::const_iterator it=data.begin();
    while ( it != data.end() ){
      if ( (*it)->element_id() == Word_t ||
	   (*it)->element_id() == Morpheme_t ){
	xmlNode *t = XmlNewNode( foliaNs(), "wref" );
	KWargs attribs;
	attribs["id"] = (*it)->id();
	string txt = (*it)->str();
	if ( !txt.empty() )
	  attribs["t"] = txt;
	addAttributes( t, attribs );
	xmlAddChild( e, t );
      }
      else {
	string at = tagToAtt( *it );
	if ( at.empty() ){
	  // otherwise handled by FoliaElement::xml() above
	  xmlAddChild( e, (*it)->xml( recursive, kanon ) );
	}
      }
      ++it;
    }
    return e;
  }

  FoliaElement *Quote::append( FoliaElement *child ){
    FoliaImpl::append( child );
    if ( child->isinstance(Sentence_t) )
      child->setAuth( false ); // Sentences under quotes are non-authoritative
    return child;
  }


  xmlNode *Content::xml( bool recurse, bool ) const {
    xmlNode *e = FoliaImpl::xml( recurse, false );
    xmlAddChild( e, xmlNewCDataBlock( 0,
				      (const xmlChar*)value.c_str() ,
				      value.length() ) );
    return e;
  }

  FoliaElement* Content::parseXml( const xmlNode *node ){
    KWargs att = getAttributes( node );
    setAttributes( att );
    xmlNode *p = node->children;
    bool isCdata = false;
    bool isText = false;
    while ( p ){
      if ( p->type == XML_CDATA_SECTION_NODE ){
	if ( isText )
	  throw XmlError( "intermixing text and CDATA in Content node" );
	isCdata = true;
	value += (char*)p->content;
      }
      else if ( p->type == XML_TEXT_NODE ){
	if ( isCdata )
	  throw XmlError( "intermixing text and CDATA in Content node" );
	isText = true;
	value += (char*)p->content;
      }
      else if ( p->type == XML_COMMENT_NODE ){
	string tag = "xml-comment";
	FoliaElement *t = createElement( mydoc, tag );
	if ( t ){
	  t = t->parseXml( p );
	  append( t );
	}
      }
      p = p->next;
    }
    if ( value.empty() )
      throw XmlError( "CDATA or Text expected in Content node" );
    return this;
  }

  UnicodeString Correction::text( const string& cls,
				  bool retaintok,
				  bool ) const {
#ifdef DEBUG_TEXT
    cerr << "TEXT(" << cls << ") op node : " << _xmltag << " id ( " << id() << ")" << endl;
#endif
    if ( cls == "current" ){
      for( size_t i=0; i < data.size(); ++i ){
#ifdef DEBUG_TEXT
	cerr << "data[" << i << "]=" << data[i] << endl;
#endif
	if ( data[i]->isinstance( New_t ) || data[i]->isinstance( Current_t ) )
	  return data[i]->text( cls, retaintok );
      }
    }
    else if ( cls == "original" ){
      for( size_t i=0; i < data.size(); ++i ){
#ifdef DEBUG_TEXT
	cerr << "data[" << i << "]=" << data[i] << endl;
#endif
	if ( data[i]->isinstance( Original_t ) )
	  return data[i]->text( cls, retaintok );
      }
    }
    throw NoSuchText("wrong cls");
  }

  string Correction::getTextDelimiter( bool retaintok ) const {
    for( size_t i=0; i < data.size(); ++i ){
      //    cerr << "data[" << i << "]=" << data[i] << endl;
      if ( data[i]->isinstance( New_t ) || data[i]->isinstance( Current_t ) )
	return data[i]->getTextDelimiter( retaintok );
    }
    return "";
  }

  TextContent *Correction::textcontent( const string& cls ) const {
    if ( cls == "current" ){
      for( size_t i=0; i < data.size(); ++i ){
	//    cerr << "data[" << i << "]=" << data[i] << endl;
	if ( data[i]->isinstance( New_t ) || data[i]->isinstance( Current_t ) )
	  return data[i]->textcontent( cls );
      }
    }
    else if ( cls == "original" ){
      for( size_t i=0; i < data.size(); ++i ){
	//    cerr << "data[" << i << "]=" << data[i] << endl;
	if ( data[i]->isinstance( Original_t ) )
	  return data[i]->textcontent( cls );;
      }
    }
    throw NoSuchText("wrong cls");
  }

  PhonContent *Correction::phoncontent( const string& cls ) const {
    if ( cls == "current" ){
      for( size_t i=0; i < data.size(); ++i ){
	//    cerr << "data[" << i << "]=" << data[i] << endl;
	if ( data[i]->isinstance( New_t ) || data[i]->isinstance( Current_t ) )
	  return data[i]->phoncontent( cls );
      }
    }
    else if ( cls == "original" ){
      for( size_t i=0; i < data.size(); ++i ){
	//    cerr << "data[" << i << "]=" << data[i] << endl;
	if ( data[i]->isinstance( Original_t ) )
	  return data[i]->phoncontent( cls );;
      }
    }
    throw NoSuchPhon("wrong cls");
  }

  bool Correction::hasNew( ) const {
    vector<FoliaElement*> v = select( New_t, false );
    return !v.empty();
  }

  NewElement *Correction::getNew() const {
    vector<NewElement*> v = FoliaElement::select<NewElement>( false );
    if ( v.empty() )
      throw NoSuchAnnotation("new");
    else
      return v[0];
  }

  FoliaElement *Correction::getNew( size_t index ) const {
    NewElement *n = getNew();
    return n->index(index);
  }

  bool Correction::hasOriginal() const {
    vector<FoliaElement*> v = select( Original_t, false );
    return !v.empty();
  }

  Original *Correction::getOriginal() const {
    vector<Original*> v = FoliaElement::select<Original>( false );
    if ( v.empty() )
      throw NoSuchAnnotation("original");
    else
      return v[0];
  }

  FoliaElement *Correction::getOriginal( size_t index ) const {
    Original *n = getOriginal();
    return n->index(index);
  }

  bool Correction::hasCurrent( ) const {
    vector<FoliaElement*> v = select( Current_t, false );
    return !v.empty();
  }

  Current *Correction::getCurrent( ) const {
    vector<Current*> v = FoliaElement::select<Current>( false );
    if ( v.empty() )
      throw NoSuchAnnotation("current");
    else
      return v[0];
  }

  FoliaElement *Correction::getCurrent( size_t index ) const {
    Current *n = getCurrent();
    return n->index(index);
  }

  bool Correction::hasSuggestions( ) const {
    vector<Suggestion*> v = suggestions();
    return !v.empty();
  }

  vector<Suggestion*> Correction::suggestions( ) const {
    return FoliaElement::select<Suggestion>( false );
  }

  Suggestion *Correction::suggestions( size_t index ) const {
    vector<Suggestion*> v = suggestions();
    if ( v.empty() || index >= v.size() )
      throw NoSuchAnnotation("suggestion");
    return v[index];
  }

  Head *Division::head() const {
    if ( data.size() > 0 ||
	 data[0]->element_id() == Head_t ){
      return dynamic_cast<Head*>(data[0]);
    }
    else
      throw runtime_error( "No head" );
    return 0;
  }

  string Gap::content() const {
    vector<FoliaElement*> cv = select( Content_t );
    if ( cv.size() < 1 )
      throw NoSuchAnnotation( "content" );
    else {
      return cv[0]->content();
    }
  }

  Headwords *Dependency::head() const {
    vector<Headwords*> v = FoliaElement::select<Headwords>();
    if ( v.size() < 1 )
      throw NoSuchAnnotation( "head" );
    else {
      return v[0];
    }
  }

  DependencyDependent *Dependency::dependent() const {
    vector<DependencyDependent *> v = FoliaElement::select<DependencyDependent>();
    if ( v.size() < 1 )
      throw NoSuchAnnotation( "dependent" );
    else {
      return v[0];
    }
  }

  void FoLiA::init(){
    _xmltag="FoLiA";
    _element_id = BASE;
    _accepted_data = { Text_t, Speech_t };
  }

  void DCOI::init(){
    _xmltag="DCOI";
    _element_id = BASE;
    _accepted_data = { Text_t, Speech_t };
  }

  void AbstractStructureElement::init(){
    _element_id = Structure_t;
    _xmltag = "structure";
    _required_attributes = ID;
    _optional_attributes = ALL;
    _occurrences_per_set=0;
    PRINTABLE = true;
    SPEAKABLE = true;
    TEXTDELIMITER = "\n\n";
  }

  void AbstractTokenAnnotation::init(){
    _element_id = TokenAnnotation_t;
    _xmltag="tokenannotation";
    _required_attributes = CLASS;
    _optional_attributes = ALL;
    _occurrences_per_set=1;
  }

  void TextContent::init(){
    _element_id = TextContent_t;
    _xmltag="t";
    _optional_attributes = CLASS|ANNOTATOR|CONFIDENCE|DATETIME;
    _accepted_data = { AbstractTextMarkup_t,
		       XmlText_t,
		       LineBreak_t };
    _annotation_type = AnnotationType::TEXT;
    _occurrences = 0;
    _occurrences_per_set=0;
    _offset = -1;
    PRINTABLE = true;
    XLINK = true;
  }

  void PhonContent::init(){
    _element_id = PhonContent_t;
    _xmltag="ph";
    _optional_attributes = CLASS|ANNOTATOR|CONFIDENCE|DATETIME;
    _accepted_data = { XmlText_t };
    _annotation_type = AnnotationType::PHON;
    _occurrences = 0;
    _occurrences_per_set=0;
    PRINTABLE = false;
    SPEAKABLE = true;
  }

  void Head::init() {
    _element_id = Head_t;
    _xmltag="head";
    _accepted_data = { Structure_t, Description_t, LineBreak_t,
		       TextContent_t, Alignment_t, Metric_t,
		       AlternativeLayers_t, TokenAnnotation_t, Gap_t };
    _occurrences=1;
    TEXTDELIMITER = " ";
  }

  void TableHead::init() {
    _element_id = TableHead_t;
    _xmltag="tablehead";
    _required_attributes = NO_ATT;
    _accepted_data ={ Row_t,
		      Correction_t,
		      AlternativeLayers_t,
		      AnnotationLayer_t,
		      Part_t };
    _annotation_type = AnnotationType::TABLE;
  }

  void Table::init() {
    _element_id = Table_t;
    _xmltag="table";
    _accepted_data = { TableHead_t,
		       Row_t,
		       Correction_t,
		       AlternativeLayers_t,
		       AnnotationLayer_t,
		       Part_t };
    _annotation_type = AnnotationType::TABLE;
  }

  void Cell::init() {
    _element_id = Cell_t;
    _xmltag="cell";
    _required_attributes = NO_ATT;
    _accepted_data = { Structure_t,
		       TokenAnnotation_t,
		       Entities_t,
		       Alignment_t,
		       Metric_t,
		       AlternativeLayers_t,
		       AnnotationLayer_t,
		       Gap_t };
    _annotation_type = AnnotationType::TABLE;
    TEXTDELIMITER = " | ";
  }

  void Row::init() {
    _element_id = Row_t;
    _xmltag="row";
    _required_attributes = NO_ATT;
    _accepted_data = { Cell_t,
		       Correction_t,
		       AlternativeLayers_t,
		       AnnotationLayer_t,
		       Part_t };
    _annotation_type = AnnotationType::TABLE;
    TEXTDELIMITER = "\n";
  }

  void LineBreak::init(){
    _xmltag = "br";
    _element_id = LineBreak_t;
    _required_attributes = NO_ATT;
    _annotation_type = AnnotationType::LINEBREAK;
  }

  void WhiteSpace::init(){
    _xmltag = "whitespace";
    _element_id = WhiteSpace_t;
    _required_attributes = NO_ATT;
    _annotation_type = AnnotationType::WHITESPACE;
  }

  void Word::init(){
    _xmltag="w";
    _element_id = Word_t;
    _accepted_data = { TextContent_t,
		       PhonContent_t,
		       PhonologyLayer_t,
		       TokenAnnotation_t,
		       Alternative_t,
		       Description_t,
		       Alignment_t,
		       Metric_t };
    _annotation_type = AnnotationType::TOKEN;
    TEXTDELIMITER = " ";
    space = true;
  }

  void String::init(){
    _xmltag="str";
    _element_id = String_t;
    _required_attributes = NO_ATT;
    _optional_attributes = ID|CLASS|ANNOTATOR|CONFIDENCE|DATETIME;
    _accepted_data = { TextContent_t, Correction_t, Lang_t,
		       Description_t, Alignment_t, Metric_t };
    _annotation_type = AnnotationType::STRING;
    _occurrences = 0;
    _occurrences_per_set=0;
    PRINTABLE = true;
    SPEAKABLE = true;
  }

  void Part::init(){
    _xmltag="part";
    _element_id = Part_t;
    _required_attributes = NO_ATT;
    _accepted_data = { Structure_t, TextContent_t,
		       TokenAnnotation_t,
		       Description_t };
    _annotation_type = AnnotationType::PART;
    TEXTDELIMITER = " ";
  }

  void PlaceHolder::init(){
    _xmltag="placeholder";
    _element_id = PlaceHolder_t;
    _required_attributes = NO_ATT;
  }

  void WordReference::init(){
    _required_attributes = ID;
    _xmltag = "wref";
    _element_id = WordReference_t;
    _auth = false;
  }

  void Alignment::init(){
    _optional_attributes = ALL;
    _xmltag = "alignment";
    _element_id = Alignment_t;
    _accepted_data = { AlignReference_t, Description_t, Metric_t };
    _occurrences_per_set=0;
    _annotation_type = AnnotationType::ALIGNMENT;
    XLINK = true;
  }

  void AlignReference::init(){
    _xmltag = "aref";
    _element_id = AlignReference_t;
  }


  void Gap::init(){
    _xmltag = "gap";
    _element_id = Gap_t;
    _annotation_type = AnnotationType::GAP;
    _accepted_data = { Content_t, Description_t, Part_t };
    _optional_attributes = CLASS|ID|ANNOTATOR|CONFIDENCE|N|DATETIME;
  }

  void MetricAnnotation::init(){
    _element_id = Metric_t;
    _xmltag = "metric";
    _accepted_data = { ValueFeature_t, Description_t };
    _optional_attributes = CLASS|ANNOTATOR|CONFIDENCE;
    _annotation_type = AnnotationType::METRIC;
  }

  void Content::init(){
    _xmltag = "content";
    _element_id = Content_t;
    _optional_attributes = ALL;
    _occurrences_per_set=0;
    PRINTABLE = true;
    SPEAKABLE = true;
  }

  void Sentence::init(){
    _xmltag="s";
    _element_id = Sentence_t;
    _accepted_data = { Structure_t,
		       TokenAnnotation_t,
		       TextContent_t, AnnotationLayer_t,
		       Correction_t,
		       Description_t, Alignment_t, Metric_t,
		       Gap_t };
    _annotation_type = AnnotationType::SENTENCE;
    TEXTDELIMITER = " ";
  }

  void Division::init(){
    _xmltag="div";
    _element_id = Division_t;
    _required_attributes = ID;
    _optional_attributes = CLASS|N|SRC|BEGINTIME|ENDTIME|SPEAKER;
    _accepted_data = { Division_t, Quote_t, Gap_t, Event_t, Example_t,
		       Entry_t, Head_t, Utterance_t, Paragraph_t,
		       Sentence_t, List_t, Figure_t, Table_t, Note_t,
		       Reference_t, TokenAnnotation_t, Description_t,
		       LineBreak_t, WhiteSpace_t, Alternative_t, TextContent_t,
		       AlternativeLayers_t, AnnotationLayer_t,
		       Correction_t, Part_t };
    _annotation_type = AnnotationType::DIVISION;
    TEXTDELIMITER = "\n\n\n";
  }

  void Text::init(){
    _xmltag="text";
    _element_id = Text_t;
    _accepted_data = { Gap_t, Division_t, Paragraph_t, Sentence_t,
		       List_t, Figure_t, Description_t, Event_t,
		       TokenAnnotation_t, Quote_t, Word_t, Table_t,
		       Note_t, Reference_t,
		       AnnotationLayer_t, String_t,
		       Correction_t, TextContent_t, Metric_t, External_t,
		       Part_t };
    _required_attributes = ID;
    TEXTDELIMITER = "\n\n";
  }

  void Speech::init(){
    _xmltag="speech";
    _element_id = Speech_t;
    _accepted_data = { Utterance_t, Gap_t, Event_t, Entry_t, Example_t,
		       Division_t, Paragraph_t, Quote_t, Sentence_t,
		       Word_t, List_t, Note_t, Reference_t, AnnotationLayer_t,
		       TokenAnnotation_t, Description_t, TextContent_t,
		       PhonContent_t, String_t, Metric_t, Correction_t };
    _required_attributes = ID;
    _optional_attributes = N|SRC|BEGINTIME|ENDTIME|SPEAKER;
    TEXTDELIMITER = "\n\n";
  }

  void Utterance::init(){
    _xmltag="utt";
    _element_id = Utterance_t;
    _accepted_data = { Word_t, Sentence_t, Quote_t, TokenAnnotation_t,
		       Correction_t, TextContent_t, PhonContent_t,
		       String_t, Gap_t, Description_t, Note_t, Reference_t,
		       Alignment_t, Metric_t, Alternative_t,
		       AlternativeLayers_t, AnnotationLayer_t, Part_t };
    _annotation_type = AnnotationType::UTTERANCE;
    TEXTDELIMITER = " ";
    PRINTABLE = true;
    SPEAKABLE = true;
  }

  void Event::init(){
    _xmltag="event";
    _element_id = Event_t;
    _accepted_data = { Gap_t, Division_t, Structure_t,
		       Description_t,
		       Feature_t, TextContent_t,
		       Part_t };
    _annotation_type = AnnotationType::EVENT;
    _occurrences_per_set=0;
  }

  void TimeSegment::init(){
    _xmltag="timesegment";
    _element_id = TimeSegment_t;
    _accepted_data = { Description_t, Feature_t, Word_t };
    _annotation_type = AnnotationType::TIMESEGMENT;
    _occurrences_per_set=0;
  }

  void Caption::init(){
    _xmltag="caption";
    _element_id = Caption_t;
    _accepted_data = { Sentence_t, Reference_t, Description_t,
		       TokenAnnotation_t, TextContent_t,
		       Correction_t, Part_t };
    _occurrences = 1;
  }

  void Label::init(){
    _xmltag="label";
    _element_id = Label_t;
    _accepted_data = { Word_t, Description_t, TextContent_t,
		       TokenAnnotation_t, Alignment_t,
		       Correction_t, Part_t };
  }

  void Item::init(){
    _xmltag="item";
    _element_id = Item_t;
    _accepted_data = { Structure_t, Description_t,
		       TokenAnnotation_t,
		       TextContent_t, String_t, Alignment_t,
		       Gap_t,
		       Correction_t };
    _annotation_type = AnnotationType::LIST;
  }

  void List::init(){
    _xmltag="list";
    _element_id = List_t;
    _accepted_data = { Item_t, Description_t,
		       Caption_t, Event_t, Lang_t,
		       Alignment_t,
		       Correction_t, Part_t };
    _annotation_type = AnnotationType::LIST;
    TEXTDELIMITER="\n";
  }

  void Figure::init(){
    _xmltag="figure";
    _element_id = Figure_t;
    _accepted_data = { Sentence_t, Description_t,
		       Caption_t, String_t, Lang_t,
		       TextContent_t,
		       Correction_t, Part_t };
    _annotation_type = AnnotationType::FIGURE;
  }

  void Paragraph::init(){
    _xmltag="p";
    _element_id = Paragraph_t;
    _accepted_data = { Structure_t,
		       TextContent_t,
		       TokenAnnotation_t,
		       Description_t,
		       Alignment_t,
		       Metric_t,
		       Gap_t };
    _annotation_type = AnnotationType::PARAGRAPH;
  }

  void SyntacticUnit::init(){
    _xmltag = "su";
    _element_id = SyntacticUnit_t;
    _required_attributes = NO_ATT;
    _annotation_type = AnnotationType::SYNTAX;
    _accepted_data = { SyntacticUnit_t, Word_t, WordReference_t,
		       Description_t, Feature_t };
  }

  void SemanticRole::init(){
    _xmltag = "semrole";
    _element_id = Semrole_t;
    _required_attributes = CLASS;
    _annotation_type = AnnotationType::SEMROLE;
    _accepted_data = { Word_t, WordReference_t, Lang_t, Headwords_t,
		       Alignment_t, Description_t, Metric_t };
  }

  void Chunk::init(){
    _required_attributes = NO_ATT;
    _xmltag = "chunk";
    _element_id = Chunk_t;
    _annotation_type = AnnotationType::CHUNKING;
    _accepted_data = { Word_t, WordReference_t, Lang_t,
		       Description_t, Feature_t };
  }

  void Entity::init(){
    _required_attributes = NO_ATT;
    _optional_attributes = ID|CLASS|ANNOTATOR|CONFIDENCE|DATETIME;
    _xmltag = "entity";
    _element_id = Entity_t;
    _annotation_type = AnnotationType::ENTITY;
    _accepted_data = { Word_t, Lang_t, WordReference_t, Morpheme_t,
		       Description_t, Feature_t, Metric_t };
  }

  void AbstractAnnotationLayer::init(){
    _xmltag = "annotationlayer";
    _element_id = AnnotationLayer_t;
    _optional_attributes = SETONLY;
  }

  static set<ElementType> asSet = { SyntacticUnit_t, Chunk_t,
				    Entity_t, Headwords_t,
				    DependencyDependent_t,
				    CoreferenceLink_t,
				    CoreferenceChain_t, Semrole_t,
				    Semroles_t, TimeSegment_t };

  vector<AbstractSpanAnnotation*> FoliaImpl::selectSpan() const {
    vector<AbstractSpanAnnotation*> res;
    for ( const auto el : asSet ){
      vector<FoliaElement*> tmp = select( el, true );
      for ( size_t i = 0; i < tmp.size(); ++i ){
	res.push_back( dynamic_cast<AbstractSpanAnnotation*>( tmp[i]) );
      }
    }
    return res;
  }

  vector<FoliaElement*> AbstractSpanAnnotation::wrefs() const {
    vector<FoliaElement*> res;
    for ( size_t i=0; i < size(); ++ i ){
      ElementType et = data[i]->element_id();
      if ( et == Word_t
	   || et == WordReference_t
	   //	   || et == Phoneme_t
	   || et == MorphologyLayer_t ){
	res.push_back( data[i] );
      }
      else {
	AbstractSpanAnnotation *as = dynamic_cast<AbstractSpanAnnotation*>(data[i]);
	if ( as != 0 ){
	  vector<FoliaElement*> sub = as->wrefs();
	  for( size_t j=0; j < sub.size(); ++j ){
	    res.push_back( sub[j] );
	  }
	}
      }
    }
    return res;
  }

  FoliaElement *AbstractSpanAnnotation::wrefs( size_t pos ) const {
    vector<FoliaElement*> v = wrefs();
    if ( pos < v.size() )
      return v[pos];
    else
      return 0;
  }

  AbstractSpanAnnotation *AbstractAnnotationLayer::findspan( const vector<FoliaElement*>& words ) const {
    vector<AbstractSpanAnnotation*> av = selectSpan();
    for ( size_t i=0; i < av.size(); ++i ){
      vector<FoliaElement*> v = av[i]->wrefs();
      if ( v.size() == words.size() ){
	bool ok = true;
	for ( size_t n = 0; n < v.size() && ok ; ++n ){
	  if ( v[n] != words[n] )
	    ok = false;
	}
	if ( ok )
	  return av[i];
      }
    }
    return 0;
  }

  void Alternative::init(){
    _xmltag = "alt";
    _element_id = Alternative_t;
    _required_attributes = NO_ATT;
    _optional_attributes = ALL;
    _accepted_data = { TokenAnnotation_t };
    _annotation_type = AnnotationType::ALTERNATIVE;
    _auth = false;
  }

  void AlternativeLayers::init(){
    _xmltag = "altlayers";
    _element_id = AlternativeLayers_t;
    _optional_attributes = ALL;
    _accepted_data = { AnnotationLayer_t };
    _auth = false;
  }

  void AbstractCorrectionChild::init(){
    _optional_attributes = NO_ATT;
    _accepted_data = { TokenAnnotation_t, SpanAnnotation_t,
		       Word_t, WordReference_t, String_t,
		       TextContent_t, Description_t };
    _occurrences = 1;
    PRINTABLE=true;
    SPEAKABLE=true;
  }

  void NewElement::init(){
    _xmltag = "new";
    _element_id = New_t;
  }

  void Current::init(){
    _xmltag = "current";
    _element_id = Current_t;
  }

  void Original::init(){
    _xmltag = "original";
    _element_id = Original_t;
    _auth = false;
  }

  void Suggestion::init(){
    _xmltag = "suggestion";
    _element_id = Suggestion_t;
    _optional_attributes = ANNOTATOR|CONFIDENCE|DATETIME|N;
    _annotation_type = AnnotationType::SUGGESTION;
    _occurrences=0;
    _occurrences_per_set=0;
    _auth = false;
  }

  void Correction::init(){
    _xmltag = "correction";
    _element_id = Correction_t;
    _required_attributes = NO_ATT;
    _annotation_type = AnnotationType::CORRECTION;
    _accepted_data = { New_t, Original_t, Suggestion_t, Current_t,
		       Description_t };
    _occurrences_per_set=0;
    PRINTABLE=true;
    SPEAKABLE=true;
  }

  void Description::init(){
    _xmltag = "desc";
    _element_id = Description_t;
    _occurrences = 1;
  }

  UnicodeString XmlText::text( const string&, bool, bool ) const {
    return UTF8ToUnicode(_value);
  }

  xmlNode *XmlText::xml( bool, bool ) const {
    return xmlNewText( (const xmlChar*)_value.c_str() );
  }

  FoliaElement* XmlText::parseXml( const xmlNode *node ){
    if ( node->content ){
      _value = (const char*)node->content;
    }
    if ( _value.empty() ){
      throw ValueError( "TextContent may not be empty" );
    }
    return this;
  }

  void Entry::init(){
    _xmltag = "entry";
    _element_id = Entry_t;
    _accepted_data = { Term_t, Definition_t, Example_t, Correction_t,
		       Description_t, Metric_t, Alignment_t,
		       AlternativeLayers_t, AnnotationLayer_t };
  }

  void Definition::init(){
    _xmltag = "def";
    _element_id = Definition_t;
    _accepted_data = { Paragraph_t, Sentence_t, Word_t, Utterance_t,
		       List_t, Figure_t, Table_t, Reference_t, Feature_t,
		       TextContent_t, PhonContent_t, String_t, Metric_t,
		       TokenAnnotation_t, Correction_t, Part_t };
    _annotation_type = AnnotationType::DEFINITION;
  }

  void Term::init(){
    _xmltag = "term";
    _element_id = Term_t;
    _accepted_data = { Paragraph_t, Event_t, Sentence_t, Word_t, Utterance_t,
		       List_t, Figure_t, Table_t, Reference_t, Feature_t,
		       TextContent_t, PhonContent_t, String_t, Metric_t,
		       TokenAnnotation_t, Correction_t, Part_t };
    _annotation_type = AnnotationType::TERM;
  }

  void Example::init(){
    _xmltag = "ex";
    _element_id = Example_t;
    _accepted_data = { Paragraph_t, Sentence_t, Word_t, Utterance_t,
		       List_t, Figure_t, Table_t, Reference_t, Feature_t,
		       TextContent_t, PhonContent_t, String_t, Metric_t,
		       TokenAnnotation_t, Correction_t, Part_t };
    _annotation_type = AnnotationType::EXAMPLE;
  }

  void XmlText::init(){
    _xmltag = "xml-text";
    _element_id = XmlText_t;
    TEXTDELIMITER = "*";
    PRINTABLE = true;
    SPEAKABLE = true;
  }

  void External::init(){
    _xmltag = "external";
    _element_id = External_t;
    _include = false;
    PRINTABLE = true;
    SPEAKABLE = false;
  }

  void External::resolve( ) {
    try {
      cerr << "try to resolve: " << _src << endl;
      Document doc( "file='"+_src +"'" );
      xmlDoc *xmldoc = doc.XmlDoc();
      xmlNode *root = xmlDocGetRootElement( xmldoc );
      xmlNode *p = root->children;
      while ( p ){
	if ( p->type == XML_ELEMENT_NODE ){
	  string tag = Name( p );
	  if ( tag == "text" ){
	    FoliaElement *parent = _parent;
	    KWargs args = parent->collectAttributes();
	    args["_id"] = "Arglebargleglop-glyf";
	    Text *tmp = new Text( mydoc, args );
	    tmp->FoliaImpl::parseXml( p );
	    FoliaElement *old = parent->replace( this, tmp->index(0) );
	    mydoc->delDocIndex( tmp, "Arglebargleglop-glyf" );
	    tmp->remove( (size_t)0, false );
	    delete tmp;
	    delete old;
	  }
	  p = p->next;
	}
      }
    }
    catch ( const exception& e ){
      throw XmlError( "resolving external " + _src + " failed: "
		      + e.what() );
    }
  }

  FoliaElement* External::parseXml( const xmlNode *node ){
    KWargs att = getAttributes( node );
    KWargs::const_iterator it = att.find("src" );
    if ( it == att.end() ){
      throw ValueError( "External: 'src' may not be empty" );
    }
    setAttributes( att );
    if ( _include ){
      mydoc->addExternal( this );
    }
    return this;
  }

  KWargs External::collectAttributes() const {
    KWargs atts = FoliaImpl::collectAttributes();
    if ( !_src.empty() ){
      atts["src"] = _src;
    }
    if ( _include ){
      atts["include"] = "yes";
    }
    return atts;
  }

  void External::setAttributes( const KWargs& kwargsin ){
    KWargs kwargs = kwargsin;
    KWargs::const_iterator it;
    it = kwargs.find( "src" );
    if ( it != kwargs.end() ) {
      _src = it->second;
      kwargs.erase( it );
    }
    it = kwargs.find( "include" );
    if ( it != kwargs.end() ) {
      _include = stringTo<bool>( it->second );
      kwargs.erase( it );
    }
    FoliaImpl::setAttributes(kwargs);
  }

  void Note::init(){
    _required_attributes = ID;
    _xmltag = "note";
    _element_id = Note_t;
    _accepted_data = { Paragraph_t,
		       Sentence_t,
		       Word_t,
		       Example_t,
		       Head_t,
		       Utterance_t,
		       List_t,
		       Figure_t,
		       Table_t,
		       Reference_t,
		       Feature_t,
		       TextContent_t,
		       PhonContent_t,
		       String_t,
		       Metric_t,
		       TokenAnnotation_t,
		       Correction_t,
		       Part_t,
		       Description_t };
    _annotation_type = AnnotationType::NOTE;
  }

  KWargs Note::collectAttributes() const {
    KWargs attribs = FoliaImpl::collectAttributes();
    return attribs;
  }

  void Note::setAttributes( const KWargs& args ){
    KWargs::const_iterator it = args.find( "_id" );
    if ( it != args.end() ){
      refId = it->second;
    }
    FoliaImpl::setAttributes( args );
  }

  void Reference::init(){
    _xmltag = "ref";
    _element_id = Reference_t;
    _required_attributes = NO_ATT;
    _optional_attributes = ID|ANNOTATOR|CONFIDENCE|DATETIME;
    _accepted_data = { TextContent_t, PhonContent_t, String_t,
		       Description_t, Metric_t };
  }

  KWargs Reference::collectAttributes() const {
    KWargs atts;
    atts["id"] = refId;
    atts["type"] = ref_type;
    return atts;
  }

  void Reference::setAttributes( const KWargs& args ){
    KWargs::const_iterator it = args.find( "id" );
    if ( it != args.end() ){
      refId = it->second;
    }
    it = args.find( "type" );
    if ( it != args.end() ){
      try {
	stringTo<ElementType>( it->second );
      }
      catch (...){
	throw XmlError( "attribute 'type' must be an Element Type" );
      }
      ref_type = it->second;
    }
  }

  xmlNode *XmlComment::xml( bool, bool ) const {
    return xmlNewComment( (const xmlChar*)_value.c_str() );
  }

  FoliaElement* XmlComment::parseXml( const xmlNode *node ){
    if ( node->content )
      _value = (const char*)node->content;
    return this;
  }

  void XmlComment::init(){
    _xmltag = "xml-comment";
    _element_id = XmlComment_t;
  }

  void Feature::setAttributes( const KWargs& kwargs ){
    //
    // Feature is special. So DON'T call ::setAttributes
    //
    KWargs::const_iterator it = kwargs.find( "subset" );
    if ( it == kwargs.end() ){
      if ( _subset.empty() ) {
	throw ValueError("subset attribute is required for " + classname() );
      }
    }
    else {
      _subset = it->second;
    }
    it = kwargs.find( "cls" );
    if ( it == kwargs.end() )
      it = kwargs.find( "class" );
    if ( it == kwargs.end() ) {
      throw ValueError("class attribute is required for " + classname() );
    }
    _class = it->second;
  }

  KWargs Feature::collectAttributes() const {
    KWargs attribs = FoliaImpl::collectAttributes();
    attribs["subset"] = _subset;
    return attribs;
  }

  vector<string> FoliaImpl::feats( const std::string& s ) const {
    //    return all classes of the given subset
    vector<string> result;
    for ( size_t i=0; i < data.size(); ++i ){
      if ( data[i]->isSubClass( Feature_t ) &&
	   data[i]->subset() == s ) {
	result.push_back( data[i]->cls() );
      }
    }
    return result;
  }

  string FoliaImpl::feat( const std::string& s ) const {
    //    return the fist class of the given subset
    for ( size_t i=0; i < data.size(); ++i ){
      if ( data[i]->isSubClass( Feature_t ) &&
	   data[i]->subset() == s ) {
	return data[i]->cls();
      }
    }
    return "";
  }

  KWargs AbstractTextMarkup::collectAttributes() const {
    KWargs attribs = FoliaImpl::collectAttributes();
    if ( !idref.empty() )
      attribs["id"] = idref;
    return attribs;
  }

  void AbstractTextMarkup::setAttributes( const KWargs& atts ){
    KWargs args = atts;
    KWargs::iterator it = args.find( "id" );
    if ( it != args.end() ){
      KWargs::const_iterator it2 = args.find( "_id" );
      if ( it2 != args.end() ){
	throw ValueError("Both 'id' and 'xml:id found for " + classname() );
      }
      idref = it->second;
      args.erase( it );
    }
    FoliaImpl::setAttributes( args );
  }

  KWargs TextMarkupCorrection::collectAttributes() const {
    KWargs attribs = AbstractTextMarkup::collectAttributes();
    if ( !_original.empty() )
      attribs["original"] = _original;
    return attribs;
  }

  void TextMarkupCorrection::setAttributes( const KWargs& args ){
    KWargs argl = args;
    KWargs::const_iterator it = argl.find( "id" );
    if ( it != argl.end() ){
      idref = it->second;
      argl.erase( it );
    }
    it = argl.find( "original" );
    if ( it != argl.end() ){
      _original = it->second;
      argl.erase( it );
    }
    FoliaImpl::setAttributes( argl );
  }

  UnicodeString AbstractTextMarkup::text( const std::string& cls,
					  bool, bool ) const {
    // we assume al TextMarkup te be tokenized already
    return FoliaImpl::text( cls, true );
  }

  UnicodeString TextMarkupCorrection::text( const std::string& cls,
					    bool ret, bool strict ) const{
    if ( cls == "original" )
      return UTF8ToUnicode(_original);
    else
      return FoliaImpl::text( cls, ret, strict );
  }

  const FoliaElement* AbstractTextMarkup::resolveid() const {
    if ( idref.empty() || !mydoc )
      return this;
    else
      return mydoc->index(idref);
  }

  void Morpheme::init(){
    _element_id = Morpheme_t;
    _xmltag = "morpheme";
    _required_attributes = NO_ATT;
    _optional_attributes = ALL;
    _accepted_data = { Feature_t, FunctionFeature_t, TextContent_t,
		       Metric_t, Alignment_t, TokenAnnotation_t,
		       Description_t, Morpheme_t };
    _annotation_type = AnnotationType::MORPHOLOGICAL;
  }

  void SyntaxLayer::init(){
    _element_id = SyntaxLayer_t;
    _xmltag = "syntax";
    _accepted_data = { SyntacticUnit_t,
		       Description_t,
		       Correction_t };
    _annotation_type = AnnotationType::SYNTAX;
  }

  void ChunkingLayer::init(){
    _element_id = Chunking_t;
    _xmltag = "chunking";
    _accepted_data = { Chunk_t,
		       Description_t,
		       Correction_t };
    _annotation_type = AnnotationType::CHUNKING;
  }

  void EntitiesLayer::init(){
    _element_id = Entities_t;
    _xmltag = "entities";
    _accepted_data = { Entity_t,
		       Description_t,
		       Correction_t };
   _annotation_type = AnnotationType::ENTITY;
  }

  void TimingLayer::init(){
    _element_id = TimingLayer_t;
    _xmltag = "timing";
    _accepted_data = { TimeSegment_t,
		       Description_t,
		       Correction_t };
  }

  void MorphologyLayer::init(){
    _element_id = MorphologyLayer_t;
    _xmltag = "morphology";
    _accepted_data = { Morpheme_t, Correction_t };
    _occurrences_per_set = 1; // Don't allow duplicates within the same set
    _annotation_type = AnnotationType::MORPHOLOGICAL;
  }

  void PhonologyLayer::init(){
    _element_id = PhonologyLayer_t;
    _xmltag = "phonology";
    _accepted_data = { Phoneme_t, Correction_t };
    _occurrences_per_set = 1; // Don't allow duplicates within the same set
    _annotation_type = AnnotationType::PHONOLOGICAL;
  }

  void CoreferenceLayer::init(){
    _element_id = Coreferences_t;
    _xmltag = "coreferences";
    _accepted_data = { CoreferenceChain_t,
		       Description_t,
		       Correction_t };
    _annotation_type = AnnotationType::COREFERENCE;
  }

  void CoreferenceLink::init(){
    _element_id = CoreferenceLink_t;
    _xmltag = "coreferencelink";
    _required_attributes = NO_ATT;
    _optional_attributes = ANNOTATOR|N|DATETIME;
    _accepted_data = { Word_t, WordReference_t, Headwords_t,
		       Description_t, Lang_t,
		       Alignment_t, TimeFeature_t, LevelFeature_t,
		       ModalityFeature_t, Metric_t};
    _annotation_type = AnnotationType::COREFERENCE;
  }

  void CoreferenceChain::init(){
    _element_id = CoreferenceChain_t;
    _xmltag = "coreferencechain";
    _required_attributes = NO_ATT;
    _accepted_data = { CoreferenceLink_t,
		       Description_t,
		       Metric_t };
    _annotation_type = AnnotationType::COREFERENCE;
  }

  void SemanticRolesLayer::init(){
    _element_id = Semroles_t;
    _xmltag = "semroles";
    _accepted_data = { Semrole_t,
		       Description_t,
		       Correction_t };
    _annotation_type = AnnotationType::SEMROLE;
  }

  void DependenciesLayer::init(){
    _element_id = Dependencies_t;
    _xmltag = "dependencies";
    _accepted_data = { Dependency_t,
		       Description_t,
		       Correction_t };
    _annotation_type = AnnotationType::DEPENDENCY;
  }

  void Dependency::init(){
    _element_id = Dependency_t;
    _xmltag = "dependency";
    _required_attributes = NO_ATT;
    _annotation_type = AnnotationType::DEPENDENCY;
    _accepted_data = { DependencyDependent_t, Headwords_t,
		       Feature_t, Description_t, Alignment_t };
  }

  void DependencyDependent::init(){
    _element_id = DependencyDependent_t;
    _xmltag = "dep";
    _required_attributes = NO_ATT;
    _optional_attributes = NO_ATT;
    _annotation_type = AnnotationType::DEPENDENCY;
    _accepted_data = { Word_t, WordReference_t, PlaceHolder_t,
		       Description_t, Feature_t, Alignment_t };
  }

  void Headwords::init(){
    _element_id = Headwords_t;
    _xmltag = "hd";
    _required_attributes = NO_ATT;
    _optional_attributes = NO_ATT;
    _accepted_data = { Word_t, WordReference_t, PlaceHolder_t,
		       Description_t, Feature_t, Metric_t,
		       Alignment_t, Lang_t };
  }

  void PosAnnotation::init(){
    _xmltag="pos";
    _element_id = Pos_t;
    _annotation_type = AnnotationType::POS;
    _accepted_data = { Feature_t,
		       Metric_t,
		       Description_t };
  }

  void LemmaAnnotation::init(){
    _xmltag="lemma";
    _element_id = Lemma_t;
    _annotation_type = AnnotationType::LEMMA;
    _accepted_data = { Feature_t, Metric_t, Description_t };
  }

  void LangAnnotation::init(){
    _xmltag="lang";
    _element_id = Lang_t;
    _annotation_type = AnnotationType::LANG;
    _accepted_data = { Feature_t, Metric_t, Description_t };
  }

  void Phoneme::init(){
    _xmltag="phoneme";
    _element_id = Phoneme_t;
    _required_attributes = NO_ATT;
    _optional_attributes = ALL;
    _annotation_type = AnnotationType::PHON;
    _accepted_data = { PhonContent_t, Feature_t, Description_t };
  }

  void DomainAnnotation::init(){
    _xmltag="domain";
    _element_id = Domain_t;
    _annotation_type = AnnotationType::DOMEIN;
    _accepted_data = { Feature_t, Description_t };
  }

  void SenseAnnotation::init(){
    _xmltag="sense";
    _element_id = Sense_t;
    _annotation_type = AnnotationType::SENSE;
    _accepted_data = { Feature_t, SynsetFeature_t, Description_t, Metric_t };
  }

  void SubjectivityAnnotation::init(){
    _xmltag="subjectivity";
    _element_id = Subjectivity_t;
    _annotation_type = AnnotationType::SUBJECTIVITY;
    _accepted_data = { Feature_t, Description_t };
  }

  void Quote::init(){
    _xmltag="quote";
    _element_id = Quote_t;
    _required_attributes = NO_ATT;
    _accepted_data = { Structure_t, String_t, Lang_t,
		       TextContent_t, Description_t, Alignment_t,
		       Gap_t };
    TEXTDELIMITER = " ";
  }

  void Feature::init() {
    _xmltag = "feat";
    _element_id = Feature_t;
    _occurrences_per_set = 0; // Allow duplicates within the same set
  }

  void BeginDateTimeFeature::init(){
    _xmltag="begindatetime";
    _element_id = BeginDateTimeFeature_t;
    _subset = "begindatetime";
  }

  void EndDateTimeFeature::init(){
    _xmltag="enddatetime";
    _element_id = EndDateTimeFeature_t;
    _subset = "enddatetime";
  }

  void SynsetFeature::init(){
    _xmltag="synset";
    _element_id = SynsetFeature_t;
    _annotation_type = AnnotationType::SENSE;
    _subset = "synset";
  }

  void ActorFeature::init(){
    _xmltag = "actor";
    _element_id = ActorFeature_t;
    _subset = "actor";
  }

  void HeadFeature::init(){
    _xmltag = "headfeature";
    _element_id = HeadFeature_t;
    _subset = "head";
  }

  void ValueFeature::init(){
    _xmltag = "value";
    _element_id = ValueFeature_t;
    _subset = "value";
  }

  void FunctionFeature::init(){
    _xmltag = "function";
    _element_id = FunctionFeature_t;
    _subset = "function";
  }

  void LevelFeature::init(){
    _xmltag = "level";
    _element_id = LevelFeature_t;
    _subset = "level";
  }

  void ModalityFeature::init(){
    _xmltag = "modality";
    _element_id = ModalityFeature_t;
    _subset = "modality";
  }

  void TimeFeature::init(){
    _xmltag = "time";
    _element_id = TimeFeature_t;
    _subset = "time";
  }

  void AbstractSpanAnnotation::init() {
    _xmltag = "spanannotation";
    _element_id = SpanAnnotation_t;
    _required_attributes = NO_ATT;
    _optional_attributes = ALL;
    _occurrences_per_set = 0; // Allow duplicates within the same set
    PRINTABLE = true;
    SPEAKABLE = true;
  }

  void ErrorDetection::init(){
    _xmltag = "errordetection";
    _element_id = ErrorDetection_t;
    _annotation_type = AnnotationType::ERRORDETECTION;
    _occurrences_per_set = 0; // Allow duplicates within the same set
  }

  void AbstractTextMarkup::init(){
    _xmltag = "textmarkup";
    _element_id = AbstractTextMarkup_t;
    _required_attributes = NO_ATT;
    _optional_attributes = ALL;
    _annotation_type = AnnotationType::NO_ANN;
    _accepted_data = { AbstractTextMarkup_t, XmlText_t };
    PRINTABLE = true;
    XLINK = true;
    TEXTDELIMITER = "";
    _occurrences_per_set = 0; // Allow duplicates within the same set
  }

  void TextMarkupString::init(){
    _xmltag = "t-str";
    _element_id = TextMarkupString_t;
    _annotation_type = AnnotationType::STRING;
  }

  void TextMarkupGap::init(){
    _xmltag = "t-gap";
    _element_id = TextMarkupGap_t;
    _annotation_type = AnnotationType::GAP;
  }

  void TextMarkupCorrection::init(){
    _xmltag = "t-correction";
    _element_id = TextMarkupCorrection_t;
    _annotation_type = AnnotationType::CORRECTION;
  }

  void TextMarkupError::init(){
    _xmltag = "t-error";
    _element_id = TextMarkupError_t;
    _annotation_type = AnnotationType::ERRORDETECTION;
  }

  void TextMarkupStyle::init(){
    _xmltag = "t-style";
    _element_id = TextMarkupStyle_t;
    _annotation_type = AnnotationType::STYLE;
  }

} // namespace folia
