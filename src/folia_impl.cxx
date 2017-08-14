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
#include <type_traits>
#include <stdexcept>
#include "ticcutils/PrettyPrint.h"
#include "ticcutils/StringOps.h"
#include "ticcutils/XMLtools.h"
#include "libfolia/folia.h"
#include "libfolia/folia_properties.h"
#include "config.h"

using namespace std;
using namespace TiCC;

namespace folia {
  using TiCC::operator <<;

  string VersionName() { return PACKAGE_STRING; }
  string Version() { return VERSION; }

  ElementType FoliaImpl::element_id() const {
    return _props.ELEMENT_ID;
  }

  size_t FoliaImpl::occurrences() const {
    return _props.OCCURRENCES;
  }

  size_t FoliaImpl::occurrences_per_set() const {
    return _props.OCCURRENCES_PER_SET;
  }

  Attrib FoliaImpl::required_attributes() const {
    return _props.REQUIRED_ATTRIBS;
  }

  Attrib FoliaImpl::optional_attributes() const {
    return _props.OPTIONAL_ATTRIBS;
  }

  const string& FoliaImpl::xmltag() const {
    return _props.XMLTAG;
  }

  const string& FoliaImpl::default_subset() const {
    return _props.SUBSET;
  }

  AnnotationType::AnnotationType FoliaImpl::annotation_type() const {
    return _props.ANNOTATIONTYPE;
  }

  const set<ElementType>& FoliaImpl::accepted_data() const {
    return _props.ACCEPTED_DATA;
  }

  const set<ElementType>& FoliaImpl::required_data() const {
    return _props.REQUIRED_DATA;
  }

  bool FoliaImpl::printable() const {
    return _props.PRINTABLE;
  }

  bool FoliaImpl::speakable() const {
    return _props.SPEAKABLE;
  }

  bool FoliaImpl::is_textcontainer() const {
    return _props.TEXTCONTAINER;
  }

  bool FoliaImpl::is_phoncontainer() const {
    return _props.PHONCONTAINER;
  }

  bool FoliaImpl::xlink() const {
    return _props.XLINK;
  }

  bool FoliaImpl::auth() const {
    return _props.AUTH;
  }

  bool FoliaImpl::setonly() const {
    return _props.SETONLY;
  }

  bool FoliaImpl::auto_generate_id() const {
    return _props.AUTO_GENERATE_ID;
  }

  bool is_structure( const FoliaElement *el ){
    return dynamic_cast<const AbstractStructureElement*>( el ) != 0;
  }

  const string FoliaImpl::href() const {
    auto it = _xlink.find("href");
    if ( it != _xlink.end() ){
      return it->second;
    }
    return "";
  }

  ostream& operator<<( ostream& os, const FoliaElement& ae ) {
    os << " <" << ae.classname();
    KWargs ats = ae.collectAttributes();
    if ( !ae.id().empty() ) {
      ats["_id"] = ae.id();
    }
    for ( const auto& it: ats ) {
      os << " " << it.first << "='" << it.second << "'";
    }
    os << " > {";
    for ( size_t i=0; i < ae.size(); ++i ) {
      os << "<" << ae.index(i)->classname() << ">,";
    }
    os << "}";
    return os;
  }

  ostream& operator<<( ostream&os, const FoliaElement *ae ) {
    if ( !ae ) {
      os << "nil";
    }
    else
      os << *ae;
    return os;
  }

  FoliaImpl::FoliaImpl( const properties& p, Document *d ) :
    _parent(0),
    _auth( p.AUTH ),
    mydoc(d),
    _annotator_type(UNDEFINED),
    _confidence(-1),
    _refcount(0),
    _props(p)
  {
  }

  FoliaImpl::~FoliaImpl( ) {
    // cerr << "delete element id=" << _id << " tag = " << xmltag() << " *= "
    // 	 << (void*)this << " datasize= " << data.size() << endl;
    for ( const auto& el : data ) {
      if ( el->refcount() == 0 ) {
	// probably only != 0 for words
	delete el;
      }
      else if ( mydoc ) {
	mydoc->keepForDeletion( el );
      }
    }
    //    cerr << "\t\tdelete element id=" << _id << " tag = " << xmltag() << " *= "
    //	 << (void*)this << " datasize= " << data.size() << endl;
    if ( mydoc ) {
      mydoc->delDocIndex( this, _id );
      mydoc->decrRef( annotation_type(), _set );
    }
  }

  xmlNs *FoliaImpl::foliaNs() const {
    if ( mydoc ) {
      return mydoc->foliaNs();
    }
    return 0;
  }

  void FoliaImpl::setAttributes( const KWargs& kwargs_in ) {
    KWargs kwargs = kwargs_in;
    Attrib supported = required_attributes() | optional_attributes();
    // if ( element_id() == Reference_t ) {
    //   cerr << "set attributes: " << kwargs << " on " << classname() << endl;
    //   cerr << "required = " <<  required_attributes() << endl;
    //   cerr << "optional = " <<  optional_attributes() << endl;
    //   cerr << "supported = " << supported << endl;
    //   cerr << "ID & supported = " << (ID & supported) << endl;
    //   cerr << "ID & _required = " << (ID & required_attributes() ) << endl;
    //   cerr << "_id=" << _id << endl;
    //   Reference*ref=dynamic_cast<Reference*>(this);
    //   cerr << "id=" << ref->refId << endl;

    //   //   cerr << "AUTH : " << _auth << ", default=" << default_auth() << endl;
    // }
    if ( mydoc && mydoc->debug > 2 ) {
      cerr << "set attributes: " << kwargs << " on " << classname() << endl;
    }

    auto it = kwargs.find( "generate_id" );
    if ( it != kwargs.end() ) {
      if ( !mydoc ) {
	throw runtime_error( "can't generate an ID without a doc" );
      }
      FoliaElement *e = (*mydoc)[it->second];
      if ( e ) {
	_id = e->generateId( xmltag() );
      }
      else {
	throw ValueError("Unable to generate an id from ID= " + it->second );
      }
      kwargs.erase( it );
    }
    else {
      it = kwargs.find( "_id" );
      auto it2 = kwargs.find( "id" );
      if ( it == kwargs.end() ) {
	it = it2;
      }
      else if ( it2 != kwargs.end() ) {
	throw ValueError("Both 'id' and 'xml:id found for " + classname() );
      }
      if ( it != kwargs.end() ) {
	if ( (!ID) & supported ) {
	  throw ValueError("ID is not supported for " + classname() );
	}
	else {
	  if ( isNCName( it->second ) ){
	    _id = it->second;
	    kwargs.erase( it );
	  }
	  else {
	    throw XmlError( "'"
			    + it->second
			    + "' is not a valid NCName." );
	  }
	}
      }
    }

    it = kwargs.find( "set" );
    string def;
    if ( it != kwargs.end() ) {
      if ( !( (CLASS & supported) || setonly() ) ) {
	throw ValueError("Set is not supported for " + classname());
      }
      else {
	_set = it->second;
      }
      if ( !mydoc ) {
	throw ValueError( "Set=" + _set + " is used on a node without a document." );
      }
      if ( !mydoc->isDeclared( annotation_type(), _set ) ) {
	throw ValueError( "Set " + _set + " is used but has no declaration " +
			  "for " + toString( annotation_type() ) + "-annotation" );
      }
      kwargs.erase( it );
    }
    else if ( mydoc && ( def = mydoc->defaultset( annotation_type() )) != "" ) {
      _set = def;
    }
    else {
      _set = "";
    }
    it = kwargs.find( "class" );
    if ( it != kwargs.end() ) {
      if ( !( CLASS & supported ) ) {
	throw ValueError("Class is not supported for " + classname() );
      }
      _class = it->second;
      if ( element_id() != TextContent_t && element_id() != PhonContent_t ) {
	if ( !mydoc ) {
	  throw ValueError( "Class=" + _class + " is used on a node without a document." );
	}
	else if ( _set == "" &&
		  mydoc->defaultset( annotation_type() ) == "" &&
		  mydoc->isDeclared( annotation_type() ) ) {
	  throw ValueError( "Class " + _class + " is used but has no default declaration " +
			    "for " + toString( annotation_type() ) + "-annotation" );
	}
	mydoc->incrRef( annotation_type(), _set );
      }
      kwargs.erase( it );
    }
    else
      _class = "";

    if ( element_id() != TextContent_t && element_id() != PhonContent_t ) {
      if ( !_class.empty() && _set.empty() ) {
	throw ValueError("Set is required for <" + classname() +
			 " class=\"" + _class + "\"> assigned without set."  );
      }
    }
    it = kwargs.find( "annotator" );
    if ( it != kwargs.end() ) {
      if ( !(ANNOTATOR & supported) ) {
	throw ValueError("Annotator is not supported for " + classname() );
      }
      else {
	_annotator = it->second;
      }
      kwargs.erase( it );
    }
    else if ( mydoc &&
	      (def = mydoc->defaultannotator( annotation_type(), _set )) != "" ) {
      _annotator = def;
    }
    else {
      _annotator = "";
    }

    it = kwargs.find( "annotatortype" );
    if ( it != kwargs.end() ) {
      if ( ! (ANNOTATOR & supported) ) {
	throw ValueError("Annotatortype is not supported for " + classname() );
      }
      else {
	_annotator_type = stringTo<AnnotatorType>( it->second );
	if ( _annotator_type == UNDEFINED ) {
	  throw ValueError( "annotatortype must be 'auto' or 'manual', got '"
			    + it->second + "'" );
	}
      }
      kwargs.erase( it );
    }
    else if ( mydoc &&
	      (def = mydoc->defaultannotatortype( annotation_type(), _set ) ) != ""  ) {
      _annotator_type = stringTo<AnnotatorType>( def );
      if ( _annotator_type == UNDEFINED ) {
	throw ValueError("annotatortype must be 'auto' or 'manual'");
      }
    }
    else {
      _annotator_type = UNDEFINED;
    }

    it = kwargs.find( "confidence" );
    if ( it != kwargs.end() ) {
      if ( !(CONFIDENCE & supported) ) {
	throw ValueError("Confidence is not supported for " + classname() );
      }
      else {
	try {
	  _confidence = stringTo<double>(it->second);
	  if ( _confidence < 0 || _confidence > 1.0 )
	    throw ValueError("Confidence must be a floating point number between 0 and 1, got " + TiCC::toString(_confidence) );
	}
	catch (...) {
	  throw ValueError("invalid Confidence value, (not a number?)");
	}
      }
      kwargs.erase( it );
    }
    else {
      _confidence = -1;
    }
    it = kwargs.find( "n" );
    if ( it != kwargs.end() ) {
      if ( !(N & supported) ) {
	throw ValueError("N is not supported for " + classname() );
      }
      else {
	_n = it->second;
      }
      kwargs.erase( it );
    }
    else
      _n = "";

    if ( xlink() ) {
      string type = "simple";
      it = kwargs.find( "type" );
      if ( it != kwargs.end() ) {
	type = it->second;
	kwargs.erase( it );
      }
      if ( type != "simple" && type != "locator" ) {
	throw XmlError( "only xlink:types: 'simple' and 'locator' are supported!" );
      }
      _xlink["type"] = type;
      it = kwargs.find( "href" );
      if ( it != kwargs.end() ) {
	_xlink["href"] = it->second;
	kwargs.erase( it );
      }
      else if ( type == "locator" ){
	throw XmlError( "xlink:type='locator' requires an 'xlink:href' attribute" );
      }
      it = kwargs.find( "role" );
      if ( it != kwargs.end() ) {
	_xlink["role"] = it->second;
	kwargs.erase( it );
      }
      it = kwargs.find( "title" );
      if ( it != kwargs.end() ) {
	_xlink["title"] = it->second;
	kwargs.erase( it );
      }
      it = kwargs.find( "label" );
      if ( it != kwargs.end() ) {
	if ( type == "simple" ){
	  throw XmlError( "xlink:type='simple' may not have an 'xlink:label' attribute" );
	}
	_xlink["label"] = it->second;
	kwargs.erase( it );
      }
      it = kwargs.find( "arcrole" );
      if ( it != kwargs.end() ) {
	if ( type == "locator" ){
	  throw XmlError( "xlink:type='locator' may not have an 'xlink:arcrole' attribute" );
	}
	_xlink["arcrole"] = it->second;
	kwargs.erase( it );
      }
      it = kwargs.find( "show" );
      if ( it != kwargs.end() ) {
	if ( type == "locator" ){
	  throw XmlError( "xlink:type='locator' may not have an 'xlink:show' attribute" );
	}
	_xlink["show"] = it->second;
	kwargs.erase( it );
      }
      it = kwargs.find( "actuate" );
      if ( it != kwargs.end() ) {
	if ( type == "locator" ){
	  throw XmlError( "xlink:type='locator' may not have an 'xlink:actuate' attribute" );
	}
	_xlink["actuate"] = it->second;
	kwargs.erase( it );
      }
    }

    it = kwargs.find( "datetime" );
    if ( it != kwargs.end() ) {
      if ( !(DATETIME & supported) ) {
	throw ValueError("datetime is not supported for " + classname() );
      }
      else {
	string time = parseDate( it->second );
	if ( time.empty() )
	  throw ValueError( "invalid datetime, must be in YYYY-MM-DDThh:mm:ss format: " + it->second );
	_datetime = time;
      }
      kwargs.erase( it );
    }
    else if ( mydoc &&
	      (def = mydoc->defaultdatetime( annotation_type(), _set )) != "" ) {
      _datetime = def;
    }
    else {
      _datetime.clear();
    }
    it = kwargs.find( "begintime" );
    if ( it != kwargs.end() ) {
      if ( !(BEGINTIME & supported) ) {
	throw ValueError( "begintime is not supported for " + classname() );
      }
      else {
	string time = parseTime( it->second );
	if ( time.empty() ) {
	  throw ValueError( "invalid begintime, must be in HH:MM:SS:mmmm format: " + it->second );
	}
	_begintime = time;
      }
      kwargs.erase( it );
    }
    else {
      _begintime.clear();
    }
    it = kwargs.find( "endtime" );
    if ( it != kwargs.end() ) {
      if ( !(ENDTIME & supported) ) {
	throw ValueError( "endtime is not supported for " + classname() );
      }
      else {
	string time = parseTime( it->second );
	if ( time.empty() ) {
	  throw ValueError( "invalid endtime, must be in HH:MM:SS:mmmm format: " + it->second );
	}
	_endtime = time;
      }
      kwargs.erase( it );
    }
    else {
      _endtime.clear();
    }

    it = kwargs.find( "src" );
    if ( it != kwargs.end() ) {
      if ( !(SRC & supported) ) {
	throw ValueError( "src is not supported for " + classname() );
      }
      else {
	_src = it->second;
      }
      kwargs.erase( it );
    }
    else
      _src.clear();

    it = kwargs.find( "speaker" );
    if ( it != kwargs.end() ) {
      if ( !(SPEAKER & supported) ) {
	throw ValueError( "speaker is not supported for " + classname() );
      }
      else {
	_speaker = it->second;
      }
      kwargs.erase( it );
    }
    else {
      _speaker.clear();
    }
    it = kwargs.find( "auth" );
    if ( it != kwargs.end() ) {
      _auth = stringTo<bool>( it->second );
      kwargs.erase( it );
    }
    if ( mydoc && !_id.empty() ) {
      mydoc->addDocIndex( this, _id );
    }
    addFeatureNodes( kwargs );
  }

  void FoliaImpl::addFeatureNodes( const KWargs& kwargs ) {
    for ( const auto& it: kwargs ) {
      string tag = it.first;
      if ( tag == "head" ) {
	// "head" is special because the tag is "headfeature"
	// this to avoid conflicts withe the "head" tag!
	tag = "headfeature";
      }
      if ( AttributeFeatures.find( tag ) == AttributeFeatures.end() ) {
	string message = "unsupported attribute: " + tag + "='" + it.second
	  + "' for node with tag '" + classname() + "'";
	if ( mydoc && mydoc->permissive() ) {
	  cerr << message << endl;
	}
	else {
	  throw XmlError( message );
	}
      }
      KWargs newa;
      newa["class"] = it.second;
      FoliaElement *new_node = createElement( tag, mydoc );
      new_node->setAttributes( newa );
      append( new_node );
    }
  }

  KWargs FoliaImpl::collectAttributes() const {
    KWargs attribs;
    bool isDefaultSet = true;
    bool isDefaultAnn = true;

    if ( !_id.empty() ) {
      attribs["_id"] = _id; // sort "id" as first!
    }
    if ( !_set.empty() &&
	 _set != mydoc->defaultset( annotation_type() ) ) {
      isDefaultSet = false;
      attribs["set"] = _set;
    }
    if ( !_class.empty() ) {
      attribs["class"] = _class;
    }
    if ( !_annotator.empty() &&
	 _annotator != mydoc->defaultannotator( annotation_type(), _set ) ) {
      isDefaultAnn = false;
      attribs["annotator"] = _annotator;
    }
    if ( xlink() ) {
      auto it = _xlink.find("type");
      if ( it != _xlink.end() ){
	string type = it->second;
	if ( type == "simple" || type == "locator" ){
	  auto it = _xlink.find("href");
	  if ( it != _xlink.end() ){
	    attribs["xlink:href"] = it->second;
	    attribs["xlink:type"] = type;
	  }
	  it = _xlink.find("role");
	  if ( it != _xlink.end() ){
	    attribs["xlink:role"] = it->second;
	  }
	  it = _xlink.find("arcrole");
	  if ( it != _xlink.end() ){
	    attribs["xlink:arcrole"] = it->second;
	  }
	  it = _xlink.find("show");
	  if ( it != _xlink.end() ){
	    attribs["xlink:show"] = it->second;
	  }
	  it = _xlink.find("actuate");
	  if ( it != _xlink.end() ){
	    attribs["xlink:actuate"] = it->second;
	  }
	  it = _xlink.find("title");
	  if ( it != _xlink.end() ){
	    attribs["xlink:title"] = it->second;
	  }
	  it = _xlink.find("label");
	  if ( it != _xlink.end() ){
	    attribs["xlink:label"] = it->second;
	  }
	}
      }
    }
    if ( !_datetime.empty() &&
	 _datetime != mydoc->defaultdatetime( annotation_type(), _set ) ) {
      attribs["datetime"] = _datetime;
    }
    if ( !_begintime.empty() ) {
      attribs["begintime"] = _begintime;
    }
    if ( !_endtime.empty() ) {
      attribs["endtime"] = _endtime;
    }
    if ( !_src.empty() ) {
      attribs["src"] = _src;
    }
    if ( !_speaker.empty() ) {
      attribs["speaker"] = _speaker;
    }
    if ( _annotator_type != UNDEFINED ) {
      AnnotatorType at = stringTo<AnnotatorType>( mydoc->defaultannotatortype( annotation_type(), _set ) );
      if ( (!isDefaultSet || !isDefaultAnn) && _annotator_type != at ) {
	if ( _annotator_type == AUTO ) {
	  attribs["annotatortype"] = "auto";
	}
	else if ( _annotator_type == MANUAL ) {
	  attribs["annotatortype"] = "manual";
	}
      }
    }

    if ( _confidence >= 0 ) {
      attribs["confidence"] = TiCC::toString(_confidence);
    }
    if ( !_n.empty() ) {
      attribs["n"] = _n;
    }
    if ( !_auth ) {
      attribs["auth"] = "no";
    }
    return attribs;
  }

  const string FoliaElement::xmlstring() const{
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

  string tagToAtt( const FoliaElement* c ) {
    string att;
    if ( c->isSubClass( Feature_t ) ) {
      att = c->xmltag();
      if ( att == "feat" ) {
	// "feat" is a Feature_t too. exclude!
	att = "";
      }
      else if ( att == "headfeature" ) {
	// "head" is special
	att = "head";
      }
    }
    return att;
  }

  xmlNode *FoliaImpl::xml( bool recursive, bool kanon ) const {
    xmlNode *e = XmlNewNode( foliaNs(), xmltag() );
    KWargs attribs = collectAttributes();
    set<FoliaElement *> attribute_elements;
    // nodes that can be represented as attributes are converted to atributes
    // and excluded of 'normal' output.

    map<string,int> af_map;
    // first we search al features that can be serialized to an attribute
    // and count them!
    for ( const auto& el : data ) {
      string at = tagToAtt( el );
      if ( !at.empty() ) {
	++af_map[at];
      }
    }
    // ok, now we create attributes for those that only occur once
    for ( const auto& el : data ) {
      string at = tagToAtt( el );
      if ( !at.empty() && af_map[at] == 1 ) {
	attribs[at] = el->cls();
	attribute_elements.insert( el );
      }
    }
    addAttributes( e, attribs );
    if ( recursive ) {
      // append children:
      // we want make sure that text elements are in the right order,
      // in front and the 'current' class first
      list<FoliaElement *> currenttextelements;
      list<FoliaElement *> textelements;
      list<FoliaElement *> otherelements;
      list<FoliaElement *> commentelements;
      multimap<ElementType, FoliaElement *, std::greater<ElementType>> otherelementsMap;
      for ( const auto& el : data ) {
	if ( attribute_elements.find(el) == attribute_elements.end() ) {
	  if ( el->isinstance(TextContent_t) ) {
	    if ( el->cls() == "current" ) {
	      currenttextelements.push_back( el );
	    }
	    else {
	      textelements.push_back( el );
	    }
	  }
	  else {
	    if ( kanon ) {
	      otherelementsMap.insert( make_pair( el->element_id(), el ) );
	    }
	    else {
	      if ( el->isinstance(XmlComment_t)
		   && currenttextelements.empty()
		   && textelements.empty() ) {
		commentelements.push_back( el );
	      }
	      else {
		otherelements.push_back( el );
	      }
	    }
	  }
	}
      }
      for ( const auto& cel : commentelements ) {
	xmlAddChild( e, cel->xml( recursive, kanon ) );
      }
      for ( const auto& tel : currenttextelements ) {
	xmlAddChild( e, tel->xml( recursive, false ) );
	// don't change the internal sequences of TextContent elements
      }
      for ( const auto& tel : textelements ) {
	xmlAddChild( e, tel->xml( recursive, false ) );
	// don't change the internal sequences of TextContent elements
      }
      if ( !kanon ) {
	for ( const auto& oel : otherelements ) {
	  xmlAddChild( e, oel->xml( recursive, kanon ) );
	}
      }
      else {
	for ( const auto& oem : otherelementsMap ) {
	  xmlAddChild( e, oem.second->xml( recursive, kanon ) );
	}
      }
    }
    return e;
  }

  const string FoliaImpl::str( const string& cls ) const {
    // if this is a TextContent or it may contain TextContent
    // then return the associated text()
    // if this is a PhonContent or it may contain PhonContent
    // then return the associated phon()
    // otherwise return empty string
    UnicodeString us;
    try {
      us = text(cls);
    }
    catch( NoSuchText& e ){
      try {
	us = phon(cls);
      }
      catch( NoSuchPhon& e ){
	// No TextContent or Phone allowed
      }
    }
    return UnicodeToUTF8( us );
  }

  const string FoliaImpl::speech_src() const {
    if ( !_src.empty() ) {
      return _src;
    }
    if ( _parent ) {
      return _parent->speech_src();
    }
    return "";
  }

  const string FoliaImpl::speech_speaker() const {
    if ( !_speaker.empty() ) {
      return _speaker;
    }
    if ( _parent ) {
      return _parent->speech_speaker();
    }
    return "";
  }

  const string FoliaImpl::language( const string& st ) const {
    std::set<ElementType> exclude;
    vector<LangAnnotation*> v = select<LangAnnotation>( st, exclude, false );
    if ( v.size() > 0 ){
      return v[0]->cls();
    }
    else if ( _parent ){
      return _parent->language( st );
    }
    else {
      return doc()->language();
    }
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

  const string& FoliaImpl::getTextDelimiter( bool retaintok ) const {
#ifdef DEBUG_TEXT_DEL
    cerr << "IN " << xmltag() << "::gettextdelimiter (" << retaintok << ")" << endl;
#endif
    if ( _props.TEXTDELIMITER == "NONE" ) {
      if ( data.size() > 0 ) {
	// attempt to get a delimiter from the last child
	const string& det = data[data.size()-1]->getTextDelimiter( retaintok );
#ifdef DEBUG_TEXT_DEL
	cerr << "out" << xmltag() << "::gettextdelimiter ==> '" << det << "'" << endl;
#endif
	return det;
      }
      else
#ifdef DEBUG_TEXT_DEL
	cerr << "out" << xmltag() << "::gettextdelimiter ==> ''" << endl;
#endif
	return EMPTY_STRING;
    }
    return _props.TEXTDELIMITER;
  }

  const UnicodeString FoliaImpl::text( const string& cls,
				       bool retaintok,
				       bool strict ) const {
    // get the UnicodeString value of underlying elements
    // default cls="current"
#ifdef DEBUG_TEXT
    cerr << "TEXT(" << cls << ") op node : " << xmltag() << " id ( " << id() << ")" << endl;
#endif
    if ( strict ) {
      return textcontent(cls)->text();
    }
    else if ( is_textcontainer() ){
      UnicodeString result;
      for ( const auto& d : data ){
	if ( !result.isEmpty() ){
	  const string& delim = d->getTextDelimiter( retaintok );
	  result += UTF8ToUnicode(delim);
	}
	result += d->text( cls );
      }
#ifdef DEBUG_TEXT
      cerr << "TEXT op a textcontainer :" << xmltag() << " returned '" << result << "'" << endl;
#endif
      return result;
    }
    else if ( !printable() ) {
      throw NoSuchText( "NON printable element: " + xmltag() );
    }
    else {
      UnicodeString result = deeptext( cls, retaintok );
      if ( result.isEmpty() ) {
	result = stricttext( cls );
      }
      if ( result.isEmpty() ) {
	throw NoSuchText( "on tag " + xmltag() + " nor it's children" );
      }
      return result;
    }
  }

  const UnicodeString FoLiA::text( const string& cls,
				   bool retaintok,
				   bool strict ) const {
#ifdef DEBUG_TEXT
    cerr << "FoLiA::TEXT(" << cls << ")" << endl;
#endif
    UnicodeString result;
    for ( const auto& d : data ){
      if ( !result.isEmpty() ){
	const string& delim = d->getTextDelimiter( retaintok );
	result += UTF8ToUnicode(delim);
      }
      result += d->text( cls, retaintok, strict );
    }
#ifdef DEBUG_TEXT
    cerr << "FoLiA::TEXT returnes '" << result << "'" << endl;
#endif
    return result;
  }

  UnicodeString trim_space( const UnicodeString& in ){
    UnicodeString cmp = " ";
    //    cerr << "in = '" << in << "'" << endl;
    UnicodeString out;
    int i = 0;
    for( ; i < in.length(); ++i ){
      //      cerr << "start: bekijk:" << UnicodeString(in[i]) << endl;
      if ( in[i] != cmp[0] ){
	break;
      }
    }
    int j = in.length()-1;
    for( ; j >= 0; --j ){
      //      cerr << "end: bekijk:" << UnicodeString(in[j]) << endl;
      if ( in[j] != cmp[0] ){
	break;
      }
    }
    // cerr << "I=" << i << endl;
    // cerr << "J=" << j << endl;
    if ( j < i ){
      //      cerr << "out = LEEG" << endl;
      return out;
    }
    out = UnicodeString( in, i, j-i+1 );
    //    cerr << "out = '" << out << "'" << endl;
    return out;
  }

  bool check_end( const UnicodeString& us, bool& only ){
    only = false;
    string tmp = UnicodeToUTF8( us );
    int j = tmp.length()-1;
    size_t found_nl = 0;
    for ( ; j >=0; --j ){
      if ( tmp[j] == '\n' ){
	++found_nl;
      }
      else {
	break;
      }
    }
    only = found_nl == tmp.length();
    return found_nl > 0;
  }

  bool no_space_at_end( FoliaElement *s ){
    bool result = false;
    //    cerr << "no space? s: " << s << endl;
    if ( s ){
      vector<Word*> words = s->select<Word>(false);
      if ( !words.empty() ){
	Word *last = words.back();
	//	cerr << "no space? last: " << last << endl;
	return !last->space();
      }
    }
    return result;
  }

  const UnicodeString FoliaImpl::deeptext( const string& cls,
					   bool retaintok ) const {
    // get the UnicodeString value of underlying elements
    // default cls="current"
#ifdef DEBUG_TEXT
    cerr << "deepTEXT(" << cls << ") op node : " << xmltag() << " id(" << id() << ") cls=" << this->cls() << ")" << endl;
#endif
#ifdef DEBUG_TEXT
    cerr << "deeptext: node has " << data.size() << " children." << endl;
#endif
    vector<UnicodeString> parts;
    vector<UnicodeString> seps;
    for ( const auto& child : data ) {
      // try to get text dynamically from children
      // skip TextContent elements
#ifdef DEBUG_TEXT
      if ( !child->printable() ) {
	cerr << "deeptext: node[" << child->xmltag() << "] NOT PRINTABLE! " << endl;
      }
#endif
      if ( child->printable()
	   && ( is_structure( child )
		|| child->isSubClass( AbstractSpanAnnotation_t )
		|| child->isinstance( Correction_t ) )
	   && !child->isinstance( TextContent_t ) ) {
#ifdef DEBUG_TEXT
	cerr << "deeptext:bekijk node[" << child->xmltag() << "]"<< endl;
#endif
	try {
	  UnicodeString tmp = child->text( cls, retaintok, false );
#ifdef DEBUG_TEXT
	  cerr << "deeptext found '" << tmp << "'" << endl;
#endif
	  if ( !isSubClass(AbstractTextMarkup_t) ){
	    //	    tmp.trim();
	    tmp = trim_space( tmp );
	  }
#ifdef DEBUG_TEXT
	  cerr << "deeptext trimmed '" << tmp << "'" << endl;
#endif
	  parts.push_back(tmp);
	  if ( child->isinstance( Sentence_t )
	       && no_space_at_end(child) ){
	    const string& delim = "";
#ifdef DEBUG_TEXT
	    cerr << "deeptext: no delimiter van "<< child->xmltag() << " on"
		 << " last w of s" << endl;
#endif
	    seps.push_back(UTF8ToUnicode(delim));
	  }
	  else {
	    // get the delimiter
	    const string& delim = child->getTextDelimiter( retaintok );
#ifdef DEBUG_TEXT
	    cerr << "deeptext:delimiter van "<< child->xmltag() << " ='" << delim << "'" << endl;
#endif
	    seps.push_back(UTF8ToUnicode(delim));
	  }
	} catch ( NoSuchText& e ) {
#ifdef DEBUG_TEXT
	  cerr << "HELAAS" << endl;
#endif
	}
      }
    }

    // now construct the result;
    UnicodeString result;
    for ( size_t i=0; i < parts.size(); ++i ) {
#ifdef DEBUG_TEXT
      cerr << "part[" << i << "]='" << parts[i] << "'" << endl;
      cerr << "sep[" << i << "]='" << seps[i] << "'" << endl;
#endif
      bool only_nl = false;
      bool end_is_nl = check_end( parts[i], only_nl );
      if ( end_is_nl ){
#ifdef DEBUG_TEXT
	cerr << "a newline after: '" << parts[i] << "'" << endl;
	if ( i < parts.size()-1 ){
	  cerr << "next sep='" << seps[i+1] << "'" << endl;
	}
#endif

	if ( only_nl ){
	  // only a newline
	  result = trim_space( result );
#ifdef DEBUG_TEXT
	  cerr << "OK it is only newline(s)" << endl;
	  cerr << "TRIMMED? '" << result << "'" << endl;
#endif
	}
      }
      result += parts[i];
      if ( !end_is_nl && i < parts.size()-1 ){
	result += seps[i];
      }
#ifdef DEBUG_TEXT
      cerr << "result='" << result << "'" << endl;
#endif
    }
#ifdef DEBUG_TEXT
    cerr << "deeptext() for " << xmltag() << " step 3 " << endl;
#endif
    if ( result.isEmpty() ) {
      result = textcontent(cls)->text();
    }
#ifdef DEBUG_TEXT
    cerr << "deeptext() for " << xmltag() << " result= '" << result << "'" << endl;
#endif
    if ( result.isEmpty() ) {
      throw NoSuchText( xmltag() + ":(class=" + cls +"): empty!" );
    }
    return result;
  }

  const UnicodeString FoliaElement::stricttext( const string& cls ) const {
    // get UnicodeString content of TextContent children only
    // default cls="current"
    return this->text(cls, false, true );
  }

  const UnicodeString FoliaElement::toktext( const string& cls ) const {
    // get UnicodeString content of TextContent children only
    // default cls="current"
    return this->text(cls, true, false );
  }

  const TextContent *FoliaImpl::textcontent( const string& cls ) const {
    // Get the text explicitly associated with this element
    // (of the specified class) the default class is 'current'
    // Returns the TextContent instance rather than the actual text.
    // (so it might return iself.. ;)
    // Does not recurse into children
    // with sole exception of Correction
    // Raises NoSuchText exception if not found.
    if ( isinstance(TextContent_t) ){
      if  ( this->cls() == cls ) {
	return dynamic_cast<const TextContent*>(this);
      }
      else {
	throw NoSuchText( xmltag() + "::textcontent(" + cls + ")" );
      }
    }
    if ( !printable() ) {
      throw NoSuchText( "non-printable element: " +  xmltag() );
    }
    for ( const auto& el : data ) {
      if ( el->isinstance(TextContent_t) && (el->cls() == cls) ) {
	return dynamic_cast<TextContent*>(el);
      }
      else if ( el->element_id() == Correction_t) {
	try {
	  return el->textcontent(cls);
	} catch ( NoSuchText& e ) {
	  // continue search for other Corrections or a TextContent
	}
      }
    }
    throw NoSuchText( xmltag() + "::textcontent(" + cls + ")" );
  }

  const PhonContent *FoliaImpl::phoncontent( const string& cls ) const {
    // Get the phon explicitly associated with this element
    // (of the specified class) the default class is 'current'
    // Returns the PhonContent instance rather than the actual phoneme.
    // Does not recurse into children
    // with sole exception of Correction
    // Raises NoSuchPhon exception if not found.
    if ( isinstance(PhonContent_t) ){
      if  ( this->cls() == cls ) {
	return dynamic_cast<const PhonContent*>(this);
      }
      else {
	throw NoSuchPhon( xmltag() + "::phoncontent(" + cls + ")" );
      }
    }
    if ( !speakable() ) {
      throw NoSuchPhon( "non-speakable element: " + xmltag() );
    }

    for ( const auto& el : data ) {
      if ( el->isinstance(PhonContent_t) && ( el->cls() == cls) ) {
	return dynamic_cast<PhonContent*>(el);
      }
      else if ( el->element_id() == Correction_t) {
	try {
	  return el->phoncontent(cls);
	} catch ( NoSuchPhon& e ) {
	  // continue search for other Corrections or a TextContent
	}
      }
    }
    throw NoSuchPhon( xmltag() + "::phoncontent(" + cls + ")" );
  }

  //#define DEBUG_PHON

  const UnicodeString FoliaImpl::phon( const string& cls,
				       bool strict ) const {
    // get the UnicodeString value of underlying elements
    // default cls="current"
#ifdef DEBUG_PHON
    cerr << "PHON(" << cls << ") op node : " << xmltag() << " id ( " << id() << ")" << endl;
#endif
    if ( strict ) {
      return phoncontent(cls)->phon();
    }
    else if ( !speakable() ) {
      throw NoSuchText( "NON speakable element: " + xmltag() );
    }
    else {
      UnicodeString result = deepphon( cls );
      if ( result.isEmpty() ) {
	result = phoncontent(cls)->phon();
      }
      if ( result.isEmpty() ) {
	throw NoSuchPhon( "on tag " + xmltag() + " nor it's children" );
      }
      return result;
    }
  }

  const UnicodeString FoliaImpl::deepphon( const string& cls ) const {
    // get the UnicodeString value of underlying elements
    // default cls="current"
#ifdef DEBUG_PHON
    cerr << "deepPHON(" << cls << ") op node : " << xmltag() << " id(" << id() << ")" << endl;
#endif
#ifdef DEBUG_PHON
    cerr << "deepphon: node has " << data.size() << " children." << endl;
#endif
    vector<UnicodeString> parts;
    vector<UnicodeString> seps;
    for ( const auto& child : data ) {
      // try to get text dynamically from children
      // skip PhonContent elements
#ifdef DEBUG_PHON
      if ( !child->speakable() ) {
	cerr << "deepphon: node[" << child->xmltag() << "] NOT SPEAKABLE! " << endl;
      }
#endif
      if ( child->speakable() && !child->isinstance( PhonContent_t ) ) {
#ifdef DEBUG_PHON
	cerr << "deepphon:bekijk node[" << child->xmltag() << "]" << endl;
#endif
	try {
	  UnicodeString tmp = child->phon( cls, false );
#ifdef DEBUG_PHON
	  cerr << "deepphon found '" << tmp << "'" << endl;
#endif
	  parts.push_back(tmp);
	  // get the delimiter
	  const string& delim = child->getTextDelimiter();
#ifdef DEBUG_PHON
	  cerr << "deepphon:delimiter van "<< child->xmltag() << " ='" << delim << "'" << endl;
#endif
	  seps.push_back(UTF8ToUnicode(delim));
	} catch ( NoSuchPhon& e ) {
#ifdef DEBUG_PHON
	  cerr << "HELAAS" << endl;
#endif
	}
      }
    }

    // now construct the result;
    UnicodeString result;
    for ( size_t i=0; i < parts.size(); ++i ) {
      result += parts[i];
      if ( i < parts.size()-1 ) {
	result += seps[i];
      }
    }
#ifdef DEBUG_TEXT
    cerr << "deepphon() for " << xmltag() << " step 3 " << endl;
#endif
    if ( result.isEmpty() ) {
      try {
	result = phoncontent(cls)->phon();
      }
      catch ( ... ) {
      }
    }
#ifdef DEBUG_TEXT
    cerr << "deepphontext() for " << xmltag() << " result= '" << result << "'" << endl;
#endif
    if ( result.isEmpty() ) {
      throw NoSuchPhon( xmltag() + ":(class=" + cls +"): empty!" );
    }
    return result;
  }


  vector<FoliaElement *>FoliaImpl::findreplacables( FoliaElement *par ) const {
    return par->select( element_id(), sett(), false );
  }

  void FoliaImpl::replace( FoliaElement *child ) {
    // Appends a child element like append(), but replaces any existing child
    // element of the same type and set.
    // If no such child element exists, this will act the same as append()

    vector<FoliaElement*> replace = child->findreplacables( this );
    if ( replace.empty() ) {
      // nothing to replace, simply call append
      append( child );
    }
    else if ( replace.size() > 1 ) {
      throw runtime_error( "Unable to replace. Multiple candidates found, unable to choose." );
    }
    else {
      this->remove( replace[0], true );
      append( child );
    }
  }

  FoliaElement* FoliaImpl::replace( FoliaElement *old,
				    FoliaElement* _new ) {
    // replaced old by _new
    // returns old
    // when not found does nothing and returns 0;
    for ( auto& el: data ) {
      if ( el == old ) {
	el = _new;
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
    if ( doc() && doc()->checktext()
	 && !isSubClass( Morpheme_t ) && !isSubClass( Phoneme_t) ){
      string deeper;
      try {
	deeper = UnicodeToUTF8(text( cls, false, false ) );
	// get deep original text: no retain tokenization, no strict
      }
      catch (...){
      }
      if ( !deeper.empty() && txt != deeper ){
	throw XmlError( "settext(cls=" + cls + "): deeper text differs from attempted\ndeeper  ='" + deeper + "'\nattempted='" + txt + "'" );
      }
    }
    KWargs args;
    args["value"] = txt;
    args["class"] = cls;
    TextContent *node = new TextContent( args, doc() );
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
    if ( doc() && doc()->checktext()
	 && !isSubClass( Morpheme_t ) && !isSubClass( Phoneme_t) ){
      string deeper;
      try {
	deeper = UnicodeToUTF8(text( cls, false, false ) );
	// get deep original text: no retain tokenization, no strict
      }
      catch (...){
      }
      if ( !deeper.empty() && txt != deeper ){
	throw XmlError( "settext(cls=" + cls + "): deeper text differs from attempted\ndeeper=  '" + deeper + "'\nattempted='" + txt + "'" );
      }
    }
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

  const string FoliaElement::description() const {
    vector<FoliaElement *> v = select( Description_t, false );
    if ( v.size() == 0 ) {
      return "";
    }
    return v[0]->description();
  }

  bool FoliaImpl::acceptable( ElementType t ) const {
    auto it = accepted_data().find( t );
    if ( it == accepted_data().end() ) {
      for ( const auto& et : accepted_data() ) {
	if ( folia::isSubClass( t, et ) ) {
	  return true;
	}
      }
      return false;
    }
    return true;
  }

  bool FoliaImpl::addable( const FoliaElement *c ) const {
    if ( !acceptable( c->element_id() ) ) {
      throw ValueError( "Unable to append object of type " + c->classname()
			+ " to a " + classname() );
    }
    if ( c->occurrences() > 0 ) {
      vector<FoliaElement*> v = select( c->element_id(), false );
      size_t count = v.size();
      if ( count >= c->occurrences() ) {
	throw DuplicateAnnotationError( "Unable to add another object of type " + c->classname() + " to " + classname() + ". There are already " + TiCC::toString(count) + " instances of this type, which is the maximum." );
      }
    }
    if ( c->occurrences_per_set() > 0 &&
	 (CLASS & c->required_attributes() || c->setonly() ) ){
      vector<FoliaElement*> v = select( c->element_id(), c->sett(), false );
      size_t count = v.size();
      if ( count >= c->occurrences_per_set() ) {
	throw DuplicateAnnotationError( "Unable to add another object of type " + c->classname() + " to " + classname() + ". There are already " + TiCC::toString(count) + " instances of this type and set, which is the maximum." );
      }
    }
    if ( c->parent() &&
	 !( c->element_id() == Word_t
	    || c->element_id() == Morpheme_t
	    || c->element_id() == Phoneme_t ) ) {
      throw XmlError( "attempt to reconnect node " + c->classname()
		      + " to a " + classname() + " node, id=" + _id
		      + ", it was already connected to a "
		      +  c->parent()->classname() + " id=" + c->parent()->id() );
    }
    if ( c->element_id() == TextContent_t && element_id() == Word_t ) {
      string val = c->str();
      val = trim( val );
      if ( val.empty() ) {
	throw ValueError( "attempt to add an empty <t> to word: " + _id );
      }
    }
    if ( c->element_id() == TextContent_t ){
      string cls = c->cls();
      string st = c->sett();
      vector<TextContent*> tmp = select<TextContent>( st, false );
      if ( !tmp.empty() ) {
	for( const auto& t : tmp ){
	  if ( t->cls() == cls ){
	    throw DuplicateAnnotationError( "attempt to add <t> with class="
					    + cls + " to element: " + _id
					    + " which already has a <t> with that class" );
	  }
	}
      }
      FoliaElement *parent = this->parent();
      if ( parent && parent->element_id() != Correction_t
	   && !(parent->doc() != 0 && !parent->doc()->checktext() )
	   && parent->hastext( cls ) ){
	// check text consistency
	UnicodeString s1 = parent->text( cls, false, true );
	UnicodeString s2 = c->text( cls, false, true );
	// no retain tokenization, strict for both
	int pos = s1.indexOf( s2 );
	if ( pos < 0 ){
	  throw XmlError( "attempt to add <t> with class="
			  + cls + " and text '" + UnicodeToUTF8(s2)
			  + "' to element: " + _id + " with parent "
			  + parent->id()
			  + " which already has a <t> with that class and text: '"
			  + UnicodeToUTF8(s1) + "'" );
	}
      }
    }
    return true;
  }

  void FoliaImpl::assignDoc( Document* doc ) {
    // attach a document-less FoliaElement (tree) to its doc
    // needs checking for correct annotation_type
    // also register the ID
    // then recurse into the children
    if ( !mydoc ) {
      mydoc = doc;
      string myid = id();
      if ( !_set.empty()
	   && (CLASS & required_attributes() )
	   && !mydoc->isDeclared( annotation_type(), _set ) ) {
	throw ValueError( "Set " + _set + " is used in " + xmltag()
			  + "element: " + myid + " but has no declaration " +
			  "for " + toString( annotation_type() ) + "-annotation" );
      }
      if ( !myid.empty() ) {
	doc->addDocIndex( this, myid );
      }
      // assume that children also might be doc-less
      for ( const auto& el : data ) {
	el->assignDoc( doc );
      }
    }
  }

  bool FoliaImpl::checkAtts() {
    if ( _id.empty()
	 && (ID & required_attributes() ) ) {
      throw ValueError( "attribute 'ID' is required for " + classname() );
    }
    if ( _set.empty()
	 && (CLASS & required_attributes() ) ) {
      throw ValueError( "attribute 'set' is required for " + classname() );
    }
    if ( _class.empty()
	 && ( CLASS & required_attributes() ) ) {
      throw ValueError( "attribute 'class' is required for " + classname() );
    }
    if ( _annotator.empty()
	 && ( ANNOTATOR & required_attributes() ) ) {
      throw ValueError( "attribute 'annotator' is required for " + classname() );
    }
    if ( _annotator_type == UNDEFINED
	 && ( ANNOTATOR & required_attributes() ) ) {
      throw ValueError( "attribute 'Annotatortype' is required for " + classname() );
    }
    if ( _confidence == -1 &&
	 ( CONFIDENCE & required_attributes() ) ) {
      throw ValueError( "attribute 'confidence' is required for " + classname() );
    }
    if ( _n.empty()
	 && ( N & required_attributes() ) ) {
      throw ValueError( "attribute 'n' is required for " + classname() );
    }
    if ( _datetime.empty()
	 && ( DATETIME & required_attributes() ) ) {
      throw ValueError( "attribute 'datetime' is required for " + classname() );
    }
    if ( _begintime.empty()
	 && ( BEGINTIME & required_attributes() ) ) {
      throw ValueError( "attribute 'begintime' is required for " + classname() );
    }
    if ( _endtime.empty()
	 && ( ENDTIME & required_attributes() ) ) {
      throw ValueError( "attribute 'endtime' is required for " + classname() );
    }
    if ( _src.empty()
	 && ( SRC & required_attributes() ) ) {
      throw ValueError( "attribute 'src' is required for " + classname() );
    }
    if ( _speaker.empty()
	 && ( SPEAKER & required_attributes() ) ) {
      throw ValueError( "attribute 'speaker' is required for " + classname() );
    }
    return true;
  }

  FoliaElement *FoliaImpl::append( FoliaElement *child ) {
    if ( !child ){
      throw XmlError( "attempt to append an empty node to a " + classname() );
    }
    bool ok = false;
    try {
      ok = child->checkAtts();
      ok &= addable( child );
    }
    catch ( XmlError& ) {
      // don't delete the offending child in case of illegal reconnection
      // it will be deleted by the true parent
      throw;
    }
    catch ( exception& ) {
      delete child;
      throw;
    }
    if ( ok ) {
      if ( mydoc ){
	child->assignDoc( mydoc );
      }
      data.push_back(child);
      if ( !child->parent() ) {
	// Only for WordRef and Morpheme
	child->setParent(this);
      }
      else {
	child->increfcount();
      }
      return child->postappend();
    }
    return 0;
  }

  FoliaElement *FoliaImpl::postappend( ) {
    if ( id().empty() && (ID & required_attributes()) && auto_generate_id() ){
      _id = generateId( xmltag() );
    }
    return this;
  }

  void FoliaImpl::remove( FoliaElement *child, bool del ) {
    auto it = std::remove( data.begin(), data.end(), child );
    data.erase( it, data.end() );
    if ( del ) {
      delete child;
    }
    else {
      child->setParent(0);
    }
  }

  void FoliaImpl::remove( size_t pos, bool del ) {
    if ( pos < data.size() ) {
      auto it = data.begin();
      while ( pos > 0 ) {
	++it;
	--pos;
      }
      if ( del ) {
	delete *it;
      }
      else {
	(*it)->setParent(0);
      }
      data.erase(it);
    }
  }

  FoliaElement* FoliaImpl::index( size_t i ) const {
    if ( i < data.size() ) {
      return data[i];
    }
    throw range_error( "[] index out of range" );
  }

  FoliaElement* FoliaImpl::rindex( size_t i ) const {
    if ( i < data.size() ) {
      return data[data.size()-1-i];
    }
    throw range_error( "[] rindex out of range" );
  }

  vector<FoliaElement*> FoliaImpl::select( ElementType et,
					   const string& st,
					   const set<ElementType>& exclude,
					   bool recurse ) const {
    vector<FoliaElement*> res;
    for ( const auto& el : data ) {
      if ( el->element_id() == et &&
	   ( st.empty() || el->sett() == st ) ) {
	res.push_back( el );
      }
      if ( recurse ) {
	if ( exclude.find( el->element_id() ) == exclude.end() ) {
	  vector<FoliaElement*> tmp = el->select( et, st, exclude, recurse );
	  res.insert( res.end(), tmp.begin(), tmp.end() );
	}
      }
    }
    return res;
  }

  vector<FoliaElement*> FoliaImpl::select( ElementType et,
					   const string& st,
					   bool recurse ) const {
    return select( et, st, default_ignore, recurse );
  }

  vector<FoliaElement*> FoliaImpl::select( ElementType et,
					   const set<ElementType>& exclude,
					   bool recurse ) const {
    return select( et, "", exclude, recurse );
  }

  vector<FoliaElement*> FoliaImpl::select( ElementType et,
					   bool recurse ) const {
    return select( et, "", default_ignore, recurse );
  }

  FoliaElement* FoliaImpl::parseXml( const xmlNode *node ) {
    KWargs att = getAttributes( node );
    setAttributes( att );
    xmlNode *p = node->children;
    while ( p ) {
      string pref;
      string ns = getNS( p, pref );
      if ( !ns.empty() && ns != NSFOLIA ){
	// skip alien nodes
	if ( doc() && doc()->debug > 2 ) {
	  cerr << "skipping non-FoLiA node: " << pref << ":" << Name(p) << endl;
	}
	p = p->next;
	continue;
      }
      if ( p->type == XML_ELEMENT_NODE ) {
	string tag = Name( p );
	FoliaElement *t = createElement( tag, doc() );
	if ( t ) {
	  if ( doc() && doc()->debug > 2 ) {
	    cerr << "created " << t << endl;
	  }
	  t = t->parseXml( p );
	  if ( t ) {
	    if ( doc() && doc()->debug > 2 ) {
	      cerr << "extend " << this << " met " << t << endl;
	    }
	    append( t );
	  }
	}
	else if ( mydoc && !mydoc->permissive() ){
	  throw XmlError( "FoLiA parser terminated" );
	}
      }
      else if ( p->type == XML_COMMENT_NODE ) {
	string tag = "_XmlComment";
	FoliaElement *t = createElement( tag, doc() );
	if ( t ) {
	  if ( doc() && doc()->debug > 2 ) {
	    cerr << "created " << t << endl;
	  }
	  t = t->parseXml( p );
	  if ( t ) {
	    if ( doc() && doc()->debug > 2 ) {
	      cerr << "extend " << this << " met " << t << endl;
	    }
	    append( t );
	  }
	}
      }
      else if ( p->type == XML_TEXT_NODE ) {
	string tag = "_XmlText";
	FoliaElement *t = createElement( tag, doc() );
	if ( t ) {
	  if ( doc() && doc()->debug > 2 ) {
	    cerr << "created " << t << endl;
	  }
	  try {
	    t = t->parseXml( p );
	  }
	  catch ( ValueError& ){
	    // ignore empty content
	    delete t;
	    t = 0;
	  }
	  if ( t ) {
	    if ( doc() && doc()->debug > 2 ) {
	      cerr << "extend " << this << " met " << t << endl;
	    }
	    append( t );
	  }
	}
      }
      p = p->next;
    }
    if ( doc() && ( doc()->checktext() || doc()->fixtext() )
	 && is_structure( this )
	 && !isSubClass( Morpheme_t ) && !isSubClass( Phoneme_t) ){
      vector<TextContent*> tv = select<TextContent>( false );
      // first see which text classes ar present
      set<string> cls;
      for ( const auto& it : tv ){
	cls.insert( it->cls() );
      }
      // check the text for every text class
      for ( const auto& st : cls ){
	UnicodeString s1, s2;
	try {
	  s1 = text( st, false, true );  // no retain tokenization, strict
	}
	catch (...){
	}
	if ( !s1.isEmpty() ){
	  try {
	    s2 = text( st, false, false ); // no retain tokenization, no strict
	  }
	  catch (...){
	  }
	  if ( !s2.isEmpty() && s1 != s2 ){
	    string mess = "node " + xmltag() + "(" + id()
	      + ") has a mismatch for the text in set:" + st
	      + "\nthe element text ='" + UnicodeToUTF8(s1)
	      + "'\n" + "the deeper text ='" + UnicodeToUTF8(s2) + "'";
	    if ( doc()->fixtext() ){
	      //	      cerr << "FIX: " << mess << endl;
	      KWargs args;
	      args["value"] = UnicodeToUTF8(s2);
	      args["class"] = st;
	      TextContent *node = new TextContent( args, doc() );
	      this->replace( node );
	    }
	    else {
	      throw( XmlError( mess ) );
	    }
	  }
	}
      }
    }
    return this;
  }

  void FoliaImpl::setDateTime( const string& s ) {
    Attrib supported = required_attributes() | optional_attributes();
    if ( !(DATETIME & supported) ) {
      throw ValueError("datetime is not supported for " + classname() );
    }
    else {
      string time = parseDate( s );
      if ( time.empty() ) {
	throw ValueError( "invalid datetime, must be in YYYY-MM-DDThh:mm:ss format: " + s );
      }
      _datetime = time;
    }
  }

  const string FoliaImpl::getDateTime() const {
    return _datetime;
  }

  const string FoliaImpl::pos( const string& st ) const {
    return annotation<PosAnnotation>( st )->cls();
  }

  const string FoliaImpl::lemma( const string& st ) const {
    return annotation<LemmaAnnotation>( st )->cls();
  }

  PosAnnotation *AllowAnnotation::addPosAnnotation( const KWargs& inargs ) {
    KWargs args = inargs;
    string st;
    auto it = args.find("set" );
    if ( it != args.end() ) {
      st = it->second;
    }
    string newId = "alt-pos";
    it = args.find("generate_id" );
    if ( it != args.end() ) {
      newId = it->second;
      args.erase("generate_id");
    }
    if ( hasannotation<PosAnnotation>( st ) > 0 ) {
      // ok, there is already one, so create an Alternative
      KWargs kw;
      kw["id"] = generateId( newId );
      Alternative *alt = new Alternative( kw, doc() );
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
    catch( NoSuchAnnotation& e ) {
      res = 0;
    }
    // now search for alternatives
    vector<Alternative *> alts = select<Alternative>( AnnoExcludeSet );
    for ( const auto& alt : alts ){
      if ( alt->size() > 0 ) { // child elements?
	for ( size_t j=0; j < alt->size(); ++j ) {
	  if ( alt->index(j)->element_id() == PosAnnotation_t &&
	       ( st.empty() || alt->index(j)->sett() == st ) ) {
	    vec.push_back( dynamic_cast<PosAnnotation*>(alt->index(j)) );
	  }
	}
      }
    }
    return res;
  }

  LemmaAnnotation *AllowAnnotation::addLemmaAnnotation( const KWargs& inargs ) {
    KWargs args = inargs;
    string st;
    auto it = args.find("set" );
    if ( it != args.end() ) {
      st = it->second;
    }
    string newId = "alt-lem";
    it = args.find("generate_id" );
    if ( it != args.end() ) {
      newId = it->second;
      args.erase("generate_id");
    }
    if ( hasannotation<LemmaAnnotation>( st ) > 0 ) {
      // ok, there is already one, so create an Alternative
      KWargs kw;
      kw["id"] = generateId( newId );
      Alternative *alt = new Alternative( kw, doc() );
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
    catch( NoSuchAnnotation& e ) {
      // ok
      res = 0;
    }
    vector<Alternative *> alts = select<Alternative>( AnnoExcludeSet );
    for ( const auto& alt : alts ){
      if ( alt->size() > 0 ) { // child elements?
	for ( size_t j =0; j < alt->size(); ++j ) {
	  if ( alt->index(j)->element_id() == LemmaAnnotation_t &&
	       ( st.empty() || alt->index(j)->sett() == st ) ) {
	    vec.push_back( dynamic_cast<LemmaAnnotation*>(alt->index(j)) );
	  }
	}
      }
    }
    return res;
  }

  MorphologyLayer *Word::addMorphologyLayer( const KWargs& inargs ) {
    KWargs args = inargs;
    string st;
    auto it = args.find("set" );
    if ( it != args.end() ) {
      st = it->second;
    }
    string newId = "alt-mor";
    it = args.find("generate_id" );
    if ( it != args.end() ) {
      newId = it->second;
      args.erase("generate_id");
    }
    if ( hasannotation<MorphologyLayer>( st ) > 0 ) {
      // ok, there is already one, so create an Alternative
      KWargs kw;
      kw["id"] = generateId( newId );
      Alternative *alt = new Alternative( kw, doc() );
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
    catch( NoSuchAnnotation& e ) {
      // ok
      res = 0;
    }
    // now search for alternatives
    vector<Alternative *> alts = select<Alternative>( AnnoExcludeSet );
    for ( const auto& alt : alts ){
      if ( alt->size() > 0 ) { // child elements?
	for ( size_t j =0; j < alt->size(); ++j ) {
	  if ( alt->index(j)->element_id() == MorphologyLayer_t &&
	       ( st.empty() || alt->index(j)->sett() == st ) ) {
	    vec.push_back( dynamic_cast<MorphologyLayer*>(alt->index(j)) );
	  }
	}
      }
    }
    return res;
  }

  Sentence *FoliaImpl::addSentence( const KWargs& args ) {
    Sentence *res = new Sentence( mydoc );
    KWargs kw = args;
    if ( kw["id"].empty() ) {
      string id = generateId( "s" );
      kw["id"] = id;
    }
    try {
      res->setAttributes( kw );
    }
    catch( DuplicateIDError& e ) {
      delete res;
      throw;
    }
    append( res );
    return res;
  }

  Word *FoliaImpl::addWord( const KWargs& args ) {
    Word *res = new Word( mydoc );
    KWargs kw = args;
    if ( kw["id"].empty() ) {
      string id = generateId( "w" );
      kw["id"] = id;
    }
    try {
      res->setAttributes( kw );
    }
    catch( DuplicateIDError& e ) {
      delete res;
      throw;
    }
    append( res );
    return res;
  }

  const string& Quote::getTextDelimiter( bool retaintok ) const {
#ifdef DEBUG_TEXT_DEL
    cerr << "IN " << xmltag() << "::gettextdelimiter (" << retaintok << ")" << endl;
#endif
    auto it = data.rbegin();
    while ( it != data.rend() ) {
      if ( (*it)->isinstance( Sentence_t ) ) {
	// if a quote ends in a sentence, we don't want any delimiter
#ifdef DEBUG_TEXT_DEL
	cerr << "OUT " << xmltag() << "::gettextdelimiter ==>''" << endl;
#endif
	return EMPTY_STRING;
      }
      else {
	const string& res = (*it)->getTextDelimiter( retaintok );
#ifdef DEBUG_TEXT_DEL
	cerr << "OUT " << xmltag() << "::gettextdelimiter ==> '"
	     << res << "'" << endl;
#endif
	return res;
      }
      ++it;
    }
    static const string SPACE = " ";
    return SPACE;
  }

  vector<Word*> Quote::wordParts() const {
    vector<Word*> result;
    for ( const auto& pnt : data ) {
      if ( pnt->isinstance( Word_t ) ) {
	result.push_back( dynamic_cast<Word*>(pnt) );
      }
      else if ( pnt->isinstance( Sentence_t ) ) {
	KWargs args;
	args["text"] = pnt->id();
	PlaceHolder *p = new PlaceHolder( args, mydoc );
	mydoc->keepForDeletion( p );
	result.push_back( p );
      }
      else if ( pnt->isinstance( Quote_t ) ) {
	vector<Word*> tmp = pnt->wordParts();
	result.insert( result.end(), tmp.begin(), tmp.end() );
      }
      else if ( pnt->isinstance( Description_t ) ) {
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
    for ( const auto& pnt : data ) {
      if ( pnt->isinstance( Word_t ) ) {
	result.push_back( dynamic_cast<Word*>(pnt) );
      }
      else if ( pnt->isinstance( Quote_t ) ) {
	vector<Word*> v = pnt->wordParts();
	result.insert( result.end(), v.begin(),v.end() );
      }
      else {
	// skip all other stuff. Is there any?
      }
    }
    return result;
  }

  Correction *Sentence::splitWord( FoliaElement *orig, FoliaElement *p1, FoliaElement *p2, const KWargs& args ) {
    vector<FoliaElement*> ov;
    ov.push_back( orig );
    vector<FoliaElement*> nv;
    nv.push_back( p1 );
    nv.push_back( p2 );
    return correctWords( ov, nv, args );
  }

  Correction *Sentence::mergewords( FoliaElement *nw,
				    const vector<FoliaElement *>& orig,
				    const string& args ) {
    vector<FoliaElement*> nv;
    nv.push_back( nw );
    return correctWords( orig, nv, getArgs(args) );
  }

  Correction *Sentence::deleteword( FoliaElement *w,
				    const string& args ) {
    vector<FoliaElement*> ov;
    ov.push_back( w );
    vector<FoliaElement*> nil1;
    return correctWords( ov, nil1, getArgs(args) );
  }

  Correction *Sentence::insertword( FoliaElement *w,
				    FoliaElement *p,
				    const string& args ) {
    if ( !p || !p->isinstance( Word_t ) ) {
      throw runtime_error( "insertword(): previous is not a Word " );
    }
    if ( !w || !w->isinstance( Word_t ) ) {
      throw runtime_error( "insertword(): new word is not a Word " );
    }
    auto it = data.begin();
    while ( it != data.end() ) {
      if ( *it == p ) {
	KWargs kwargs;
	kwargs["text"] = "dummy";
	kwargs["id"] = "dummy";
	Word *tmp = new Word( kwargs );
	tmp->setParent( this ); // we create a dummy Word as member of the
	// Sentence. This makes correctWords() happy
	it = data.insert( ++it, tmp );
	break;
      }
      ++it;
    }
    if ( it == data.end() ) {
      throw runtime_error( "insertword(): previous not found" );
    }
    vector<FoliaElement *> ov;
    ov.push_back( *it );
    vector<FoliaElement *> nv;
    nv.push_back( w );
    // so we attempt to 'correct' the dummy word into w
    return correctWords( ov, nv, getArgs(args) );
  }

  Correction *Sentence::correctWords( const vector<FoliaElement *>& orig,
				      const vector<FoliaElement *>& _new,
				      const KWargs& argsin ) {
    // Generic correction method for words. You most likely want to use the helper functions
    //      splitword() , mergewords(), deleteword(), insertword() instead

    // sanity check:
    for ( const auto& org : orig ) {
      if ( !org || !org->isinstance( Word_t) ) {
	throw runtime_error("Original word is not a Word instance" );
      }
      else if ( org->sentence() != this ) {
	throw runtime_error( "Original not found as member of sentence!");
      }
    }
    for ( const auto& nw : _new ) {
      if ( ! nw->isinstance( Word_t) ) {
	throw runtime_error("new word is not a Word instance" );
      }
    }
    auto ait = argsin.find("suggest");
    if ( ait != argsin.end() && ait->second == "true" ) {
      FoliaElement *sugg = new Suggestion();
      for ( const auto& nw : _new ) {
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

  void TextContent::setAttributes( const KWargs& args ) {
    KWargs kwargs = args; // need to copy
    auto it = kwargs.find( "value" );
    if ( it != kwargs.end() ) {
      XmlText *t = new XmlText();
      string value = it->second;
      if ( value.empty() ) {
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
    it = kwargs.find( "class" );
    if ( it == kwargs.end() ) {
      kwargs["class"] = "current";
    }
    FoliaImpl::setAttributes(kwargs);
  }

  void PhonContent::setAttributes( const KWargs& args ) {
    KWargs kwargs = args; // need to copy
    auto it = kwargs.find( "offset" );
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
    it = kwargs.find( "class" );
    if ( it == kwargs.end() ) {
      kwargs["class"] = "current";
    }
    FoliaImpl::setAttributes(kwargs);
  }

  KWargs TextContent::collectAttributes() const {
    KWargs attribs = FoliaImpl::collectAttributes();
    if ( _class == "current" ) {
      attribs.erase( "class" );
    }
    // else if ( _class == "original" && parent() && parent()->isinstance( Original_t ) ) {
    //   attribs.erase( "class" );
    // }

    if ( _offset >= 0 ) {
      attribs["offset"] = TiCC::toString( _offset );
    }
    return attribs;
  }

  KWargs PhonContent::collectAttributes() const {
    KWargs attribs = FoliaImpl::collectAttributes();
    if ( _class == "current" ) {
      attribs.erase( "class" );
    }
    if ( _offset >= 0 ) {
      attribs["offset"] = TiCC::toString( _offset );
    }
    return attribs;
  }

  void Linebreak::setAttributes( const KWargs& args_in ){
    KWargs args = args_in;
    auto it = args.find( "pagenr" );
    if ( it != args.end() ) {
      _pagenr = it->second;
      args.erase( it );
    }
    it = args.find( "linenr" );
    if ( it != args.end() ) {
      _linenr = it->second;
      args.erase( it );
    }
    it = args.find( "newpage" );
    if ( it != args.end() ) {
      _newpage = ( it->second == "yes" );
      args.erase( it );
    }
    FoliaImpl::setAttributes( args );
  }

  KWargs Linebreak::collectAttributes() const {
    KWargs atts;
    if ( ! _linenr.empty() ){
      atts["linenr"] = _linenr;
    }
    if ( ! _pagenr.empty() ){
      atts["pagenr"] = _pagenr;
    }
    if ( _newpage ){
      atts["newpage"] = "yes";
    }
    return atts;
  }

  vector<FoliaElement *>TextContent::findreplacables( FoliaElement *par ) const {
    vector<FoliaElement *> result;
    vector<TextContent*> v = par->FoliaElement::select<TextContent>( _set, false );
    // cerr << "TextContent::findreplacable found " << v << endl;
    for ( const auto& el:v ) {
      // cerr << "TextContent::findreplacable bekijkt " << el << " ("
      if ( el->cls() == _class ) {
	result.push_back( el );
      }
    }
    //  cerr << "TextContent::findreplacable resultaat " << v << endl;
    return result;
  }

  const UnicodeString PhonContent::phon( const string& cls,
					 bool ) const {
    // get the UnicodeString value of underlying elements
    // default cls="current"
#ifdef DEBUG_PHON
    cerr << "PhonContent::PHON(" << cls << ") " << endl;
#endif
    UnicodeString result;
    for ( const auto& el : data ) {
      // try to get text dynamically from children
#ifdef DEBUG_PHON
      cerr << "PhonContent: bekijk node[" << el->str(cls) << endl;
#endif
      try {
#ifdef DEBUG_PHON
	cerr << "roep text(" << cls << ") aan op " << el << endl;
#endif
	UnicodeString tmp = el->text( cls );
#ifdef DEBUG_PHON
	cerr << "PhonContent found '" << tmp << "'" << endl;
#endif
	result += tmp;
      } catch ( NoSuchPhon& e ) {
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

  const string AllowGenerateID::generateId( const string& tag ){
    // generate an new ID using my ID
    // if no ID, look upward.
    string nodeId = id();
    // cerr << "node: " << this << endl;
    // cerr << "ID=" << nodeId << endl;
    if ( nodeId.empty() ){
      FoliaElement *par = parent();
      if ( !par ){
	throw XmlError( "unable to generate an ID. No StructureElement parent found?" );
      }
      // cerr << "call on parent:" << par << endl;
      return par->generateId( tag );
    }
    else {
      int max = 0;
      if ( !tag.empty() ) {
	max = ++id_map[tag];
      }
      // cerr << "MAX = " << max << endl;
      string id = nodeId + '.' + tag + '.' +  TiCC::toString( max );
      // cerr << "new id = " << id << endl;
      return id;
    }
  }

  void AllowGenerateID::setMaxId( FoliaElement *child ) {
    if ( !child->id().empty() && !child->xmltag().empty() ) {
      vector<string> parts;
      size_t num = TiCC::split_at( child->id(), parts, "." );
      if ( num > 0 ) {
	string val = parts[num-1];
	int i;
	try {
	  i = stringTo<int>( val );
	}
	catch ( exception ) {
	  // no number, so assume so user defined id
	  return;
	}
	const auto& it = id_map.find( child->xmltag() );
	if ( it == id_map.end() ) {
	  id_map[child->xmltag()] = i;
	}
	else {
	  if ( it->second < i ) {
	    it->second = i;
	  }
	}
      }
    }
  }

  //#define DEBUG_CORRECT 1

  Correction * AllowCorrection::correct( const vector<FoliaElement*>& _original,
					 const vector<FoliaElement*>& current,
					 const vector<FoliaElement*>& _newv,
					 const vector<FoliaElement*>& _suggestions,
					 const KWargs& args_in ) {
#ifdef DEBUG_CORRECT
    cerr << "correct " << this << endl;
    cerr << "original= " << _original << endl;
    cerr << "current = " << current << endl;
    cerr << "new     = " << _newv << endl;
    cerr << "suggestions     = " << _suggestions << endl;
    cerr << "args in     = " << args_in << endl;
#endif
    // Apply a correction
    Document *mydoc = doc();
    Correction *corr = 0;
    bool suggestionsonly = false;
    bool hooked = false;
    New *addnew = 0;
    KWargs args = args_in;
    vector<FoliaElement*> original = _original;
    vector<FoliaElement*> _new = _newv;
    vector<FoliaElement*> suggestions = _suggestions;
    auto it = args.find("new");
    if ( it != args.end() ) {
      KWargs my_args;
      my_args["value"] = it->second;
      TextContent *t = new TextContent( my_args, mydoc );
      _new.push_back( t );
      args.erase( it );
    }
    it = args.find("suggestion");
    if ( it != args.end() ) {
      KWargs my_args;
      my_args["value"] = it->second;
      TextContent *t = new TextContent( my_args, mydoc );
      suggestions.push_back( t );
      args.erase( it );
    }
    it = args.find("reuse");
    if ( it != args.end() ) {
      // reuse an existing correction instead of making a new one
      try {
	corr = dynamic_cast<Correction*>(mydoc->index(it->second));
      }
      catch ( exception& e ) {
	throw ValueError("reuse= must point to an existing correction id!");
      }
      if ( !corr->isinstance( Correction_t ) ) {
	throw ValueError("reuse= must point to an existing correction id!");
      }
      hooked = true;
      suggestionsonly = (!corr->hasNew()
			 && !corr->hasOriginal()
			 && corr->hasSuggestions() );
      if ( !_new.empty() && corr->hasCurrent() ) {
	// can't add new if there's current, so first set original to current, and then delete current

	if ( !current.empty() ) {
	  throw runtime_error( "Can't set both new= and current= !");
	}
	if ( original.empty() ) {
	  FoliaElement *cur = corr->getCurrent();
	  original.push_back( cur );
	  corr->remove( cur, false );
	}
      }
    }
    else {
      KWargs args2 = args;
      args2.erase("suggestion" );
      args2.erase("suggestions" );
      string id = generateId( "correction" );
      args2["id"] = id;
      corr = new Correction( args2, mydoc );
    }
#ifdef DEBUG_CORRECT
    cerr << "now corr= " << corr << endl;
#endif
    if ( !current.empty() ) {
      if ( !original.empty() || !_new.empty() ) {
	throw runtime_error("When setting current=, original= and new= can not be set!");
      }
      for ( const auto& cur : current ) {
	FoliaElement *add = new Current( mydoc );
	cur->setParent(0);
	add->append( cur );
	corr->replace( add );
	if ( !hooked ) {
	  for ( size_t i=0; i < size(); ++i ) {
	    if ( index(i) == cur ) {
	      replace( index(i), corr );
	      hooked = true;
	    }
	  }
	}
      }
#ifdef DEBUG_CORRECT
      cerr << "now corr= " << corr << endl;
#endif
    }
    if ( !_new.empty() ) {
#ifdef DEBUG_CORRECT
      cerr << "there is new! " << endl;
#endif
      addnew = new New( mydoc );
      corr->append(addnew);
      for ( const auto& nw : _new ) {
	nw->setParent(0);
	addnew->append( nw );
      }
#ifdef DEBUG_CORRECT
      cerr << "after adding " << corr << endl;
#endif
      vector<Current*> v = corr->FoliaElement::select<Current>();
      //delete current if present
      for ( const auto& cur:v ) {
	corr->remove( cur, false );
      }
#ifdef DEBUG_CORRECT
      cerr << "after removing cur " << corr << endl;
#endif
    }
    if ( !original.empty() ) {
#ifdef DEBUG_CORRECT
      cerr << "there is original! " << endl;
#endif
      FoliaElement *add = new Original( mydoc );
      corr->replace(add);
#ifdef DEBUG_CORRECT
      cerr << " corr after replace " << corr << endl;
      cerr << " new original= " << add << endl;
#endif
      for ( const auto& org: original ) {
#ifdef DEBUG_CORRECT
	cerr << " examine org " << org << endl;
#endif
	bool dummyNode = ( org->id() == "dummy" );
	if ( !dummyNode ) {
	  org->setParent(0);
	  add->append( org );
	}
#ifdef DEBUG_CORRECT
	cerr << " NOW original= " << add << endl;
#endif
	for ( size_t i=0; i < size(); ++i ) {
#ifdef DEBUG_CORRECT
	  cerr << "in loop, bekijk " << index(i) << endl;
#endif
	  if ( index(i) == org ) {
#ifdef DEBUG_CORRECT
	    cerr << "OK hit on ORG" << endl;
#endif
	    if ( !hooked ) {
#ifdef DEBUG_CORRECT
	      cerr << "it isn't hooked!" << endl;
	      FoliaElement * tmp = replace( index(i), corr );
	      cerr << " corr after replace " << corr << endl;
	      cerr << " replaced " << tmp << endl;
#else
	      replace( index(i), corr );
#endif
	      hooked = true;
	    }
	    else {
#ifdef DEBUG_CORRECT
	      cerr << " corr before remove " << corr << endl;
	      cerr << " remove  " << org << endl;
#endif
	      this->remove( org, false );
#ifdef DEBUG_CORRECT
	      cerr << " corr after remove " << corr << endl;
#endif
	    }
	  }
	}
      }
    }
    else if ( addnew ) {
      // original not specified, find automagically:
      vector<FoliaElement *> orig;
#ifdef DEBUG_CORRECT
      cerr << "start to look for original " << endl;
#endif
      for ( size_t i=0; i < len(addnew); ++ i ) {
	FoliaElement *p = addnew->index(i);
#ifdef DEBUG_CORRECT
	cerr << "bekijk " << p << endl;
#endif
	vector<FoliaElement*> v = p->findreplacables( this );
	for ( const auto& el: v ) {
	  orig.push_back( el );
	}
      }
      if ( orig.empty() ) {
	throw runtime_error( "No original= specified and unable to automatically infer");
      }
      else {
#ifdef DEBUG_CORRECT
	cerr << "we seem to have some originals! " << endl;
#endif
	FoliaElement *add = new Original( mydoc );
#ifdef DEBUG_CORRECT
	cerr << "corr before adding new original! " << corr << endl;
#endif
	corr->replace(add);
#ifdef DEBUG_CORRECT
	cerr << "corr after adding new original! " << corr << endl;
	cerr << "now parent = " << add->parent() << endl;
#endif

	for ( const auto& org: orig ) {
#ifdef DEBUG_CORRECT
	  cerr << " examine original : " << org << endl;
	  cerr << "with parent = " << org->parent() << endl;
#endif
	  // first we lookup org in our data and remove it there
	  for ( size_t i=0; i < size(); ++i ) {
#ifdef DEBUG_CORRECT
	    cerr << "in loop, bekijk " << index(i) << endl;
#endif
	    if ( index(i) == org ) {
#ifdef DEBUG_CORRECT
	      cerr << "found original " << endl;
#endif
	      if ( !hooked ) {
#ifdef DEBUG_CORRECT
		cerr << "it isn't hooked!" << endl;
		FoliaElement *tmp = replace( index(i), corr );
		cerr << " corr after replace " << corr << endl;
		cerr << " replaced " << tmp << endl;
#else
		replace( index(i), corr );
#endif

		hooked = true;
	      }
	      else {
#ifdef DEBUG_CORRECT
		cerr << " corr before remove " << corr << endl;
		cerr << " remove  " << org << endl;
#endif
		this->remove( org, false );
#ifdef DEBUG_CORRECT
		cerr << " corr after remove " << corr << endl;
#endif
	      }
	    }
	  }
	  // now we conect org to the new original node
	  org->setParent( 0 );
	  add->append( org );
#ifdef DEBUG_CORRECT
	  cerr << " add after append : " << add << endl;
	  cerr << "parent = " << org->parent() << endl;
#endif
	}
	vector<Current*> v = corr->FoliaElement::select<Current>();
	//delete current if present
	for ( const auto& cur: v ) {
#ifdef DEBUG_CORRECT
	  cerr << " remove cur=" << cur << endl;
#endif
	  this->remove( cur, false );
	}
      }
    }
#ifdef DEBUG_CORRECT
    cerr << " corr after edits " << corr << endl;
#endif
    if ( addnew ) {
      for ( const auto& org : original ) {
#ifdef DEBUG_CORRECT
	cerr << " remove  " << org << endl;
#endif
	bool dummyNode = ( org->id() == "dummy" );
	corr->remove( org, dummyNode );
      }
    }
#ifdef DEBUG_CORRECT
    cerr << " corr after removes " << corr << endl;
#endif
    if ( !suggestions.empty() ) {
      if ( !hooked ) {
	append(corr);
      }
      for ( const auto& sug : suggestions ) {
	if ( sug->isinstance( Suggestion_t ) ) {
	  sug->setParent(0);
	  corr->append( sug );
	}
	else {
	  FoliaElement *add = new Suggestion( mydoc );
	  sug->setParent(0);
	  add->append( sug );
	  corr->append( add );
	}
      }
    }

    it = args.find("reuse");
    if ( it != args.end() ) {
      if ( addnew && suggestionsonly ) {
	vector<Suggestion*> sv = corr->suggestions();
	for ( const auto& sug : sv ){
	  if ( !corr->annotator().empty() && sug->annotator().empty() ) {
	    sug->annotator( corr->annotator() );
	  }
	  if ( !(corr->annotatortype() == UNDEFINED) &&
	       (sug->annotatortype() == UNDEFINED ) ) {
	    sug->annotatortype( corr->annotatortype() );
	  }
	}
      }
      it = args.find("annotator");
      if ( it != args.end() ) {
	corr->annotator( it->second );
      }
      it = args.find("annotatortype");
      if ( it != args.end() )
	corr->annotatortype( stringTo<AnnotatorType>(it->second) );
      it = args.find("confidence");
      if ( it != args.end() ) {
	corr->confidence( stringTo<double>(it->second) );
      }
    }
    return corr;
  }

  Correction *AllowCorrection::correct( const string& s ) {
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
					const KWargs& args ) {
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

  FoliaElement *AbstractStructureElement::append( FoliaElement *child ) {
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
    if ( index < v.size() ) {
      return v[index];
    }
    throw range_error( "sentences(): index out of range" );
  }

  Sentence *AbstractStructureElement::rsentences( size_t index ) const {
    vector<Sentence*> v = sentences();
    if ( index < v.size() ) {
      return v[v.size()-1-index];
    }
    throw range_error( "rsentences(): index out of range" );
  }

  Paragraph *AbstractStructureElement::paragraphs( size_t index ) const {
    vector<Paragraph*> v = paragraphs();
    if ( index < v.size() ) {
      return v[index];
    }
    throw range_error( "paragraphs(): index out of range" );
  }

  Paragraph *AbstractStructureElement::rparagraphs( size_t index ) const {
    vector<Paragraph*> v = paragraphs();
    if ( index < v.size() ) {
      return v[v.size()-1-index];
    }
    throw range_error( "rparagraphs(): index out of range" );
  }

  Word *AbstractStructureElement::words( size_t index ) const {
    vector<Word*> v = words();
    if ( index < v.size() ) {
      return v[index];
    }
    throw range_error( "words(): index out of range" );
  }

  Word *AbstractStructureElement::rwords( size_t index ) const {
    vector<Word*> v = words();
    if ( index < v.size() ) {
      return v[v.size()-1-index];
    }
    throw range_error( "rwords(): index out of range" );
  }

  const Word* AbstractStructureElement::resolveword( const string& id ) const{
    const Word *result = 0;
    for ( const auto& el : data ) {
      result = el->resolveword( id );
      if ( result ) {
	break;
      }
    }
    return result;
  }

  vector<Alternative *> AbstractStructureElement::alternatives( ElementType elt,
								const string& st ) const {
    // Return a list of alternatives, either all or only of a specific type, restrained by set
    vector<Alternative *> alts = FoliaElement::select<Alternative>( AnnoExcludeSet );
    if ( elt == BASE ) {
      return alts;
    }
    else {
      vector<Alternative*> res;
      for ( const auto& alt : alts ){
	if ( alt->size() > 0 ) { // child elements?
	  for ( size_t j =0; j < alt->size(); ++j ) {
	    if ( alt->index(j)->element_id() == elt &&
		 ( alt->sett().empty() || alt->sett() == st ) ) {
	      res.push_back( alt ); // not the child!
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
    if ( !_t.empty() ) {
      atts["t"] = _t;
    }
    return atts;
  }

  void AlignReference::setAttributes( const KWargs& argsin ) {
    KWargs args = argsin;
    auto it = args.find( "id" );
    if ( it != args.end() ) {
      refId = it->second;
      args.erase(it);
    }
    it = args.find( "type" );
    if ( it != args.end() ) {
      ref_type = it->second;
      args.erase(it);
    }
    it = args.find( "t" );
    if ( it != args.end() ) {
      _t = it->second;
      args.erase(it);
    }
    FoliaImpl::setAttributes(args);
  }

  void Word::setAttributes( const KWargs& args_in ) {
    KWargs args = args_in;
    auto it = args.find( "space" );
    if ( it != args.end() ) {
      if ( it->second == "no" ) {
	_space = false;
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
    if ( !_space ) {
      atts["space"] = "no";
    }
    return atts;
  }

  const string& Word::getTextDelimiter( bool retaintok ) const {
    if ( _space || retaintok ) {
      return PROPS.TEXTDELIMITER;
    }
    return EMPTY_STRING;
  }

  Correction *Word::split( FoliaElement *part1, FoliaElement *part2,
			   const string& args ) {
    return sentence()->splitWord( this, part1, part2, getArgs(args) );
  }

  FoliaElement *Word::append( FoliaElement *child ) {
    if ( child->isSubClass( AbstractAnnotationLayer_t ) ) {
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
    while( p ) {
      if ( p->isinstance( Sentence_t ) ) {
	return dynamic_cast<Sentence*>(p);
      }
      p = p->parent();
    }
    return 0;
  }

  Paragraph *Word::paragraph( ) const {
    // return the sentence this word is a part of, otherwise return null
    FoliaElement *p = _parent;
    while( p ) {
      if ( p->isinstance( Paragraph_t ) ) {
	return dynamic_cast<Paragraph*>(p);
      }
      p = p->parent();
    }
    return 0;
  }

  Division *Word::division() const {
    // return the <div> this word is a part of, otherwise return null
    FoliaElement *p = _parent;
    while( p ) {
      if ( p->isinstance( Division_t ) ) {
	return dynamic_cast<Division*>(p);
      }
      p = p->parent();
    }
    return 0;
  }

  vector<Morpheme *> Word::morphemes( const string& set ) const {
    vector<Morpheme *> result;
    vector<MorphologyLayer*> mv = FoliaElement::select<MorphologyLayer>();
    for ( const auto& mor : mv ){
      vector<Morpheme*> tmp = mor->FoliaElement::select<Morpheme>( set );
      result.insert( result.end(), tmp.begin(), tmp.end() );
    }
    return result;
  }

  Morpheme * Word::morpheme( size_t pos, const string& set ) const {
    vector<Morpheme *> tmp = morphemes( set );
    if ( pos < tmp.size() ) {
      return tmp[pos];
    }
    throw range_error( "morpheme() index out of range" );
  }

  Correction *Word::incorrection( ) const {
    // Is the Word part of a correction? If it is, it returns the Correction element, otherwise it returns 0;
    FoliaElement *p = _parent;
    while ( p ) {
      if ( p->isinstance( Correction_t ) ) {
	return dynamic_cast<Correction*>(p);
      }
      else if ( p->isinstance( Sentence_t ) )
	break;
      p = p->parent();
    }
    return 0;
  }

  Word *Word::previous() const{
    Sentence *s = sentence();
    vector<Word*> words = s->words();
    for ( size_t i=0; i < words.size(); ++i ) {
      if ( words[i] == this ) {
	if ( i > 0 ) {
	  return words[i-1];
	}
	else {
	  return 0;
	}
	break;
      }
    }
    return 0;
  }

  Word *Word::next() const{
    Sentence *s = sentence();
    vector<Word*> words = s->words();
    for ( size_t i=0; i < words.size(); ++i ) {
      if ( words[i] == this ) {
	if ( i+1 < words.size() ) {
	  return words[i+1];
	}
	else {
	  return 0;
	}
	break;
      }
    }
    return 0;
  }

  vector<Word*> Word::context( size_t size,
			       const string& val ) const {
    vector<Word*> result;
    if ( size > 0 ) {
      vector<Word*> words = mydoc->words();
      for ( size_t i=0; i < words.size(); ++i ) {
	if ( words[i] == this ) {
	  size_t miss = 0;
	  if ( i < size ) {
	    miss = size - i;
	  }
	  for ( size_t index=0; index < miss; ++index ) {
	    if ( val.empty() ) {
	      result.push_back( 0 );
	    }
	    else {
	      KWargs args;
	      args["text"] = val;
	      PlaceHolder *p = new PlaceHolder( args );
	      mydoc->keepForDeletion( p );
	      result.push_back( p );
	    }
	  }
	  for ( size_t index=i-size+miss; index < i + size + 1; ++index ) {
	    if ( index < words.size() ) {
	      result.push_back( words[index] );
	    }
	    else {
	      if ( val.empty() ) {
		result.push_back( 0 );
	      }
	      else {
		KWargs args;
		args["text"] = val;
		PlaceHolder *p = new PlaceHolder( args );
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
    if ( size > 0 ) {
      vector<Word*> words = mydoc->words();
      for ( size_t i=0; i < words.size(); ++i ) {
	if ( words[i] == this ) {
	  size_t miss = 0;
	  if ( i < size ) {
	    miss = size - i;
	  }
	  for ( size_t index=0; index < miss; ++index ) {
	    if ( val.empty() ) {
	      result.push_back( 0 );
	    }
	    else {
	      KWargs args;
	      args["text"] = val;
	      PlaceHolder *p = new PlaceHolder( args );
	      mydoc->keepForDeletion( p );
	      result.push_back( p );
	    }
	  }
	  for ( size_t index=i-size+miss; index < i; ++index ) {
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
    if ( size > 0 ) {
      vector<Word*> words = mydoc->words();
      size_t begin;
      size_t end;
      for ( size_t i=0; i < words.size(); ++i ) {
	if ( words[i] == this ) {
	  begin = i + 1;
	  end = begin + size;
	  for ( ; begin < end; ++begin ) {
	    if ( begin >= words.size() ) {
	      if ( val.empty() ) {
		result.push_back( 0 );
	      }
	      else {
		KWargs args;
		args["text"] = val;
		PlaceHolder *p = new PlaceHolder( args );
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
    if ( _id == id ) {
      return this;
    }
    return 0;
  }

  ElementType layertypeof( ElementType et ) {
    switch( et ) {
    case Entity_t:
    case EntitiesLayer_t:
      return EntitiesLayer_t;
    case Chunk_t:
    case ChunkingLayer_t:
      return ChunkingLayer_t;
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
    case CoreferenceLayer_t:
      return CoreferenceLayer_t;
    case Observation_t:
    case ObservationLayer_t:
      return ObservationLayer_t;
    // case Predicate_t:
    // case PredicateLayer_t:
    //   return PredicateLayer_t;
    case SentimentLayer_t:
    case Sentiment_t:
      return SentimentLayer_t;
    case StatementLayer_t:
    case Statement_t:
      return SentimentLayer_t;
    case SemanticRolesLayer_t:
    case SemanticRole_t:
      return SemanticRolesLayer_t;
    case DependenciesLayer_t:
    case Dependency_t:
      return DependenciesLayer_t;
    default:
      return BASE;
    }
  }

  vector<AbstractSpanAnnotation*> Word::findspans( ElementType et,
						   const string& st ) const {
    ElementType layertype = layertypeof( et );
    vector<AbstractSpanAnnotation *> result;
    if ( layertype != BASE ) {
      const FoliaElement *e = parent();
      if ( e ) {
	vector<FoliaElement*> v = e->select( layertype, st, false );
	for ( const auto& el : v ){
	  for ( size_t k=0; k < el->size(); ++k ) {
	    FoliaElement *f = el->index(k);
	    AbstractSpanAnnotation *as = dynamic_cast<AbstractSpanAnnotation*>(f);
	    if ( as ) {
	      vector<FoliaElement*> wrefv = f->wrefs();
	      for ( const auto& wr : wrefv ){
		if ( wr == this ) {
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

  FoliaElement* WordReference::parseXml( const xmlNode *node ) {
    KWargs att = getAttributes( node );
    string id = att["id"];
    if ( id.empty() ) {
      throw XmlError( "empty id in WordReference" );
    }
    if ( mydoc->debug ) {
      cerr << "Found word reference" << id << endl;
    }
    FoliaElement *res = (*mydoc)[id];
    if ( res ) {
      // To DO: check type. Word_t, Phoneme_t or Morpheme_t??
      res->increfcount();
    }
    else {
      if ( mydoc->debug ) {
	cerr << "...Unresolvable id: " << id << endl;
      }
      throw XmlError( "Unresolvable id " + id + "in WordReference" );
    }
    delete this;
    return res;
  }

  FoliaElement* AlignReference::parseXml( const xmlNode *node ) {
    KWargs att = getAttributes( node );
    string val = att["id"];
    if ( val.empty() ) {
      throw XmlError( "ID required for AlignReference" );
    }
    refId = val;
    if ( mydoc->debug ) {
      cerr << "Found AlignReference ID " << refId << endl;
    }
    ref_type = att["type"];
    val = att["t"];
    if ( !val.empty() ) {
      _t = val;
    }
    return this;
  }

  FoliaElement *AlignReference::resolve_element( const Alignment *ref ) const {
    if ( ref->href().empty() ) {
      return (*mydoc)[refId];
    }
    throw NotImplementedError( "AlignReference::resolve() for external doc" );
  }

  void Alignment::setAttributes( const KWargs& kwargsin ) {
    KWargs kwargs = kwargsin;
    auto it = kwargs.find( "format" );
    if ( it != kwargs.end() ) {
      _format = it->second;
      kwargs.erase( it );
    }
    FoliaImpl::setAttributes(kwargs);
  }

  KWargs Alignment::collectAttributes() const {
    KWargs atts = FoliaImpl::collectAttributes();
    if ( !_format.empty() && _format != "text/folia+xml" ) {
      atts["format"] = _format;
    }
    return atts;
  }

  vector<FoliaElement *> Alignment::resolve() const {
    vector<FoliaElement*> result;
    vector<AlignReference*> v = FoliaElement::select<AlignReference>();
    for ( const auto& ar : v ){
      result.push_back( ar->resolve_element( this ) );
    }
    return result;
  }

  void PlaceHolder::setAttributes( const KWargs& args ) {
    auto it = args.find( "text" );
    if ( it == args.end() ) {
      throw ValueError("text attribute is required for " + classname() );
    }
    else if ( args.size() != 1 ) {
      throw ValueError("only the text attribute is supported for " + classname() );
    }
    Word::setAttributes( args );
  }

  const UnicodeString Figure::caption() const {
    vector<FoliaElement *> v = select(Caption_t);
    if ( v.empty() ) {
      throw NoSuchText("caption");
    }
    else {
      return v[0]->text();
    }
  }

  void Description::setAttributes( const KWargs& kwargs ) {
    auto it = kwargs.find( "value" );
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

  FoliaElement* Description::parseXml( const xmlNode *node ) {
    KWargs att = getAttributes( node );
    auto it = att.find("value" );
    if ( it == att.end() ) {
      att["value"] = XmlContent( node );
    }
    setAttributes( att );
    return this;
  }

  void Comment::setAttributes( const KWargs& kwargsin ) {
    KWargs kwargs = kwargsin;
    auto it = kwargs.find( "value" );
    if ( it == kwargs.end() ) {
      throw ValueError("value attribute is required for " + classname() );
    }
    _value = it->second;
    kwargs.erase( it );
    FoliaImpl::setAttributes( kwargs );
  }

  xmlNode *Comment::xml( bool, bool ) const {
    xmlNode *e = FoliaImpl::xml( false, false );
    xmlAddChild( e, xmlNewText( (const xmlChar*)_value.c_str()) );
    return e;
  }

  FoliaElement* Comment::parseXml( const xmlNode *node ) {
    KWargs att = getAttributes( node );
    auto it = att.find("value" );
    if ( it == att.end() ) {
      att["value"] = XmlContent( node );
    }
    setAttributes( att );
    return this;
  }

  FoliaElement *AbstractSpanAnnotation::append( FoliaElement *child ) {
    FoliaImpl::append( child );
    if ( child->isinstance(PlaceHolder_t) ) {
      child->increfcount();
    }
    return child;
  }

  void AbstractAnnotationLayer::assignset( FoliaElement *child ) {
    // If there is no set (yet), try to get the set from the child
    // but not if it is the default set.
    // for a Correction child, we look deeper.
    if ( _set.empty() ) {
      if ( child->isSubClass( AbstractSpanAnnotation_t ) ) {
	string st = child->sett();
	if ( !st.empty()
	     && mydoc->defaultset( child->annotation_type() ) != st ) {
	  _set = st;
	  mydoc->incrRef( child->annotation_type(), _set );
	}
      }
      else if ( child->isinstance(Correction_t) ) {
	Original *org = child->getOriginal();
	if ( org ) {
	  for ( size_t i=0; i < org->size(); ++i ) {
	    FoliaElement *el = org->index(i);
	    if ( el->isSubClass( AbstractSpanAnnotation_t ) ) {
	      string st = el->sett();
	      if ( !st.empty()
		   && mydoc->defaultset( el->annotation_type() ) != st ) {
		_set = st;
		mydoc->incrRef( el->annotation_type(), _set );
		return;
	      }
	    }
	  }
	}
	New *nw = child->getNew();
	if ( nw ) {
	  for ( size_t i=0; i < nw->size(); ++i ) {
	    FoliaElement *el = nw->index(i);
	    if ( el->isSubClass( AbstractSpanAnnotation_t ) ) {
	      string st = el->sett();
	      if ( !st.empty()
		   && mydoc->defaultset( el->annotation_type() ) != st ) {
		_set = st;
		mydoc->incrRef( el->annotation_type(), _set );
		return;
	      }
	    }
	  }
	}
	auto v = child->suggestions();
	for ( const auto& el : v ) {
	  if ( el->isSubClass( AbstractSpanAnnotation_t ) ) {
	    string st = el->sett();
	    if ( !st.empty()
		 && mydoc->defaultset( el->annotation_type() ) != st ) {
	      _set = st;
	      mydoc->incrRef( el->annotation_type(), _set );
	      return;
	    }
	  }
	}
      }
    }
  }

  FoliaElement *AbstractAnnotationLayer::append( FoliaElement *child ) {
    FoliaImpl::append( child );
    assignset( child );
    return child;
  }

  KWargs AbstractAnnotationLayer::collectAttributes() const {
    KWargs attribs = FoliaImpl::collectAttributes();
    auto it = attribs.find("set");
    if ( it != attribs.end() ) {
      attribs.erase(it);
    }
    return attribs;
  }

  xmlNode *AbstractSpanAnnotation::xml( bool recursive, bool kanon ) const {
    xmlNode *e = FoliaImpl::xml( false, false );
    // append Word, Phon and Morpheme children as WREFS
    for ( const auto& el : data ) {
      if ( el->element_id() == Word_t ||
	   el->element_id() == Phoneme_t ||
	   el->element_id() == Morpheme_t ) {
	xmlNode *t = XmlNewNode( foliaNs(), "wref" );
	KWargs attribs;
	attribs["id"] = el->id();
	string txt = el->str();
	if ( !txt.empty() ) {
	  attribs["t"] = txt;
	}
	addAttributes( t, attribs );
	xmlAddChild( e, t );
      }
      else {
	string at = tagToAtt( el );
	if ( at.empty() ) {
	  // otherwise handled by FoliaElement::xml() above
	  xmlAddChild( e, el->xml( recursive, kanon ) );
	}
      }
    }
    return e;
  }

  FoliaElement *Quote::append( FoliaElement *child ) {
    FoliaImpl::append( child );
    if ( child->isinstance(Sentence_t) ) {
      child->setAuth( false ); // Sentences under quotes are non-authoritative
    }
    return child;
  }

  xmlNode *Content::xml( bool recurse, bool ) const {
    xmlNode *e = FoliaImpl::xml( recurse, false );
    xmlAddChild( e, xmlNewCDataBlock( 0,
				      (const xmlChar*)value.c_str() ,
				      value.length() ) );
    return e;
  }

  FoliaElement* Content::parseXml( const xmlNode *node ) {
    KWargs att = getAttributes( node );
    setAttributes( att );
    xmlNode *p = node->children;
    bool isCdata = false;
    bool isText = false;
    while ( p ) {
      if ( p->type == XML_CDATA_SECTION_NODE ) {
	if ( isText ) {
	  throw XmlError( "intermixing text and CDATA in Content node" );
	}
	isCdata = true;
	value += (char*)p->content;
      }
      else if ( p->type == XML_TEXT_NODE ) {
	if ( isCdata ) {
	  throw XmlError( "intermixing text and CDATA in Content node" );
	}
	isText = true;
	value += (char*)p->content;
      }
      else if ( p->type == XML_COMMENT_NODE ) {
	string tag = "_XmlComment";
	FoliaElement *t = createElement( tag, mydoc );
	if ( t ) {
	  t = t->parseXml( p );
	  append( t );
	}
      }
      p = p->next;
    }
    if ( value.empty() ) {
      throw XmlError( "CDATA or Text expected in Content node" );
    }
    return this;
  }

  const UnicodeString Correction::text( const string& cls,
					bool retaintok,
					bool ) const {
#ifdef DEBUG_TEXT
    cerr << "TEXT(" << cls << ") op node : " << xmltag() << " id ( " << id() << ")" << endl;
#endif
    // we cannot use textcontent() on New, Origibal or Current,
    // because texcontent doesn't recurse!
    for ( const auto& el : data ) {
#ifdef DEBUG_TEXT
      cerr << "data=" << el << endl;
#endif
       if ( el->isinstance( New_t ) || el->isinstance( Current_t ) ) {
         return el->text( cls, retaintok );
       }
    }
    throw NoSuchText( "cls=" + cls );
  }

  const string& Correction::getTextDelimiter( bool retaintok ) const {
    for ( const auto& el : data ) {
      if ( el->isinstance( New_t ) || el->isinstance( Current_t ) ) {
	return el->getTextDelimiter( retaintok );
      }
    }
    return EMPTY_STRING;
  }

  const TextContent *Correction::textcontent( const string& cls ) const {
    // TODO: this implements correctionhandling::EITHER only
    for ( const auto& el : data ) {
      if ( el->isinstance( New_t ) || el->isinstance( Current_t ) ) {
	const TextContent *res = 0;
	try {
	  res = el->textcontent( cls );
	  return res;
	}
	catch (...){
	}
      }
    }
    for ( const auto& el : data ) {
      if ( el->isinstance( Original_t ) ) {
	const TextContent *res = 0;
	try {
	  res =  el->textcontent( cls );
	  return res;
	}
	catch ( ... ){
	}
      }
      else if ( cls == "current" && el->hastext( "original" ) ){
	cerr << "text(original)= " << el->textcontent( cls )->text()<< endl;
	// hack for old and erroneous behaviour
	return el->textcontent( "original" );
      }
    }
    throw NoSuchText("wrong cls");
  }

  const PhonContent *Correction::phoncontent( const string& cls ) const {
    // TODO: this implements correctionhandling::EITHER only
    for ( const auto& el: data ) {
      if ( el->isinstance( New_t ) || el->isinstance( Current_t ) ) {
	return el->phoncontent( cls );
      }
    }
    for ( const auto& el: data ) {
      if ( el->isinstance( Original_t ) ) {
	return el->phoncontent( cls );
      }
    }
    throw NoSuchPhon("wrong cls");
  }

  bool Correction::hasNew( ) const {
    vector<FoliaElement*> v = select( New_t, false );
    return !v.empty();
  }

  New *Correction::getNew() const {
    vector<New*> v = FoliaElement::select<New>( false );
    if ( v.empty() ) {
      throw NoSuchAnnotation( "new" );
    }
    return v[0];
  }

  FoliaElement *Correction::getNew( size_t index ) const {
    New *n = getNew();
    return n->index(index);
  }

  bool Correction::hasOriginal() const {
    vector<FoliaElement*> v = select( Original_t, false );
    return !v.empty();
  }

  Original *Correction::getOriginal() const {
    vector<Original*> v = FoliaElement::select<Original>( false );
    if ( v.empty() ) {
      throw NoSuchAnnotation( "original" );
    }
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
    if ( v.empty() ) {
      throw NoSuchAnnotation( "current" );
    }
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
    if ( v.empty() || index >= v.size() ) {
      throw NoSuchAnnotation( "suggestion" );
    }
    return v[index];
  }

  Head *Division::head() const {
    if ( data.size() > 0 ||
	 data[0]->element_id() == Head_t ) {
      return dynamic_cast<Head*>(data[0]);
    }
    throw runtime_error( "No head" );
  }

  const string Gap::content() const {
    vector<FoliaElement*> cv = select( Content_t );
    if ( cv.empty() ) {
      throw NoSuchAnnotation( "content" );
    }
    return cv[0]->content();
  }

  Headspan *Dependency::head() const {
    vector<Headspan*> v = FoliaElement::select<Headspan>();
    if ( v.size() < 1 ) {
      throw NoSuchAnnotation( "head" );
    }
    return v[0];
  }

  DependencyDependent *Dependency::dependent() const {
    vector<DependencyDependent *> v = FoliaElement::select<DependencyDependent>();
    if ( v.empty() ) {
      throw NoSuchAnnotation( "dependent" );
    }
    return v[0];
  }

  vector<AbstractSpanAnnotation*> FoliaImpl::selectSpan() const {
    vector<AbstractSpanAnnotation*> res;
    for ( const auto& el : SpanSet ) {
      vector<FoliaElement*> tmp = select( el, true );
      for ( auto& sp : tmp ) {
	res.push_back( dynamic_cast<AbstractSpanAnnotation*>( sp ) );
      }
    }
    return res;
  }

  vector<FoliaElement*> AbstractSpanAnnotation::wrefs() const {
    vector<FoliaElement*> res;
    for ( const auto& el : data ) {
      ElementType et = el->element_id();
      if ( et == Word_t
	   || et == WordReference_t
	   || et == Phoneme_t
	   || et == MorphologyLayer_t ) {
	res.push_back( el );
      }
      else {
	AbstractSpanAnnotation *as = dynamic_cast<AbstractSpanAnnotation*>(el);
	if ( as != 0 ) {
	  vector<FoliaElement*> sub = as->wrefs();
	  for ( auto& wr : sub ) {
	    res.push_back( wr );
	  }
	}
      }
    }
    return res;
  }

  FoliaElement *AbstractSpanAnnotation::wrefs( size_t pos ) const {
    vector<FoliaElement*> v = wrefs();
    if ( pos < v.size() ) {
      return v[pos];
    }
    return 0;
  }

  AbstractSpanAnnotation *AbstractAnnotationLayer::findspan( const vector<FoliaElement*>& words ) const {
    vector<AbstractSpanAnnotation*> av = selectSpan();
    for ( const auto& span : av ){
      vector<FoliaElement*> v = span->wrefs();
      if ( v.size() == words.size() ) {
	bool ok = true;
	for ( size_t n = 0; n < v.size(); ++n ) {
	  if ( v[n] != words[n] ) {
	    ok = false;
	    break;
	  }
	}
	if ( ok ) {
	  return span;
	}
      }
    }
    return 0;
  }

  bool XmlText::setvalue( const std::string& s ){
    _value = s;
    return true;
  }

  const UnicodeString XmlText::text( const string&, bool, bool ) const {
    return UTF8ToUnicode(_value);
  }

  xmlNode *XmlText::xml( bool, bool ) const {
    return xmlNewText( (const xmlChar*)_value.c_str() );
  }

  FoliaElement* XmlText::parseXml( const xmlNode *node ) {
    string tmp;
    if ( node->content ) {
      _value = (const char*)node->content;
      tmp = trim( _value );
    }
    if ( tmp.empty() ) {
      throw ValueError( "TextContent may not be empty" );
    }
    return this;
  }

  static int error_sink(void *mydata, xmlError *error ) {
    int *cnt = (int*)mydata;
    if ( *cnt == 0 ) {
      cerr << "\nXML-error: " << error->message << endl;
    }
    (*cnt)++;
    return 1;
  }

  void External::resolve_external( ) {
    try {
      cerr << "try to resolve: " << _src << endl;
      int cnt = 0;
      xmlSetStructuredErrorFunc( &cnt, (xmlStructuredErrorFunc)error_sink );
      xmlDoc *extdoc = xmlReadFile( _src.c_str(), 0, XML_PARSE_NOBLANKS|XML_PARSE_HUGE );
      if ( extdoc ) {
	xmlNode *root = xmlDocGetRootElement( extdoc );
	xmlNode *p = root->children;
	while ( p ) {
	  if ( p->type == XML_ELEMENT_NODE ) {
	    string tag = Name( p );
	    if ( tag == "text" ) {
	      FoliaElement *parent = _parent;
	      KWargs args = parent->collectAttributes();
	      args["_id"] = "Arglebargleglop-glyf";
	      Text *tmp = new Text( args, mydoc );
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
	xmlFreeDoc( extdoc );
      }
      else {
	throw XmlError( "resolving external " + _src + " failed" );
      }
    }
    catch ( const exception& e ) {
      throw XmlError( "resolving external " + _src + " failed: "
		      + e.what() );
    }
  }

  FoliaElement* External::parseXml( const xmlNode *node ) {
    KWargs att = getAttributes( node );
    setAttributes( att );
    if ( _include ) {
      mydoc->addExternal( this );
    }
    return this;
  }

  KWargs External::collectAttributes() const {
    KWargs atts = FoliaImpl::collectAttributes();
    if ( _include ) {
      atts["include"] = "yes";
    }
    return atts;
  }

  void External::setAttributes( const KWargs& kwargsin ) {
    KWargs kwargs = kwargsin;
    auto it = kwargs.find( "include" );
    if ( it != kwargs.end() ) {
      _include = TiCC::stringTo<bool>( it->second );
      kwargs.erase( it );
    }
    FoliaImpl::setAttributes(kwargs);
  }

  void Note::setAttributes( const KWargs& args ) {
    auto it = args.find( "_id" );
    if ( it != args.end() ) {
      refId = it->second;
    }
    FoliaImpl::setAttributes( args );
  }

  KWargs Reference::collectAttributes() const {
    KWargs atts = FoliaImpl::collectAttributes();
    if ( !_id.empty() ){
      atts["_id"] = _id;
    }
    atts["id"] = refId;
    atts["type"] = ref_type;
    if ( !_format.empty() && _format != "text/folia+xml" ) {
      atts["format"] = _format;
    }
    return atts;
  }

  void Reference::setAttributes( const KWargs& argsin ) {
    KWargs args = argsin;
    auto it = args.find( "_id" );
    if ( it != args.end() ) {
      _id = it->second;
      args.erase( it );
    }
    it = args.find( "id" );
    if ( it != args.end() ) {
      refId = it->second;
      args.erase( it );
    }
    it = args.find( "type" );
    if ( it != args.end() ) {
      ref_type = it->second;
      args.erase( it );
    }
    it = args.find( "format" );
    if ( it != args.end() ) {
      _format = it->second;
      args.erase( it );
    }
    FoliaImpl::setAttributes(args);
  }

  xmlNode *XmlComment::xml( bool, bool ) const {
    return xmlNewComment( (const xmlChar*)_value.c_str() );
  }

  FoliaElement* XmlComment::parseXml( const xmlNode *node ) {
    if ( node->content ) {
      _value = (const char*)node->content;
    }
    return this;
  }

  KWargs Suggestion::collectAttributes() const {
    KWargs atts = FoliaImpl::collectAttributes();
    if ( !_split.empty() ) {
      atts["split"] = _split;
    }
    if ( !_merge.empty() ) {
      atts["merge"] = _merge;
    }
    return atts;
  }

  void Suggestion::setAttributes( const KWargs& kwargsin ) {
    KWargs kwargs = kwargsin;
    auto it = kwargs.find( "split" );
    if ( it != kwargs.end() ) {
      _split = it->second;
      kwargs.erase( it );
    }
    it = kwargs.find( "merge" );
    if ( it != kwargs.end() ) {
      _merge = it->second;
      kwargs.erase( it );
    }
    FoliaImpl::setAttributes(kwargs);
  }


  void Feature::setAttributes( const KWargs& kwargs ) {
    //
    // Feature is special. So DON'T call ::setAttributes
    //
    auto it = kwargs.find( "subset" );
    if ( it == kwargs.end() ) {
      _subset = default_subset();
      if ( _subset.empty() ){
	throw ValueError("subset attribute is required for " + classname() );
      }
    }
    else {
      if ( it->second.empty() ) {
	throw ValueError("subset attribute may never be empty: " + classname() );
      }
      _subset = it->second;
    }
    it = kwargs.find( "class" );
    if ( it == kwargs.end() ) {
      throw ValueError("class attribute is required for " + classname() );
    }
    if ( it->second.empty() ) {
      throw ValueError("class attribute may never be empty: " + classname() );
    }
    _class = it->second;
  }

  KWargs Feature::collectAttributes() const {
    KWargs attribs = FoliaImpl::collectAttributes();
    attribs["subset"] = _subset;
    return attribs;
  }

  vector<string> FoliaImpl::feats( const string& s ) const {
    //    return all classes of the given subset
    vector<string> result;
    for ( const auto& el : data ) {
      if ( el->isSubClass( Feature_t ) &&
	   el->subset() == s ) {
	result.push_back( el->cls() );
      }
    }
    return result;
  }

  const string FoliaImpl::feat( const string& s ) const {
    //    return the fist class of the given subset
    for ( const auto& el : data ) {
      if ( el->isSubClass( Feature_t ) &&
	   el->subset() == s ) {
	return el->cls();
      }
    }
    return "";
  }

  ForeignData::~ForeignData(){
    xmlFreeNode( _foreign_data );
  }

  FoliaElement* ForeignData::parseXml( const xmlNode *node ){
    set_data( node );
    return this;
  }

  xmlNode *ForeignData::xml( bool, bool ) const {
    return get_data();
  }

  void ForeignData::set_data( const xmlNode *node ){
    xmlNode *p = (xmlNode *)node->children;
    while ( p ){
      string pref;
      string ns = getNS( p, pref );
      if ( ns == NSFOLIA ){
	throw XmlError( "ForeignData MAY NOT be in the FoLiA namespace" );
      }
      p = p->next;
    }
    _foreign_data = xmlCopyNode( (xmlNode*)node, 1 );
  }

  void clean_ns( xmlNode *node, const string& ns ){
    xmlNs *p = node->nsDef;
    xmlNs *prev = 0;
    while ( p ){
      string val = (char *)p->href;
      if ( val == ns ){
	if ( prev ){
	  prev->next = p->next;
	}
	else {
	  node->nsDef = p->next;
	}
	return;
      }
      prev = p;
      p = p->next;
    }
  }

  xmlNode* ForeignData::get_data() const {
    xmlNode *result = xmlCopyNode(_foreign_data, 1 );
    clean_ns( result, NSFOLIA ); // HACK: remove FoLiA namespace def
    return result;
  }

  KWargs AbstractTextMarkup::collectAttributes() const {
    KWargs attribs = FoliaImpl::collectAttributes();
    if ( !idref.empty() ) {
      attribs["id"] = idref;
    }
    return attribs;
  }

  void AbstractTextMarkup::setAttributes( const KWargs& atts ) {
    KWargs args = atts;
    auto it = args.find( "id" );
    if ( it != args.end() ) {
      auto it2 = args.find( "_id" );
      if ( it2 != args.end() ) {
	throw ValueError("Both 'id' and 'xml:id found for " + classname() );
      }
      idref = it->second;
      args.erase( it );
    }
    it = args.find( "text" );
    if ( it != args.end() ) {
      XmlText *txt = new XmlText();
      txt->setvalue( it->second );
      append(txt);
      args.erase( it );
    }
    FoliaImpl::setAttributes( args );
  }

  KWargs TextMarkupCorrection::collectAttributes() const {
    KWargs attribs = AbstractTextMarkup::collectAttributes();
    if ( !_original.empty() ) {
      attribs["original"] = _original;
    }
    return attribs;
  }

  void TextMarkupCorrection::setAttributes( const KWargs& args ) {
    KWargs argl = args;
    auto it = argl.find( "id" );
    if ( it != argl.end() ) {
      idref = it->second;
      argl.erase( it );
    }
    it = argl.find( "original" );
    if ( it != argl.end() ) {
      _original = it->second;
      argl.erase( it );
    }
    FoliaImpl::setAttributes( argl );
  }

  const UnicodeString TextMarkupCorrection::text( const string& cls,
						  bool ret,
						  bool strict ) const{
    if ( cls == "original" ) {
      return UTF8ToUnicode(_original);
    }
    return FoliaImpl::text( cls, ret, strict );
  }

  const FoliaElement* AbstractTextMarkup::resolveid() const {
    if ( idref.empty() || !mydoc ) {
      return this;
    }
    else {
      return mydoc->index(idref);
    }
  }

  void TextContent::init() {
    _offset = -1;
  }

  void PhonContent::init() {
    _offset = -1;
  }

  void Word::init() {
    _space = true;
  }

  void Linebreak::init() {
    _newpage = false;
  }

  void Alignment::init() {
    _format = "text/folia+xml";
  }

  void Reference::init() {
    _format = "text/folia+xml";
  }

  void ForeignData::init() {
    _foreign_data = 0;
  }

} // namespace folia
