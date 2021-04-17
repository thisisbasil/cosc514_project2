#include "System.h"

// initialize everything
System::System()
{
// initialize clock
    clock = 0;

// initialize end flag
    end = false;
}

// output final statistics
System::~System()
{

}

// used to turn on the system
void System::powerOn()
{
}

// used to turn off the system
void System::powerOff()
{
    end = true;
}

// the main event loop
void System::eventLoop()
{
// keep processing until powering off
    while (!end)
    {

// increment the clock upon each 
        clock++;
    }
}

// add a process to the system
// called by System::access().addProcess()
void System::addProcess(const process& p)
{
// add a process to the queue
// might need to use a mutex
}

