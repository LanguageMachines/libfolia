/*
  Copyright (c) 2006 - 2019
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
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <random>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
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
    p.print( os, 0 );
    return os;
  }

  ostream& operator<<( ostream& os, const processor *p ){
    if ( p ){
      p->print( os, 0 );
    }
    else {
      os << "NO PROCESSOR";
    }
    return os;
  }

  unsigned long long random64(){
    static std::random_device rd;
    static std::uniform_int_distribution<unsigned long long> dis;
    static std::mt19937_64 gen(rd());
    return dis(gen);
  }

  string randnum( int len ){
    stringstream ss;
    ss << random64() << endl;
    string result = ss.str();
    return result.substr(0,len);
  }

  string getfqdn( ){
    string result;
    struct addrinfo hints, *info, *p;
    int gai_result;

    char hostname[1024];
    hostname[1023] = '\0';
    gethostname(hostname, 1023);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; /*either IPV4 or IPV6*/
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_CANONNAME;

    if ((gai_result = getaddrinfo(hostname, "http", &hints, &info)) != 0) {
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(gai_result));
      exit(1);
    }

    for(p = info; p != NULL; p = p->ai_next) {
      //      printf("hostname: %s\n", p->ai_canonname);
      result = p->ai_canonname;
      break;
    }
    freeaddrinfo(info);
    return result;
  }

  string get_user(){
    string result;
    const char *env = getenv( "USER" );
    if ( env ){
      result = env;
    }
    return result;
  }

  void processor::get_system_defaults(){
    _host = getfqdn();
    _begindatetime = get_ISO_date();
    _folia_version = folia::folia_version();
    _user = get_user();
  }

  string processor::generate_id( Provenance *prov,
				 const string& name ){
    string new_id;
    auto it = prov->_names.find(name);
    if ( it == prov->_names.end() ){
      if ( !isNCName(name) ){
	throw XmlError( "generated_id: '" + name
			+ "' is not a valid base for an NCName." );
      }
      prov->_names[name].insert(1);
      new_id = name + ".1";
    }
    else {
      int val = *(it->second.end());
      prov->_names[name].insert(++val);
      new_id = name + "." + TiCC::toString(val);
    }
    if ( prov->_doc->index(new_id) != 0 ){
      // oops creating an existing one. Not good
      return generate_id( prov, name + "_1" );
    }
    return new_id;
  }

  string processor::calculate_next_id(){
    string new_id;
    if ( !processors().empty() ){
      string prev_id = processors().back()->id();
      vector<string> v = TiCC::split_at( prev_id, "." );
      int val;
      if ( TiCC::stringTo( v.back(), val ) ){
	v.back() = TiCC::toString(++val);
      }
      else {
	// not a number, just add .1 then, and pray
	v.back() += ".1";
      }
      for ( const auto& it :  v ){
	new_id += it + ".";
      }
      new_id.pop_back();
    }
    else {
      new_id = id() + ".1";
    }
    return new_id;
  }

  //#define PROC_DEBUG

  processor::processor( Provenance *prov,
			processor* parent,
			const KWargs& atts_in ) {
    _type = AUTO;
    KWargs atts = atts_in;
    string name = atts.extract("name");
    if ( name.empty() ){
      throw XmlError( "processor: missing 'name' attribute" );
    }
    else {
      _name = name;
    }
#ifdef PROC_DEBUG
    cerr << "new processor(" << atts_in << ")" << endl;
#endif
    string id = atts.extract("id");
    if ( id.empty() ){
      id = atts.extract("xml:id");
    }
    if ( id.empty() ){
      string gen = atts.extract("generate_id");
      if ( gen.empty() ){
	throw XmlError( "processor: missing 'xml:id' attribute" );
      }
#ifdef PROC_DEBUG
      cerr << "new processor generate_id() gen==" << gen << endl;
#endif
      if ( gen == "auto()" ){
	id = generate_id( prov, _name );
#ifdef PROC_DEBUG
	cerr << "new processor generate_id(" << _name << ") ==>" << id << endl;
#endif
      }
      else if ( gen == "next()" ){
	if ( !parent ){
	  // fall back to auto()
	  id = generate_id( prov, _name );
	  //	throw invalid_argument( "processor id=next() impossible. No parent" );
	}
	else {
	  id = parent->calculate_next_id();
	}
#ifdef PROC_DEBUG
	cerr << "new processor calculate_next() ==>" << id << endl;
#endif
      }
      else {
	id = generate_id( prov, gen );
#ifdef PROC_DEBUG
	cerr << "new processor generate_id(" << gen << ") ==>" << id << endl;
#endif
      }
    }
    else if ( id == "next()" ){
      if ( !parent ){
	// fall back to auto()
	id = generate_id( prov, _name );
	//	throw invalid_argument( "processor id=next() impossible. No parent" );
      }
      else {
	id = parent->calculate_next_id();
      }
#ifdef PROC_DEBUG
      cerr << "new processor calculate SPECIAAL() ==>" << id << endl;
#endif
    }
    processor *check = prov->get_processor( id );
    if ( check ){
      throw DuplicateIDError( "processor '" + id + "' already exist" );
    }
    _id = id;
    for ( const auto& att : atts ){
      if ( att.first == "begindatetime" ){
	if ( att.second == "now()" ){
	  _begindatetime = get_ISO_date();
	}
	else {
	  _begindatetime = att.second;
	}
      }
      else if ( att.first == "enddatetime" ){
	if ( att.second == "now()" ){
	  _enddatetime = get_ISO_date();
	}
	else {
	  _enddatetime = att.second;
	}
      }
      else if ( att.first == "version" ){
	_version = att.second;
      }
      else if ( att.first == "document_version" ){
	_document_version = att.second;
      }
      else if ( att.first == "command" ){
	_command = att.second;
      }
      else if ( att.first == "folia_version" ){
	_folia_version = att.second;
      }
      else if ( att.first == "type" ){
	try {
	  _type = TiCC::stringTo<AnnotatorType>( att.second );
	}
	catch (...){
	  throw XmlError( "processor: invalid value for 'type' attribute: "
			  + att.second );
	}
      }
      else if ( att.first == "host" ){
	_host = att.second;
      }
      else if ( att.first == "resourcelink" ){
	_resourcelink = att.second;
      }
      else if ( att.first == "user" ){
	_user = att.second;
      }
      else if ( att.first == "src" ){
	_src = att.second;
      }
      else if ( att.first == "format" ){
	_format = att.second;
      }
      else if ( att.first == "generator" ){
	// we automagicly add a subprocessor.
	KWargs atts;
	atts["folia_version"] = folia::folia_version();
	atts["version"] = library_version();
	atts["type"] = "GENERATOR";
	atts["id"] = _id + ".generator";
	atts["name"] = "libfolia";
	new processor( prov, this, atts );
      }
    }
    if ( parent ){
      parent->_processors.push_back( this );
    }
    else {
      prov->processors.push_back( this );
    }
    prov->add_index(this);
  }

  processor::~processor(){
    for ( const auto& p : _processors ){
      delete p;
    }
  }

  bool processor::set_metadata( const string& id,
				const string& val ){
    if ( _metadata[id].empty() ){
      _metadata[id] = val;
      return true;
    }
    else {
      return false;
    }
  }

  string processor::get_metadata( const string& id ){
    auto it = _metadata.find( id );
    if ( it != _metadata.end() ){
      return it->second;
    }
    return "";
  }

  Provenance::~Provenance(){
    for ( const auto& p : processors ){
      delete p;
    }
  }

  processor *Provenance::get_processor( const string& pid ) const {
    const auto& p = _index.find( pid );
    if ( p != _index.end() ){
      return p->second;
    }
    else {
      return 0;
    }
  }

  vector<processor*> Provenance::get_processors_by_name( const string& name ) const {
    vector<processor*> result;
    for ( auto p = _name_index.lower_bound( name );
	  p !=  _name_index.upper_bound( name );
	  ++p ){
      result.push_back( p->second );
    }
    return result;
  }

  processor *Provenance::get_top_processor() const {
    return _first_proc;
  }

  void Provenance::add_index( processor *p ){
    _index[p->id()] = p;
    _name_index.insert( make_pair(p->name(),p) );
    if ( _first_proc == 0 ){
      _first_proc = p;
    }
  }

  processor *Provenance::parse_processor( const xmlNode *node,
					  processor *parent ) {
    KWargs atts = getAttributes( node );
    processor *result = new processor( this, parent, atts );
    //    cerr << "created procesor(" << atts << ")" << endl;
    xmlNode *n = node->children;
    while ( n ){
      string tag = TiCC::Name( n );
      if ( tag == "processor" ){
     	parse_processor(n,result);
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
	string value = TiCC::XmlContent( n );
	result->_metadata[id] = value;
      }
      n = n->next;
    }
    return result;
  }

  ostream& operator<<( ostream& os, const Provenance& p ){
    os << "provenance data" << endl;
    os << "NAMES: " << p._names << endl;
    for ( const auto& pr : p.processors ){
      pr->print( os, 2 );
      os << endl;
    }
    return os;
  }

  ostream& operator<<( ostream& os, const Provenance* p ){
    if ( p ){
      os << *p;
    }
    else {
      os << "no provenance";
    }
    return os;
  }

} // namespace folia
