/*
  Copyright (c) 2006 - 2018
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

#ifndef FOLIA_PROCESSOR_H
#define FOLIA_PROCESSOR_H

#include <string>
#include <iostream>
#include "libfolia/folia.h"
#include "libxml/xmlreader.h"

namespace folia {

  class Processor {
  public:
    Processor();
  Processor( const std::string& i, const std::string& o="" ):
    Processor() { init_doc(i,o); };
    bool init_doc( const std::string&, const std::string& ="" );
    ~Processor();
    FoliaElement *get_node( const std::string& );
    bool next();
    void append_node( FoliaElement *, int );
    void save( const std::string&, bool=false );
    bool output_header();
    bool output_footer();
    bool flush();
    bool finish();
    bool ok() const { return _ok; };
    void declare( AnnotationType::AnnotationType,
		  const std::string&,
		  const std::string& = "" );
    bool is_declared( AnnotationType::AnnotationType,
		      const std::string& = "" ) const;
    void declare( AnnotationType::AnnotationType,
		  const std::string&, const std::string&,
		  const std::string&, const std::string&,
		  const std::string& = "" );
    bool is_declared( AnnotationType::AnnotationType,
		      const std::string&, const std::string&,
		      const std::string& ) const;
    void set_metadata( const std::string&, const std::string& );
    bool set_debug( bool d ) { bool res = _debug; _debug = d; return res; };
    Document *doc() { return _out_doc; };
  private:
    xmlTextReader *_in_doc;
    Document *_out_doc;
    FoliaElement *_root_node;
    FoliaElement *_external_node;
    FoliaElement *_current_node;
    FoliaElement *_last_added;
    int _last_depth;
    std::ostream *_os;
    std::string _footer;
    std::string _out_name;
    std::string ns_prefix;
    bool _paused;
    bool _header_done;
    bool _finished;
    bool _ok;
    bool _done;
    bool _debug;
  };

}
#endif // FOLIA_PROCESSOR_H
