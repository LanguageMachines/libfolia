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

#include <iostream>
#include <string>
#include <map>
#include "ticcutils/StringOps.h"
#include "libfolia/folia.h"

using namespace std;
using namespace icu;
using namespace folia;
using namespace icu;

int main() {
  cout << "checking sanity" << endl;
  cout << "AnnotationType sanity" << endl;
  if ( ! AT_sanity_check() ){
    cout << "too bad. no use to continue" << endl;
    return EXIT_FAILURE;
  }
  cout << "ElementType sanity" << endl;
  if ( ! ET_sanity_check() ){
    cout << "too bad. no use to continue" << endl;
    return EXIT_FAILURE;
  }
  cout << " Creating a document from scratch: ";
  Document d( "id='example'" );
  d.declare( AnnotationType::TOKEN, "adhocset", "annotator='proycon'" );
  if ( d.defaultset(AnnotationType::TOKEN) != "adhocset"
       ||
       d.defaultannotator(AnnotationType::TOKEN) != "proycon" ){
    cout << " Defaultset or defaultannotator does not match" << endl;
    return EXIT_FAILURE;
  }
  string id = d.id() + ".text.1";
  KWargs kw = getArgs( "id='" + id + "'" );
  FoliaElement *text = d.addText( kw );
  kw.clear();
  kw = getArgs( "generate_id='" + text->id() + "'" );
  FoliaElement *s = new Sentence( kw, &d );
  text->append( s );
  kw.clear();
  kw["text"] = "De";
  s->addWord( kw );
  kw["text"] = "site";
  s->addWord( kw );
  kw["text"] = "staat";
  s->addWord( kw );
  kw["text"] = "online";
  s->addWord( kw );
  kw["text"] = ".";
  s->addWord( kw );
  if ( d[id+".s.1"]->size() != 5 ) {
    cout << " Unexpected sentence size, " <<  d[id+".s.1"]->size() << ", expected 5" << endl;
    return EXIT_FAILURE;
  }
  UnicodeString txt = s->text();
  if ( txt != "De site staat online ." ) {
    cout << " Text does not match reference: '" << txt << "' vs reference: 'De site staat online .'" << endl;
    return EXIT_FAILURE;
  }
  cout << s->text() << endl;
  UnicodeString dirty = "    A    dir\ty \n  string\r.\n   ";
  UnicodeString clean = normalize_spaces( dirty );
  UnicodeString wanted = "A dirty string.";
  if ( clean != wanted ){
    cerr << "normalize_space() failed: got:'" << clean << "'"
	 << "                 but expected:'" << wanted << "'" << endl;
  }
  dirty = "\n";
  clean = normalize_spaces( dirty );
  wanted = " ";
  if ( clean != wanted ){
    cerr << "normalize_space() failed: got:'" << clean << "'"
	 << "                 but expected:'" << wanted << "'" << endl;
  }
  dirty = "\r    ";
  clean = normalize_spaces( dirty );
  wanted = " ";
  if ( clean != wanted ){
    cerr << "normalize_space() failed: got:'" << clean << "'"
	 << "                 but expected:'" << wanted << "'" << endl;
  }
  return EXIT_SUCCESS;
}
