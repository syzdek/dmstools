

all:
	cp example.c    test.c
	cp example.txt  test.txt
	cp example.html test.html
	codetagger -f example-defaults.tags                test.txt
	codetagger -l '//'   -r ''    -f example-c.tags    test.c
	codetagger -l '<!--' -r '-->' -f example-html.tags test.html

clean:
	rm -f test.*
