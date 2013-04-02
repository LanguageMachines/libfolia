# $Id$
# $URL $

AM_CPPFLAGS = -I@top_srcdir@/include

LDADD = libfolia.la

lib_LTLIBRARIES = libfolia.la
libfolia_la_LDFLAGS = -version-info 1:0:0

libfolia_la_SOURCES =  folia.cxx document.cxx foliautils.cxx

bin_PROGRAMS = folialint
folialint_SOURCES = folialint.cxx

check_PROGRAMS = simpletest
TESTS = $(check_PROGRAMS) 
TESTS_ENVIRONMENT = topsrcdir=$(top_srcdir)
simpletest_SOURCES = simpletest.cxx
CLEANFILES = simpletest.out
