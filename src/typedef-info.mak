typedef-info: Makefile typedef-info.c typedef-info.mak
	$(LIBTOOL) --mode=compile --tag=CC $(CC) $(CFLAGS) -c typedef-info.c
	$(LIBTOOL) --mode=link    --tag=CC $(CC) $(CFLAGS) -o typedef-info typedef-info.lo

typedef-info-clean:
	$(LIBTOOL) --mode=clean rm -f typedef-info.lo typedef-info

