#include "PCB.h"

unsigned PCB::procNum = 0;

process generate_process(const std::string& name)
{
    process p = std::make_shared<PCB>(name);
    return p;
}
