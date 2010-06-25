#!/bin/sh
#
#   DMS Tools and Utilities
#   Copyright (C) 2010 David M. Syzdek <david@syzdek.net>
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
