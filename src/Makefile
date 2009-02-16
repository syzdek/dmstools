#
#   DMS Tools and Utilities
#   Copyright (C) 2009 David M. Syzdek <david@syzdek.net>.
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
#   src/Makefile  simple make file for building binaries during testing
#

PROGRAM		?=
prefix		?= /usr/local
bindir		?= $(prefix)/bin

LIBTOOL		?= libtool
CC		= gcc

CFLAGS		?= -W -Wall -g -O2 -DHAVE_COMMON_H=1
LDFLAGS		?= -W -Wall -g -O2

-include $(PROGRAM).mak

all:
	for BINARY in `ls *.mak |sed -e "s/.mak$$//g"`;do \
		make $(MAKEFLAGS) PROGRAM=$${BINARY} $${BINARY}; \
	done;

install:
	mkdir -p $(bindir)
	for BINARY in `ls *.mak |sed -e "s/.mak$$//g"`;do \
		make $(MAKEFLAGS) PROGRAM=$${BINARY} $${BINARY}; \
		$(LIBTOOL) --mode=install install -c $${BINARY} $(bindir)/$${BINARY}; \
	done;

uninstall:
	for BINARY in `ls *.mak |sed -e "s/.mak$$//g"`;do \
		$(LIBTOOL) --mode=uninstall rm -f $(bindir)/$${BINARY}; \
	done;

clean: 
	for BINARY in `ls *.mak |sed -e "s/.mak$$//g"`;do \
		make $(MAKEFLAGS) PROGRAM=$${BINARY} $${BINARY}-clean; \
	done;

# end of Makefile
