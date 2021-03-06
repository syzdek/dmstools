#
#   DMS Tools and Utilities
#   Copyright (C) 2008, 2013 David M. Syzdek <david@syzdek.net>
#
#   @SYZDEK_LICENSE_HEADER_START@
#
#   Redistribution and use in source and binary forms, with or without
#   modification, are permitted provided that the following conditions are
#   met:
#
#      * Redistributions of source code must retain the above copyright
#        notice, this list of conditions and the following disclaimer.
#      * Redistributions in binary form must reproduce the above copyright
#        notice, this list of conditions and the following disclaimer in the
#        documentation and/or other materials provided with the distribution.
#      * Neither the name of David M. Syzdek nor the
#        names of its contributors may be used to endorse or promote products
#        derived from this software without specific prior written permission.
#
#   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
#   IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
#   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#   PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL DAVID M. SYZDEK BE LIABLE FOR
#   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
#   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
#   SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
#   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
#   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
#   OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
#   SUCH DAMAGE.
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
ACLOCAL_AMFLAGS				= -I m4 -I contrib/bindletools/m4 -W all -W error
DEFS					= -DLOCALEDIR="\"$(localedir)\"" @DEFS@


# automake targets
bin_PROGRAMS				= src/bindump \
					  src/bitcopy \
					  src/bitops \
					  src/colors \
					  src/dmsversion \
					  src/macaddrinfo \
					  src/netcalc \
					  src/numconvert \
					  src/typedef-info
include_HEADERS				= include/dms.h \
					  include/dmsversion.h
lib_LTLIBRARIES				= lib/libdms.la
noinst_PROGRAMS				=
noinst_HEADERS				= src/common.h
man_MANS				= doc/bitops.1 \
					  doc/bindump.1 \
					  doc/codetagger.1 \
					  doc/macaddrinfo.1 \
					  doc/numconvert.1 \
					  doc/posixregex.1


# macros for lib/libotadm.la
lib_libdms_la_DEPENDENCIES		= Makefile lib/libdms.sym
lib_libdms_la_CFLAGS			= $(AM_CFLAGS)
lib_libdms_la_LIBADD			= $(AM_LIBS)
lib_libdms_la_LDFLAGS			= -rpath '$(libdir)' \
					  -version-info $(LIB_VERSION_INFO) \
					  $(AM_LDFLAGS) \
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


# macros for src/endian
src_endian_DEPENDENCIES			= $(lib_LTLIBRARIES) Makefile
src_endian_CPPFLAGS			= -DPROGRAM_NAME="\"endian\"" $(AM_CPPFLAGS)
src_endian_SOURCES			= $(noinst_HEADERS) \
					  src/endian.c
bin_PROGRAMS += src/endian


# macros for src/dmsversion
src_dmsversion_DEPENDENCIES		= $(lib_LTLIBRARIES) Makefile
src_dmsversion_CPPFLAGS			= -DPROGRAM_NAME="\"dmsversion\"" $(AM_CPPFLAGS)
src_dmsversion_LDADD			= $(AM_LDADD) lib/libdms.la
src_dmsversion_SOURCES			= $(noinst_HEADERS) \
					  src/dmsversion.c



# macros for src/netcalc
src_macaddrinfo_DEPENDENCIES		= $(lib_LTLIBRARIES) Makefile
src_macaddrinfo_CPPFLAGS		= -DPROGRAM_NAME="\"macaddrinfo\"" $(AM_CPPFLAGS)
src_macaddrinfo_SOURCES			= $(noinst_HEADERS) \
					  src/macaddrinfo.c


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


# macros for src/typedef-info
src_typedef_info_DEPENDENCIES		= $(lib_LTLIBRARIES) Makefile
src_typedef_info_CPPFLAGS		= -DPROGRAM_NAME="\"typedef-info\"" $(AM_CPPFLAGS)
src_typedef_info_SOURCES		= $(noinst_HEADERS) \
					  src/typedef-info.c


# lists
BUILT_SOURCES				= include/dmsversion.h
TESTS					=
XFAIL_TESTS				=
EXTRA_DIST				= autogen.sh \
					  build-aux/git-tar-name.txt \
					  doc/bindump.1.in \
					  doc/bitops.1.in \
					  doc/macaddrinfo.1.in \
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
					  autoscan.log        $(srcdir)/autoscan.log \
					  config.h.in~        $(srcdir)/config.h.in~ \
					  configure.scan      $(srcdir)/configure.scan \
					  $(man_MANS) \
					  include/dmsversion.h \
					  @PACKAGE_TARNAME@-*.tar.* \
					  @PACKAGE_TARNAME@-*.zip


# Makefile includes
GIT_PACKAGE_VERSION_DIR=include
SUBST_EXPRESSIONS =  \
	-e 's,[@]RELEASE_MONTH[@],@RELEASE_MONTH@,g'
include $(srcdir)/contrib/bindletools/build-aux/makefile-bindle.am
include $(srcdir)/contrib/bindletools/build-aux/makefile-autotools.am
include $(srcdir)/contrib/bindletools/build-aux/makefile-subst.am
include $(srcdir)/contrib/bindletools/build-aux/makefile-version.am

# custom targets
PHONY:

doc/bitops.1: Makefile $(srcdir)/doc/bitops.1.in
	$(do_subst_dt)

doc/bindump.1: Makefile $(srcdir)/doc/bindump.1.in
	$(do_subst_dt)

doc/codetagger.1: Makefile $(srcdir)/doc/codetagger.1.in
	$(do_subst_dt)

doc/macaddrinfo.1: Makefile $(srcdir)/doc/macaddrinfo.1.in
	$(do_subst_dt)

doc/numconvert.1: Makefile $(srcdir)/doc/numconvert.1.in
	$(do_subst_dt)

doc/posixregex.1: Makefile $(srcdir)/doc/posixregex.1.in
	$(do_subst_dt)

include/dmsversion.h: Makefile $(srcdir)/include/dmsversion.h.in
	$(mkdir_p) include
	$(do_subst_dt)


# local targets
install-exec-local:

install-data-local: 

uninstall-local:

clean-local:

distclean-local:
	rm -fR $(srcdir)/autom4te.cache
	rm -fR $(srcdir)/doc/html

# end of automake file
