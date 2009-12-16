bitcopy: Makefile bitcopy.c bitcopy.mak
	$(LIBTOOL) --mode=compile --tag=CC $(CC) $(CFLAGS) -c bitcopy.c
	$(LIBTOOL) --mode=link    --tag=CC $(CC) $(CFLAGS) -o bitcopy bitcopy.lo

bitcopy-clean:
	$(LIBTOOL) --mode=clean rm -f bitcopy.lo bitcopy

