#
#   DMS Tools and Utilities
#   Copyright (C) 2008, 2009, 2010 David M. Syzdek <david@syzdek.net>
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
#   configure.in - automate compiling on a unix platform
#

AC_PREREQ(2.61)

AC_COPYRIGHT([Copyright (c) 2008, 2009, 2010 David M. Syzdek <david@syzdek.net>.])
AC_REVISION($Revision: 1.13 $)
AC_INIT([DMS Tools and Utilities],[1.1],[david@syzdek.net],[dmstools])

AC_SUBST([RELEASE_MONTH],     ["September 2010"])
AC_SUBST([PACKAGE_COPYRIGHT], ["Copyright (C) 2008, 2009, 2010 David M. Syzdek."])
AC_DEFINE_UNQUOTED(PACKAGE_COPYRIGHT, ["$PACKAGE_COPYRIGHT"], ["Copyright info"])

# Version Info:
#    Current    -- the current version number of this API
#    Revision   -- the revision of the implementation of the API version
#    Age        -- How many seqential past API versions is supported by
#                  this implementation
# Format => Current:Revision:Age
AC_SUBST([LIB_VERSION_CURRENT],  [0])
AC_SUBST([LIB_VERSION_REVISION], [0])
AC_SUBST([LIB_VERSION_AGE],      [0])
AC_SUBST([LIB_VERSION_INFO], [${LIB_VERSION_CURRENT}:${LIB_VERSION_REVISION}:${LIB_VERSION_AGE}])
AC_SUBST([LIB_RELEASE_INFO], [${LIB_VERSION_CURRENT}]) # used by Win32 DLLs

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_SRCDIR([build-aux/dmstools.tags])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# determines host type
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

# configures for automake
AM_INIT_AUTOMAKE(gnu std-options subdir-objects)

# Compilers                                                       
AC_PROG_CC 
#AC_PROG_OBJC
#AC_PROG_CXX
AM_PROG_CC_C_O
AC_PROG_INSTALL
AC_USE_SYSTEM_EXTENSIONS # replaces AC_AIX AC_GNU_SOURCE AC_MINIX
AC_C_BIGENDIAN

# binary locations
AC_PATH_PROG([SHELL],	sh bash,	[AC_MSG_ERROR([missing "sh"])], [$PATH])
AC_CHECK_TOOLS(AR,	ar gar,		[AC_MSG_ERROR([missing binutil "ar"])])
AC_CHECK_TOOLS(LD,	ld gld,		[AC_MSG_ERROR([missing binutil "ld"])])
AC_CHECK_TOOLS(NM,	nm gnm,		[AC_MSG_ERROR([missing binutil "nm"])])
AC_CHECK_TOOLS(RANLIB,	ranlib granlib,	[AC_MSG_ERROR([missing binutil "ranlib"])])
AC_CHECK_TOOLS(STRIP,	strip gstrip,	[AC_MSG_ERROR([missing binutil "strip"])])

# shell programs
AC_PROG_AWK
AC_PROG_EGREP
AC_PROG_FGREP
AM_PROG_LEX
AC_PROG_LN_S

# GNU Libtool Support
AC_LIBTOOL_DLOPEN
AC_DISABLE_FAST_INSTALL
AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL

# custom configure options
AC_DMS_ENABLE_WARNINGS

# sets package version based on the SCM
AC_DMS_GIT_PACKAGE_VERSION

# tests for windows platform
case $host in
   *mingw*) win32_host='yes';;
   *)       win32_host='no';;
esac
AM_CONDITIONAL([IS_WIN32], [test x$win32_host = xyes])

have_regex=yes
AC_SEARCH_LIBS([regcomp],   [regex],,[have_regex=no])
AC_SEARCH_LIBS([regexec],   [regex],,[have_regex=no])
AC_SEARCH_LIBS([regfree],   [regex],,[have_regex=no])
AC_SEARCH_LIBS([regerror],  [regex],,[have_regex=no])
AM_CONDITIONAL([HAVE_REGEX], [test x$have_regex = xyes])

AC_CHECK_LIB([users],[noobs],,[AC_MSG_NOTICE([No noobs found, disabling hand_holding().])])

have_signal=yes
AC_CHECK_HEADERS([signal.h],  ,[have_signal=no])
AC_CHECK_TYPES([sig_t],       ,[have_signal=no],[#include <signal.h>])
AC_SEARCH_LIBS([signal],     ,,[have_signal=no])
AM_CONDITIONAL([HAVE_SIGNAL], [test x$have_signal = xyes])

# Creates outputs
AC_CONFIG_FILES([Makefile])
AC_OUTPUT

# Show local config
AC_MSG_NOTICE([ ])
AC_MSG_NOTICE([   Please send suggestions to:   $PACKAGE_BUGREPORT])
AC_MSG_NOTICE([ ])
AC_MSG_NOTICE([   run `make all'])
AC_MSG_NOTICE([ ])

# end of autoconf script
