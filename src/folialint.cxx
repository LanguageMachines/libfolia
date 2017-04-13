/*
  Copyright (c) 2006 - 2017
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
  cerr << "options are" << endl;
  cerr << "\t-h or --help\t\t This help" << endl;
  cerr << "\t-V or --version\t\t Show versions" << endl;
  cerr << "\t--strip\t\t\t strip variable items from the FoLiA. Like all dates." << endl;
  cerr << "\t\t\t\t This is usefull to generate FoLiA that can be diffed." << endl;
  cerr << "\t--output='file'\t\t name an outputfile. (default is stdout)" << endl;
  cerr << "\t--nooutput\t\t Suppress output. Only warnings/errors are displayed." << endl;
  cerr << "\t--checktext check if text is consistent inside structure tags" << endl;
  cerr << "\t--debug=value\t\t Run more verbose." << endl;
  cerr << "\t--permissive\t\t Accept some unwise constructions." << endl;
}

int main( int argc, char* argv[] ){
  string outputName;
  bool permissive;
  bool strip;
  bool nooutput = false;
  bool checktext = false;
  string debug;
  vector<string> fileNames;
  try {
    TiCC::CL_Options Opts( "hV",
			   "checktext,debug:,permissive,strip,output:,nooutput,help,version");
    Opts.init(argc, argv );
    if ( Opts.extract( 'h' )
	 || Opts.extract( "help" ) ){
      usage();
      return EXIT_SUCCESS;
    }
    if ( Opts.extract( 'V' )
	 || Opts.extract( "version" ) ){
      cout << "folialint version 0.4" << endl;
      cout << "based on [" << folia::VersionName() << "]" << endl;
      return EXIT_SUCCESS;
    }
    permissive = Opts.extract("permissive");
    nooutput = Opts.extract("nooutput");
    checktext = Opts.extract("checktext");
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
      string mode;
      if ( permissive ){
	mode = ", mode='permissive";
      }
      else if ( strip ){
	mode = ", mode='strip";
      }
      if ( checktext ){
	if ( mode.empty() ){
	  mode = ", mode='checktext'";
	}
	else {
	  mode += ",checktext'";
	}
      }
      else if ( !mode.empty() ){
	mode += "'";
      }
      cmd += mode;
      folia::Document d( cmd );
      if ( !outputName.empty() ){
	d.save( outputName );
      }
      else if ( !nooutput ){
	cout << d;
      }
    }
    catch( exception& e ){
      cerr << inputName << " failed: " << endl << e.what() << endl;
      //      exit( EXIT_FAILURE );
    }
  }
  exit( EXIT_SUCCESS );
}
