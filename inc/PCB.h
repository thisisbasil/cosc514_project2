#ifndef PCB_H
#define PCB_H

#include <iostream>
#include <vector>
#include <string>
#include <mutex>
#include <chrono>
#include "Semaphore.hpp"
#include <memory>

using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;

namespace
{
    Semaphore<10> proc_sem;
}

class PCB
{
public:
	enum class State { NEW, READY, RUNNING, WAITING, TERMINATED };

private:
	static unsigned procNum;

	State currState;
	std::string name;
	unsigned number;
    TimePoint startTime;
	unsigned long PC;

public:
	PCB(const std::string& _name = "") 
    : currState (State::NEW), name(_name), number(procNum++),
      startTime (std::chrono::steady_clock::now())
    {
        if (name.size() == 0) name = std::to_string(number);
        proc_sem.wait();
        std::cout << "created " << number << std::endl;
    }

    ~PCB() { proc_sem.signal(); std::cout << "destroyed " << number << std::endl;}

	inline const State getState()       { return currState; }
	inline const std::string& getName() { return name;      }
	inline const unsigned getProcNum()  { return number;    }
	inline const unsigned long getPC()  { return PC;        }
    void getRunningTime();

	// no dynamic memory, so no need to
	// explicity define these
	PCB(const PCB&)				= default;
	PCB(PCB&&)					= default;		
	PCB& operator=(const PCB&)  = default;
	PCB& operator=(PCB&&)		= default;

	friend std::ostream& operator<<(std::ostream& out, const PCB& toOutput)
	{
		std::string state;
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

using process = std::shared_ptr<PCB>;

process generate_process(const std::string& name = "");

#endif
