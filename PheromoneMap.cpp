#include "PheromoneMap.hpp"

#include <omp.h>

constexpr float k_pheromoneMinIntensity = 0.f;
constexpr float k_pheromoneMaxIntensity = 255.f;

PheromoneMap::PheromoneMap(size_t width, size_t height, float evaporationRate, Color pheromoneColor)
		:
		m_width(static_cast<int>(width)), m_height(static_cast<int>(height)),
		m_evaporationRate(evaporationRate)
{
	m_pheromones.resize(m_height);
	for ( int y = 0; y < m_height; ++y )
	{
		m_pheromones[y].resize(m_width);
		for ( int x = 0; x < m_width; ++x )
		{
			m_pheromones[y][x] = 0.f;
		}
		m_pheromones[y].shrink_to_fit();
	}
	m_pheromones.shrink_to_fit();

	m_colorMap = std::make_unique<ColorMap>(m_width, m_height, pheromoneColor);

	m_updateTimer.SetDelay(10);
	m_visualUpdateTimer.SetDelay(50);
}

void PheromoneMap::Update()
{
	m_updateTimer.Update(1);
	if ( m_updateTimer.IsElapsed())
	{
		Evaporate();
		m_updateTimer.Reset();
	}

	m_visualUpdateTimer.Update(1);
	if ( m_visualUpdateTimer.IsElapsed())
	{
		m_colorMap->Update();
		m_visualUpdateTimer.Reset();
	}
}

void PheromoneMap::Clear()
{
	for ( int y = 0; y < m_height; ++y)
	{
		for (int x = 0; x < m_width; ++x)
		{
			m_pheromones[y][x] = 0.f;
			UpdateColor({x, y});
		}
	}
	m_colorMap->Update();
}

void PheromoneMap::Add(int x, int y, float intensity)
{
	if ( !IsInBounds(x, y, 0, m_width, 0, m_height))
	{
		return;
	}

	m_pheromones[y][x] = std::max(m_pheromones[y][x], intensity);
}

void PheromoneMap::Set(int x, int y, float intensity)
{
	if ( !IsInBounds(x, y, 0, m_width, 0, m_height))
	{
		return;
	}

	m_pheromones[y][x] = intensity;
}

void PheromoneMap::Draw() const
{
//	m_colorMap->Update();
	m_colorMap->Draw();
}

void PheromoneMap::Evaporate()
{
#pragma omp parallel for default(none) shared(m_pheromones, m_evaporationRate, k_pheromoneMinIntensity)
	for ( int y = 0; y < m_height; ++y )
	{
		for ( int x = 0; x < m_width; ++x )
		{
			m_pheromones[y][x] = std::max(m_pheromones[y][x] - m_evaporationRate, k_pheromoneMinIntensity);
			UpdateColor({x, y});
		}
	}
}

void PheromoneMap::UpdateColor(const IntVec2 &pos)
{
	m_colorMap->GetMutable(pos).a = static_cast<unsigned char>(m_pheromones[pos.y][pos.x]);
}
