#ifndef RANDOMBETWEEN
#define RANDOMBETWEEN

#include <chrono>
#include <random>


namespace rand_between
{
    std::random_device rd{};
    std::seed_seq ss{rd(), rd(), rd(), rd(), rd(), rd(), rd()};
	std::mt19937 mt {ss};

	// Generate a random int between [min, max] (inclusive)
	inline int rand_between(int min, int max)
	{
		return std::uniform_int_distribution{min, max}(mt);
	}

    // Generate a random int between [min, max] (inclusive)
	inline double rand_between(double min, double max)
	{
		return std::uniform_real_distribution{min, max}(mt);
	}
}

#endif