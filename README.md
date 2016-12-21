[![Build Status](https://travis-ci.org/LanguageMachines/libfolia.svg?branch=master)](https://travis-ci.org/LanguageMachines/libfolia) [![Language Machines Badge](http://applejack.science.ru.nl/lamabadge.php/ticcutils)](http://applejack.science.ru.nl/languagemachines/) 

===================================
Libfolia: FoLiA Library for C++
===================================

    Libfolia (c) CLS/ILK 2010 - 2017
    Centre for Language Studies, Radboud University Nijmegen
    Induction of Linguistic Knowledge Research Group, Tilburg University

**Website:** https://proycon.github.io/folia/
**Source repository:** https://LanguageMachines.github.io/libfolia
**Contact**: lamasoftware AT science DOT ru DOT nl

This is a C++ Library, developed by Ko van der Sloot, for working with the
[Format for Linguistic Annotation (FoLiA)](https://proycon.github.io/folia/).
The software is intended for C++ developers, and provides a high-level
API to read, manipulate, and create FoLiA documents. This software is a
necessary dependency for various other tools that use FoLiA.

libfolia is distributed under the GNU Public Licence v3 (see the file COPYING).

-----------------------------------------------------------------------
Installation
-----------------------------------------------------------------------

This software has been tested to compile with Gcc or Clang, and runs on the
following platforms:
- Intel platforms running several versions of Linux, including Ubuntu, Debian,
  Arch Linux, Fedora (both 32 and 64 bits)
- MacOS X 10.10 or higher, FreeBSD

*Contents of this distribution:*
- Sources
- Licensing information ( COPYING )
- Build system based on GNU Autotools

*Dependencies:*

To be able to succesfully build libfolia from source, you need the following dependencies:
- [ticcutils](https://github.com/LanguageMachines/ticcutils)
- ``libxml2-dev`` (use your distribution's package manager)
- ``libicu-dev``
- A sane C++ build environment with autoconf, automake, autoconf-archive, make, gcc or clang, libtool, pkg-config

To install libfolia, *first consult whether your distribution's package manager
has an up-to-date package for it*.  If not, for easy installation of libfolia
and all dependencies, it is included as part of our software distribution
LaMachine: https://proycon.github.io/LaMachine .

To compile and install manually from source, provided you have all the
dependencies installed:

    $ bash bootstrap.sh
    $ ./configure
    $ make
    $ make install

and, optionally:
    $ make check

-----------------------------------------------------------------------
Documentation
-----------------------------------------------------------------------

An API reference or tutorial is currently lacking. Contact us if you're
intending to use libfolia and are in need of documentation.

-----------------------------------------------------------------------
Related software
-----------------------------------------------------------------------


This software is developed in parallel to the FoLiA library for Python (part of
[PyNLPl](https://github.com/proycon/pynlpl)), and draws from the same external specification.

Libfolia is used by various in-house projects:
 * [ucto](https://LanguageMachines.github.io/ucto)
 * [frog](https://LanguageMachines.github.io/frog)
 * [foliautils](https://github.com/LanguageMachines/foliautils)
 * [colibri-core](https://proycon.github.io/colibri-core)
 * [wopr](https://github.com/LanguageMachines/wopr)

Libfolia is tested (unit and integration tests) using the following extensive test suite:
 * [foliatest](https://github.com/LanguageMachines/foliatest)

For more information on FoLiA itself, consult the [FoLiA website](https://proycon.github.io/folia/)

-----------------------------------------------------------------------
Acknowledgements
-----------------------------------------------------------------------

This software is funded in the scope of [CLARIAH](http:/www.clariah.nl)
project.
