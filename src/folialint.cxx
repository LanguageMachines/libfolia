/*
  Copyright (c) 2006 - 2025
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
#include <map>
#include "ticcutils/CommandLine.h"
#include "libfolia/folia.h"

using namespace std;

void usage(){
  cerr << "usage: folialint [options] <foliafiles>" << endl;
  cerr << "options are" << endl;
  cerr << "\t-h, --help\t\t This help" << endl;
  cerr << "\t-V, --version\t\t Show versions" << endl;
  cerr << "\t--strip\t\t\t strip variable items from the FoLiA. Like all dates." << endl;
  cerr << "\t\t\t\t This is usefull to generate FoLiA that can be diffed." << endl;
  cerr << "\t-o or --output='file'\t\t name an outputfile. (default is stdout)" << endl;
  cerr << "\t--nooutput\t\t Suppress output. Only warnings/errors are displayed." << endl;
  cerr << "\t--nochecktext\t\t DO NOT check if text is consistent inside structure tags." << endl;
  cerr << "\t\t\t\t Default is to do so." << endl;
  cerr << "\t--fixtext.\t\t Try to fixup text errors like wrong offsets." << endl;
  cerr << "\t\t\t\t Default is to DON'T DO THAT." << endl;
  cerr << "\t-a --autodeclare\t Attempt to automaticly fix missing annotations." << endl;
  cerr << "\t\t\t\t (default: false)" << endl;
  cerr << "\t-x --explicit\t\t output explicit FoLiA. (default: false)" << endl;
  cerr << "\t--permissive.\t\t Allow some dubious constructs." << endl;
  cerr << "\t--warn\t\t\t add some extra warnings about library versions and unused" << endl;
  cerr << "\t\t\t\t annotation declarations" << endl;
  cerr << "\t-c --canonical\t\t output in a predefined order. Makes comparisons easier" << endl;
  cerr << "\t-d value, --debug=value\t Run more verbose." << endl;
  cerr << "\t--permissive\t\t Accept some unwise constructions." << endl;
}

int main( int argc, const char* argv[] ){
  string outputName;
  bool permissive;
  bool warn;
  bool strip ;
  bool nooutput = false;
  bool nochecktext = false;
  bool fixtext = false;
  bool kanon = false;
  bool autodeclare = false;
  bool do_explicit = false;
  string debug;
  vector<string> fileNames;
  string command;
  try {
    TiCC::CL_Options Opts( "hVd:acxo:",
			   "nochecktext,debug:,permissive,strip,output:,"
			   "nooutput,help,fixtext,warn,version,canonical,"
			   "explicit,autodeclare");
    Opts.init(argc, argv );
    if ( Opts.extract( 'h' )
	 || Opts.extract( "help" ) ){
      usage();
      return EXIT_SUCCESS;
    }
    if ( Opts.extract( 'V' )
	 || Opts.extract( "version" ) ){
      cout << "folialint version 0.5" << endl;
      cout << "based on [" << folia::VersionName() << "]" << endl;
      return EXIT_SUCCESS;
    }
    command = Opts.toString();
    if ( !command.empty() ){
      command = "folialint " + command;
    }
    else {
      command = "folialint";
    }
    permissive = Opts.extract("permissive");
    do_explicit = ( Opts.extract("explicit") || Opts.extract('x') );
    warn = Opts.extract("warn");
    nooutput = Opts.extract("nooutput");
    fixtext = Opts.extract("fixtext");
    kanon = Opts.extract("canonical") || Opts.extract("KANON");
    if ( Opts.extract("nochecktext") ){
      nochecktext = true;
    }
    strip = Opts.extract("strip");
    if ( strip && permissive ){
      cerr << "conflicting options: 'permissive' and 'strip'" << endl;
      return EXIT_FAILURE;
    }
    Opts.extract( "debug", debug ) || Opts.extract( 'd', debug );
    Opts.extract( "output", outputName ) || Opts.extract( 'o', outputName );
    autodeclare = Opts.extract( "autodeclare" ) || Opts.extract( 'a' );

    if ( !Opts.empty() ){
      cerr << "unsupported option(s): " << Opts.toString() << endl;
      return EXIT_FAILURE;
    }
    fileNames = Opts.getMassOpts();
    if ( fileNames.size() == 0 ){
      cerr << "missing input file" << endl;
      usage();
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
  catch( const exception& e ){
    cerr << "FAIL: " << e.what() << endl;
    exit( EXIT_FAILURE );
  }

  int fail_count = 0;
  string mode;
  if ( permissive ){
    mode += ",permissive";
  }
  if ( strip ){
    mode += ",strip";
  }
  if ( fixtext ){
    mode += ",fixtext";
  }
  if ( do_explicit ){
    mode += ",explicit";
  }
  if ( autodeclare ){
    mode += ",autodeclare";
  }
  else {
    mode += ",noautodeclare"; // the default
  }
  if ( nochecktext ){
    mode += ",nochecktext";
  }
  else {
    mode += ",checktext";	// the default
  }
  if ( !mode.empty() ){
    mode = ", mode='" + mode + "'";
  }
  if ( !debug.empty() ){
    mode += ", debug='" + debug + "'";
  }
  for ( const auto& inputName : fileNames ){
    try {
      string cmd = "file='" + inputName + "'";
      cmd += mode;
      //      cerr << "running " << cmd << endl;
      folia::Document d( cmd );
      if ( !d.version_below(2,0)
	   && !(kanon||strip)
	   && d.get_processors_by_name( "folialint" ).empty() ){
	folia::KWargs args;
	args.add("name","folialint");
	args.add("id","folialint");
	args.add("generator","yes");
	args.add("begindatetime","now()");
	args.add("command",command);
	folia::processor *proc = d.add_processor( args );
	proc->get_system_defaults();
	proc->set_metadata( "valid", "yes" );
      }
      if ( !outputName.empty() ){
	d.save( outputName, kanon );
      }
      else if ( !nooutput ){
	d.set_canonical(kanon);
	cout << d;
      }
      else {
	cerr << "Validated successfully: " << inputName << endl;
      }
      if ( warn ){
	if ( d.compare_to_build_version() ){
	  cerr << "WARNING: the document had version: " << d.version()
	       << " and the library is at version: "
	       <<  folia::folia_version() << endl;
	}
	multimap<folia::AnnotationType, string> und = d.unused_declarations();
	if ( !und.empty() ){
	  cerr << "the following annotationsets are declared but unused: " << endl;
	  for ( const auto& [ann,sett] : und ){
	    cerr << folia::toString( ann )<< "-annotation, set=" << sett << endl;
	  }
	}
      }
    }
    catch( const exception& e ){
      cerr << e.what() << endl;
      ++fail_count;
    }
  }
  if ( fail_count > 0 ){
    exit( EXIT_FAILURE );
  }
  exit( EXIT_SUCCESS );
}
