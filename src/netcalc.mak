netcalc: Makefile netcalc.c netcalc.mak
	$(LIBTOOL) --mode=compile --tag=CC $(CC) $(CFLAGS) -c netcalc.c
	$(LIBTOOL) --mode=link    --tag=CC $(CC) $(CFLAGS) -o netcalc netcalc.lo

netcalc-clean:
	$(LIBTOOL) --mode=clean rm -f netcalc.lo netcalc

