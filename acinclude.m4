#
#   DMS Tools and Utilities
#   Copyright (C) 2014 David M. Syzdek <david@syzdek.net>
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
#   acinclude.m4 - custom m4 macros used by configure.ac
#

# AC_DMS_WIN32
# ______________________________________________________________________________
AC_DEFUN([AC_DMS_WIN32],[dnl

   # prerequists
   AC_REQUIRE([AC_PROG_CC])

   # tests for windows platform
   case $host in
      *mingw*) win32_host='yes';;
      *)       win32_host='no';;
   esac

   AM_CONDITIONAL([IS_WIN32], [test x$win32_host = xyes])
])dnl


# AC_DMS_REGEX
# ______________________________________________________________________________
AC_DEFUN([AC_DMS_REGEX],[dnl

   # prerequists
   AC_REQUIRE([AC_PROG_CC])

   have_regex=yes
   AC_SEARCH_LIBS([regcomp],   [regex],,[have_regex=no])
   AC_SEARCH_LIBS([regexec],   [regex],,[have_regex=no])
   AC_SEARCH_LIBS([regfree],   [regex],,[have_regex=no])
   AC_SEARCH_LIBS([regerror],  [regex],,[have_regex=no])

   AM_CONDITIONAL([HAVE_REGEX], [test x$have_regex = xyes])
])dnl


# AC_DMS_SIGNAL
# ______________________________________________________________________________
AC_DEFUN([AC_DMS_SIGNAL],[dnl

   # prerequists
   AC_REQUIRE([AC_PROG_CC])

   have_signal=yes
   AC_CHECK_HEADERS([signal.h],  ,[have_signal=no])
   AC_CHECK_TYPES([sig_t],       ,[have_signal=no],[#include <signal.h>])
   AC_SEARCH_LIBS([signal],     ,,[have_signal=no])

   AM_CONDITIONAL([HAVE_SIGNAL], [test x$have_signal = xyes])
])dnl


# end of M4 file
