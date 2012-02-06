#
#   DMS Tools and Utilities
#   Copyright (C) 2008 David M. Syzdek <david@syzdek.net>
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

# AC_DMS_GIT_PACKAGE_VERSION()
# -----------------------------------
AC_DEFUN([AC_DMS_GIT_PACKAGE_VERSION],[dnl

   if test -f ${srcdir}/.git/config;then
      GPV=$(git describe --abbrev=7 HEAD 2>/dev/null)
      GPV=$(echo "$GPV" | sed -e 's/-/./g')
      GPV=$(echo "$GPV" | sed -e 's/^v//g')
      if test "x${GPV}" != "x";then
         AC_MSG_NOTICE([using git package version ${GPV}])
         if test -d ${ac_aux_dir};then 
            echo ${GPV} > ${ac_aux_dir}/git-package-version
         fi
      fi
   elif test -f ${ac_aux_dir}/git-package-version;then
      GPV=$(cat ${ac_aux_dir}/git-package-version)
      if test -d ${ac_aux_dir};then
         AC_MSG_NOTICE([using cached git package version ${GPV}])
      fi
   fi

   if test "x${GPV}" = "x";then
      AC_MSG_WARN([unable to determine package version from Git tags])
   else
      #
      # set internal variables
      GIT_PACKAGE_VERSION=${GPV}
      PACKAGE_VERSION=${GPV}
      VERSION=${GPV}
      #
      # set substitution variables
      AC_SUBST([GIT_PACKAGE_VERSION], [${GPV}])
      AC_SUBST([PACKAGE_VERSION], [${GPV}])
      AC_SUBST([VERSION], [${GPV}])
      #
      # set C/C++/Objc preprocessor macros
      AC_DEFINE_UNQUOTED([GIT_PACKAGE_VERSION], ["${GIT_PACKAGE_VERSION}"], [package version determined from git repository])
      AC_DEFINE_UNQUOTED([PACKAGE_VERSION], ["${GIT_PACKAGE_VERSION}"], [package version determined from git repository])
      AC_DEFINE_UNQUOTED([VERSION], ["${GIT_PACKAGE_VERSION}"], [package version determined from git repository])
   fi
])dnl

# end of M4 file
