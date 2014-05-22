/*
  $Id$
  $URL$

  Copyright (c) 1998 - 2014
  ILK   - Tilburg University
  CLiPS - University of Antwerp

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
      http://ilk.uvt.nl/software.html
  or send mail to:
      timbl@uvt.nl
*/

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include "libfolia/document.h"

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
    folia::Document d("file='" + string(argv[1]) + "'"); //,debug='3'");"
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
