CFLAGS= -Wall -pedantic -std=c11 -I. -g
OFILES=queue.o hash.o webpage.o pageio.o indexio.o lqueue.o lhash.o

all: $(OFILES)
	ar cr ../lib/libutils.a $(OFILES)

# o files
%.o: %.c %.h
	gcc $(CFLAGS) -c $<

clean: 
	rm -f *.o
