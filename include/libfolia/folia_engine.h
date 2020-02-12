/*
  Copyright (c) 2006 - 2020
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

#ifndef FOLIA_ENGINE_H
#define FOLIA_ENGINE_H

#include <string>
#include <set>
#include <vector>
#include <iostream>
#include "ticcutils/LogStream.h"
#include "libfolia/folia.h"
#include "libxml/xmlreader.h"

namespace folia {

  class xml_tree {
    /// a lightweight representation of an Xml tree, used in the enumeration
    /// process
  public:
    xml_tree( int, int, const std::string&, const std::string& );
    ~xml_tree();
    int depth;
    int index;
    std::string tag;
    std::string textclass;
    xml_tree *parent;
    xml_tree *link;
    xml_tree *next;
  };

  void print( std::ostream&, const xml_tree* );

  class Engine {
  public:
    /// the document type, determines the type of the top node (\<text> or \<speech>)
    enum doctype { TEXT, //!< the topnode is \<text>
		   SPEECH //!< the topnode is \<speech>
    };
    Engine();
  Engine( const std::string& i, const std::string& o="" ):
    Engine() { init_doc(i,o); };
    virtual bool init_doc( const std::string&, const std::string& ="" );
    virtual ~Engine();
    FoliaElement *get_node( const std::string& );
    bool next();
    void save( const std::string&, bool=false );
    void save( std::ostream&, bool=false );
    bool output_header();
    bool output_footer();
    bool flush();
    bool finish();
    /// return the status of the Engine. True when still valid. False otherwise.
    bool ok() const { return _ok; };
    void un_declare( const AnnotationType&,
		     const std::string& = "" );
    void declare( const AnnotationType&,
		  const std::string&,
		  const std::string& = "" );
    void declare( const AnnotationType&,
		  const std::string&,
		  const KWargs& );
    bool is_declared( const AnnotationType&,
		      const std::string& = "" ) const;
    bool is_declared( const AnnotationType&,
		      const std::string&, const std::string&,
		      const std::string&, const std::string& = "" ) const;
    bool is_declared( const AnnotationType&,
		      const std::string&, const std::string&,
		      const AnnotatorType&, const std::string& = "" ) const;
    void set_metadata( const std::string&, const std::string& );
    bool set_debug( bool d );
    void set_dbg_stream( TiCC::LogStream * );
    Document *doc( bool=false ); // returns the doc. may disconnect
    xml_tree *create_simple_tree( const std::string& ) const;
  protected:
    xmlTextReader *_reader;
    Document *_out_doc;
    FoliaElement *_root_node;
    FoliaElement *_external_node;
    FoliaElement *_current_node;
    FoliaElement *_last_added;
    int _last_depth;
    int _start_index;
    doctype _doc_type;
    TiCC::LogStream *_dbg_file;
    std::ostream *_os;
    std::string _footer;
    std::string _out_name;
    std::string ns_prefix;
    bool _header_done;
    bool _finished;
    bool _ok;
    bool _done;
    bool _debug;

    FoliaElement *handle_match( const std::string&, int );
    void handle_element( const std::string&, int );
    int handle_content( const std::string&, int );
    void add_default_node( int );
    void add_comment( int );
    void add_text( int );
    void append_node( FoliaElement *, int );
  };

  class TextEngine: public Engine {
  public:
  TextEngine(): Engine(){};
  TextEngine( const std::string& i, const std::string& o="" ):
    TextEngine(){
      init_doc( i, o );
    }
    bool init_doc( const std::string&, const std::string& ="" );
    void setup( const std::string& ="", bool = false );
    std::map<int,int> enumerate_text_parents( const std::string& ="",
					      bool = false ) const;
    size_t text_parent_count() const { return text_parent_map.size(); };
    FoliaElement *next_text_parent();
  private:
    int _next_text_node;
    int _node_count;
    std::string _in_file;
    std::map<int,int> text_parent_map;
    std::map<int,int> search_text_parents( const xml_tree*,
					   const std::string&, bool ) const;
    bool _is_setup;
  };

}
#endif // FOLIA_ENGINE_H
