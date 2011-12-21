#include <iostream>
#include <string>
#include <map>
#include "folia/document.h"
#include "folia/folia.h"

using namespace std;
using namespace folia;

int main() {
  cout << " Creating a document from scratch: ";
  Document d( "id='example'" );
  d.declare( AnnotationType::TOKEN, "adhocset", "annotator='proycon'" );
  if ( d.defaultset(AnnotationType::TOKEN) != "adhocset" 
       ||
       d.defaultannotator(AnnotationType::TOKEN) != "proycon" ){
    return EXIT_FAILURE;
  }
  string id = d.id() + ".text.1";
  KWargs kw = getArgs( "id='" + id + "'" );
  FoliaElement *text = d.addNode( Text_t, kw );
  kw.clear();
  FoliaElement *s = new Sentence( &d, "generate_id='" + text->id() + "'" );
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
  if ( d[id+".s.1"]->size() != 5 )
    return EXIT_FAILURE;
  UnicodeString txt = s->text();
  if ( txt != "De site staat online ." )
    return EXIT_FAILURE;
  cout << s->text() << endl;
  return EXIT_SUCCESS;
}
