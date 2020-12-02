#----- Modify CC and CFLAGS as appropriate in your environment
CC = gcc
CFLAGS = -O3 -fopenmp -fno-stack-protector

LIB = -lm -lc

UNITFILES = 

#------------ EXECUTABLE FILES ---------------

all: histo histo_locks histo_atomic histo_creative

histo: histo.c
	$(CC) $(CFLAGS) -o histo histo.c $(UNITFILES) $(LIB)

histo_locks: histo_locks.c
	$(CC) $(CFLAGS) -o histo_locks histo_locks.c $(UNITFILES) $(LIB)

histo_atomic: histo_atomic.c
	$(CC) $(CFLAGS) -o histo_atomic histo_atomic.c $(UNITFILES) $(LIB)

histo_creative: histo_creative.c
	$(CC) $(CFLAGS) -o histo_creative histo_creative.c $(UNITFILES) $(LIB)

.PHONY: all

clean:
	rm -f histo histo_locks histo_atomic histo_creative *.o *~


