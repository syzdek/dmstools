codetagger: Makefile codetagger.c codetagger.mak
	$(LIBTOOL) --mode=compile --tag=CC $(CC) $(CFLAGS) -c codetagger.c
	$(LIBTOOL) --mode=link    --tag=CC $(CC) $(CFLAGS) -o codetagger codetagger.lo

codetagger-clean:
	$(LIBTOOL) --mode=clean rm -f codetagger.lo codetagger

