AM_CPPFLAGS = -I@top_srcdir@/include
AM_CXXFLAGS = -std=c++0x -g -O3 # -Weffc++


LDADD = libfolia.la

lib_LTLIBRARIES = libfolia.la
libfolia_la_LDFLAGS = -version-info 5:0:0

libfolia_la_SOURCES = folia_impl.cxx folia_document.cxx folia_utils.cxx \
	folia_types.cxx folia_properties.cxx

bin_PROGRAMS = folialint
folialint_SOURCES = folialint.cxx

check_PROGRAMS = simpletest
TESTS = $(check_PROGRAMS)
simpletest_SOURCES = simpletest.cxx
CLEANFILES = simpletest.out
