/*
  Copyright (c) 2006 - 2023
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

  const UnicodeString FoLiA::private_text( const TextPolicy& tp ) const {
    /// get the UnicodeString value of a FoLiA topnode
    /*!
     * \param tp The TextPolicy to use
     * \return the Unicode String representation found. Throws when
     * no text can be found
     */
    if ( tp.debug() ){
      cerr << "FoLiA::private_text(" << tp.get_class() << ")" << endl;
    }
    UnicodeString result;
    for ( const auto& d : data() ){
      if ( !result.isEmpty() ){
	const string& delim = d->get_delimiter( tp );
	result += TiCC::UnicodeFromUTF8(delim);
      }
      result += d->private_text( tp );
    }
    if ( tp.debug() ){
      cerr << "FoLiA::TEXT returns '" << result << "'" << endl;
    }
    return result;
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
      if ( doc()->checktext()
	   && _offset != -1 ){
	doc()->cache_phoncontent(this);
      }
      if ( !doc()->declared( AnnotationType::PHON ) ){
	doc()->declare( AnnotationType::PHON, DEFAULT_PHON_SET );
      }
    }
    return this;
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

  const string& Quote::get_delimiter( const TextPolicy& tp ) const {
    /// get the default delimiter of a Quote object.
    /*!
     * \param tp the TextPolicy to use
     * \return a string representing the delimiter
     * When the last data item in the Quote is a sentence, we don't
     * want a delimiter and return ""
     */
    bool retaintok  = tp.is_set( TEXT_FLAGS::RETAIN );
    if ( tp.debug() ){
      cerr << "IN " << xmltag() << "::get_delimiter (" << retaintok << ")"
	   << endl;
    }
    const vector<FoliaElement*>& data = this->data();
    auto it = data.rbegin();
    while ( it != data.rend() ) {
      if ( (*it)->isinstance( Sentence_t ) ) {
	// if a quote ends in a sentence, we don't want any delimiter
	if ( tp.debug() ){
	  cerr << "OUT " << xmltag() << "::get_delimiter ==>''" << endl;
	}
	return EMPTY_STRING;
      }
      else {
	const string& res = (*it)->get_delimiter( tp );
	if ( tp.debug() ){
	  cerr << "OUT " << xmltag() << "::get_delimiter ==> '"
	       << res << "'" << endl;
	}
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
    /// create a Correction where an extra word is inserted
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
    if ( any_of( _new.begin(),
		 _new.end(),
		 []( const FoliaElement *e ){ return !e->isinstance( Word_t ); } ) ){
      throw runtime_error("new word is not a Word instance" );
    }
    auto ait = argsin.find("suggest");
    if ( ait != argsin.end() && ait->second == "true" ) {
      FoliaElement *sugg = new Suggestion();
      for ( const auto& nw : _new ) {
	sugg->append( nw );
      }
      vector<FoliaElement *> o_tmp;
      vector<FoliaElement *> c_tmp = orig;
      vector<FoliaElement *> n_tmp;
      vector<FoliaElement *> s_tmp;
      s_tmp.push_back( sugg );
      KWargs args = argsin;
      args.erase("suggest");
      return correct( o_tmp, c_tmp, n_tmp, s_tmp, args );
    }
    else {
      vector<FoliaElement *> o_tmp = orig;
      vector<FoliaElement *> c_tmp;
      vector<FoliaElement *> n_tmp = _new;
      vector<FoliaElement *> s_tmp;
      return correct( o_tmp, c_tmp, n_tmp, s_tmp, argsin );
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
      add_child<XmlText>( value );
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
     * that is a Structure, String or Subtoken which may hold TextContent
     */
    int depth = 0;
    FoliaElement *p = parent();
    while ( p ){
      if ( ( p->isSubClass( String_t )
	     || p->isSubClass( AbstractStructureElement_t )
	     || p->isSubClass( AbstractSubtokenAnnotation_t ) )
	   && p->acceptable( TextContent_t ) ){
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
      TextPolicy tp( cls(), TEXT_FLAGS::STRICT );
      if ( !trim_spaces ) {
	tp.set( TEXT_FLAGS::NO_TRIM_SPACES );
      }
      UnicodeString mt = this->text( tp );
      UnicodeString pt = ref->text( tp );
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
    if ( cls() == "current" && doc() && !doc()->has_explicit() ) {
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

  FoliaElement *PhonContent::get_reference( bool trim_spaces ) const {
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
      TextPolicy tp( cls(), TEXT_FLAGS::STRICT );
      if ( !trim_spaces ) {
	tp.set( TEXT_FLAGS::NO_TRIM_SPACES );
      }
      UnicodeString mt = this->phon( tp );
      UnicodeString pt = ref->phon( tp );
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
     * search in the DIRECT children for nodes with the same tag, set and
     * textclass as the element par
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

  const UnicodeString PhonContent::phon( const TextPolicy& tp ) const {
    /// get the UnicodeString phon value
    /*!
     * \param tp the TextPolicy to use
     */
    if ( tp.debug() ){
      cerr << "PhonContent::PHON, Policy= " << tp << endl;
    }
    string desired_class = tp.get_class();
    UnicodeString result;
    for ( const auto& el : data() ) {
      // try to get text dynamically from children
      if ( tp.debug() ){
	cerr << "PhonContent: bekijk node[" << el->str( tp ) << endl;
      }
      try {
	if ( tp.debug() ){
	  cerr << "roep text(" << desired_class << ") aan op " << el << endl;
	}
	UnicodeString tmp = el->text( tp );
	if ( tp.debug() ){
	cerr << "PhonContent found '" << tmp << "'" << endl;
	}
	result += tmp;
      } catch ( const NoSuchPhon& e ) {
	if ( tp.debug() ){
	  cerr << "PhonContent::HELAAS" << endl;
	}
      }
    }
    result.trim();
    if ( tp.debug() ){
      cerr << "PhonContent return " << result << endl;
    }
    return result;
  }

  const UnicodeString PhonContent::phon( const string& cls,
					 TEXT_FLAGS flags ) const {
    /// get the UnicodeString phon value
    /*!
      \param cls the textclass the text should be in
      \param flags the TEXT_FLAGS for the TextPolicy to use
      \return the UnicodeString with the phon content
    */
    TextPolicy tp( cls, flags );
    return phon( tp );
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

  const string& Word::get_delimiter( const TextPolicy& tp ) const {
    /// get the default delimiter of a Word
    /*!
     * \param tp the TextPolicy to use
     * \return a string representing the delimiter
     */
    bool retaintok  = tp.is_set( TEXT_FLAGS::RETAIN );
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
      child->destroy();
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
      xmlAddChild( e,
		   xmlNewText( to_xmlChar(_value.c_str())) );
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
      xmlAddChild( e,
		   xmlNewText( to_xmlChar(_value.c_str())) );
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
      set_set( c_set );
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
				      to_xmlChar(value.c_str()),
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

  //#define DEBUG_TEXT_CORRECTION

  const UnicodeString Correction::private_text( const TextPolicy& tp ) const {
    /// get the UnicodeString value of an Correction
    /*!
     * \param tp the TextPolicy to use
     * \return the Unicode String representation found. Throws when
     * no text can be found.
     */
    if ( tp.debug() ){
      cerr << "PRIVATE_TEXT(" << tp.get_class() << ") on CORRECTION"
	   << " id=" << id() << endl;
      cerr << "TextPolicy: " << tp << endl;
    }
    //
    // we cannot use text_content() on New, Original or Current,
    // because textcontent doesn't recurse!
    bool deletion = false;
    UnicodeString new_result;
    UnicodeString org_result;
    UnicodeString cur_result;
    CORRECTION_HANDLING ch = tp.get_correction_handling();
    if ( tp.get_class() == "original" ){
      // backward compatability
      ch = CORRECTION_HANDLING::ORIGINAL;
    }
    if ( ch == CORRECTION_HANDLING::CURRENT
	 || ch == CORRECTION_HANDLING::EITHER ){
      for ( const auto& el : data() ) {
#ifdef DEBUG_TEXT_CORRECTION
	cerr << "data=" << el << endl;
#endif
	if ( el->isinstance( New_t ) ){
	  if ( el->size() == 0 ){
	    deletion = true;
	  }
	  else {
	    try {
	      new_result = el->private_text( tp );
#ifdef DEBUG_TEXT_CORRECTION
	      cerr << "New ==> '" << new_result << "'" << endl;
#endif
	    }
	    catch ( ... ){
	      // try other nodes
	    }
	  }
	}
	if ( new_result.isEmpty() ){
	  if ( el->isinstance( Current_t ) ){
	    try {
	      cur_result = el->private_text( tp );
#ifdef DEBUG_TEXT_CORRECTION
	      cerr << "Current ==> '" << cur_result << "'" << endl;
#endif
	    }
	    catch ( ... ){
	      // try other nodes
	    }
	  }
	  if ( cur_result.isEmpty()
	       && ch == CORRECTION_HANDLING::EITHER ){
	    if ( el->isinstance( Original_t ) ){
	      try {
		org_result = el->private_text( tp );
#ifdef DEBUG_TEXT_CORRECTION
		cerr << "Original ==> '" << org_result << "'" << endl;
#endif
	      }
	      catch ( ... ){
		// try other nodes
	      }
	    }
	  }
	}
      }
    }
    else if ( ch == CORRECTION_HANDLING::ORIGINAL ){
      for ( const auto& el : data() ) {
#ifdef DEBUG_TEXT_CORRECTION
      cerr << "data=" << el << endl;
#endif
      if ( el->isinstance( Original_t ) ){
	  try {
	    org_result = el->private_text( tp );
#ifdef DEBUG_TEXT_CORRECTION
	    cerr << "Orig ==> '" << org_result << "'" << endl;
#endif
	  }
	  catch ( ... ){
	    // try other nodes
	  }
	}
      }
    }
    UnicodeString final_result;
    if ( !deletion ){
      if ( !new_result.isEmpty() ){
#ifdef DEBUG_TEXT_CORRECTION
	cerr << "return new text '" << new_result << "'" << endl;
#endif
	final_result = new_result;
      }
      else if ( !cur_result.isEmpty() ){
#ifdef DEBUG_TEXT_CORRECTION
	cerr << "return cur text '" << cur_result << "'" << endl;
#endif
	final_result = cur_result;
      }
      else if ( !org_result.isEmpty() ){
#ifdef DEBUG_TEXT_CORRECTION
	cerr << "return ori text '" << org_result << "'" << endl;
#endif
	final_result = org_result;
      }
    }
    else {
      if ( !cur_result.isEmpty() ){
#ifdef DEBUG_TEXT_CORRECTION
	cerr << "Deletion: return cur text '" << cur_result << "'" << endl;
#endif
	final_result = cur_result;
      }
    }
    if ( final_result.isEmpty() ){
      throw NoSuchText( "cls=" + tp.get_class() );
    }
    if ( tp.debug() ){
      cerr << "PRIVATE_TEXT(" << tp.get_class() << ") on correction gave '"
	   << final_result << "'" << endl;
    }
    return final_result;
  }
#undef DEBUG_TEXT_CORRECTION

  const string& Correction::get_delimiter( const TextPolicy& tp ) const {
    /// get the default delimiter of a Correction
    /*!
     * \param tp the TextPolicy to use
     * \return a string representing the delimiter
     */
    for ( const auto& el : data() ) {
      //      if ( el->isinstance( New_t ) || el->isinstance( Current_t ) ) {
      return el->get_delimiter( tp );
      //      }
    }
    return EMPTY_STRING;
  }

  const TextContent *Correction::text_content( const TextPolicy& tp ) const {
    /// Get the TextContent explicitly associated with a Correction
    /*!
     * \param tp the TextPolicy to use
     *
     * Returns the TextContent instance rather than the actual text.
     * (so it might return iself.. ;)
     * recurses into children looking for New or Current nodes
     * might throw NoSuchText exception if not found.
     */
    CORRECTION_HANDLING ch = tp.get_correction_handling();
    if ( tp.get_class() == "original" ){
      // backward compatability
      ch = CORRECTION_HANDLING::ORIGINAL;
    }
    switch( ch ){
    case CORRECTION_HANDLING::CURRENT:
      // fallthrough
    case CORRECTION_HANDLING::EITHER: {
      const auto& it = find_if( data().begin(), data().end(),
				[]( const FoliaElement *e ){
				  return ( e->isinstance( New_t )
					   || e->isinstance( Current_t ) ); } );
      if ( it != data().end() ){
	return (*it)->text_content( tp );
      }
    }
      break;
    case CORRECTION_HANDLING::ORIGINAL: {
      const auto& it = find_if( data().begin(), data().end(),
				[]( const FoliaElement *e ){
				  return e->isinstance( Original_t ); } );
      if ( it != data().end() ){
	return (*it)->text_content( tp );
      }
    }
      break;
    default:
      // fallthrough
      break;
    };
    throw NoSuchText("wrong cls");
  }

  const TextContent *Correction::text_content( const string& cls,
					       bool debug ) const {
    /// Get the TextContent explicitly associated with a Correction
    /*!
     * \param cls the textclass to search for
     * \param debug enables debugging when true
     *
     * Returns the TextContent instance rather than the actual text.
     * (so it might return iself.. ;)
     * recurses into children looking for New or Current nodes
     * might throw NoSuchText exception if not found.
     */
    // TODO: this implements correctionhandling::EITHER only
    TextPolicy tp( cls );
    tp.set_debug( debug );
    tp.set_correction_handling(CORRECTION_HANDLING::EITHER);
    return text_content( tp );
  }

  Correction *Correction::correct( const vector<FoliaElement*>&,
				   const vector<FoliaElement*>& vc,
				   const vector<FoliaElement*>& vn,
				   const vector<FoliaElement*>& vs,
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

  Correction *Correction::correct( const string& args ){
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

  Correction *New::correct( const vector<FoliaElement*>& vo,
			    const vector<FoliaElement*>& vc,
			    const vector<FoliaElement*>& vn,
			    const vector<FoliaElement*>& vs,
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

  Correction *New::correct( const string& args ){
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

  bool New::addable( const FoliaElement *parent ) const {
    /// test if a New element might succesfully appended to \em parent
    /*!
     * \param parent the node to check
     * \return true if it doesn't throw
     *
     * \note It will allways throw an error, instead of returning false
     */
    if ( !AbstractElement::addable( parent ) ){
      return false;
    }
    vector<FoliaElement*> v = parent->select( Current_t, SELECT_FLAGS::LOCAL );
    if ( !v.empty() ){
      throw XmlError( "Cant't add New element to Correction if there is a Current item" );
    }
    return true;
  }

  bool Original::addable( const FoliaElement *parent ) const {
    /// test if a Original element might succesfully appended to \em parent
    /*!
     * \param parent the node to check
     * \return true if it doesn't throw
     *
     * \note It will allways throw an error, instead of returning false
     */
    if ( !AbstractElement::addable( parent ) ){
      return false;
    }
    vector<FoliaElement*> v = parent->select( Current_t, SELECT_FLAGS::LOCAL );
    if ( !v.empty() ){
      throw XmlError( "Cant't add Original element to Correction if there is a Current item" );
    }
    return true;
  }

  bool Current::addable( const FoliaElement *parent ) const {
    /// test if a Original element might succesfully appended to \em parent
    /*!
     * \param parent the node to check
     * \return true if it doesn't throw
     *
     * \note It will allways throw an error, instead of returning false
     */
    if ( !AbstractElement::addable( parent ) ){
      return false;
    }
    vector<FoliaElement*> v = parent->select( New_t, SELECT_FLAGS::LOCAL );
    if ( !v.empty() ){
      throw XmlError( "Cant't add Current element to Correction if there is a New item" );
    }
    v = parent->select( Original_t, SELECT_FLAGS::LOCAL );
    if ( !v.empty() ){
      throw XmlError( "Cant't add Current element to Correction if there is an Original item" );
    }
    return true;
  }

  const PhonContent *Correction::phon_content( const TextPolicy& tp ) const {
    /// Get the PhonContent explicitly associated with this element.
    /*!
     * \param tp the TextPolicy to use
     *
     * Returns the PhonContent instance rather than the actual text.
     * (so it might return iself.. ;)
     * recurses into children looking for New or Current
     * might throw NoSuchPhon exception if not found.
     */
    CORRECTION_HANDLING ch = tp.get_correction_handling();
    if ( tp.get_class() == "original" ){
      // backward compatability
      ch = CORRECTION_HANDLING::ORIGINAL;
    }
    switch( ch ){
    case CORRECTION_HANDLING::CURRENT:
      // fallthrough
    case CORRECTION_HANDLING::EITHER: {
      const auto& it = find_if( data().begin(), data().end(),
				[]( const FoliaElement *e ){
				  return ( e->isinstance( New_t )
					   || e->isinstance( Current_t ) ); } );
      if ( it != data().end() ){
	return (*it)->phon_content( tp );
      }
    }
      break;
    case CORRECTION_HANDLING::ORIGINAL: {
      const auto& it = find_if( data().begin(), data().end(),
				[]( const FoliaElement *e ){
				  return e->isinstance( Original_t ); } );
      if ( it != data().end() ){
	return (*it)->phon_content( tp );
      }
    }
      break;
    default:
      break;
    }
    throw NoSuchPhon("wrong cls");
  }

  const PhonContent *Correction::phon_content( const string& cls,
					       bool debug ) const {
    /// Get the PhonContent explicitly associated with this element.
    /*!
     * \param cls the textclass to search for
     * \param debug enables debugging when true
     *
     * Returns the PhonContent instance rather than the actual text.
     * (so it might return iself.. ;)
     * recurses into children looking for New or Current
     * might throw NoSuchPhon exception if not found.
     */
    // TODO: this implements correctionhandling::EITHER only
    TextPolicy tp( cls );
    tp.set_debug( debug );
    tp.set_correction_handling(CORRECTION_HANDLING::EITHER);
    return phon_content( tp );
  }

  bool Correction::space() const {
    // The space property of a correction is defined by that of
    // it's (Word) members. Start searching with the New node and so on
    // THIS IS A GROSS HACK
    bool result = AbstractElement::space();
    FoliaElement *e = getNew();
    if ( !e ){
      e = getOriginal();
    }
    if ( !e ){
      e = getCurrent();
    }
    if ( e ){
      vector<Word*> wv = e->select<Word>(false);
      if ( !wv.empty() ){
	FoliaElement *last = wv.back();
	// cerr << "Correction::space!" << last << " ==> "
	//      << (last->space()?"YES":"NO") << endl;
	result = last->space();
      }
    }
    return result;
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
    auto const hd = find_if( data().begin(),
			     data().end(),
			     []( const FoliaElement *h ){
			       return h->element_id() == Head_t;} );
    if ( hd != data().end() ){
      return dynamic_cast<Head*>(*hd);
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

  void XmlText::setuvalue( const UnicodeString& us ){
    /*!
     * \param us a Unicode string
     */
    _value = TiCC::UnicodeToUTF8( us );
  }

  void XmlText::setvalue( const string& s ){
    /// set the value of an XmlText element in NFC endcoded UTF8
    /*!
     * \param s an UTF8 string
     */
    static TiCC::UnicodeNormalizer norm;  // defaults to a NFC normalizer
    if ( !s.empty() ){
      UnicodeString us = TiCC::UnicodeFromUTF8(s);
      us = norm.normalize( us );
      us = dumb_spaces( us );
      _value = TiCC::UnicodeToUTF8( us );
    }
  }

  const UnicodeString XmlText::private_text( const TextPolicy& ) const {
    /// get the UnicodeString value of an XmlText element
    /*!
     */
    return TiCC::UnicodeFromUTF8(_value);
  }

  void XmlText::setAttributes( KWargs& args ){
    string txt = args.extract( "text" );
    if ( !txt.empty() ){
      setvalue( txt );
    }
    if ( !args.empty() ){
      throw ValueError( "unsupported attribute for XmlText: "
			+ args.begin()->first );
    }
  }

  xmlNode *XmlText::xml( bool, bool ) const {
    ///  convert an XmlText node to an xmlNode
    return xmlNewText( to_xmlChar(_value.c_str()) );
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
    int *cnt = static_cast<int*>(mydata);
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
	      tmp->remove( tmp->data()[0] );
	      tmp->destroy();
	      old->destroy();
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
    return xmlNewComment( to_xmlChar(_value.c_str()) );
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
    set_cls( it->second );
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

  ForeignMetaData::~ForeignMetaData(){
    /// destructor for ForeignMetaData
    for ( const auto& it : foreigners ){
      it->destroy();
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
    xmlNode *p = const_cast<xmlNode *>(node->children);
    while ( p ){
      string pref;
      string ns = getNS( p, pref );
      if ( ns == NSFOLIA ){
	throw XmlError( "ForeignData MAY NOT be in the FoLiA namespace" );
      }
      p = p->next;
    }
    _foreign_data = xmlCopyNode( const_cast<xmlNode*>(node), 1 );
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
      string val = to_char(p->href);
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
      add_child<XmlText>( it->second );
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
     * \param tp The TextPolicy to use
     * \return the Unicode String representation found. Throws when
     * no text can be found
     */
    // cerr << "TEXT MARKUP CORRECTION " << this << endl;
    // cerr << "TEXT MARKUP CORRECTION parent cls=" << parent()->cls() << endl;
    if ( tp.get_class() == "original" ) {
      return TiCC::UnicodeFromUTF8(_original);
    }
    return AbstractElement::private_text( tp );
  }

  const UnicodeString TextMarkupHSpace::private_text( const TextPolicy& tp ) const {
    /// get the UnicodeString value of a TextMarkupHSpace element
    /*!
     * \param tp the TextPolicy to use
     * \return the embedded XmlText value if ADD_FORMATTING is set
     * OR one space
     */
    UnicodeString result = " ";
    if ( tp.is_set( TEXT_FLAGS::ADD_FORMATTING ) ){
      result = AbstractElement::private_text( tp );
      if ( result.isEmpty() ){
	result = " ";
      }
    }
    return result;
  }

  const UnicodeString Hyphbreak::private_text( const TextPolicy& tp ) const {
    /// get the UnicodeString value of a Hyphbreak element
    /*!
     * \param tp the TextPolicy to use
     * \return When tp.ADD_FORMATTING is set, the embedded XmlText value
     *         or one '-', otherwise an empty string;
     */
    UnicodeString result;
    if ( tp.is_set( TEXT_FLAGS::ADD_FORMATTING ) ){
      result = AbstractElement::private_text( tp );
      if ( result.isEmpty() ){
	result = "-";
      }
    }
    return result;
  }

  const UnicodeString Row::private_text( const TextPolicy& tp ) const {
    /// get the UnicodeString value of a Row
    /*!
     * \param tp the TextPolicy to use
     * \return the Unicode String representation found. Throws when
     * no text can be found.
     */
    bool my_debug = tp.debug();
    //    my_debug = true;
    if ( my_debug ){
      cerr << "Row private text, tp=" << tp << endl;
    }
    UnicodeString result;
    for ( const auto& d : data() ){
      UnicodeString part;
      try {
	part = d->text( tp );
      }
      catch ( ... ){
      }
      if ( !part.isEmpty() ){
	if ( my_debug ){
	  cerr << "d=" << d->xmltag() << " has some text part:" << part << endl;
	}
	if ( !result.isEmpty() ){
	  result += TiCC::UnicodeFromUTF8( d->get_delimiter( tp ) );
	}
	result += part;
      }
    }
    if ( result.isEmpty() ){
      // special case, a row without any text in its children. Thus "empty"
      result = " "; // this will trigger appending of
                    // the delimitter one level above
    }
    if ( my_debug ){
      cerr << "Row private text, returns '" << result << "'" << endl;
    }
    return result;
  }


  const UnicodeString Cell::private_text( const TextPolicy& tp ) const {
    /// get the UnicodeString value of a Cell
    /*!
     * \param tp the TextPolicy to use
     * \return the Unicode String representation found. Throws when
     * no text can be found.
     */
    bool my_debug = tp.debug();
    //    my_debug = true;
    if ( my_debug ){
      cerr << "Cell private text, tp=" << tp << endl;
    }
    UnicodeString result;
    try {
      // check for direct text, then we are almost done
      const TextContent *tc = text_content( tp.get_class() );
      result = tc->text( tp );
      if ( my_debug ){
	cerr << "the Cell has it's own text part:" << result << endl;
      }
    }
    catch ( const NoSuchText& ){
      // no direct text, gather it from the children
      for ( const auto& d : data() ){
	UnicodeString part;
	try {
	  part = d->text( tp );
	}
	catch ( ... ){
	}
	if ( !part.isEmpty() ){
	  if ( my_debug ){
	    cerr << "d=" << d->xmltag() << " has some text part:" << part << endl;
	  }
	  if ( !result.isEmpty() ){
	    result += " ";
	}
	  result += part;
	}
      }
      if ( result.isEmpty() ){
	result = " "; // this will trigger appending of
	              // the delimitter one level above
      }
    }
    if ( my_debug ){
      cerr << "Cell private text, returns '" << result << "'" << endl;
    }
    return result;
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
