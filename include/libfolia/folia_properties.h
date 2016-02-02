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
    ElementType _element_id;
    std::string _xmltag;
    std::set<ElementType> _accepted_data;
    Attrib _required_attributes;
    Attrib _optional_attributes;
    AnnotationType::AnnotationType _annotation_type;
    size_t _occurrences;
    size_t _occurrences_per_set;
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
}

#endif
