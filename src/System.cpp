#include "System.h"

// static definitions
std::deque<process> System::newQ;
std::deque<process> System::ReadyQ;
std::deque<process> System::DiskQ;
std::vector<std::thread> System::tPool;
std::mutex System::m;
process System::currProc;
TimePoint System::clock;

void System::powerOff()
{}

void System::eventLoop()
{
    clock = std::chrono::steady_clock::now();
    while(true)
    {}
}

void System::addProcess(const std::string& name)
{
    std::lock_guard<std::mutex> lock(m);
    newQ.push_front(process(name));
}

void System::addProcess(const process& p)
{
    std::lock_guard<std::mutex> lock(m);
    newQ.push_front(p);
}

System& System::access()
{
	// instantiating a static variable
	// is an atomic operation post-C++11
	static System instance;
    static bool doOnce = true;
    if (doOnce)
    {
        doOnce = false;
        tPool.push_back(std::thread(System::eventLoop));
    }
	return instance;
}
