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
    AnnotatorType _annotator_type;
    bool _auth;
    double _confidence;
    size_t _occurrences;
    size_t _occurrences_per_set;
    std::string TEXTDELIMITER;
    bool PRINTABLE;
    bool SPEAKABLE;
    bool XLINK;
  };

  //  std::ostream& operator<<( std::ostream& os, const properties& );
  //  std::ostream& operator<<( std::ostream& os, const properties * );

}

#endif
