/*
  Copyright (c) 2006 - 2016
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
#include <fstream>
#include <string>
#include "ticcutils/CommandLine.h"
#include "libfolia/folia.h"

using namespace std;

void usage(){
  cerr << "usage: folialint [options] <foliafile> [<outputfile>]" << endl;
}

int main( int argc, char* argv[] ){
  string inputName;
  string outputName;
  bool permissive;
  string debug;
  vector<string> fileNames;
  try {
    TiCC::CL_Options Opts( "hV", "debug:,permissive");
    Opts.init(argc, argv );
    if ( Opts.extract( 'h' ) ){
      usage();
      return EXIT_SUCCESS;
    }
    if ( Opts.extract( 'V' ) ){
      cout << "folialint version 0.2" << endl;
      cout << "based on [" << folia::VersionName() << "]" << endl;
      return EXIT_SUCCESS;
    }
    permissive = Opts.extract("permissive");
    Opts.extract( "debug", debug );
    if ( !Opts.empty() ){
      cerr << "unsupported option(s): " << Opts.toString() << endl;
      return EXIT_FAILURE;
    }
    fileNames = Opts.getMassOpts();
    if ( fileNames.size() == 0 ){
      cerr << "missing input file" << endl;
      return EXIT_FAILURE;
    }
    if ( fileNames.size() > 2 ){
      cerr << "expected 1 input file, and 1 output file" << endl;
      return EXIT_FAILURE;
    }
    inputName = fileNames[0];
    if ( fileNames.size() == 2 ){
      outputName = fileNames[1];
    }
    if ( inputName == outputName ){
      cerr << "output name cannot be same as input name!" << endl;
      return EXIT_FAILURE;
    }
  }
  catch( exception& e ){
    cerr << "FAIL: " << e.what() << endl;
    exit( EXIT_FAILURE );
  }
  try {
    string cmd = "file='" + inputName + "'";
    if ( !debug.empty() )
      cmd += ", debug='" + debug + "'";
    if ( permissive )
      cmd += ", mode='permissive'";
    folia::Document d( cmd );
    if ( !outputName.empty() )
      d.save( outputName );
    else
      cout << d;
  }
  catch( exception& e ){
    cerr << "FAIL: " << e.what() << endl;
    exit( EXIT_FAILURE );
  }
  exit( EXIT_SUCCESS );
}
