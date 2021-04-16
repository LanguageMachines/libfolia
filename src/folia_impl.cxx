/*
  Copyright (c) 2006 - 2021
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
#include "ticcutils/Unicode.h"
#include "libfolia/folia.h"
#include "libfolia/folia_properties.h"
#include "config.h"

using namespace std;
using namespace icu;
using namespace TiCC;

namespace folia {
  using TiCC::operator <<;

  string VersionName() { return PACKAGE_STRING; } ///< Returns the PACKAGE_STRING info of the package
  string Version() { return VERSION; }  ///< Returns version of the library

  TextPolicy::TextPolicy( const string& cl ):
    _class("current"),
    _text_flags(TEXT_FLAGS::NONE)
  {
    if ( !cl.empty() ){
      _class = cl;
    }
  }

  TextPolicy::TextPolicy( const std::string& cls, const TEXT_FLAGS flags ):
    _class(cls),
    _text_flags( flags )
  {
  }

  ostream& operator<<( ostream& os, const TextPolicy& tp ){
    bool retain  = tp.is_set( TEXT_FLAGS::RETAIN );
    bool strict  = tp.is_set( TEXT_FLAGS::STRICT );
    bool hide    = tp.is_set( TEXT_FLAGS::HIDDEN );
    bool trim = !tp.is_set( TEXT_FLAGS::NO_TRIM_SPACES );
    os << (strict?"strict":"not strict") << "\t"
       << (retain?"retain":"untokenized") << "\t"
       << (hide?"show_hidden":"hide hidden") << "\t"
       << (trim?"trimming spaces":"not trimming spaces");
    return os;
  }

  bool TextPolicy::is_set( TEXT_FLAGS tf ) const {
    return ( tf & _text_flags ) == tf;
  }

  void TextPolicy::set( TEXT_FLAGS tf ) {
    _text_flags |= tf;
  }

  void TextPolicy::add_handler( const std::string& label,
				const stringFunctionPointer& sfp ){
    _tag_handlers.insert( make_pair( label, sfp ) );
  }

  ElementType AbstractElement::element_id() const {
    /// return the ELEMENT_ID property
    return _props.ELEMENT_ID;
  }

  size_t AbstractElement::occurrences() const {
    /// return the OCCURENCES property
    return _props.OCCURRENCES;
  }

  size_t AbstractElement::occurrences_per_set() const {
    /// return the OCCURRENCES_PER_SET property
    return _props.OCCURRENCES_PER_SET;
  }

  Attrib AbstractElement::required_attributes() const {
    /// return the REQUIRED_ATTRIBUTES property
    return _props.REQUIRED_ATTRIBS;
  }

  Attrib AbstractElement::optional_attributes() const {
    /// return the OPTONAL_ATTRIBUTES property
    return _props.OPTIONAL_ATTRIBS;
  }

  bool AbstractElement::hidden() const {
    /// return the HIDDEN property
    return _props.HIDDEN;
  }

  const string& AbstractElement::xmltag() const {
    /// return the XMLTAG property
    /*
      For pre 1.5 documents, it will return the OLD name of that property.
      e.g. "spanrelation" is translated to the old "complexalignment"
    */
    const string& result = _props.XMLTAG;
    if ( doc() && doc()->version_below(1,6) ){
      const auto& it = reverse_old.find(result);
      if ( it != reverse_old.end() ){
	return it->second;
      }
    }
    return result;
  }

  const string& AbstractElement::default_subset() const {
    /// return the SUBSET property
    return _props.SUBSET;
  }

  AnnotationType AbstractElement::annotation_type() const {
    /// return the ANNOTATIONTYPE property
    return _props.ANNOTATIONTYPE;
  }

  const set<ElementType>& AbstractElement::accepted_data() const {
    /// return the ACCEPTED_DATA property
    return _props.ACCEPTED_DATA;
  }

  const set<ElementType>& AbstractElement::required_data() const {
    /// return the REQUIRED_DATA property
    return _props.REQUIRED_DATA;
  }

  bool AbstractElement::printable() const {
    /// return the PRINTABLE property
    return _props.PRINTABLE;
  }

  bool AbstractElement::speakable() const {
    /// return the SPEAKABLE property
    return _props.SPEAKABLE;
  }

  bool AbstractElement::referable() const {
    /// return the WREFABLE property
    return _props.WREFABLE;
  }

  bool AbstractElement::is_textcontainer() const {
    /// return the TEXTCONTAINER property
    return _props.TEXTCONTAINER;
  }

  bool AbstractElement::is_phoncontainer() const {
    /// return the PHONCONTAINER property
    return _props.PHONCONTAINER;
  }

  bool AbstractElement::xlink() const {
    /// return the XLINK property
    return _props.XLINK;
  }

  bool AbstractElement::auth() const {
    /// return the AUTH property
    return _props.AUTH;
  }

  bool AbstractElement::setonly() const {
    /// return the SETONLY property
    return _props.SETONLY;
  }

  bool AbstractElement::auto_generate_id() const {
    /// return the AUTO_GENERATE_ID property
    return _props.AUTO_GENERATE_ID;
  }

  bool is_structure( const FoliaElement *el ){
    /// test if the object is a Structure Element.
    /*!
      \param el the FoliaElement to test
      \return true when the parameter is an AbstractStructureElement
      or a derivative of an AbstractStructureElement
    */
    return dynamic_cast<const AbstractStructureElement*>( el ) != 0;
  }

  const string AllowXlink::href() const {
    /// return the 'href' value of the object
    /*!
     * if the object has an xlink value for 'href' it is returned as a string
     * otherwise the result is ""
     */
    auto it = _xlink.find("href");
    if ( it != _xlink.end() ){
      return it->second;
    }
    return "";
  }

  const string AbstractElement::settag( const string& t ) {
    /// set a value for the _tags attribute
    /*!
     * \param t the new value (may be empty)
     * \return the old value (can be empty)
     * thows when the FoliaElement doesn't support the tag attribute
     */
    Attrib supported = required_attributes() | optional_attributes();
    if ( !(TAG & supported) ) {
      throw ValueError( "settag is not supported for " + classname() );
    }
    string r = _tags;
    _tags = t;
    return r;
  }

  ostream& operator<<( ostream& os, const FoliaElement& ae ) {
    /// Output operator for FoliaElements. (for DEBUGGING only)
    /*!
     * \param os the output stream
     * \param ae the FoliaElement
     */
    os << " <" << ae.classname();
    KWargs ats = ae.collectAttributes();
    if ( !ae.id().empty() ) {
      os << " xml:id='" << ae.id() << '"';
      ats.erase("xml:id");
    }

    for ( const auto& it: ats ) {
      os << " " << it.first << "='" << it.second << "'";
    }
    os << " > {";
    for ( size_t i=0; i < ae.size(); ++i ) {
      os << "<" << ae.index(i)->classname() << ">,";
    }
    os << "}";
    if ( ae.printable() && ae.classname()[0] == 't' ){
      os << " \"" << ae.str(ae.textclass()) << "\" (" << ae.textclass() << ")";
    }
    return os;
  }

  ostream& operator<<( ostream&os, const FoliaElement *ae ) {
    /// Output operator for FoliaElements. (for DEBUGGING only)
    /*!
     * \param os the output stream
     * \param ae the FoliaElement
     */
    if ( !ae ) {
      os << "nil";
    }
    else
      os << *ae;
    return os;
  }

  AbstractElement::AbstractElement( const properties& p, Document *d ) :
    /// Constructor for AbstractElements.
    /*!
     * \param p a properties block (required)
     * \param d a parent document
     */
    _mydoc(d),
    _parent(0),
    _auth( p.AUTH ),
    _space(true),
    _annotator_type(UNDEFINED),
    _refcount(0),
    _confidence(-1),
    _preserve_spaces(SPACE_FLAGS::UNSET),
    _props(p)
  {
  }

  AbstractElement::~AbstractElement( ) {
    /// Destructor for AbstractElements.
    bool debug = false;
    // if ( xmltag() == "w"
    // 	 || xmltag() == "s"
    // 	 || xmltag() == "entity"
    // 	 || xmltag() == "entities"
    // 	 || xmltag() == "morpheme"
    // 	 || xmltag() == "morphology" ){
    //   debug = false;
    // }
    if (debug ){
      cerr << "delete " << xmltag() << " id=" << _id << " class= "
    	   << cls() << " datasize= " << _data.size() << endl;
      cerr << "REFCOUNT = " << refcount() << endl;
    }
    if ( refcount() > 0 ){
      if ( doc() ) {
	doc()->keepForDeletion( this );
      }
      //      decrefcount();
    }
    else {
      for ( const auto& el : _data ) {
	if ( el->refcount() == 0 ) {
	  if ( debug ){
	    cerr << "dus delete: " << el << endl;
	  }
	  // probably only != 0 for words
	  delete el;
	}
	else if ( doc() ) {
	  if ( debug ){
	    cerr << "dus KEEP: " << el << endl;
	  }
	  doc()->keepForDeletion( el );
	}
      }
    }
    if ( debug ){
      cerr << "\t\tsucces deleting element id=" << _id << " tag = " << xmltag() << " class= "
     	   << cls() << " datasize= " << _data.size() << endl;
    }
    if ( doc() ) {
      doc()->del_doc_index( _id );
      doc()->decrRef( annotation_type(), _set );
    }
  }

  xmlNs *AbstractElement::foliaNs() const {
    /// return the associated xmlNs object.
    /*!
     * \return the XML namespace element of the associated FoLiA document
     * or 0 when no xml document is available
     */
    if ( doc() ) {
      return doc()->foliaNs();
    }
    return 0;
  }

  void AbstractElement::check_declaration(){
    /// check the declation consistency of an object.
    /// throws an exception on error
    /*!
     * When the object has an associated document, the declaration of the
     * 'set' attribute is checked. Or the default set when no 'set' is provided
     * Also the presence of an appropiate annotation declaration is checked
     * for the annotation-type of the object. This might auto-declare
     * the anntotation-type, when de document allows this.
     */

    if ( _mydoc ){
      string def;
      if ( !_set.empty() ){
	if ( !doc()->declared( annotation_type(), _set ) ) {
	  throw DeclarationError( "Set '" + _set
				  + "' is used but has no declaration " +
				  "for " + toString( annotation_type() )
				  + "-annotation" );
	}
      }
      else {
	if ( _mydoc->debug > 2 ) {
	  cerr << "get def for " <<  annotation_type() << endl;
	}
	def = doc()->default_set( annotation_type() );
	if ( doc()->debug > 2 ) {
	  cerr << "got def='" <<  def << "'" << endl;
	}
	if ( doc()->is_incremental() && def.empty() ){
	  // when there is NO default set, AND we are parsing using
	  // folia::Engine, we must check if there WAS an empty set originally
	  // which is 'obscured' by newly added declarations
	  def = doc()->original_default_set( annotation_type() );
	  if ( doc()->debug > 2 ) {
	    cerr << "from original got def='" <<  def << "'" << endl;
	  }
	}
	if ( !def.empty() ){
	  _set = def;
	}
	else if ( CLASS & required_attributes() ){
	  throw XmlError( "unable to assign a default set for tag: " + xmltag() );
	}
      }
      if ( annotation_type() != AnnotationType::NO_ANN
	   && !_mydoc->version_below( 2, 0 ) ){
	if ( !_mydoc->declared( annotation_type() ) ){
	  if ( _mydoc->autodeclare() ){
	    _mydoc->auto_declare( annotation_type(), _set );
	  }
	  else {
	    throw DeclarationError( "Encountered an instance of <"
				    + xmltag()
				    + "> without a proper "
				    + toString(annotation_type())
				    + "-annotation" );
	  }
	}
	else if ( _set.empty()
		  && !isSubClass( AbstractAnnotationLayer_t )
		  && !doc()->declared( annotation_type(), "None" ) ){
	  if ( _mydoc->autodeclare() ){
	    _mydoc->auto_declare( annotation_type(), _set );
	  }
	  else {
	    throw DeclarationError( "Encountered an instance of <"
				    + xmltag()
				    + "> without a proper "
				    + toString(annotation_type())
				    + "-annotation" );
	  }
	}
      }
    }
  }


  void AllowXlink::setAttributes( KWargs& kwargs ) {
    /// set the objects attributes given a set of Key-Value pairs.
    /*!
     * \param kwargs a KWargs set of Key-Value pairs
     * the given keys are checked agains a range of criteria:
     *     - if the object supports the attribue
     *     - if the object provided value is valid
     *     - if the attribute is declared for the annotation-type
     */
    string type = "simple";
    string val = kwargs.extract( "xlink:type" );
    if ( !val.empty() ) {
      type = val;
    }
    if ( type != "simple" && type != "locator" ) {
      throw XmlError( "only xlink:types: 'simple' and 'locator' are supported!" );
    }
    _xlink["type"] = type;
    val = kwargs.extract( "xlink:href" );
    if ( !val.empty() ) {
      _xlink["href"] = val;
    }
    else if ( type == "locator" ){
      throw XmlError( "xlink:type='locator' requires an 'xlink:href' attribute" );
    }
    val = kwargs.extract( "xlink:role" );
    if ( !val.empty() ) {
      _xlink["role"] = val;
    }
    val = kwargs.extract( "xlink:title" );
    if ( !val.empty() ) {
      _xlink["title"] = val;
    }
    val = kwargs.extract( "xlink:label" );
    if ( !val.empty() ) {
      if ( type == "simple" ){
	throw XmlError( "xlink:type='simple' may not have an 'xlink:label' attribute" );
      }
      _xlink["label"] = val;
    }
    val = kwargs.extract( "xlink:arcrole" );
    if ( !val.empty() ) {
      if ( type == "locator" ){
	throw XmlError( "xlink:type='locator' may not have an 'xlink:arcrole' attribute" );
      }
      _xlink["arcrole"] = val;
    }
    val = kwargs.extract( "xlink:show" );
    if ( !val.empty() ) {
      if ( type == "locator" ){
	throw XmlError( "xlink:type='locator' may not have an 'xlink:show' attribute" );
      }
      _xlink["show"] = val;
    }
    val = kwargs.extract( "xlink:actuate" );
    if ( !val.empty() ) {
      if ( type == "locator" ){
	throw XmlError( "xlink:type='locator' may not have an 'xlink:actuate' attribute" );
      }
      _xlink["actuate"] = val;
    }
  }

  void AbstractElement::setAttributes( KWargs& kwargs ) {
    /// set the objects attributes given a set of Key-Value pairs.
    /*!
     * \param kwargs a KWargs set of Key-Value pairs
     * the given keys are checked agains a range of criteria:
     *     - if the object supports the attribue
     *     - if the object provided value is valid
     *     - if the attribute is declared for the annotation-type
     */
    // for the moment, always look for the 'xml:space' attribute
    string sval = kwargs.extract( "xml:space" );
    if ( !sval.empty() ){
      if ( sval == "preserve" ){
	_preserve_spaces = SPACE_FLAGS::PRESERVE;
      }
      else if ( sval == "default" ){
	_preserve_spaces = SPACE_FLAGS::DEFAULT;
      }
      else {
	throw runtime_error( "invalid value for attribute xml:space, must be "
			     "'default' or 'preserve', found: '" + sval + "'");
      }
    }
    Attrib supported = required_attributes() | optional_attributes();
    //#define LOG_SET_ATT
#ifdef LOG_SET_ATT
    int db_level = doc()->debug;
    if ( element_id() == Paragraph_t ) {
      doc()->setdebug(8);
      cerr << "set attributes: " << kwargs << " on " << classname() << endl;
      //      cerr << "required = " <<  toString(required_attributes()) << endl;
      //      cerr << "optional = " <<  optional_attributes() << endl;
      //cerr << "supported = " << supported << endl;
      //      cerr << "ID & supported = " << (ID & supported) << endl;
      //      cerr << "ID & _required = " << (ID & required_attributes() ) << endl;
      // cerr << "_id=" << _id << endl;
      // cerr << "AUTH : " << _auth << endl;
    }
#endif
    if ( doc() && doc()->debug > 2 ) {
      cerr << "set attributes: " << kwargs << " on " << classname() << endl;
    }

    string val = kwargs.extract( "generate_id" );
    if ( !val.empty() ) {
      if ( !doc() ) {
	throw runtime_error( "can't generate an ID without a doc" );
      }
      FoliaElement *e = (*doc())[val];
      if ( e ) {
	_id = e->generateId( xmltag() );
      }
      else {
	throw ValueError("Unable to generate an id from ID= " + val );
      }
    }
    else {
      val = kwargs.extract( "xml:id" );
      if ( val.empty() ) {
	val = kwargs.extract( "_id" ); // for backward compatibility
      }
      if ( !val.empty() ) {
	if ( (!ID) & supported ) {
	  throw ValueError("xml:id is not supported for " + classname() );
	}
	else if ( isNCName( val ) ){
	  _id = val;
	}
	else {
	  throw XmlError( "'" + val + "' is not a valid NCName." );
	}
      }
    }

    _set.clear();
    val = kwargs.extract( "set" );
    if ( !val.empty() ) {
      if ( !doc() ) {
	throw ValueError( "attribute set=" + val + " is used on a node without a document." );
      }
      if ( !( (CLASS & supported) || setonly() ) ) {
	throw ValueError("attribute 'set' is not supported for " + classname());
      }
      else {
	string st = doc()->unalias( annotation_type(), val );
	if ( st.empty() ){
	  _set = val;
	}
	else {
	  _set = st;
	}
      }
    }

    check_declaration();

    _class.clear();
    val = kwargs.extract( "class" );
    if ( !val.empty() ) {
      if ( !( CLASS & supported ) ) {
	throw ValueError("Class is not supported for " + classname() );
      }
      if ( element_id() != TextContent_t && element_id() != PhonContent_t ) {
	if ( !doc() ) {
	  throw ValueError( "Class=" + val + " is used on a node without a document." );
	}
	if ( _set.empty() ){
	  if ( !doc()->declared( annotation_type(), "None" ) ) {
	    cerr << endl << doc()->annotationdefaults() << endl << endl;
	    throw ValueError( xmltag() +": An empty set is used but that has no declaration "
			      "for " + toString( annotation_type() )
			      + "-annotation" );
	  }
	  _set = "None";
	}
	doc()->incrRef( annotation_type(), _set );
      }
      _class = val;
    }

    if ( element_id() != TextContent_t && element_id() != PhonContent_t ) {
      if ( !_class.empty() && _set.empty() ) {
	throw ValueError("Set is required for <" + classname() +
			 " class=\"" + _class + "\"> assigned without set."  );
      }
    }

    _annotator.clear();
    val = kwargs.extract( "annotator" );
    if ( !val.empty() ) {
      if ( !(ANNOTATOR & supported) ) {
	throw ValueError("attribute 'annotator' is not supported for " + classname() );
      }
      else {
	_annotator = val;
      }
    }
    else {
      string def;
      if ( doc() &&
	   (def = doc()->default_annotator( annotation_type(), _set )) != "" ) {
	_annotator = def;
      }
    }

    _annotator_type = UNDEFINED;
    val = kwargs.extract( "annotatortype" );
    if ( !val.empty() ) {
      if ( ! (ANNOTATOR & supported) ) {
	throw ValueError("Annotatortype is not supported for " + classname() );
      }
      else {
	_annotator_type = stringTo<AnnotatorType>( val );
	if ( _annotator_type == UNDEFINED ) {
	  throw ValueError( "annotatortype must be 'auto' or 'manual', got '"
			    + val + "'" );
	}
      }
    }
    else {
      if ( doc() ){
	AnnotatorType def = doc()->default_annotatortype( annotation_type(), _set );
	if ( def != UNDEFINED ) {
	  _annotator_type = def;
	}
      }
    }

    val = kwargs.extract( "processor" );
    if ( !val.empty() ){
      if ( doc() && doc()->debug > 2 ) {
	cerr << "set processor= " << val << " on " << classname() << endl;
      }
      if ( _set.empty() ){
	_set = "None";
      }
      if ( !(ANNOTATOR & supported) ){
	throw ValueError("attribute 'processor' is not supported for " + classname() );
      }
      else {
	if ( doc() && doc()->get_processor(val) == 0 ){
	  throw ValueError("attribute 'processor' has unknown value: " + val );
	}
	if ( doc()
	     && !doc()->declared( annotation_type(), _set, "", _annotator_type, val ) ){
	  if (	!doc()->version_below( 2, 0 )
		&& doc()->autodeclare() ) {
	    KWargs args;
	    args["processor"] = val;
	    args["annotatortype"] = _annotator_type;
	    doc()->declare( annotation_type(), _set, args );
	  }
	  else {
	    throw DeclarationError( "Processor '" + val
				    + "' is used for annotationtype '"
				    + toString( annotation_type() )
				    + "' with set='" + _set +"'"
				    + " but there is no corresponding <annotator>"
				    + " referring to it in the annotation"
				    + " declaration block." );
	  }
	}
	_processor = val;
      }
    }
    else if ( (ANNOTATOR & supported) && doc() ){
      string def;
      try {
	def = doc()->default_processor( annotation_type(), _set );
      }
      catch ( const NoDefaultError& e ){
	if ( doc()->is_incremental() ){
	  // when there is NO default processor, AND we are parsing using
	  // folia::Engine, we must check if there WAS a processor originally
	  // which is 'obscured' by newly added declarations
	  def = doc()->original_default_processor( annotation_type() );
	  if ( doc()->debug > 2 ) {
	    cerr << "from original got default processor='" <<  def << "'" << endl;
	  }
	}
	else {
	  throw;
	}
      }
      _processor = def;
    }

    _confidence = -1;
    val = kwargs.extract( "confidence" );
    if ( !val.empty() ) {
      if ( !(CONFIDENCE & supported) ) {
	throw ValueError("Confidence is not supported for " + classname() );
      }
      else {
	try {
	  _confidence = stringTo<double>(val);
	  if ( _confidence < 0 || _confidence > 1.0 ){
	    throw ValueError("Confidence must be a floating point number between 0 and 1, got " + TiCC::toString(_confidence) );
	  }
	}
	catch (...) {
	  throw ValueError( "invalid Confidence value: " + val
			    + " (not a number?)");
	}
      }
    }

    _n = "";
    val = kwargs.extract( "n" );
    if ( !val.empty() ) {
      if ( !(N & supported) ) {
	throw ValueError("N attribute is not supported for " + classname() );
      }
      else {
	_n = val;
      }
    }
    _datetime.clear();
    val = kwargs.extract( "datetime" );
    if ( !val.empty() ) {
      if ( !(DATETIME & supported) ) {
	throw ValueError("datetime attribute is not supported for " + classname() );
      }
      else {
	string time = parseDate( val );
	if ( time.empty() ){
	  throw ValueError( "invalid datetime, must be in YYYY-MM-DDThh:mm:ss format: " + val );
	}
	_datetime = time;
      }
    }
    else {
      string def;
      if ( doc() &&
	   (def = doc()->default_datetime( annotation_type(), _set )) != "" ) {
	_datetime = def;
      }
    }
    val = kwargs.extract( "begintime" );
    if ( !val.empty() ) {
      if ( !(BEGINTIME & supported) ) {
	throw ValueError( "begintime attribute is not supported for " + classname() );
      }
      else {
	string time = parseTime( val );
	if ( time.empty() ) {
	  throw ValueError( "invalid begintime, must be in HH:MM:SS.mmm format: " + val );
	}
	_begintime = time;
      }
    }
    else {
      _begintime.clear();
    }
    val = kwargs.extract( "endtime" );
    if ( !val.empty() ) {
      if ( !(ENDTIME & supported) ) {
	throw ValueError( "endtime attribute is not supported for " + classname() );
      }
      else {
	string time = parseTime( val );
	if ( time.empty() ) {
	  throw ValueError( "invalid endtime, must be in HH:MM:SS.mmm format: " + val );
	}
	_endtime = time;
      }
    }
    else {
      _endtime.clear();
    }

    val = kwargs.extract( "src" );
    if ( !val.empty() ) {
      if ( !(SRC & supported) ) {
	throw ValueError( "src attribute is not supported for " + classname() );
      }
      else {
	_src = val;
      }
    }
    else {
      _src.clear();
    }
    val = kwargs.extract( "tag" );
    if ( !val.empty() ) {
      if ( !(TAG & supported) ) {
	throw ValueError( "tag attribute is not supported for " + classname() );
      }
      else {
	_tags = val;
      }
    }
    else {
      _tags.clear();
    }

    if ( SPACE & supported ){
      _space = true;
    }
    val = kwargs.extract( "space" );
    if ( !val.empty() ) {
      if ( !(SPACE & supported) ){
	throw ValueError( "space attribute is not supported for " + classname() );
      }
      else {
	if ( val == "no" ) {
	  _space = false;
	}
	else if ( val == "yes" ) {
	  _space = true;
	}
	else {
	  throw ValueError( "invalid value for space attribute: '" + val + "'" );
	}
      }
    }

    val = kwargs.extract( "metadata" );
    if ( !val.empty() ) {
      if ( !(METADATA & supported) ) {
	throw ValueError( "Metadata attribute is not supported for " + classname() );
      }
      else {
	_metadata = val;
	if ( doc() && doc()->get_submetadata( _metadata ) == 0 ){
	  throw KeyError( "No such metadata defined: " + _metadata );
	}
      }
    }
    else {
      _metadata.clear();
    }
    val = kwargs.extract( "speaker" );
    if ( !val.empty() ) {
      if ( !(SPEAKER & supported) ) {
	throw ValueError( "speaker attribute is not supported for " + classname() );
      }
      else {
	_speaker = val;
      }
    }
    else {
      _speaker.clear();
    }

    val = kwargs.extract( "textclass" );
    if ( !val.empty() ) {
      if ( !(TEXTCLASS & supported) ) {
	throw ValueError( "textclass attribute is not supported for " + classname() );
      }
      else {
	_textclass = val;
      }
    }
    else {
      _textclass = "current";
    }

    val = kwargs.extract( "auth" );
    if ( !val.empty() ){
      _auth = stringTo<bool>( val );
    }
    if ( doc() && !_id.empty() ) {
      try {
	doc()->add_doc_index( this, _id );
      }
      catch ( const DuplicateIDError& e ){
	if ( element_id() != WordReference_t ){
	  throw;
	}
      }
    }
    kwargs.erase("typegroup"); //this is used in explicit form only, we can safely discard it
    addFeatureNodes( kwargs );
#ifdef LOG_SET_ATT
    doc()->setdebug(db_level);
#endif
  }

  void AbstractElement::addFeatureNodes( const KWargs& kwargs ) {
    /// add children to the object, based on the set of Key-Value pairs.
    /*!
     * \param kwargs a KWargs set of Key-Value pairs
     * the given keys must be in the AttributeFeatures set.
     * the values are used as class attribute for the new children
     */
    for ( const auto& it: kwargs ) {
      string tag = it.first;
      if ( tag == "head" ) {
	// "head" is special because the tag is "headfeature"
	// this to avoid conflicts with the "head" tag!
	tag = "headfeature";
      }
      if ( AttributeFeatures.find( tag ) == AttributeFeatures.end() ) {
	string message = "unsupported attribute: " + tag + "='" + it.second
	  + "' for node with tag '" + classname() + "'";
	if ( tag == "id" ){
	  message += "\ndid you mean xml:id?";
	}
	if ( doc() && doc()->permissive() ) {
	  cerr << message << endl;
	}
	else {
	  throw XmlError( message );
	}
      }
      KWargs newa;
      newa["class"] = it.second;
      FoliaElement *new_node = createElement( tag, doc() );
      new_node->setAttributes( newa );
      append( new_node );
    }
  }

  string toDoubleString( double d ){
    if ( d == 1.0 ){
      return "1.0";
    }
    else if ( d == 0.0 ){
      return "0.0";
    }
    else {
      stringstream ss;
      ss.precision(6);
      ss << d;
      return ss.str();
    }
  }

  KWargs AllowXlink::collectAttributes() const {
    KWargs attribs;
    auto it = _xlink.find("type");
    if ( it != _xlink.end() ){
      string type = it->second;
      if ( type == "simple" || type == "locator" ){
	it = _xlink.find("href");
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
    return attribs;
  }

  void AbstractElement::set_typegroup( KWargs& attribs ) const {
    if ( isSubClass( AbstractStructureElement_t ) ){
      attribs["typegroup"] = "structure";
    }
    else if ( isSubClass(  Feature_t ) ){
      attribs["typegroup"] = "feature";
    }
    else if ( isSubClass( AbstractInlineAnnotation_t ) ){
      attribs["typegroup"] = "inline";
    }
    else if ( isSubClass( AbstractHigherOrderAnnotation_t ) ){
      attribs["typegroup"] = "higherorder";
    }
    else if ( isSubClass(  AbstractSpanRole_t ) ){
      attribs["typegroup"] = "spanrole";
    }
    else if ( isSubClass(  AbstractSpanAnnotation_t ) ){
      attribs["typegroup"] = "span";
    }
    else if ( isSubClass(  AbstractTextMarkup_t ) ){
      attribs["typegroup"] = "textmarkup";
    }
    else if ( isSubClass(  AbstractContentAnnotation_t ) ){
      attribs["typegroup"] = "content";
    }
    else if ( isSubClass(  AbstractAnnotationLayer_t ) ){
      attribs["typegroup"] = "layer";
    }
    else if ( isSubClass(  AbstractSubtokenAnnotation_t ) ){
      attribs["typegroup"] = "subtoken";
    }
    else if ( isSubClass(  AbstractCorrectionChild_t ) ){
      attribs["typegroup"] = "correctionchild";
    }
    else {
      cerr << "UNHANDLED " << element_id() << endl;
    }
  }

  KWargs AbstractElement::collectAttributes() const {
    /// extract all Attribute-Value pairs from the object
    /*!
     * \return a KWargs set of Attribute-value pairs
     * Might also use declaration defaults and alias declarations to extract
     * default values
     */
    KWargs attribs;
    bool Explicit = false;
    Attrib supported = required_attributes() | optional_attributes();
    if ( doc()->has_explicit() ){
      Explicit = true;
      set_typegroup( attribs );
    }
    if ( !_id.empty() ) {
      attribs["xml:id"] = _id;
    }
    if ( _preserve_spaces == SPACE_FLAGS::PRESERVE ) {
      attribs["xml:space"] = "preserve";
    }
    string default_set = doc()->default_set( annotation_type() );
    bool isDefaultSet = (_set == default_set);
    if ( Explicit && _set != "None" && !default_set.empty() ){
      if ( _set.empty() ){
	attribs["set"] = default_set;
      }
      else {
	attribs["set"] = _set;
      }
    }
    else if ( _set != "None"
	      && !_set.empty()
	      && !isDefaultSet ){
      string ali = doc()->alias( annotation_type(), _set );
      if ( ali.empty() ){
	attribs["set"] = _set;
      }
      else {
	attribs["set"] = ali;
      }
    }
    if ( !_class.empty() ) {
      attribs["class"] = _class;
    }
    if ( !_processor.empty() ){
      string tmp;
      try {
	tmp = doc()->default_processor( annotation_type(), _set );
	if ( Explicit ){
	  attribs["processor"] = tmp;
	}
      }
      catch ( NoDefaultError& ){
      }
      catch ( ... ){
	throw;
      }
      if ( tmp != _processor ){
	attribs["processor"] = _processor;
      }
    }
    else {
      bool isDefaultAnn = true;
      if ( !_annotator.empty() &&
	   _annotator != doc()->default_annotator( annotation_type(), _set ) ) {
	isDefaultAnn = false;
	attribs["annotator"] = _annotator;
      }
      if ( _annotator_type != UNDEFINED ){
	AnnotatorType at = doc()->default_annotatortype( annotation_type(), _set );
	if ( (!isDefaultSet || !isDefaultAnn)
	     && _annotator_type != at ) {
	  if ( _annotator_type == AUTO ) {
	    attribs["annotatortype"] = "auto";
	  }
	  else if ( _annotator_type == MANUAL ) {
	    attribs["annotatortype"] = "manual";
	  }
	}
      }
    }
    if ( !_datetime.empty() &&
	 _datetime != doc()->default_datetime( annotation_type(), _set ) ) {
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
    if ( !_tags.empty() ) {
      attribs["tag"] = _tags;
    }
    if ( !_metadata.empty() ) {
      attribs["metadata"] = _metadata;
    }
    if ( !_speaker.empty() ) {
      attribs["speaker"] = _speaker;
    }
    if ( ( TEXTCLASS & supported)
	 && ( !_textclass.empty() &&
	      ( _textclass != "current" || Explicit ) ) ){
      attribs["textclass"] = _textclass;
    }

    if ( _confidence >= 0 ) {
      attribs["confidence"] = toDoubleString(_confidence);
    }
    if ( !_n.empty() ) {
      attribs["n"] = _n;
    }
    if ( !_auth ) {
      attribs["auth"] = "no";
    }
    if ( SPACE & optional_attributes() ){
      if ( !_space ) {
	attribs["space"] = "no";
      }
    }
    return attribs;
  }

  const string FoliaElement::xmlstring( bool add_ns ) const{
    /// serialize a FoLiAElement to a string (XML fragment)
    /*!
     * \param add_ns Also add the NameSpace declarations
     * \return a string representation of the FoLiA XML
     */
    return xmlstring( false, 0, add_ns );
  }

  const string FoliaElement::xmlstring( bool format,
					int indent,
					bool add_ns ) const{
    /// serialize a FoLiAElement to a string (XML fragment)
    /*!
     * \param format allow output formating
     * \param indent number of spaces to indent
     * \param add_ns Also add the NameSpace declarations
     * \return a string representation of the FoLiA XML
     */
    xmlNode *n = xml( true, false );
    if ( add_ns ){
      xmlSetNs( n, xmlNewNs( n, (const xmlChar *)NSFOLIA.c_str(), 0 ) );
    }
    xmlBuffer *buf = xmlBufferCreate();
    //    xmlKeepBlanksDefault(0);
    xmlNodeDump( buf, 0, n, indent, (format?1:0) );
    string result = (const char*)xmlBufferContent( buf );
    xmlBufferFree( buf );
    xmlFreeNode( n );
    return result;
  }

  string tagToAtt( const FoliaElement* c ) {
    /// helper function. Given an element of type Feature_t, return the tag value
    /*!
     * \param c some FoLiAElement
     * \return the string value of attribute related to the tag of the parameter
     * if the element is of type Feature_t is has an asscociated attribute
     * otherwise not, and the empty string is returned.
     */
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

  void AbstractElement::check_append_text_consistency( const FoliaElement *child ) const {
    /// check the text consistency of a new child against the Element.
    /*!
     * \param child the new child
     *
     * When a document is available AND it has the checktext() property
     * the text of the child is checked against the text of the parent.
     *
     * will throw on error.
     *
     * For Word, String and TextContent children, we assume that their text is
     * embedded in the parents text.
     *
     * For all other cases, the text of the child should match the parents text.
     * \note Matching is opaque to spaces, newlines and tabs
     */
    //    cerr << "VOOR checkappend I am=" << this << endl;
    //    cerr << "VOOR checkappend child=" << child << endl;
    if ( !doc() || !doc()->checktext() || doc()->fixtext() ){
      return;
    }
    string cls = child->cls();
    //    cerr << "HIER 2 " << cls << endl;
    if ( child->size() == 0
	 || ( child->is_textcontainer()
	      && !child->hastext( cls ) ) ){
      // no use to proceed. not adding real text
      return;
    }
    //    cerr << "HIER 3 " << endl;
    const FoliaElement *parent = 0;
    if ( child->is_textcontainer() ){
      parent = this->parent();
    }
    else {
      parent = this;
      cls = child->index(0)->cls();
    }
    //    cerr << "PARENT? " << parent << endl;
    if ( parent
	 && parent->element_id() != Correction_t
	 && parent->hastext( cls ) ){
      // check text consistency for parents with text
      // but SKIP Corrections
      UnicodeString s1 = parent->stricttext( cls );
      UnicodeString s2 = child->stricttext( cls );
      // cerr << "check parent: " << s1 << endl;
      // cerr << "check child: " << s2 << endl;
      // no retain tokenization, strict for both
      s1 = normalize_spaces( s1 );
      s2 = normalize_spaces( s2 );
      if ( !s1.isEmpty() && !s2.isEmpty() ){
	bool test_fail;
	if ( isSubClass( Word_t )
	     || child->isSubClass( TextContent_t )
	     || isSubClass( String_t )
	     || child->isSubClass( Word_t ) ){
	  // Words and Strings are 'per definition' PART of their parents
	  test_fail = ( s1.indexOf( s2 ) < 0 ); // aren't they?
	}
	else {
	  // otherwise an exacte match is needed
	  test_fail = ( s1 != s2 );
	}
	if ( test_fail ){
	  throw InconsistentText( "adding text (class="
				  + cls + ") from node: " + child->xmltag()
				  + "(" + child->id() + ")"
				  + " with value\n'" + TiCC::UnicodeToUTF8(s2)
				  + "'\n to element: " + parent->xmltag() +
				  + "(" + parent->id() + ") which already has "
				  + "text in that class and value: \n'"
				  + TiCC::UnicodeToUTF8(s1) + "'\n" );
	}
      }
    }
  }

  void AbstractElement::check_text_consistency( bool trim_spaces ) const {
    /// check the text consistency of the combined text of the children
    /// against the text of the Element.
    /*!
     * When a document is available AND it has the checktext() property
     * the combined text of ALL the children is checked against the text of
     * the parent.
     *
     * will throw on error
     *
     * For Word and String children, we only assume that their text is
     * embedded in the parents text.
     *
     * For all other cases, the text should exactly match the parents text.
     * \note Matching is opaque to spaces, newlines and tabs
     */
    if ( !doc() || !doc()->checktext() || !printable() ){
      return;
    }

    string cls = this->cls();
    FoliaElement *parent = this->parent();
    if ( parent
	 && parent->element_id() != Correction_t
	 && parent->hastext( cls ) ){
      // check text consistency for parents with text
      // but SKIP Corrections
      // no retain tokenization, strict for parent, deeper for child
      TEXT_FLAGS flags = TEXT_FLAGS::STRICT;
      if (!trim_spaces) {
	flags |= TEXT_FLAGS::NO_TRIM_SPACES;
      }
      UnicodeString s1 = parent->text( cls, flags );
      flags = TEXT_FLAGS::NONE;
      if ( !trim_spaces ) {
	flags |= TEXT_FLAGS::NO_TRIM_SPACES;
      }
      UnicodeString s2 = this->text( cls, flags );
      s1 = normalize_spaces( s1 );
      s2 = normalize_spaces( s2 );
      bool test_fail;
      if ( isSubClass( Word_t )
	   || isSubClass( String_t )
	   || isSubClass( AbstractTextMarkup_t ) ) {
	// Words, Strings and AbstractTextMarkup are 'per definition' PART of
	// their text parents
	test_fail = ( s1.indexOf( s2 ) < 0 ); // aren't they?
      }
      else {
	// otherwise an exacte match is needed
	test_fail = ( s1 != s2 );
      }
      if ( test_fail ){
        bool warn_only = false;
        if ( trim_spaces ) {
	  //ok, we failed according to the >v2.4.1 rules
	  //but do we also fail under the old rules?
	  try {
	    this->check_text_consistency(false);
	    warn_only = true;
	  } catch ( const InconsistentText& ) {
	    //ignore, we raise the newer error
	  }
        }
	string msg = "text (class="
	  + cls + ") from node: " + xmltag()
	  + "(" + id() + ")"
	  + " with value\n'" + TiCC::UnicodeToUTF8(s2)
	  + "'\n to element: " + parent->xmltag() +
	  + "(" + parent->id() + ") which already has "
	  + "text in that class and value: \n'"
	  + TiCC::UnicodeToUTF8(s1) + "'\n";
        if (warn_only) {
	  msg += "However, according to the older rules (<v2.4.1) the text is consistent. So we are treating this as a warning rather than an error. We do recommend fixing this if this is a document you intend to publish.\n";
	  cerr << "WARNING: inconsistent text: " << msg << endl;
        }
	else {
	  throw InconsistentText(msg);
        }
      }
    }
  }

  //#define DEBUG_TEXT
  //#define DEBUG_TEXT_DEL

  void AbstractElement::check_text_consistency_while_parsing( bool trim_spaces ) {
      // this block was moved from parseXml into a separate function
      // it remains to be seen how much overlaps with check_text_consistency()
      // and whether we can't make do with one function
      //
      // unlike the other function, this does do some fixing when requested
      //

#ifdef DEBUG_TEXT
      cerr << "DEBUG: BEGIN check_text_consistency_while_parsing(" << trim_spaces << ")" << endl;
#endif

      vector<TextContent*> tv = select<TextContent>( false );
      // first see which text classes are present
      set<string> cls;
      for ( const auto& it : tv ){
	cls.insert( it->cls() );
      }
      // check the text for every text class
      for ( const auto& st : cls ){
	UnicodeString s1, s2;
        TEXT_FLAGS flags = TEXT_FLAGS::STRICT;
        if ( !trim_spaces ) {
	  flags |= TEXT_FLAGS::NO_TRIM_SPACES;
	}
	try {
	  s1 = text( st, flags );  // no retain tokenization, strict
	}
	catch (...){
	}
	if ( !s1.isEmpty() ){
	  //	  cerr << "S1: " << s1 << endl;
	  flags = TEXT_FLAGS::NONE;
	  if ( !trim_spaces ) {
	    flags |= TEXT_FLAGS::NO_TRIM_SPACES;
	  }
	  try {
	    s2 = text( st, flags ); // no retain tokenization, no strict
	  }
	  catch (...){
	  }
	  //	  cerr << "S2: " << s2 << endl;
	  s1 = normalize_spaces( s1 );
	  s2 = normalize_spaces( s2 );
	  if ( !s2.isEmpty() && s1 != s2 ){
	    if ( doc()->fixtext() ){
	      //	      cerr << "FIX: " << s1 << "==>" << s2 << endl;
	      KWargs args;
	      args["value"] = TiCC::UnicodeToUTF8(s2);
	      args["class"] = st;
	      TextContent *node = new TextContent( args, doc() );
	      this->replace( node );
	    }
	    else {
              bool warn_only = false;
              if ( trim_spaces ) {
		//ok, we failed according to the >v2.4.1 rules
		//but do we also fail under the old rules?
		try {
#ifdef DEBUG_TEXT
		  cerr << "DEBUG: (testing according to older rules now)" << endl;
#endif
		  this->check_text_consistency_while_parsing(false);
		  warn_only = true;
		}
		catch ( const InconsistentText& e ) {
		  cerr << "(tested according to older rules (<v2.4.1) as well, but this failed too)" << endl;
		  //ignore, we raise the newer error
		}
              }
	      string msg = "node " + xmltag() + "(" + id()
		+ ") has a mismatch for the text in set:" + st
		+ "\nthe element text ='" + TiCC::UnicodeToUTF8(s1)
		+ "'\n" + " the deeper text ='" + TiCC::UnicodeToUTF8(s2) + "'";
              if ( warn_only ) {
		msg += "\nHOWEVER, according to the older rules (<v2.4.1) the text is consistent. So we are treating this as a warning rather than an error. We do recommend fixing this if this is a document you intend to publish.\n";
		cerr << "WARNING: inconsistent text: " << msg << endl;
              }
	      else {
#ifdef DEBUG_TEXT
		cerr << "DEBUG: CONSISTENCYERROR check_text_consistency_while_parsing(" << trim_spaces << ")" << endl;
#endif
		throw InconsistentText(msg);
              }
	    }
	  }
	}
      }
#ifdef DEBUG_TEXT
      cerr << "DEBUG: END-OK check_text_consistency_while_parsing(" << trim_spaces << ")" << endl;
#endif
  }

  xmlNode *AbstractElement::xml( bool recursive, bool kanon ) const {
    /// convert an Element to an xmlNode
    /*!
     * \param recursive Convert the children too, creating a xmlNode tree
     * \param kanon Output in a canonical form to make comparions easy
     * \return am xmlNode object(-tree)
     */
    xmlNode *e = XmlNewNode( foliaNs(), xmltag() );
    KWargs attribs = collectAttributes();
    if ( _preserve_spaces == SPACE_FLAGS::PRESERVE ){
      // we carry an 'xml:space="preserve" flag?
      if ( doc()->preserve_spaces() ){
	// if our ancestor did also, clear it here
	attribs.extract( "xml:space" );
      }
      else {
	// otherwise leave it, and notify our document
	doc()->set_preserve_spaces(true);
      }
    }
    else if ( doc()->preserve_spaces() ){
      // this subtree should go back to "default" then
      attribs["xml:space"] = "default";
      // and the doc needs to know it
      doc()->set_preserve_spaces(false);
    }
    set<FoliaElement *> attribute_elements;
    // nodes that can be represented as attributes are converted to atributes
    // and excluded of 'normal' output.

    if ( !doc()->has_explicit() ){
      map<string,int> af_map;
      // first we search al features that can be serialized to an attribute
      // and count them!
      for ( const auto& el : _data ) {
	string at = tagToAtt( el );
	if ( !at.empty() ) {
	  ++af_map[at];
	}
      }
      // ok, now we create attributes for those that only occur once
      for ( const auto& el : _data ) {
	string at = tagToAtt( el );
	if ( !at.empty() && af_map[at] == 1 ) {
	  attribs[at] = el->cls();
	  attribute_elements.insert( el );
	}
      }
    }
    addAttributes( e, attribs );
    if ( _data.empty() ){
      return e; // we are done
    }
    if ( recursive ) {
      // append children:
      // we want make sure that text elements are in the right order,
      // in front and the 'current' class first
      list<FoliaElement *> currenttextelements;
      list<FoliaElement *> textelements;
      list<FoliaElement *> otherelements;
      list<FoliaElement *> commentelements;
      multimap<ElementType, FoliaElement *, std::greater<ElementType>> otherelementsMap;
      for ( const auto& el : _data ) {
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
	for ( const auto& oem : otherelements ) {
	  xmlAddChild( e, oem->xml( recursive, kanon ) );
	}
      }
      else {
	for ( const auto& oem : otherelementsMap ) {
	  xmlAddChild( e, oem.second->xml( recursive, kanon ) );
	}
      }
      check_text_consistency();
    }
    return e;
  }

  const string AbstractElement::str( const string& cls ) const {
    /// return the text value of this element
    /*!
     * \param cls The desired textclass
     * \return the string value (UTF8 encoded)
     *
     * if this is a TextContent or it may contain TextContent
     * then return the associated text()
     *
     * if this is a PhonContent or it may contain PhonContent
     * then return the associated phon()
     *
     * otherwise return the empty string
     */
    UnicodeString us;
    try {
      us = text(cls);
    }
    catch( const NoSuchText& ){
      try {
	us = phon(cls);
      }
      catch( const NoSuchPhon&){
	// No TextContent or Phone is allowed
      }
    }
    return TiCC::UnicodeToUTF8( us );
  }

  const string AbstractElement::str( const TextPolicy& tp ) const {
    /// return the text value of this element
    /*!
     * \param tp the TextPolicy to use
     * \return the string value (UTF8 encoded)
     *
     * if this is a TextContent or it may contain TextContent
     * then return the associated text()
     *
     * if this is a PhonContent or it may contain PhonContent
     * then return the associated phon()
     *
     * otherwise return the empty string
     */
    UnicodeString us;
    try {
      us = text( tp );
    }
    catch( const NoSuchText& ){
      try {
	us = phon( tp._class);
      }
      catch( const NoSuchPhon&){
	// No TextContent or Phone is allowed
      }
    }
    return TiCC::UnicodeToUTF8( us );
  }

  const string AbstractElement::speech_src() const {
    /// give the value of the _scr of an element
    /*!
     * return a (possibly empty) string.
     *
     * This function recurses upward to the first element which carries _src
     */
    if ( !_src.empty() ) {
      return _src;
    }
    if ( _parent ) {
      return _parent->speech_src();
    }
    return "";
  }

  const string AbstractElement::speech_speaker() const {
    /// give the value of the _speaker of an element
    /*!
     * return a (possibly empty) string.
     *
     * This function recurses upward to the first element which carries _speaker
     */
    if ( !_speaker.empty() ) {
      return _speaker;
    }
    if ( _parent ) {
      return _parent->speech_speaker();
    }
    return "";
  }

  const string AbstractElement::language( const string& st ) const {
    /// give the language value of an element
    /*!
     * \param st the setname to us for searching
     * The search will start at the object, and recurse upward until
     * the document level, where it will return the Documents language
     * Might return "" when no match is found
     */
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
    /// check if the element has a TextContent with class 'cls'
    /*!
     * \param cls The desired textclass
     * \return true if there is a TextContent available. Otherwise false
     */
    try {
      this->text_content(cls);
      return true;
    } catch ( const NoSuchText& e ) {
      return false;
    }
  }

  bool FoliaElement::hasphon( const string& cls ) const {
    /// check if the element has a PhonContent with class 'cls'
    /*!
     * \param cls The desired textclass
     * \return true if there is a PhonContent available. Otherwise false
     */
    try {
      this->phon_content(cls);
      return true;
    } catch ( const NoSuchPhon& e ) {
      return false;
    }
  }

  const string& AbstractElement::get_delimiter( bool retaintok ) const {
    /// get the default delimiter of this object.
    /*!
     * \param retaintok retain the tokenization assigned to this element
     * \return a string representing the delimiter
     *
     * If the object has a TEXTDELIMITER property thats is returned
     * Otherwise, the last child is taken and its delimiter is returned IF
     * it is a Structure Element.
     * When this test fails, an empty string is returned, UNLESS the element has
     * the SPACE attribute AND retaintok is specified
     */

#ifdef DEBUG_TEXT_DEL
    cerr << "IN <" << xmltag() << ">:get_delimiter (" << retaintok << ")" << endl;
#endif
    if ( (SPACE & optional_attributes()) ){
      if ( ! ( _space || retaintok ) ){
#ifdef DEBUG_TEXT_DEL
	cerr << " space = NO, return: '" << EMPTY_STRING << "'" << endl;
#endif
	return EMPTY_STRING;
      }
    }

    if ( !data().empty() ){
      FoliaElement *last = data().back();
      if ( last &&
	   last->isSubClass(AbstractStructureElement_t)
	   && !last->space() ){
	return EMPTY_STRING;
      }
    }
    if ( _props.TEXTDELIMITER != "NONE" ) {
      return _props.TEXTDELIMITER;
    }
    else if ( _data.size() > 0 ) {
      // attempt to get a delimiter from the last child
      FoliaElement *last = _data.back();
      if ( last->isSubClass(AbstractStructureElement_t) ){
	const string& det = last->get_delimiter( retaintok );
#ifdef DEBUG_TEXT_DEL
	cerr << "out <" << xmltag() << ">:get_delimiter ==> '" << det << "'" << endl;
#endif
	return det;
      }
    }
#ifdef DEBUG_TEXT_DEL
    cerr << "out <" << xmltag() << ">:get_delimiter ==> ''" << endl;
#endif
    return EMPTY_STRING;
  }

  bool is_space( const UChar32 kar ){
    return ( kar == 0x0020       // space
	     || kar == 0x0009    // tab
	     || kar == 0x000a    // newline
	     || kar == 0x000d ); // carriage return
  }

  UnicodeString AbstractElement::text_container_text( const TextPolicy& tp ) const {
    if ( isinstance( TextContent_t )
	 && this->cls() != tp._class ) {
      // take a shortcut for TextContent in wrong class
#ifdef DEBUG_TEXT
      cerr << "TextContent shortcut, class=" << this->cls()
	   << " but looking for: " << tp._class << endl;
#endif
      return "";
    }
    UnicodeString result;
    bool pendingspace = false;
    bool trim_spaces = !tp.is_set( TEXT_FLAGS::NO_TRIM_SPACES);
    bool retaintok  = tp.is_set( TEXT_FLAGS::RETAIN );
    for ( const auto& d : _data ){
      if (d->isinstance( XmlText_t)) {
	// 'true' text child
	if (pendingspace) {
	  result += " ";
	  pendingspace = false;
	}
	if ( trim_spaces) {
	  //This implements https://github.com/proycon/folia/issues/88
	  //FoLiA >= v2.5 behaviour (introduced earlier in v2.4.1 but modified thereafter)
	  const int l = result.length();
	  UnicodeString text = d->text( tp._class );
	  int begin = 0;
	  int linenr = 0;
	  for ( int i = 0; i < text.length(); ++i ) {
	    if ( text[i] == 0x000a
		 || (i == text.length() - 1) ) {
	      //newline or end
	      UnicodeString line;
	      if ( text[i] == 0x000a ) { //newline
		line = UnicodeString(text, begin, i - begin);
	      }
	      else {
		line = UnicodeString(text, begin, text.length() - begin);
	      }
	      begin = i+1;

	      UnicodeString subresult;
	      if ( _preserve_spaces == SPACE_FLAGS::PRESERVE) {
		if ( line.length() > 0
		     && line[line.length() - 1] == 0x000d) {
		  //carriage return
		  //remove artefacts of any DOS-style line endings (not sure if still
		  //needed here but better safe than sorry)
		  line = UnicodeString(line, 0, line.length() - 1);
		}
		subresult = line;
	      }
	      else {
		subresult = normalize_spaces(trim_space(line));
	      }

	      if ( (linenr > 0)
		   && (subresult.length() > 0)
		   && (result.length() != l) ) {
		//insert spaces between lines that used to be newline separated
		result.append((UChar32) 0x0020);
	      }
	      else if ( (subresult.length() > 0)
			&& (line.length() > 0)
			&& ( is_space(line[0]) )
			&& this->_preserve_spaces != SPACE_FLAGS::PRESERVE ) {
		//we have leading indentation we may need to collapse or ignore entirely
		//we can't be sure yet what to do so we add a temporary placeholder \1
		//this will later be handled in postprocess_spaces() (converts to a space only if no space preceeds it)
		result.append(0x0001);
	      }
	      result += subresult;
	      linenr++;
	    }
	  }

	  if ( this->_preserve_spaces != SPACE_FLAGS::PRESERVE
	       && text.length() > 0
	       && result.length() > 0
	       && is_space(text[text.length() - 1]) ){
	    //this item has trailing spaces but we stripped them
	    //this may be premature so
	    //we reserve to output them later in case there is a next item
	    pendingspace = true;
	  }
	}
	else {
	  //old FoLiA <= v2.4.1 behaviour, we don't trim anything
	  result += d->text( tp._class );
	}
      }
      else if ( d->printable() ){
	// this is some TextMarkup I hope
	if (pendingspace) {
	  result += " ";
	  pendingspace = false;
	}
	string tv = d->tag();
	if ( !tv.empty() ){
	  vector<string> tvv = TiCC::split(tv);
	  bool no_match = true;
	  for ( const auto& v : tvv ){
	    auto fun = tp._tag_handlers.find( v );
	    if ( fun != tp._tag_handlers.end() ){
	      no_match = false;
	      UnicodeString tmp_result = (*fun).second( d, tp );
	      result += tmp_result;
	    }
	  }
	  if ( no_match ){
	    result += d->text( tp );
	  }
	}
	else {
	  result += d->text( tp );
	}
	if ( !result.isEmpty() ){
	  const string& delim = d->get_delimiter( retaintok );
#ifdef DEBUG_TEXT
	  cerr << "append delimiter: '" << delim << "'" << endl;
#endif
	  result += TiCC::UnicodeFromUTF8(delim);
	}
      }
      else {
	// non interesting stuff like <feature>, <comment> etc.
      }
    }
    if (trim_spaces && this->preserve_spaces() != SPACE_FLAGS::PRESERVE) {
      result = postprocess_spaces(result);
    }
#ifdef DEBUG_TEXT
    cerr << "TEXT(" << tp._class << ") on a textcontainer :" << xmltag()
	 << " returned '" << result << "'" << endl;
#endif
      return result;
  }

  const UnicodeString AbstractElement::private_text( const TextPolicy& tp ) const {
    /// get the UnicodeString value of an element
    /*!
     * \param cls The textclass we are looking for
     * \param retaintok retain the tokenisation information
     * \param strict If true, return the text of this level only
     * when false, allow recursing into children
     * \param show_hidden include text form 'hidden' nodes too.
     * \param trim_spaces Trim leading and trailing spaces (defaults to True since FoLiA v2.4.1)
     * \return the Unicode String representation found. Throws when
     * no text can be found
     */
    bool strict = tp.is_set( TEXT_FLAGS::STRICT );
    bool show_hidden = tp.is_set( TEXT_FLAGS::HIDDEN );
    bool trim = !tp.is_set( TEXT_FLAGS::NO_TRIM_SPACES );
#ifdef DEBUG_TEXT
    cerr << "TEXT(" << cls << ") on node : " << xmltag() << " id="
	 << id() << endl;
    cerr << "TextPolicy: " << tp << endl;
#endif
    if ( strict ) {
      /// WARNING. Don't call text(tp) here. We will get into an infinite
      /// recursion. Can't we do better then calling ourself again, sort of?
      TextPolicy tmp = tp;
      tmp._text_flags = TEXT_FLAGS( tmp._text_flags & ~TEXT_FLAGS::STRICT );
      return text_content(tp._class,show_hidden)->text(tmp);
    }
    else if ( !printable() || ( hidden() && !show_hidden ) ){
      throw NoSuchText( "NON printable element: " + xmltag() );
    }
    else if ( is_textcontainer() ){
      return text_container_text( tp );
    }
    else {
      //
      UnicodeString result = deeptext( tp );
      if ( result.isEmpty() ) {
	TEXT_FLAGS flags = TEXT_FLAGS::STRICT;
	if ( !trim ) {
	  flags |= TEXT_FLAGS::NO_TRIM_SPACES;
	}
	result = text( tp._class, flags );
      }
      if ( result.isEmpty() ) {
	throw NoSuchText( "on tag " + xmltag() + " nor it's children" );
      }
      return result;
    }
  }

  const UnicodeString AbstractElement::text( const TextPolicy& tp ) const {
    /// get the UnicodeString text value of an element
    /*!
     * \param tp a TextPolicy
     * \param flags the search parameters to use. See TEXT_FLAGS.
     */
#ifdef DEBUG_TEXT
    cerr << "DEBUG <" << xmltag() << ">.text() Policy=" << tp << endl;
#endif
    return private_text( tp );
  }

  const UnicodeString AbstractElement::text( const std::string& cls,
					     TEXT_FLAGS flags ) const {
    /// get the UnicodeString text value of an element
    /*!
     * \param cls the textclass the text should be in
     * \param flags the search parameters to use. See TEXT_FLAGS.
     */
    TextPolicy tp( cls, flags );
#ifdef DEBUG_TEXT
    cerr << "DEBUG <" << xmltag() << ">.text() Policy=" << tp << endl;
#endif
    return private_text( tp );
  }

  void FoLiA::setAttributes( KWargs& kwargs ){
    /// set the attributes of a FoLiA top node
    /*!
     * \param kwargs an attribute-value list
     * the FoLiA top is special, as it may accept special attributes
     * which are stored in the associated document, and NOT in the node
     */
    // we store some attributes in the document itself
    doc()->setDocumentProps( kwargs );
    // use remaining attributes for the FoLiA node
    // probably only the ID
    AbstractElement::setAttributes( kwargs );
  }

  FoliaElement* FoLiA::parseXml( const xmlNode *node ){
    ///
    /// recursively parse a complete FoLiA tree
    /// \param node an xmlNode that MUST be a FoLiA root node
    /// \return the parsed tree. Throws on error.
    /*!
     * the topnode is special, as it also carries the main document properties
     *
     */
    KWargs atts = getAttributes( node );
    if ( !doc() ){
      throw XmlError( "FoLiA root without Document" );
    }
    setAttributes( atts );
    bool meta_found = false;
    xmlNode *p = node->children;
    while ( p ){
      if ( p->type == XML_ELEMENT_NODE ){
	if ( TiCC::Name(p) == "metadata" &&
	     checkNS( p, NSFOLIA ) ){
	  if ( doc()->debug > 1 ){
	    cerr << "Found metadata" << endl;
	  }
	  doc()->parse_metadata( p );
	  meta_found = true;
	}
	else if ( p && TiCC::getNS(p) == NSFOLIA ){
	  string tag = TiCC::Name( p );
	  if ( !meta_found  && !doc()->version_below(1,6) ){
	    throw XmlError( "Expecting element metadata, got '" + tag + "'" );
	  }
	  FoliaElement *t = AbstractElement::createElement( tag, doc() );
	  if ( t ){
	    if ( doc()->debug > 2 ){
	      cerr << "created " << t << endl;
	    }
	    t = t->parseXml( p );
	    if ( t ){
	      if ( doc()->debug > 2 ){
		cerr << "extend " << this << " met " << tag << endl;
	      }
	      this->append( t );
	    }
	  }
	}
      }
      else if ( p->type == XML_TEXT_NODE ){
	// This MUST be 'empty space', so only spaces and tabs formatting
	string txt = TextValue(p);
	txt = TiCC::trim(txt);
	if ( !txt.empty() ){
	  if ( p->prev ){
	    string tg = "<" + Name(p->prev) + ">";
	    throw XmlError( "found extra text '" + txt + "' after element "
			    + tg + ", NOT allowed there." );
	  }
	  else {
	    string tg = "<" + Name(p->parent) + ">";
	    throw XmlError( "found extra text '" + txt + "' inside element "
			    + tg + ", NOT allowed there." );
	  }
	}
      }
      p = p->next;
    }
    return this;
  }

  const UnicodeString FoLiA::private_text( const TextPolicy& tp ) const {
    /// get the UnicodeString value of a FoLiA topnode
    /*!
     * \param cls The textclass we are looking for
     * \param retaintok retain the tokenisation information
     * \param strict If true, return the text of the direct children only
     * when false, allow recursing into children
     * \param trim_spaces Trim leading and trailing spaces (defaults to True since FoLiA v2.4.1)
     * \return the Unicode String representation found. Throws when
     * no text can be found
     */
    bool retain = tp.is_set( TEXT_FLAGS::RETAIN );
#ifdef DEBUG_TEXT
    cerr << "FoLiA::TEXT(" << cls << ")" << endl;
#endif
    const vector<FoliaElement *>& data = this->data();
    UnicodeString result;
    for ( const auto& d : data ){
      if ( !result.isEmpty() ){
	const string& delim = d->get_delimiter( retain );
	result += TiCC::UnicodeFromUTF8(delim);
      }
      result += d->private_text( tp );
    }
#ifdef DEBUG_TEXT
    cerr << "FoLiA::TEXT returns '" << result << "'" << endl;
#endif
    return result;
  }

  UnicodeString trim_space( const UnicodeString& in ){
    /// remove leading and traling spaces. KEEP newlines etc.
    /*!
     * \param in an untrimmed UnicodeString
     * \return an UnicodeString with all leading and trailing spaces removed.
     * Other 'whitespace' characters like newline and tab are retained!
     */
    const char16_t space = 0x0020;
    //    cerr << "in = '" << in << "'" << endl;
    UnicodeString out;
    int i = 0;
    for( ; i < in.length(); ++i ){
      //      cerr << "start: bekijk:" << UnicodeString(in[i]) << endl;
      if ( in[i] != space ){
	break;
      }
    }
    int j = in.length()-1;
    for( ; j >= 0; --j ){
      //      cerr << "end: bekijk:" << UnicodeString(in[j]) << endl;
      if ( in[j] != space ){
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

  UnicodeString postprocess_spaces( const UnicodeString& in ){
    ///Postprocessing for spaces, translates temporary \1 codepoints to spaces
    /// if they are are not preceeded by whitespace
    bool need_postprocessing = false;
    for (int i = 0; i < in.length(); i++) {
      if (in[i] == 0x0001) {
	need_postprocessing = true;
	break;
      }
    }
    if (!need_postprocessing) {
      return in;
    }
    else {
      UnicodeString result;
      for (int i = 0; i < in.length(); ++i) {
	if ( in[i] == 0x0001 ) {
	  if ( i > 0
	       && !is_space(in[i-1]) ){
	    result.append((UChar32) 0x0020); //add a space
	    // 1 byte is dropped otherwise
	  }
	}
	else {
	  result.append(in[i]);
	}
      }
      return result;
    }
  }

  bool check_end( const UnicodeString& us, bool& only ){
    /// check for newline characters at the end
    /*!
     * \param us the UnicodeString to check for '\n'
     * \param only set to true if the whole string consists of only '\n'
     * \return true when at least 1 '\n' is found at the end.
     */
    only = false;
    string tmp = TiCC::UnicodeToUTF8( us );
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
    /// given a FoliaElement check if the last Word in it has space()
    /*!
     * \param s a FoliaElement
     * \return true if the element contains Word children and the last
     * one has space()
     */
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

  const UnicodeString AbstractElement::deeptext( const TextPolicy& tp ) const {
    /// get the UnicodeString text value of underlying elements
    /*!
     * \param tp the TextPolicy to use
     * \return The Unicode Text found.
     * Will throw on error.
     */
#ifdef DEBUG_TEXT
    cerr << "deeptext, policy: " << tp << ", on node : " << xmltag() << " id=" << id() << ", cls=" << this->cls() << ")" << endl;
    cerr << "deeptext: node has " << _data.size() << " children." << endl;
#endif
    vector<UnicodeString> parts;
    vector<UnicodeString> seps;
    for ( const auto& child : this->data() ) {
      // try to get text dynamically from printable children
      // skipping the TextContent elements
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
	  UnicodeString tmp = child->text( tp );
#ifdef DEBUG_TEXT
	  cerr << "deeptext found '" << tmp << "'" << endl;
#endif
	  parts.push_back(tmp);
	  if ( child->isinstance( Sentence_t )
	       && no_space_at_end(child) ){
	    const string& delim = "";
#ifdef DEBUG_TEXT
	    cerr << "deeptext: no delimiter van "<< child->xmltag() << " on"
		 << " last w of s" << endl;
#endif
	    seps.push_back(TiCC::UnicodeFromUTF8(delim));
	  }
	  else {
	    // get the delimiter
	    bool retain = tp.is_set( TEXT_FLAGS::RETAIN );
	    const string& delim = child->get_delimiter( retain );
#ifdef DEBUG_TEXT
	    cerr << "deeptext:delimiter van "<< child->xmltag() << " ='" << delim << "'" << endl;
#endif
	    seps.push_back(TiCC::UnicodeFromUTF8(delim));
	  }
	} catch ( const NoSuchText& e ) {
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
      // so no deeper text is found. Well, lets look here then
      bool hidden = tp.is_set( TEXT_FLAGS::HIDDEN );
      result = text_content(tp._class,hidden)->text(tp);
    }
#ifdef DEBUG_TEXT
    cerr << "deeptext() for " << xmltag() << " result= '" << result << "'" << endl;
#endif
    if ( result.isEmpty() ) {
      throw NoSuchText( xmltag() + ":(class=" + tp._class +"): empty!" );
    }
    return result;
  }

  const UnicodeString FoliaElement::stricttext( const string& cls ) const {
    /// get the UnicodeString value of TextContent children only
    /*!
     * \param cls the textclass
     * \return The Unicode Text found.
     * Will throw on error.
     */
    TEXT_FLAGS flags = TEXT_FLAGS::STRICT;
    return this->text(cls, flags );
  }

  const UnicodeString FoliaElement::toktext( const string& cls ) const {
    /// get the UnicodeString value of TextContent children only, retaining
    /// tokenization
    /*!
     * \param cls the textclass
     * \return The Unicode Text found.
     * Will throw on error.
     */
    TEXT_FLAGS flags = TEXT_FLAGS::RETAIN;
    return this->text(cls, flags );
  }

  const TextContent *AbstractElement::text_content( const string& cls,
						    bool show_hidden ) const {
    /// Get the TextContent explicitly associated with this element.
    /*!
     * \param cls the textclass to search for
     * \param show_hidden if true also return text of 'hidden' nodes
     *
     * Returns the TextContent instance rather than the actual text.
     * (so it might return itself.. ;)
     * Does not recurse into children with the sole exception of Correction
     * might throw NoSuchText exception if not found.
     */

#ifdef DEBUG_TEXT
    cerr << "text_content(" << cls << "," << (show_hidden?"show_hidden":"")
	 << ")" << endl;
#endif
    if ( isinstance(TextContent_t) ){
#ifdef DEBUG_TEXT
      cerr << "A textcontent!!" << endl;
#endif
      if  ( this->cls() == cls ) {
#ifdef DEBUG_TEXT
	cerr << "return myself..." << endl;
#endif
	return dynamic_cast<const TextContent*>(this);
      }
      else {
	throw NoSuchText( "TextContent::text_content(" + cls + ")" );
      }
    }
#ifdef DEBUG_TEXT
    cerr << (!printable()?"NOT":"") << " printable: " << xmltag() << endl;
    cerr << (!hidden()?"NOT":"") << " hidden: " << xmltag() << endl;
#endif
    if ( !printable() || ( hidden() && !show_hidden ) ) {
      throw NoSuchText( "non-printable element: " +  xmltag() );
    }
#ifdef DEBUG_TEXT
    cerr << "recurse into children...." << endl;
#endif
    for ( const auto& el : data() ) {
      if ( el->isinstance(TextContent_t) && (el->cls() == cls) ) {
	return dynamic_cast<TextContent*>(el);
      }
      else if ( el->element_id() == Correction_t) {
	try {
	  return el->text_content(cls,show_hidden);
	} catch ( const NoSuchText& e ) {
	  // continue search for other Corrections or a TextContent
	}
      }
    }
    throw NoSuchText( xmltag() + "::text_content(" + cls + ")" );
  }

  const PhonContent *AbstractElement::phon_content( const string& cls,
						    bool show_hidden ) const {
    /// Get the PhonContent explicitly associated with this element.
    /*!
     * \param cls the textclass to search for
     * \param show_hidden if true also return text og 'hidden' nodes
     *
     * Returns the PhonContent instance rather than the actual text.
     * (so it might return iself.. ;)
     * Does not recurse into children with the sole exception of Correction
     * might throw NoSuchPhon exception if not found.
     */
    if ( isinstance(PhonContent_t) ){
      if  ( this->cls() == cls ) {
	return dynamic_cast<const PhonContent*>(this);
      }
      else {
	throw NoSuchPhon( xmltag() + "::phon_content(" + cls + ")" );
      }
    }
    if ( !speakable() || ( hidden() && !show_hidden ) ) {
      throw NoSuchPhon( "non-speakable element: " + xmltag() );
    }

    for ( const auto& el : _data ) {
      if ( el->isinstance(PhonContent_t) && ( el->cls() == cls) ) {
	return dynamic_cast<PhonContent*>(el);
      }
      else if ( el->element_id() == Correction_t) {
	try {
	  return el->phon_content(cls,show_hidden);
	} catch ( const NoSuchPhon& e ) {
	  // continue search for other Corrections or a TextContent
	}
      }
    }
    throw NoSuchPhon( xmltag() + "::phon_content(" + cls + ")" );
  }

  //#define DEBUG_PHON

  const UnicodeString AbstractElement::phon( const string& cls,
					     TEXT_FLAGS flags ) const {
    /// get the UnicodeString phon value of an element
    /*!
     * \param cls the textclass the text should be in
     * \param flags the search parameters to use. See TEXT_FLAGS.
     */
    bool hidden = ( TEXT_FLAGS::HIDDEN & flags ) == TEXT_FLAGS::HIDDEN;
    bool strict = ( TEXT_FLAGS::STRICT & flags ) == TEXT_FLAGS::STRICT;
#ifdef DEBUG_PHON
    cerr << "PHON(" << cls << ") on node : " << xmltag() << " id=" << id() << endl;
#endif
    if ( strict ) {
      return phon_content(cls)->phon();
    }
    else if ( !speakable() || ( this->hidden() && !hidden ) ) {
      throw NoSuchPhon( "NON speakable element: " + xmltag() );
    }
    else {
      UnicodeString result = deepphon( cls, flags );
      if ( result.isEmpty() ) {
	result = phon_content(cls,hidden)->phon();
      }
      if ( result.isEmpty() ) {
	throw NoSuchPhon( "on tag " + xmltag() + " nor it's children" );
      }
      return result;
    }
  }

  const UnicodeString AbstractElement::deepphon( const string& cls,
						 TEXT_FLAGS flags ) const {
    /// get the UnicodeString phon value of underlying elements
    /*!
     * \param cls the textclass
     * \param flags the search parameters to use
     * \return The Unicode Text found.
     * Will throw on error.
     */
#ifdef DEBUG_PHON
    cerr << "deepPHON(" << cls << ") on node : " << xmltag()
	 << " id=" << id() << endl;
    cerr << "deepphon: node has " << _data.size() << " children." << endl;
#endif
    vector<UnicodeString> parts;
    vector<UnicodeString> seps;
    for ( const auto& child : _data ) {
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
	  UnicodeString tmp = child->phon( cls );
#ifdef DEBUG_PHON
	  cerr << "deepphon found '" << tmp << "'" << endl;
#endif
	  parts.push_back(tmp);
	  // get the delimiter
	  const string& delim = child->get_delimiter();
#ifdef DEBUG_PHON
	  cerr << "deepphon:delimiter van "<< child->xmltag() << " ='" << delim << "'" << endl;
#endif
	  seps.push_back(TiCC::UnicodeFromUTF8(delim));
	} catch ( const NoSuchPhon& e ) {
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
      bool hidden = ( TEXT_FLAGS::HIDDEN & flags ) == TEXT_FLAGS::HIDDEN;
      try {
	result = phon_content(cls,hidden)->phon();
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


  vector<FoliaElement *>AbstractElement::find_replacables( FoliaElement *par ) const {
    // find all children with the same signature as the parameter
    /*!
     * \param par the FoliaElement to search
     * \return a vector of matching elements
     * search in the DIRECT children for nodes with the same tag AND set
     * as the element par
     */
    return par->select( element_id(), sett(), SELECT_FLAGS::LOCAL );
  }

  void AbstractElement::replace( FoliaElement *child ) {
    /// replace a child element
    /*!
     * \param child Teh element to substitute
     * This function searches for A child of the same signature (type and set)
     * If found, that child is replaced.
     * If no such child element exists, this will act the same as append()
     */
    vector<FoliaElement*> replace = child->find_replacables( this );
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

  FoliaElement* AbstractElement::replace( FoliaElement *old,
					  FoliaElement* _new ) {
    /// replace in the children old by _new
    /*!
     * \param old The node to be replacec
     * \param _new the new node to add
     * \return old
     * First old is looked up, if present it is replaced
     *
     * when not found this function does nothing and returns 0
     */
    auto it = find_if( _data.begin(),
		       _data.end(),
		       [&]( FoliaElement *el ){ return el == old; } );
    if ( it != _data.end() ){
      *it = _new;
      _new->set_parent(this);
    }
    return 0;
  }

  void AbstractElement::insert_after( FoliaElement *pos, FoliaElement *add ){
    /// append a node after a certain element
    /*!
     * \param pos The location after which to insert add
     * \param add the element to add
     *
     * throws when pos is not found
     */
    auto it = _data.begin();
    while ( it != _data.end() ) {
      if ( *it == pos ) {
	it = _data.insert( ++it, add );
	break;
      }
      ++it;
    }
    if ( it == _data.end() ) {
      throw runtime_error( "insert_after(): previous not found" );
    }
  }

  void FoliaElement::clear_textcontent( const string& textclass ){
    for ( size_t i=0; i < size(); ++i ){
      FoliaElement *p = index(i);
      if ( p->element_id() == TextContent_t ) {
	if ( p->cls() == textclass ){
	  this->remove(p,true);
	  break;
	}
      }
    }
  }

  TextContent *FoliaElement::settext( const string& txt,
				      const string& cls ){
    /// append a TextContent child of class txt with value txt
    /*!
     * \param txt the UTF8 text value
     * \param cls the textclass of the new TextContent
     * \return the new created TextContent
     * may throw on error
     *
     * when the associated document has the checktext mode, (which is the
     * default) both text consistency and the offset are checked.
     */
    return settext( txt, -1, cls );
  }

  TextContent *FoliaElement::setutext( const UnicodeString& txt,
				       const string& cls ){
    /// append a TextContent child of class cls with value txt
    /*!
     * \param txt the Unicode text value
     * \param cls the textclass of the new TextContent
     * \return the new created TextContent
     * may throw on error
     *
     * when the associated document has the checktext mode, (which is the
     * default) both text consistency and the offset are checked.
     */
    string utf8 = TiCC::UnicodeToUTF8(txt);
    return settext( utf8, cls );
  }

  TextContent *FoliaElement::settext( const string& txt,
				      int offset,
				      const string& cls ){
    /// append a TextContent child of class cls with value txt
    /*!
     * \param txt the UTF8 text value
     * \param offset offset of the text in the text of the parent,
              when offset < 0 it is ignored.
     * \param cls the textclass of the new TextContent
     * \return the new created TextContent
     * may throw on error
     *
     * when the associated document has the checktext mode, (which is the
     * default) both text consistency and the offset are checked.
     */
    UnicodeString txt_u = TiCC::UnicodeFromUTF8( txt );
    if ( doc() && doc()->checktext()
	 && !isSubClass( Morpheme_t ) && !isSubClass( Phoneme_t) ){
      UnicodeString deeper_u;
      try {
	deeper_u = text( cls );
	// get deep original text: no retain tokenization, no strict
      }
      catch (...){
      }
      deeper_u = normalize_spaces( deeper_u );
      txt_u = strip_control_chars( txt_u );
      UnicodeString txt_check_u = normalize_spaces( txt_u );
      if ( !deeper_u.isEmpty() && txt_check_u != deeper_u ){
	throw InconsistentText( "settext(cls=" + cls + "): deeper text differs from attempted\ndeeper='" + TiCC::UnicodeToUTF8(deeper_u) + "'\nattempted='" + TiCC::UnicodeToUTF8(txt_u) + "'" );
      }
    }
    KWargs args;
    args["value"] = TiCC::UnicodeToUTF8(txt_u);
    args["class"] = cls;
    if ( offset >= 0 ){
      args["offset"] = TiCC::toString(offset);
    }
    TextContent *node = new TextContent( args, doc() );
    replace( node );
    return node;
  }

  TextContent *FoliaElement::setutext( const UnicodeString& txt,
				       int offset,
				       const string& cls ){
    /// append a TextContent child of class cls with value txt
    /*!
     * \param txt the Unicode text value
     * \param offset offset of the text in the text of the parent,
              when offset < 0 it is ignored.
     * \param cls the textclass of the new TextContent
     * \return the new created TextContent
     * may throw on error
     *
     * when the associated document has the checktext mode, (which is the
     * default) both text consistency and the offset are checked.
     */
    string utf8 = TiCC::UnicodeToUTF8(txt);
    return settext( utf8, offset, cls );
  }

  const string FoliaElement::description() const {
    /// return the string value of the description tag (if present)
    /*!
     * \return a string
     * search for Description nodes in this object.
     * When 1 or more are found, return the value of the first one
     */
    vector<FoliaElement *> v = select( Description_t, SELECT_FLAGS::LOCAL );
    if ( v.size() == 0 ) {
      return "";
    }
    return v[0]->description();
  }

  bool AbstractElement::acceptable( ElementType t ) const {
    /// test if this ElementType is acceptable for the current node
    /*!
     * \param t the ElementType to test
     *
     * This function tests if t is in the accepted_data list of the node
     * OR if it is a SubClass of one of the accepted types
     */

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

  bool AbstractElement::addable( const FoliaElement *c ) const {
    /// test if an element \c might succesfully appended
    /*!
     * \param c the node to check
     * \return true if it doesn't throw
     *
     * \note It will allways throw an error, instead of returning false
     */
    if ( !acceptable( c->element_id() ) ) {
      string mess = "Unable to append object of type " + c->classname()
	+ " to a <" + classname() + ">";
      if ( !_id.empty() ){
	mess += " (id=" + _id + ")";
      }
      throw ValueError( mess );
    }
    if ( c->occurrences() > 0 ) {
      vector<FoliaElement*> v = select( c->element_id(), SELECT_FLAGS::LOCAL );
      size_t count = v.size();
      if ( count >= c->occurrences() ) {
	throw DuplicateAnnotationError( "Unable to add another object of type " + c->classname() + " to " + classname() + ". There are already " + TiCC::toString(count) + " instances of this type, which is the maximum." );
      }
    }
    if ( c->occurrences_per_set() > 0 &&
	 (CLASS & c->required_attributes() || c->setonly() ) ){
      vector<FoliaElement*> v = select( c->element_id(), c->sett(), SELECT_FLAGS::LOCAL );
      size_t count = v.size();
      if ( count >= c->occurrences_per_set() ) {
	throw DuplicateAnnotationError( "Unable to add another object of type " + c->classname() + " to " + classname() + ". There are already " + TiCC::toString(count) + " instances of this type and set (" + c->sett() + "), which is the maximum." );
      }
    }
    if ( c->parent() &&
	 !( c->element_id() == WordReference_t
	    || c->referable() ) ){
      throw XmlError( "attempt to reconnect node " + c->classname() + "("
		      + c->id()
		      + ") to a " + classname() + " node, id=" + _id
		      + ", it was already connected to a "
		      +  c->parent()->classname() + " id=" + c->parent()->id() );
    }
#ifdef NOT_WORKING
    // this fails. needs attention
    if ( c->element_id() == WordReference_t ){
      string tval = atts["t"];
      if ( !tval.empty() ){
      	string tc = ref->textclass();
      	string rtval = ref->str(tc);
      	if ( tval != rtval ){
      	  throw XmlError( "WordReference id=" + id + " has another value for "
      			  + "the t attribute than it's reference. ("
      			  + tval + " versus " + rtval + ")" );
      	}
      }
    }
#endif
    if ( c->element_id() == TextContent_t
	 && element_id() == Word_t ) {
      string val = c->str(c->cls());
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
    }
    if ( c->is_textcontainer() ||
	 c->element_id() == Word_t ){
      check_append_text_consistency( c );
    }
    return true;
  }

  void AbstractElement::assignDoc( Document* the_doc ) {
    /// attach a document-less FoliaElement (-tree) to a Document the_doc
    /*!
     * \param the_doc The Document to attach to
     *
     * if the node already has a Document assigned , nothing is done.
     *
     * Otherwise: The annotation type is checked. If not set yet and
     * the doc has autodeclare mode set, it is attempted to do so.
     *
     * Also the ID is registered in the_doc.
     *
     * Finaly, all children are also assigned to the_doc
     */
    if ( !_mydoc ) {
      _mydoc = the_doc;
      if ( annotation_type() != AnnotationType::NO_ANN
	   && !the_doc->version_below( 2, 0 )
	   && the_doc->is_undeclared( annotation_type() ) ){
	// cerr << "assignDoc: " << this << endl;
	// cerr << "ant: " << annotation_type() << endl;
	// cerr << "set: " << _set << endl;
	// so when appending a document-less child, make sure that
	// an annotation declaration is present or added.
	if ( doc()->autodeclare() ){
	  doc()->auto_declare( annotation_type(), _set );
	}
	else {
	  throw DeclarationError( "Encountered an instance of <"
				  + xmltag()
				  + "> without a proper declaration for "
				  + toString(annotation_type())
				  + "-annotation" );
	}
      }
      string myid = id();
      if ( !_set.empty()
	   && (CLASS & required_attributes() )
	   && !_mydoc->declared( annotation_type(), _set ) ) {
	throw DeclarationError( "Set " + _set + " is used in " + xmltag()
			  + "element: " + myid + " but has no declaration " +
			  "for " + toString( annotation_type() ) + "-annotation" );
      }
      if ( !myid.empty() ) {
	_mydoc->add_doc_index( this, myid );
      }
      // assume that children also might be doc-less
      for ( const auto& el : _data ) {
	el->assignDoc( _mydoc );
      }
    }
  }

  bool AbstractElement::checkAtts() {
    /// check if all the REQUIRED attributes of the node are set
    /*!
     * \return true, or throws
     */
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
    if ( _metadata.empty()
	 && ( METADATA & required_attributes() ) ) {
      throw ValueError( "attribute 'metadata' is required for " + classname() );
    }
    if ( _speaker.empty()
	 && ( SPEAKER & required_attributes() ) ) {
      throw ValueError( "attribute 'speaker' is required for " + classname() );
    }
    return true;
  }

  FoliaElement *AbstractElement::append( FoliaElement *child ){
    /// append child to this node
    /*!
     * \param child the node to add
     * \return the appended child
     *
     * will throw on error
     */
    if ( !child ){
      throw XmlError( "attempt to append an empty node to a " + classname() );
    }
    bool ok = false;
    try {
      ok = child->checkAtts();
      ok &= addable( child );
    }
    catch ( const XmlError& ) {
      // don't delete the offending child in case of illegal reconnection
      // it will be deleted by the true parent
      throw;
    }
    catch ( const exception& ) {
      delete child;
      throw;
    }
    if ( ok ) {
      if ( doc() ){
	child->assignDoc( doc() );
      }
      _data.push_back(child);
      if ( !child->parent() ) {
	child->set_parent(this);
      }
      if ( child->referable() ){
	child->increfcount();
      }
      if ( child->preserve_spaces() == SPACE_FLAGS::UNSET ){
	child->preserve_spaces( _preserve_spaces );
      }
      return child->postappend();
    }
    return 0;
  }

  FoliaElement *AbstractElement::postappend( ) {
    /// perform some post correction after appending
    if ( id().empty() && (ID & required_attributes()) && auto_generate_id() ){
      _id = generateId( xmltag() );
    }
    return this;
  }

  FoliaElement *TextContent::postappend( ) {
    /// perform some extra checks after appending a TextContent
    if ( doc() ){
      if ( doc()->checktext()
	   && _offset != -1
	   && ( parent() && parent()->auth() ) ){
	doc()->cache_textcontent(this);
      }
      if ( !doc()->declared( AnnotationType::TEXT ) ){
	doc()->declare( AnnotationType::TEXT, DEFAULT_TEXT_SET );
      }
    }
    return this;
  }

  FoliaElement *PhonContent::postappend( ) {
    /// perform some extra checks after appending a PhonContent
    if ( doc() ){
      if ( doc()->checktext() && _offset != -1 ){
	doc()->cache_phoncontent(this);
      }
      if ( !doc()->declared( AnnotationType::PHON ) ){
	doc()->declare( AnnotationType::PHON, DEFAULT_PHON_SET );
      }
    }
    return this;
  }

  void AbstractElement::remove( FoliaElement *child, bool del ) {
    /// remove a child from a node
    /*!
     * \param child the element to remove
     * \param del If true, really delete the child
     */
    auto it = std::remove( _data.begin(), _data.end(), child );
    _data.erase( it, _data.end() );
    if ( del ) {
      if ( child->refcount() > 0 ){
	// dont really delete yet!
	doc()->keepForDeletion( child );
      }
      else {
	delete child;
      }
    }
    else {
      child->set_parent(0);
    }
  }

  void AbstractElement::remove( size_t pos, bool del ) {
    /// remove a child from a node
    /*!
     * \param pos the index of the element to remove
     * \param del If true, really delete the child
     */
    if ( pos < _data.size() ) {
      auto it = _data.begin();
      while ( pos > 0 ) {
	++it;
	--pos;
      }
      if ( del ) {
	if ( (*it)->refcount() > 0 ){
	  // dont really delete yet!
	  doc()->keepForDeletion( *it );
	}
	else {
	  delete *it;
	}
      }
      else {
	(*it)->set_parent(0);
      }
      _data.erase(it);
    }
  }

  FoliaElement* AbstractElement::index( size_t i ) const {
    /// return the child at index i
    /*!
     * \param i the index
     * \return the child at index i
     *
     * Will throw when the index is out of range
     */
    if ( i < _data.size() ) {
      return _data[i];
    }
    throw range_error( "[] index out of range" );
  }

  FoliaElement* AbstractElement::rindex( size_t ri ) const {
    /// return the child at reversed index ri
    /*!
     * \param ri the index
     * \return the child at index ri
     *
     * Will throw when the index is out of range
     */
    if ( ri < _data.size() ) {
      return _data[_data.size()-1-ri];
    }
    throw range_error( "[] rindex out of range" );
  }

  vector<FoliaElement*> AbstractElement::select( ElementType et,
						 const string& st,
						 const set<ElementType>& exclude,
						 SELECT_FLAGS flag ) const {
    /// The generic 'select()' function on which all other variants are based
    ///   it searches a FoLiA node for matchins sibblings.
    /*!
     * \param et which type of element we are looking for
     * \param st when not empty ("") we also must match on the 'sett' of the nodes
     * \param exclude a set of ElementType to exclude from searching.
     * These are skipped, and NOT recursed into.
     * \param flag determines special search stategies:
     *     - RECURSE : recurse the whole FoLia from the given node downwards
     *                 returning all matching nodes, even within matches
     *                 This is the default.
     *     - LOCAL   : just look at the direct sibblings of the node
     *     - TOP_HIT : like recurse, but do NOT recurse into sibblings
     *               of matching node
     */
    vector<FoliaElement*> res;
    for ( const auto& el : _data ) {
      if ( el->element_id() == et &&
	   ( st.empty() || el->sett() == st ) ) {
	res.push_back( el );
	if ( flag == SELECT_FLAGS::TOP_HIT ){
	  flag = SELECT_FLAGS::LOCAL;
	}
      }
      if ( flag != SELECT_FLAGS::LOCAL ){
	// not at this level, search deeper when recurse is true
	if ( exclude.find( el->element_id() ) == exclude.end() ) {
	  vector<FoliaElement*> tmp = el->select( et, st, exclude, flag );
	  res.insert( res.end(), tmp.begin(), tmp.end() );
	}
      }
    }
    return res;
  }

  vector<FoliaElement*> AbstractElement::select( ElementType et,
						 const string& st,
						 SELECT_FLAGS flag ) const {
    /// wrapper around the the generic select()
    /*!
     * calls select() with a default ignore set.
     */
    return select( et, st, default_ignore, flag );
  }

  vector<FoliaElement*> AbstractElement::select( ElementType et,
						 const set<ElementType>& exclude,
						 SELECT_FLAGS flag ) const {
    /// wrapper around the the generic select()
    /*!
     * calls select() with a default setname.
     */
    return select( et, "", exclude, flag );
  }

  vector<FoliaElement*> AbstractElement::select( ElementType et,
						 SELECT_FLAGS flag ) const {
    /// wrapper around the the generic select()
    /*!
     * calls select() with a default setname and the default ignore set
     */
    return select( et, "", default_ignore, flag );
  }

  void AbstractElement::unravel( set<FoliaElement*>& store ){
    /// split the node and all siblings into a set of nodes
    /*!
     * \param store
     * recursively go throuhg this node and its children an collect all
     * node pointers in store.
     * Erase the _data array of every node
     *
     * This function is used when erasing a document. Creating a set avoids
     * deleting nodes twice
     */
    resetrefcount();
    store.insert( this );
    auto dit = _data.begin();
    while ( dit != _data.end() ){
      (*dit)->unravel( store );
      dit = _data.erase(dit);
    }
  }

  FoliaElement* AbstractElement::parseXml( const xmlNode *node ) {
    /// recursively parse a FoLiA tree starting at node
    /*!
     * \param node an xmlNode representing a FoLiA subtree
     * \return the parsed tree. Throws on error.
     */
    KWargs att = getAttributes( node );
    int sp = xmlNodeGetSpacePreserve(node);
    if ( sp == 1 ){
      att["xml:space"] = "preserve";
    }
    else if ( sp == 0 ){
      att["xml:space"] = "default";
    }

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
	else if ( doc() && !doc()->permissive() ){
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
      else if ( p->type == XML_ENTITY_REF_NODE ){
	XmlText *t = new XmlText();
	string txt = TextValue( p );
	t->setvalue( txt );
	append( t );
	if ( doc() && doc()->debug > 2 ) {
	  cerr << "created " << t << "(" << t->text() << ")" << endl;
	  cerr << "extended " << this << " met " << t << endl;
	  cerr << "this.size()= " << size() << " t.size()=" << t->size() << endl;
	}
      }
      else if ( p->type == XML_TEXT_NODE ){
	if ( this->is_textcontainer()
	     || this->is_phoncontainer() ){
	  // non empty text is allowed (or even required) here
	  string txt = TextValue( p );
	  if ( !txt.empty() ) {
	    XmlText *t = new XmlText();
	    t->setvalue( txt );
	    append( t );
	    if ( doc() && doc()->debug > 2 ) {
	      cerr << "created " << t << "(" << t->text() << ")" << endl;
	      cerr << "extended " << this << " met " << t << endl;
	      cerr << "this.size()= " << size() << " t.size()=" << t->size() << endl;
	    }
	  }
	}
	else {
	  // This MUST be 'empty space', so only spaces and tabs formatting
	  string tag = "_XmlText";
	  FoliaElement *t = createElement( tag, doc() );
	  if ( t ) {
	    if ( doc() && doc()->debug > 2 ) {
	      cerr << "created " << t << endl;
	    }
	    try {
	      t = t->parseXml( p );
	    }
	    catch ( const ValueError& e ){
	      delete t;
	      t = 0;
	    }
	  }
	  if ( t ) {
	    if ( doc() && doc()->debug > 2 ) {
	      cerr << "extend " << this << " met " << t << endl;
	    }
	    string txt = t->str();
	    txt = TiCC::trim(txt);
	    if ( !txt.empty() ){
	      if ( p->prev ){
		string tg = "<" + Name(p->prev) + ">";
		throw XmlError( "found extra text '" + txt + "' after element "
				+ tg + ", NOT allowed there." );
	      }
	      else {
		string tg = "<" + Name(p->parent) + ">";
		throw XmlError( "found extra text '" + txt + "' inside element "
				+ tg + ", NOT allowed there." );
	      }
	    }
	    append( t );
	  }
	}
      }
      p = p->next;
    }
    if ( doc() && ( doc()->checktext() || doc()->fixtext() )
	 && this->printable()
	 && !isSubClass( Morpheme_t ) && !isSubClass( Phoneme_t) ){
      check_text_consistency_while_parsing();
    }
    return this;
  }

  void AbstractElement::setDateTime( const string& s ) {
    /// set the DATETIME value of a node
    /*!
     * \param s a date/time in ISO.... format. (YYYY-MM-DDThh:mm:ss)
     */
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

  const string AbstractElement::getDateTime() const {
    /// return the _datetime value
    return _datetime;
  }

  const string AbstractWord::pos( const string& st ) const {
    /// return the POS tag value of the node
    /*!
     * \param st the annotation setname to search
     * \return the POS tag as a string
     *  will throw if the PosAnnotation doesn't exist
     */
    return annotation<PosAnnotation>( st )->cls();
  }

  const string AbstractWord::lemma( const string& st ) const {
    /// return the LEMMA value of the node
    /*!
     * \param st the annotation setname to search
     * \return the lemma as a string
     *  will throw if the LemmaAnnotation doesn't exist
     */
    return annotation<LemmaAnnotation>( st )->cls();
  }

  PosAnnotation *AllowInlineAnnotation::addPosAnnotation( const KWargs& inargs ) {
    /// add a PosAnnotation node given the parameters
    /*!
     * \param inargs A list of Attribute-Value pairs
     * \return the created PosAnnotation node
     *
     * when the *this node already has a PosAnnotation in the specified set,
     * an ALTERNATIVE node is added
     */
    KWargs args = inargs;
    string st;
    auto it = args.find("set" );
    if ( it != args.end() ) {
      st = it->second;
    }
    string newId = args.extract("generate_id" );
    if ( newId.empty() ){
      newId = "alt-pos";
    }
    if ( has_annotation<PosAnnotation>( st ) > 0 ) {
      // ok, there is already one, so create an Alternative
      KWargs kw;
      kw["xml:id"] = generateId( newId );
      if ( !doc()->declared( AnnotationType::ALTERNATIVE ) ){
	doc()->declare( AnnotationType::ALTERNATIVE, "" );
      }
      Alternative *alt = new Alternative( kw, doc() );
      append( alt );
      return alt->addAnnotation<PosAnnotation>( args );
    }
    else {
      return addAnnotation<PosAnnotation>( args );
    }
  }

  PosAnnotation* AllowInlineAnnotation::getPosAnnotations( const string& st,
							   vector<PosAnnotation*>& alts ) const {
    /// return the PosAnnotation AND all alternatives
    /*!
     * \param st the annotation set
     * \param alts all the alternatives in set st
     * \return the PosAnnotation in set st
     *
     * \note The return value may be 0, even when there ARE alternatives!
     */
    PosAnnotation *res = annotation<PosAnnotation>( st ); // may be 0
    alts.clear();
    // now search for alternatives
    vector<Alternative *> alt_nodes = select<Alternative>( AnnoExcludeSet );
    for ( const auto& alt : alt_nodes ){
      if ( alt->size() > 0 ) { // child elements?
	for ( size_t j=0; j < alt->size(); ++j ) {
	  if ( alt->index(j)->element_id() == PosAnnotation_t &&
	       ( st.empty() || alt->index(j)->sett() == st ) ) {
	    alts.push_back( dynamic_cast<PosAnnotation*>(alt->index(j)) );
	  }
	}
      }
    }
    return res;
  }

  LemmaAnnotation *AllowInlineAnnotation::addLemmaAnnotation( const KWargs& inargs ) {
    /// add a LemmaAnnotation node given the parameters
    /*!
     * \param inargs A list of Attribute-Value pairs
     * \return the created LemmaAnnotation node
     *
     * when the *this node already has a LemmaAnnotation in the specified set,
     * an ALTERNATIVE node is added
     */
    KWargs args = inargs;
    string st;
    auto it = args.find("set" );
    if ( it != args.end() ) {
      st = it->second;
    }
    string newId = args.extract("generate_id" );
    if ( newId.empty() ){
      newId = "alt-lem";
    }
    if ( has_annotation<LemmaAnnotation>( st ) > 0 ) {
      // ok, there is already one, so create an Alternative
      KWargs kw;
      kw["xml:id"] = generateId( newId );
      if ( !doc()->declared( AnnotationType::ALTERNATIVE ) ){
	doc()->declare( AnnotationType::ALTERNATIVE, "" );
      }
      Alternative *alt = new Alternative( kw, doc() );
      append( alt );
      return alt->addAnnotation<LemmaAnnotation>( args );
    }
    else {
      return addAnnotation<LemmaAnnotation>( args );
    }
  }

  LemmaAnnotation* AllowInlineAnnotation::getLemmaAnnotations( const string& st,
							       vector<LemmaAnnotation*>& alts ) const {
    /// return the LemmaAnnotation AND all alternatives
    /*!
     * \param st the annotation set
     * \param alts all the alternatives in set st
     * \return the LemmaAnnotation in set st
     *
     * \note The return value may be 0, even when there ARE alternatives!
     */
    alts.clear();
    LemmaAnnotation *res = annotation<LemmaAnnotation>( st ); // may be 0 !
    // also search alternatives
    vector<Alternative *> alt_nodes = select<Alternative>( AnnoExcludeSet );
    for ( const auto& alt : alt_nodes ){
      if ( alt->size() > 0 ) { // child elements?
	for ( size_t j =0; j < alt->size(); ++j ) {
	  if ( alt->index(j)->element_id() == LemmaAnnotation_t &&
	       ( st.empty() || alt->index(j)->sett() == st ) ) {
	    alts.push_back( dynamic_cast<LemmaAnnotation*>(alt->index(j)) );
	  }
	}
      }
    }
    return res;
  }

  MorphologyLayer *Word::addMorphologyLayer( const KWargs& inargs ) {
    /// add a MorphologyLayer node given the parameters
    /*!
     * \param inargs A list of Attribute-Value pairs
     * \return the created Morphologylayer
     *
     * when the *this node already has a MorphologyLayer in the specified set,
     * an ALTERNATIVE node is added
     */
    KWargs args = inargs;
    string st;
    auto it = args.find("set" );
    if ( it != args.end() ) {
      st = it->second;
    }
    string newId = args.extract("generate_id" );
    if ( newId.empty() ){
      newId = "alt-mor";
    }
    if ( has_annotation<MorphologyLayer>( st ) > 0 ) {
      // ok, there is already one, so create an Alternative
      KWargs kw;
      kw["xml:id"] = generateId( newId );
      if ( !doc()->declared( AnnotationType::ALTERNATIVE ) ){
	doc()->declare( AnnotationType::ALTERNATIVE, "" );
      }
      Alternative *alt = new Alternative( kw, doc() );
      append( alt );
      return alt->addAnnotation<MorphologyLayer>( args );
    }
    else {
      return addAnnotation<MorphologyLayer>( args );
    }
  }

  MorphologyLayer *Word::getMorphologyLayers( const string& st,
					      vector<MorphologyLayer*>& alts ) const {
    /// return the MorhologyLayer AND all alternatives
    /*!
     * \param st the annotation set
     * \param alts all the alternatives in set st
     * \return the MorphologyLayer in set st
     *
     * \note The return value may be 0, even when there ARE alternatives!
     */
    alts.clear();
    MorphologyLayer *res = annotation<MorphologyLayer>( st ); // may be 0
    // now search for alternatives
    vector<Alternative *> alt_nodes = select<Alternative>( AnnoExcludeSet );
    for ( const auto& alt : alt_nodes ){
      if ( alt->size() > 0 ) { // child elements?
	for ( size_t j =0; j < alt->size(); ++j ) {
	  if ( alt->index(j)->element_id() == MorphologyLayer_t &&
	       ( st.empty() || alt->index(j)->sett() == st ) ) {
	    alts.push_back( dynamic_cast<MorphologyLayer*>(alt->index(j)) );
	  }
	}
      }
    }
    return res;
  }

  Sentence *AbstractElement::addSentence( const KWargs& in_args ) {
    /// add a Sentence node given the parameters
    /*!
     * \param in_args A list of Attribute-Value pairs
     * \return the created Sentence
     * may throw when the 'xml:id' is nor unique
     */
    Sentence *res = 0;
    KWargs kw = in_args;
    if ( !kw.is_present("xml:id") ){
      string id = generateId( "s" );
      kw["xml:id"] = id;
    }
    try {
      res = new Sentence( kw, doc() );
    }
    catch( const DuplicateIDError& e ) {
      delete res;
      throw;
    }
    append( res );
    return res;
  }

  Word *AbstractElement::addWord( const KWargs& in_args ) {
    /// add a Word node given the parameters
    /*!
     * \param in_args A list of Attribute-Value pairs
     * \return the created Word
     * may throw when the 'xml:id' is nor unique, or when appending fails
     */
    Word *res = new Word( doc() );
    KWargs kw = in_args;
    if ( !kw.is_present("xml:id") ){
      string id = generateId( "w" );
      kw["xml:id"] = id;
    }
    try {
      res->setAttributes( kw );
    }
    catch( const DuplicateIDError& e ) {
      delete res;
      throw;
    }
    append( res );
    return res;
  }

  Word *AbstractElement::addWord( const std::string& s ){
    /// add a Word given the string s
    /*!
      \param s a string with text OR an encode KWargs list
      \return a new Word

      If the string is a KWargs properties list it is used to create a word
      with those properties. Otherwise it is assumed that \em s represents
      the text value for the Word
    */
    KWargs args = getArgs(s);
    if ( args.empty() ){
      args["text"] = s;
    }
    return addWord( args );
  }

  const string& Quote::get_delimiter( bool retaintok ) const {
    /// get the default delimiter of a Quote object.
    /*!
     * \param retaintok retain the tokenization assigned to this element
     * \return a string representing the delimiter
     * When the last data item in the Quote is a sentence, we don't
     * want a delimiter and return ""
     */
#ifdef DEBUG_TEXT_DEL
    cerr << "IN " << xmltag() << "::get_delimiter (" << retaintok << ")" << endl;
#endif
    const vector<FoliaElement*>& data = this->data();
    auto it = data.rbegin();
    while ( it != data.rend() ) {
      if ( (*it)->isinstance( Sentence_t ) ) {
	// if a quote ends in a sentence, we don't want any delimiter
#ifdef DEBUG_TEXT_DEL
	cerr << "OUT " << xmltag() << "::get_delimiter ==>''" << endl;
#endif
	return EMPTY_STRING;
      }
      else {
	const string& res = (*it)->get_delimiter( retaintok );
#ifdef DEBUG_TEXT_DEL
	cerr << "OUT " << xmltag() << "::get_delimiter ==> '"
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
    for ( const auto& pnt : data() ) {
      if ( pnt->isinstance( Word_t ) ) {
	result.push_back( dynamic_cast<Word*>(pnt) );
      }
      else if ( pnt->isinstance( Sentence_t ) ) {
	KWargs args;
	args["text"] = pnt->id();
	PlaceHolder *p = new PlaceHolder( args, doc() );
	doc()->keepForDeletion( p );
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
    for ( const auto& pnt : data() ) {
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

  Correction *Sentence::splitWord( FoliaElement *orig,
				   FoliaElement *p1,
				   FoliaElement *p2,
				   const KWargs& args ) {
    /// create a split Correction of an Element in a Sentence given 2
    /// FoliaElement nodes
    /*!
     * \param orig the node to correct
     * \param p1 the first part of the split
     * \param p2 the second part of the split
     * \param args additional arguments in Attribute-value pairs
     * \return the created Correction
     */
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
    /// create a merge Correction of list of Element in a Sentence
    /// into one new FoliaElement node
    /*!
     * \param nw the new (corrected) node
     * \param orig the list of nodes to merge
     * \param args additional arguments in Attribute-value pairs
     * \return the created Correction
     */
    vector<FoliaElement*> nv;
    nv.push_back( nw );
    return correctWords( orig, nv, getArgs(args) );
  }

  Correction *Sentence::deleteword( FoliaElement *w,
				    const string& args ) {
    /// create a Correction where a word is deleted
    /*!
     * \param w the node to delete
     * \param args additional arguments in Attribute-value pairs
     * \return the created Correction
     */
    vector<FoliaElement*> ov;
    ov.push_back( w );
    vector<FoliaElement*> nil1;
    return correctWords( ov, nil1, getArgs(args) );
  }

  Correction *Sentence::insertword( FoliaElement *w,
				    FoliaElement *p,
				    const string& args ) {
    /// create a Correction where a extra word is inserted
    /*!
     * \param w the Word to insert
     * \param p the Word after which to insert
     * \param args additional arguments in Attribute-value pairs
     * \return the created Correction
     */
    if ( !p || !p->isinstance( Word_t ) ) {
      throw runtime_error( "insertword(): previous is not a Word " );
    }
    if ( !w || !w->isinstance( Word_t ) ) {
      throw runtime_error( "insertword(): new word is not a Word " );
    }
    KWargs kwargs;
    kwargs["text"] = "dummy";
    kwargs["xml:id"] = "dummy";
    Word *dummy = new Word( kwargs );
    dummy->set_parent( this ); // we create a dummy Word.
    // now we insert it as member of the Sentence.
    // This makes correctWords() happy
    AbstractElement::insert_after( p, dummy );
    vector<FoliaElement *> ov;
    ov.push_back( dummy );
    vector<FoliaElement *> nv;
    nv.push_back( w );
    // so we attempt to 'correct' the dummy word into w
    return correctWords( ov, nv, getArgs(args) );
  }

  Correction *Sentence::correctWords( const vector<FoliaElement *>& orig,
				      const vector<FoliaElement *>& _new,
				      const KWargs& argsin ) {
    /// Generic correction method for words.
    /*!
     * \param orig a list of original elements
     * \param _new a list of new elements to replace \orig
     * \param args additional arguments in Attribute-value pairs
     * \return the created Correction
     *
     * You most likely want to use the helper functions
     *   splitword() , mergewords(), deleteword(), insertword() instead
     */
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

  void TextContent::setAttributes( KWargs& kwargs ) {
    /// set the TextContent attributes given a set of Key-Value pairs.
    /*!
     * \param kwargs a KWargs set of Key-Value pairs
     *
     * checks and sets the special attributes for TextContent:
     * value, offset, ref, class
     */
    auto it = kwargs.find( "value" );
    if ( it != kwargs.end() ) {
      string value = it->second;
      kwargs.erase(it);
      if ( value.empty() ) {
	// can this ever happen?
	throw ValueError( "TextContent: 'value' attribute may not be empty." );
      }
      XmlText *t = new XmlText();
      t->setvalue( value );
      append( t );
    }
    it = kwargs.find( "offset" );
    if ( it != kwargs.end() ) {
      _offset = stringTo<int>(it->second);
      kwargs.erase(it);
    }
    else {
      _offset = -1;
    }
    it = kwargs.find( "ref" );
    if ( it != kwargs.end() ) {
      _ref = it->second;
      kwargs.erase(it);
    }
    it = kwargs.find( "class" );
    if ( it == kwargs.end() ) {
      kwargs["class"] = "current";
    }
    AllowXlink::setAttributes(kwargs);
    AbstractElement::setAttributes(kwargs);
  }

  void PhonContent::setAttributes( KWargs& kwargs ) {
    /// set the PhonContent attributes given a set of Key-Value pairs.
    /*!
     * \param kwargs a KWargs set of Key-Value pairs
     *
     * checks and sets the special attributes for PhonContent:
     * offset, ref, class
     */
    auto it = kwargs.find( "offset" );
    if ( it != kwargs.end() ) {
      _offset = stringTo<int>(it->second);
      kwargs.erase(it);
    }
    else {
      _offset = -1;
    }
    if ( kwargs.is_present( "ref" ) ) {
      throw NotImplementedError( "ref attribute in PhonContent" );
    }
    if ( !kwargs.is_present( "class" ) ){
      kwargs["class"] = "current";
    }
    AbstractElement::setAttributes(kwargs);
  }

  FoliaElement *TextContent::find_default_reference() const {
    /// find the 'true' parent of a TextContent
    /*!
     * recurse the parent() nodes upward. halt at the second parent
     * that is a Structure, String or Subtoken
     */
    int depth = 0;
    FoliaElement *p = parent();
    while ( p ){
      if ( p->isSubClass( String_t )
	   || p->isSubClass( AbstractStructureElement_t )
	   || p->isSubClass( AbstractSubtokenAnnotation_t ) ){
	if ( ++depth == 2 ){
	  return p;
	}
      }
      p = p->parent();
    }
    return 0;
  }

  FoliaElement *TextContent::get_reference(bool trim_spaces) const {
    /// get the FoliaElement _ref is refering to and does offset validation
    /*!
     * \return the refered element OR the default parent when _ref is 0
     */
    FoliaElement *ref = 0;
    if ( _offset == -1 ){
      return 0;
    }
    else if ( !_ref.empty() ){
      try{
	ref = (*doc())[_ref];
      }
      catch (...){
      }
    }
    else {
      ref = find_default_reference();
    }
    if ( !ref ){
      throw UnresolvableTextContent( "Default reference for textcontent not found!" );
    }
    else if ( !ref->hastext( cls() ) ){
      throw UnresolvableTextContent( "Reference (ID " + _ref + ") has no such text (class=" + cls() + ")" );
    }
    else if ( doc()->checktext() || doc()->fixtext() ){
      TEXT_FLAGS flags = TEXT_FLAGS::STRICT;
      if ( !trim_spaces ) flags |= TEXT_FLAGS::NO_TRIM_SPACES;
      UnicodeString mt = this->text( this->cls(), flags );
      UnicodeString pt = ref->text( this->cls(), flags );
      UnicodeString sub( pt, this->offset(), mt.length() );
      if ( mt != sub ){
	if ( doc()->fixtext() ){
	  int pos = pt.indexOf( mt );
	  if ( pos < 0 ){
	    throw UnresolvableTextContent( "Reference (ID " + ref->id() +
					   ",class=" + cls()
					   + " found, but no substring match "
					   + TiCC::UnicodeToUTF8(mt)
					   + " in " +  TiCC::UnicodeToUTF8(pt) );
	  }
	  else {
	    this->set_offset( pos );
	  }
	}
	else {
	  throw UnresolvableTextContent( "Reference (ID " + ref->id() +
					 ",class='" + cls()
					 + "') found, but no text match at "
					 + "offset=" + TiCC::toString(offset())
					 + " Expected '" + TiCC::UnicodeToUTF8(mt)
					 + "' but got '" +  TiCC::UnicodeToUTF8(sub) + "'" );
	}
      }
    }
    return ref;
  }

  KWargs TextContent::collectAttributes() const {
    /// extract all Attribute-Value pairs for TextContent
    /*!
     * \return a KWargs set of Attribute-value pairs
     * inclusive: offset and ref
     */
    KWargs attribs = AbstractElement::collectAttributes();
    if ( cls() == "current" && !doc()->has_explicit() ) {
      attribs.erase( "class" );
    }
    if ( _offset >= 0 ) {
      attribs["offset"] = TiCC::toString( _offset );
    }
    if ( !_ref.empty() ) {
      attribs["ref"] = _ref;
    }
    KWargs more = AllowXlink::collectAttributes();
    attribs.insert( more.begin(), more.end() );
    return attribs;
  }

  FoliaElement *PhonContent::find_default_reference() const {
    /// find the 'true' parent of a PhonContent
    /*!
     * recurse the parent() nodes upward. halt at the second parent
     * that is a Structure, String or Subtoken
     */
    int depth = 0;
    FoliaElement *p = parent();
    while ( p ){
      if ( p->isSubClass( AbstractStructureElement_t )
	   || p->isSubClass( AbstractSubtokenAnnotation_t ) ){
	if ( ++depth == 2 ){
	  return p;
	}
      }
      p = p->parent();
    }
    return 0;
  }

  FoliaElement *PhonContent::get_reference(bool trim_spaces) const {
    /// get the FoliaElement _ref is refering to
    /*!
     * \return the refered element OR the default parent when _ref is 0
     */
    FoliaElement *ref = 0;
    if ( _offset == -1 ){
      return 0;
    }
    else if ( !_ref.empty() ){
      try{
	ref = (*doc())[_ref];
      }
      catch (...){
      }
    }
    else {
      ref = find_default_reference();
    }
    if ( !ref ){
      throw UnresolvableTextContent( "Default reference for phonetic content not found!" );
    }
    else if ( !ref->hasphon( cls() ) ){
      throw UnresolvableTextContent( "Reference (ID " + _ref + ") has no such phonetic content (class=" + cls() + ")" );
    }
    else if ( doc()->checktext() || doc()->fixtext() ){
      TEXT_FLAGS flags = TEXT_FLAGS::STRICT;
      if ( !trim_spaces ) flags |= TEXT_FLAGS::NO_TRIM_SPACES;
      UnicodeString mt = this->phon( this->cls(), flags );
      UnicodeString pt = ref->phon( this->cls(), flags );
      UnicodeString sub( pt, this->offset(), mt.length() );
      if ( mt != sub ){
	if ( doc()->fixtext() ){
	  int pos = pt.indexOf( mt );
	  if ( pos < 0 ){
	    throw UnresolvableTextContent( "Reference (ID " + ref->id() +
					   ",class=" + cls()
					   + " found, but no substring match "
					   + TiCC::UnicodeToUTF8(mt)
					   + " in " +  TiCC::UnicodeToUTF8(pt) );
	  }
	  else {
	    this->set_offset( pos );
	  }
	}
	else {
	  throw UnresolvableTextContent( "Reference (ID " + ref->id() +
					 ",class=" + cls()
					 + " found, but no text match at "
					 + "offset=" + TiCC::toString(offset())
					 + " Expected " + TiCC::UnicodeToUTF8(mt)
					 + " but got " +  TiCC::UnicodeToUTF8(sub) );
	}
      }
    }
    return ref;
  }

  KWargs PhonContent::collectAttributes() const {
    /// extract all Attribute-Value pairs for PhonContent
    /*!
     * \return a KWargs set of Attribute-value pairs
     * inclusive: offset
     */
    KWargs attribs = AbstractElement::collectAttributes();
    if ( cls() == "current" && !doc()->has_explicit() ) {
      attribs.erase( "class" );
    }
    if ( _offset >= 0 ) {
      attribs["offset"] = TiCC::toString( _offset );
    }
    return attribs;
  }

  void Linebreak::setAttributes( KWargs& kwargs ){
    /// set the Linebreak attributes given a set of Key-Value pairs.
    /*!
     * \param kwargs a KWargs set of Key-Value pairs
     *
     * checks and sets the special attributes for Linebreak:
     * pagenr, linenr, newpage
     */
    _pagenr = kwargs.extract( "pagenr" );
    _linenr = kwargs.extract( "linenr" );
    string val = kwargs.extract( "newpage" );
    if ( !val.empty() ) {
      _newpage = ( val == "yes" );
    }
    AllowXlink::setAttributes( kwargs );
    AbstractElement::setAttributes( kwargs );
  }

  KWargs Linebreak::collectAttributes() const {
    /// extract all Attribute-Value pairs for LineBreak
    /*!
     * \return a KWargs set of Attribute-value pairs
     * inclusive: linenr, pagenr and newpage
     */
    KWargs atts = AbstractElement::collectAttributes();
    if ( ! _linenr.empty() ){
      atts["linenr"] = _linenr;
    }
    if ( ! _pagenr.empty() ){
      atts["pagenr"] = _pagenr;
    }
    if ( _newpage ){
      atts["newpage"] = "yes";
    }
    KWargs more = AllowXlink::collectAttributes();
    atts.insert( more.begin(), more.end() );
    return atts;
  }

  vector<FoliaElement *>TextContent::find_replacables( FoliaElement *par ) const {
    // find all children with the same type, set AND textclass as the parameter
    /*!
     * \param par the FoliaElement to search
     * \return a vector of matching elements
     * search in the DIRECT children for nodes with the same tag, set and text
     * class as the element par
     */
    vector<FoliaElement *> result;
    vector<TextContent*> v = par->FoliaElement::select<TextContent>( sett(),
								     false );
    copy_if( v.begin(),
	     v.end(),
	     back_inserter(result),
	     [&]( FoliaElement *el ){ return el->cls() == cls(); } );
    return result;
  }

  const UnicodeString PhonContent::phon( const string& cls,
					 TEXT_FLAGS ) const {
    /// get the UnicodeString phon value
    /*!
     * \param cls the textclass the text should be in
     * The second parameter is NOT used (yet)
     */
#ifdef DEBUG_PHON
    cerr << "PhonContent::PHON(" << cls << ") " << endl;
#endif
    UnicodeString result;
    for ( const auto& el : data() ) {
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
      } catch ( const NoSuchPhon& e ) {
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
    /// generate an new xml:id
    /*!
     * \param tag an extra string to use in the result
     * \return a string with an unique id
     *
     * The new id is constructed from the elements id, or from a parent id
     */
    string nodeId = id();
    // cerr << "node: " << this << endl;
    // cerr << "ID=" << nodeId << endl;
    if ( nodeId.empty() ){
      // if no ID, look upward.
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
    /// register the child id for later use
    /*!
      * \param child
      * if the child has an id, try to extract the last part as a number
      * if so, check the registration of that numer for the childs tag
      */
    if ( !child->id().empty() && !child->xmltag().empty() ) {
      vector<string> parts = TiCC::split_at( child->id(), "." );
      if ( !parts.empty() ) {
	string val = parts.back();
	int i;
	try {
	  i = stringTo<int>( val );
	}
	catch ( const exception& ) {
	  // no number, so assume some user defined id
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

  Correction * AllowCorrections::correct( const vector<FoliaElement*>& _original,
					  const vector<FoliaElement*>& _current,
					  const vector<FoliaElement*>& _newv,
					  const vector<FoliaElement*>& _suggestions,
					  const KWargs& args_in ) {
    /// generic function to correct a group of FoliaElements into a Correction
    /*!
     * \param _original a group of nodes to correct and add to the Original
     * \param _current a group of nodes to add to the Current
     * \param _newv a group of nodes to replace _original, added to New
     * \param _suggestions a group of nodes to add to Suggestions
     * \param args_in additional arguments
     * \return the Correction node. Might throw on problems
     */
#ifdef DEBUG_CORRECT
    cerr << "correct " << this << endl;
    cerr << "original= " << _original << endl;
    cerr << "current = " << _current << endl;
    cerr << "new     = " << _newv << endl;
    cerr << "suggestions     = " << _suggestions << endl;
    cerr << "args in     = " << args_in << endl;
#endif
    // Apply a correction
    Document *doc = this->doc();
    Correction *corr = 0;
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
      TextContent *t = new TextContent( my_args, doc );
      _new.push_back( t );
      args.erase( it );
    }
    it = args.find("suggestion");
    if ( it != args.end() ) {
      KWargs my_args;
      my_args["value"] = it->second;
      TextContent *t = new TextContent( my_args, doc );
      suggestions.push_back( t );
      args.erase( it );
    }
    it = args.find("reuse");
    if ( it != args.end() ) {
      // reuse an existing correction instead of making a new one
      try {
	corr = dynamic_cast<Correction*>(doc->index(it->second));
      }
      catch ( const exception& e ) {
	throw ValueError("reuse= must point to an existing correction id!");
      }
      if ( !corr->isinstance( Correction_t ) ) {
	throw ValueError("reuse= must point to an existing correction id!");
      }
      hooked = true;
      if ( !_new.empty() && corr->hasCurrent() ) {
	// can't add new if there's current, so first set original to current, and then delete current

	if ( !_current.empty() ) {
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
      args2["xml:id"] = id;
      corr = new Correction( args2, doc );
    }
#ifdef DEBUG_CORRECT
    cerr << "now corr= " << corr << endl;
#endif
    if ( !_current.empty() ) {
      if ( !original.empty() || !_new.empty() ) {
	throw runtime_error("When setting current=, original= and new= can not be set!");
      }
      for ( const auto& cur : _current ) {
	FoliaElement *add = new Current( doc );
	cur->set_parent(0);
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
      vector<New*> old_new = corr->select<New>();
      if ( !old_new.empty() && old_new[0]->size() == 0 ){
	// there is an EMPTY <new> tag!
	// use it to expand
	addnew = old_new[0];
      }
      else {
	// create a <new> tag, might throw is there is a non-empty one
	addnew = new New( doc );
	corr->append(addnew);
      }
      for ( const auto& nw : _new ) {
	nw->set_parent(0);
	addnew->append( nw );
      }
#ifdef DEBUG_CORRECT
      cerr << "after adding NEW: " << corr->xmlstring() << endl;
#endif
      vector<Current*> v = corr->FoliaElement::select<Current>();
      //delete current if present
      for ( const auto& cur:v ) {
	corr->remove( cur, false );
      }
#ifdef DEBUG_CORRECT
      cerr << "after removing CUR: " << corr->xmlstring() << endl;
#endif
    }
    else {
      vector<New*> old_new = corr->select<New>();
      if ( !old_new.empty() && old_new[0]->size() == 0 ){
	// there is aleady an EMPTY <new> tag!
      }
      else {
	// create a <new> tag, might throw is there is a non-empty one
	New *add_new = new New( doc );
	corr->append(add_new);
      }
    }
    if ( !original.empty() ) {
#ifdef DEBUG_CORRECT
      cerr << "there is original! " << endl;
#endif
      FoliaElement *add = new Original( doc );
      corr->replace(add);
#ifdef DEBUG_CORRECT
      cerr << " corr after replacing original " << corr->xmlstring() << endl;
      cerr << " new original= " << add << endl;
#endif
      for ( const auto& org: original ) {
#ifdef DEBUG_CORRECT
	cerr << " examine org " << org << endl;
#endif
	bool dummyNode = ( org->id() == "dummy" );
	if ( !dummyNode ) {
	  org->set_parent(0);
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
	      cerr << " corr after replace " << corr->xmlstring() << endl;
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
	vector<FoliaElement*> v = p->find_replacables( this );
	// for ( const auto& el: v ) {
	//   orig.push_back( el );
	// }
	copy( v.begin(), v.end(), back_inserter(orig) );
      }
      if ( orig.empty() ) {
	throw runtime_error( "No original= specified and unable to automatically infer");
      }
      else {
#ifdef DEBUG_CORRECT
	cerr << "we seem to have some originals! " << endl;
#endif
	FoliaElement *add = new Original( doc );
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
	  org->set_parent( 0 );
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
    cerr << " corr after edits " << corr->xmlstring() << endl;
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
    cerr << " corr after removes " << corr->xmlstring() << endl;
#endif
    if ( !suggestions.empty() ) {
      if ( !hooked ) {
	append(corr);
      }
      for ( const auto& sug : suggestions ) {
	if ( sug->isinstance( Suggestion_t ) ) {
	  sug->set_parent(0);
	  corr->append( sug );
	}
	else {
	  FoliaElement *add = new Suggestion( doc );
	  sug->set_parent(0);
	  add->append( sug );
	  corr->append( add );
	}
      }
    }

    it = args.find("reuse");
    if ( it != args.end() ) {
      it = args.find("annotator");
      if ( it != args.end() ) {
	corr->annotator( it->second );
      }
      it = args.find("annotatortype");
      if ( it != args.end() ){
	corr->annotatortype( stringTo<AnnotatorType>(it->second) );
      }
      it = args.find("confidence");
      if ( it != args.end() ) {
	corr->confidence( stringTo<double>(it->second) );
      }
    }
    return corr;
  }

  Correction *AllowCorrections::correct( const string& s ) {
    /// use an Attribute-Value list to create a Correction
    /*!
     * \param s a string representation of a Attribute-Value list
     * \return the created Correcion
     * The parameter is converted to a KWargs list which is handled over
     * to correct()
     */
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

  Correction *AllowCorrections::correct( FoliaElement *_old,
					 FoliaElement *_new,
					 const vector<FoliaElement*>& sugg,
					 const KWargs& args ) {
    /// create a correction using the parameters
    /*!
     * \param _old the node to correct
     * \param _new the corrected node
     * \param sugg a list of possible suggestions
     * \param args additonal arguments
     * \return the created Correcion
     */
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

  Correction *AllowCorrections::correct( FoliaElement* _old,
					 FoliaElement* _new,
					 const KWargs& args ) {
    /// create a correction using the parameters
    /*!
     * \param _old the node to correct
     * \param _new the corrected node
     * \param args additonal arguments
     * \return the created Correcion
     */
    const std::vector<FoliaElement*> sugg;
    return correct( _old, _new, sugg, args );
  }

  FoliaElement *AbstractStructureElement::append( FoliaElement *child ){
    /// append child to an AbstractStructureElement node
    /*!
     * \param child the node to add
     * \return the appended child
     *
     * will throw on error
     * Sets the ID of the child, if not provided yet
     */
    AbstractElement::append( child );
    setMaxId( child );
    return child;
  }

  vector<Paragraph*> AbstractStructureElement::paragraphs() const {
    /// return all embedded Paragraph nodes
    /*!
     * \return a list of the found paragraphs, throws when none are found
     * This function recursus into all children of the node, except for
     * children in the default_ignore_structure set, like Alternative, Foreign
     * and the Original and Suggestion parts of Correction
     */
    return FoliaElement::select<Paragraph>( default_ignore_structure );
  }

  vector<Sentence*> AbstractStructureElement::sentences() const{
    /// return all embedded Sentence nodes
    /*!
     * \return a list of the found sentences, throws when none are found
     * This function recursus into all children of the node, except for
     * children in the default_ignore_structure set, like Alternative, Foreign
     * and the Original and Suggestion parts of Correction
     */
    return FoliaElement::select<Sentence>( default_ignore_structure );
  }

  vector<Word*> AbstractStructureElement::words( const string& st ) const{
    /// return all embedded Word nodes in set st
    /*!
     * \param st the required set. If st="" then ALL sets are searched.
     * \return a list of the found words, throws when none are found
     * This function recursus into all children of the node, except for
     * children in the default_ignore_structure set, like Alternative, Foreign
     * and the Original and Suggestion parts of Correction
     */
    return FoliaElement::select<Word>( st, default_ignore_structure );
  }

  Sentence *AbstractStructureElement::sentences( size_t index ) const {
    /// return the Sentence at index
    /*!
     * \param index the position
     * \return the found Sentence, throws when not found
     * This function recursus into all children of the node, except for
     * children in the default_ignore_structure set, like Alternative, Foreign
     * and the Original and Suggestion parts of Correction
     */
    vector<Sentence*> v = sentences();
    if ( index < v.size() ) {
      return v[index];
    }
    throw range_error( "sentences(): index out of range" );
  }

  Sentence *AbstractStructureElement::rsentences( size_t index ) const {
    /// return the Sentence at REVERSE index
    /*!
     * \param index the position from the back of the Word list
     * \return the found Sentence, throws when not found
     * This function recursus into all children of the node, except for
     * children in the default_ignore_structure set, like Alternative, Foreign
     * and the Original and Suggestion parts of Correction
     */
    vector<Sentence*> v = sentences();
    if ( index < v.size() ) {
      return v[v.size()-1-index];
    }
    throw range_error( "rsentences(): index out of range" );
  }

  Paragraph *AbstractStructureElement::paragraphs( size_t index ) const {
    /// return the Paragraph at index
    /*!
     * \param index the position
     * \return the found Paragraph, throws when not found
     * This function recursus into all children of the node, except for
     * children in the default_ignore_structure set, like Alternative, Foreign
     * and the Original and Suggestion parts of Correction
     */
    vector<Paragraph*> v = paragraphs();
    if ( index < v.size() ) {
      return v[index];
    }
    throw range_error( "paragraphs(): index out of range" );
  }

  Paragraph *AbstractStructureElement::rparagraphs( size_t index ) const {
    /// return the Paragraph at REVERSE index
    /*!
     * \param index the position from the back of the Word list
     * \return the found Paragraph, throws when not found
     * This function recursus into all children of the node, except for
     * children in the default_ignore_structure set, like Alternative, Foreign
     * and the Original and Suggestion parts of Correction
     */
    vector<Paragraph*> v = paragraphs();
    if ( index < v.size() ) {
      return v[v.size()-1-index];
    }
    throw range_error( "rparagraphs(): index out of range" );
  }

  Word *AbstractStructureElement::words( size_t index,
					 const string& st ) const {
    /// return the Word at index, in set st
    /*!
     * \param index the position
     * \param st the required set. If st="" then ALL sets are searched.
     * \return the found Word, throws when not found
     * This function recursus into all children of the node, except for
     * children in the default_ignore_structure set, like Alternative, Foreign
     * and the Original and Suggestion parts of Correction
     */
    vector<Word*> v = words(st);
    if ( index < v.size() ) {
      return v[index];
    }
    throw range_error( "words(): index out of range" );
  }

  Word *AbstractStructureElement::rwords( size_t index,
					  const string& st ) const {
    /// return the Word at REVERSE index, in set st
    /*!
     * \param index the position from the back of the Word list
     * \param st the required set. If st="" then ALL sets are searched.
     * \return the found Word, throws when not found
     * This function recursus into all children of the node, except for
     * children in the default_ignore_structure set, like Alternative, Foreign
     * and the Original and Suggestion parts of Correction
     */
    vector<Word*> v = words(st);
    if ( index < v.size() ) {
      return v[v.size()-1-index];
    }
    throw range_error( "rwords(): index out of range" );
  }

  const Word* AbstractStructureElement::resolveword( const string& id ) const{
    /// recursively search for a Word with xml:id id
    /*!
     * \param id The xml:id we are looking for
     * \return pointer to the Word. Or 0 when not found.
     */
    const Word *result = 0;
    for ( const auto& el : data() ) {
      result = el->resolveword( id );
      if ( result ) {
	break;
      }
    }
    return result;
  }

  vector<Alternative *> AllowInlineAnnotation::alternatives( ElementType elt,
							     const string& st ) const {
    /// Return a list of alternatives, either all restrained by the parameters
    /*!
     * \param elt only look for alternatives of type elt, default ALL types
     * \param st only look for alternatives in set st, default NO restriction
     * \return a list of matching Alternative nodes
     */
    vector<Alternative *> alts = FoliaElement::select<Alternative>( AnnoExcludeSet );
    if ( elt == BASE ) {
      return alts;
    }
    else {
      vector<Alternative*> res;
      for ( const auto& alt : alts ){
	if ( alt->size() > 0 ) { // child elements?
	  for ( size_t j =0; j < alt->size(); ++j ) {
	    auto hit = alt->index(j);
	    if ( hit->element_id() == elt &&
		 ( hit->sett().empty() || hit->sett() == st ) ) {
	      res.push_back( alt ); // not the child!
	    }
	  }
	}
      }
      return res;
    }
  }

  KWargs LinkReference::collectAttributes() const {
    /// extract all Attribute-Value pairs for LinkReference
    /*!
     * \return a KWargs set of Attribute-value pairs
     * inclusive: ref_id and type
     */
    KWargs atts;
    atts["id"] = ref_id;
    atts["type"] = ref_type;
    if ( !_t.empty() ) {
      atts["t"] = _t;
    }
    return atts;
  }

  void LinkReference::setAttributes( KWargs& kwargs ) {
    /// set the LinkReference attributes given a set of Key-Value pairs.
    /*!
     * \param kwargs a KWargs set of Key-Value pairs
     *
     * checks and sets the special attributes for LinkReference:
     * id, type, t
     */
    ref_id = kwargs.extract( "id" );
    ref_type = kwargs.extract( "type" );
    _t = kwargs.extract( "t" );
    AbstractElement::setAttributes(kwargs);
  }

  void Word::setAttributes( KWargs& kwargs ) {
    /// set the Word attributes given a set of Key-Value pairs.
    /*!
     * \param kwargs a KWargs set of Key-Value pairs
     *
     * checks and sets the special attributes for Word: text
     */
    string value = kwargs.extract( "text" );
    if ( !value.empty() ) {
      settext( value );
    }
    AbstractElement::setAttributes( kwargs );
  }

  const string& Word::get_delimiter( bool retaintok ) const {
    /// get the default delimiter of a Word
    /*!
     * \param retaintok retain the tokenization assigned to this element
     * \return a string representing the delimiter
     */
    if ( space() || retaintok ) {
      return PROPS.TEXTDELIMITER;
    }
    return EMPTY_STRING;
  }

  Correction *Word::split( FoliaElement *part1,
			   FoliaElement *part2,
			   const string& args ) {
    /// split a word into 2 new words
    /*!
     * \param part1 the first new Word
     * \param part2 the second new Word
     * \param args additional arguments
     * \return the Correction
     *
     * correction takes place in de context of the Sentence of the object
     */
    return sentence()->splitWord( this, part1, part2, getArgs(args) );
  }

  FoliaElement *Word::append( FoliaElement *child ){
    /// append child to a Word
    /*!
     * \param child the node to add
     * \return the appended child
     *
     * will throw on error
     * checks uniqueness of the child when it is an annotation
     */
    if ( child->isSubClass( AbstractAnnotationLayer_t ) ) {
      // sanity check, there may be no other child within the same set
      vector<FoliaElement*> v = select( child->element_id(), child->sett() );
      if ( v.empty() ) {
    	// OK!
    	return AbstractElement::append( child );
      }
      delete child;
      throw DuplicateAnnotationError( "Word::append" );
    }
    return AbstractElement::append( child );
  }

  Sentence *AbstractWord::sentence() const {
    /// return the sentence this word is a part of, otherwise return null
    /*
     * recurses upward through all parents until a hit or no parent left
     */
    FoliaElement *p = parent();
    while( p ) {
      if ( p->isinstance( Sentence_t ) ) {
	return dynamic_cast<Sentence*>(p);
      }
      p = p->parent();
    }
    return 0;
  }

  Paragraph *AbstractWord::paragraph( ) const {
    /// return the sentence this word is a part of, otherwise return null
    /*
     * recurses upward through all parents until a hit or no parent left
     */
    FoliaElement *p = parent();
    while( p ) {
      if ( p->isinstance( Paragraph_t ) ) {
	return dynamic_cast<Paragraph*>(p);
      }
      p = p->parent();
    }
    return 0;
  }

  Division *AbstractWord::division() const {
    /// return the Division this word is a part of, otherwise return null
    /*
     * recurses upward through all parents until a hit or no parent left
     */
    FoliaElement *p = parent();
    while( p ) {
      if ( p->isinstance( Division_t ) ) {
	return dynamic_cast<Division*>(p);
      }
      p = p->parent();
    }
    return 0;
  }

  vector<Morpheme *> AbstractWord::morphemes( const string& set ) const {
    /// return all morphemes in this AbstractWord
    /*
     * \param set the set to use in searching.
     * \return a list of Morpheme nodes. Might be empty.
     */
    vector<Morpheme *> result;
    vector<MorphologyLayer*> mv = FoliaElement::select<MorphologyLayer>();
    for ( const auto& mor : mv ){
      vector<Morpheme*> tmp = mor->FoliaElement::select<Morpheme>( set );
      result.insert( result.end(), tmp.begin(), tmp.end() );
    }
    return result;
  }

  Morpheme *AbstractWord::morpheme( size_t pos, const string& set ) const {
    /// return the Morpheme with indes pos
    /*
     * \param set the set to use in searching.
     * \return the Morpheme. Throws on error.
     */
    vector<Morpheme *> tmp = morphemes( set );
    if ( pos < tmp.size() ) {
      return tmp[pos];
    }
    throw range_error( "morpheme() index out of range" );
  }

  Correction *Word::incorrection( ) const {
    /// find out if a Word is part of a correction?
    /*!
     * \return the Correction the Word it is part of, or 0 when uncorrected.
     * recurses upward through the parents, until a result is found, or
     * the Sentence level or the document level..
     */
    FoliaElement *p = parent();
    while ( p ) {
      if ( p->isinstance( Correction_t ) ) {
	return dynamic_cast<Correction*>(p);
      }
      else if ( p->isinstance( Sentence_t ) ){
	break;
      }
      p = p->parent();
    }
    return 0;
  }

  Word *Word::previous() const {
    /// return the previous Word in the Sentence
    /*!
     * \return the previous Word or 0, when not found.
     */
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
    /// return the next Word in the Sentence
    /*!
     * \return the next Word or 0, when not found.
     */
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
    /// return the (Word) context the Word is in.
    /*!
     * \param size limits the number of context words
     * \param val string value of the PlaceHolder for the Word itself
     * \return a list of (maximum) size Word nodes surrounding the Word.
     *
     * The result is a list of Word nodes of length 'size', where the Word
     * itself is replaced by the 0 pointer, or a PlaceHolder with value val
     * in the middle of the list.
     */
    vector<Word*> result;
    if ( size > 0 ) {
      vector<Word*> words = doc()->words();
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
	      doc()->keepForDeletion( p );
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
		doc()->keepForDeletion( p );
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
    /// return the left (Word) context the Word is in.
    /*!
     * \param size limits the number of context words
     * \param val string value of the PlaceHolder for the Word itself
     * \return a list of (maximum) size Word nodes preceding teh Word
     *
     * The result is a list of Word nodes of length 'size', where the Word
     * itself is replaced by the 0 pointer, or a PlaceHolder with value val
     * at the end of the list.
     */
    //  cerr << "leftcontext : " << size << endl;
    vector<Word*> result;
    if ( size > 0 ) {
      vector<Word*> words = doc()->words();
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
	      doc()->keepForDeletion( p );
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
    /// return the rigth (Word) context the Word is in.
    /*!
     * \param size limits the number of context words
     * \param val string value of the PlaceHolder for the Word itself
     * \return a list of (maximum) size Word nodes succeeding the Word
     *
     * The result is a list of Word nodes of length 'size', where the Word
     * itself is replaced by the 0 pointer, or a PlaceHolder with value val
     * at the beginning of the list.
     */
    vector<Word*> result;
    //  cerr << "rightcontext : " << size << endl;
    if ( size > 0 ) {
      vector<Word*> words = doc()->words();
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
		doc()->keepForDeletion( p );
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
    /// search for Word with xml:id id
    /*!
     * \param id The xml:id we are looking for
     * \return pointer to the Word. Or 0 when not found.
     */
    if ( AbstractElement::id() == id ) {
      return this;
    }
    return 0;
  }

  ElementType layertypeof( ElementType et ) {
    /// return the Layer ElemenType of the given ElementType
    /*!
     * \param et the ElementType to check
     * \return The Layer ElementType er belongs to. Or BASE if it has no Layer.
     */
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

  vector<AbstractSpanAnnotation*> AbstractWord::findspans( ElementType et,
							   const string& st ) const {
    /// find all SpanAnnotation nodes of this object for a given type
    /*!
     * \param et the ElementType to search for
     * \param st limit the search to set st
     * \return a list of SpanAnnotations
     */
    ElementType layertype = layertypeof( et );
    vector<AbstractSpanAnnotation *> result;
    if ( layertype != BASE ) {
      const FoliaElement *e = parent();
      if ( e ) {
	vector<FoliaElement*> v = e->select( layertype, st, SELECT_FLAGS::LOCAL );
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
    /// parse a WordReference node at node
    /*!
     * \param node a WordReference
     * \return the parsed tree. Throws on error.
     */
    KWargs atts = getAttributes( node );
    string id = atts["id"];
    if ( id.empty() ) {
      throw XmlError( "empty id in WordReference" );
    }
    if ( doc()->debug ) {
      cerr << "Found word reference: " << id << endl;
    }
    FoliaElement *ref = (*doc())[id];
    if ( ref ) {
      if ( !ref->referable() ){
	throw XmlError( "WordReference id=" + id + " refers to a non-referable word: "
			+ ref->xmltag() );
      }
      // Disabled test! should consider the textclass of the yet unknown
      // parent!
      // addable() should check this. But that is impossible!
      // we don't return a WordReference but the ref to the word!
      //
      // string tval = atts["t"];
      // if ( !tval.empty() ){
      // 	string tc = ref->textclass();
      // 	string rtval = ref->str(tc);
      // 	if ( tval != rtval ){
      // 	  throw XmlError( "WordReference id=" + id + " has another value for "
      // 			  + "the t attribute than it's reference. ("
      // 			  + tval + " versus " + rtval + ")" );
      // 	}
      // }
      ref->increfcount();
    }
    else {
      throw XmlError( "Unresolvable id " + id + " in WordReference" );
    }
    delete this;
    return ref;
  }

  FoliaElement* LinkReference::parseXml( const xmlNode *node ) {
    /// parse a LinkReference node at node
    /*!
     * \param node a LinkReference
     * \return the parsed tree. Throws on error.
     */
    KWargs att = getAttributes( node );
    string val = att["id"];
    if ( val.empty() ) {
      throw XmlError( "ID required for LinkReference" );
    }
    ref_id = val;
    if ( doc()->debug ) {
      cerr << "Found LinkReference ID " << ref_id << endl;
    }
    ref_type = att["type"];
    val = att["t"];
    if ( !val.empty() ) {
      _t = val;
    }
    return this;
  }

  FoliaElement *LinkReference::resolve_element( const Relation *ref ) const {
    /// return the refered FoliaElement of this Reference
    /*!
     * \param ref the Relation which called us
     * \return pointer to refered FoliaElement. Throws when not found.
     */
    if ( ref->href().empty() ) {
      return (*doc())[ref_id];
    }
    throw NotImplementedError( "LinkReference::resolve() for external doc" );
  }

  void Relation::setAttributes( KWargs& kwargs ) {
    /// set the Relation attributes given a set of Key-Value pairs.
    /*!
     * \param kwargs a KWargs set of Key-Value pairs
     *
     * checks and sets the special attributes for Relation: format
     */
    _format = kwargs.extract( "format" );
    AllowXlink::setAttributes(kwargs);
    AbstractElement::setAttributes(kwargs);
  }

  KWargs Relation::collectAttributes() const {
    /// extract all Attribute-Value pairs for Relation
    /*!
     * \return a KWargs set of Attribute-value pairs
     * inclusive: format
     */
    KWargs atts = AbstractElement::collectAttributes();
    if ( !_format.empty() && _format != "text/folia+xml" ) {
      atts["format"] = _format;
    }
    KWargs more = AllowXlink::collectAttributes();
    atts.insert( more.begin(), more.end() );
    return atts;
  }

  vector<FoliaElement *> Relation::resolve() const {
    /// return a list of refered FoliaElement nodes
    /*!
     * \return a list of refered FoliaElement. Throws on error.
     */
    vector<FoliaElement*> result;
    vector<LinkReference*> v = FoliaElement::select<LinkReference>();
    transform( v.begin(), v.end(),
	       back_inserter(result),
	       [&]( LinkReference *r ){ return r->resolve_element(this); } );
    return result;
  }

  void PlaceHolder::setAttributes( KWargs& kwargs ) {
    /// set the PlaceHolder attributes given a set of Key-Value pairs.
    /*!
     * \param kwargs a KWargs set of Key-Value pairs
     *
     * checks and sets the special attributes for PlaceHolder: text
     */
    if ( !kwargs.is_present("text") ) {
      throw ValueError("text attribute is required for " + classname() );
    }
    else if ( kwargs.size() != 1 ) {
      throw ValueError("only the text attribute is supported for " + classname() );
    }
    Word::setAttributes( kwargs );
  }

  const UnicodeString Figure::caption() const {
    /// return the caption of a Figure
    /*!
     * \return A UnicodeString with the caption. Throws on error.
     */
    vector<FoliaElement *> v = select(Caption_t);
    if ( v.empty() ) {
      throw NoSuchText("caption");
    }
    else {
      return v[0]->text();
    }
  }

  void Description::setAttributes( KWargs& kwargs ) {
    /// set the Descriptions attributes given a set of Key-Value pairs.
    /*!
     * \param kwargs a KWargs set of Key-Value pairs
     *
     * checks and sets the special attributes for Description: value
     */
    _value = kwargs.extract( "value" );
    AbstractElement::setAttributes( kwargs );
  }

  xmlNode *Description::xml( bool, bool ) const {
    ///  convert the Description to an xmlNode
    xmlNode *e = AbstractElement::xml( false, false );
    if ( !_value.empty() ){
      xmlAddChild( e, xmlNewText( (const xmlChar*)_value.c_str()) );
    }
    return e;
  }

  FoliaElement* Description::parseXml( const xmlNode *node ) {
    /// parse a Description node at node
    /*!
     * \param node a Description
     * \return the parsed tree. Throws on error.
     */
    KWargs att = getAttributes( node );
    if ( !att.is_present("value") ) {
      att["value"] = XmlContent( node );
    }
    setAttributes( att );
    return this;
  }

  void Comment::setAttributes( KWargs& kwargs ) {
    /// set the Comments attributes given a set of Key-Value pairs.
    /*!
     * \param kwargs a KWargs set of Key-Value pairs
     *
     * checks and sets the special attributes for Comment: value
     */
    _value = kwargs.extract( "value" );
    AbstractElement::setAttributes( kwargs );
  }

  xmlNode *Comment::xml( bool, bool ) const {
    ///  convert the Comment to an xmlNode
    xmlNode *e = AbstractElement::xml( false, false );
    if ( !_value.empty() ){
      xmlAddChild( e, xmlNewText( (const xmlChar*)_value.c_str()) );
    }
    return e;
  }

  FoliaElement* Comment::parseXml( const xmlNode *node ) {
    /// parse a Comment node at node
    /*!
     * \param node a Comment
     * \return the parsed tree. Throws on error.
     */
    KWargs att = getAttributes( node );
    if ( !att.is_present("value") ) {
      att["value"] = XmlContent( node );
    }
    setAttributes( att );
    return this;
  }

  FoliaElement *AbstractSpanAnnotation::append( FoliaElement *child ){
    /// append child to an AbstractSpanAnnotation
    /*!
     * \param child the node to add
     * \return the appended child
     *
     * will throw on error
     * if the child has the  'referable' property, check that is not refered
     * to already
     */
    if ( child->referable() ){
      // cerr << "append a word: " << child << " to " << this << endl;
      // cerr << "refcnt=" << child->refcount() << endl;
      if ( child->refcount() == 0 ){
	throw XmlError( "connecting a <w> to an <" + xmltag()
       			+ "> is forbidden, use <wref>" );
      }
    }
    AbstractElement::append( child );
    if ( child->isinstance(PlaceHolder_t) ) {
      child->increfcount();
    }
    return child;
  }

  void AbstractAnnotationLayer::assignset( FoliaElement *child ) {
    // If there is no set (yet), try to get the set from the child
    // but not if it is the default set.
    // for a Correction child, we look deeper.
    // BARF when the sets are incompatible.
    string c_set;
    if ( child->isSubClass( AbstractSpanAnnotation_t ) ) {
      string st = child->sett();
      if ( !st.empty()
	   && doc()->default_set( child->annotation_type() ) != st ) {
	c_set = st;
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
		 && doc()->default_set( el->annotation_type() ) != st ) {
	      c_set = st;
	      break;
	    }
	  }
	}
      }
      if ( c_set.empty() ){
	New *nw = child->getNew();
	if ( nw ) {
	  for ( size_t i=0; i < nw->size(); ++i ) {
	    FoliaElement *el = nw->index(i);
	    if ( el->isSubClass( AbstractSpanAnnotation_t ) ) {
	      string st = el->sett();
	      if ( !st.empty()
		   && doc()->default_set( el->annotation_type() ) != st ) {
		c_set = st;
		break;
	      }
	    }
	  }
	}
      }
      if ( c_set.empty() ){
	auto v = child->suggestions();
	for ( const auto& el : v ) {
	  if ( el->isSubClass( AbstractSpanAnnotation_t ) ) {
	    string st = el->sett();
	    if ( !st.empty()
		 && doc()->default_set( el->annotation_type() ) != st ) {
	      c_set = st;
	      break;
	    }
	  }
	}
      }
    }
    if ( c_set.empty() ){
      return;
    }
    if ( sett().empty() ) {
      update_set( c_set );
    }
    else if ( sett() != c_set ){
      throw DuplicateAnnotationError( "appending child: " + child->xmltag()
				      + " with set='"
				      +  c_set + "' to " + xmltag()
				      + " failed while it already has set='"
				      + sett() + "'" );
    }
    doc()->incrRef( child->annotation_type(), sett() );
  }

  FoliaElement *AbstractAnnotationLayer::append( FoliaElement *child ){
    /// append child to an AbstractAnnotationLayer
    /*!
     * \param child the node to add
     * \return the appended child
     *
     * will throw on error
     * Make sure that the layer has the same 'set' as the new child
     */
    assignset( child );
    return AbstractElement::append( child );
  }

  KWargs AbstractAnnotationLayer::collectAttributes() const {
    /// extract all Attribute-Value pairs for AbstractAnnotationLayer
    /*!
     * \return a KWargs set of Attribute-value pairs
     * inclusive: set
     */
    KWargs attribs = AbstractElement::collectAttributes();
    auto it = attribs.find("set");
    if ( it != attribs.end() ) {
      attribs.erase(it);
    }
    return attribs;
  }

  xmlNode *AbstractSpanAnnotation::xml( bool recursive, bool kanon ) const {
    ///  convert an SpanAnnotation to an xmlNode
    /*!
     * \param recursive recurse into children, producing an XmlNode tree.
     * \param kanon if true, output in a canonical way.
     * \return The created XmlNode (-tree)
     *
     * Extra care has to be taken for referable children. These should be added
     * as Wref, except for there first occurrence in the document.
     */
    xmlNode *e = AbstractElement::xml( false, false );
    // append referable children as WREFS
    //   EXCEPT when there are NO references to it
    for ( const auto& el : data() ) {
      if ( el-referable()
	   && el->refcount() > 0 ){
	xmlNode *t = XmlNewNode( foliaNs(), "wref" );
	KWargs attribs;
	attribs["id"] = el->id();
	string txt = el->str( el->textclass() );
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

  xmlNode *Content::xml( bool recursive, bool ) const {
    ///  convert a Content node to an xmlNode
    /*!
     * \param recursive recurse into children, producing an XmlNode tree.
     * \return The created XmlNode (-tree) with the value of Content added
     * as a CData block
     */
    xmlNode *e = AbstractElement::xml( recursive, false );
    xmlAddChild( e, xmlNewCDataBlock( 0,
				      (const xmlChar*)value.c_str() ,
				      value.length() ) );
    return e;
  }

  void Content::setAttributes( KWargs& kwargs ){
    /// set the Contents attributes given a set of Key-Value pairs.
    /*!
     * \param kwargs a KWargs set of Key-Value pairs
     *
     * checks and sets the special attributes for Content: value
     */
    value = kwargs.extract( "value" );
    AbstractElement::setAttributes( kwargs );
  }

  FoliaElement* Content::parseXml( const xmlNode *node ) {
    /// parse a Content node at node
    /*!
     * \param node a Content
     * \return the parsed tree. Throws on error.
     * A content can also be a CDATA section
     */
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
	value += TextValue( p );
	isCdata = !value.empty();
      }
      else if ( p->type == XML_TEXT_NODE ) {
	// "empty" text nodes may appear before or after CDATA
	// we just ignore those
	string tmp = TextValue( p );
	tmp = TiCC::trim(tmp);
	if ( !tmp.empty()
	     && isCdata ) {
	  throw XmlError( "intermixing CDATA and text in Content node" );
	}
	isText = !tmp.empty();
	value += tmp;
      }
      else if ( p->type == XML_COMMENT_NODE ) {
	string tag = "_XmlComment";
	FoliaElement *t = createElement( tag, doc() );
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

  //#define DEBUG_TEXT
  const UnicodeString Correction::private_text( const TextPolicy& tp ) const {
    /// get the UnicodeString value of an Correction
    /*!
     * \param cls The textclass we are looking for
     * \param retaintok retain the tokenisation information
     * \return the Unicode String representation found. Throws when
     * no text can be found.
     */
#ifdef DEBUG_TEXT
    cerr << "TEXT(" << cls << ") on node : " << xmltag() << " id="
	 << id() << " TextPolicy: " << tp << endl;
#endif
    // we cannot use text_content() on New, Original or Current,
    // because textcontent doesn't recurse!
    bool deletion = false;
    UnicodeString new_result;
    UnicodeString org_result;
    UnicodeString cur_result;
    for ( const auto& el : data() ) {
#ifdef DEBUG_TEXT
      cerr << "data=" << el << endl;
#endif
      if ( el->isinstance( New_t ) ){
	if ( el->size() == 0 ){
	  deletion = true;
	}
	else {
	  try {
	    new_result = el->private_text( tp );
	  }
	  catch ( ... ){
	    // try other nodes
	  }
	}
      }
      else if ( el->isinstance( Original_t ) ){
	try {
	  org_result = el->private_text( tp );
	}
	catch ( ... ){
	  // try other nodes
	}
      }
      else if ( el->isinstance( Current_t ) ){
	try {
	  cur_result = el->private_text( tp );
	}
	catch ( ... ){
	  // try other nodes
	}
      }
    }
    if ( !deletion ){
      if ( !new_result.isEmpty() ){
#ifdef DEBUG_TEXT
	cerr << "return new text '" << new_result << "'" << endl;
#endif
	return new_result;
      }
      else if ( !cur_result.isEmpty() ){
#ifdef DEBUG_TEXT
	cerr << "return cur text '" << cur_result << "'" << endl;
#endif
	return cur_result;
      }
      else if ( !org_result.isEmpty() ){
#ifdef DEBUG_TEXT
	cerr << "return ori text '" << org_result << "'" << endl;
#endif
	return org_result;
      }
    }
    else {
      if ( !cur_result.isEmpty() ){
#ifdef DEBUG_TEXT
	cerr << "Deletion: return cur text '" << cur_result << "'" << endl;
#endif
	return cur_result;
      }
    }
    throw NoSuchText( "cls=" + tp._class );
  }
  //#undef DEBUG_TEXT

  const string& Correction::get_delimiter( bool retaintok ) const {
    /// get the default delimiter of a Correction
    /*!
     * \param retaintok retain the tokenization assigned to this element
     * \return a string representing the delimiter
     */
    for ( const auto& el : data() ) {
      //      if ( el->isinstance( New_t ) || el->isinstance( Current_t ) ) {
      return el->get_delimiter( retaintok );
      //      }
    }
    return EMPTY_STRING;
  }

  const TextContent *Correction::text_content( const string& cls,
					       bool show_hidden ) const {
    /// Get the TextContent explicitly associated with a Correction
    /*!
     * \param cls the textclass to search for
     * \param show_hidden if true also return text of 'hidden' nodes
     *
     * Returns the TextContent instance rather than the actual text.
     * (so it might return iself.. ;)
     * recurses into children looking for New or Current nodes
     * might throw NoSuchText exception if not found.
     */
    // TODO: this implements correctionhandling::EITHER only
    for ( const auto& el : data() ) {
      if ( el->isinstance( New_t ) || el->isinstance( Current_t ) ) {
	try {
	  const TextContent *res = el->text_content( cls, show_hidden );
	  return res;
	}
	catch (...){
	}
      }
    }
    for ( const auto& el : data() ) {
      if ( el->isinstance( Original_t ) ) {
	try {
	  const TextContent *res = el->text_content( cls, show_hidden );
	  return res;
	}
	catch ( ... ){
	}
      }
      else if ( cls == "current" && el->hastext( "original" ) ){
	cerr << "text(original)= "
	     << el->text_content( cls, show_hidden )->text()<< endl;
	// hack for old and erroneous behaviour
	return el->text_content( "original", show_hidden );
      }
    }
    throw NoSuchText("wrong cls");
  }

  Correction *Correction::correct( const std::vector<FoliaElement*>&,
				   const std::vector<FoliaElement*>& vc,
				   const std::vector<FoliaElement*>& vn,
				   const std::vector<FoliaElement*>& vs,
				   const KWargs& args){
    /// correct a Correction
    /*!
     * \param vc a group of nodes to add to the Current
     * \param vn a group of nodes to replace Original, added to New
     * \param vs a group of nodes to add to Suggestions
     * \param args additional arguments
     * \return a new Correction node. The old Coorection is added as Original
     */
    vector<FoliaElement*> new_vo; // ignore users hints about original
    new_vo.push_back( this );
    return parent()->correct( new_vo, vc, vn, vs, args );
  }

  Correction *Correction::correct( const std::string& args ){
    /// correct a Correction using an Attribute-value list
    /*!
     * \param args the list to parse into a KWargs struct
     * \return a new Correction node. The old Correction is added as Original
     */
    vector<FoliaElement*> nv;
    vector<FoliaElement*> ov;
    vector<FoliaElement*> cv;
    vector<FoliaElement*> sv;
    ov.push_back( this );
    return parent()->correct( ov,nv,cv,sv, args );
  }

  Correction *New::correct( const std::vector<FoliaElement*>& vo,
			    const std::vector<FoliaElement*>& vc,
			    const std::vector<FoliaElement*>& vn,
			    const std::vector<FoliaElement*>& vs,
			    const KWargs& args){
    /// create a Correction on the PARENT of this New node.
    /*!
     * \param vo a group of nodes to add to the Original (ignored i assume)
     * \param vc a group of nodes to add to the Current
     * \param vn a group of nodes to replace Original, added to New
     * \param vs a group of nodes to add to Suggestions
     * \param args additional arguments
     * \return the Correction node. Might throw on problems
     */
    return parent()->correct( vo, vc, vn, vs, args );
  }

  Correction *New::correct( const std::string& args ){
    /// use an Attribute-Value list to create a Correction on the parent
    /// of this New node.
    /*!
     * \param args a string representation of a Attribute-Value list
     * \return the created Correcion
     * The parameter is converted to a KWargs list which is handled over
     * to use correct() on the PARENT (Correction) of this node
     */
    return parent()->correct( args );
  }

  const PhonContent *Correction::phon_content( const string& cls,
					       bool show_hidden ) const {
    /// Get the PhonContent explicitly associated with this element.
    /*!
     * \param cls the textclass to search for
     * \param show_hidden if true also return text og 'hidden' nodes
     *
     * Returns the PhonContent instance rather than the actual text.
     * (so it might return iself.. ;)
     * recurses into children looking for New or Current
     * might throw NoSuchPhon exception if not found.
     */
    // TODO: this implements correctionhandling::EITHER only
    for ( const auto& el: data() ) {
      if ( el->isinstance( New_t ) || el->isinstance( Current_t ) ) {
	return el->phon_content( cls, show_hidden );
      }
    }
    for ( const auto& el: data() ) {
      if ( el->isinstance( Original_t ) ) {
	return el->phon_content( cls, show_hidden );
      }
    }
    throw NoSuchPhon("wrong cls");
  }

  bool Correction::hasNew() const {
    ///  check if this Correction has a New node
    vector<FoliaElement*> v = select( New_t, SELECT_FLAGS::LOCAL );
    return !v.empty();
  }

  New *Correction::getNew() const {
    /// extract the New node of a Correction
    /*!
     * \return the new node or 0 if not available
     */
    vector<New*> v = FoliaElement::select<New>( false );
    if ( v.empty() ) {
      return 0;
    }
    return v[0];
  }

  FoliaElement *Correction::getNew( size_t index ) const {
    /// extract the child at position 'index' in the New of a Correction
    /*!
     * \param index the position in the children of the New node
     * \return the child or 0 if not available
     */
    New *n = getNew();
    return n->index(index);
  }

  bool Correction::hasOriginal() const {
    ///  check if this Correction has an Original node
    vector<FoliaElement*> v = select( Original_t, SELECT_FLAGS::LOCAL );
    return !v.empty();
  }

  Original *Correction::getOriginal() const {
    /// extract the Original node of a Correction
    /*!
     * \return the new node or 0 if not available
     */
    vector<Original*> v = FoliaElement::select<Original>( false );
    if ( v.empty() ) {
      return 0;
    }
    return v[0];
  }

  FoliaElement *Correction::getOriginal( size_t index ) const {
    /// extract the child at position 'index' in the Original of a Correction
    /*!
     * \param index the position in the children of the Original node
     * \return the child or 0 if not available
     */
    Original *n = getOriginal();
    return n->index(index);
  }

  bool Correction::hasCurrent( ) const {
    ///  check if this Correction has a New node
    vector<FoliaElement*> v = select( Current_t, SELECT_FLAGS::LOCAL );
    return !v.empty();
  }

  Current *Correction::getCurrent( ) const {
    /// extract the Cuurent node of a Correction
    /*!
     * \return the new node or 0 if not available
     */
    vector<Current*> v = FoliaElement::select<Current>( false );
    if ( v.empty() ) {
      throw NoSuchAnnotation( "current" );
    }
    return v[0];
  }

  FoliaElement *Correction::getCurrent( size_t index ) const {
    /// extract the child at position 'index' in the Current of a Correction
    /*!
     * \param index the position in the children of the Current node
     * \return the child or 0 if not available
     */
    Current *n = getCurrent();
    return n->index(index);
  }

  bool Correction::hasSuggestions( ) const {
    ///  check if this Correction has Suggestion nodes
    vector<Suggestion*> v = suggestions();
    return !v.empty();
  }

  vector<Suggestion*> Correction::suggestions( ) const {
    /// get all Suggestion nodes of this Correction
    return FoliaElement::select<Suggestion>( false );
  }

  Suggestion *Correction::suggestions( size_t index ) const {
    /// extract the Suggestion at position 'index' of a Correction
    /*!
     * \param index the position in list of Suggestion nodes
     * \return the Suggestion or 0 if not available
     */
    vector<Suggestion*> v = suggestions();
    if ( v.empty() || index >= v.size() ) {
      throw NoSuchAnnotation( "suggestion" );
    }
    return v[index];
  }

  Head *Division::head() const {
    /// return the Head node in a Division
    /*!
     * \return the element with ElementType Head_t. throws when not found.
     */
    const vector<FoliaElement*>& data = this->data();
    for ( const auto& h : data ){
      if ( h->element_id() == Head_t ) {
	return dynamic_cast<Head*>(h);
      }
    }
    throw runtime_error( "No head" );
  }

  const string Gap::content() const {
    /// return the content of a Gap
    /*!
     * \return the an UTF8 string of the content in the Gap. Throws if not found
     */
    vector<FoliaElement*> cv = select( Content_t );
    if ( cv.empty() ) {
      throw NoSuchAnnotation( "content" );
    }
    return cv[0]->content();
  }

  Headspan *Dependency::head() const {
    /// return the HeadSpan node in a Dependency
    /*!
     * \return the element with ElementType Headspan. throws when not found.
     */
    vector<Headspan*> v = FoliaElement::select<Headspan>();
    if ( v.size() < 1 ) {
      throw NoSuchAnnotation( "head" );
    }
    return v[0];
  }

  DependencyDependent *Dependency::dependent() const {
    /// return the DependencyDependent node in a Dependency
    /*!
     * \return the element with ElementType DependencyDependent.
     *  throws when not found.
     */
    vector<DependencyDependent *> v = FoliaElement::select<DependencyDependent>();
    if ( v.empty() ) {
      throw NoSuchAnnotation( "dependent" );
    }
    return v[0];
  }

  vector<AbstractSpanAnnotation*> AbstractElement::selectSpan() const {
    /// select als SpanAnnotation nodes in the FoliaElement
    /*!
     * \return a list of SpanAnnotation nodes.
     * All possible Span types are collected in this list. (see SpanSet)
     */
    vector<AbstractSpanAnnotation*> res;
    for ( const auto& el : SpanSet ) {
      vector<FoliaElement*> tmp = select( el );
      for ( auto& sp : tmp ) {
	res.push_back( dynamic_cast<AbstractSpanAnnotation*>( sp ) );
      }
    }
    return res;
  }

  vector<FoliaElement*> AbstractSpanAnnotation::wrefs() const {
    /// select all referable Elements present in this object
    /*!
     * \return al list of FoliAElements
     * recurses through al the children to look for referable nodes
     * (see WREFABLE) and collects them in one list.
     */
    vector<FoliaElement*> res;
    for ( const auto& el : data() ) {
      ElementType et = el->element_id();
      if ( el->referable()
	   || et == WordReference_t ){
	res.push_back( el );
      }
      else {
	AbstractSpanAnnotation *as = dynamic_cast<AbstractSpanAnnotation*>(el);
	if ( as != 0 ) {
	  vector<FoliaElement*> sub = as->wrefs();
	  copy( sub.begin(), sub.end(), back_inserter(res) );
	}
      }
    }
    return res;
  }

  FoliaElement *AbstractSpanAnnotation::wrefs( size_t pos ) const {
    /// select the referable Element at position pos
    /*!
     * \return a referable FoliAElement or 0, when not found.
     * recurses through al the children to look for referable nodes
     * (see WREFABLE) and collects them in one list.
     */
    vector<FoliaElement*> v = wrefs();
    if ( pos < v.size() ) {
      return v[pos];
    }
    return 0;
  }

  AbstractSpanAnnotation *AbstractAnnotationLayer::findspan( const vector<FoliaElement*>& words ) const {
    /// find the SpanAnnotation which spans the whole list of words
    /*!
     * \param words a list of nodes
     * \return the spanning element, or 0 if not found.
     * All available SpanAnnotations are search for one that spans EXACTLY
     * the 'words' list
     */
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

  UnicodeString dumb_spaces( const UnicodeString& is ){
    UnicodeString os;
    for ( int i=0; i < is.length(); ++i ){
      if ( u_isspace( is[i] )
	   && !( ( is[i] == '\t' ) || is[i] == '\n' || is[i] == '\r' ) ){
	os += " ";
      }
      else {
	os += is[i];
      }
    }
    return os;
  }

  bool XmlText::setvalue( const std::string& s ){
    /// set the value of an XmlText element in NFC endcoded UTF8
    /*!
     * \param s an UTF8 string
     * \return true always
     */
    static TiCC::UnicodeNormalizer norm;  // defaults to a NFC normalizer
    if ( !s.empty() ){
      UnicodeString us = TiCC::UnicodeFromUTF8(s);
      us = norm.normalize( us );
      us = dumb_spaces( us );
      _value = TiCC::UnicodeToUTF8( us );
    }
    return true;
  }

  const UnicodeString XmlText::private_text( const TextPolicy& ) const {
    /// get the UnicodeString value of an XmlText element
    /*!
     */
    return TiCC::UnicodeFromUTF8(_value);
  }

  xmlNode *XmlText::xml( bool, bool ) const {
    ///  convert an XmlText node to an xmlNode
    return xmlNewText( (const xmlChar*)_value.c_str() );
  }

  FoliaElement* XmlText::parseXml( const xmlNode *node ) {
    /// parse a Xmltext node at node
    /*!
     * \param node an XmlText
     * \return the parsed tree. Throws on error.
     */
    string txt = TextValue( node );
    txt = trim( txt );
    if ( txt.empty() ) {
      throw ValueError( "TextContent may not be empty" );
    }
    setvalue( txt );
    return this;
  }

  static void error_sink(void *mydata, xmlError *error ) {
    /// helper function for Xml parsing
    int *cnt = (int*)mydata;
    if ( *cnt == 0 ) {
      cerr << "\nXML-error: " << error->message << endl;
    }
    (*cnt)++;
  }

  void External::resolve_external( ) {
    /// resolve external references
    /*!
     * given the src location of the External node, try to parse a FoLiA
     * document from that location (URL) and insert it's Text part into our
     * document.
     * might fail in numourous ways!
     */
    string src;
    try {
      src = AbstractElement::src();
      cerr << "try to resolve: " << src << endl;
      int cnt = 0;
      xmlSetStructuredErrorFunc( &cnt, (xmlStructuredErrorFunc)error_sink );
      xmlDoc *extdoc = xmlReadFile( src.c_str(), 0, XML_PARSER_OPTIONS );
      if ( extdoc ) {
	xmlNode *root = xmlDocGetRootElement( extdoc );
	xmlNode *p = root->children;
	while ( p ) {
	  if ( p->type == XML_ELEMENT_NODE ) {
	    string tag = Name( p );
	    if ( tag == "text" ) {
	      const string bogus_id = "Arglebargleglop-glyf";
	      FoliaElement *par = parent();
	      KWargs args = par->collectAttributes();
	      args["xml:id"] = bogus_id;
	      Text *tmp = new Text( args, doc() );
	      tmp->AbstractElement::parseXml( p );
	      FoliaElement *old = par->replace( this, tmp->index(0) );
	      doc()->del_doc_index( bogus_id );
	      tmp->remove( (size_t)0, false );
	      delete tmp;
	      delete old;
	    }
	  }
	  p = p->next;
	}
	xmlFreeDoc( extdoc );
      }
      else {
	throw XmlError( "resolving external " + src + " failed" );
      }
    }
    catch ( const exception& e ) {
      throw XmlError( "resolving external " + src + " failed: "
		      + e.what() );
    }
  }

  FoliaElement* External::parseXml( const xmlNode *node ) {
    /// parse an External node at node
    /*!
     * \param node an External
     * \return the parsed tree. Throws on error.
     * if succesful, the external is added to the external documents list of
     * the associated Document
     */
   KWargs att = getAttributes( node );
    setAttributes( att );
    /*if ( _include ) {
      doc()->addExternal( this );
    }*/
    return this;
  }

  void Note::setAttributes( KWargs& kwargs ) {
    /// set the Node attributes given a set of Key-Value pairs.
    /*!
     * \param kwargs a KWargs set of Key-Value pairs
     *
     * checks and sets the special attributes for Note: id
     */
    ref_id = kwargs.extract( "id" );
    AbstractElement::setAttributes( kwargs );
  }

  KWargs Reference::collectAttributes() const {
    /// extract all Attribute-Value pairs for Reference
    /*!
     * \return a KWargs set of Attribute-value pairs
     * inclusive: id, type, format
     */
    KWargs atts = AbstractElement::collectAttributes();
    if ( !ref_id.empty() ){
      atts["id"] = ref_id;
    }
    if ( !ref_type.empty() ){
      atts["type"] = ref_type;
    }
    if ( !_format.empty() && _format != "text/folia+xml" ) {
      atts["format"] = _format;
    }
    KWargs more = AllowXlink::collectAttributes();
    atts.insert( more.begin(), more.end() );
    return atts;
  }

  void Reference::setAttributes( KWargs& kwargs ) {
    /// set the Reference attributes given a set of Key-Value pairs.
    /*!
     * \param kwargs a KWargs set of Key-Value pairs
     *
     * checks and sets the special attributes for Reference: id, type, format
     */
    ref_id = kwargs.extract( "id" );
    ref_type = kwargs.extract( "type" );
    _format = kwargs.extract( "format" );
    AllowXlink::setAttributes(kwargs);
    AbstractElement::setAttributes(kwargs);
  }

  KWargs TextMarkupReference::collectAttributes() const {
    /// extract all Attribute-Value pairs for TextMarkupReference
    /*!
     * \return a KWargs set of Attribute-value pairs
     * inclusive: id, type and format
     */
    KWargs atts = AbstractTextMarkup::collectAttributes();
    if ( !ref_id.empty() ){
      atts["id"] = ref_id;
    }
    if ( !ref_type.empty() ){
      atts["type"] = ref_type;
    }
    if ( !_format.empty() && _format != "text/folia+xml" ) {
      atts["format"] = _format;
    }
    return atts;
  }

  void TextMarkupReference::setAttributes( KWargs& kwargs ) {
    /// set the TextMarkupReference attributes given a set of Key-Value pairs.
    /*!
     * \param kwargs a KWargs set of Key-Value pairs
     *
     * checks and sets the special attributes for TextMarkupReference:
     * id, type, format
     */
    ref_id = kwargs.extract( "id" );
    ref_type = kwargs.extract( "type" );
    _format = kwargs.extract( "format" );
    AbstractTextMarkup::setAttributes(kwargs);
  }

  xmlNode *XmlComment::xml( bool, bool ) const {
    ///  convert an XmlComment node to an xmlNode
    return xmlNewComment( (const xmlChar*)_value.c_str() );
  }

  FoliaElement* XmlComment::parseXml( const xmlNode *node ) {
    /// parse a XmlComment node
    /*!
     * \param node an XmlComment
     * \return the parsed tree. Throws on error.
     */
    _value = TextValue( node );
    return this;
  }

  KWargs Suggestion::collectAttributes() const {
    /// extract all Attribute-Value pairs for Suggestion
    /*!
     * \return a KWargs set of Attribute-value pairs
     * inclusive: split and merge
     */
    KWargs atts = AbstractElement::collectAttributes();
    if ( !_split.empty() ) {
      atts["split"] = _split;
    }
    if ( !_merge.empty() ) {
      atts["merge"] = _merge;
    }
    return atts;
  }

  void Suggestion::setAttributes( KWargs& kwargs ) {
    /// set the Suggestion attributes given a set of Key-Value pairs.
    /*!
     * \param kwargs a KWargs set of Key-Value pairs
     *
     * checks and sets the special attributes for Suggestion:
     * split, merge
     */
    _split = kwargs.extract( "split" );
    _merge = kwargs.extract( "merge" );
    AbstractElement::setAttributes(kwargs);
  }


  void Feature::setAttributes( KWargs& kwargs ) {
    /// set the Feature attributes given a set of Key-Value pairs.
    /*!
     * \param kwargs a KWargs set of Key-Value pairs
     *
     * checks and sets the special attributes for Feature
     * subset, class
     * \note Feature is special. So DON'T call setAttributes
     */
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
    update_cls( it->second );
  }

  KWargs Feature::collectAttributes() const {
    /// extract all Attribute-Value pairs for Feature
    /*!
     * \return a KWargs set of Attribute-value pairs
     * inclusive: subset
     */
    KWargs attribs = AbstractElement::collectAttributes();
    attribs["subset"] = _subset;
    return attribs;
  }

  vector<string> AbstractElement::feats( const string& s ) const {
    /// return all classes of the given subset
    /*!
     * \param s a subset name
     * \return a list of all classes in the subset of the Feature nodes
     * The function loops through all children and for Feature_t children
     * it check the subset and collects the matching ones
     */
    vector<string> result;
    for ( const auto& el : data() ) {
      if ( el->isSubClass( Feature_t ) &&
	   el->subset() == s ) {
	result.push_back( el->cls() );
      }
    }
    return result;
  }

  const string AbstractElement::feat( const string& s ) const {
    /// return the class of the first matching Feature with subset s
    /*!
     * \param s a subset name
     * \return the first class of the first Feature node in subset s
     */
    for ( const auto& el : data() ) {
      if ( el->isSubClass( Feature_t ) &&
	   el->subset() == s ) {
	return el->cls();
      }
    }
    return "";
  }

  ForeignMetaData::~ForeignMetaData(){
    /// destructor for ForeignMetaData
    for ( const auto& it : foreigners ){
      delete it;
    }
  }

  void ForeignMetaData::add_foreign( const xmlNode *node ){
    /// add a ForeignData node
    /*!
     * \param node a (foreign by definition) node
     */
    ForeignData *fd = new ForeignData();
    fd->set_data( node );
    foreigners.push_back( fd );
  }

  ForeignData::~ForeignData(){
    /// destructor for ForeignData
    xmlFreeNode( _foreign_data );
  }

  FoliaElement* ForeignData::parseXml( const xmlNode *node ){
    /// parse a ForeignData
    /*!
     * \param node an ForeignData node
     * \return the parsed tree. Throws on error.
     */
    set_data( node );
    return this;
  }

  xmlNode *ForeignData::xml( bool, bool ) const {
    /// retrieve the data of the ForeignData node as an xmlNode-tree
    return get_data();
  }

  void ForeignData::set_data( const xmlNode *node ){
    /// assign node to _foreign_data
    /*!
     * performs sanity check to avoid adding FoLiA nodes
     */
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
    /// strip the NameSpace with value ns from the node
    /*!
     * \param node the xmlNode to work on
     * \param ns the HREF value of the namespace to remove
     */
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
    /// get the _foreign_data as an xmlNode tree
    /*!
     * \return a COPY of the _foreign_daya member.
     * With cleaned-out FoLiA namespace declarations.
     */
    xmlNode *result = xmlCopyNode(_foreign_data, 1 );
    clean_ns( result, NSFOLIA ); // Sanity: remove FoLiA namespace defs, if any
    return result;
  }

  const MetaData* AbstractElement::get_metadata() const {
    /// Get the MetaData node related to this element
    /*!
     * \return the _metadata or 0 if not available
     * may recurse upwards through the parent nodes
     */
    if ( !_metadata.empty() && doc() ){
      return doc()->get_submetadata(_metadata);
    }
    else if ( parent() ){
      return parent()->get_metadata();
    }
    else {
      return 0;
    }
  }

  const string AbstractElement::get_metadata( const string& key ) const {
    /// Get the metadata value for this key
    /*!
     * \param key which metadata field do we want?
     * \return the metadata value for this key
     */
    if ( !_metadata.empty() && doc() ){
      const MetaData *what = doc()->get_submetadata(_metadata);
      if ( what && what->datatype() == "NativeMetaData" && !key.empty() ){
	return what->get_val( key );
      }
      return "";
    }
    else if ( parent() ){
      return parent()->get_metadata( key );
    }
    else {
      return "";
    }
  }

  KWargs AbstractTextMarkup::collectAttributes() const {
    /// extract all Attribute-Value pairs for AbstractTextMarkup
    /*!
     * \return a KWargs set of Attribute-value pairs
     * inclusive: id
     */
    KWargs attribs = AbstractElement::collectAttributes();
    if ( !idref.empty() ) {
      attribs["id"] = idref;
    }
    KWargs more = AllowXlink::collectAttributes();
    attribs.insert( more.begin(), more.end() );
    return attribs;
  }

  void AbstractTextMarkup::setAttributes( KWargs& kwargs ) {
    /// set the AbstractTextMarkup attributes given a set of Key-Value pairs.
    /*!
     * \param kwargs a KWargs set of Key-Value pairs
     *
     * checks and sets the special attributes for AbstractTextMarkup:
     * id, text
     */
    auto it = kwargs.find( "id" );
    if ( it != kwargs.end() ) {
      auto it2 = kwargs.find( "xml:id" );
      if ( it2 != kwargs.end() ) {
	throw ValueError("Both 'id' and 'xml:id found for " + classname() );
      }
      idref = it->second;
      kwargs.erase( it );
    }
    it = kwargs.find( "text" );
    if ( it != kwargs.end() ) {
      XmlText *txt = new XmlText();
      txt->setvalue( it->second );
      append(txt);
      kwargs.erase( it );
    }
    AllowXlink::setAttributes( kwargs );
    AbstractElement::setAttributes( kwargs );
  }

  KWargs TextMarkupCorrection::collectAttributes() const {
    /// extract all Attribute-Value pairs for TextMarkupCorrection
    /*!
     * \return a KWargs set of Attribute-value pairs
     * inclusive: original
     */
    KWargs attribs = AbstractTextMarkup::collectAttributes();
    if ( !_original.empty() ) {
      attribs["original"] = _original;
    }
    return attribs;
  }

  void TextMarkupCorrection::setAttributes( KWargs& kwargs ) {
    /// set the TextMarkupCorrection attributes given a set of Key-Value pairs.
    /*!
     * \param kwargs a KWargs set of Key-Value pairs
     *
     * checks and sets the special attributes for TextMarkupCorrection:
     * id, original
     */
    idref = kwargs.extract( "id" );
    _original = kwargs.extract( "original" );
    AbstractElement::setAttributes( kwargs );
  }

  const UnicodeString TextMarkupCorrection::private_text( const TextPolicy& tp ) const {
    /// get the UnicodeString value of a TextMarkupCorrection element
    /*!
     * \param cls The textclass we are looking for
     * \param retaintok retain the tokenisation information
     * \param strict If true, return the text of this level only
     * when false, allow recursing into children
     * \param show_hidden include text form 'hidden' nodes too.
     * \return the Unicode String representation found. Throws when
     * no text can be found
     */
    // cerr << "TEXT MARKUP CORRECTION " << this << endl;
    // cerr << "TEXT MARKUP CORRECTION parent cls=" << parent()->cls() << endl;
    if ( tp._class == "original" ) {
      return TiCC::UnicodeFromUTF8(_original);
    }
    return AbstractElement::private_text( tp );
  }

  const UnicodeString TextMarkupHSpace::private_text( const TextPolicy& ) const {
    /// get the UnicodeString value of a TextMarkupHSpace element
    /*!
     * \return A single space.
     */
#ifdef DEBUG_TEXT
    cerr << "TEXT MARKUP HSPACE " << " return ' '" << endl;
#endif
    return " ";
  }

  const FoliaElement* AbstractTextMarkup::resolveid() const {
    /// return the FoliaElement refered to by idref
    /*!
     * \return the refered FoliaElement, or 'this' if not applicable
     */
    if ( idref.empty() || !doc() ) {
      return this;
    }
    else {
      return doc()->index(idref);
    }
  }

  void TextContent::init() {
    /// set default value on creation
    _offset = -1;
  }

  void PhonContent::init() {
    /// set default value on creation
    _offset = -1;
  }

  void Linebreak::init() {
    /// set default value on creation
    _newpage = false;
  }

  void Relation::init() {
    /// set default value on creation
    _format = "text/folia+xml";
  }

  void Reference::init() {
    /// set default value on creation
    _format = "text/folia+xml";
  }

  void TextMarkupReference::init() {
    /// set default value on creation
    _format = "text/folia+xml";
  }

  void ForeignData::init() {
    /// set default value on creation
    _foreign_data = 0;
  }

} // namespace folia
