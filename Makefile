CC=g++
CPPFLAGS=-g -Wall -pthread -std=c++17 -I./inc -gdwarf-2 -gstrict-dwarf

default: all

all: PCB System main
	$(CC) $(CPPFLAGS) -o bin/os objs/PCB.o objs/Sys.o objs/main.o

PCB: 
	$(CC) $(CPPFLAGS) -c src/PCB.cpp -o objs/PCB.o


System: PCB 
	$(CC) $(CPPFLAGS) -c src/System.cpp -o objs/Sys.o

main: PCB System
	$(CC) $(CPPFLAGS) -c src/main.cpp -o objs/main.o

clean:
	for file in objs/* bin/*; do \
		if [ -e $$file ]; then \
			rm $$file || exit 1; \
		fi \
	done
