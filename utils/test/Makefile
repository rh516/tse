CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g
LIBS=-lutils -lcurl

test:
	gcc $(CFLAGS) test.c $(LIBS) -o $@

clean:
	rm -f test