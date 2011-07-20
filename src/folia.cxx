#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <list>
#include <vector>
#include <map>
#include <algorithm>
#include <stdexcept>
#include "folia/folia.h"

using namespace std;

const string NSFOLIA = "http://ilk.uvt.nl/folia";
const string NSDCOI = "http://lands.let.ru.nl/projects/d-coi/ns/1.0";
const string NSIMDI = "http://www.mpi.nl/IMDI/Schema/IMDI";

std::ostream& operator<<( std::ostream& os, const AbstractElement& ae ){
  os << " <" << ae.classname();
  KWargs ats = ae.collectAttributes();
  if ( !ae._id.empty() )
    ats["id"] = ae._id;

  KWargs::const_iterator it = ats.begin();
  while ( it != ats.end() ){
    os << " " << it->first << "=" << it->second;
    ++it;
  }
  os << " > " << ae._text << " {";
  for( size_t i=0; i < ae.data.size(); ++i ){
    os << "<" << ae.data[i]->classname() << ">,";
  }
  os << "}";
  return os;
}

std::ostream& operator<<( std::ostream&os, const AbstractElement *ae ){
  if ( !ae )
    os << "nil";
  else
    os << *ae;
  return os;
}

AbstractElement::AbstractElement( Document *d ){
  mydoc = d;
  _confidence = -1;
  _element_id = BASE;
  _annotation_type = AnnotationType::NO_ANN;
  _annotator_type = UNDEFINED;
  refcount = 0;
  _parent = 0;
  _required_attributes = NO_ATT;
  _optional_attributes = NO_ATT;
  _xmltag = "ThIsIsSoWrOnG";
  occurrences = 0;  //#Number of times this element may occur in its parent (0=unlimited, default=0)
  occurrences_per_set = 1; // #Number of times this element may occur per set (0=unlimited, default=1)
  MINTEXTCORRECTIONLEVEL = UNCORRECTED;
  TEXTDELIMITER = " " ;
  PRINTABLE = true;
}

AbstractElement::~AbstractElement( ){
  //  cerr << "delete element " << _xmltag << " *= " << (void*)this << endl;
  for ( size_t i=0; i < data.size(); ++i ){
    if ( data[i]->refcount == 0 ) // probably only for words
      delete data[i];
    else {
      mydoc->keepForDeletion( data[i] );
    }
  }
}

Sentence *AbstractElement::addSentence( const KWargs& args ){
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

Word *AbstractElement::addWord( const KWargs& args ){
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

AbstractElement *Word::split( AbstractElement *part1, AbstractElement *part2,
			      const std::string& args ){
  return sentence()->splitWord( this, part1, part2, getArgs(args) );
}

Correction *Sentence::splitWord( AbstractElement *orig, AbstractElement *p1, AbstractElement *p2, const KWargs& args ){
  vector<AbstractElement*> ov;
  ov.push_back( orig );
  vector<AbstractElement*> nv;
  nv.push_back( p1 );
  nv.push_back( p2 );
  vector<AbstractElement*> nil;
  return correctWords( ov, nv, nil, args );
}

Correction *Sentence::mergewords( AbstractElement *nw, 
				  vector<AbstractElement *>& orig,
				  const string& args ){
  vector<AbstractElement*> nv;
  nv.push_back( nw );
  vector<AbstractElement*> nil;
  return correctWords( orig, nv, nil, getArgs(args) );
}

Correction *Sentence::deleteword( AbstractElement *w, 
				  const string& args ){
  vector<AbstractElement*> ov;
  ov.push_back( w );
  vector<AbstractElement*> nil;
  return correctWords( ov, nil, nil, getArgs(args) );
}

Correction *Sentence::insertword( AbstractElement *w, 
				  AbstractElement *p,
				  const string& args ){
  if ( !p || !p->isinstance( Word_t ) )
    throw runtime_error( "insertword(): previous is not a Word " );
  if ( !w || !w->isinstance( Word_t ) )
    throw runtime_error( "insertword(): new word is not a Word " );
  Word *tmp = new Word( "text='dummy', id='dummy'" );
  tmp->setParent( this ); // we create a dummy Word as member of the
                          // Sentence. This makes correct() happy
  vector<AbstractElement *>::iterator it = data.begin();
  while ( it != data.end() ){
    if ( *it == p ){
      it = data.insert( ++it, tmp );
      break;
    }
    ++it;
  }
  if ( it == data.end() )
    throw runtime_error( "insertword(): previous not found" );
  vector<AbstractElement *> ov;
  ov.push_back( *it );
  vector<AbstractElement *> nv;
  nv.push_back( w );
  vector<AbstractElement*> nil;
  return correctWords( ov, nv, nil, getArgs(args) );  
}

Correction *AbstractStructureElement::correct( vector<AbstractElement*> original,
					       vector<AbstractElement*> current,
					       vector<AbstractElement*> _new,
					       vector<AbstractElement*> suggestions,
					       const KWargs& args_in ){
  // cerr << "correct " << this << endl;
  // cerr << "original= " << original << endl;
  // cerr << "current = " << current << endl;
  // cerr << "new     = " << _new << endl;
  // cerr << "suggestions     = " << suggestions << endl;
  // cerr << "args in     = " << args_in << endl;
  // Apply a correction
  Correction *c = 0;
  bool suggestionsonly = false;
  bool hooked = false;
  AbstractElement * addnew = 0;
  KWargs args = args_in;
  KWargs::const_iterator it = args.find("new");
  if ( it != args.end() ){
    TextContent *t = new TextContent( mydoc, "text='" +  it->second + "'" );
    _new.push_back( t );
    args.erase("new");
  }
  it = args.find("suggestion");
  if ( it != args.end() ){
    TextContent *t = new TextContent( mydoc, "text='" +  it->second + "'" );
    suggestions.push_back( t );
    args.erase("suggestion");
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
    suggestionsonly = (!c->hasNew() && !c->hasOriginal() && !c->hasSuggestions() );
    if ( !_new.empty() && c->hasCurrent() ){
      // can't add new if there's current, so first set original to current, and then delete current
      
      if ( !current.empty() )
	throw runtime_error( "Can't set both new= and current= !");
      if ( original.empty() ){
	AbstractElement *cur = c->getCurrent();
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
    vector<AbstractElement *>::iterator cit = current.begin();
    while ( cit != current.end() ){
      AbstractElement *add = new Current( mydoc );
      add->append( *cit );
      c->replace( add );
      if ( !hooked ) {
	for ( size_t i=0; i < data.size(); ++i ){
	  if ( data[i] == *cit ){
	    //	    delete data[i];
	    data[i] = c;
	    hooked = true;
	  }
	}
      }
      ++cit;
    }
  }
  if ( !_new.empty() ){
    //    cerr << "there is new! " << endl;
    addnew = new NewElement( mydoc );
    c->append(addnew);
    vector<AbstractElement *>::iterator nit = _new.begin();    
    while ( nit != _new.end() ){
      addnew->append( *nit );
      ++nit;
    }
    //    cerr << "after adding " << c << endl;
    vector<AbstractElement*> v = c->select(Current_t);
    //delete current if present
    nit = v.begin();        
    while ( nit != v.end() ){
      c->remove( *nit, false );
      ++nit;
    }
  }
  if ( !original.empty() ){
    AbstractElement *add = new Original( mydoc );
    c->replace(add);
    vector<AbstractElement *>::iterator nit = original.begin();
    while ( nit != original.end() ){
      bool dummyNode = ( (*nit)->id() == "dummy" );
      if ( !dummyNode )
	add->append( *nit );
      for ( size_t i=0; i < data.size(); ++i ){
	if ( data[i] == *nit ){
	  if ( !hooked ) {
	    if ( dummyNode )
	      delete data[i];
	    data[i] = c;
	    hooked = true;
	  }
	  else 
	    remove( *nit, false );
	}
      }
      ++nit;
    }
  }
  else if ( addnew ){
    // original not specified, find automagically:
    vector<AbstractElement *> orig;
    //    cerr << "start to look for original " << endl;
    for ( size_t i=0; i < len(addnew); ++ i ){
      AbstractElement *p = addnew->index(i);
      //      cerr << "bekijk " << p << endl;
      KWargs args2;
      if ( p->isinstance( TextContent_t ) )
	args2["corrected"] = p->corrected();
      string set = p->st();
      vector<AbstractElement*> v = p->findreplacables(this);
      vector<AbstractElement*>::iterator vit=v.begin();      
      while ( vit != v.end() ){
	orig.push_back( *vit );
	++vit;
      }
    }
    if ( orig.empty() ){
      throw runtime_error( "No original= specified and unable to automatically infer");
    }
    else {
      //      cerr << "we seem to have some originals! " << endl;
      AbstractElement *add = new Original( mydoc );
      c->replace(add);
      vector<AbstractElement *>::iterator oit = orig.begin();
      while ( oit != orig.end() ){
	//	cerr << " an original is : " << *oit << endl;
	add->append( *oit );
	for ( size_t i=0; i < data.size(); ++i ){
	  if ( data[i] == *oit ){
	    if ( !hooked ) {
	      //delete data[i];
	      data[i] = c;
	      hooked = true;
	    }
	    else 
	      remove( *oit, false );
	  }
	}
	++oit;
      }
      vector<AbstractElement*> v = c->select(Current_t);
      //delete current if present
      oit = v.begin();        
      while ( oit != v.end() ){
	remove( *oit, false );
	++oit;
      }
    }
  }
  
  if ( addnew ){
    vector<AbstractElement*>::iterator oit = original.begin();
    while ( oit != original.end() ){
      c->remove( *oit, false );
      ++oit;
    }
  }

  if ( !suggestions.empty() ){
    AbstractElement *add = new Suggestion( mydoc );
    c->append(add);
    if ( !hooked )
      append(c);
    vector<AbstractElement *>::iterator nit = suggestions.begin();
    while ( nit != suggestions.end() ){
      add->append( *nit );
      ++nit;
    }
  }
  
  it = args.find("reuse");
  if ( it != args.end() ){
    if ( addnew  ){
      vector<AbstractElement *> sv = c->suggestions();
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
      c->annotatortype( stringToANT(it->second) );
    it = args.find("confidence");
    if ( it != args.end() )
      c->confidence( stringTo<double>(it->second) );

  }

  return c;
}

Correction *Sentence::correctWords( vector<AbstractElement *>& orig,
				    vector<AbstractElement *>& _new,
				    vector<AbstractElement *>& current, 
				    const KWargs& args ){
  // Generic correction method for words. You most likely want to use the helper functions
  //      splitword() , mergewords(), deleteword(), insertword() instead
  
  // sanity check:
  vector<AbstractElement *>::const_iterator it = orig.begin();
  while ( it != orig.end() ){
    if ( !(*it) || !(*it)->isinstance( Word_t) )
      throw runtime_error("Original word is not a Word instance" );
    else if ( (*it)->sentence() != this )
      throw runtime_error( "Original not found as member of sentence!");
    ++it;
  }
  it = _new.begin();
  while ( it != _new.end() ){
    if ( ! (*it)->isinstance( Word_t) )
      throw runtime_error("new word is not a Word instance" );
    ++it;
  }
  it = current.begin();
  while ( it != current.end() ){
    if ( ! (*it)->isinstance( Word_t) )
      throw runtime_error("current word is not a Word instance" );
    ++it;
  }
  KWargs::const_iterator ait = args.find("suggest");
  if ( ait != args.end() && ait->second == "true" ){
    vector<AbstractElement *> nil;
    return correct( nil, orig, nil, _new, args );
  }
  else {
    vector<AbstractElement *> nil;
    return correct( orig, nil, _new, nil, args );
  }
}

Alternative *AbstractElement::addAlternative( ElementType et,
					      const KWargs& args ){
  Alternative *res = new Alternative( mydoc );
  KWargs kw;
  string id = generateId( "w" );
  kw["id"] = id;
  res->setAttributes( kw );
  if ( et == Pos_t )
    res->addPosAnnotation( args );
  else if ( et == Lemma_t )
    res->addLemmaAnnotation( args );
  else
    throw runtime_error( "addAlternative not implemenentd for " + toString(et) );
  append( res );
  return res;
}

AbstractTokenAnnotation *AbstractElement::addAnnotation( ElementType et,
							 const KWargs& args ){
  if ( et == Pos_t )
    return addPosAnnotation( args );
  else if ( et == Lemma_t )
    return addLemmaAnnotation( args );
  else
    throw runtime_error( "addAnnotation not implemenentd for " + toString(et) );
}

AbstractTokenAnnotation *AbstractElement::addPosAnnotation( const KWargs& args ){
  PosAnnotation *res = new PosAnnotation( mydoc );
  res->setAttributes( args );
  append( res );
  return res;
}

AbstractTokenAnnotation *AbstractElement::addLemmaAnnotation( const KWargs& args ){
  LemmaAnnotation *res = new LemmaAnnotation( mydoc );
  res->setAttributes( args );
  append( res );
  return res;
}

AbstractElement *Word::append( AbstractElement *child ) {
  if ( child->element_id() == Pos_t ||
       child->element_id() == Lemma_t ){
    // sanity check, there may be no other child within the same set
    try {
      annotation( child->element_id(), child->st() );
    }
    catch ( NoSuchAnnotation &e ){
      // OK!
      return AbstractElement::append( child );
    }
    delete child;
    throw DuplicateAnnotationError( "Word::append" );
  }
  if ( child->element_id() == TextContent_t ){
    // sanity check, there may be no other TextContent child with the same 
    // correction level
    vector<AbstractElement *> v = select( TextContent_t, false );
    if ( v.size() > 0 ){
      vector<AbstractElement*>::iterator it = v.begin();
      while ( it != v.end() ){
	if ( (*it)->corrected() > child->corrected() ){
	  delete child;
	  throw ValueError( "'corrected' value must be < " + 
			    toString((*it)->corrected()) );
	}
	else if ( (*it)->corrected() == child->corrected() ){
	  delete child;
	  throw DuplicateAnnotationError( "A TextContent with 'corrected' value of " +  toString((*it)->corrected()) + " already exists." );
	}
	++it;
      }
      // no conflict found
    }
  }
  return AbstractElement::append( child );
}

string AbstractElement::generateId( const string&, const string& ){
  throw runtime_error( "generateId not allowed for " + _element_id );
}

string AbstractStructureElement::generateId( const string& tag, 
					     const string& id_in  ){
  //  cerr << "generateId," << _xmltag << " maxids=" << maxid << endl;
  int max = getMaxId(tag);
  //  cerr << "MAX = " << max << endl;
  string id = id_in;
  if ( id.empty() )
    id = _id;
  id += '.' + tag + '.' +  toString( max + 1 );
  return id;
}

void AbstractElement::setAttributes( const KWargs& kwargs ){
  Attrib supported = _required_attributes | _optional_attributes;
  // if ( _element_id == Pos_t ){
  //   cerr << "set attributes: " << kwargs << " on " << toString(_element_id) << endl;
  //   cerr << "required = " <<  _required_attributes << endl;
  //   cerr << "optional = " <<  _optional_attributes << endl;
  //   cerr << "supported = " << supported << endl;
  //   cerr << "ID & supported = " << (ID & supported) << endl;
  //   cerr << "ID & _required = " << (ID & _required_attributes ) << endl;
  // }
  if ( mydoc && mydoc->debug > 2 )
    cerr << "set attributes: " << kwargs << " on " << toString(_element_id) << endl;
  
  map<string,string>::const_iterator it = kwargs.find( "generate_id" );
  if ( it != kwargs.end() ) {
    if ( !mydoc ){
      throw runtime_error( "can't generate an ID without a doc" );
    }
    AbstractElement * e = (*mydoc)[it->second];
    if ( e ){
      _id = e->generateId( _xmltag );
    }
    else
      throw ValueError("Unable to genarate and if from ID= " + it->second );
  }
  else {
    it = kwargs.find( "id" );
    if ( it != kwargs.end() ) {
      if ( !ID & supported )
	throw ValueError("ID is not supported");
      else {
	_id = it->second;
      }
    }
    else if ( ID & _required_attributes )
      throw ValueError("ID is required for " + classname() );
    else
      _id = "";
  }

  it = kwargs.find( "set" );
  string def;
  if ( it != kwargs.end() ) {
    if ( !(CLASS & supported) )
      throw ValueError("Set is not supported");
    else {
      _set = it->second;
    }
    if ( mydoc &&
	 mydoc->defaultset( _annotation_type, true ) == "" )
      throw ValueError( "Set " + _set + " is used but has no declaration!" );
  }
  else if ( mydoc && ( def = mydoc->defaultset( _annotation_type, true )) != "" ){
    _set = def;
  }
  else if ( CLASS & _required_attributes )
    throw ValueError("Set is required for " + classname() );
  else
    _set = "";

  it = kwargs.find( "class" );
  if ( it == kwargs.end() )
    it = kwargs.find( "cls" );
  if ( it != kwargs.end() ) {
    if ( !( CLASS & supported ) )
      throw ValueError("Class is not supported for " + classname() );
    else {
      _cls = it->second;
    }
  }
  else if ( CLASS & _required_attributes )
    throw ValueError("Class is required for " + classname() );
  else
    _cls = "";

      
  it = kwargs.find( "annotator" );    
  if ( it != kwargs.end() ) {
    if ( !(ANNOTATOR & supported) )
      throw ValueError("Annotator is not supported for " + classname() );
    else {
      _annotator = it->second;
    }
  }
  else if ( mydoc &&
	    (def = mydoc->defaultannotator( _annotation_type, "", true )) != "" ){
    _annotator = def;
  }
  else if ( ANNOTATOR & _required_attributes )
    throw ValueError("Annotator is required for " + classname() );
  else
    _annotator = "";
  
  it = kwargs.find( "annotatortype" );    
  if ( it != kwargs.end() ) {
    if ( ! (ANNOTATOR & supported) )
      throw ValueError("Annotatortype is not supported for " + classname() );
    else {
      if ( it->second == "auto" )
	_annotator_type = AUTO;
      else if ( it->second == "manual" )
	_annotator_type = MANUAL;
      else
	throw ValueError("annotatortype must be 'auto' or 'manual'");
    }
  }
  else if ( mydoc &&
	    (def = mydoc->defaultannotatortype( _annotation_type, "", true ) ) != ""  ){
    if ( def == "auto" )
      _annotator_type = AUTO;
    else if ( def == "manual" )
      _annotator_type = MANUAL;
    else
      throw ValueError("annotatortype must be 'auto' or 'manual'");
  }
  else if ( ANNOTATOR & _required_attributes )
    throw ValueError("Annotatortype is required for " + classname() );
  else
    _annotator_type = UNDEFINED;

        
  it = kwargs.find( "confidence" );
  if ( it != kwargs.end() ) {
    if ( !(CONFIDENCE & supported) )
      throw ValueError("Confidence is not supported for " + classname() );
    else {
      _confidence = toDouble(it->second);
      if ( ( _confidence < 0.0 || _confidence > 1.0 ) )
	throw ValueError("Confidence must be a floating point number between 0 and 1");
    }
  }
  else if ( CONFIDENCE & _required_attributes )
    throw ValueError("Confidence is required for " + classname() );
  else
    _confidence = -1;

  it = kwargs.find( "n" );
  if ( it != kwargs.end() ) {
    if ( !(N & supported) )
      throw ValueError("N is not supported for " + classname() );
    else {
      _n = it->second;
    }
  }
  else if ( N & _required_attributes )
    throw ValueError("N is required");
  else
    _n = "";
  
  if ( mydoc && !_id.empty() )
    mydoc->addDocIndex( this, _id );  
}

void Word::setAttributes( const KWargs& kwargs ){
  map<string,string>::const_iterator it = kwargs.find( "text" );
  if ( it != kwargs.end() ) {
    addText( it->second, MINTEXTCORRECTIONLEVEL );
  }
  AbstractElement::setAttributes(kwargs);
}

void Feature::setAttributes( const KWargs& kwargs ){
  map<string,string>::const_iterator it;
  if ( _subset.empty() ){
    it = kwargs.find( "subset" );
    if ( it == kwargs.end() ) {
      throw ValueError("subset attribute is required for " + classname() );
    }
    _subset = it->second;
  }
  it = kwargs.find( "cls" );
  if ( it == kwargs.end() )
    it = kwargs.find( "class" );
  if ( it == kwargs.end() ) {
    throw ValueError("class attribute is required for " + classname() );
  }
  _cls = it->second;
}

void TextContent::setAttributes( const KWargs& kwargs ){
  map<string,string>::const_iterator it = kwargs.find( "text" );
  if ( it != kwargs.end() ) {
    _text = UTF8ToUnicode(it->second);
  }
  it = kwargs.find( "corrected" );
  if ( it != kwargs.end() ) {
    _corrected = stringToTCL(it->second);
  }
  it = kwargs.find( "offset" );
  if ( it != kwargs.end() ) {
    offset = stringTo<int>(it->second);
  }
  AbstractElement::setAttributes(kwargs);
}

bool AbstractElement::contains( const AbstractElement *child ) const {
  vector<AbstractElement*>::const_iterator it = data.begin();
  while ( it != data.end() ){
    if ( *it == child )
      return true;
    ++it;
  }
  return false;
}

vector<AbstractElement *>AbstractElement::findreplacables( AbstractElement *par ){
  set<ElementType> st;
  return par->select( element_id(), st, false );
}

void AbstractElement::replace( AbstractElement *child ){
  // Appends a child element like append(), but replaces any existing child 
  // element of the same type and set. 
  // If no such child element exists, this will act the same as append()
  
  vector<AbstractElement*> replace = child->findreplacables( this );
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

            
vector<AbstractElement *>TextContent::findreplacables( AbstractElement *par ){
  set<ElementType> st;
  vector<AbstractElement*> v = par->select( TextContent_t, st, false );
  // cerr << "TextContent::findreplacable found " << v << endl;
  // cerr << "looking for " << toString( _corrected) << endl;
  vector<AbstractElement *>::iterator it = v.begin();
  while ( it != v.end() ){
    // cerr << "TextContent::findreplacable bekijkt " << *it << " (" 
    // 	 << toString( dynamic_cast<TextContent*>(*it)->_corrected ) << ")" << endl;
    if ( dynamic_cast<TextContent*>(*it)->_corrected != _corrected )
      it = v.erase(it);
    else
      ++it;
  }
  //  cerr << "TextContent::findreplacable resultaat " << v << endl;
  return v;
}


string AbstractElement::str() const {
  return _xmltag;
}

bool AbstractElement::hastext( TextCorrectionLevel corr ) const {
  //  cerr << _xmltag << "::hastext()" << endl;
  // Does this element have text?
  vector<AbstractElement*> v = const_cast<AbstractElement *>(this)->select(TextContent_t,false);
  if ( corr == NOCORR ){
    // regardless of correctionlevel:
    return v.size() > 0;
  }
  else {
    vector<AbstractElement*>::const_iterator it = v.begin();
    while ( it != v.end() ){
      if ( (*it)->corrected() == corr )
	return true;
      ++it;
    }
  }
  return false;
}

UnicodeString AbstractElement::text( TextCorrectionLevel corr ) const {
  if ( !PRINTABLE )
    throw NoSuchText( _xmltag );
  //  cerr << "text() for " << _xmltag << " step 1 " << endl;
  if ( corr != NOCORR ){
    if ( MINTEXTCORRECTIONLEVEL > corr ){
      throw  NotImplementedError( "text() for " + _xmltag );
    }
    AbstractElement *t = 0;
    for( size_t i=0; i < data.size(); ++i ){
      if ( data[i]->element_id() == TextContent_t 
	   && data[i]->corrected() == corr ){
	t = data[i];
	break;
      }
    }
    //    cerr << "text() for " << _xmltag << " step 2 >> " << endl;
    if ( t ){
      return t->text();
    }
    else
      throw NoSuchText( "inside " + _xmltag );
  }
  else if ( hastext( CORRECTED )  ){
    //    cerr << "text() for " << _xmltag << " step 3 " << endl;
    return text( CORRECTED );
  }
  else {
    // try to get text from children.
    //    cerr << "text() for " << _xmltag << " step 4 " << endl;

    UnicodeString result;
    for( size_t i=0; i < data.size(); ++i ){
      try {
	UnicodeString tmp = data[i]->text();
	result += tmp;
	if ( !tmp.isEmpty() )
	  result += UTF8ToUnicode(TEXTDELIMITER);
      }
      catch ( NoSuchText& e ){
	//	cerr << "hmm: " << e.what() << endl;
      }
    }
    //    cerr << "text() for " << _xmltag << " step 5, result= " << result << endl;
    result.trim();
    if ( !result.isEmpty() ){
      //      cerr << "text() for " << _xmltag << " step 6, result= " << result << endl;
      return result;
    }
    else if ( MINTEXTCORRECTIONLEVEL <= UNCORRECTED ){
      //      cerr << "text() for " << _xmltag << " step 7"<< endl;
      return text( UNCORRECTED );
    }
    else
      throw NoSuchText( ":{" );
  }
}

string TextContent::str() const{
  return UnicodeToUTF8(_text);
}

UnicodeString TextContent::text( TextCorrectionLevel l ) const{
  //  cerr << "TextContent::text()  step 1 " << endl;
  if ( l == NOCORR || l == _corrected )
    return _text;
  else
    throw NoSuchText( _xmltag );
}

UnicodeString Correction::text( TextCorrectionLevel corr ) const {
  UnicodeString result;
  for( size_t i=0; i < data.size(); ++i ){
    //    cerr << "data[" << i << "]=" << data[i] << endl;
    if ( data[i]->isinstance( New_t ) || data[i]->isinstance( Current_t ) )
      result += data[i]->text( corr );
  }
  return result;
}

TextContent *AbstractElement::addText( const string& txt, 
				       TextCorrectionLevel lv ){
  TextCorrectionLevel myl = lv;
  if ( lv == NOCORR )
    myl = MINTEXTCORRECTIONLEVEL;
  KWargs args;
  args["text"] = txt;
  args["corrected"] = toString( lv );
  TextContent *node = new TextContent( mydoc );
  node->setAttributes( args );
  append( node );
  return node;
}

Sentence *Word::sentence( ) const {
  // return the sentence this word is a part of, otherwise return null
  AbstractElement *p = _parent; 
  while( p ){
    if ( p->isinstance( Sentence_t ) )
      return dynamic_cast<Sentence*>(p);
    p = p->parent();
  }
  return 0;
}

AbstractElement *Word::previous() const{
  Sentence *s = sentence();
  vector<AbstractElement*> words = s->words();
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

AbstractElement *Word::next() const{
  Sentence *s = sentence();
  vector<AbstractElement*> words = s->words();
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

vector<AbstractElement *> Word::context( size_t size, 
					 const string& val ) const {
  vector<AbstractElement *> result;
  if ( size > 0 ){
    vector<AbstractElement*> words = mydoc->words();
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
	    PlaceHolder *p = new PlaceHolder( val );
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
	      PlaceHolder *p = new PlaceHolder( val );
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


vector<AbstractElement *> Word::leftcontext( size_t size, 
					     const string& val ) const {
  //  cerr << "leftcontext : " << size << endl;
  vector<AbstractElement *> result;
  if ( size > 0 ){
    vector<AbstractElement*> words = mydoc->words();
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
	    PlaceHolder *p = new PlaceHolder( val );
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

vector<AbstractElement *> Word::rightcontext( size_t size, 
					      const string& val ) const {
  vector<AbstractElement *> result;
  //  cerr << "rightcontext : " << size << endl;
  if ( size > 0 ){
    vector<AbstractElement*> words = mydoc->words();
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
	      PlaceHolder *p = new PlaceHolder( val );
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

string AbstractElement::description() const {
  vector<AbstractElement *> v =  const_cast<AbstractElement *>(this)->select( Description_t, false );
  if ( v.size() == 0 )
    throw NoDescription();
  else
    return v[0]->description();
}

string AbstractStructureElement::str() const{
  UnicodeString result = text();
  return UnicodeToUTF8(result);
}
  
void AbstractStructureElement::setMaxId( AbstractElement *child ) {
  if ( !child->id().empty() && !child->xmltag().empty() ){
    vector<string> parts;
    size_t num = split_at( child->id(), parts, "." );
    if ( num > 0 ){
      string val = parts[num-1];
      int i = stringTo<int>( val );
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

int AbstractStructureElement::getMaxId( const string& xmltag ) {
  int res = 0;
  if ( !xmltag.empty() ){
    res = maxid[xmltag];
    ++maxid[xmltag];
  }
  return res;
}

AbstractElement *AbstractStructureElement::append( AbstractElement *child ){
  AbstractElement::append( child );
  setMaxId( child );
  return child;
}

bool AbstractElement::acceptable( ElementType t ) const {
  set<ElementType>::const_iterator it = _accepted_data.find( t );
  if ( it == _accepted_data.end() )
    return false;
  return true;
}
 
bool AbstractElement::addable( const AbstractElement *c,
			       const string& setname ) const {
  static set<ElementType> selectSet;
  if ( selectSet.empty() ){
    selectSet.insert( Original_t );
    selectSet.insert( Suggestion_t );
    selectSet.insert( Alternative_t );
  }
  if ( !acceptable( c->_element_id ) ){
    throw ValueError( "type " + c->classname() + " is inaccaptable for " + classname() );
    return false;
  }
  if ( c->occurrences > 0 ){
    vector<AbstractElement*> v = const_cast<AbstractElement*>(this)->select( c->_element_id, selectSet );
    size_t count = v.size();
    if ( count > c->occurrences )
      throw DuplicateAnnotationError( "Unable to add another object of type " + c->classname() + " to " + classname() + ". There are already " + toString(count) + " instances of this class, which is the maximum." );
  }
  if ( c->occurrences_per_set > 0 && !setname.empty() &&
       ( CLASS & c->_required_attributes ) ){
    vector<AbstractElement*> v = const_cast<AbstractElement*>(this)->select( c->_element_id, setname, selectSet );
    size_t count = v.size();
    if ( count > c->occurrences_per_set )
      throw DuplicateAnnotationError( "Unable to add another object of type " + c->classname() + " to " + classname() + ". There are already " + toString(count) + " instances of this class, which is the maximum." );
  }
  return true;
}
 
AbstractElement *AbstractElement::append( AbstractElement *child ){
  if ( addable( child ) ){
    data.push_back(child);
    if ( !child->mydoc ){
      child->mydoc = mydoc;
      string id = child->id();
      if ( !id.empty() )
	mydoc->addDocIndex( child, id );  
    }
    if ( !child->_parent ) // Only for WordRef i hope
      child->_parent = this;
  }
  else {
    throw ValueError( "Unable to append object of type " + child->classname()
		      + " to a " + classname() );
  }
  return child;
}


void AbstractElement::remove( AbstractElement *child, bool del ){
  vector<AbstractElement*>::iterator it = std::remove( data.begin(), data.end(), child );
  data.erase( it, data.end() );
  if ( del )
    delete child;
}

void setAtt( xmlNode *node, const KWargs& attribs ){
  map<string,string>::const_iterator it = attribs.begin();
  while ( it != attribs.end() ){
    xmlNewNsProp( node, 0, 
		  (const xmlChar*)it->first.c_str(), 
		  (const xmlChar*)it->second.c_str() );
    ++it;
  }
}

xmlNode *newXMLNode( xmlNs *ns,  const string& elem ){
  return xmlNewNode( ns, (const xmlChar*)elem.c_str() );
}

xmlNode *newTextNode( xmlDoc *doc, xmlNs *ns,  
		      const string& elem, const string& val ){
  return xmlNewDocNode( doc, ns,
			(const xmlChar*)elem.c_str(), 
			(const xmlChar*)val.c_str() );
}

KWargs AbstractElement::collectAttributes() const {
  KWargs attribs;
  bool isDefaultSet = true;
  if ( !_set.empty() &&
       _set != mydoc->defaultset( _annotation_type ) ){
    isDefaultSet = false;
    attribs["set"] = _set;
  }
  if ( !_cls.empty() )
    attribs["class"] = _cls;
  
  if ( !_annotator.empty() &&
       _annotator != mydoc->defaultannotator( _annotation_type, "", true ) ){
    attribs["annotator"] = _annotator;
  }
  
  if ( _annotator_type != UNDEFINED ){
    if ( mydoc->isDefaultAnn( _annotation_type ) ){
      if ( _annotator_type == AUTO )
	attribs["annotatortype"] = "auto";
      else if ( _annotator_type == MANUAL )
	attribs["annotatortype"] = "manual";
    }
  }
  
  if ( _confidence >= 0 )
    attribs["confidence"] = toString(_confidence);
  
  if ( !_n.empty() )
    attribs["n"] = _n;

  return attribs;
}

string AbstractElement::xmlstring() const{
  // serialize to a string (XML fragment)
  if ( !mydoc ){
    throw runtime_error( "xmlstring() on a node without a doc" );
  }
  xmlNode *n = xml( mydoc, true );
  xmlDoc *doc = xmlNewDoc( (const xmlChar*)"1.0" );
  xmlNode *e = xmlNewDocNode( doc, 0, (const xmlChar*)"root", 0 );
  xmlDocSetRootElement( doc, e );
  xmlSetNs( n, xmlNewNs( n, (const xmlChar *)NSFOLIA.c_str(), 0 ) );
  xmlAddChild( e, n );
  xmlChar *buf; 
  int size;
  xmlDocDumpFormatMemory( doc, &buf, &size, 0 );
  string result = string( (const char *)buf, size );
  xmlFree( buf );
  xmlFreeDoc( doc );
  string::size_type s_pos = result.find( "<root>" );
  string::size_type e_pos = result.find( "</root>" );
  result = result.substr( s_pos+6, e_pos-s_pos-6 );
  return result;
}

xmlNode *AbstractElement::xml( Document *doc, bool recursive ) const {
  xmlNode *e = newXMLNode( doc->foliaNs(), _xmltag );
  if ( !_id.empty() ){
    xmlNewNsProp( e, 0, XML_XML_ID,  (const xmlChar *)_id.c_str() );
  }
  KWargs attribs = collectAttributes();
  setAtt( e, attribs );
  if ( recursive ){
    // append children:
    vector<AbstractElement*>::const_iterator it=data.begin();
    while ( it != data.end() ){
      xmlAddChild( e, (*it)->xml( doc, recursive ) );
      ++it;
    }
  }
  return e;
}

xmlNode *TextContent::xml( Document *doc, bool ) const {
  xmlNode *e = newXMLNode( doc->foliaNs(), _xmltag );
  xmlAddChild( e, xmlNewText( (const xmlChar*)str().c_str()) );
  if ( !_id.empty() ){
    xmlNewNsProp( e, 0, XML_XML_ID,  (const xmlChar *)_id.c_str() );
  }
  KWargs attribs = collectAttributes();
  if ( offset >= 0 )
    attribs["offset"] = toString( offset );
  setAtt( e, attribs );
  return e;
}

xmlNode *Description::xml( Document *doc, bool ) const {
  xmlNode *e = newXMLNode( doc->foliaNs(), _xmltag );
  xmlAddChild( e, xmlNewText( (const xmlChar*)value.c_str()) );
  KWargs attribs = collectAttributes();
  setAtt( e, attribs );
  return e;
}

xmlNode *Content::xml( Document *doc, bool ) const {
  xmlNode *e = newXMLNode( doc->foliaNs(), _xmltag );
  xmlAddChild( e, xmlNewCDataBlock( doc->XmlDoc(), (const xmlChar*)value.c_str() , value.length() ) );
  KWargs attribs = collectAttributes();
  setAtt( e, attribs );
  return e;
}

xmlNode *SyntacticUnit::xml( Document *doc, bool recursive ) const {
  xmlNode *e = AbstractElement::xml( doc, false );
  // append Word children:
  vector<AbstractElement*>::const_iterator it=data.begin();
  while ( it != data.end() ){
    if ( (*it)->element_id() == Word_t ){
      xmlNode *t = newXMLNode( doc->foliaNs(), "wref" );
      KWargs attribs;
      attribs["id"] = (*it)->id();
      string txt = (*it)->str();
      if ( !txt.empty() )
	attribs["t"] = txt;
      setAtt( t, attribs );
      xmlAddChild( e, t );
    }
    else
      xmlAddChild( e, (*it)->xml( doc, recursive ) );
    ++it;
  }
  return e;
}

AbstractElement *SyntacticUnit::append( AbstractElement *child ){
  if ( child->isinstance(Word_t) and acceptable( WordReference_t ) )
    child->increfcount();
  AbstractElement::append( child );
  return child;
}

xmlDoc *readXmlDoc( const string& s ){
  xmlParserCtxt *ctxt = xmlNewParserCtxt();
  xmlInitParserCtxt( ctxt );
  xmlDoc *doc = xmlCtxtReadFile( ctxt, s.c_str(), 0, 0 );
  xmlFreeParserCtxt( ctxt );
  return doc;
}


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
  map<string,string>::const_iterator it = kwargs.find( "debug" );
  if ( it != kwargs.end() )
    debug = stringTo<int>( it->second );
  else
    debug = 0;
  it = kwargs.find( "load" );
  if ( it != kwargs.end() )
    loadall = stringTo<bool>( it->second );
  else
    loadall = true;
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
  xmldoc = readXmlDoc( s );
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
  char buffer[L_tmpnam];
  tmpnam( buffer );
  ofstream os( buffer );
  os << s << endl;
  return readFromFile( buffer );
}

bool Document::save( const string& fn ) {
  string out = toXml();
  ofstream os( fn.c_str() );
  if ( os.good() ) {
    os << out << endl;
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
};

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
  return foliadoc->select( Sentence_t );
}

AbstractElement* Document::sentences( size_t index ) const {
  vector<AbstractElement*> v = sentences();
  if ( index < v.size() ){
    return v[index];
  }
  else
    throw range_error( "word index out of range" );
}


vector<AbstractElement*> Document::words() const {
  set<ElementType> st;
  st.insert( SyntacticUnit_t );
  return foliadoc->select( Word_t, st );
}

AbstractElement* Document::words( size_t index ) const {
  vector<AbstractElement*> v = words();
  if ( index < v.size() ){
    return v[index];
  }
  else
    throw range_error( "word index out of range" );
}

AbstractElement* Document::rwords( size_t index ) const {
  vector<AbstractElement*> v = words();
  if ( index < v.size() ){
    return v[v.size()-1-index];
  }
  else
    throw range_error( "word reverse index out of range" );
}

AbstractElement* Document::rparagraphs( size_t index ) const {
  vector<AbstractElement*> v = paragraphs();
  if ( index < v.size() ){
    return v[v.size()-1-index];
  }
  else
    throw range_error( "paragraph reverse index out of range" );
}

AbstractElement *Document::append( AbstractElement *txt ){
  if ( !isinstance( txt, Text_t ) )
    throw runtime_error( "can only append() a Text_t element to a Document" );
  else {
    return foliadoc->append( txt );
  }
}

AbstractElement* AbstractElement::index( size_t i ) const {
  if ( i < data.size() )
    return data[i];
  else
    throw range_error( "[] index out of range" );
}

AbstractElement* AbstractElement::rindex( size_t i ) const {
  if ( i < data.size() )
    return data[data.size()-1-i];
  else
    throw range_error( "[] rindex out of range" );
}

vector<AbstractElement*> AbstractElement::words() const {
  static set<ElementType> selectSet;
  if ( selectSet.empty() ){
    selectSet.insert( Original_t );
    selectSet.insert( Suggestion_t );
    selectSet.insert( Alternative_t );
  }
  return const_cast<AbstractElement*>(this)->select( Word_t, selectSet );
}

AbstractElement* AbstractElement::words( size_t index ) const {
  vector<AbstractElement*> v = words();
  if ( index < v.size() ){
    return v[index];
  }
  else
    throw range_error( "word index out of range" );
}

AbstractElement* AbstractElement::rwords( size_t index ) const {
  vector<AbstractElement*> v = words();
  if ( index < v.size() ){
    return v[v.size()-1-index];
  }
  else
    throw range_error( "word reverse index out of range" );
}


vector<AbstractElement*> AbstractElement::select( ElementType et,
						  const string& val,
						  bool recurse ) {
  vector<AbstractElement*> res;
  for ( size_t i = 0; i < data.size(); ++i ){
    if ( data[i]->_element_id == et && data[i]->_set == val ){
      res.push_back( data[i] );
    }
    if ( recurse ){
      vector<AbstractElement*> tmp = data[i]->select( et, val, recurse );
      res.insert( res.end(), tmp.begin(), tmp.end() );
    }
  }
  return res;
}

vector<AbstractElement*> AbstractElement::select( ElementType et,
						  set<ElementType>& exclude,
						  bool recurse ) {
  vector<AbstractElement*> res;
  for ( size_t i = 0; i < data.size(); ++i ){
    if ( data[i]->_element_id == et ){
      res.push_back( data[i] );
    }
    if ( recurse ){
      if ( exclude.find( data[i]->_element_id ) == exclude.end() ){
	vector<AbstractElement*> tmp = data[i]->select( et, exclude, recurse );
	res.insert( res.end(), tmp.begin(), tmp.end() );
      }
    }
  }
  return res;
}

vector<AbstractElement*> AbstractElement::select( ElementType et,
						  const string& val,
						  set<ElementType>& exclude,
						  bool recurse ) {
  vector<AbstractElement*> res;
  for ( size_t i = 0; i < data.size(); ++i ){
    if ( data[i]->_element_id == et  && data[i]->_set == val ){
      res.push_back( data[i] );
    }
    if ( recurse ){
      if ( exclude.find( data[i]->_element_id ) == exclude.end() ){
	vector<AbstractElement*> tmp = data[i]->select( et, val, exclude, recurse );
	res.insert( res.end(), tmp.begin(), tmp.end() );
      }
    }
  }
  return res;
}

vector<AbstractElement*> AbstractElement::select( ElementType et,
						  bool recurse ) {
  vector<AbstractElement*> res;
  for ( size_t i = 0; i < data.size(); ++i ){
    if ( data[i]->_element_id == et ){
      res.push_back( data[i] );
    }
    if ( recurse ){
      vector<AbstractElement*> tmp = data[i]->select( et, recurse );
      res.insert( res.end(), tmp.begin(), tmp.end() );
    }
  }
  return res;
}

map<string,string> getAtt( xmlNode *node ){
  map<string,string> atts;
  if ( node ){
    xmlAttr *a = node->properties;
    while ( a ){
      atts[(char*)a->name] = (char *)a->children->content;
      a = a->next;
    }
  }
  return atts;
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
      map<string,string> att = getAtt( n );
      map<string,string>::const_iterator it = att.find("set" );
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
      //      cerr << "inserted [" << s << "](" << a << "," << t << ")" << endl;
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
  map<string,string> att = getAtt( root );
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

AbstractElement* Document::parseXml( ){
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

UnicodeString extractText( xmlNode *node ){
  string result;
  if ( node && node->children ){
    result = XmlContent( node );
  }
  return UTF8ToUnicode( result );
}

AbstractElement* TextContent::parseXml( xmlNode *node ){
  map<string,string> att = getAtt( node );
  setAttributes( att );
  _text = extractText( node );
  if ( mydoc->debug > 2 )
    cerr << "set textcontent to " << _text << endl;
  return this;
}

AbstractElement* Description::parseXml( xmlNode *node ){
  map<string,string> att = getAtt( node );
  setAttributes( att );
  value = XmlContent( node );
  return this;
}

AbstractElement* Content::parseXml( xmlNode *node ){
  map<string,string> att = getAtt( node );
  setAttributes( att );
  xmlNode *p = node->children;
  while ( p ){
    if ( p->type == XML_CDATA_SECTION_NODE ){
      value += (char*)p->content;
    }
    p = p->next;
  }
  if ( value.empty() )
    throw XmlError( "CDATA expected in Content node" );
  return this;
}


AbstractElement* AbstractElement::parseXml( xmlNode *node ){
  map<string,string> att = getAtt( node );
  setAttributes( att );
  xmlNode *p = node->children;
  while ( p ){
    if ( p->type == XML_ELEMENT_NODE ){
      string tag = Name( p );
      AbstractElement *t = createElement( mydoc, tag );
      if ( t ){
	if ( mydoc->debug > 2 )
	  cerr << "created " << t << endl;
	t = t->parseXml( p );
	if ( t ){
	  if ( mydoc->debug > 2 )
	    cerr << "extend " << this << " met " << tag << endl;
	  append( t );
	}
      }
    }
    p = p->next;
  }
  return this;
}

AbstractElement* WordReference::parseXml( xmlNode *node ){
  map<string,string> att = getAtt( node );
  string id = att["id"];
  if ( id.empty() )
    throw XmlError( "empty id in WordReference" );
  if ( mydoc->debug ) 
    cerr << "Found word reference" << id << endl;
  AbstractElement *res = (*mydoc)[id];
  if ( res ){
    res->increfcount();
  }
  else {
    if ( mydoc->debug )
      cerr << "...Unresolvable!" << endl;
    res = this;
  }
  delete this;
  return res;
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

std::string Document::defaultset( AnnotationType::AnnotationType type,
				  bool noThrow ) const {
  map<AnnotationType::AnnotationType,map<string,at_t> >::const_iterator mit1 = annotationdefaults.find(type);
  string result;
  if ( mit1 != annotationdefaults.end() ){
    result = mit1->second.begin()->first;
  }
  //  cerr << "get defaultset ==> " << result << endl;
  if ( !noThrow && result.empty() )
    throw NoDefaultError( "defaultset" );
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
    throw NoDefaultError( "defaultannotator" );
  return result;
}

bool Document::isDefaultAnn( AnnotationType::AnnotationType ann ) const {
  map<AnnotationType::AnnotationType,map<string,at_t> >::const_iterator mit1 = annotationdefaults.find(ann);
  if ( mit1 != annotationdefaults.end() ){
    return false;
  }
  return true;
}

void Document::setannotations( xmlNode *node ){
  list<ts_t>::const_iterator it = annotations.begin();
  while ( it != annotations.end() ){
    // Find the 'label' 
    string label = lowercase(toString( it->t ) );
    label += "-annotation";
    xmlNode *n = xmlAddChild( node, newXMLNode( _foliaNs, label ) );
    KWargs args;
    string s = defaultannotator( it->t, "", true );
    if ( !s.empty() )
      args["annotator"] = s;
    s = defaultannotatortype( it->t, "", true );
    if ( !s.empty() )
      args["annotatortype"] = s;
    s = defaultset( it->t, true );
    if ( !s.empty() )
      args["set"] = s;
    setAtt( n, args );
    ++it;
  } 
}

void Document::setmetadata( xmlNode *node ){
  map<string,string> atts;
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
  setAtt( node, atts );
}

string Document::toXml() {
  string result;
  if ( foliadoc ){
    xmlDoc *outDoc = xmlNewDoc( (const xmlChar*)"1.0" );
    xmlNode *e = xmlNewDocNode( outDoc, 0, (const xmlChar*)"FoLiA", 0 );
    xmlDocSetRootElement( outDoc, e );
    xmlNs *xl = xmlNewNs( e, (const xmlChar *)"http://www.w3.org/1999/xlink", 
			  (const xmlChar *)"xlink" );
    xmlSetNs( e, xl );
    if ( !_foliaNs ){
      _foliaNs = xmlNewNs( e, (const xmlChar *)NSFOLIA.c_str(), 0 );
    }
    xmlSetNs( e, _foliaNs );
    string id = foliadoc->id();
    if ( !id.empty() ){
      xmlNewNsProp( e, 0, XML_XML_ID,  (const xmlChar *)id.c_str() );
    }
    xmlNode *md = xmlAddChild( e,  newXMLNode( _foliaNs, "metadata" ) );  
    xmlNode *an = xmlAddChild( md,  newXMLNode( _foliaNs, "annotations" ) );
    setannotations( an );
    setmetadata( md );
    vector<AbstractElement*>::const_iterator it= foliadoc->data.begin();
    while ( it != foliadoc->data.end() ){
      xmlAddChild( e, (*it)->xml( this, true ) );
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
  static set<ElementType> selectSet;
  if ( selectSet.empty() ){
    selectSet.insert( Original_t );
    selectSet.insert( Suggestion_t );
    selectSet.insert( Alternative_t );
  }
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
      // cerr << "inner LOOP I = " << i << endl;
      UnicodeString value;
      if ( pat.matchannotation == BASE )
	value = mywords[i]->text();
      else {
	vector<AbstractElement *> v = mywords[i]->select( pat.matchannotation, 
							  selectSet,
							  true );
	if ( v.size() != 1 )
	  throw ValueError( "findwords(): more then 1 annotation matched" );
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

AbstractElement *Division::head() const {
  if ( data.size() > 0 ||
       data[0]->element_id() == Head_t ){
    return data[0];
  }
  else
    throw runtime_error( "No head" );
  return 0;
}
  
AbstractElement *AbstractElement::annotation( ElementType ){
  throw runtime_error( "annotation not allowed on " + classname() );
}

AbstractElement *AbstractElement::annotation( ElementType, const string& ){
  throw runtime_error( "annotation not allowed on " + classname() );
}

AbstractElement *AbstractStructureElement::annotation( ElementType et ){
  vector<AbstractElement *>v = annotations( et );
  return v[0]; // always exist, otherwise annotations would throw()
}

vector<AbstractElement *>AbstractElement::annotations( ElementType et ){
  static set<ElementType> selectSet;
  if ( selectSet.empty() ){
    selectSet.insert( Original_t );
    selectSet.insert( Suggestion_t );
    selectSet.insert( Alternative_t );
  }
  vector<AbstractElement *>v = select( et, selectSet, true ); //non-recursive
  if ( v.size() >= 1 )
    return v;
  else
    throw NoSuchAnnotation( toString(et) );
}

AbstractElement *AbstractStructureElement::annotation( ElementType et,
						       const string& val ){
  // Will return a SINGLE annotation (even if there are multiple). 
  // Raises a NoSuchAnnotation exception if none was found
  static set<ElementType> selectSet;
  if ( selectSet.empty() ){
    selectSet.insert( Original_t );
    selectSet.insert( Suggestion_t );
    selectSet.insert( Alternative_t );
  }
  vector<AbstractElement *>v = select( et, val, selectSet, true ); //non-recursive
  if ( v.size() >= 1 )
    return v[0];
  else
    throw NoSuchAnnotation( toString(et) );
  return 0;
}

vector<AbstractElement *> AbstractStructureElement::alternatives( const string& set,
								  AnnotationType::AnnotationType type ){
  // Return a list of alternatives, either all or only of a specific type, restrained by set
  vector<AbstractElement*> alts = select( Alternative_t );
  if ( type == AnnotationType::NO_ANN ){
    return alts;
  }
  else {
    vector<AbstractElement*> res;
    for ( size_t i=0; i < alts.size(); ++i ){
      if ( alts[i]->size() > 0 ) { // child elements?
	for ( size_t j =0; j < alts[i]->size(); ++j ){
	  if ( alts[i]->index(j)->annotation_type() == type &&
	       ( alts[i]->st().empty() || alts[i]->st() == set ) ){
	    res.push_back( alts[i] ); // not the child!
	    break; // yield an alternative only once (in case there are multiple matches)
	  }
	}
      }
    }
    return res;
  }
};

string AbstractElement::pos(){
  return annotation( Pos_t )->cls();
}

string AbstractElement::lemma(){
  return annotation( Lemma_t )->cls();
}

string Gap::content() {
  vector<AbstractElement*> cv = select( Content_t );  
  if ( cv.size() < 1 )
    throw NoSuchAnnotation( "content" );
  else {
    return cv[0]->content();
  }
}

string Content::content() {
  return value; 
};

void PlaceHolder::init(){
  _xmltag="placeholder";
  _element_id = PlaceHolder_t;
  const ElementType accept[] = { TextContent_t };
  _accepted_data = std::set<ElementType>(accept, accept+1);
  _annotation_type = AnnotationType::TOKEN;
  _required_attributes = NO_ATT;
  MINTEXTCORRECTIONLEVEL = CORRECTED;
  TEXTDELIMITER = " ";
}

void PlaceHolder::setAttributes( const string&s ){
  KWargs args;
  args["text"] = s;
  Word::setAttributes( args );
}

void FoLiA::init(){
  _xmltag="FoLiA";
  _element_id = BASE;
  const ElementType accept[] = { Text_t };
  _accepted_data = std::set<ElementType>(accept, accept+1); 
}

void DCOI::init(){
  _xmltag="DCOI";
  _element_id = BASE;
  const ElementType accept[] = { Text_t };
  _accepted_data = std::set<ElementType>(accept, accept+1); 
}

void TextContent::init(){
  _element_id = TextContent_t;
  _xmltag="t";
  _corrected = CORRECTED;
  offset = -1;
}

void Head::init() {
  _element_id = Head_t;
  _xmltag="head";
  const ElementType accept[] = { Sentence_t };
  _accepted_data = std::set<ElementType>(accept, accept+1); 
  _annotation_type = AnnotationType::TOKEN;
  TEXTDELIMITER = " ";
}

void Division::init(){
  _xmltag="div";
  _element_id = Division_t;
  const ElementType accept[] = { Head_t, Paragraph_t };
  _accepted_data = std::set<ElementType>(accept, accept+2); 
  _annotation_type = AnnotationType::TOKEN;
}

void Word::init(){
  _xmltag="w";
  _element_id = Word_t;
  const ElementType accept[] = { Text_t, TextContent_t, Pos_t, Lemma_t, Alternative_t, 
				 Correction_t, ErrorDetection_t, Description_t,
				 Morphology_t };
  _accepted_data = std::set<ElementType>(accept, accept+9);
  _annotation_type = AnnotationType::TOKEN;
  _required_attributes = ID;
  _optional_attributes = CLASS|ANNOTATOR|CONFIDENCE;
  MINTEXTCORRECTIONLEVEL = CORRECTED;
  TEXTDELIMITER = " ";
}

void Gap::init(){
  _xmltag = "gap";
  _element_id = Gap_t;
  const ElementType accept[] = { Content_t, Description_t };
  _accepted_data = std::set<ElementType>(accept, accept+2); 
  _optional_attributes = ID|CLASS|ANNOTATOR|CONFIDENCE|N;
}

void Content::init(){
  _xmltag = "content";
  _element_id = Content_t;
}

void Sentence::init(){
  _xmltag="s";
  _element_id = Sentence_t;
  const ElementType accept[] = { Word_t, TextContent_t, Annolay_t, Correction_t,
				 Description_t };
  _accepted_data = std::set<ElementType>(accept, accept+5); 
  _required_attributes = ID;
  _optional_attributes = N;
}

void Text::init(){
  _xmltag="text";
  _element_id = Text_t;
  const ElementType accept[] = { Division_t, Paragraph_t, Sentence_t, Gap_t };
  _accepted_data = std::set<ElementType>(accept, accept+4); 
  _required_attributes = ID;
  TEXTDELIMITER = "\n\n";
}

void Paragraph::init(){
  _xmltag="p";
  _element_id = Paragraph_t;
  const ElementType accept[] = { Sentence_t, Head_t };
  _accepted_data = std::set<ElementType>(accept, accept+2);
  _required_attributes = ID;
}

void PosAnnotation::init(){
  _xmltag="pos";
  _element_id = Pos_t;
  _annotation_type = AnnotationType::POS;
  _required_attributes = CLASS;
  _optional_attributes = ANNOTATOR|CONFIDENCE;
  const ElementType accept[] = { Feature_t, Description_t };
  _accepted_data = std::set<ElementType>(accept, accept+2);
}

void LemmaAnnotation::init(){
  _xmltag="lemma";
  _element_id = Lemma_t;
  _annotation_type = AnnotationType::LEMMA;
  _required_attributes = CLASS;
  _optional_attributes = ANNOTATOR|CONFIDENCE;
  const ElementType accept[] = { Feature_t, Description_t };
  _accepted_data = std::set<ElementType>(accept, accept+2);
}

void SenseAnnotation::init(){
  _xmltag="sense";
  _element_id = Sense_t;
  _annotation_type = AnnotationType::SENSE;
}

void WordReference::init(){
  _required_attributes = ID;
  _xmltag = "wref";
  _element_id = WordReference_t;
  //      ANNOTATIONTYPE = AnnotationType.TOKEN
}

void SyntacticUnit::init(){
  _required_attributes = NO_ATT;
  _optional_attributes = ID|CLASS|ANNOTATOR|CONFIDENCE;
  _xmltag = "su";
  _element_id = SyntacticUnit_t;
  _annotation_type = AnnotationType::SYNTAX;
  const ElementType accept[] = { SyntacticUnit_t, Word_t, WordReference_t };
  _accepted_data = std::set<ElementType>(accept, accept+3);
}

void AbstractAnnotationLayer::init(){
  _optional_attributes = CLASS;
  _element_id = Annolay_t;
  PRINTABLE=false;
}

void SyntaxLayer::init(){
  _xmltag = "syntax";
  const ElementType accept[] = { SyntacticUnit_t };
  _accepted_data = std::set<ElementType>(accept, accept+1);
}

void Alternative::init(){
  _required_attributes = ID;
  _xmltag = "alt";
  _element_id = Alternative_t;
    const ElementType accept[] = { Pos_t, Lemma_t, Correction_t };
  _accepted_data = std::set<ElementType>(accept, accept+3);
  _annotation_type = AnnotationType::ALTERNATIVE;
  PRINTABLE = false;
}

void NewElement::init(){
  _xmltag = "new";
  _element_id = New_t;
  const ElementType accept[] = { Pos_t, Lemma_t, Word_t, TextContent_t };
  _accepted_data = std::set<ElementType>(accept, accept+4);
  MINTEXTCORRECTIONLEVEL = CORRECTED;
}

void Current::init(){
  _xmltag = "current";
  _element_id = Current_t;
  const ElementType accept[] = { Pos_t, Lemma_t, Word_t, TextContent_t };
  _accepted_data = std::set<ElementType>(accept, accept+4);
}

void NewElement::setAttributes( const KWargs& args ){
  KWargs::const_iterator it = args.find( "cls" );
  if ( it != args.end() ) {
    LemmaAnnotation *res = new LemmaAnnotation( mydoc );
    res->setAttributes( args );
    append( res );
  }
  else {
    it = args.find( "text" );
    if ( it != args.end() ) {
      TextContent *res = new TextContent( mydoc );
      res->setAttributes( args );
      append( res );
    }
  }
}

void Original::setAttributes( const KWargs& args ){
  KWargs::const_iterator it = args.find( "cls" );
  if ( it != args.end() ) {
    LemmaAnnotation *res = new LemmaAnnotation( mydoc );
    res->setAttributes( args );
    append( res );
  }
  else {
    it = args.find( "text" );
    if ( it != args.end() ) {
      TextContent *res = new TextContent( mydoc );
      res->setAttributes( args );
      append( res );
    }
  }
}

void Original::init(){
  _xmltag = "original";
  _element_id = Original_t;
  const ElementType accept[] = { Pos_t, Lemma_t, TextContent_t, Word_t };
  _accepted_data = std::set<ElementType>(accept, accept+4);
}

void Suggestion::init(){
  _xmltag = "suggestion";
  _element_id = Suggestion_t;
  const ElementType accept[] = { Pos_t, Lemma_t, TextContent_t, Word_t };
  _accepted_data = std::set<ElementType>(accept, accept+4);
  _optional_attributes = ANNOTATOR|CONFIDENCE;
  MINTEXTCORRECTIONLEVEL = CORRECTED;
}

void Correction::init(){
  _xmltag = "correction";
  _element_id = Correction_t;
  _required_attributes = ID;
  _optional_attributes = CLASS|ANNOTATOR|CONFIDENCE;
  _annotation_type = AnnotationType::CORRECTION;
  const ElementType accept[] = { New_t, Original_t, Suggestion_t, Current_t };
  _accepted_data = std::set<ElementType>(accept, accept+4);
}

void Description::init(){
  _xmltag = "desc";
  _element_id = Description_t;
}

bool Correction::hasNew( ) {
  vector<AbstractElement*> v = select( New_t, false );
  return !v.empty();
}

AbstractElement *Correction::getNew( int index ) {
  vector<AbstractElement*> v = select( New_t, false );
  if ( v.empty() )
    throw NoSuchAnnotation("new");
  if ( index < 0 )
    return v[0];
  else
    return v[0]->index(index);
}

bool Correction::hasOriginal() { 
  vector<AbstractElement*> v = select( Original_t, false );
  return !v.empty();
}

AbstractElement *Correction::getOriginal( int index ) { 
  vector<AbstractElement*> v = select( Original_t, false );
  if ( v.empty() )
    throw NoSuchAnnotation("original");
  if ( index < 0 )
    return v[0];
  else 
    return v[0]->index(index);
}

bool Correction::hasCurrent( ) { 
  vector<AbstractElement*> v = select( Current_t, false );
  return !v.empty();
}

AbstractElement *Correction::getCurrent( int index ) { 
  vector<AbstractElement*> v = select( Current_t, false );
  if ( v.empty() )
    throw NoSuchAnnotation("current");
  if ( index < 0 )
    return v[0];
  else 
    return v[0]->index(index);
}

bool Correction::hasSuggestions( ) { 
  vector<AbstractElement*> v = suggestions();
  return !v.empty();
}

vector<AbstractElement*> Correction::suggestions( ) { 
  return select( Suggestion_t, false );
}

AbstractElement *Correction::getSuggestion( int index ) { 
  vector<AbstractElement*> v = suggestions();
  if ( v.empty() )
    throw NoSuchAnnotation("suggestion");
  if ( index < 0 )
    return v[0];
  else
    return v[0]->index(index);
}

Correction *Word::correct( const std::string& s ){
  vector<AbstractElement*> nil;
  KWargs args = getArgs( s );
  //  cerr << "word::correct() <== " << this << endl;
  Correction *tmp = AbstractStructureElement::correct( nil, nil, nil, nil, args );
  //  cerr << "word::correct() ==> " << this << endl;
  return tmp;
}

Correction *Word::correct( AbstractElement *old,
			   AbstractElement *_new,
			   const KWargs& args ){
  vector<AbstractElement *> nv;
  nv.push_back( _new );
  vector<AbstractElement *> ov;
  ov.push_back( old );
  vector<AbstractElement *> nil;
  //  cerr << "correct() <== " << this;
  Correction *tmp =AbstractStructureElement::correct( ov, nil, nv, nil, args );
  //  cerr << "correct() ==> " << this;
  return tmp;
}

void ErrorDetection::setAttributes( const KWargs& kwargs ){
  map<string,string>::const_iterator it = kwargs.find( "error" );
  if ( it != kwargs.end() ) {
    string tmp = lowercase( it->second );
    if ( tmp == "no" || tmp == "false" )
      error = false;
    else
      error = true;
  }
  AbstractElement::setAttributes(kwargs);
}

void ErrorDetection::init(){
  _xmltag = "errordetection";
  _element_id = ErrorDetection_t;
  _optional_attributes = CLASS|ANNOTATOR|CONFIDENCE;
  _annotation_type = AnnotationType::ERRORDETECTION;
  error = true;
}

xmlNode *ErrorDetection::xml( Document *doc, bool ) const {
  xmlNode *e = newXMLNode( doc->foliaNs(), _xmltag );
  KWargs attribs = collectAttributes();
  if ( error )
    attribs["error"] = "yes";
  setAtt( e, attribs );
  return e;
}

xmlNode *Feature::xml( Document *doc, bool ) const {
  xmlNode *e = newXMLNode( doc->foliaNs(), _xmltag );
  KWargs attribs = collectAttributes();
  attribs["subset"] = _subset;
  setAtt( e, attribs );
  return e;
}

std::string AbstractAnnotation::feat( const std::string& s ) const {
  vector<AbstractElement*> v = const_cast<AbstractAnnotation *>(this)->select( Feature_t, false );
  vector<AbstractElement*>::const_iterator it = v.begin();
  while ( it != v.end() ){
    if ( (*it)->subset() == s )
      return (*it)->cls();
    ++it;
  }
  return "";
}

void AbstractSubtokenAnnotationLayer::init(){

}

void MorphologyLayer::init(){
  _element_id = Morphology_t;
  _xmltag = "morphology";
  const ElementType accept[] = { Morpheme_t };
  _accepted_data = std::set<ElementType>(accept, accept+1);
}

void Morpheme::init(){
  _element_id = Morpheme_t;
  _xmltag = "morpheme";
  _required_attributes = NO_ATT;
  _optional_attributes = ID|CLASS|ANNOTATOR|CONFIDENCE;
  const ElementType accept[] = { Feature_t, TextContent_t };
  _accepted_data = std::set<ElementType>(accept, accept+2);
  _annotation_type = AnnotationType::MORPHOLOGICAL;
}

void AbstractSubtokenAnnotation::init() {
  occurrences_per_set = 0; // Allow duplicates within the same set
}

void Feature::init() {
  _xmltag = "feat";
  _element_id = Feature_t;
  _required_attributes = CLASS;
}

