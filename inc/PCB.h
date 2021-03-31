#ifndef PCB_H
#define PCB_H

#include <iostream>
#include <vector>
#include <string>
#include <mutex>
#include <chrono>

using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;

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
      startTime (std::chrono::steady_clock::now()) {}

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
	~PCB()						= default;

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
			<< "\tState: " << state << std::endl
			<< "\tProcess #: " << toOutput.number << std::endl
			<< "\tPC #: " << toOutput.PC << std::endl << std::endl;
		return out;
	}
};

using process = PCB;

#endif
