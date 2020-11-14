CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g
LIBS=-lutils -lcurl

lindexer:
	gcc $(CFLAGS) lindexer.c $(LIBS) -o $@ -pthread

indexer:
	gcc $(CFLAGS) indexer.c $(LIBS) -o $@

clean:
	rm -f indexer lindexer
