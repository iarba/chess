# makefile

CC= g++
CFLAGS= -g -Wall -Werror

all: play.run

play.run: chess.o play.o
	$(CC) $(CFLAGS) chess.o play.o -o play.run

chess.o: chess.cpp chess.hpp defines.hpp cmrv.hpp
	$(CC) $(CFLAGS) chess.cpp -c

play.o: play.cpp chess.hpp defines.hpp cmrv.hpp
	$(CC) $(CFLAGS) play.cpp -c

clean:
	rm *.o play.run
