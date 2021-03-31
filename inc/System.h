#ifndef SYSTEM_H
#define SYSTEM_H

#include "PCB.h"
#include "Mem.h"
#include "Semaphore.hpp"
#include <deque>
#include <mutex>
#include <thread>
#include <vector>

class System
{
private:
	// only allow one system to exist
    System(){}
	
	// disable moving and copying
	System(const System&)            = delete;
	System(System&&)                 = delete;
	System& operator=(const System&) = delete;
	System& operator=(System&&)      = delete;

	static std::mutex m;
    static std::deque<process> newQ;
    static std::deque<process> ReadyQ;
    static std::deque<process> DiskQ;
	static process currProc;
    static TimePoint clock;
    static std::vector<std::thread> tPool;
    static void eventLoop();
    static void powerOff();

public:
	// provide a single point of access to the system
	static System& access();
    ~System() = default;
    
	static void addProcess(const process&);
	static void addProcess(const std::string&);	
};

#endif
