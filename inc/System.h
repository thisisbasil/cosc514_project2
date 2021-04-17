#ifndef SYSTEM_H
#define SYSTEM_H

// import the process class
#include "PCB.h"

// import the custom semaphore class
// might not be needed
#include "Semaphore.hpp"

/*
   import STL double-ended queue
   this seems like the most appropriate
   STL container for this, as access, insertion,
   and deletion are all O(1)
   
   reference for deque: https://en.cppreference.com/w/cpp/container/deque
   
   if a better container e.g. vector, linked lists (forward_list which is 
   singly linked or list which is doubly linked), array, the we can
   certainly change it
   
   the good thing about STL containers is that they allow for standard
   looping i.e.
   
   deque<PCB> readyQ;
   for (int i = 0; i < readyQ.size(); ++i)
   { 
   // do something with readyQ[i]
   }
   
   as well as range looping, which tends to be faster i.e.
   
   dqeue<PCB> readyQ;
   for (PCB currElement : readyQ)
   {
   // do something with currElem
   }
*/
#include <deque>


/* 
   imports standard algorithms, simplifies things
   like searching, finding, sorting
   
   reference: https://en.cppreference.com/w/cpp/algorithm
   
*/
#include <algorithm>

// imports max/min numbers for each type
#include <climits>

// use the standard namespace so we dont have to use
// "std::" everywhere
using namespace std;

/* 
   make the maximum clock value equal
   to the maximum value for unsigned long
   integers i.e. strictly non-negative numbers 
*/
const unsigned long MAX_CLOCK = ULONG_MAX;

// class representing the CPU
class System
{
private:
	
// queues representing processes waiting and ready
    deque<process> waitQ;
    deque<process> readyQ;
    
// the process currently executing
	process currProc;
	
// a variable representing when to end
// may or may not be necessary
   bool end;
    
// the clock, initialized to 0
   unsigned long clock;
    
// the event loop
   void eventLoop();

public:
    System();
    ~System();
    // disable moving and copying
	System(const System&)            = delete;
	System(System&&)                 = delete;
	System& operator=(const System&) = delete;
	System& operator=(System&&)      = delete;
    
    
    // add a process to the system, defined in System.cpp
    static void addProcess(const process&);
    
    void powerOn();
    void powerOff();
};

#endif
