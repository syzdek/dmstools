macaddrinfo.oui.txt:
	curl -o macaddrinfo.oui.txt http://standards.ieee.org/develop/regauth/oui/oui.txt

macaddrinfo: Makefile macaddrinfo.c macaddrinfo.mak macaddrinfo.oui.txt
	$(LIBTOOL) --mode=compile --tag=CC $(CC) $(CFLAGS) -c macaddrinfo.c
	$(LIBTOOL) --mode=link    --tag=CC $(CC) $(CFLAGS) -o macaddrinfo macaddrinfo.lo

macaddrinfo-clean:
	$(LIBTOOL) --mode=clean rm -f macaddrinfo.lo macaddrinfo

