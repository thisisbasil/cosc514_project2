#ifndef MEM_H
#define MEM_H

#include <mutex>
#include <thread>
#include <vector>
#include "Semaphore.hpp"

const unsigned pageSize = 64;


class Memory
{
private:
	// only allow one memory manager to exist
	Memory();

	Memory(const Memory&)      		 = delete;
	Memory(Memory&&)			     = delete;
	Memory& operator=(const Memory&) = delete;
	Memory& operator=(Memory&&)		 = delete;

	static std::mutex m;

public:
	static Memory& access();
};

#endif
