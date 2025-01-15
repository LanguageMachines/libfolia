/*
  Copyright (c) 2006 - 2025
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
#include <string>
#include <stdexcept>
#include <numeric>
#include "ticcutils/PrettyPrint.h"
#include "ticcutils/XMLtools.h"
#include "ticcutils/StringOps.h"
#include "libfolia/folia.h"
#include "libfolia/folia_provenance.h"

using namespace std;
using namespace icu;

namespace folia {
  using TiCC::operator<<;

  void processor::print( ostream& os, const int indent ) const {
    /// print a complete processor instance to a stream (Debugging purposes)
    /*!
      \param os The output stream
      \param indent The identation (in spaces) to use
    */
    string space = string(indent,'\t');
    os << space << "name=" << _name << endl;
    os << space << "id=" << _id << endl;
    os << space << "version=" << _version << endl;
    os << space << "type=" << TiCC::toString(_type) << endl;
    os << space << "folia_version=" << _folia_version << endl;
    os << space << "document_version=" << _document_version << endl;
    os << space << "command=" << _command << endl;
    os << space << "host=" << _host << endl << endl;
    for( const auto& c : _processors ){
      c->print( os, indent+1 );
    }
  }

  ostream& operator<<( ostream& os, const processor& p ){
    /// output a processor
    /*!
      \param os the output stream
      \param p the processor
    */
    p.print( os, 0 );
    return os;
  }

  ostream& operator<<( ostream& os, const processor *p ){
    /// output a processor
    /*!
      \param os the output stream
      \param p the processor
    */
    if ( p ){
      p->print( os, 0 );
    }
    else {
      os << "NO PROCESSOR";
    }
    return os;
  }

  void processor::get_system_defaults(){
    /// set the system information in this processor
    /*!
      will set the hostname, the username, the current time and the FoLiA
      version

      The hostname is cached, to avoid excessive call to getaddrinfo()
    */
    _host = host_name;
    _begindatetime = get_ISO_date();
    _folia_version = folia::folia_version();
    _user = get_user();
  }

  string processor::generate_id( Provenance *prov,
				 const string& in_name ){
    /// generate an processor id
    /*!
      \param prov the provenance data context
      \param in_name use this name as base for the id
      \return the new id

      First we lookup \em name in the Provenance \em prov. If it is found
      we generate a new id as sub-id of the name. When not found, we just create
      a new id \e 'name.1'

    */

    string new_id;
    string f_name = create_NCName(in_name);
    auto it = prov->_names.find(f_name);
    if ( it == prov->_names.end() ){
      if ( _debug ){
	cerr << "generate_id, " << f_name << " is new, add it " << endl;
      }
      prov->_names[f_name].insert(1);
      new_id = f_name + ".1";
    }
    else {
      int val = *(it->second.rbegin());
      if ( _debug ){
	cerr << "generate_id, " << f_name << " already there " << endl;
	cerr << "generate_id, val=" << val << endl;
      }
      prov->_names[f_name].insert(++val);
      new_id = f_name + "." + TiCC::toString(val);
    }
    if ( _debug ){
      cerr << "generate_id, generated new id" << new_id << endl;
    }
    if ( prov->get_processor_by_id(new_id) != 0 ){
      if ( _debug ){
	cerr << "generate_id, id=" << new_id << " exists, loop!" << endl;
      }
      // oops creating an existing one. Not good
      return generate_id( prov, f_name + "_1" );
    }
    return new_id;
  }

  string processor::calculate_next_id(){
    /// create a successor id for this processor
    /*!
      \return the new id

      When the processor has subprocessors, we create an id which is 1 beyond
      that of the last subprocessor

      Otherwise we create an id for the first subprocessor
    */
    string new_id;
    if ( !sub_processors().empty() ){
      string prev_id = sub_processors().back()->id();
      vector<string> v = TiCC::split_at( prev_id, "." );
      int val;
      if ( TiCC::stringTo( v.back(), val ) ){
	v.back() = TiCC::toString(++val);
      }
      else {
	// not a number, just add .1 then, and pray
	v.back() += ".1";
      }
      auto dot_insert = []( string& a, const string& b ){
			  return std::move(a) + "." + b;
			};
      new_id = std::accumulate( std::next(v.begin()), v.end(),
				v[0],
				dot_insert );
    }
    else {
      new_id = id() + ".1";
    }
    return new_id;
  }

  processor::processor( Provenance *prov,
			processor* parent,
			const KWargs& atts_in ) {
    /// initialize a processor
    /*!
      \param prov The provenance context
      \param parent A parent to connect to
      \param atts_in A KWargs list with values to set for the processor
    */
    if ( !prov ){
      throw logic_error( "processor: no Provenance context" );
    }
    _debug = prov->_debug;
    _type = AnnotatorType::AUTO;
    KWargs atts = atts_in;
    string name_value = atts.extract("name");
    if ( name_value.empty() ){
      throw XmlError( "processor: missing 'name' attribute" );
    }
    else {
      _name = name_value;
    }
    if ( _debug ){
      cerr << "new processor(" << atts_in << ")" << endl;
    }
    string id_val = atts.extract("id");
    if ( id_val.empty() ){
      id_val = atts.extract("xml:id");
    }
    if ( id_val.empty() ){
      string gen = atts.extract("generate_id");
      if ( gen.empty() ){
	throw XmlError( "processor: missing 'xml:id' attribute" );
      }
      if ( _debug ){
	cerr << "new processor generate_id() gen==" << gen << endl;
      }
      if ( gen == "auto()" ){
	id_val = generate_id( prov, _name );
	if ( _debug ){
	  cerr << "new processor generate_id(" << _name << ") ==>"
	     << id_val << endl;
	}
      }
      else if ( gen == "next()" ){
	if ( !parent ){
	  // fall back to auto()
	  id_val = generate_id( prov, _name );
	  //	throw invalid_argument( "processor id=next() impossible. No parent" );
	}
	else {
	  id_val = parent->calculate_next_id();
	}
	if ( _debug ){
	  cerr << "new processor calculate_next() ==>" << id_val << endl;
	}
      }
      else {
	id_val = generate_id( prov, gen );
	if ( _debug ){
	  cerr << "new processor generate_id(" << gen << ") ==>"
	       << id_val << endl;
	}
      }
    }
    else if ( id_val == "next()" ){
      if ( !parent ){
	// fall back to auto()
	id_val = generate_id( prov, _name );
      }
      else {
	id_val = parent->calculate_next_id();
      }
      if ( _debug ){
	cerr << "new processor calculate SPECIAAL() ==>" << id_val << endl;
      }
    }
    const processor *check = prov->get_processor_by_id( id_val );
    if ( check ){
      throw DuplicateIDError( "processor '" + id_val + "' already exists" );
    }
    _id = id_val;
    for ( const auto& [att,val] : atts ){
      if ( att == "begindatetime" ){
	if ( val == "now()" ){
	  _begindatetime = get_ISO_date();
	}
	else {
	  _begindatetime = val;
	}
      }
      else if ( att == "enddatetime" ){
	if ( val == "now()" ){
	  _enddatetime = get_ISO_date();
	}
	else {
	  _enddatetime = val;
	}
      }
      else if ( att == "version" ){
	_version = val;
      }
      else if ( att == "document_version" ){
	_document_version = val;
      }
      else if ( att == "command" ){
	_command = val;
      }
      else if ( att == "folia_version" ){
	_folia_version = val;
      }
      else if ( att == "type" ){
	try {
	  _type = TiCC::stringTo<AnnotatorType>( val );
	}
	catch (...){
	  throw XmlError( "processor: invalid value for 'type' attribute: "
			  + val );
	}
      }
      else if ( att == "host" ){
	_host = val;
      }
      else if ( att == "resourcelink" ){
	_resourcelink = val;
      }
      else if ( att == "user" ){
	_user = val;
      }
      else if ( att == "src" ){
	_src = val;
      }
      else if ( att == "format" ){
	_format = val;
      }
      else if ( att == "generator" ){
	// we automagicly add a subprocessor.
	KWargs g_atts;
	g_atts.add("folia_version", folia::folia_version());
	g_atts.add("version", library_version());
	g_atts.add("type","GENERATOR");
	g_atts.add("id",_id + ".generator");
	g_atts.add("name","libfolia");
	processor *sub = new processor( prov, this, g_atts );
	this->_processors.push_back( sub );
      }
    }
    prov->add_to_index(this);
  }

  processor::~processor(){
    /// deconstructor for a processor and its subprocessors
    for ( const auto& p : _processors ){
      delete p;
    }
  }

  bool processor::set_metadata( const string& id,
				const string& val ){
    /// set a metadata property in the processor
    /*!
      \param id the name of the property
      \param val the value to set
      \return true when set, false when already set
    */
    if ( _metadata[id].empty() ){
      _metadata[id] = val;
      return true;
    }
    else {
      return false;
    }
  }

  string processor::get_metadata( const string& id ){
    /// get a metadata property from the processor
    /*!
      \param id the name of the property to return
      \return the value when found or "" when not found
    */
    auto it = _metadata.find( id );
    if ( it != _metadata.end() ){
      return it->second;
    }
    return "";
  }

  Provenance::~Provenance(){
    /// deconstruct this provenance context and it's processors
    for ( const auto& p : processors ){
      delete p;
    }
  }

  processor *Provenance::get_processor_by_id( const string& id ) const {
    ///  return a processor with the given id
    /*!
      \param id the processor id we search for
      \return the found processor or 0 when not found
    */
    const auto& p = _index.find( id );
    if ( p != _index.end() ){
      return p->second;
    }
    else {
      return 0;
    }
  }

  vector<processor*> Provenance::get_processors_by_name( const string& name ) const {
    /// give a list of all processors with this name
    /*!
      \param name the name to search for
      \return a list of found processors

      \note processor id's are UNIQUE, processor names ARN'T
    */
    vector<processor*> result;
    for ( auto p = _name_index.lower_bound( name );
	  p !=  _name_index.upper_bound( name );
	  ++p ){
      result.push_back( p->second );
    }
    return result;
  }

  processor *Provenance::get_top_processor() const {
    /// return the main processor in this Provenance context
    return _first_proc;
  }

  void Provenance::add_to_index( processor *p ){
    /// add a procesor to the index
    _index[p->id()] = p;
    _name_index.insert( make_pair(p->name(),p) );
    if ( _first_proc == 0 ){
      _first_proc = p;
    }
  }

  void Provenance::parse_processor( const xmlNode *node,
				    processor *parent ) {
    /// parse a processor from XML
    /*!
      \param node the xmlNode whre the processor is found
      \param parent the processor to connect to (may be 0)
     */
    KWargs node_atts = getAttributes( node );
    processor *main = new processor( this, parent, node_atts );
    if ( parent ){
      parent->_processors.push_back( main );
    }
    else {
      processors.push_back( main );
    }
    //    cerr << "created procesor(" << node_atts << ")" << endl;
    const xmlNode *n = node->children;
    while ( n ){
      string tag = TiCC::Name( n );
      if ( tag == "processor" ){
     	parse_processor(n,main);
      }
      else if ( tag == "meta" ){
	KWargs atts = getAttributes( n );
	string id = atts["id"];
	if ( id.empty() ){
	  throw XmlError( "processor: missing 'id' for meta tag" );
	}
	if ( atts.size() != 1 ){
	  throw XmlError( "processor: invalid attribute(s) in meta tag" );
	}
	string value = TiCC::TextValue( n );
	if ( value.empty() ){
	  throw XmlError( "processor: empty id" );
	}
	main->_metadata[id] = value;
      }
      n = n->next;
    }
  }

  ostream& operator<<( ostream& os, const Provenance& p ){
    /// output the provenance context (debugging only)
    os << "provenance data" << endl;
    os << "NAMES: " << p._names << endl;
    for ( const auto* pr : p.processors ){
      pr->print( os, 2 );
      os << endl;
    }
    return os;
  }

  ostream& operator<<( ostream& os, const Provenance* p ){
    /// output the provenance context (debugging only)
    if ( p ){
      os << *p;
    }
    else {
      os << "no provenance";
    }
    return os;
  }

} // namespace folia
