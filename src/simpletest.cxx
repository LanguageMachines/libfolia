/*
  Copyright (c) 2006 - 2024
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

#include <iostream>
#include <string>
#include <map>
#include <cassert>
#include "ticcutils/StringOps.h"
#include "ticcutils/Unicode.h"
#include "libfolia/folia.h"

using namespace std;
using namespace icu;
using namespace folia;
using namespace icu;

int main() {
  cout << "checking sanity" << endl;
  cout << "Type Hierarchy" << endl;
  print_type_hierarchy( cout );
  cout << "AnnotationType sanity" << endl;
  if ( ! AT_sanity_check() ){
    return EXIT_FAILURE;
  }
  cout << "ElementType sanity" << endl;
  if ( ! ET_sanity_check() ){
    return EXIT_FAILURE;
  }
  cout << "AnnotatorType sanity" << endl;
  if ( ! annotator_sanity_check() ){
    return EXIT_FAILURE;
  }
  cout << "AnnotationType sanity" << endl;
  if ( ! annotation_sanity_check() ){
    return EXIT_FAILURE;
  }
  cout << "Document sanity" << endl;
  if ( !document_sanity_check() ){
    return EXIT_FAILURE;
  }
  cout << "Spaces handling sanity" << endl;
  if ( !space_sanity_check() ){
    return EXIT_FAILURE;
  }

  cout << "Subclass sanity" << endl;
  if ( !subclass_sanity_check() ){
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
