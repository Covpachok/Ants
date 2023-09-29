#ifndef ANTS_PHEROMONEMAP_HPP
#define ANTS_PHEROMONEMAP_HPP

#include <cstdint>
#include <vector>
#include <memory>
#include <raylib.h>
#include <array>

#include "IntVec.hpp"
#include "BoundsChecker.hpp"
#include "Timer.hpp"

#include "ColorMap.hpp"

class PheromoneMap
{
public:
	enum Type
	{
		Food, Nest, Lost, Amount
	};

public:
	PheromoneMap(size_t width, size_t height, float evaporationRate);

	void Update();

	void Clear();

	void Add(Type pheromoneType, int x, int y, float intensity);
	inline void Add(Type pheromoneType, const IntVec2 &pos, float intensity)
	{
		Add(pheromoneType, pos.x, pos.y, intensity);
	}

	void Substract(Type pheromoneType, int x, int y, float intensity);
	inline void Substract(Type pheromoneType, const IntVec2 &pos, float intensity)
	{
		Substract(pheromoneType, pos.x, pos.y, intensity);
	}

	void Set(Type pheromoneType, int x, int y, float intensity);
	inline void Set(Type pheromoneType, const IntVec2 &pos, float intensity)
	{
		Set(pheromoneType, pos.x, pos.y, intensity);
	}

	inline float Get(Type pheromoneType, int x, int y) const;
	inline float Get(Type pheromoneType, const IntVec2 &pos) const { return Get(pheromoneType, pos.x, pos.y); };

	void Draw() const;

private:
	void Evaporate();

	void UpdateColor(int x, int y);

private:
	int m_width, m_height;

	float m_evaporationRate;

	std::array<std::vector<std::vector<float>>, Type::Amount> m_pheromones;

	ColorMap m_colorMap;

	Timer m_updateTimer;
	Timer m_visualUpdateTimer;

	BoundsChecker2D m_boundsChecker;
};

using PheromoneType = PheromoneMap::Type;

float PheromoneMap::Get(PheromoneType pheromoneType, int x, int y) const
{
	return m_boundsChecker.IsInBounds(x, y) ? m_pheromones[pheromoneType][y][x] : 0.f;
}

#endif //ANTS_PHEROMONEMAP_HPP
