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

  struct properties {
    ElementType ELEMENT_ID;
    std::string XMLTAG;
    std::set<ElementType> ACCEPTED_DATA;
    Attrib REQUIRED_ATTRIBS;
    Attrib OPTIONAL_ATTRIBS;
    AnnotationType::AnnotationType ANNOTATIONTYPE;
    size_t OCCURRENCES;
    size_t OCCURRENCES_PER_SET;
    std::string TEXTDELIMITER;
    bool PRINTABLE;
    bool SPEAKABLE;
    bool XLINK;
  };

  extern const std::set<ElementType> default_ignore;
  extern const std::set<ElementType> default_ignore_annotations;
  extern const std::set<ElementType> default_ignore_structure;
  extern const std::set<ElementType> AnnoExcludeSet;
  extern const std::set<ElementType> SpanSet;

  void static_init();
  void print_type_hierarchy( std::ostream& );
}

#endif
