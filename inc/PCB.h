#ifndef PCB_H
#define PCB_H

// imports i/o streams e.g. cout, endl
#include <iostream>

// imports standard string
#include <string>

// imports custom semaphore, will probably be removed
// if not needed
#include "Semaphore.hpp"

/* 
   imports standard algorithms, simplifies things
   like searching, finding, sorting
   
   reference: https://en.cppreference.com/w/cpp/algorithm
   
*/
#include <algorithm>

// use the standard namespace so we dont have to use
// "std::" everywhere
using namespace std;

const unsigned num_procs = 10;


// PCB class representing the particular processes
class PCB
{
public:

// an enumeration representing the various states that a process
// can be in
	enum class State { NEW, READY, RUNNING, WAITING, TERMINATED };

private:

// a static number representing the process number
	static unsigned procNum;

// the current state of the process
	State currState;

// the name of the process, may not be needed
	string name;
	
// the process number, set in the constructor
	unsigned number;

// the program counter, unsure how this will
// be used
	unsigned long PC;

public:
    // using an inline constructor 
	PCB(const string& _name = "") 
    {
        // What should PC be initialized to?
        currState = State::NEW;
        name = _name;
        number = procNum++;
    }

// the destructor, defined in PCB.cpp
    ~PCB();
    
// simple getter functions
	inline const State getState()       { return currState; }
	inline const string& getName()      { return name;      }
	inline const unsigned getProcNum()  { return number;    }
	inline const unsigned long getPC()  { return PC;        }
    void getRunningTime();

// no dynamic memory, so no need to
// explicity define these
	PCB(const PCB&)				= default;
	PCB(PCB&&)					= default;		
	PCB& operator=(const PCB&)  = default;
	PCB& operator=(PCB&&)		= default;

/* 
    This allows you to add it to output via any output stream
    example:
     
    PCB aPCB;
    // do something with PCB
    cout << aPCB << endl;
*/
	friend ostream& operator<<(ostream& out, const PCB& toOutput)
	{
/* 
       Out represents any out stream, in this case, it will
       essentially represent outputting to stdout i.e. cout
	       
       I am guessing that this will change depending on how
       we want to output
*/
		string state;
		switch(toOutput.currState)
		{
		case State::NEW: state = "new"; break;
		case State::READY: state = "ready"; break;
		case State::RUNNING: state = "running"; break;
		case State::WAITING: state = "waiting"; break;
		case State::TERMINATED: state = "terminated"; break;
		};
		out << "Process " << toOutput.name << ':' << std::endl
            << "  State: " << state << std::endl
            << "  Process #: " << toOutput.number << std::endl
            << "  PC #: " << toOutput.PC;
		return out;
	}
};

// typedef process as PCB, can be used interchangably though
using process = PCB;

#endif
