numconvert: Makefile numconvert.c numconvert.mak
	$(LIBTOOL) --mode=compile --tag=CC $(CC) $(CFLAGS) -c numconvert.c
	$(LIBTOOL) --mode=link    --tag=CC $(CC) $(CFLAGS) -o numconvert numconvert.lo

numconvert-clean:
	$(LIBTOOL) --mode=clean rm -f numconvert.lo numconvert

