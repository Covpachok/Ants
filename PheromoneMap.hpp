#ifndef ANTS_PHEROMONEMAP_HPP
#define ANTS_PHEROMONEMAP_HPP

#include <cstdint>
#include <vector>
#include <memory>

#include "IntVec.hpp"
#include "BoundsChecker.hpp"

class ColorMap;

class PheromoneMap
{
public:
	PheromoneMap(size_t width, size_t height, float screenToMapRatio, float evaporationRate);

	void Update();

	void Clear();

	void Set(int x, int y, float intensity);
	inline void Set(const IntVec2 &pos, float intensity);

	inline float Get(int x, int y) const;
	inline float Get(const IntVec2 &pos) const;

	void Draw() const;

private:
	int m_width, m_height;

	float m_evaporationRate;

	std::vector<std::vector<float>> m_pheromones;

	std::unique_ptr<ColorMap> m_colorMap;
};



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

#endif //ANTS_PHEROMONEMAP_HPP
