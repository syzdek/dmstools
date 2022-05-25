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

# AC_DMS_REGEX
# ______________________________________________________________________________
AC_DEFUN([AC_DMS_REGEX],[dnl

   # prerequists
   AC_REQUIRE([AC_PROG_CC])

   HAVE_REGEX=yes
   AC_SEARCH_LIBS([regcomp],   [regex],,[HAVE_REGEX=no])
   AC_SEARCH_LIBS([regexec],   [regex],,[HAVE_REGEX=no])
   AC_SEARCH_LIBS([regfree],   [regex],,[HAVE_REGEX=no])
   AC_SEARCH_LIBS([regerror],  [regex],,[HAVE_REGEX=no])

   AM_CONDITIONAL([HAVE_REGEX], [test x$HAVE_REGEX = xyes])
])dnl


# AC_DMS_TOOL_ALL
# ______________________________________________________________________________
AC_DEFUN([AC_DMS_TOOL_ALL],[dnl
   enableval=""
   AC_ARG_ENABLE(
      all,
      [AS_HELP_STRING([--enable-all], [enable all tools])],
      [ EALL=$enableval ],
      [ EALL=$enableval ]
   )
   if test "x${EALL}" != "xno" && test "x${EALL}" != "xyes";then
      EALL="auto"
   fi
])dnl


# AC_DMS_TOOL_BINDUMP
# ______________________________________________________________________________
AC_DEFUN([AC_DMS_TOOL_BINDUMP],[dnl

   AC_REQUIRE([AC_DMS_TOOL_ALL])

   enableval=""
   AC_ARG_ENABLE(
      bindump,
      [AS_HELP_STRING([--disable-bindump], [disable building bindump])],
      [ EBINDUMP=$enableval ],
      [ EBINDUMP=$enableval ]
   )

   if test "x${EBINDUMP}" == "x";then
      EBINDUMP="${EALL}"
   fi
   if test "x${EBINDUMP}" == "xauto";then
      EBINDUMP=yes
   fi

   AM_CONDITIONAL([WANT_BINDUMP],   [test "$EBINDUMP" = "yes"])
])dnl


# AC_DMS_TOOL_BITCOPY
# ______________________________________________________________________________
AC_DEFUN([AC_DMS_TOOL_BITCOPY],[dnl

   AC_REQUIRE([AC_DMS_TOOL_ALL])

   enableval=""
   AC_ARG_ENABLE(
      bitcopy,
      [AS_HELP_STRING([--enable-bitcopy], [enable building bitcopy])],
      [ EBITCOPY=$enableval ],
      [ EBITCOPY=$enableval ]
   )

   if test "x${EBITCOPY}" == "x";then
      EBITCOPY="${EALL}"
   fi
   if test "x${EBITCOPY}" == "xauto";then
      EBITCOPY=no
   fi

   AM_CONDITIONAL([WANT_BITCOPY],   [test "$EBITCOPY" = "yes"])
])dnl


# AC_DMS_TOOL_BITOPS
# ______________________________________________________________________________
AC_DEFUN([AC_DMS_TOOL_BITOPS],[dnl

   AC_REQUIRE([AC_DMS_TOOL_ALL])

   enableval=""
   AC_ARG_ENABLE(
      bitops,
      [AS_HELP_STRING([--enable-bitops], [enable building binops])],
      [ EBITOPS=$enableval ],
      [ EBITOPS=$enableval ]
   )

   if test "x${EBITOPS}" == "x";then
      EBITOPS="${EALL}"
   fi
   if test "x${EBITOPS}" == "xauto";then
      EBITOPS=no
   fi

   AM_CONDITIONAL([WANT_BITOPS],   [test "$EBITOPS" = "yes"])
])dnl


# AC_DMS_TOOL_CODETAGGER
# ______________________________________________________________________________
AC_DEFUN([AC_DMS_TOOL_CODETAGGER],[dnl

   AC_REQUIRE([AC_DMS_TOOL_ALL])
   AC_REQUIRE([AC_DMS_WIN32])

   enableval=""
   AC_ARG_ENABLE(
      codetagger,
      [AS_HELP_STRING([--enable-codetagger], [enable building codetagger])],
      [ ECODETAGGER=$enableval ],
      [ ECODETAGGER=$enableval ]
   )

   if test "x${ECODETAGGER}" == "x";then
      ECODETAGGER="${EALL}"
   fi
   if test "x${ECODETAGGER}" == "xauto";then
      ECODETAGGER=no
   fi
   

   AM_CONDITIONAL([WANT_CODETAGGER],   [test "$ECODETAGGER" = "yes"])
])dnl


# AC_DMS_TOOL_CODETAGGER
# ______________________________________________________________________________
AC_DEFUN([AC_DMS_TOOL_CODETAGGER],[dnl

   AC_REQUIRE([AC_DMS_TOOL_ALL])

   enableval=""
   AC_ARG_ENABLE(
      codetagger,
      [AS_HELP_STRING([--enable-codetagger], [enable building codetagger])],
      [ ECODETAGGER=$enableval ],
      [ ECODETAGGER=$enableval ]
   )

   if test "x${ECODETAGGER}" == "x";then
      ECODETAGGER="${EALL}"
   fi
   if test "x${ECODETAGGER}" == "xauto";then
      ECODETAGGER=yes
   fi

   AM_CONDITIONAL([WANT_CODETAGGER],   [test "$ECODETAGGER" = "yes"])
])dnl


# AC_DMS_TOOL_COLORS
# ______________________________________________________________________________
AC_DEFUN([AC_DMS_TOOL_COLORS],[dnl

   AC_REQUIRE([AC_DMS_TOOL_ALL])

   enableval=""
   AC_ARG_ENABLE(
      colors,
      [AS_HELP_STRING([--enable-colors], [enable building colors])],
      [ ECOLORS=$enableval ],
      [ ECOLORS=$enableval ]
   )

   if test "x${ECOLORS}" == "x";then
      ECOLORS="${EALL}"
   fi
   if test "x${ECOLORS}" == "xauto";then
      ECOLORS=yes
   fi

   AM_CONDITIONAL([WANT_COLORS],   [test "$ECOLORS" = "yes"])
])dnl


# AC_DMS_TOOL_ENDIAN
# ______________________________________________________________________________
AC_DEFUN([AC_DMS_TOOL_ENDIAN],[dnl

   AC_REQUIRE([AC_DMS_TOOL_ALL])

   enableval=""
   AC_ARG_ENABLE(
      endian,
      [AS_HELP_STRING([--enable-endian], [enable building endian])],
      [ EENDIAN=$enableval ],
      [ EENDIAN=$enableval ]
   )

   if test "x${EENDIAN}" == "x";then
      EENDIAN="${EALL}"
   fi
   if test "x${EENDIAN}" == "xauto";then
      EENDIAN=no
   fi

   AM_CONDITIONAL([WANT_ENDIAN],   [test "$EENDIAN" = "yes"])
])dnl


# AC_DMS_TOOL_MACADDRINFO
# ______________________________________________________________________________
AC_DEFUN([AC_DMS_TOOL_MACADDRINFO],[dnl

   AC_REQUIRE([AC_DMS_TOOL_ALL])

   enableval=""
   AC_ARG_ENABLE(
      macaddrinfo,
      [AS_HELP_STRING([--enable-macaddrinfo], [enable building macaddrinfo])],
      [ EMACADDRINFO=$enableval ],
      [ EMACADDRINFO=$enableval ]
   )

   if test "x${EMACADDRINFO}" == "x";then
      EMACADDRINFO="${EALL}"
   fi
   if test "x${EMACADDRINFO}" == "xauto";then
      EMACADDRINFO=yes
   fi

   AM_CONDITIONAL([WANT_MACADDRINFO],   [test "$EMACADDRINFO" = "yes"])
])dnl


# AC_DMS_TOOL_NETCALC
# ______________________________________________________________________________
AC_DEFUN([AC_DMS_TOOL_NETCALC],[dnl

   AC_REQUIRE([AC_DMS_TOOL_ALL])

   enableval=""
   AC_ARG_ENABLE(
      netcalc,
      [AS_HELP_STRING([--enable-netcalc], [enable building netcalc])],
      [ ENETCALC=$enableval ],
      [ ENETCALC=$enableval ]
   )

   if test "x${ENETCALC}" == "x";then
      ENETCALC="${EALL}"
   fi
   if test "x${ENETCALC}" == "xauto";then
      ENETCALC=yes
   fi

   AM_CONDITIONAL([WANT_NETCALC],   [test "$ENETCALC" = "yes"])
])dnl


# AC_DMS_TOOL_NUMCONVERT
# ______________________________________________________________________________
AC_DEFUN([AC_DMS_TOOL_NUMCONVERT],[dnl

   AC_REQUIRE([AC_DMS_TOOL_ALL])

   enableval=""
   AC_ARG_ENABLE(
      numconvert,
      [AS_HELP_STRING([--enable-numconvert], [enable building numconvert])],
      [ ENUMCONVERT=$enableval ],
      [ ENUMCONVERT=$enableval ]
   )

   if test "x${ENUMCONVERT}" == "x";then
      ENUMCONVERT="${EALL}"
   fi
   if test "x${ENUMCONVERT}" == "xauto";then
      ENUMCONVERT=yes
   fi

   AM_CONDITIONAL([WANT_NUMCONVERT],   [test "$ENUMCONVERT" = "yes"])
])dnl


# AC_DMS_TOOL_POSIXREGEX
# ______________________________________________________________________________
AC_DEFUN([AC_DMS_TOOL_POSIXREGEX],[dnl

   AC_REQUIRE([AC_DMS_TOOL_ALL])
   AC_REQUIRE([AC_DMS_REGEX])

   enableval=""
   AC_ARG_ENABLE(
      posixregex,
      [AS_HELP_STRING([--enable-posixregex], [enable building posixregex])],
      [ EPOSIXREGEX=$enableval ],
      [ EPOSIXREGEX=$enableval ]
   )

   if test "x${EPOSIXREGEX}" == "x";then
      if test "x${HAVE_REGEX}" == "xno";then
         EPOSIXREGEX=no
      else
         EPOSIXREGEX="${EALL}"
      fi
   fi
   if test "x${EPOSIXREGEX}" == "xauto";then
      if test "x${HAVE_REGEX}" == "xno";then
         EPOSIXREGEX=no
      else
         EPOSIXREGEX=yes
      fi
   fi
   if test  "x${EPOSIXREGEX}" == "xyes" && test "x${HAVE_REGEX}" == "xno";then
      AC_MSG_ERROR([posixregex requires regcomp, regexec, regfree, and regerror])
   fi

   AM_CONDITIONAL([WANT_POSIXREGEX],   [test "$EPOSIXREGEX" = "yes"])
])dnl


# AC_DMS_TOOL_RECURSE
# ______________________________________________________________________________
AC_DEFUN([AC_DMS_TOOL_RECURSE],[dnl

   AC_REQUIRE([AC_DMS_TOOL_ALL])

   enableval=""
   AC_ARG_ENABLE(
      recurse,
      [AS_HELP_STRING([--enable-recurse], [enable building recurse])],
      [ ERECURSE=$enableval ],
      [ ERECURSE=$enableval ]
   )

   if test "x${ERECURSE}" == "x";then
      ERECURSE="${EALL}"
   fi
   if test "x${ERECURSE}" == "xauto";then
      ERECURSE=no
   fi

   AM_CONDITIONAL([WANT_RECURSE],   [test "$ERECURSE" = "yes"])
])dnl


# AC_DMS_TOOL_SUICIDE
# ______________________________________________________________________________
AC_DEFUN([AC_DMS_TOOL_SUICIDE],[dnl

   AC_REQUIRE([AC_DMS_TOOL_ALL])

   enableval=""
   AC_ARG_ENABLE(
      suicide,
      [AS_HELP_STRING([--enable-suicide], [enable building suicide])],
      [ ESUICIDE=$enableval ], 
      [ ESUICIDE=$enableval ]
   )

   if test "x${ESUICIDE}" == "x";then
      ESUICIDE="${EALL}"
   fi
   if test "x${ESUICIDE}" == "xauto";then
      ESUICIDE=no
   fi

   AM_CONDITIONAL([WANT_SUICIDE],   [test "$ESUICIDE" = "yes"])
])dnl


# AC_DMS_TOOL_TYPEDEF_INFO
# ______________________________________________________________________________
AC_DEFUN([AC_DMS_TOOL_TYPEDEF_INFO],[dnl

   AC_REQUIRE([AC_DMS_TOOL_ALL])

   enableval=""
   AC_ARG_ENABLE(
      typedef-info,
      [AS_HELP_STRING([--enable-typedef-info], [enable building typedef-info])],
      [ ETYPEDEF_INFO=$enableval ],
      [ ETYPEDEF_INFO=$enableval ]
   )

   if test "x${ETYPEDEF_INFO}" == "x";then
      ETYPEDEF_INFO="${EALL}"
   fi
   if test "x${ETYPEDEF_INFO}" == "xauto";then
      ETYPEDEF_INFO=no
   fi

   AM_CONDITIONAL([WANT_TYPEDEF_INFO],   [test "$ETYPEDEF_INFO" = "yes"])
])dnl


# AC_DMS_TOOL_URLDESC
# ______________________________________________________________________________
AC_DEFUN([AC_DMS_TOOL_URLDESC],[dnl

   AC_REQUIRE([AC_DMS_TOOL_ALL])

   enableval=""
   AC_ARG_ENABLE(
      urldesc,
      [AS_HELP_STRING([--enable-urldesc], [enable building urldesc])],
      [ EURLDESC=$enableval ],
      [ EURLDESC=$enableval ]
   )

   if test "x${EURLDESC}" == "x";then
      EURLDESC="${EALL}"
   fi
   if test "x${EURLDESC}" == "xauto";then
      EURLDESC=yes
   fi

   AM_CONDITIONAL([WANT_URLDESC],   [test "$EURLDESC" = "yes"])
])dnl


# AC_DMS_TOOL_URLENCODING
# ______________________________________________________________________________
AC_DEFUN([AC_DMS_TOOL_URLENCODING],[dnl

   AC_REQUIRE([AC_DMS_TOOL_ALL])

   enableval=""
   AC_ARG_ENABLE(
      urlencoding,
      [AS_HELP_STRING([--enable-urlencoding], [enable building urlencoding])],
      [ EURLENCODING=$enableval ],
      [ EURLENCODING=$enableval ]
   )

   if test "x${EURLENCODING}" == "x";then
      EURLENCODING="${EALL}"
   fi
   if test "x${EURLENCODING}" == "xauto";then
      EURLENCODING=yes
   fi

   AM_CONDITIONAL([WANT_URLENCODING],   [test "$EURLENCODING" = "yes"])
])dnl


# end of M4 file
