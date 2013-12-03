macaddrutil.oui.txt:
	curl -o macaddrutil.oui.txt http://standards.ieee.org/develop/regauth/oui/oui.txt

macaddrutil: Makefile macaddrutil.c macaddrutil.mak macaddrutil.oui.txt
	$(LIBTOOL) --mode=compile --tag=CC $(CC) $(CFLAGS) -c macaddrutil.c
	$(LIBTOOL) --mode=link    --tag=CC $(CC) $(CFLAGS) -o macaddrutil macaddrutil.lo

macaddrutil-clean:
	$(LIBTOOL) --mode=clean rm -f macaddrutil.lo macaddrutil

