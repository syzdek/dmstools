recurse: Makefile recurse.c recurse.mak
	$(LIBTOOL) --mode=compile --tag=CC $(CC) $(CFLAGS) -c recurse.c
	$(LIBTOOL) --mode=link    --tag=CC $(CC) $(CFLAGS) -o recurse recurse.lo

recurse-clean:
	$(LIBTOOL) --mode=clean rm -f recurse.lo recurse

