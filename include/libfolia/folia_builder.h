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

#ifndef FOLIA_BUILDER_H
#define FOLIA_BUILDER_H

#include <string>
#include <iostream>
#include "libfolia/folia.h"

namespace folia {

  class Builder {
  public:
    enum doctype {TEXT,SPEECH};
    Builder( std::ostream&, const std::string&, doctype = TEXT );
    ~Builder();
    Document *doc() const { return _doc; }
    bool add( FoliaElement * );
    bool output_header();
    bool output_footer();
    bool flush();
    bool finish();
  private:
    Document *_doc;
    doctype _doc_type;
    FoliaElement *root_node;
    std::ostream& _os;
    std::string _footer;
    bool header_done;
    bool finished;
  protected:
    Builder();
  };

}
#endif // FOLIA_BUILDER_H
