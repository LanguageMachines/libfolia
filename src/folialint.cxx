#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include "folia/folia.h"
#include "folia/document.h"

using namespace std;

void usage(){
  cerr << "usage: folialint <foliafile> [<outputfile>]" << endl;
}

int main( int argc, char* argv[] ){
  string outName;
  if ( argc < 2 ){
    usage();
    exit( EXIT_FAILURE);
  }
  if ( argc == 3 ){
    outName == argv[2];
  }
  try {
    folia::Document d;
    d.readFromFile( argv[1] );
    if ( !outName.empty() )
      d.save( outName );
    else
      cout << d;
  }
  catch( exception& e ){
    cerr << "FAIL: " << e.what() << endl;
    exit( EXIT_FAILURE );
  }
  exit( EXIT_SUCCESS );
}
