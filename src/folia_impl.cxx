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

static TiCC::LogStream DBG_CERR(cerr,NoStamp);
#define DBG *TiCC::Log((_dbg_file?_dbg_file:&DBG_CERR))

namespace folia {
  using TiCC::operator <<;

  string VersionName() { return PACKAGE_STRING; } ///< Returns the PACKAGE_STRING info of the package
  string Version() { return VERSION; }  ///< Returns version of the library

  const ElementType& AbstractElement::element_id() const {
    /// return the ELEMENT_ID property
    return _props.ELEMENT_ID;
  }

  const size_t& AbstractElement::occurrences() const {
    /// return the OCCURENCES property
    return _props.OCCURRENCES;
  }

  const size_t& AbstractElement::occurrences_per_set() const {
    /// return the OCCURRENCES_PER_SET property
    return _props.OCCURRENCES_PER_SET;
  }

  const Attrib& AbstractElement::required_attributes() const {
    /// return the REQUIRED_ATTRIBUTES property
    return _props.REQUIRED_ATTRIBS;
  }

  const Attrib& AbstractElement::optional_attributes() const {
    /// return the OPTONAL_ATTRIBUTES property
    return _props.OPTIONAL_ATTRIBS;
  }

  const bool& AbstractElement::hidden() const {
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

  const AnnotationType& AbstractElement::annotation_type() const {
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

  const bool& AbstractElement::printable() const {
    /// return the PRINTABLE property
    return _props.PRINTABLE;
  }

  const bool& AbstractElement::speakable() const {
    /// return the SPEAKABLE property
    return _props.SPEAKABLE;
  }

  const bool& AbstractElement::referable() const {
    /// return the WREFABLE property
    return _props.WREFABLE;
  }

  const bool& AbstractElement::is_textcontainer() const {
    /// return the TEXTCONTAINER property
    return _props.TEXTCONTAINER;
  }

  const bool& AbstractElement::implicitspace() const {
    /// return the IMPLICITSPACE property
    return _props.IMPLICITSPACE;
  }

  const bool& AbstractElement::is_phoncontainer() const {
    /// return the PHONCONTAINER property
    return _props.PHONCONTAINER;
  }

  const string& AbstractElement::text_delimiter() const {
    /// return the TEXTDELIMITER property
    return _props.TEXTDELIMITER;
  }

  const bool& AbstractElement::xlink() const {
    /// return the XLINK property
    return _props.XLINK;
  }

  const bool& AbstractElement::auth() const {
    /// return the AUTH property
    return _props.AUTH;
  }

  const bool& AbstractElement::setonly() const {
    /// return the SETONLY property
    return _props.SETONLY;
  }

  const bool& AbstractElement::auto_generate_id() const {
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

  const string AbstractElement::set_tag( const string& t ) {
    /// set a value for the _tags attribute
    /*!
     * \param t the new value (may be empty)
     * \return the old value (can be empty)
     * thows when the FoliaElement doesn't support the tag attribute
     */
    Attrib supported = required_attributes() | optional_attributes();
    if ( !( supported % Attrib::TAG ) ) {
      throw ValueError( this,
			"settag() is not supported for " + classname() );
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
      os << " xml:id=\"" << ae.id() << '"';
      ats.erase("xml:id");
    }

    for ( const auto& [att,val] : ats ) {
      os << " " << att << "=\"" << val << '"';
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

  void AbstractElement::dbg( const string& msg ) const{
    DBG << msg << ": " << "<" << AbstractElement::xmltag() << ">"
	 << " address=" << reinterpret_cast<const void*>(this) << endl;
  }

  //  #define DE_AND_CONSTRUCT_DEBUG

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
    _annotator_type(AnnotatorType::UNDEFINED),
    _refcount(0),
    _line_no(-1),
    _confidence(-1),
    _preserve_spaces(SPACE_FLAGS::UNSET),
    _props(p)
  {
    if ( d && d->debug % DocDbg::MEMORY ){
      dbg( "AbstractElement::created" );
    }
  }

  AbstractElement::AbstractElement( const properties& p, FoliaElement *el ) :
    /// Constructor for AbstractElements.
    /*!
     * \param p a properties block (required)
     * \param el a parent node, to append to
     */
    AbstractElement( p, el->doc() )
  {
    el->append( this );
  }

  //#define DE_AND_CONSTRUCT_DEBUG
  AbstractElement::~AbstractElement( ) {
#ifdef DE_AND_CONSTRUCT_DEBUG
    dbg( "really delete" );
#endif
  }

  void AbstractElement::destroy( ) {
    /// Pseudo destructor for AbstractElements.
    /// recursively destroys this nodes and it's children
    /// Will also remove it from it's parent when no references are left
    if ( doc() ) {
      if ( doc()->debug % DocDbg::MEMORY ){
	dbg( "\ndestroy" );
	DBG << "  id=" << _id << " class= "
	     << cls() << " datasize= " << _data.size() << endl;
	DBG << "  REFCOUNT = " << refcount() << endl;
	DBG << "  AT= " << annotation_type() << " (" << _set << ")" << endl;
      }
      doc()->decrRef( annotation_type(), _set );
      if ( refcount() > 0 ){
	decrefcount();
	doc()->keepForDeletion( this );
	if ( doc()->debug % DocDbg::MEMORY ){
	  DBG << "\t\tstill keeping element id=" << _id << " ";
	  dbg( "" );
	  DBG << " class= " << cls()
	       << " datasize= " << _data.size() << endl;
	}
	return;
      }
      doc()->del_doc_index( _id );
    }
    if ( _parent ){
      if ( doc() && doc()->debug % DocDbg::MEMORY ){
	DBG << "STILL A PARENT: " << _parent << endl;
      }
      _parent->remove( this );
    }
    else {
      if ( doc() && doc()->debug % DocDbg::MEMORY ){
	DBG << "Object has no PARENT: " << endl;
      }
    }
    if ( doc() && doc()->debug % DocDbg::MEMORY ){
      DBG << "object has " << _data.size() << " children" << endl;
    }
    for ( const auto& el : _data ) {
      el->set_parent(0);
      el->destroy();
    }
    _data.clear();
    if ( doc() && doc()->debug % DocDbg::MEMORY ){
      dbg( "\tfinished destroying element" );
      DBG << "\t id=" << _id << " class= " << cls()
	   << " datasize= " << _data.size() << endl;
    }
    delete this;
  }

  void destroy( FoliaElement *el ){
    if ( el ){
      el->destroy();
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

  void AbstractElement::check_set_declaration(){
    /// check the declation consistency of an object.
    /// throws an exception on error
    /*!
     * When the object has an associated document, the declaration of the
     * 'set' attribute is checked. Or the default set when no 'set' is provided
     * Also the presence of an appropiate annotation declaration is checked
     * for the annotation-type of the object. This might auto-declare
     * the anntotation-type, when de document allows this.
     */

    if ( isSubClass<AbstractCorrectionChild>() ){
      return;
    }

    if ( _mydoc ){
      string def;
      if ( !_set.empty() ){
	if ( !doc()->declared( annotation_type(), _set ) ) {
	  throw DeclarationError( this,
				  "Set '" + _set
				  + "' is used but has no declaration " +
				  "for " + toString( annotation_type() )
				  + "-annotation" );
	}
      }
      else {
	if ( _mydoc->debug % DocDbg::DECLARATIONS ) {
	  DBG << "get def for " <<  annotation_type() << endl;
	}
	def = doc()->default_set( annotation_type() );
	if ( doc()->debug % DocDbg::DECLARATIONS ) {
	  DBG << "got def='" <<  def << "'" << endl;
	}
	if ( doc()->is_incremental() && def.empty() ){
	  // when there is NO default set, AND we are parsing using
	  // folia::Engine, we must check if there WAS an empty set originally
	  // which is 'obscured' by newly added declarations
	  def = doc()->original_default_set( annotation_type() );
	  if ( doc()->debug % DocDbg::DECLARATIONS ) {
	    DBG << "from original got def='" <<  def << "'" << endl;
	  }
	}
	if ( !def.empty() ){
	  _set = def;
	}
	else if ( required_attributes() % Attrib::CLASS ){
	  throw XmlError( this,
			  "unable to assign a default set for tag: " + xmltag() );
	}
      }
      if ( annotation_type() != AnnotationType::NO_ANN
	   && !_mydoc->version_below( 2, 0 ) ){
	if ( !_mydoc->declared( annotation_type(), _set ) ){
	  if ( _mydoc->autodeclare() ){
	    _mydoc->auto_declare( annotation_type(), _set );
	  }
	  else {
	    throw DeclarationError( this,
				    "Encountered an instance of <"
				    + xmltag()
				    + "> without a proper "
				    + toString(annotation_type())
				    + "-annotation" );
	  }
	}
	else if ( _set.empty()
		  && !isSubClass<AbstractAnnotationLayer>()
		  && !doc()->declared( annotation_type(), "None" ) ){
	  if ( _mydoc->autodeclare() ){
	    _mydoc->auto_declare( annotation_type(), _set );
	  }
	  else {
	    throw DeclarationError( this,
				    "Encountered an instance of <"
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
      throw XmlError( this,
		      "only xlink:types: 'simple' and 'locator' are supported!" );
    }
    _xlink["type"] = type;
    val = kwargs.extract( "xlink:href" );
    if ( !val.empty() ) {
      _xlink["href"] = val;
    }
    else if ( type == "locator" ){
      throw XmlError( this,
		      "xlink:type='locator' requires an 'xlink:href' attribute" );
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
	throw XmlError( this,
			"xlink:type='simple' may not have an 'xlink:label' attribute" );
      }
      _xlink["label"] = val;
    }
    val = kwargs.extract( "xlink:arcrole" );
    if ( !val.empty() ) {
      if ( type == "locator" ){
	throw XmlError( this,
			"xlink:type='locator' may not have an 'xlink:arcrole' attribute" );
      }
      _xlink["arcrole"] = val;
    }
    val = kwargs.extract( "xlink:show" );
    if ( !val.empty() ) {
      if ( type == "locator" ){
	throw XmlError( this,
			"xlink:type='locator' may not have an 'xlink:show' attribute" );
      }
      _xlink["show"] = val;
    }
    val = kwargs.extract( "xlink:actuate" );
    if ( !val.empty() ) {
      if ( type == "locator" ){
	throw XmlError( this,
			"xlink:type='locator' may not have an 'xlink:actuate' attribute" );
      }
      _xlink["actuate"] = val;
    }
  }

  void AbstractElement::set_processor_name( const string& val ){
    if ( doc() && doc()->debug % DocDbg::PROVENANCE ){
      DBG << "set processor_name= " << val << " on " << classname() << endl;
    }
    if ( annotation_type() == AnnotationType::NO_ANN ){
      throw ValueError( this,
			"Unable to set processor on " + classname()
			+ ". AnnotationType is None!" );
    }
    if ( _set.empty() ){
      _set = "None";
    }
    else {
      if ( doc() && doc()->get_processor(val) == 0 ){
	throw ValueError( this,
			  "attribute 'processor' has unknown value: " + val );
      }
      if ( doc() ){
	auto annotators = doc()->get_annotators( annotation_type(), _set );
	bool found = false;
	for ( const auto& a : annotators ){
	  if ( a == val ) {
	    found = true;
	  }
	}
	if ( !found ){
	  if ( !doc()->version_below( 2, 0 )
	       && doc()->autodeclare() ) {
	    KWargs args;
	    args.add("processor",val);
	    args.add("annotatortype",toString(_annotator_type));
	    doc()->declare( annotation_type(), _set, args );
	  }
	  else {
	    throw DeclarationError( this,
				    "Processor '" + val
				    + "' is used for annotationtype '"
				    + toString( annotation_type() )
				    + "' with set='" + _set +"'"
				    + " but there is no corresponding <annotator>"
				    + " referring to it in the annotation"
				    + " declaration block." );
	  }
	}
      }
      _processor_id = val;
    }
  }

  void AbstractElement::annotator2processor( const string& annotator,
					     const string& an_type ){
    folia::processor *top = doc()->get_default_processor();
    AnnotatorType at = AnnotatorType::AUTO;
    if ( !an_type.empty() ){
      at = stringTo<AnnotatorType>( an_type );
    }
    auto procs = doc()->get_processors( annotation_type(), _set );
    const folia::processor *found=0;
    for ( const auto& p : procs ){
      if ( p->annotator() == annotator
	   && p->annotatortype() == at ){
	found = p;
	break;
      }
    }
    if ( found ) {
      set_processor_name( found->name() );
    }
    else {
      // no matching processor. create a simple one
      Provenance *prov = doc()->provenance();
      if ( !prov ){
	// no provenance at all,, so create first
	prov = new Provenance( doc() );
	//	DBG << "created new Provenance: " << prov << endl;
      }
      KWargs args;
      args.add("name",annotator);
      args.add("annotatortype",an_type);
      args.add("generate_id","auto()");
      const folia::processor *new_p = new folia::processor( prov, top, args );
      set_processor_name( new_p->name() );
      //      DBG << "created new processor: " << new_p << endl;
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
    bool att_dbg = false;
    DocDbg doc_dbg = DocDbg::NODEBUG;
    // we will switch it off temporarely. so remember
    if ( doc() ){
      doc_dbg = doc()->debug;
      att_dbg = ( doc_dbg == DocDbg::ATTRIBUTES );
    }
    if ( att_dbg ) {
      if ( isinstance<New>()
	   || isinstance<Original>() ) {
	if ( doc() ){
	  doc()->setdebug(DocDbg::NODEBUG);
	}
	DBG << "set attributes: '" << kwargs << "' on " << classname() << endl;
	// DBG << "required = " <<  toString(required_attributes()) << endl;
	// DBG << "optional = " <<  optional_attributes() << endl;
	// DBG << "supported = " << supported << endl;
	// DBG << "ID & supported = " << (ID & supported) << endl;
	// DBG << "ID & _required = " << (ID & required_attributes() ) << endl;
	// DBG << "_id=" << _id << endl;
	// DBG << "AUTH : " << _auth << endl;
      }
    }
    string val = kwargs.extract( "generate_id" );
    if ( !val.empty() ) {
      if ( !doc() ) {
	throw runtime_error( "can't generate an ID without a doc" );
      }
      if ( !( supported % Attrib::ID ) ) {
	throw ValueError( this,
			  "generate_id: xml:id is not supported for "
			  + classname() );
      }
      if ( val == "auto()" ){
	FoliaElement *par = parent();
	if ( par ) {
	  _id = par->generateId( xmltag() );
	}
	else {
	  throw ValueError( this,
			    "generate_id `auto()' not possible without parent" );
	}
      }
      else {
	FoliaElement *e = (*doc())[val];
	if ( e ) {
	  _id = e->generateId( xmltag() );
	}
	else {
	  throw ValueError( this,
			    "Unable to generate an id from ID= " + val );
	}
      }
    }
    else {
      val = kwargs.extract( "xml:id" );
      if ( val.empty() ) {
	val = kwargs.extract( "_id" ); // for backward compatibility
      }
      if ( !val.empty() ) {
	if ( ! (supported % Attrib::ID ) ) {
	  throw ValueError( this,
			    "xml:id is not supported for " + classname() );
	}
	else if ( val == "auto()" ){
	  FoliaElement *par = parent();
	  if ( par ) {
	    _id = par->generateId( xmltag() );
	  }
	  else {
	    throw ValueError( this,
			      "auto-generate of 'xml:id' not possible without parent" );
	  }
	}
	else if ( isNCName( val ) ){
	  _id = val;
	}
	else {
	  throw XmlError( this,
			  "'" + val + "' is not a valid NCName." );
	}
      }
    }

    _set.clear();
    val = kwargs.extract( "set" );
    if ( !val.empty() ) {
      if ( !doc() ) {
	throw ValueError( this,
			  "attribute set=" + val
			  + " is used on a node without a document." );
      }
      if ( !( (supported % Attrib::CLASS ) || setonly() ) ) {
	throw ValueError( this,
			  "attribute 'set' is not supported for "
			  + classname() );
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

    check_set_declaration();

    _class.clear();
    val = kwargs.extract( "class" );
    if ( !val.empty() ) {
      if ( !(supported % Attrib::CLASS ) ) {
	throw ValueError( this,
			  "Class is not supported for " + classname() );
      }
      if ( !isinstance<TextContent>()
	   && !isinstance<PhonContent>() ) {
	if ( !doc() ) {
	  throw ValueError( this,
			    "Class=" + val + " is used on a node without a document." );
	}
	if ( _set.empty() ){
	  if ( !doc()->declared( annotation_type(), "None" ) ) {
	    cerr << endl << doc()->annotationdefaults() << endl << endl;
	    throw DeclarationError( this,
				    xmltag() +": An empty set is used but that has no declaration "
				    "for " + toString( annotation_type() )
				    + "-annotation" );
	  }
	  _set = "None";
	}
	doc()->incrRef( annotation_type(), _set );
      }
      _class = val;
    }

    if ( !isinstance<TextContent>()
	 && !isinstance<PhonContent>() ) {
      if ( !_class.empty() && _set.empty() ) {
	throw ValueError( this,
			  "Set is required for <" + classname() +
			  " class=\"" + _class + "\"> assigned without set."  );
      }
    }

    val = kwargs.extract( "processor" );
    if ( !val.empty() ){
      if ( !( supported % Attrib::ANNOTATOR ) ){
	throw ValueError( this,
			  "attribute 'processor' is not supported for " + classname() );
      }
      set_processor_name( val );
    }
    else if ( (supported % Attrib::ANNOTATOR) && doc() ){
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
	  if ( doc()->debug % DocDbg::PROVENANCE ) {
	    DBG << "from original got default processor='" <<  def << "'" << endl;
	  }
	}
	else {
	  throw;
	}
      }
      _processor_id = def;
    }

    _annotator.clear();
    val = kwargs.extract( "annotator" );
    if ( !val.empty() ) {
      if ( !(supported % Attrib::ANNOTATOR) ) {
	throw ValueError( this,
			  "attribute 'annotator' is not supported for "
			  + classname() );
      }
      else {
	if ( !_processor_id.empty()
	     && val != doc()->get_processor(_processor_id)->name() ){
	  if ( doc() && doc()->autodeclare() ){
	    annotator2processor( val,
				 kwargs.extract( "annotatortype" ) );
	  }
	  else {
	    throw DeclarationError( this,
				    "Autodeclarations are disabled but an "
				    "annotator (" + val + ") was specified "
				    "that differs from the one in the declared "
				    "processor for this annotation type: "
				    + toString(annotation_type()) );
	  }
	}
	else {
	  _annotator = val;
	}
      }
    }
    else {
      string def;
      if ( doc() &&
	   (def = doc()->default_annotator( annotation_type(), _set )) != "" ) {
	_annotator = def;
      }
    }

    _annotator_type = AnnotatorType::UNDEFINED;
    val = kwargs.extract( "annotatortype" );
    if ( !val.empty() ) {
      if ( ! (supported % Attrib::ANNOTATOR ) ) {
	throw ValueError( this,
			  "Annotatortype is not supported for " + classname() );
      }
      else {
	_annotator_type = stringTo<AnnotatorType>( val );
	if ( _annotator_type == AnnotatorType::UNDEFINED ) {
	  throw ValueError( this,
			    "annotatortype must be 'auto' or 'manual', got '"
			    + val + "'" );
	}
      }
    }
    else {
      if ( doc() ){
	AnnotatorType def = doc()->default_annotatortype( annotation_type(), _set );
	if ( def != AnnotatorType::UNDEFINED ) {
	  _annotator_type = def;
	}
      }
    }

    _confidence = -1;
    val = kwargs.extract( "confidence" );
    if ( !val.empty() ) {
      if ( !( supported % Attrib::CONFIDENCE ) ) {
	throw ValueError( this,
			  "Confidence is not supported for " + classname() );
      }
      else {
	try {
	  _confidence = stringTo<double>(val);
	  if ( _confidence < 0 || _confidence > 1.0 ){
	    throw ValueError( this,
			      "Confidence must be a floating point number "
			      "between 0 and 1, got "
			      + TiCC::toString(_confidence) );
	  }
	}
	catch (...) {
	  throw ValueError( this,
			    "invalid Confidence value: " + val
			    + " (not a number?)");
	}
      }
    }

    _n = "";
    val = kwargs.extract( "n" );
    if ( !val.empty() ) {
      if ( !( supported % Attrib::N) ) {
	throw ValueError( this,
			  "N attribute is not supported for " + classname() );
      }
      else {
	_n = val;
      }
    }
    _datetime.clear();
    val = kwargs.extract( "datetime" );
    if ( !val.empty() ) {
      if ( !( supported % Attrib::DATETIME ) ) {
	throw ValueError( this,
			  "datetime attribute is not supported for "
			  + classname() );
      }
      else {
	string time = parseDate( val );
	if ( time.empty() ){
	  throw ValueError( this,
			    "invalid datetime, must be in YYYY-MM-DDThh:mm:ss"
			    "format: " + val );
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
      if ( !( supported % Attrib::BEGINTIME) ) {
	throw ValueError( this,
			  "begintime attribute is not supported for "
			  + classname() );
      }
      else {
	string time = parseTime( val );
	if ( time.empty() ) {
	  throw ValueError( this,
			    "invalid begintime, must be in HH:MM:SS.mmm "
			    "format: " + val );
	}
	_begintime = time;
      }
    }
    else {
      _begintime.clear();
    }
    val = kwargs.extract( "endtime" );
    if ( !val.empty() ) {
      if ( !( supported % Attrib::ENDTIME) ) {
	throw ValueError( this,
			  "endtime attribute is not supported for "
			  + classname() );
      }
      else {
	string time = parseTime( val );
	if ( time.empty() ) {
	  throw ValueError( this, "invalid endtime, must be in HH:MM:SS.mmm "
			    "format: " + val );
	}
	_endtime = time;
      }
    }
    else {
      _endtime.clear();
    }

    val = kwargs.extract( "src" );
    if ( !val.empty() ) {
      if ( !( supported % Attrib::SRC) ) {
	throw ValueError( this,
			  "src attribute is not supported for " + classname() );
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
      if ( !( supported % Attrib::TAG) ) {
	throw ValueError( this,
			  "tag attribute is not supported for " + classname() );
      }
      else {
	_tags = val;
      }
    }
    else {
      _tags.clear();
    }

    if ( supported % Attrib::SPACE  ){
      _space = true;
    }
    val = kwargs.extract( "space" );
    if ( !val.empty() ) {
      if ( !( supported % Attrib::SPACE ) ){
	throw ValueError( this,
			  "space attribute is not supported for "
			  + classname() );
      }
      else {
	if ( val == "no" ) {
	  _space = false;
	}
	else if ( val == "yes" ) {
	  _space = true;
	}
	else {
	  throw ValueError( this,
			    "invalid value for space attribute: '"
			    + val + "'" );
	}
      }
    }

    val = kwargs.extract( "metadata" );
    if ( !val.empty() ) {
      if ( !( supported % Attrib::METADATA) ) {
	throw ValueError( this,
			  "Metadata attribute is not supported for "
			  + classname() );
      }
      else {
	_metadata = val;
	if ( doc() && doc()->get_submetadata( _metadata ) == 0 ){
	  throw ValueError( this,
			    "No such metadata defined: " + _metadata );
	}
      }
    }
    else {
      _metadata.clear();
    }
    val = kwargs.extract( "speaker" );
    if ( !val.empty() ) {
      if ( !(supported % Attrib::SPEAKER) ) {
	throw ValueError( this,
			  "speaker attribute is not supported for "
			  + classname() );
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
      if ( !(supported % Attrib::TEXTCLASS) ) {
	throw ValueError( this,
			  "textclass attribute is not supported for "
			  + classname() );
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
	doc()->add_doc_index( this );
      }
      catch ( const DuplicateIDError& e ){
	if ( !isinstance<WordReference>() ){
	  throw;
	}
      }
      doc()->setdebug( doc_dbg );
    }
    kwargs.erase("typegroup"); //this is used in explicit form only, we can safely discard it
    addFeatureNodes( kwargs );
  }

  void AbstractElement::addFeatureNodes( const KWargs& kwargs ) {
    /// add children to the object, based on the set of Key-Value pairs.
    /*!
     * \param kwargs a KWargs set of Key-Value pairs
     * the given keys must be in the AttributeFeatures set.
     * the values are used as class attribute for the new children
     * will throw for unexpected attributes, except when in permisive mode
     */
    for ( const auto& [f_feat,val] : kwargs ) {
      string f_tag = f_feat;
      if ( f_tag == "head" ) {
	// "head" is special because the tag is "headfeature"
	// this to avoid conflicts with the "head" tag!
	f_tag = "headfeature";
      }
      if ( !isAttributeFeature(f_tag) ){
	string message = "unsupported attribute: " + f_tag + "='" + val
	  + "' for node with tag '" + classname() + "'";
	if ( f_tag == "id" ){
	  message += "\ndid you mean xml:id?";
	}
	if ( doc() && doc()->permissive() ) {
	  DBG << message << endl;
	}
	else {
	  throw XmlError( this, message );
	}
      }
      KWargs newa("class",val);
      FoliaElement *new_node;
      try {
	new_node = createElement( f_tag, doc() );
      }
      catch( const exception& e ){
	throw ValueError( this,
			  string("adding features failed (")
			  + e.what() + ")" );
      }
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
	  attribs.add("xlink:href",it->second);
	  attribs.add("xlink:type",type);
	}
	it = _xlink.find("role");
	if ( it != _xlink.end() ){
	  attribs.add("xlink:role",it->second);
	}
	it = _xlink.find("arcrole");
	if ( it != _xlink.end() ){
	  attribs.add("xlink:arcrole",it->second);
	}
	it = _xlink.find("show");
	if ( it != _xlink.end() ){
	  attribs.add("xlink:show",it->second);
	}
	it = _xlink.find("actuate");
	if ( it != _xlink.end() ){
	  attribs.add("xlink:actuate",it->second);
	}
	it = _xlink.find("title");
	if ( it != _xlink.end() ){
	  attribs.add("xlink:title",it->second);
	}
	it = _xlink.find("label");
	if ( it != _xlink.end() ){
	  attribs.add("xlink:label",it->second);
	}
      }
    }
    return attribs;
  }

  void AbstractElement::set_typegroup( KWargs& attribs ) const {
    if ( isSubClass<AbstractStructureElement>() ){
      attribs.add("typegroup","structure");
    }
    else if ( isSubClass<AbstractFeature>() ){
      attribs.add("typegroup","feature");
    }
    else if ( isSubClass<AbstractInlineAnnotation>() ){
      attribs.add("typegroup","inline");
    }
    else if ( isSubClass<AbstractHigherOrderAnnotation>() ){
      attribs.add("typegroup","higherorder");
    }
    else if ( isSubClass<AbstractSpanRole>() ){
      attribs.add("typegroup","spanrole");
    }
    else if ( isSubClass<AbstractSpanAnnotation>() ){
      attribs.add("typegroup","span");
    }
    else if ( isSubClass<AbstractTextMarkup>() ){
      attribs.add("typegroup","textmarkup");
    }
    else if ( isSubClass<AbstractContentAnnotation>() ){
      attribs.add("typegroup","content");
    }
    else if ( isSubClass<AbstractAnnotationLayer>() ){
      attribs.add("typegroup","layer");
    }
    else if ( isSubClass<AbstractSubtokenAnnotation>() ){
      attribs.add("typegroup","subtoken");
    }
    else if ( isSubClass<AbstractCorrectionChild>() ){
      attribs.add("typegroup","correctionchild");
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
    bool Explicit = false;
    Attrib supported = required_attributes() | optional_attributes();
    KWargs attribs;
    if ( doc() && doc()->has_explicit() ){
      Explicit = true;
      set_typegroup( attribs );
    }
    attribs.add("xml:id",_id);
    if ( _preserve_spaces == SPACE_FLAGS::PRESERVE ) {
      attribs.add("xml:space","preserve");
    }
    if ( doc() ){
      string default_set = doc()->default_set( annotation_type() );
      bool isDefaultSet = (_set == default_set);
      if ( Explicit && _set != "None" && !default_set.empty() ){
	if ( _set.empty() ){
	  attribs.add("set",default_set);
	}
	else {
	  attribs.add("set",_set);
	}
      }
      else if ( _set != "None"
		&& !_set.empty()
		&& !isDefaultSet ){
	string ali = doc()->alias( annotation_type(), _set );
	if ( ali.empty() ){
	  attribs.add("set",_set);
	}
	else {
	  attribs.add("set",ali);
	}
      }
      attribs.add("class",_class);
      if ( !_processor_id.empty() ){
	string tmp;
	try {
	  tmp = doc()->default_processor( annotation_type(), _set );
	  if ( Explicit ){
	    attribs.add("processor",tmp);
	  }
	}
	catch ( const NoDefaultError& ){
	}
	catch ( ... ){
	  throw;
	}
	if ( tmp != _processor_id ){
	  attribs.add("processor",_processor_id);
	}
      }
      else {
	bool isDefaultAnn = true;
	if ( !_annotator.empty() &&
	     _annotator != doc()->default_annotator( annotation_type(), _set ) ) {
	  isDefaultAnn = false;
	  attribs.add("annotator",_annotator);
	}
	if ( _annotator_type != AnnotatorType::UNDEFINED ){
	  AnnotatorType at = doc()->default_annotatortype( annotation_type(), _set );
	  if ( !isDefaultSet
	       || !isDefaultAnn
	       || _annotator_type != at){
	    if ( _annotator_type != at ) {
	      if ( _annotator_type == AnnotatorType::AUTO ){
		attribs.add("annotatortype","auto");
	      }
	      else if ( _annotator_type == AnnotatorType::MANUAL ) {
		attribs.add("annotatortype","manual");
	      }
	    }
	  }
	}
      }
    }
    if ( !_datetime.empty() &&
	 _datetime != doc()->default_datetime( annotation_type(), _set ) ) {
      attribs.add("datetime",_datetime);
    }
    attribs.add("begintime",_begintime);
    attribs.add("endtime",_endtime);
    attribs.add("src",_src);
    attribs.add("tag",_tags);
    attribs.add("metadata",_metadata);
    attribs.add("speaker",_speaker);
    if ( ( supported % Attrib::TEXTCLASS )
	 && ( !_textclass.empty() &&
	      ( _textclass != "current" || Explicit ) ) ){
      attribs.add("textclass",_textclass);
    }

    if ( _confidence >= 0 ) {
      attribs.add("confidence",toDoubleString(_confidence));
    }
    attribs.add("n",_n);
    if ( !_auth ) {
      attribs.add("auth","no");
    }
    if ( optional_attributes() % Attrib::SPACE ){
      if ( !_space ) {
	attribs.add("space","no");
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
      xmlSetNs( n, xmlNewNs( n, to_xmlChar(NSFOLIA), 0 ) );
    }
    xmlBuffer *buf = xmlBufferCreate();
    //    xmlKeepBlanksDefault(0);
    xmlNodeDump( buf, 0, n, indent, (format?1:0) );
    string result = to_string( xmlBufferContent( buf ) );
    xmlBufferFree( buf );
    xmlFreeNode( n );
    return result;
  }

  const string FoliaElement::str( const string& cls ) const {
    /// return the UTF8 value of this element
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
    return TiCC::UnicodeToUTF8( unicode( cls ) );
  }

  const string FoliaElement::str( const TextPolicy& tp ) const {
    /// return the UTF8 value of this element
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
    return TiCC::UnicodeToUTF8( unicode( tp ) );
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

  const UnicodeString FoliaElement::stricttext( const string& cls,
						bool debug ) const {
    /// get the UnicodeString value of TextContent children only
    /*!
     * \param cls the textclass
     * \param debug (default false)
     * \return The Unicode Text found.
     * Will throw on error.
     */
    TextPolicy tp( cls, TEXT_FLAGS::STRICT );
    tp.set_debug( debug );
    return this->text( tp );
  }

  const UnicodeString FoliaElement::toktext( const string& cls ) const {
    /// get the UnicodeString value of TextContent children only, retaining
    /// tokenization
    /*!
     * \param cls the textclass
     * \return The Unicode Text found.
     * Will throw on error.
     */
    TextPolicy tp( cls, TEXT_FLAGS::RETAIN );
    return this->text( tp );
  }

    void FoliaElement::clear_textcontent( const string& textclass ){
    for ( size_t i=0; i < size(); ++i ){
      FoliaElement *p = index(i);
      if ( p->isinstance<TextContent>() ) {
	if ( p->cls() == textclass ){
	  p->destroy();
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
	 && !isSubClass<Morpheme>() && !isSubClass<Phoneme>() ){
      UnicodeString deeper_u;
      try {
	deeper_u = text( cls );
	// get deep original text: no retain tokenization, no strict
      }
      catch (...){
      }
      deeper_u = normalize_spaces( deeper_u );
      UnicodeString txt_check_u = normalize_spaces( txt_u );
      if ( !deeper_u.isEmpty()
	   && txt_check_u != deeper_u ){
	throw InconsistentText( this,
				xmltag() + "::settext(cls=" + cls
				+ "): deeper text differs from attempted\n"
				+ "   deeper='"
				+ TiCC::UnicodeToUTF8(deeper_u)
				+ "'\nattempted='"
				+ TiCC::UnicodeToUTF8(txt_u) + "'" );
      }
    }
    string my_set;
    if ( doc() ){
      my_set = doc()->default_set( AnnotationType::TEXT );
    }
    KWargs args;
    args.add("value",TiCC::UnicodeToUTF8(txt_u));
    args.add("class",cls);
    args.add("set",my_set);
    if ( offset >= 0 ){
      args.add("offset",TiCC::toString(offset));
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
    vector<Description*> v = select<Description>( SELECT_FLAGS::LOCAL );
    if ( v.size() == 0 ) {
      return "";
    }
    return v[0]->description();
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
    if ( c->isSubClass<AbstractFeature>() ) {
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

  void CheckText( const FoliaElement *parent,
		  const FoliaElement *child,
		  const string& cls,
		  bool txt_dbg ){
    if ( parent
	 && !parent->isinstance<Correction>()
	 && parent->hastext( cls ) ){
      // check text consistency for parents with text
      // but SKIP Corrections
      UnicodeString s1 = parent->stricttext( cls );
      UnicodeString s2 = child->stricttext( cls );
      if ( txt_dbg ){
	DBG << "check_text parent: " << s1 << endl;
	DBG << "check_text child: " << s2 << endl;
      }
      // no retain tokenization, strict for both
      s1 = normalize_spaces( s1 );
      s2 = normalize_spaces( s2 );
      if ( !s1.isEmpty() && !s2.isEmpty() ){
	bool test_fail;
	if ( child->isSubClass<TextContent>()
	     || child->isSubClass<AbstractTextMarkup>()
	     || child->isSubClass<String>()
	     || child->isSubClass<Word>() ){
	  // Words and Strings are 'per definition' PART of their parents
	  test_fail = ( s1.indexOf( s2 ) < 0 ); // aren't they?
	}
	else {
	  // otherwise an exacte match is needed
	  test_fail = ( s1 != s2 );
	}
	if ( test_fail ){
	  throw InconsistentText( child,
				  "adding text (class="
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

  void  CheckText2( const FoliaElement *parent,
		    const FoliaElement *child,
		    const string& cls,
		    bool trim_spaces,
		    bool txt_dbg ){
    if ( parent
	 && parent->hastext( cls ) ){
      // check text consistency for parents with text
      // but SKIP Corrections
      // no retain tokenization, strict for parent, deeper for child
      TextPolicy tp( cls );
      tp.set( TEXT_FLAGS::STRICT );
      if ( !trim_spaces ) {
	tp.set( TEXT_FLAGS::NO_TRIM_SPACES );
      }
      if ( txt_dbg ){
	tp.set_debug();
      }
      UnicodeString s1 = parent->text( tp );
      tp.clear( TEXT_FLAGS::STRICT );
      UnicodeString s2 = child->text( tp );
      s1 = normalize_spaces( s1 );
      s2 = normalize_spaces( s2 );
      bool test_fail;
      if ( child->isSubClass<Word>()
	   || child->isSubClass<String>()
	   || child->isSubClass<AbstractTextMarkup>() ) {
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
	    child->check_text_consistency(false);
	    warn_only = true;
	  } catch ( const InconsistentText& ) {
	    //ignore, we raise the newer error
	  }
        }
	string msg = "conflicting text (class="
	  + cls + ") from node: " + child->xmltag()
	  + "(" + child->id() + ")"
	  + " with value\n'" + TiCC::UnicodeToUTF8(s2)
	  + "'\n with parent: " + parent->xmltag() +
	  + "(" + parent->id() + ") which already has "
	  + "text in that class and value: \n'"
	  + TiCC::UnicodeToUTF8(s1) + "'\n";
        if ( warn_only ) {
	  msg += "However, according to the older rules (<v2.4.1) the text is consistent. So we are treating this as a warning rather than an error. We do recommend fixing this if this is a document you intend to publish.\n";
	  cerr << "WARNING: inconsistent text: " << msg << endl;
	  parent->doc()->increment_warn_count();
        }
	else {
	  throw InconsistentText( child, msg);
        }
      }
    }
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
    bool txt_cst_dbg = false;
    if ( doc() ){
      txt_cst_dbg = ( doc()->debug % DocDbg::TEXT_CONSISTENCY );
    }
    if ( txt_cst_dbg ){
      DBG << "BEFORE checkappend I am=" << this << endl;
      DBG << "      checkappend child=" << child << endl;
    }
    if ( !doc() || !doc()->checktext() || doc()->fixtext() ){
      if ( txt_cst_dbg ){
	DBG << "quick return" << endl;
      }
      return;
    }
    string c_cls = child->cls();
    if ( txt_cst_dbg ){
      DBG << "class=" << c_cls << endl;
    }
    if ( child->size() == 0
	 || ( child->is_textcontainer()
	      && !child->hastext( c_cls ) ) ){
      // no use to proceed. not adding real text
      if ( txt_cst_dbg ){
	DBG << "nothing wrong here , exit()" << endl;
      }
      return;
    }
    const FoliaElement *par = 0;
    if ( child->is_textcontainer() ){
      par = this->parent();
    }
    else {
      par = this;
      c_cls = child->index(0)->cls();
    }
    if ( txt_cst_dbg ){
      DBG << "PARENT = " << par << endl;
    }
    CheckText( par, child, c_cls, txt_cst_dbg );
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
    bool txt_cst_dbg = false;
    if ( doc() ){
      txt_cst_dbg = ( doc()->debug % DocDbg::TEXT_CONSISTENCY );
    }
    string my_cls = cls();
    const FoliaElement *par = parent();
    CheckText2( par, this, my_cls, trim_spaces, txt_cst_dbg );
  }

  void AbstractElement::check_text_consistency_while_parsing( bool trim_spaces,
							      bool txt_dbg ) {
      // this block was moved from parseXml into a separate function
      // it remains to be seen how much overlaps with check_text_consistency()
      // and whether we can't make do with one function
      //
      // unlike the other function, this does do some fixing when requested
      //
    if ( txt_dbg ){
      DBG << "DEBUG: BEGIN check_text_consistency_while_parsing("
	   << trim_spaces << ")" << endl;
    }
    // check the text for every possible text class
    for ( const auto& st : doc()->textclasses() ){
      UnicodeString s1, s2;
      TextPolicy tp( st );
      tp.set_correction_handling(CORRECTION_HANDLING::EITHER);
      tp.set( TEXT_FLAGS::STRICT );
      tp.set_debug( txt_dbg );
      if ( !trim_spaces ) {
	tp.set( TEXT_FLAGS::NO_TRIM_SPACES );
      }
      try {
	s1 = text( tp );  // no retain tokenization, strict
      }
      catch (...){
      }
      if ( !s1.isEmpty() ){
	if ( txt_dbg  ){
	  DBG << "S1: " << s1 << endl;
	}
	tp.clear( TEXT_FLAGS::STRICT );
	try {
	  s2 = text( tp ); // no retain tokenization, no strict
	}
	catch (...){
	}
	if ( txt_dbg ){
	  DBG << "S2: " << s2 << endl;
	}
	s1 = normalize_spaces( s1 );
	s2 = normalize_spaces( s2 );
	if ( !s2.isEmpty() && s1 != s2 ){
	  if ( doc()->fixtext() ){
	    if ( txt_dbg  ){
	      DBG << "FIX: " << s1 << "==>" << s2 << endl;
	    }
	    KWargs args;
	    args.add("value", TiCC::UnicodeToUTF8(s2));
	    args.add("class",st);
	    TextContent *node = new TextContent( args, doc() );
	    this->replace( node );
	  }
	  else {
	    bool warn_only = false;
	    if ( trim_spaces ) {
	      //ok, we failed according to the >v2.4.1 rules
	      //but do we also fail under the old rules?
	      try {
		if ( txt_dbg ){
		  DBG << "DEBUG: (testing according to older rules now)" << endl;
		}
		this->check_text_consistency_while_parsing( false, txt_dbg );
		warn_only = true;
	      }
	      catch ( const InconsistentText& e ) {
		if ( txt_dbg ){
		  DBG << "(tested according to older rules (<v2.4.1) as well, but this failed too)" << endl;
		}
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
	      doc()->increment_warn_count();
	    }
	    else {
	      if ( txt_dbg ){
		DBG << "DEBUG: CONSISTENCYERROR check_text_consistency_while_parsing(" << trim_spaces << ")" << endl;
	      }
	      throw InconsistentText( this, msg);
	    }
	  }
	}
      }
    }
    if ( isinstance<TextContent>() ){
      // we have to check for at least one XmlText or TEXTCONTAINER child
      auto valid_text = []( auto elt ){
	return ( elt->element_id() == ElementType::XmlText_t
		 || elt->is_textcontainer() ); };
      bool found = std::any_of( _data.begin(), _data.end(),
				valid_text );
      if ( !found ){
	string msg = "Empty text content elements are not allowed. Parsing <"
	  + xmltag() + ">, id=" + id();
	throw XmlError( this, msg );
      }
    }
    if ( txt_dbg ){
      DBG << "DEBUG: END-OK check_text_consistency_while_parsing("
	   << trim_spaces << ")" << endl;
    }
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
      attribs.add("xml:space","default");
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
      for ( const auto* el : _data ) {
	string at = tagToAtt( el );
	if ( !at.empty() ) {
	  ++af_map[at];
	}
      }
      // ok, now we create attributes for those that only occur once
      for ( const auto& el : _data ) {
	string at = tagToAtt( el );
	if ( !at.empty() && af_map[at] == 1 ) {
	  attribs.add(at,el->cls());
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
      list<FoliaElement *> PIelements;
      multimap<ElementType, FoliaElement *, std::greater<ElementType>> otherelementsMap;
      for ( const auto& el : _data ) {
	if ( attribute_elements.find(el) == attribute_elements.end() ) {
	  if ( el->isinstance<TextContent>() ) {
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
	      if ( el->isinstance<XmlComment>()
		   && currenttextelements.empty()
		   && textelements.empty() ) {
		commentelements.push_back( el );
	      }
	      else if ( el->isinstance<ProcessingInstruction>()
		   && currenttextelements.empty()
		   && textelements.empty() ) {
		PIelements.push_back( el );
	      }
	      else {
		otherelements.push_back( el );
	      }
	    }
	  }
	}
      }
      for ( const auto* cel : commentelements ) {
	xmlAddChild( e, cel->xml( recursive, kanon ) );
      }
      for ( const auto* pel : PIelements ) {
	xmlAddChild( e, pel->xml( recursive, kanon ) );
      }
      for ( const auto* tel : currenttextelements ) {
	xmlAddChild( e, tel->xml( recursive, false ) );
	// don't change the internal sequences of TextContent elements
      }
      for ( const auto* tel : textelements ) {
	xmlAddChild( e, tel->xml( recursive, false ) );
	// don't change the internal sequences of TextContent elements
      }
      if ( !kanon ) {
	for ( const auto* oem : otherelements ) {
	  xmlAddChild( e, oem->xml( recursive, kanon ) );
	}
      }
      else {
	for ( const auto& [dummy,elt] : otherelementsMap ) {
	  xmlAddChild( e, elt->xml( recursive, kanon ) );
	}
      }
      check_text_consistency();
    }
    return e;
  }

  const UnicodeString AbstractElement::unicode( const string& cls ) const {
    /// return the Unicode text value of this element
    /*!
     * \param cls The desired textclass
     * \return the Unicode value
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
    return us;
  }

  const UnicodeString AbstractElement::unicode( const TextPolicy& tp ) const {
    /// return the Unicode text value of this element
    /*!
     * \param tp the TextPolicy to use
     * \return the UnicodeString
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
	us = phon( tp );
      }
      catch( const NoSuchPhon&){
	// No TextContent or Phone is allowed
      }
    }
    return us;
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
    set<ElementType> exclude;
    vector<LangAnnotation*> v = select<LangAnnotation>( st,
							exclude,
							SELECT_FLAGS::LOCAL );
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

  const string& AbstractElement::get_delimiter( const TextPolicy& tp ) const {
    /// get the default delimiter of this object.
    /*!
     * \param tp the TextPolicy to use
     * \return a string representing the delimiter
     *
     * If the object has a TEXTDELIMITER property thats is returned
     * Otherwise, the last child is taken and its delimiter is returned IF
     * it is a Structure Element.
     * When this test fails, an empty string is returned, UNLESS the element has
     * the SPACE attribute AND retaintok is specified
     */
    bool retaintok  = tp.is_set( TEXT_FLAGS::RETAIN );
    if ( tp.debug() ){
      DBG << "IN <" << xmltag() << ">:get_delimiter (" << retaintok << ")"
	   << endl;
    }
    if ( (optional_attributes() % Attrib::SPACE ) ){
      if ( ! ( _space || retaintok ) ){
	if ( tp.debug() ){
	  DBG << " space = NO, return: '" << EMPTY_STRING << "'" << endl;
	}
	return EMPTY_STRING;
      }
      if ( tp.debug() ){
	DBG << " space = YES, Go on" << endl;
      }
    }

    if ( !_data.empty() ){
      const FoliaElement *last = _data.back();
      if ( last && tp.debug() ){
	DBG << "last is " << last << endl;
	DBG << "isSubClass<AbstractWord>() == " << last->isSubClass<AbstractWord>() << endl;
	DBG << "last->space() == " << last->space() << endl;
      }
      if ( last
	   && last->isSubClass<AbstractWord>()
	   && !last->space() ){
	return EMPTY_STRING;
      }
    }
    if ( text_delimiter() != "NONE" ) {
      if ( tp.debug() ){
	DBG << "text_delimiter() == '" << text_delimiter() << "'" << endl;
      }
      return text_delimiter();
    }
    else if ( _data.size() > 0 ) {
      // attempt to get a delimiter from the last child
      const FoliaElement *last = _data.back();
      if ( last->isSubClass<AbstractWord>() ){
	const string& det = last->get_delimiter( tp );
	if ( tp.debug() ){
	  DBG << "out <" << xmltag() << ">:get_delimiter ==> '" << det << "'"
	       << endl;
	}
	return det;
      }
    }
    if ( tp.debug() ){
      DBG << "out <" << xmltag() << ">:get_delimiter ==> ''" << endl;
    }
    return EMPTY_STRING;
  }

  bool is_space( const UChar32 kar ){
    return ( kar == 0x0020       // space
	     || kar == 0x0009    // tab
	     || kar == 0x000a    // newline
	     || kar == 0x000d ); // carriage return
  }

  UnicodeString AbstractElement::text_container_text( const TextPolicy& tp ) const {
    string desired_class = tp.get_class();
    if ( isinstance<TextContent>()
	 && cls() != desired_class ) {
      // take a shortcut for TextContent in wrong class
      if ( tp.debug() ){
	DBG << "TextContent shortcut, class=" << cls()
	     << " but looking for: " << desired_class << endl;
      }
      return "";
    }
    UnicodeString result;
    bool pendingspace = false;
    bool trim_spaces = !tp.is_set( TEXT_FLAGS::NO_TRIM_SPACES);
    if ( tp.debug() ){
      DBG << "TextContainer.text() " << xmltag() << "[";
      for ( const auto* d : _data ){
	DBG << d->xmltag() << ",";
      }
      DBG << "]" << endl;
    }
    for ( const auto& d : _data ){
      if ( d->isinstance<XmlText>() ) {
	// 'true' text child
	if ( pendingspace ) {
	  result += " ";
	  pendingspace = false;
	}
	if ( trim_spaces) {
	  //This implements https://github.com/proycon/folia/issues/88
	  //FoLiA >= v2.5 behaviour (introduced earlier in v2.4.1 but modified thereafter)
	  const int l = result.length();
	  UnicodeString txt = d->text( tp );
	  int begin = 0;
	  int linenr = 0;
	  for ( int i = 0; i < txt.length(); ++i ) {
	    if ( txt[i] == 0x000a
		 || (i == txt.length() - 1) ) {
	      //newline or end
	      UnicodeString line;
	      if ( txt[i] == 0x000a ) { //newline
		line = UnicodeString(txt, begin, i - begin);
	      }
	      else {
		line = UnicodeString(txt, begin, txt.length() - begin);
	      }
	      begin = i+1;

	      UnicodeString subresult;
	      if ( _preserve_spaces == SPACE_FLAGS::PRESERVE) {
		if ( line.length() > 0
		     && line[line.length() - 1] == 0x000d) {
		  // carriage return
		  // remove artefacts of any DOS-style line endings
		  // (not sure if still needed here but better safe than sorry)
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
	       && txt.length() > 0
	       && result.length() > 0
	       && is_space(txt[txt.length() - 1])
	       && !is_space(result[result.length() - 1]) ){
	    // this item has trailing spaces but we stripped them
	    // this may be premature so
	    // we reserve to output them later in case there is a next item
	    pendingspace = true;
	  }
	}
	else {
	  // old FoLiA <= v2.4.1 behaviour, we don't trim anything
	  result += d->text( tp );
	}
      }
      else if ( d->printable() ){
	// this is some TextMarkup I hope
	if (pendingspace) {
	  if (!d->implicitspace()) result += " ";
	  pendingspace = false;
	}
	string tv = d->tag();
	if ( !tv.empty() ){
	  vector<string> tvv = TiCC::split(tv);
	  bool no_match = true;
	  for ( const auto& v : tvv ){
	    TextPolicy::tag_handler match = tp.get_handler( v );
	    if ( match ){
	      no_match = false;
	      UnicodeString tmp_result = match( d, tp );
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
	  const string& delim = d->get_delimiter( tp );
	  if ( tp.debug() ){
	    DBG << "append delimiter: '" << delim << "'" << endl;
	  }
	  result += TiCC::UnicodeFromUTF8(delim);
	}
      }
      else {
	// non interesting stuff like <feature>, <comment> etc.
      }
    }
    if (trim_spaces && this->spaces_flag() != SPACE_FLAGS::PRESERVE) {
      result = postprocess_spaces(result);
    }
    if ( tp.debug() ){
      DBG << "TEXT(" << tp.get_class() << ") on a textcontainer :" << xmltag()
	   << " returned '" << result << "'" << endl;
    }
    return result;
  }

  const UnicodeString AbstractElement::private_text( const TextPolicy& tp ) const {
    /// get the UnicodeString value of an element
    /*!
     * \param tp The TextPolicy to use
     * \return the Unicode String representation found. Throws when
     * no text can be found
     */
    bool strict = tp.is_set( TEXT_FLAGS::STRICT );
    bool show_hidden = tp.is_set( TEXT_FLAGS::HIDDEN );
    bool trim = !tp.is_set( TEXT_FLAGS::NO_TRIM_SPACES );
    if ( tp.debug() ){
      DBG << "PRIVATE_TEXT(" << tp.get_class() << ") on node : " << xmltag() << " id="
	   << id() << endl;
      DBG << "TextPolicy: " << tp << endl;
    }
    UnicodeString result;
    if ( strict ) {
      /// WARNING. Don't call text(tp) here. We will get into an infinite
      /// recursion. Can't we do better then calling ourself again, sort of?
      TextPolicy tmp = tp;
      tmp.clear( TEXT_FLAGS::STRICT );
      result = text_content(tmp)->text( tmp );
    }
    else if ( !printable() || ( hidden() && !show_hidden ) ){
      throw NoSuchText( this, "NON printable element: " + xmltag() );
    }
    else if ( is_textcontainer() ){
      result = text_container_text( tp );
    }
    else {
      //
      result = deeptext( tp );
      if ( result.isEmpty() ) {
	TextPolicy tmp = tp;
	tmp.set( TEXT_FLAGS::STRICT );
	if ( !trim ) {
	  tmp.set( TEXT_FLAGS::NO_TRIM_SPACES );
	}
	result = text( tmp );
      }
      if ( result.isEmpty() ) {
	throw NoSuchText( this,
			  "on tag " + xmltag() + " nor it's children" );
      }
    }
    if ( tp.debug() ){
      DBG << "PRIVATE_TEXT on node : " << xmltag() << " returns: '" << result
	   << "'" << endl;
    }
    return result;
  }

  const UnicodeString AbstractElement::text( const TextPolicy& tp ) const {
    /// get the UnicodeString text value of an element
    /*!
     * \param tp a TextPolicy
     */
    if ( tp.debug() ){
      DBG << "DEBUG <" << xmltag() << ">.text() Policy=" << tp << endl;
    }
    return private_text( tp );
  }

  const UnicodeString AbstractElement::text( const string& cls,
					     TEXT_FLAGS flags,
					     bool txt_dbg ) const {
    /// get the UnicodeString text value of an element
    /*!
     * \param cls the textclass the text should be in
     * \param flags the search parameters to use. See TEXT_FLAGS.
     * \param txt_dbg enables debugging when true
     */
    TextPolicy tp( cls, flags );
    tp.set_debug( txt_dbg );
    if ( txt_dbg ){
      DBG << "DEBUG <" << xmltag() << ">.text() Policy=" << tp << endl;
    }
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
      throw XmlError( this, "FoLiA root without Document" );
    }
    setAttributes( atts );
    set_line_number( xmlGetLineNo(node) );
    bool meta_found = false;
    const xmlNode *p = node->children;
    while ( p ){
      set_line_number( xmlGetLineNo(p) );
      if ( p->type == XML_ELEMENT_NODE ){
	if ( TiCC::Name(p) == "metadata" &&
	     checkNS( p, NSFOLIA ) ){
	  if ( doc()->debug % DocDbg::PARSING ){
	    DBG << "Found metadata" << endl;
	  }
	  doc()->parse_metadata( p );
	  meta_found = true;
	}
	else if ( TiCC::getNS(p) == NSFOLIA ){
	  string tag = TiCC::Name( p );
	  if ( !meta_found  && !doc()->version_below(1,6) ){
	    if ( doc()->autodeclare() ){
	      doc()->fixup_metadata();
	      meta_found = true;
	      // and jus go on. assuming <text> to come
	    }
	    else {
	      throw XmlError( this,
			      "Expecting element metadata, got '" + tag + "'" );
	    }
	  }
	  FoliaElement *t=0;
	  try {
	    t = AbstractElement::createElement( tag, doc() );
	  }
	  catch ( const exception& e ){
	    throw XmlError( this,
			    string( "parsing <" ) + tag + "> failed:\n\t"
			    + e.what() );
	  }
	  if ( doc()->debug % DocDbg::PARSING ){
	    DBG << "created " << t << endl;
	  }
	  t = t->parseXml( p );
	  if ( t ){
	    if ( doc()->debug % DocDbg::PARSING ){
	      DBG << "extend " << this << " met " << tag << endl;
	    }
	    this->append( t );
	  }
	}
      }
      else if ( p->type == XML_COMMENT_NODE ) {
	string xml_tag = "_XmlComment";
	FoliaElement *t;
	try {
	  t = createElement( xml_tag, doc() );
	}
	catch ( const exception& e ){
	  throw XmlError( this,
			  string( "parsing <" ) + xml_tag + "> failed:\n\t"
			  + e.what() );
	}
	if ( doc() && doc()->debug % DocDbg::PARSING ) {
	  DBG << "created " << t << endl;
	}
	t = t->parseXml( p );
	if ( t ) {
	  if ( doc() && doc()->debug % DocDbg::PARSING ) {
	    DBG << "extend " << this << " met " << t << endl;
	  }
	  append( t );
	}
      }
      else if ( p->type == XML_PI_NODE ){
	// found a processing instruction on the top level
	// When this is a style-sheet, it is already handled
	// otherwise just skip
      }
      else if ( p->type == XML_TEXT_NODE ){
	// This MUST be 'empty space', so only spaces and tabs formatting
	string txt = TextValue(p);
	txt = TiCC::trim(txt);
	if ( !txt.empty() ){
	  if ( p->prev ){
	    string tg = "<" + Name(p->prev) + ">";
	    throw XmlError( this,
			    "found extra text '" + txt + "' after element "
			    + tg + ", NOT allowed there." );
	  }
	  else {
	    string tg = "<" + Name(p->parent) + ">";
	    throw XmlError( this,
			    "found extra text '" + txt + "' inside element "
			    + tg + ", NOT allowed there." );
	  }
	}
      }
      p = p->next;
    }
    return this;
  }

  UnicodeString trim_space( const UnicodeString& in ){
    /// remove leading and traling spaces. KEEP newlines etc.
    /*!
     * \param in an untrimmed UnicodeString
     * \return an UnicodeString with all leading and trailing spaces removed.
     * Other 'whitespace' characters like newline and tab are retained!
     */
    const char16_t space = 0x0020;
    //    DBG << "in = '" << in << "'" << endl;
    UnicodeString out;
    int i = 0;
    for( ; i < in.length(); ++i ){
      //      DBG << "start: bekijk:" << UnicodeString(in[i]) << endl;
      if ( in[i] != space ){
	break;
      }
    }
    int j = in.length()-1;
    for( ; j >= 0; --j ){
      //      DBG << "end: bekijk:" << UnicodeString(in[j]) << endl;
      if ( in[j] != space ){
	break;
      }
    }
    // DBG << "I=" << i << endl;
    // DBG << "J=" << j << endl;
    if ( j < i ){
      //      DBG << "out = LEEG" << endl;
      return out;
    }
    out = UnicodeString( in, i, j-i+1 );
    //    DBG << "out = '" << out << "'" << endl;
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

  bool no_space_at_end( const FoliaElement *s, bool txt_dbg ){
    /// given a FoliaElement check if the last Word in it has space()
    /*!
     * \param s a FoliaElement
     * \param txt_dbg set to true for more info
     * \return true if the element contains Word children and the last
     * one has space()
     */
    bool result = false;
    if ( txt_dbg ){
      DBG << "no space? s: " << s << endl;
    }
    if ( s ){
      vector<FoliaElement*> elts = s->select_set( {ElementType::Word_t,
	  ElementType::Correction_t},
	"",
	{},
	SELECT_FLAGS::LOCAL );
      if ( !elts.empty() ){
	if ( txt_dbg ){
	  DBG << "found some mixed stuff: " << elts << endl;
	}
	const FoliaElement *last = elts.back();
	result = !last->space();
	if ( txt_dbg ){
	  DBG << "no space? last: " << last
	       << (result?" WEL":" NIET") << endl;
	}
	return result;
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
    if ( tp.debug() ){
      DBG << "deeptext, policy: " << tp << ", on node : <" << xmltag()
	   << " id=" << id() << ", cls=" << this->cls() << ">" << endl;
      DBG << "deeptext: node has " << _data.size() << " children." << endl;
    }
    vector<UnicodeString> parts;
    vector<UnicodeString> seps;
    for ( const auto* child : _data ) {
      // try to get text dynamically from printable children
      // skipping the TextContent elements
      if ( tp.debug() ){
	if ( !child->printable() ) {
	  DBG << "deeptext: node[" << child->xmltag() << "] NOT PRINTABLE! "
	       << endl;
	}
      }
      if ( child->printable()
	   && ( is_structure( child )
		|| child->isSubClass<AbstractSpanAnnotation>()
		|| child->isinstance<Correction>() )
	   && !child->isinstance<TextContent>() ) {
	if ( tp.debug() ){
	  DBG << "deeptext:bekijk node[" << child->xmltag() << "]"<< endl;
	}
	try {
	  UnicodeString tmp = child->text( tp );
	  if ( tp.debug() ){
	    DBG << "deeptext found '" << tmp << "'" << endl;
	  }
	  parts.push_back(tmp);
	  if ( child->isinstance<Sentence>()
	       && no_space_at_end(child,tp.debug()) ){
	    const string& delim = "";
	    if ( tp.debug() ){
	      DBG << "deeptext: no delimiter van "<< child->xmltag() << " on"
		   << " last w of s" << endl;
	    }
	    seps.push_back(TiCC::UnicodeFromUTF8(delim));
	  }
	  else {
	    // get the delimiter
	    const string& delim = child->get_delimiter( tp );
	    if ( tp.debug() ){
	      DBG << "deeptext:delimiter van "<< child->xmltag() << " ='"
		   << delim << "'" << endl;
	    }
	    seps.push_back(TiCC::UnicodeFromUTF8(delim));
	  }
	}
	catch ( const NoSuchText& e ) {
	  if ( tp.debug() ){
	    DBG << "HELAAS" << endl;
	  }
	}
      }
    }

    // now construct the result;
    UnicodeString result;
    for ( size_t i=0; i < parts.size(); ++i ) {
      if ( tp.debug() ){
	DBG << "part[" << i << "]='" << parts[i] << "'" << endl;
	DBG << "sep[" << i << "]='" << seps[i] << "'" << endl;
      }
      bool only_nl = false;
      bool end_is_nl = check_end( parts[i], only_nl );
      if ( end_is_nl ){
	if ( tp.debug() ){
	  DBG << "a newline after: '" << parts[i] << "'" << endl;
	  if ( i < parts.size()-1 ){
	    DBG << "next sep='" << seps[i+1] << "'" << endl;
	  }
	}

	if ( only_nl ){
	  // only a newline
	  result = trim_space( result );
	  if ( tp.debug() ){
	    DBG << "OK it is only newline(s)" << endl;
	    DBG << "TRIMMED? '" << result << "'" << endl;
	  }
	}
      }
      result += parts[i];
      if ( !end_is_nl && i < parts.size()-1 ){
	result += seps[i];
      }
      if ( tp.debug() ){
	DBG << "result='" << result << "'" << endl;
      }
    }
    if ( tp.debug() ){
      DBG << "deeptext() for " << xmltag() << " step 3 " << endl;
    }
    if ( result.isEmpty() ) {
      // so no deeper text is found. Well, lets look here then
      result = text_content(tp)->text( tp );
    }
    if ( tp.debug() ){
      DBG << "deeptext() for " << xmltag() << " result= '" << result << "'"
	   << endl;
    }
    if ( result.isEmpty() ) {
      throw NoSuchText( this,
			xmltag() + ":(class=" + tp.get_class() +"): empty!" );
    }
    return result;
  }

  const TextContent *AbstractElement::text_content( const TextPolicy& tp ) const {
    /// Get the TextContent explicitly associated with this element.
    /*!
     * \param tp the TextPolicy to use
     *
     * Returns the TextContent instance rather than the actual text.
     * (so it might return itself.. ;)
     * Does not recurse into children with the sole exception of Correction
     * might throw NoSuchText exception if not found.
     */

    if ( tp.debug() ){
      DBG << "text_content, policy= " << tp << endl;
    }
    string desired_class = tp.get_class();
    if ( isinstance<TextContent>() ){
      if ( tp.debug() ){
	DBG << "A textcontent!!" << endl;
      }
      if  ( this->cls() == desired_class ) {
	if ( tp.debug() ){
	  DBG << "return myself..." << endl;
	}
	return dynamic_cast<const TextContent*>(this);
      }
      else {
	throw NoSuchText( this,
			  "TextContent::text_content(" + desired_class + ")" );
      }
    }
    bool show_hidden = tp.is_set( TEXT_FLAGS::HIDDEN );
    if ( tp.debug() ){
      DBG << (!printable()?"NOT":"") << " printable: " << xmltag() << endl;
      DBG << (!hidden()?"NOT":"") << " hidden: " << xmltag() << endl;
    }
    if ( !printable() || ( hidden() && !show_hidden ) ) {
      throw NoSuchText( this,
			"non-printable element: " +  xmltag() );
    }
    if ( tp.debug() ){
      DBG << "recurse into children...." << endl;
    }
    for ( const auto& el : data() ) {
      if ( el->isinstance<TextContent>()
	   && (el->cls() == desired_class ) ) {
	return dynamic_cast<TextContent*>(el);
      }
      else if ( el->isinstance<Correction>() ){
	if ( tp.debug() ){
	  DBG << "look into correction...." << endl;
	}
	try {
	  return el->text_content( tp );
	}
	catch ( const NoSuchText& e ) {
	  // continue search for other Corrections or a TextContent
	}
      }
    }
    throw NoSuchText( this,
		      xmltag() + "::text_content(" + desired_class + ")" );
  }

  TextContent *AbstractElement::text_content( const TextPolicy& tp ) {
    return const_cast<TextContent*>
      ( static_cast<const AbstractElement &>(*this).text_content( tp ) );
  }

  const TextContent *AbstractElement::text_content( const string& cls,
						    bool txt_dbg ) const {
    /// Get the TextContent explicitly associated with this element.
    /*!
     * \param cls the textclass to search for
     * \param txt_dbg enables debugging when true
     *
     * Returns the TextContent instance rather than the actual text.
     * (so it might return itself.. ;)
     * Does not recurse into children with the sole exception of Correction
     * might throw NoSuchText exception if not found.
     */
    TextPolicy tp( cls );
    tp.set_debug( txt_dbg );
    return text_content( tp );
  }

  TextContent *AbstractElement::text_content( const string& cls,
					      bool txt_dbg ) {
    return const_cast<TextContent*>
      ( static_cast<const AbstractElement &>(*this).text_content( cls, txt_dbg ) );
  }

  const PhonContent *AbstractElement::phon_content( const TextPolicy& tp ) const {
    /// Get the PhonContent explicitly associated with this element.
    /*!
     * \param tp the TextPolicy to use
     *
     * Returns the PhonContent instance rather than the actual text.
     * (so it might return iself.. ;)
     * Does not recurse into children with the sole exception of Correction
     * might throw NoSuchPhon exception if not found.
     */
    string desired_class = tp.get_class();
    if ( isinstance<PhonContent>() ){
      if  ( cls() == desired_class ){
	return dynamic_cast<const PhonContent*>(this);
      }
      else {
	throw NoSuchPhon( this,
			  xmltag() + "::phon_content(" + desired_class + ")" );
      }
    }
    bool show_hidden = tp.is_set( TEXT_FLAGS::HIDDEN );
    if ( !speakable() || ( hidden() && !show_hidden ) ) {
      throw NoSuchPhon( this,
			"non-speakable element: " + xmltag() );
    }

    for ( const auto& el : _data ) {
      if ( el->isinstance<PhonContent>() && ( el->cls() == desired_class ) ) {
	return dynamic_cast<PhonContent*>(el);
      }
      else if ( el->isinstance<Correction>() ) {
	try {
	  return el->phon_content(tp);
	} catch ( const NoSuchPhon& e ) {
	  // continue search for other Corrections or a TextContent
	}
      }
    }
    throw NoSuchPhon( this,
		      xmltag() + "::phon_content(" + desired_class + ")" );
  }

  PhonContent *AbstractElement::phon_content( const TextPolicy& tp ) {
    return const_cast<PhonContent*>
      ( static_cast<const AbstractElement &>(*this).phon_content( tp ) );
  }

  const PhonContent *AbstractElement::phon_content( const string& cls,
						    bool txt_dbg ) const {
    /// Get the PhonContent explicitly associated with this element.
    /*!
     * \param cls the textclass to search for
     * \param txt_dbg enable txt_dbgging when true
     *
     * Returns the PhonContent instance rather than the actual text.
     * (so it might return iself.. ;)
     * Does not recurse into children with the sole exception of Correction
     * might throw NoSuchPhon exception if not found.
     */
    TextPolicy tp(cls );
    tp.set_debug( txt_dbg );
    return phon_content( tp );
  }

  PhonContent *AbstractElement::phon_content( const string& cls,
					      bool txt_dbg ) {
    return const_cast<PhonContent*>
      ( static_cast<const AbstractElement &>(*this).phon_content( cls, txt_dbg ) );
  }

  const UnicodeString AbstractElement::phon( const TextPolicy& tp ) const {
    /// get the UnicodeString phon value of an element
    /*!
     * \param tp the TextPolic to use
     */
    bool show_hidden = tp.is_set( TEXT_FLAGS::HIDDEN );
    bool strict = tp.is_set( TEXT_FLAGS::STRICT );
    if ( tp.debug() ){
      DBG << "PHON, Policy= " << tp << " on node : " << xmltag() << " id="
	   << id() << endl;
    }
    if ( strict ) {
      return phon_content(tp)->phon();
    }
    else if ( !speakable() || ( this->hidden() && !show_hidden ) ) {
      throw NoSuchPhon( this,
			"NON speakable element: " + xmltag() );
    }
    else {
      UnicodeString result = deepphon( tp );
      if ( result.isEmpty() ) {
	result = phon_content(tp)->phon();
      }
      if ( result.isEmpty() ) {
	throw NoSuchPhon( this,
			  "on tag " + xmltag() + " nor it's children" );
      }
      return result;
    }
  }

  const UnicodeString AbstractElement::phon( const string& cls,
					     TEXT_FLAGS flags,
					     bool debug ) const {
    /// get the UnicodeString phon value of an element
    /*!
     * \param cls the textclass the text should be in
     * \param flags the search parameters to use. See TEXT_FLAGS.
     */
    TextPolicy tp( cls, flags );
    tp.set_debug( debug );
    return phon( tp );
  }

  const UnicodeString AbstractElement::deepphon( const TextPolicy& tp ) const {
    /// get the UnicodeString phon value of underlying elements
    /*!
     * \param tp the TextPolicu to use
     * \return The Unicode Text found.
     * Will throw on error.
     */
    if ( tp.debug() ){
      DBG << "deepPHON, policy= " << tp << ", on node : " << xmltag()
	   << " id=" << id() << endl;
      DBG << "deepphon: node has " << _data.size() << " children." << endl;
    }
    vector<UnicodeString> parts;
    vector<UnicodeString> seps;
    for ( const auto* child : _data ) {
      // try to get text dynamically from children
      // skip PhonContent elements
      if ( tp.debug() ){
	if ( !child->speakable() ) {
	  DBG << "deepphon: node[" << child->xmltag() << "] NOT SPEAKABLE! "
	     << endl;
	}
      }
      if ( child->speakable()
	   && !child->isinstance<PhonContent>() ) {
	if ( tp.debug() ){
	  DBG << "deepphon:bekijk node[" << child->xmltag() << "]" << endl;
	}
	try {
	  UnicodeString tmp = child->phon( tp );
	  if ( tp.debug() ){
	    DBG << "deepphon found '" << tmp << "'" << endl;
	  }
	  parts.push_back(tmp);
	  // get the delimiter
	  const string& delim = child->get_delimiter(tp);
	  if ( tp.debug() ){
	    DBG << "deepphon:delimiter van "<< child->xmltag()
		 << " ='" << delim << "'" << endl;
	  }
	  seps.push_back(TiCC::UnicodeFromUTF8(delim));
	} catch ( const NoSuchPhon& e ) {
	  if ( tp.debug() ){
	    DBG << "HELAAS" << endl;
	  }
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
    if ( tp.debug() ){
      DBG << "deepphon() for " << xmltag() << " step 3 " << endl;
    }
    if ( result.isEmpty() ) {
      try {
	result = phon_content(tp)->phon();
      }
      catch ( ... ) {
      }
    }
    if ( tp.debug() ){
      DBG << "deepphontext() for " << xmltag() << " result= '" << result
	   << "'" << endl;
    }
    if ( result.isEmpty() ) {
      throw NoSuchPhon( this,
			xmltag() + ":(class=" + tp.get_class() +"): empty!" );
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
     * \param child The element to substitute
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
      replace[0]->destroy();
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
    FoliaElement *result = 0;
    auto it = find_if( _data.begin(),
		       _data.end(),
		       [&]( const FoliaElement *el ){ return el == old; } );
    if ( it != _data.end() ){
      *it = _new;
      result = old;
      _new->set_parent(this);
    }
    return result;
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

  vector<ProcessingInstruction*> AbstractElement::getPI( const string& target ){
    /// get PI nodes for this Element. Non recursive.
    /*!
     * \param target Only return PI's with this target. Or all if target
     * is empty
     * \return all targets found
     */
    auto PIS = select<ProcessingInstruction>(SELECT_FLAGS::LOCAL);
    if ( target.empty() ){
      return PIS;
    }
    else {
      vector<ProcessingInstruction*> result;
      for ( const auto& it : PIS ){
	if ( it->target() == target ){
	  result.push_back( it );
	}
      }
      return result;
    }
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
    if ( it != accepted_data().end() ) {
      return true;
    }
    else {
      return any_of( accepted_data().cbegin(),
		     accepted_data().cend(),
		     [t]( const ElementType& et) {
		       return ( folia::is_subtype( t, et ) ); } );
    }
  }

  bool AbstractElement::addable( const FoliaElement *parent ) const {
    /// test if an element might succesfully appended to \em parent
    /*!
     * \param parent the node to check
     * \return true if it doesn't throw
     *
     * \note It will allways throw an error, instead of returning false
     */
    if ( !parent->acceptable( element_id() ) ) {
      string mess = "Unable to append object of type " + classname()
	+ " to a <" + parent->classname() + ">";
      if ( !parent->id().empty() ){
	mess += " (id=" + parent->id() + ")";
      }
      throw ValueError( this, mess );
    }
    if ( occurrences() > 0 ) {
      vector<FoliaElement*> v = parent->select( element_id(),
						SELECT_FLAGS::LOCAL );
      size_t count = v.size();
      if ( count >= occurrences() ) {
	string mess = "Unable to add another object of type " + classname()
	  + " to " + parent->classname() + ". There are already "
	  + TiCC::toString(count)
	  + " instances of this type, which is the maximum.";
	throw DuplicateAnnotationError( this, mess );
      }
    }
    if ( occurrences_per_set() > 0 &&
	 ( ( required_attributes() % Attrib::CLASS ) || setonly() ) ){
      vector<FoliaElement*> v = select( element_id(),
					sett(),
					SELECT_FLAGS::LOCAL );
      size_t count = v.size();
      if ( count >= occurrences_per_set() ) {
	string mess = "Unable to add another object of type " + classname()
	  + " to " + parent->classname() + ". There are already "
	  + TiCC::toString(count) + " instances of this type and set ("
	  + sett() + "), which is the maximum.";
	throw DuplicateAnnotationError( this, mess );
      }
    }
    if ( _parent &&
	 !( isinstance<WordReference>()
	    || referable() ) ){
      throw XmlError( this,
		      "attempt to reconnect node " + classname() + "("
		      + id()
		      + ") to a " + parent->classname() + " node, id="
		      + parent->id()
		      + ", it was already connected to a "
		      +  parent->classname() + " id=" + parent->id() );
    }
#ifdef NOT_WORKING
    // this fails. needs attention
    if ( c->element_id() == WordReference_t ){
      string tval = atts["t"];
      if ( !tval.empty() ){
      	string tc = ref->textclass();
      	string rtval = ref->str(tc);
      	if ( tval != rtval ){
      	  throw XmlError( this,
			  "WordReference id=" + id + " has another value for "
      			  + "the t attribute than it's reference. ("
      			  + tval + " versus " + rtval + ")" );
      	}
      }
    }
#endif
    if ( isinstance<TextContent>()
	 && parent->isinstance<Word>() ) {
      string val = str(cls());
      val = trim( val );
      if ( val.empty() ) {
	// we have to check for a child with the IMPLICITSPACE property
	// ONLY in that case, an "empty" text is allowed.
	auto implicit = []( auto elt ){ return elt->implicitspace(); };
	bool has_implicit = std::any_of( _data.begin(), _data.end(),
					 implicit );
	if ( !has_implicit ){
	  throw ValueError( this,
			    "attempt to add an empty <t> to word: "
			    + parent->id() );
	}
      }
    }
    if ( isinstance<TextContent>() ){
      string my_cls = cls();
      string st = sett();
      vector<TextContent*> tmp = parent->select<TextContent>( st,
							      SELECT_FLAGS::LOCAL );
      if ( any_of( tmp.cbegin(),
		   tmp.cend(),
		   [my_cls]( const TextContent *t) { return ( t->cls() == my_cls);} ) ){
	throw DuplicateAnnotationError( this,
					"attempt to add <t> with class="
					+ my_cls + " to element: "
					+ parent->id()
					+ " which already has a <t> with that class" );
	}
    }
    if ( is_textcontainer() ||
	 isinstance<Word>() ){
      parent->check_append_text_consistency( this );
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
     * For TextContent and PhonContent, a default is added too
     *
     * Also the ID is registered in the_doc.
     *
     * Finaly, all children are also assigned to the_doc
     */
    if ( !_mydoc ) {
      _mydoc = the_doc;
      if ( annotation_type() != AnnotationType::NO_ANN
	   && !the_doc->version_below( 2, 0 )
	   && !the_doc->declared( annotation_type() ) ){
	// DBG << "assignDoc: " << this << endl;
	// DBG << "ant: " << annotation_type() << endl;
	// DBG << "set: " << _set << endl;
	// so when appending a document-less child, make sure that
	// an annotation declaration is present or added.
	if ( annotation_type() ==  AnnotationType::TEXT ){
	  if ( _set.empty() ){
	    doc()->declare( AnnotationType::TEXT, DEFAULT_TEXT_SET );
	  }
	  else {
	    doc()->declare( AnnotationType::TEXT, _set );
	  }
	}
	else if ( annotation_type() == AnnotationType::PHON ){
	  if ( _set.empty() ){
	    doc()->declare( AnnotationType::PHON, DEFAULT_PHON_SET );
	  }
	  else {
	    doc()->declare( AnnotationType::PHON, _set );
	  }
	}
	else if ( doc()->autodeclare() ){
	  doc()->auto_declare( annotation_type(), _set );
	}
	else {
	  throw DeclarationError( this,
				  "Encountered an instance of <"
				  + xmltag()
				  + "> without a proper declaration for "
				  + toString(annotation_type())
				  + "-annotation" );
	}
      }
      if ( !_set.empty()
	   && ( required_attributes() % Attrib::CLASS )
	   && !_mydoc->declared( annotation_type(), _set ) ) {
	throw DeclarationError( this,
				"Set " + _set + " is used in " + xmltag()
				+ "element: " + _id
				+ " but has no declaration "
				+ "for " + toString( annotation_type() )
				+ "-annotation" );
      }
      if ( !_id.empty() ) {
	_mydoc->add_doc_index( this );
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
	 && ( required_attributes() % Attrib::ID  ) ) {
      throw ValueError( this,
			"attribute 'ID' is required for " + classname() );
    }
    if ( _set.empty()
	 && ( required_attributes() % Attrib::CLASS ) ) {
      throw ValueError( this,
			"attribute 'set' is required for " + classname() );
    }
    if ( _class.empty()
	 && ( required_attributes() % Attrib::CLASS ) ) {
      throw ValueError( this,
			"attribute 'class' is required for " + classname() );
    }
    if ( _annotator.empty()
	 && ( required_attributes() % Attrib::ANNOTATOR ) ) {
      throw ValueError( this,
			"attribute 'annotator' is required for " + classname() );
    }
    if ( _annotator_type == AnnotatorType::UNDEFINED
	 && (  required_attributes() % Attrib::ANNOTATOR ) ) {
      throw ValueError( this,
			"attribute 'Annotatortype' is required for " + classname() );
    }
    if ( _confidence == -1 &&
	 ( required_attributes() % Attrib::CONFIDENCE  ) ) {
      throw ValueError( this,
			"attribute 'confidence' is required for " + classname() );
    }
    if ( _n.empty()
	 && ( required_attributes() % Attrib::N  ) ) {
      throw ValueError( this,
			"attribute 'n' is required for " + classname() );
    }
    if ( _datetime.empty()
	 && ( required_attributes() % Attrib::DATETIME  ) ) {
      throw ValueError( this,
			"attribute 'datetime' is required for " + classname() );
    }
    if ( _begintime.empty()
	 && ( required_attributes() % Attrib::BEGINTIME  ) ) {
      throw ValueError( this,
			"attribute 'begintime' is required for " + classname() );
    }
    if ( _endtime.empty()
	 && ( required_attributes() % Attrib::ENDTIME  ) ) {
      throw ValueError( this,
			"attribute 'endtime' is required for " + classname() );
    }
    if ( _src.empty()
	 && ( required_attributes() % Attrib::SRC  ) ) {
      throw ValueError( this,
			"attribute 'src' is required for " + classname() );
    }
    if ( _metadata.empty()
	 && ( required_attributes() % Attrib::METADATA  ) ) {
      throw ValueError( this,
			"attribute 'metadata' is required for " + classname() );
    }
    if ( _speaker.empty()
	 && ( required_attributes() % Attrib::SPEAKER  ) ) {
      throw ValueError( this,
			"attribute 'speaker' is required for " + classname() );
    }
    return true;
  }

  void AbstractElement::classInit(){
    // we could call 'init()' directly, but this is more esthetic
    // keep in balance with the next function
    init(); // virtual init
  }

  void AbstractElement::classInit( const KWargs& a ){
    // this funcion is needed because calling the virtual function
    // setAttributes from the constructor will NOT call the right version
    // THIS IS BY DESIGN in C++
    init(); // virtual init
    KWargs a1 = a;
    setAttributes( a1 ); // also virtual!
    checkAtts(); // check if all needed attributes are set
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
      throw XmlError( this,
		      "attempt to append an empty node to a " + classname() );
    }
    bool ok = false;
    try {
      ok = child->addable( this );
    }
    catch ( const XmlError& ) {
      // don't delete the offending child in case of illegal reconnection
      // it will be deleted by the true parent
      throw;
    }
    catch ( const exception& ) {
      //      child->destroy(); This somehow crashes.
      // we are exiting the program anyway
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
      if ( child->spaces_flag() == SPACE_FLAGS::UNSET ){
	child->set_spaces_flag( _preserve_spaces );
      }
      return child->postappend();
    }
    return 0;
  }

  FoliaElement *AbstractElement::postappend( ) {
    /// perform some post correction after appending
    if ( id().empty()
	 && (required_attributes() % Attrib::ID )
	 && auto_generate_id() ){
      _id = generateId( xmltag() );
    }
    return this;
  }

  void AbstractElement::remove( FoliaElement *child ) {
    /// remove a child from a node
    /*!
     * \param child the element to remove
     */
    if ( doc() && doc()->debug % DocDbg::MEMORY ){
      DBG << "\nremove " << child->xmltag();
      dbg( " from" );
      DBG << " id=" << _id << " class= " << endl;
    }
    auto it = std::remove( _data.begin(), _data.end(), child );
    _data.erase( it, _data.end() );
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

  FoliaElement* AbstractElement::opaque_index( size_t i ) const {
    /// return the child at index i
    /*!
     * \param i the index
     * \return the child at index i or highet skipping XmlComment
     *
     * Will throw when the index is out of range
     */
    if ( i < _data.size() ) {
      while ( _data[i]->isinstance<XmlComment>()
	      && ++i < _data.size() );
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

  vector<FoliaElement*> AbstractElement::select_set( const set<ElementType>& elts,
						     const string& st,
						     const set<ElementType>& exclude,
						     SELECT_FLAGS flag ) const {
    /// A generic 'select()' function which returns all matching elements
    ///   it searches a FoLiA node for matching sibblings.
    ///   returns a mixed vector of FoLiAElement nodes
    /*!
     * \param elts a set of types of elements we are looking for
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
     * \return An ordered vector of FoLiAElement nodes that match the conditions
     *
     */
    vector<FoliaElement*> res;
    for ( const auto& el : _data ) {
      if ( elts.find(el->element_id()) != elts.end() &&
	   ( st.empty() || el->sett() == st ) ) {
	res.push_back( el );
	if ( flag == SELECT_FLAGS::TOP_HIT ){
	  flag = SELECT_FLAGS::LOCAL;
	}
      }
      if ( flag != SELECT_FLAGS::LOCAL ){
	// not at this level, search deeper when recurse is true
	if ( exclude.find( el->element_id() ) == exclude.end() ) {
	  vector<FoliaElement*> tmp = el->select_set( elts, st, exclude, flag );
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
     * recursively go through this node and its children an collect all
     * node pointers in store.
     * Erase the _data array of every node
     *
     * This function is used when erasing a document. Creating a set avoids
     * deleting nodes twice
     */
    resetrefcount();
    _parent = 0;
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
    KWargs atts = getAttributes( node );
    int sp = xmlNodeGetSpacePreserve(node);
    if ( sp == 1 ){
      atts.add("xml:space","preserve");
    }
    else if ( sp == 0 ){
      atts.add("xml:space","default");
    }
    setAttributes( atts );
    set_line_number( xmlGetLineNo(node) );
    const xmlNode *p = node->children;
    while ( p ) {
      set_line_number( xmlGetLineNo(p) );
      string pref;
      string ns = getNS( p, pref );
      if ( !ns.empty() && ns != NSFOLIA ){
	// skip alien nodes
	if ( doc() && doc()->debug % DocDbg::PARSING ) {
	  DBG << "skipping non-FoLiA node: " << pref << ":" << Name(p) << endl;
	}
	p = p->next;
	continue;
      }
      if ( p->type == XML_ELEMENT_NODE ) {
	string xml_tag = Name( p );
	FoliaElement *t = 0;
	try {
	  t = createElement( xml_tag, doc() );
	}
	catch ( const exception& e ){
	  if ( doc() && !doc()->permissive() ){
	    throw XmlError( this,
			    string( "parsing <" ) + xml_tag + "> failed:\n\t"
			    + e.what() );
	  }
	}
	if ( doc() && doc()->debug % DocDbg::PARSING ) {
	  DBG << "created " << t << endl;
	}
	t = t->parseXml( p );
	if ( t ) {
	  if ( doc() && doc()->debug % DocDbg::PARSING ) {
	    DBG << "extend " << this << " met " << t << endl;
	  }
	  append( t );
	}
      }
      else if ( p->type == XML_PI_NODE ){
	// found a processing instruction
	string xml_tag = "PI";
	FoliaElement *t;
	try {
	  t = createElement( xml_tag, doc() );
	}
	catch ( const exception& e ){
	  throw XmlError( this,
			  string( "parsing " ) + xml_tag + " failed:\n\t"
			  + e.what() );
	}
	if ( doc() && doc()->debug % DocDbg::PARSING ) {
	  DBG << "created " << t << endl;
	}
	t = t->parseXml( p );
	if ( t ) {
	  if ( doc() && doc()->debug % DocDbg::PARSING ) {
	    DBG << "extend " << this << " met " << t << endl;
	  }
	  append( t );
	}
      }
      else if ( p->type == XML_COMMENT_NODE ) {
	string xml_tag = "_XmlComment";
	FoliaElement *t;
	try {
	  t = createElement( xml_tag, doc() );
	}
	catch ( const exception& e ){
	  throw XmlError( this,
			  string( "parsing " ) + xml_tag + " failed:\n\t"
			  + e.what() );
	}
	if ( doc() && doc()->debug % DocDbg::PARSING ) {
	  DBG << "created " << t << endl;
	}
	t = t->parseXml( p );
	if ( t ) {
	  if ( doc() && doc()->debug % DocDbg::PARSING ) {
	    DBG << "extend " << this << " met " << t << endl;
	  }
	  append( t );
	}
      }
      else if ( p->type == XML_ENTITY_REF_NODE ){
	string txt = TextValue( p );
	const XmlText *t = add_child<XmlText>( txt );
	if ( doc() && doc()->debug % DocDbg::PARSING ) {
	  DBG << "created " << t << "(" << t->text() << ")" << endl;
	  DBG << "extended " << this << " met " << t << endl;
	  DBG << "this.size()= " << size() << " t.size()=" << t->size() << endl;
	}
      }
      else if ( p->type == XML_TEXT_NODE ){
	if ( this->is_textcontainer()
	     || this->is_phoncontainer() ){
	  // non empty text is allowed (or even required) here
	  string txt = TextValue( p );
	  if ( !txt.empty() ) {
	    const XmlText *t = add_child<XmlText>( txt );
	    if ( doc() && doc()->debug % DocDbg::PARSING ) {
	      DBG << "created " << t << "(" << t->text() << ")" << endl;
	      DBG << "extended " << this << " met " << t << endl;
	      DBG << "this.size()= " << size() << " t.size()=" << t->size() << endl;
	    }
	  }
	}
	else {
	  // This MUST be 'empty space', so only spaces and tabs formatting
	  string txt = TextValue( p );
	  txt = TiCC::trim( txt );
	  if ( !txt.empty() ){
	    if ( p->prev ){
	      string tg = "<" + Name(p->prev) + ">";
	      throw XmlError( this,
			      "found extra text '" + txt + "' after element "
			      + tg + ", NOT allowed there." );
	    }
	    else {
	      string tg = "<" + Name(p->parent) + ">";
	      throw XmlError( this,
			      "found extra text '" + txt + "' inside element "
			      + tg + ", NOT allowed there." );
	    }
	  }
	  else {
	    // just skip it
	  }
	}
      }
      p = p->next;
    }
    if ( doc() && ( doc()->checktext() || doc()->fixtext() )
	 && this->printable()
	 && !isSubClass<Morpheme>() && !isSubClass<Phoneme>() ){
      check_text_consistency_while_parsing( true,
					    doc()->debug % DocDbg::TEXTHANDLING );
    }
    return this;
  }

  void AbstractElement::setDateTime( const string& s ) {
    /// set the DATETIME value of a node
    /*!
     * \param s a date/time in ISO.... format. (YYYY-MM-DDThh:mm:ss)
     */
    Attrib supported = required_attributes() | optional_attributes();
    if ( !( supported % Attrib::DATETIME) ) {
      throw ValueError( this,
			"datetime is not supported for " + classname() );
    }
    else {
      string time = parseDate( s );
      if ( time.empty() ) {
	throw ValueError( this,
			  "invalid datetime, must be in YYYY-MM-DDThh:mm:ss format: " + s );
      }
      _datetime = time;
    }
  }

  const string AbstractElement::getDateTime() const {
    /// return the _datetime value
    return _datetime;
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
    string st = args.lookup("set" );
    string newId = args.extract("generate_id" );
    if ( newId.empty() ){
      newId = "alt-pos";
    }
    if ( has_annotation<PosAnnotation>( st ) > 0 ) {
      // ok, there is already one, so create an Alternative
      KWargs kw("xml:id", generateId( newId ));
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
	  if ( alt->index(j)->isinstance<PosAnnotation>()
	       && ( st.empty() || alt->index(j)->sett() == st ) ) {
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
    string st = args.lookup("set");
    string newId = args.extract("generate_id" );
    if ( newId.empty() ){
      newId = "alt-lem";
    }
    if ( has_annotation<LemmaAnnotation>( st ) > 0 ) {
      // ok, there is already one, so create an Alternative
      KWargs kw( "xml:id",generateId( newId ));
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
	  if ( alt->index(j)->isinstance<LemmaAnnotation>()
	       && ( st.empty() || alt->index(j)->sett() == st ) ) {
	    alts.push_back( dynamic_cast<LemmaAnnotation*>(alt->index(j)) );
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
      string new_id = generateId( "s" );
      kw.add("xml:id", new_id);
    }
    try {
      res = new Sentence( kw, doc() );
    }
    catch( const DuplicateIDError& e ) {
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
      string new_id = generateId( "w" );
      kw.add("xml:id",new_id);
    }
    try {
      res->setAttributes( kw );
    }
    catch( const DuplicateIDError& e ) {
      res->destroy();
      throw;
    }
    append( res );
    return res;
  }

  Word *AbstractElement::addWord( const string& s ){
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
      args.add("text",s);
    }
    return addWord( args );
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
    // DBG << "node: " << this << endl;
    // DBG << "ID=" << nodeId << endl;
    if ( nodeId.empty() ){
      // if no ID, look upward.
      FoliaElement *par = parent();
      if ( !par ){
	throw XmlError( this,
			"unable to generate an ID. No StructureElement parent found?" );
      }
      // DBG << "call on parent:" << par << endl;
      return par->generateId( tag );
    }
    else {
      int max = 0;
      if ( !tag.empty() ) {
	max = ++id_map[tag];
      }
      // DBG << "MAX = " << max << endl;
      string id = nodeId + '.' + tag + '.' +  TiCC::toString( max );
      // DBG << "new id = " << id << endl;
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
    Document *doc = this->doc();
    bool corr_debug = false;
    if ( doc ){
      corr_debug = ( doc->debug % DocDbg::CORRECTION );
    }
    if ( corr_debug ){
      DBG << "correct " << this << endl;
      DBG << "original= " << _original << endl;
      DBG << "current = " << _current << endl;
      DBG << "new     = " << _newv << endl;
      DBG << "suggestions     = " << _suggestions << endl;
      DBG << "args in     = " << args_in << endl;
    }
    // Apply a correction
    Correction *corr = 0;
    bool hooked = false;
    New *addnew = 0;
    KWargs args = args_in;
    vector<FoliaElement*> original = _original;
    vector<FoliaElement*> _new = _newv;
    vector<FoliaElement*> suggestions = _suggestions;
    string val = args.extract("new");
    if ( !val.empty() ){
      KWargs my_args("value",val);
      TextContent *t = new TextContent( my_args, doc );
      _new.push_back( t );
    }
    else {
      val = args.extract("suggestion");
      if ( !val.empty() ){
	KWargs my_args("value",val);
	TextContent *t = new TextContent( my_args, doc );
	suggestions.push_back( t );
      }
    }
    val = args.lookup("reuse");
    if ( !val.empty() ){
      // reuse an existing correction instead of making a new one
      if ( !doc ){
	throw ValueError( this,
			  "reuse= needs a Document" );
      }
      try {
	corr = dynamic_cast<Correction*>(doc->index(val));
      }
      catch ( const exception& e ) {
	throw ValueError( this,
			  "reuse= must point to an existing correction id!" );
      }
      if ( !corr->isinstance<Correction>() ) {
	throw ValueError( this,
			  "reuse= must point to an existing correction id!" );
      }
      hooked = true;
      if ( !_new.empty() && corr->hasCurrent() ) {
	// can't add new if there's current, so first set original to current, and then delete current

	if ( !_current.empty() ) {
	  throw runtime_error( "Can't set both new= and current= !");
	}
	if ( original.empty() ) {
	  // move the current to Original
	  FoliaElement *cur = corr->getCurrent();
	  original.push_back( cur );
	  corr->remove( cur );
	}
      }
    }
    else {
      KWargs args2 = args;
      args2.erase("suggestion" );
      args2.erase("suggestions" );
      string id = generateId( "correction" );
      args2.add("xml:id",id);
      corr = new Correction( args2, doc );
    }
    if ( corr_debug ){
      DBG << "now corr= " << corr << endl;
    }
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
      if ( corr_debug ){
	DBG << "now corr= " << corr << endl;
      }
    }
    if ( !_new.empty() ) {
      if ( corr_debug ){
	DBG << "there is new! " << endl;
      }
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
      if ( corr_debug ){
	DBG << "after adding NEW: " << corr->xmlstring() << endl;
      }
      vector<Current*> v = corr->select<Current>();
      //delete current if present
      for ( const auto& cur:v ) {
	corr->remove( cur );
      }
      if ( corr_debug ){
	DBG << "after removing CUR: " << corr->xmlstring() << endl;
      }
    }
    else if ( !original.empty() ){
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
      if ( corr_debug ){
	DBG << "there is original! " << endl;
      }
      FoliaElement *add = new Original( doc );
      corr->replace(add);
      if ( corr_debug ){
	DBG << " corr after replacing original " << corr->xmlstring() << endl;
	DBG << " new original= " << add << endl;
      }
      for ( const auto& org: original ) {
	if ( corr_debug ){
	  DBG << " examine org " << org << endl;
	}
	bool dummyNode = ( org->id() == "dummy" );
	if ( !dummyNode ) {
	  org->set_parent(0);
	  add->append( org );
	}
	if ( corr_debug ){
	  DBG << " NOW original= " << add << endl;
	}
	for ( size_t i=0; i < size(); ++i ) {
	  if ( corr_debug ){
	    DBG << "in loop, bekijk " << index(i) << endl;
	  }
	  if ( index(i) == org ) {
	    if ( corr_debug ){
	      DBG << "OK hit on ORG :" << org << endl;
	    }
	    if ( !hooked ) {
	      if ( corr_debug ){
		DBG << "it isn't hooked!" << endl;
		const FoliaElement *tmp = replace( index(i), corr );
		DBG << " corr after replace " << corr->xmlstring() << endl;
		DBG << " replaced " << tmp << endl;
	      }
	      else {
		replace( index(i), corr );
	      }
	      hooked = true;
	    }
	    else {
	      if ( corr_debug ){
		DBG << " corr before remove " << corr << endl;
		DBG << " remove  " << org << endl;
	      }
	      this->remove( org );
	      if ( corr_debug ){
		DBG << " corr after remove " << corr << endl;
	      }
	    }
	  }
	}
      }
      if ( add->size() == 0 ){
	corr->remove( add );
      }
    }
    else if ( addnew ) {
      // original not specified, find automagically:
      vector<FoliaElement *> orig;
      if ( corr_debug ){
	DBG << "start to look for original " << endl;
      }
      for ( size_t i=0; i < len(addnew); ++ i ) {
	const FoliaElement *p = addnew->index(i);
	if ( corr_debug ){
	  DBG << "bekijk " << p << endl;
	}
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
	if ( corr_debug ){
	  DBG << "we seem to have some originals! " << endl;
	}
	FoliaElement *add = new Original( doc );
	if ( corr_debug ){
	  DBG << "corr before adding new original! " << corr << endl;
	}
	corr->replace(add);
	if ( corr_debug ){
	  DBG << "corr after adding new original! " << corr << endl;
	  DBG << "now parent = " << add->parent() << endl;
	}

	for ( const auto& org: orig ) {
	  if ( corr_debug ){
	    DBG << " examine original : " << org << endl;
	    DBG << "with parent = " << org->parent() << endl;
	  }
	  // first we lookup org in our data and remove it there
	  for ( size_t i=0; i < size(); ++i ) {
	    if ( corr_debug ){
	      DBG << "in loop, bekijk " << index(i) << endl;
	    }
	    if ( index(i) == org ) {
	      if ( corr_debug ){
		DBG << "found original " << endl;
	      }
	      if ( !hooked ) {
		if ( corr_debug ){
		  DBG << "it isn't hooked!" << endl;
		  const FoliaElement *tmp = replace( index(i), corr );
		  DBG << " corr after replace " << corr << endl;
		  DBG << " replaced " << tmp << endl;
		}
		else {
		  replace( index(i), corr );
		}
		hooked = true;
	      }
	      else {
		if ( corr_debug ){
		  DBG << " corr before remove " << corr << endl;
		  DBG << " remove  " << org << endl;
		}
		this->remove( org );
		if ( corr_debug ){
		  DBG << " corr after remove " << corr << endl;
		}
	      }
	    }
	  }
	  // now we connect org to the new original node
	  org->set_parent( 0 );
	  add->append( org );
	  if ( corr_debug ){
	    DBG << " add after append : " << add << endl;
	    DBG << "parent = " << org->parent() << endl;
	  }
	}
	vector<Current*> v = corr->select<Current>();
	//delete current if present
	for ( const auto& cur: v ) {
	  if ( corr_debug ){
	    DBG << " remove cur=" << cur << endl;
	  }
	  this->remove( cur );
	}
      }
    }
    if ( corr_debug ){
      DBG << " corr after edits " << corr->xmlstring() << endl;
    }
    if ( addnew ) {
      for ( const auto& org : original ) {
	if ( corr_debug ){
	  DBG << " remove  " << org << endl;
	}
	bool dummyNode = ( org->id() == "dummy" );
	corr->remove( org );
	if ( dummyNode ){
	  org->destroy();
	}
      }
    }
    if ( corr_debug ){
      DBG << " corr after removes " << corr->xmlstring() << endl;
    }
    if ( !suggestions.empty() ) {
      if ( !hooked ) {
	append(corr);
      }
      for ( const auto& sug : suggestions ) {
	if ( sug->isinstance<Suggestion>() ) {
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

    val = args.extract("reuse");
    if ( !val.empty() ){
      val = args.extract("annotator");
      if ( !val.empty() ){
	corr->annotator( val );
      }
      val = args.extract("annotatortype");
      if ( !val.empty() ){
	corr->annotatortype( stringTo<AnnotatorType>(val) );
      }
      val = args.extract("confidence");
      if ( !val.empty() ) {
	corr->confidence( stringTo<double>(val) );
      }
    }
    corr->check_type_consistency();
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
    //    DBG << xmltag() << "::correct() <== " << this << endl;
    Correction *tmp = correct( nil1, nil2, nil3, nil4, args );
    //    DBG << xmltag() << "::correct() ==> " << this << endl;
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
    //    DBG << xmltag() << "::correct() <== " << this << endl;
    Correction *tmp = correct( ov, nil, nv, sugg, args );
    //    DBG << xmltag() << "::correct() ==> " << this << endl;
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
    const vector<FoliaElement*> sugg;
    return correct( _old, _new, sugg, args );
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
    for ( const auto *el : data() ) {
      if ( el->isSubClass<AbstractFeature>() &&
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
    const auto& it = find_if( _data.begin(), _data.end(),
			      [s]( const FoliaElement *e ){
				return ( e->isSubClass<AbstractFeature>()
					 && e->subset() == s ); } );
    if ( it == _data.end() ){
      return "";
    }
    else {
      return (*it)->cls();
    }
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

  vector<AbstractSpanAnnotation*> AbstractElement::selectSpan() const {
    /// select all SpanAnnotation nodes in the FoliaElement
    /*!
     * \return a list of SpanAnnotation nodes.
     * All possible Span types are collected in this list. (see SpanSet)
     */
    vector<AbstractSpanAnnotation*> res;
    for ( const auto& el : SpanSet ) {
      vector<FoliaElement*> tmp = select( el );
      transform( tmp.begin(), tmp.end(),
		 back_inserter(res),
		 [&]( FoliaElement *e ){
		   return dynamic_cast<AbstractSpanAnnotation*>( e ); } );
    }
    return res;
  }

} // namespace folia
