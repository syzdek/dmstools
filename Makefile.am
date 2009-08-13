#
#   DMS Tools and Utilities
#   Copyright (C) 2008 David M. Syzdek <david@syzdek.net>
#
#   @SYZDEK_LICENSE_HEADER_START@
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License Version 2 as
#   published by the Free Software Foundation.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
#
#   @SYZDEK_LICENSE_HEADER_END@
#
#   Makefile.am - automate compiling on a unix platform
#

# Sub directories
SUBDIRS					= po
DIST_SUBDIRS				= po


# Global flags
AM_LIBS					= @LTLIBINTL@
AM_LDFLAGS				= @LTLIBINTL@
AM_CFLAGS				= -O2 -W -Wall -Werror
AM_CXXFLAGS				= -O2 -W -Wall -Werror
AM_OBJCFLAGS				= -O2 -W -Wall -Werror
AM_CPPFLAGS				= -O2 -W -Wall -Werror -DHAVE_COMMON_H=1 -I$(top_srcdir)/include
ACLOCAL_AMFLAGS				= -I m4
DEFS					= -DLOCALEDIR="\"$(localedir)\"" @DEFS@


# automake targets
bin_PROGRAMS				= src/bitops src/codetagger src/numconvert src/posixregex
noinst_PROGRAMS				= src/recurse
noinst_HEADERS				= src/common.h
man_MANS				= doc/codetagger.1


# macros for src/bitops
src_bitops_DEPENDENCIES			= $(lib_LTLIBRARIES) Makefile
src_bitops_CPPFLAGS			= -DPROGRAM_NAME="\"bitops\"" $(AM_CPPFLAGS)
src_bitops_SOURCES			= $(noinst_HEADERS) \
					  src/bitops.c


# macros for src/codetagger
src_codetagger_DEPENDENCIES		= $(lib_LTLIBRARIES) Makefile
src_codetagger_CPPFLAGS			= -DPROGRAM_NAME="\"codetagger\"" $(AM_CPPFLAGS)
src_codetagger_SOURCES			= $(noinst_HEADERS) \
					  src/codetagger.c


# macros for src/numconvert
src_numconvert_DEPENDENCIES		= $(lib_LTLIBRARIES) Makefile
src_numconvert_CPPFLAGS			= -DPROGRAM_NAME="\"numconvert\"" $(AM_CPPFLAGS)
src_numconvert_SOURCES			= $(noinst_HEADERS) \
					  src/numconvert.c


# macros for src/posixregex
src_posixregex_DEPENDENCIES		= $(lib_LTLIBRARIES) Makefile
src_posixregex_CPPFLAGS			= -DPROGRAM_NAME="\"posixregex\"" $(AM_CPPFLAGS)
src_posixregex_SOURCES			= $(noinst_HEADERS) \
					  src/posixregex.c


# macros for src/posixregex
src_recurse_DEPENDENCIES		= $(lib_LTLIBRARIES) Makefile
src_recurse_CPPFLAGS			= -DPROGRAM_NAME="\"recurse\"" $(AM_CPPFLAGS)
src_recurse_SOURCES			= $(noinst_HEADERS) \
					  src/recurse.c


# macros for src/codetagger
#src_vaportar_DEPENDENCIES		= $(lib_LTLIBRARIES) Makefile
#src_vaportar_CPPFLAGS			= -DPROGRAM_NAME="\"vaportar\"" $(AM_CPPFLAGS)
#src_vaportar_SOURCES			= $(noinst_HEADERS) \
#					  src/vaportar.c


# lists
TESTS					=
XFAIL_TESTS				=
EXTRA_DIST				= build-aux/config.rpath \
					  build-aux/git-package-version \
					  include/gettext.h \
					  $(srcdir)/dmstools.tags \
					  $(srcdir)/doc/Doxyfile \
					  $(srcdir)/doc/codetagger.1.in \
					  $(srcdir)/examples/Makefile \
					  $(srcdir)/examples/example-c.tags \
					  $(srcdir)/examples/example-defaults.tags \
					  $(srcdir)/examples/example-html.tags \
					  $(srcdir)/examples/example.c \
					  $(srcdir)/examples/example.html \
					  $(srcdir)/examples/example.txt \
					  $(srcdir)/Makefile-xcode \
					  $(srcdir)/dmstools.xcodeproj/project.pbxproj
CLEANFILES				= \
					  $(builddir)/a.out   $(srcdir)/a.out \
					  $(builddir)/*/a.out $(srcdir)/*/a.out \
					  config.h.in~ $(srcdir)/config.h.in~ \
					  $(man_MANS) \
					  @PACKAGE_TARNAME@-*.tar.* \
					  @PACKAGE_TARNAME@-*.zip


# substitution routine
do_subst = sed \
	-e 's,[@]SHELL[@],@SHELL@,g' \
	-e 's,[@]PACKAGE_VERSION[@],@PACKAGE_VERSION@,g' \
	-e 's![@]PACKAGE_NAME[@]!@PACKAGE_NAME@!g' \
	-e 's,[@]PACKAGE_TARNAME[@],@PACKAGE_TARNAME@,g' \
	-e 's,[@]PACKAGE_BUGREPORT[@],@PACKAGE_BUGREPORT@,g' \
	-e 's|[@]PACKAGE_COPYRIGHT[@]|@PACKAGE_COPYRIGHT@|g' \
	-e 's,[@]RELEASE_MONTH[@],@RELEASE_MONTH@,g'
do_subst_fn = \
		echo "do_subst < ${@}.in > ${@}"; \
		mkdir -p `dirname ${@}` || exit 1; \
		${do_subst} < $(srcdir)/${@}.in > ${@} || exit 1; \
		chmod 0755 ${@}


# custom targets
build-aux/git-package-version:
	mkdir -p build-aux
	echo $(PACKAGE_VERSION) > $(srcdir)/build-aux/git-package-version

doc/codetagger.1: Makefile $(srcdir)/doc/codetagger.1.in
	@$(do_subst_fn)

license-tags: $(srcdir)/dmstools.tags $(builddir)/src/codetagger
	$(builddir)/src/codetagger -f $(srcdir)/dmstools.tags $(srcdir)/AUTHORS
	$(builddir)/src/codetagger -f $(srcdir)/dmstools.tags $(srcdir)/acinclude.m4
	$(builddir)/src/codetagger -f $(srcdir)/dmstools.tags $(srcdir)/configure.ac
	$(builddir)/src/codetagger -f $(srcdir)/dmstools.tags $(srcdir)/ChangeLog
	$(builddir)/src/codetagger -f $(srcdir)/dmstools.tags $(srcdir)/doc/codetagger.1.in
	$(builddir)/src/codetagger -f $(srcdir)/dmstools.tags $(srcdir)/Makefile.am
	$(builddir)/src/codetagger -f $(srcdir)/dmstools.tags $(srcdir)/NEWS
	$(builddir)/src/codetagger -f $(srcdir)/dmstools.tags $(srcdir)/README
	$(builddir)/src/codetagger -f $(srcdir)/dmstools.tags $(srcdir)/src/codetagger.c
	$(builddir)/src/codetagger -f $(srcdir)/dmstools.tags $(srcdir)/src/common.h
	$(builddir)/src/codetagger -f $(srcdir)/dmstools.tags $(srcdir)/dmstools.tags
	$(builddir)/src/codetagger -f $(srcdir)/dmstools.tags $(srcdir)/TODO

# local targets
install-exec-local:

install-data-local: 

uninstall-local:

distclean-local:
	rm -fR $(srcdir)/autom4te.cache
	rm -fR $(srcdir)/doc/html

# end of automake file
