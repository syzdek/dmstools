endian: Makefile endian.c endian.mak
	$(LIBTOOL) --mode=compile --tag=CC $(CC) $(CFLAGS) -c endian.c
	$(LIBTOOL) --mode=link    --tag=CC $(CC) $(CFLAGS) -o endian endian.lo

endian-clean:
	$(LIBTOOL) --mode=clean rm -f endian.lo endian

