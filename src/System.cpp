#include "System.h"

// static definitions
std::deque<process> System::newQ;
std::deque<process> System::ReadyQ;
std::deque<process> System::DiskQ;
std::vector<std::thread> System::tPool;
std::mutex System::m;
process System::currProc;
TimePoint System::clock;
bool System::end = false;

void System::powerOff()
{
    end = true;
    for (auto& thread : tPool) thread.join();
}

void System::eventLoop()
{
    clock = std::chrono::steady_clock::now();

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

void System::destroyOne()
{
    std::lock_guard<std::mutex> lock(m);
    newQ.pop_front();
}
