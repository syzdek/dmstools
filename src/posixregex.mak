posixregex: Makefile posixregex.c posixregex.mak
	$(LIBTOOL) --mode=compile --tag=CC $(CC) $(CFLAGS) -c posixregex.c
	$(LIBTOOL) --mode=link    --tag=CC $(CC) $(CFLAGS) -o posixregex posixregex.lo

posixregex-clean:
	$(LIBTOOL) --mode=clean rm -f posixregex.lo posixregex

