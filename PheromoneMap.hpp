#ifndef ANTS_PHEROMONEMAP_HPP
#define ANTS_PHEROMONEMAP_HPP

#include <cstdint>
#include <vector>
#include <memory>
#include <raylib.h>

#include "IntVec.hpp"
#include "BoundsChecker.hpp"
#include "Timer.hpp"

class ColorMap;

class PheromoneMap
{
public:
	PheromoneMap(size_t width, size_t height, float screenToMapRatio, float evaporationRate, Color pheromoneColor);

	void Update();

	void Clear();

	void Add(int x, int y, float intensity);
	inline void Add(const IntVec2 &pos, float intensity);

	void Set(int x, int y, float intensity);
	inline void Set(const IntVec2 &pos, float intensity);

	inline float Get(int x, int y) const;
	inline float Get(const IntVec2 &pos) const;

	inline float UnsafeGet(int x, int y) const;
	inline float UnsafeGet(const IntVec2 &pos) const;

	void Draw() const;

private:
	int m_width, m_height;

	float m_evaporationRate;

	std::vector<std::vector<float>> m_pheromones;

	std::unique_ptr<ColorMap> m_colorMap;

	Timer m_updateTimer;
	Timer m_visualUpdateTimer;
};


void PheromoneMap::Add(const IntVec2 &pos, float intensity)
{
	Add(pos.x, pos.y, intensity);
}

void PheromoneMap::Set(const IntVec2 &pos, float intensity)
{
	Set(pos.x, pos.y, intensity);
}

float PheromoneMap::Get(int x, int y) const
{
	return IsInBounds(x, y, 0, m_width, 0, m_height) ? m_pheromones[y][x] : 0.f;
}
float PheromoneMap::Get(const IntVec2 &pos) const
{
	return Get(pos.x, pos.y);
}

float PheromoneMap::UnsafeGet(int x, int y) const
{
	return m_pheromones[y][x];
}

float PheromoneMap::UnsafeGet(const IntVec2 &pos) const
{
	return UnsafeGet(pos.x, pos.y);
}

#endif //ANTS_PHEROMONEMAP_HPP
