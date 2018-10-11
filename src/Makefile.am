AM_CPPFLAGS = -I@top_srcdir@/include
AM_CXXFLAGS = -std=c++11 -g -O3 -W -Wall -pedantic


LDADD = libfolia.la

lib_LTLIBRARIES = libfolia.la
libfolia_la_LDFLAGS = -version-info 9:0:0

libfolia_la_SOURCES = folia_impl.cxx folia_document.cxx folia_utils.cxx \
	folia_types.cxx folia_properties.cxx \
	folia_builder.cxx folia_processor.cxx

bin_PROGRAMS = folialint
folialint_SOURCES = folialint.cxx

bin_SCRIPTS = foliadiff.sh

check_PROGRAMS = simpletest
TESTS = $(check_PROGRAMS)
simpletest_SOURCES = simpletest.cxx
CLEANFILES = simpletest.out

EXTRA_DIST = foliadiff.sh
