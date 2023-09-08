#ifndef ANTS_RANDOM_HPP
#define ANTS_RANDOM_HPP

#include <random>

class Random
{
public:
	static float Float(float min, float max)
	{
		std::uniform_real_distribution<float> dist(min, max);
		return dist(m_generator);
	}

	static int Int(int min, int max)
	{
		std::uniform_int_distribution<int> dist(min, max);
		return dist(m_generator);
	}

	static bool Bool(float trueChance)
	{
		std::bernoulli_distribution dist(trueChance);
		return dist(m_generator);
	}

private:
	inline static std::mt19937 m_generator{std::random_device()()};
};

#endif //ANTS_RANDOM_HPP
