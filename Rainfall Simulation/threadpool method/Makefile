CC = g++
CFLAGS = -Wall -Werror -std=gnu++11 -O2 -pthread

all: rainfall_seq rainfall_pt rainfall_th

rainfall_seq: rainfall_seq.cpp utils.hpp
	$(CC) $(CFLAGS) -o rainfall_seq rainfall_seq.cpp

rainfall_pt: rainfall_pt.cpp threadpool.hpp utils.hpp
	$(CC) $(CFLAGS) -o rainfall_pt rainfall_pt.cpp

rainfall_th: rainfall_th.cpp threadpool.hpp utils.hpp
	$(CC) $(CFLAGS) -o rainfall_th rainfall_th.cpp

.PHONY: all

clean:
	rm -f rainfall_seq rainfall_pt rainfall_th *.o *~
