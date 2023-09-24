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
    Engine(); //!< default constructor. needs a call to init_doc() to get started
    // cppcheck-suppress noExplicitConstructor
    // We want to be able to use const char parameters AND string
    Engine( const std::string& i, const std::string& o="" ):
      Engine() {
      /// construct and initialize an engine.
      /*!
	\param i name of the inputfile
	\param o optional name of an outputfile
      */
      init_doc(i,o);
    };
    bool init_doc( const std::string&, const std::string& ="" );
    virtual ~Engine();
    FoliaElement *get_node( const std::string& );
    bool next() { return true; }; /// A stub. NOT needed!
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
    void set_metadata( const std::string&, const std::string& );
    bool set_debug( bool d );
    void set_dbg_stream( TiCC::LogStream * );
    Document *doc( bool=false ); // returns the doc. may disconnect
    xml_tree *create_simple_tree( const std::string& ) const;
  protected:
    xmlTextReader *_reader; //!< the xmlTextReader we use for parsing the input
    Document *_out_doc;     //!< the output Document we are constructing
    FoliaElement *_root_node;     //!< the root node (Speech or Text)
    FoliaElement *_external_node; //!< the last node we exposed to the world
    FoliaElement *_current_node;  //!< the FoliaElement at hand
    FoliaElement *_last_added;    //!< the last added FoliaElement
    int _last_depth;        //!< at what depth in the tree was the last addition
    int _start_index;       //!< the index of the first relevant node
    doctype _doc_type;      //!< do we process TEXT or SPEECH?
    TiCC::LogStream *_dbg_file; //!< the debugging stream
    std::ostream *_os;      //!< optional outputstream
    std::string _out_name;  //!< the name of the output file connected to _os
    std::string ns_prefix;  //!< a namespace name to use. (copied from the input file)
    std::string _footer;    //!< the constructed string to output at the end
    bool _ok;               //!< are we fine?
    bool _done;             //!< are we done parsing?
    bool _header_done;      //!< is the header outputed yet?
    bool _finished;         //!< did we finish the whole process?
    bool _debug;            //!< is debug on?

    FoliaElement *handle_match( const std::string&, int );
    void handle_element( const std::string&, int );
    int handle_content( const std::string&, int );
    void add_default_node( int );
    void add_comment( int );
    void add_PI( int );
    void add_text( int );
    void append_node( FoliaElement *, int );
  };

  class TextEngine: public Engine {
  public:
    TextEngine(): Engine(), //!< default construcor. Needs a call to init_doc()
		  _next_text_node(0),
		  _node_count(0),
		  _is_setup(false)
    {
    };
    // cppcheck-suppress noExplicitConstructor
    // We want to be able to use const char parameters AND string
    TextEngine( const std::string& i, const std::string& o="" ):
      TextEngine(){
      /// construct a TextEngine
      /*!
	\param i the input file
	\param o an optional output file
	To be able to use the TextEngine, a call to setup() is still needed
      */
      init_doc( i, o );
    }
    bool init_doc( const std::string&, const std::string& ="" );
    void setup( const std::string& ="", bool = false );
    const std::map<int,int>& enumerate_text_parents( const std::string& ="",
						     bool = false );
    size_t text_parent_count() const {
      /// return the number of textparents found
      return text_parent_map.size();
    };
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
