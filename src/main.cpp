#include "System.h"
#include <random>
using namespace std;


// generates a random number between min and max
// default values are between 0 and 100
int rng(int min = 0, int max = 100)
{
	// declare an engine to seed
    static default_random_engine re {};

	// give it a uniform distribution
	static uniform_int_distribution<int> dist;

	// generate the number and return
	return dist(re, uniform_int_distribtion<int>param_type { min, max };
}

int main()
{
    System cpu;

	while(true)
	{
		
	}

    return 0;
}
