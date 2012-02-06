#
#   DMS Tools and Utilities
#   Copyright (C) 2009 David M. Syzdek <david@syzdek.net>.
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
#   src/Makefile  simple make file for building binaries during testing
#

PROGRAM		?=
prefix		?= /usr/local
bindir		?= $(prefix)/bin

LIBTOOL		?= libtool
CC		= gcc

CFLAGS		?= -W -Wall -g -O2 -DHAVE_COMMON_H=1
LDFLAGS		?= -W -Wall -g -O2

all:
	@for BINARY in `ls *.mak |sed -e "s/.mak$$//g"`;do \
		make $(MAKEFLAGS) $${BINARY} || exit $?; \
	done;

%-install:
	BINARY=$$(echo ${@} |sed -e 's/-install$$//g'); \
	make $(MAKEFLAGS) $${BINARY}; \
	$(LIBTOOL) --mode=install install -c $${BINARY} $(bindir)/$${BINARY};

install: all
	mkdir -p $(bindir)
	for BINARY in `ls *.mak |sed -e "s/.mak$$//g"`;do \
		$(LIBTOOL) --mode=install install -c $${BINARY} $(bindir)/$${BINARY} || exit $?; \
	done;

%-uninstall:
	BINARY=$$(echo ${@} |sed -e 's/-install$$//g'); \
	$(LIBTOOL) --mode=uninstall rm -f $(bindir)/$${BINARY};

uninstall:
	for BINARY in `ls *.mak |sed -e "s/.mak$$//g"`;do \
		$(LIBTOOL) --mode=uninstall rm -f $(bindir)/$${BINARY} || exit $?; \
	done;

clean: 
	rm -f a.out
	@for BINARY in `ls *.mak |sed -e "s/.mak$$//g"`;do \
		make $(MAKEFLAGS) $${BINARY}-clean; \
	done;

-include *.mak

# end of Makefile
