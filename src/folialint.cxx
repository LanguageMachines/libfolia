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
  cerr << "usage: folialint [options] <foliafiles>" << endl;
}

int main( int argc, char* argv[] ){
  string outputName;
  bool permissive;
  bool strip;
  bool nooutput = false;
  string debug;
  vector<string> fileNames;
  try {
    TiCC::CL_Options Opts( "hV", "debug:,permissive,strip,output:,nooutput");
    Opts.init(argc, argv );
    if ( Opts.extract( 'h' ) ){
      usage();
      return EXIT_SUCCESS;
    }
    if ( Opts.extract( 'V' ) ){
      cout << "folialint version 0.3" << endl;
      cout << "based on [" << folia::VersionName() << "]" << endl;
      return EXIT_SUCCESS;
    }
    permissive = Opts.extract("permissive");
    nooutput = Opts.extract("nooutput");
    strip = Opts.extract("strip");
    if ( strip && permissive ){
      cerr << "conflicting options: 'permissive' and 'strip'" << endl;
      return EXIT_FAILURE;
    }
    Opts.extract( "debug", debug );
    Opts.extract("output", outputName);
    if ( !Opts.empty() ){
      cerr << "unsupported option(s): " << Opts.toString() << endl;
      return EXIT_FAILURE;
    }
    fileNames = Opts.getMassOpts();
    if ( fileNames.size() == 0 ){
      cerr << "missing input file" << endl;
      return EXIT_FAILURE;
    }
    if ( fileNames.size() > 1 && !outputName.empty() ){
      cerr << "--outputname not supported for more then 1 inputfile" << endl;
      return EXIT_FAILURE;
    }
    string inputName = fileNames[0];
    if ( inputName == outputName ){
      cerr << "output name cannot be same as input name!" << endl;
      return EXIT_FAILURE;
    }
  }
  catch( exception& e ){
    cerr << "FAIL: " << e.what() << endl;
    exit( EXIT_FAILURE );
  }
  for ( const auto& inputName : fileNames ){
    try {
      string cmd = "file='" + inputName + "'";
      if ( !debug.empty() )
	cmd += ", debug='" + debug + "'";
      if ( permissive )
	cmd += ", mode='permissive'";
      else if ( strip )
	cmd += ", mode='strip'";
      folia::Document d( cmd );
      if ( !outputName.empty() ){
	d.save( outputName );
      }
      else if ( !nooutput ){
	cout << d;
      }
    }
    catch( exception& e ){
      cerr << "FAIL: " << e.what() << endl;
      exit( EXIT_FAILURE );
    }
  }
  exit( EXIT_SUCCESS );
}
