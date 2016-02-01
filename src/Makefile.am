# $Id$
# $URL $

AM_CPPFLAGS = -I@top_srcdir@/include
AM_CXXFLAGS = -std=c++0x -g

LDADD = libfolia.la

lib_LTLIBRARIES = libfolia.la
libfolia_la_LDFLAGS = -version-info 3:1:0

libfolia_la_SOURCES = folia.cxx document.cxx foliautils.cxx \
	foliatypes.cxx foliaproperties.cxx

bin_PROGRAMS = folialint
folialint_SOURCES = folialint.cxx

check_PROGRAMS = simpletest
TESTS = $(check_PROGRAMS)
#TESTS_ENVIRONMENT = topsrcdir=$(top_srcdir)
simpletest_SOURCES = simpletest.cxx
CLEANFILES = simpletest.out
