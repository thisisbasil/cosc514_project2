#include "Mem.h"

std::mutex Memory::m;

Memory::Memory()
{
	
}

Memory& Memory::access()
{
	static Memory instance;
	return instance;
}
