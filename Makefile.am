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
SUBDIRS					=
DIST_SUBDIRS				=


# Global flags
AM_LIBS					=
AM_LDFLAGS				=
AM_CFLAGS				= -O2 -W -Wall -Werror $(CFLAGS_WARNINGS)
AM_CXXFLAGS				= -O2 -W -Wall -Werror
AM_OBJCFLAGS				= -O2 -W -Wall -Werror
AM_CPPFLAGS				= -O2 -W -Wall -Werror -DHAVE_COMMON_H=1 -I$(top_builddir)/include -I$(top_srcdir)/include
ACLOCAL_AMFLAGS				= -I m4
DEFS					= -DLOCALEDIR="\"$(localedir)\"" @DEFS@


# automake targets
bin_PROGRAMS				= src/bindump src/bitcopy src/bitops src/colors src/dmsversion src/netcalc src/numconvert src/typedef-info
include_HEADERS				= include/dms.h include/dmsversion.h
lib_LTLIBRARIES				= lib/libdms.la
noinst_PROGRAMS				=
noinst_HEADERS				= src/common.h
man_MANS				= doc/bitops.1 doc/bindump.1 doc/codetagger.1 doc/numconvert.1 doc/posixregex.1


# macros for lib/libotadm.la
lib_libdms_la_DEPENDENCIES		= Makefile lib/libdms.sym
lib_libdms_la_CFLAGS			= $(AM_CFLAGS)
lib_libdms_la_LIBADD			= $(AM_LIBS)
lib_libdms_la_LDFLAGS			= -rpath '$(libdir)' \
					  -version-info $(LIB_VERSION_INFO) \
					  $(AM_LDFLAGS) \
					  -pthread \
					  -export-symbols $(srcdir)/lib/libdms.sym
lib_libdms_la_SOURCES			= include/dms.h \
					  include/dmsversion.h \
					  lib/libdms.h \
					  lib/libdms_version.c \
					  lib/libdms_version.h


# macros for src/bindump
src_bindump_DEPENDENCIES		= $(lib_LTLIBRARIES) Makefile
src_bindump_CPPFLAGS			= -DPROGRAM_NAME="\"bindump\"" $(AM_CPPFLAGS)
src_bindump_SOURCES			= $(noinst_HEADERS) \
					  src/bindump.c


# macros for src/bitops
src_bitcopy_DEPENDENCIES		= $(lib_LTLIBRARIES) Makefile
src_bitcopy_CPPFLAGS			= -DPROGRAM_NAME="\"bitcopy\"" $(AM_CPPFLAGS)
src_bitcopy_SOURCES			= $(noinst_HEADERS) \
					  src/bitcopy.c


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
if ! IS_WIN32
bin_PROGRAMS += src/codetagger
endif


# macros for src/colors
src_colors_DEPENDENCIES			= $(lib_LTLIBRARIES) Makefile
src_colors_CPPFLAGS			= -DPROGRAM_NAME="\"colors\"" $(AM_CPPFLAGS)
src_colors_SOURCES			= $(noinst_HEADERS) \
					  src/colors.c


# macros for src/dmsversion
src_dmsversion_DEPENDENCIES		= $(lib_LTLIBRARIES) Makefile
src_dmsversion_CPPFLAGS			= -DPROGRAM_NAME="\"dmsversion\"" $(AM_CPPFLAGS)
src_dmsversion_LDADD			= $(AM_LDADD) lib/libdms.la
src_dmsversion_SOURCES			= $(noinst_HEADERS) \
					  src/dmsversion.c


# macros for src/netcalc
src_netcalc_DEPENDENCIES		= $(lib_LTLIBRARIES) Makefile
src_netcalc_CPPFLAGS			= -DPROGRAM_NAME="\"netcalc\"" $(AM_CPPFLAGS)
src_netcalc_SOURCES			= $(noinst_HEADERS) \
					  src/netcalc.c


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
if HAVE_REGEX
bin_PROGRAMS += src/posixregex
endif


# macros for src/suicide
src_suicide_DEPENDENCIES		= $(lib_LTLIBRARIES) Makefile
src_suicide_CPPFLAGS			= -DPROGRAM_NAME="\"suicide\"" $(AM_CPPFLAGS)
src_suicide_SOURCES			= $(noinst_HEADERS) \
					  src/suicide.c
if HAVE_SIGNAL
bin_PROGRAMS += src/suicide
endif


# macros for src/posixregex
src_recurse_DEPENDENCIES		= $(lib_LTLIBRARIES) Makefile
src_recurse_CPPFLAGS			= -DPROGRAM_NAME="\"recurse\"" $(AM_CPPFLAGS)
src_recurse_SOURCES			= $(noinst_HEADERS) \
					  src/recurse.c
if ! IS_WIN32
bin_PROGRAMS += src/recurse
endif

# macros for src/typedef-info
src_typedef_info_DEPENDENCIES		= $(lib_LTLIBRARIES) Makefile
src_typedef_info_CPPFLAGS		= -DPROGRAM_NAME="\"typedef-info\"" $(AM_CPPFLAGS)
src_typedef_info_SOURCES		= $(noinst_HEADERS) \
					  src/typedef-info.c


# lists
BUILT_SOURCES				= include/dmsversion.h
TESTS					=
XFAIL_TESTS				=
EXTRA_DIST				= build-aux/git-package-version \
					  doc/bindump.1.in \
					  doc/bitops.1.in \
					  doc/numconvert.1.in \
					  doc/posixregex.1.in \
					  include/dmsversion.h.in \
					  lib/libdms.sym \
					  m4/dmstools_gcc.m4 \
					  m4/dmstools_git.m4 \
					  $(srcdir)/build-aux/dmstools.tags \
					  $(srcdir)/doc/Doxyfile \
					  $(srcdir)/doc/codetagger.1.in \
					  $(srcdir)/examples/Makefile \
					  $(srcdir)/examples/example-c.tags \
					  $(srcdir)/examples/example-defaults.tags \
					  $(srcdir)/examples/example-html.tags \
					  $(srcdir)/examples/example.c \
					  $(srcdir)/examples/example.html \
					  $(srcdir)/examples/example.txt \
					  $(srcdir)/build-aux/Makefile-xcode \
					  $(srcdir)/dmstools.xcodeproj/project.pbxproj
CLEANFILES				= \
					  $(builddir)/a.out   $(srcdir)/a.out \
					  $(builddir)/*/a.out $(srcdir)/*/a.out \
					  config.h.in~ $(srcdir)/config.h.in~ \
					  $(man_MANS) \
					  include/dmsversion.h \
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
	-e 's,[@]LIB_VERSION_CURRENT[@],@LIB_VERSION_CURRENT@,g' \
	-e 's,[@]LIB_VERSION_REVISION[@],@LIB_VERSION_REVISION@,g' \
	-e 's,[@]LIB_VERSION_AGE[@],@LIB_VERSION_AGE@,g' \
	-e 's,[@]LIB_VERSION_INFO[@],@LIB_VERSION_INFO@,g' \
	-e 's,[@]LIB_RELEASE_INFO[@],@LIB_RELEASE_INFO@,g' \
	-e 's,[@]RELEASE_MONTH[@],@RELEASE_MONTH@,g'
do_subst_fn = \
		echo "do_subst < ${@}.in > ${@}"; \
		mkdir -p `dirname ${@}` || exit 1; \
		${do_subst} < $(srcdir)/${@}.in > ${@} || exit 1; \
		chmod 0755 ${@}
do_subst_dt = \
		echo "do_subst < ${@}.in > ${@}"; \
		mkdir -p `dirname ${@}` || exit 1; \
		${do_subst} < $(srcdir)/${@}.in > ${@} || exit 1; \
		chmod 0644 ${@}


# custom targets
PHONY: git-package-version-check

git-package-version-check:
	@if test -f $(srcdir)/.git/config;then \
		GPV=`git --git-dir=$(srcdir)/.git describe 2> /dev/null`; \
		GPV=`echo $${GPV}|sed -e 's/-/./g' -e 's/^v//g'`; \
		if test "x$${GPV}" != "x$(PACKAGE_VERSION)" && test "x$${GPV}" != "x";then \
			$(top_builddir)/config.status --recheck; \
			$(top_builddir)/config.status; \
		fi; \
	fi

$(srcdir)/acinclude.m4: git-package-version-check

doc/bitops.1: Makefile $(srcdir)/doc/bitops.1.in
	@$(do_subst_dt)

doc/bindump.1: Makefile $(srcdir)/doc/bindump.1.in
	@$(do_subst_dt)

build-aux/git-package-version:
	$(top_builddir)/config.status --recheck

doc/codetagger.1: Makefile $(srcdir)/doc/codetagger.1.in
	@$(do_subst_dt)

doc/numconvert.1: Makefile $(srcdir)/doc/numconvert.1.in
	@$(do_subst_dt)

doc/posixregex.1: Makefile $(srcdir)/doc/posixregex.1.in
	@$(do_subst_dt)

include/dmsversion.h: Makefile $(srcdir)/include/dmsversion.h.in
	$(mkdir_p) include
	@$(do_subst_dt)

license-tags: $(srcdir)/build-aux/dmstools.tags $(builddir)/src/codetagger
	$(builddir)/src/codetagger -f $(srcdir)/build-aux/dmstools.tags $(srcdir)/AUTHORS
	$(builddir)/src/codetagger -f $(srcdir)/build-aux/dmstools.tags $(srcdir)/acinclude.m4
	$(builddir)/src/codetagger -f $(srcdir)/build-aux/dmstools.tags $(srcdir)/configure.ac
	$(builddir)/src/codetagger -f $(srcdir)/build-aux/dmstools.tags $(srcdir)/ChangeLog
	$(builddir)/src/codetagger -f $(srcdir)/build-aux/dmstools.tags $(srcdir)/doc/codetagger.1.in
	$(builddir)/src/codetagger -f $(srcdir)/build-aux/dmstools.tags $(srcdir)/Makefile.am
	$(builddir)/src/codetagger -f $(srcdir)/build-aux/dmstools.tags $(srcdir)/NEWS
	$(builddir)/src/codetagger -f $(srcdir)/build-aux/dmstools.tags $(srcdir)/README
	$(builddir)/src/codetagger -f $(srcdir)/build-aux/dmstools.tags $(srcdir)/src/codetagger.c
	$(builddir)/src/codetagger -f $(srcdir)/build-aux/dmstools.tags $(srcdir)/src/common.h
	$(builddir)/src/codetagger -f $(srcdir)/build-aux/dmstools.tags $(srcdir)/dmstools.tags
	$(builddir)/src/codetagger -f $(srcdir)/build-aux/dmstools.tags $(srcdir)/TODO

# local targets
install-exec-local:

install-data-local: 

uninstall-local:

distclean-local:
	rm -fR $(srcdir)/autom4te.cache
	rm -fR $(srcdir)/doc/html

# end of automake file
