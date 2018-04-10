ACLOCAL_AMFLAGS = -I m4 --install

SUBDIRS = src include m4

EXTRA_DIST = bootstrap.sh AUTHORS TODO NEWS README.md folia.pc.in

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = folia.pc

ChangeLog: NEWS
	git pull; git2cl > ChangeLog
