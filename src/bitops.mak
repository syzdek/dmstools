bitops: Makefile bitops.c bitops.mak
	$(LIBTOOL) --mode=compile --tag=CC $(CC) $(CFLAGS) -c bitops.c
	$(LIBTOOL) --mode=link    --tag=CC $(CC) $(CFLAGS) -o bitops bitops.lo

bitops-clean:
	$(LIBTOOL) --mode=clean rm -f bitops.lo bitops

