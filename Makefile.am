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

# GNU make options
V					= 0

# Global flags
AM_LIBS					=
AM_LDFLAGS				=
AM_CFLAGS				= -O2 $(CFLAGS_WARNINGS)
AM_CXXFLAGS				= -O2 @AM_CXXFLAGS@
AM_OBJCFLAGS				= -O2 @AM_OBJCFLAGS@
AM_CPPFLAGS				= -O2 \
					  -DHAVE_CONFIG_H=1 \
					  -DHAVE_COMMON_H=1 \
					  -I$(top_builddir)/include \
					  -I$(top_srcdir)/include
ACLOCAL_AMFLAGS				= -I m4 -W all -W error
DEFS					= -DLOCALEDIR="\"$(localedir)\"" @DEFS@

# custom directories

# automake targets
check_PROGRAMS				=
bin_PROGRAMS				=
bin_SCRIPTS				=
include_HEADERS				=
lib_LTLIBRARIES				=
lib_LIBRARIES				=
sbin_SCRIPTS				=
sbin_PROGRAMS				=
noinst_PROGRAMS				=
noinst_HEADERS				= src/common.h \
					  include/bindle_prefix.h
noinst_LTLIBRARIES			=
noinst_LIBRARIES			= lib/libbindle.a
noinst_PROGRAMS				=
man_MANS				=
EXTRA_PROGRAMS				=
EXTRA					= src/bindump \
					  src/bitcopy \
					  src/bitops \
					  src/codetagger \
					  src/colors \
					  src/endian \
					  src/macaddrinfo \
					  src/netcalc \
					  src/numconvert \
					  src/posixregex \
					  src/recurse \
					  src/suicide \
					  src/typedef-info


# lists
AM_INSTALLCHECK_STD_OPTIONS_EXEMPT	=
BUILT_SOURCES				= include/bindle_prefix.h
TESTS					= $(LIBBINDLE_TESTS)
XFAIL_TESTS				=
EXTRA_MANS				=
EXTRA_DIST				= autogen.sh \
					  build-aux/git-tar-name.txt \
					  doc/bindump.1.in \
					  doc/bitops.1.in \
					  doc/macaddrinfo.1.in \
					  doc/numconvert.1.in \
					  doc/posixregex.1.in \
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
CLEANFILES				= $(builddir)/a.out   $(srcdir)/a.out \
					  $(builddir)/*/a.out $(srcdir)/*/a.out \
					  autoscan.log        $(srcdir)/autoscan.log \
					  config.h.in~        $(srcdir)/config.h.in~ \
					  configure.scan      $(srcdir)/configure.scan \
					  $(man_MANS) \
					  include/bindle_prefix.h \
					  @PACKAGE_TARNAME@-*.tar.* \
					  @PACKAGE_TARNAME@-*.zip
DISTCHECK_CONFIGURE_FLAGS		= --enable-strictwarnings \
					  --enable-all


# macros for src/bindump
src_bindump_DEPENDENCIES		= $(lib_LTLIBRARIES) Makefile
src_bindump_CPPFLAGS			= -DPROGRAM_NAME="\"bindump\"" $(AM_CPPFLAGS)
src_bindump_SOURCES			= $(noinst_HEADERS) src/bindump.c
if WANT_BINDUMP
   bin_PROGRAMS				+= src/bindump
   man_MANS				+= doc/bindump.1
endif


# macros for src/bitcopy
src_bitcopy_DEPENDENCIES		= $(lib_LTLIBRARIES) Makefile
src_bitcopy_CPPFLAGS			= -DPROGRAM_NAME="\"bitcopy\"" $(AM_CPPFLAGS)
src_bitcopy_SOURCES			= $(noinst_HEADERS) src/bitcopy.c
if WANT_BITCOPY
   bin_PROGRAMS				+= src/bitcopy
endif


# macros for src/bitops
src_bitops_DEPENDENCIES			= $(lib_LTLIBRARIES) Makefile
src_bitops_CPPFLAGS			= -DPROGRAM_NAME="\"bitops\"" $(AM_CPPFLAGS)
src_bitops_SOURCES			= $(noinst_HEADERS) src/bitops.c
if WANT_BITOPS
   bin_PROGRAMS				+= src/bitops
   man_MANS				+= doc/bitops.1
endif


# macros for src/codetagger
src_codetagger_DEPENDENCIES		= $(lib_LTLIBRARIES) Makefile
src_codetagger_CPPFLAGS			= -DPROGRAM_NAME="\"codetagger\"" $(AM_CPPFLAGS)
src_codetagger_SOURCES			= $(noinst_HEADERS) src/codetagger.c
if WANT_CODETAGGER
   bin_PROGRAMS				+= src/codetagger
   man_MANS				+= doc/codetagger.1
endif


# macros for src/colors
src_colors_DEPENDENCIES			= $(lib_LTLIBRARIES) Makefile
src_colors_CPPFLAGS			= -DPROGRAM_NAME="\"colors\"" $(AM_CPPFLAGS)
src_colors_SOURCES			= $(noinst_HEADERS) src/colors.c
if WANT_COLORS
   bin_PROGRAMS				+= src/colors
endif


# macros for src/endian
src_endian_DEPENDENCIES			= $(lib_LTLIBRARIES) Makefile
src_endian_CPPFLAGS			= -DPROGRAM_NAME="\"endian\"" $(AM_CPPFLAGS)
src_endian_SOURCES			= $(noinst_HEADERS) src/endian.c
if WANT_ENDIAN
   bin_PROGRAMS				+= src/endian
endif


# macros for src/netcalc
src_macaddrinfo_DEPENDENCIES		= $(lib_LTLIBRARIES) Makefile
src_macaddrinfo_CPPFLAGS		= -DPROGRAM_NAME="\"macaddrinfo\"" $(AM_CPPFLAGS)
src_macaddrinfo_SOURCES			= $(noinst_HEADERS) src/macaddrinfo.c
if WANT_MACADDRINFO
   bin_PROGRAMS				+= src/macaddrinfo
   man_MANS				+= doc/macaddrinfo.1
endif


# macros for src/netcalc
src_netcalc_DEPENDENCIES		= $(lib_LTLIBRARIES) Makefile
src_netcalc_CPPFLAGS			= -DPROGRAM_NAME="\"netcalc\"" $(AM_CPPFLAGS)
src_netcalc_SOURCES			= $(noinst_HEADERS) src/netcalc.c
if WANT_NETCALC
   bin_PROGRAMS				+= src/netcalc
endif


# macros for src/numconvert
src_numconvert_DEPENDENCIES		= $(lib_LTLIBRARIES) Makefile
src_numconvert_CPPFLAGS			= -DPROGRAM_NAME="\"numconvert\"" $(AM_CPPFLAGS)
src_numconvert_SOURCES			= $(noinst_HEADERS) src/numconvert.c
if WANT_NUMCONVERT
   bin_PROGRAMS				+= src/numconvert
   man_MANS				+= doc/numconvert.1
endif


# macros for src/posixregex
src_posixregex_DEPENDENCIES		= $(lib_LTLIBRARIES) Makefile
src_posixregex_CPPFLAGS			= -DPROGRAM_NAME="\"posixregex\"" $(AM_CPPFLAGS)
src_posixregex_SOURCES			= $(noinst_HEADERS) src/posixregex.c
if WANT_POSIXREGEX
   bin_PROGRAMS				+= src/posixregex
   man_MANS				+= doc/posixregex.1
endif


# macros for src/suicide
src_suicide_DEPENDENCIES		= $(lib_LTLIBRARIES) Makefile
src_suicide_CPPFLAGS			= -DPROGRAM_NAME="\"suicide\"" $(AM_CPPFLAGS)
src_suicide_SOURCES			= $(noinst_HEADERS) src/suicide.c
if WANT_SUICIDE
   bin_PROGRAMS				+= src/suicide
endif


# macros for src/typedef-info
src_typedef_info_DEPENDENCIES		= $(lib_LTLIBRARIES) Makefile
src_typedef_info_CPPFLAGS		= -DPROGRAM_NAME="\"typedef-info\"" $(AM_CPPFLAGS)
src_typedef_info_SOURCES		= $(noinst_HEADERS) src/typedef-info.c
if WANT_TYPEDEF_INFO
   bin_PROGRAMS				+= src/typedef-info
endif


# Makefile includes
GIT_PACKAGE_VERSION_DIR=include
SUBST_EXPRESSIONS = -e 's,[@]RELEASE_MONTH[@],@RELEASE_MONTH@,g'
include contrib/bindletools/build-aux/makefile-subst.am
include contrib/bindletools/build-aux/makefile-version.am
include contrib/bindletools/build-aux/makefile-libbindle.am
include contrib/bindletools/build-aux/makefile-bindle.am
include contrib/bindletools/build-aux/makefile-autotools.am


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


# local targets
install-exec-local:

install-data-local: 

install-data-hook:

uninstall-local:

clean-local:

distclean-local:
	rm -fR $(srcdir)/autom4te.cache
	rm -fR $(srcdir)/doc/html

# end of automake file
