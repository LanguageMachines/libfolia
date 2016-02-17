[![Build Status](https://travis-ci.org/LanguageMachines/libfolia.svg?branch=master)](https://travis-ci.org/LanguageMachines/libfolia) [![Language Machines Badge](http://applejack.science.ru.nl/lamabadge.php/ticcutils)](http://applejack.science.ru.nl/languagemachines/) 

===================================
Libfolia: FoLiA Library for C++
===================================

    Libfolia (c) CLS/ILK 1998 - 2016
    Centre for Language Studies, Radboud University Nijmegen
    Induction of Linguistic Knowledge Research Group, Tilburg University

**Website:** https://proycon.github.io/folia/

This is a C++ Library, developed by Ko van der Sloot,  for working with the
[Format for Linguistic Annotation (FoLiA)](https://proycon.github.io/folia/), or FoLiA.

libfolia is distributed under the GNU Public Licence v3 (see the file COPYING).

-----------------------------------------------------------------------

This software has been tested on:
- Intel platforms running several versions of Linux, including Ubuntu, Debian,
  Arch Linux, Fedora (both 32 and 64 bits)
- MAC platform running OS X 10.10


Contents of this distribution:
- Sources
- Licensing information ( COPYING )
- Build system based on GNU Autotools

Dependencies:
To be able to succesfully build libfolia from source, you need the following dependencies:
- [ticcutils](https://github.com/LanguageMachines/ticcutils)
- ``libxml2-dev``
- ``libicu-dev``
- A sane C++ build environment with autoconf, automake, autoconf-archive, make, gcc or clang, libtool, pkg-config

To install libfolia, first consult whether your distribution's package manager has an up-to-date package.
If not, for easy installation of libfolia and all dependencies, it is included as part of our software
distribution LaMachine: https://proycon.github.io/LaMachine .

To compile and install manually from source, provided you have all the
dependencies installed:

    $ bash bootstrap.sh
    $ ./configure
    $ make
    $ make install

and, optionally:
    $ make check
