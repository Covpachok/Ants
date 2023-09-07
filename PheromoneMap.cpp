#include <omp.h>

#include "PheromoneMap.hpp"
#include "ColorMap.hpp"

constexpr float k_pheromoneMinIntensity = 0.f;
constexpr float k_pheromoneMaxIntensity = 255.f;

PheromoneMap::PheromoneMap(size_t width, size_t height, float screenToMapRatio, float evaporationRate)
		:
		m_width(static_cast<int>(width)), m_height(static_cast<int>(height)), m_evaporationRate(evaporationRate),
		m_colorMap(new ColorMap(m_width, m_height, screenToMapRatio, BLACK))
{
	m_pheromones.reserve(m_height);
	for ( int y = 0; y < m_height; ++y )
	{
		m_pheromones[y].reserve(m_width);
		for ( int x = 0; x < m_width; ++x )
		{
			m_pheromones[y][x] = 0.f;
		}
		m_pheromones[y].shrink_to_fit();
	}
	m_pheromones.shrink_to_fit();
}

void PheromoneMap::Update()
{
	const float evapRate = m_evaporationRate;

#pragma omp parallel for default(none) shared(m_pheromones, evapRate, k_pheromoneMinIntensity)
	for ( int y = 0; y < m_height; ++y )
	{
		for ( int x = 0; x < m_width; ++x )
		{
			m_pheromones[y][x] = std::max(m_pheromones[y][x] - evapRate, k_pheromoneMinIntensity);
			m_colorMap->GetMutable(x, y).a = static_cast<unsigned char>(m_pheromones[y][x]);
		}
	}
}

void PheromoneMap::Clear()
{
	for ( auto &row: m_pheromones )
	{
		for ( auto &pheromone: row )
		{
			pheromone = 0.f;
		}
	}
}

void PheromoneMap::Set(int x, int y, float intensity)
{
	if ( !IsInBounds(x, y, 0, m_width, 0, m_height) ||
	     intensity < k_pheromoneMinIntensity || intensity > k_pheromoneMaxIntensity )
	{
		return;
	}

	m_pheromones[y][x] = intensity;
}

void PheromoneMap::Draw() const
{
	m_colorMap->Update();
	m_colorMap->Draw();
}
