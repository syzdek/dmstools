suicide: Makefile suicide.c suicide.mak
	$(LIBTOOL) --mode=compile --tag=CC $(CC) $(CFLAGS) -c suicide.c
	$(LIBTOOL) --mode=link    --tag=CC $(CC) $(CFLAGS) -o suicide suicide.lo

suicide-clean:
	$(LIBTOOL) --mode=clean rm -f suicide.lo suicide

