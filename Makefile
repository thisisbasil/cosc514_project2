CC=clang++
CPPFLAGS=-g -Wall -pthread -std=c++20 -stdlib=libc++ -I./inc 

default: all

all: PCB Memory System main
	$(CC) $(CPPFLAGS) -o bin/os objs/PCB.o objs/Mem.o objs/Sys.o objs/main.o

PCB: 
	$(CC) $(CPPFLAGS) -c src/PCB.cpp -o objs/PCB.o

Memory:
	$(CC) $(CPPFLAGS) -c src/Mem.cpp -o objs/Mem.o

System: PCB Memory
	$(CC) $(CPPFLAGS) -c src/System.cpp -o objs/Sys.o

main: PCB Memory System
	$(CC) $(CPPFLAGS) -c src/main.cpp -o objs/main.o

clean:
	for file in objs/* bin/*; do \
		if [ -e $$file ]; then \
			rm $$file || exit 1; \
		fi \
	done
