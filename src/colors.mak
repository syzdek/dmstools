colors: Makefile colors.c colors.mak
	$(LIBTOOL) --mode=compile --tag=CC $(CC) $(CFLAGS) -c colors.c
	$(LIBTOOL) --mode=link    --tag=CC $(CC) $(CFLAGS) -o colors colors.lo

colors-clean:
	$(LIBTOOL) --mode=clean rm -f colors.lo colors

