typedef-sizes: Makefile typedef-sizes.c typedef-sizes.mak
	$(LIBTOOL) --mode=compile --tag=CC $(CC) $(CFLAGS) -c typedef-sizes.c
	$(LIBTOOL) --mode=link    --tag=CC $(CC) $(CFLAGS) -o typedef-sizes typedef-sizes.lo

typedef-sizes-clean:
	$(LIBTOOL) --mode=clean rm -f typedef-sizes.lo typedef-sizes

