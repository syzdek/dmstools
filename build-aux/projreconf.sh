#!/bin/sh
#
#   DMS Tools and Utilities
#   Copyright (C) 2010 David M. Syzdek <david@syzdek.net>
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
#   build-aux/projreconf.sh - automate running auto tools
#

# sets program information
cd `dirname ${0}`;cd ..
PROGRAM_BASE=`pwd`
PROGRAM_NAME=`basename ${0}`
DATE=`date`
FILESTAMP=`date "+%Y-%m-%d-%H%M%S"`-$$

# test for required programs
which cp              > /dev/null 2>&1 || { echo "${PROGRAM_NAME}: cp not found"; exit 1; };
which mv              > /dev/null 2>&1 || { echo "${PROGRAM_NAME}: mv not found"; exit 1; };
which autoreconf      > /dev/null 2>&1 || { echo "${PROGRAM_NAME}: autoreconf not found"; exit 1; };
which gettextize      > /dev/null 2>&1 || { echo "${PROGRAM_NAME}: gettextize not found"; exit 1; };

# test for required files
test -f configure.ac    || { echo "${PROGRAM_NAME}: configure.ac not found"; exit 1; };
test -f Makefile.am     || { echo "${PROGRAM_NAME}: Makefile.am not found"; exit 1; };

# restore function
restore()
{
   echo "$1"
   test -f configure.ac-${FILESTAMP} && mv configure.ac-${FILESTAMP}  configure.ac
   test -f Makefile.am-${FILESTAMP}  && mv Makefile.am-${FILESTAMP}   Makefile.am
   exit $2
}

# backup files
echo "backing up files..."
cp configure.ac configure.ac-${FILESTAMP} || restore "${PROGRAM_NAME}: unable to backup configure.ac" 1
cp Makefile.am  Makefile.am-${FILESTAMP}  || restore "${PROGRAM_NAME}: unable to backup Makefile.am"  1

# run scripts
echo "running gettextize -f --no-changelog..."
gettextize -f --no-changelog || restore "" 1
echo "running autoreconf -i..."
autoreconf -i || restore "" 1

# restore files
restore "restoring files..." 0

# end of script
