#include "PheromoneMap.hpp"

#include <omp.h>

constexpr float k_pheromoneMinIntensity     = 0.f;
constexpr float k_pheromoneMaxIntensity     = 255.f;
constexpr float k_lostEvaporationMultiplier = 16.f;

PheromoneMap::PheromoneMap(size_t width, size_t height, float evaporationRate)
		:
		m_width(static_cast<int>(width)), m_height(static_cast<int>(height)), m_evaporationRate(evaporationRate),
		m_colorMap(m_width, m_height, {0, 0, 0, 0}),
		m_boundsChecker(0, m_width, 0, m_height)
{
	for ( int i = 0; i < PheromoneType::Amount; ++i )
	{
		m_pheromones[i].resize(m_height);
		for ( int y = 0; y < m_height; ++y )
		{
			m_pheromones[i][y].resize(m_width);
			for ( int x = 0; x < m_width; ++x )
			{
				m_pheromones[i][y][x] = 0.f;
			}
			m_pheromones[i][y].shrink_to_fit();
		}
		m_pheromones[i].shrink_to_fit();
	}

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
		m_colorMap.Update();
		m_visualUpdateTimer.Reset();
	}
}

void PheromoneMap::Clear()
{
	for ( int y = 0; y < m_height; ++y )
	{
		for ( int x = 0; x < m_width; ++x )
		{
			m_pheromones[Food][y][x] = 0.f;
			m_pheromones[Nest][y][x] = 0.f;
			m_pheromones[Lost][y][x] = 0.f;
			UpdateColor(x, y);
		}
	}
	m_colorMap.Update();
}

void PheromoneMap::Add(Type pheromoneType, int x, int y, float intensity)
{
	if ( !m_boundsChecker.IsInBounds(x, y))
	{
		return;
	}

	m_pheromones[pheromoneType][y][x] = std::max(m_pheromones[pheromoneType][y][x], intensity);
}

void PheromoneMap::Substract(PheromoneMap::Type pheromoneType, int x, int y, float intensity)
{
	if ( !m_boundsChecker.IsInBounds(x, y))
	{
		return;
	}

	m_pheromones[pheromoneType][y][x] = std::max(m_pheromones[pheromoneType][y][x] - intensity, 0.f);
}

void PheromoneMap::Set(Type pheromoneType, int x, int y, float intensity)
{
	if ( !m_boundsChecker.IsInBounds(x, y))
	{
		return;
	}

	m_pheromones[pheromoneType][y][x] = intensity;
}

void PheromoneMap::Draw() const
{
	m_colorMap.Draw();
}

void PheromoneMap::Evaporate()
{
#pragma omp parallel for default(none) shared(m_pheromones, m_evaporationRate, k_pheromoneMinIntensity)
	for ( int y = 0; y < m_height; ++y )
	{
		for ( int x = 0; x < m_width; ++x )
		{
			m_pheromones[Food][y][x] = std::max(m_pheromones[Food][y][x] - m_evaporationRate, k_pheromoneMinIntensity);
			m_pheromones[Nest][y][x] = std::max(m_pheromones[Nest][y][x] - m_evaporationRate, k_pheromoneMinIntensity);
			m_pheromones[Lost][y][x] = std::max(
					m_pheromones[Lost][y][x] - m_evaporationRate * k_lostEvaporationMultiplier,
					k_pheromoneMinIntensity);
			UpdateColor(x, y);
		}
	}
}

void PheromoneMap::UpdateColor(int x, int y)
{
	auto &color = m_colorMap.GetMutable(x, y);
	auto r      = static_cast<unsigned char>(m_pheromones[Lost][y][x]);
	auto g      = static_cast<unsigned char>(m_pheromones[Food][y][x]);
	auto b      = static_cast<unsigned char>(m_pheromones[Nest][y][x]);

	if ( r > g )
	{
		color.r = static_cast<unsigned char>(m_pheromones[Lost][y][x]);
		color.g = 0;
		color.b = 0;
	}
	else
	{
		color.r = 0;
		color.g = static_cast<unsigned char>(m_pheromones[Food][y][x]);
		color.b = static_cast<unsigned char>(m_pheromones[Nest][y][x]);
	}

	int sum = color.r;
	sum += color.g;
	sum += color.b;

	color.a = static_cast<unsigned char>(std::min(sum, 255));
//	color.a = sum >= (255 * 3) ? 0 : 255;
}
