CC = g++
CFLAGS = -Wall -Werror -std=gnu++11 -O2 -pthread

all: rainfall_seq rainfall_pt

rainfall_seq: rainfall_seq.cpp utils.hpp
	$(CC) $(CFLAGS) -o rainfall_seq rainfall_seq.cpp

rainfall_pt: rainfall_pt.cpp utils.hpp
	$(CC) $(CFLAGS) -o rainfall_pt rainfall_pt.cpp

.PHONY: all

clean:
	rm -f rainfall_seq rainfall_pt *.o *~