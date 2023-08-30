#ifndef ANTS_RANDOM_HPP
#define ANTS_RANDOM_HPP

#include <random>

class Random
{
public:
	Random() :
			m_generator(std::random_device()())
	{
	}

	template<typename T>
	T GenerateReal(T min, T max)
	{
		std::uniform_real_distribution<T> dist(min, max);
		return dist(m_generator);
	}

	template<typename T>
	T GenerateInt(T min, T max)
	{
		std::uniform_int_distribution<T> dist(min, max);
		return dist(m_generator);
	}

	bool GenerateBool(float trueChance)
	{
		std::bernoulli_distribution dist(trueChance);
		return dist(m_generator);
	}

private:
	std::mt19937       m_generator;
};

inline Random g_random;

#endif //ANTS_RANDOM_HPP
