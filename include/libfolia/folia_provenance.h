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

#ifndef FOLIA_PROVENANCE_H
#define FOLIA_PROVENANCE_H

#include <list>
#include <set>
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include "unicode/unistr.h"
#include "unicode/regex.h"
#include "libxml/tree.h"
#include "libxml/xpath.h"
#include "libfolia/folia.h"

namespace folia {

  class processor {
    friend class Document;
    friend class Provenance;
  public:
    processor( Provenance*, processor*, const KWargs& );
    ~processor();
    void get_system_defaults();
    std::string generate_id( Provenance*, const std::string& );
    std::string calculate_next_id();
    std::string name() const { return _name; };
    std::string annotator() const { return _name; };
    std::string id() const { return _id; };
    AnnotatorType type() const { return _type; };
    AnnotatorType annotatortype() const { return _type; };
    std::string version() const { return _version; } ;
    std::string document_version() const { return _document_version; };
    std::string folia_version() const { return _folia_version; };
    std::string command() const { return _command; };
    std::string host() const { return _host; };
    std::string user() const { return _user; };
    std::string begindatetime() const { return _begindatetime; };
    std::string enddatetime() const { return _enddatetime; };
    std::string resourcelink() const { return _resourcelink; };
    std::string src() const { return _src; };
    std::string format() const { return _format; };
    std::vector<processor*> processors() const { return _processors; };
    bool set_metadata( const std::string&, const std::string& );
    std::string get_metadata( const std::string& );
    void print( std::ostream&, const int ) const;
  private:
    std::string _name;
    std::string _id;
    AnnotatorType _type;
    std::string _version;
    std::string _document_version;
    std::string _folia_version;
    std::string _command;
    std::string _host;
    std::string _user;
    std::string _begindatetime;
    std::string _enddatetime;
    std::string _resourcelink;
    std::string _src;
    std::string _format;
    std::map<std::string,std::string> _metadata;
    std::vector<processor*> _processors;
  };

  std::ostream& operator<<( std::ostream&, const processor& );
  std::ostream& operator<<( std::ostream&, const processor * );

  class Provenance {
    friend class processor;
    friend std::ostream& operator<<( std::ostream&, const Provenance& );
  public:
  Provenance( Document *doc ): _doc(doc),_first_proc(0){};
    ~Provenance();
    void parse_processor( const xmlNode *, processor * =0 );
    processor *get_processor( const std::string& ) const;
    std::vector<processor*> get_processors_by_name( const std::string& ) const;
    processor *get_top_processor() const;
    xmlNode *xml();
    Provenance *parseXml( const xmlNode * );
    std::vector<processor*> processors;
    processor* operator []( const std::string& s ) const {
      //select processor with index s
      return get_processor( s );
    };
    processor* index( const std::string& s ) const {
      //select processor with index s
      return get_processor( s );
    };
    void add_index( processor *p );
  private:
    Document*  _doc; // which doc we belong to.
    processor*  _first_proc;
    std::map<std::string,processor*> _index;
    std::map<std::string,std::set<int>> _names;
    std::multimap<std::string,processor*> _name_index;
    Provenance( const Provenance& ); // inhibit copy
    Provenance& operator=( const Provenance& ); // inhibit copies
  };

  std::ostream& operator<<( std::ostream&, const Provenance& );
  std::ostream& operator<<( std::ostream&, const Provenance * );

  std::string randnum( int len );
  //  std::string randnum8(){ return randnum(8); };

} // namespace folia

#endif // FOLIA_PROVENANCE_H
