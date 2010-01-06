bindump: Makefile bindump.c bindump.mak
	$(LIBTOOL) --mode=compile --tag=CC $(CC) $(CFLAGS) -c bindump.c
	$(LIBTOOL) --mode=link    --tag=CC $(CC) $(CFLAGS) -o bindump bindump.lo

bindump-clean:
	$(LIBTOOL) --mode=clean rm -f bindump.lo bindump

