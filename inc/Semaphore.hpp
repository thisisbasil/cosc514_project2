#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <mutex>
#include <condition_variable>
#include <exception>

class SizeError : public std::exception
{
public:
	const char* what() const throw() { return "semaphore size must be greater than 0!"; }
};

template <std::size_t SIZE = 1>
class Semaphore
{
private:
	int count;
	std::condition_variable cv;
	std::mutex m;

public:
	Semaphore(const Semaphore&)            = delete;
	Semaphore(Semaphore&&)                 = delete;
	Semaphore& operator=(const Semaphore&) = delete;
	Semaphore& operator=(Semaphore&&)      = delete;
	~Semaphore()                           = default;

	Semaphore() : count(SIZE) { if (count < 1) throw SizeError(); }

	void wait(int num = 1)
	{
		std::unique_lock<std::mutex> lock(m);
		while ((count - num) < 0) cv.wait(lock);
		count -= num;
	}

	void signal(int num = 1)
	{
		std::unique_lock<std::mutex> lock(m);
		count += num;
		cv.notify_one();
	}
};

using Binary_Semaphore = Semaphore<1>;

#endif
