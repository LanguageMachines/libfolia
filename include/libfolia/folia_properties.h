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

#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <set>
#include <string>

namespace folia {
  enum ElementType : unsigned int;
  enum Attrib : int;
  enum AnnotatorType: int;
  enum AnnotationType : int;

  class properties {
   public:
    properties();
    ElementType ELEMENT_ID;
    std::string XMLTAG;
    std::set<ElementType> ACCEPTED_DATA;
    std::set<ElementType> REQUIRED_DATA;
    Attrib REQUIRED_ATTRIBS;
    Attrib OPTIONAL_ATTRIBS;
    AnnotationType ANNOTATIONTYPE;
    std::string SUBSET;
    size_t OCCURRENCES;
    size_t OCCURRENCES_PER_SET;
    std::string TEXTDELIMITER;
    bool PRINTABLE; // does text() work on this element
    bool SPEAKABLE; // does phon() work on this element
    bool WREFABLE;  // can this element be refered to by a wref?
    bool HIDDEN;
    bool XLINK;
    bool AUTH;
    bool SETONLY;
    bool AUTO_GENERATE_ID;
    bool TEXTCONTAINER;
    bool PHONCONTAINER;
    std::string LABEL;
  };

  extern const std::set<std::string> AttributeFeatures;
  extern const std::map<ElementType,std::string> et_s_map;
  extern const std::map<std::string,ElementType> s_et_map;
  extern const std::map<AnnotationType,std::string> ant_s_map;
  extern const std::map<std::string,AnnotationType> s_ant_map;
  extern const std::map<AnnotationType, ElementType> annotationtype_elementtype_map;
  extern const std::map<AnnotationType,std::string> annotationtype_xml_map;
  extern const std::map<std::string,std::string> oldtags;
  extern std::map<std::string,std::string> reverse_old;

  extern const std::set<ElementType> default_ignore;
  extern const std::set<ElementType> default_ignore_annotations;
  extern const std::set<ElementType> default_ignore_structure;
  extern const std::set<ElementType> AnnoExcludeSet;
  extern const std::set<ElementType> SpanSet;
  extern const std::set<ElementType> wrefables;

  extern const int MAJOR_VERSION;
  extern const int MINOR_VERSION;
  extern const int SUB_VERSION;
  extern const std::string NSFOLIA;
  extern const std::string NSDCOI;
  extern const std::string NSIMDI;
  extern const std::string DEFAULT_TEXT_SET;
  extern const std::string DEFAULT_PHON_SET;

  bool isAttributeFeature( const std::string& );

  void static_init();
  void print_type_hierarchy( std::ostream& );

  extern const int XML_PARSER_OPTIONS;
}

#endif
