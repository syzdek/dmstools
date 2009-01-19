#
#   $Id: $
#
#   Syzdek Coding Tools
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
#   acinclude.m4 - custom m4 macros used by configure.ac
#

# AC_SYZDEK_GIT_PACKAGE_VERSION()
# -----------------------------------
AC_DEFUN([AC_SYZDEK_GIT_PACKAGE_VERSION],[dnl
   if test -d ${srcdir}/.git -o -f ${srcdir}/.git;then
      GPV=$(git describe --abbrev=4 HEAD 2>/dev/null)
      GPV=$(echo "$GPV" | sed -e 's/-/./g');
      GPV=$(echo "$GPV" | sed -e 's/^v//g');
      if test "x${GPV}" = "x";then
         AC_MSG_WARN([unable to determine package version from Git tags])
      else
         GIT_PACKAGE_VERSION=${GPV}
         AC_SUBST([GIT_PACKAGE_VERSION], [${GPV}])
         AC_SUBST([PACKAGE_VERSION], [${GPV}])
         AC_SUBST([VERSION], [${GPV}])
         AC_DEFINE_UNQUOTED([GIT_PACKAGE_VERSION], ["${GIT_PACKAGE_VERSION}"], [package version determined from git repository])
         AC_DEFINE_UNQUOTED([PACKAGE_VERSION], ["${GIT_PACKAGE_VERSION}"], [package version determined from git repository])
         AC_DEFINE_UNQUOTED([VERSION], ["${GIT_PACKAGE_VERSION}"], [package version determined from git repository])
      fi
   fi
])dnl

# end of M4 file
