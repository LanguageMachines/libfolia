# $Id: Makefile.am 8053 2011-01-27 16:06:19Z joostvb $
# $URL: https://ilk.uvt.nl/svn/trunk/sources/ucto/trunk/Makefile.am $

ACLOCAL_AMFLAGS = -I m4 --install

SUBDIRS = src include m4

EXTRA_DIST = bootstrap.sh AUTHORS TODO NEWS folia.pc.in

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = folia.pc

ChangeLog: NEWS
	svn update; svn2cl --include-rev
