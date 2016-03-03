#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <set>
#include <string>

namespace folia {
  enum ElementType : unsigned int;
  enum Attrib : int;
  enum AnnotatorType: int;

  namespace AnnotationType {
    enum AnnotationType : int;
  }

  class properties {
   public:
    properties();
    ElementType ELEMENT_ID;
    std::string XMLTAG;
    std::set<ElementType> ACCEPTED_DATA;
    std::set<ElementType> REQUIRED_DATA;
    Attrib REQUIRED_ATTRIBS;
    Attrib OPTIONAL_ATTRIBS;
    AnnotationType::AnnotationType ANNOTATIONTYPE;
    std::string SUBSET;
    size_t OCCURRENCES;
    size_t OCCURRENCES_PER_SET;
    std::string TEXTDELIMITER;
    bool PRINTABLE;
    bool SPEAKABLE;
    bool XLINK;
    bool AUTH;
    bool SETONLY;
  };

  extern const std::map<ElementType,std::string> et_s_map;
  extern const std::map<std::string,ElementType> s_et_map;
  extern const std::map<AnnotationType::AnnotationType,std::string> ant_s_map;
  extern const std::map<std::string,AnnotationType::AnnotationType> s_ant_map;

  extern const std::set<ElementType> default_ignore;
  extern const std::set<ElementType> default_ignore_annotations;
  extern const std::set<ElementType> default_ignore_structure;
  extern const std::set<ElementType> AnnoExcludeSet;
  extern const std::set<ElementType> SpanSet;

  extern const int MAJOR_VERSION;
  extern const int MINOR_VERSION;
  extern const int SUB_VERSION;
  extern const std::string NSFOLIA;
  extern const std::string NSDCOI;
  extern const std::string NSIMDI;

  void static_init();
  void print_type_hierarchy( std::ostream& );
}

#endif
