#include "World.hpp"

#include <algorithm>
#include <raymath.h>

#include "Random.hpp"
#include "omp.h"

World::World(const Settings &settings)
{
	m_worldSettings = &settings.GetWorldSettings();
}

void World::Init()
{
	m_width  = m_worldSettings->mapWidth;
	m_height = m_worldSettings->mapHeight;

	m_boundsChecker = std::make_unique<BoundsChecker2D>(0, m_width, 0, m_height);

	m_screenToWorldRatio        = m_worldSettings->screenToMapRatio;
	m_screenToWorldInverseRatio = 1.f / m_screenToWorldRatio;

	m_screenWidth  = m_width * m_screenToWorldRatio;
	m_screenHeight = m_height * m_screenToWorldRatio;

	// -------------


	m_tileMap = std::make_unique<TileMap>(m_width, m_height);

	// -------------

	// -------------

	m_homePheromoneEvaporationRate = m_worldSettings->homePheromoneEvaporationRate;
	m_foodPheromoneEvaporationRate = m_worldSettings->foodPheromoneEvaporationRate;


	m_homePheromoneMap = std::make_unique<PheromoneMap>(m_width, m_height,
	                                                    m_homePheromoneEvaporationRate,
	                                                    m_worldSettings->homePheromoneColor);
	m_foodPheromoneMap = std::make_unique<PheromoneMap>(m_width, m_height,
	                                                    m_foodPheromoneEvaporationRate,
	                                                    m_worldSettings->foodPheromoneColor);

	// -------------

	m_homeColor = m_worldSettings->homePheromoneColor;
	m_homeColor.a = 255;

	m_homeRadius = m_worldSettings->homeRadius;
	m_homePos    = m_worldSettings->homePos;

	m_screenHomePos    = {static_cast<float>(m_homePos.x) * m_screenToWorldRatio,
	                      static_cast<float>(m_homePos.y) * m_screenToWorldRatio};
	m_screenHomeRadius = static_cast<float>(m_homeRadius) * m_screenToWorldRatio;

	for ( int y = -m_homeRadius; y < m_homeRadius; ++y )
	{
		for ( int x = -m_homeRadius; x < m_homeRadius; ++x )
		{
			if ( x * x + y * y <= m_homeRadius * m_homeRadius )
			{
				m_homeTilePositions.emplace_back(m_homePos.x + x, m_homePos.y + y);
			}
		}
	}

	m_collectedFoodAmount = 0;
	m_deliveredFoodAmount = 0;
	m_remainingFoodAmount = 0;
	m_totalFoodAmount     = 0;

	GenerateMap();

	m_nests.push_back(std::make_unique<Nest>(*this));
}

World::~World()
{
	Erase();
}

void World::Update(double delta)
{
	for ( auto &pos: m_homeTilePositions )
	{
		m_homePheromoneMap->Set(pos, 255);
	}

	m_homePheromoneMap->Update();
	m_foodPheromoneMap->Update();
}

void World::ClearPheromones()
{
	m_homePheromoneMap->Clear();
	m_foodPheromoneMap->Clear();
}

void World::ClearMap()
{
	m_tileMap->Clear();
}

void World::AddNest(const IntVec2& pos)
{
	m_nests.push_back(std::make_unique<Nest>(*this));
}

void World::Draw(bool h, bool f) const
{
	DrawRectangle(-5, -5, m_screenWidth + 10, m_screenHeight + 10, RED);
	DrawRectangle(0, 0, m_screenWidth, m_screenHeight, BLACK);

	m_tileMap->Draw();
	if ( h )
	{
		m_homePheromoneMap->Draw();
	}
	if ( f )
	{
		m_foodPheromoneMap->Draw();
	}

	DrawCircleSector(m_screenHomePos, m_screenHomeRadius, 0.f, 360.f, 18, m_homeColor);
}

void World::Erase()
{
	m_homeTilePositions.clear();
	// ??
}

void World::Reset(int width, int height)
{
	Erase();
	Init();
}

void World::GenerateMap()
{
	std::function<void(int, int, Color)> genFunc;

	const auto foodOnly = [&](int x, int y, Color pixel) {
		if ( pixel.r >= m_worldSettings->mapGenFoodLowThreshold &&
		     pixel.r <= m_worldSettings->mapGenFoodHighThreshold )
		{
			m_tileMap->SetTile({x, y}, TileType::Food);
		}
	};

	const auto wallsOnly = [&](int x, int y, Color pixel) {
		if ( pixel.r >= m_worldSettings->mapGenWallLowThreshold &&
		     pixel.r <= m_worldSettings->mapGenWallHighThreshold )
		{
			m_tileMap->SetTile({x, y}, TileType::Wall);
		}
	};

	const auto foodAndWalls = [&](int x, int y, Color pixel) {
		foodOnly(x, y, pixel);
		wallsOnly(x, y, pixel);
	};

	switch ( m_worldSettings->mapGenSettings )
	{
		case MapGenSettings::FoodOnly:
			genFunc = foodOnly;
			break;

		case MapGenSettings::WallsOnly:
			genFunc = wallsOnly;
			break;

		case MapGenSettings::FoodAndWalls:
			genFunc = foodAndWalls;
			break;

		default:
			return;

	}

	Image noiseImage = GenImagePerlinNoise(m_width, m_width, Random::Int(-10000, 10000),
	                                       Random::Int(-10000, 10000), m_worldSettings->mapGenNoiseScale);
	ImageResizeCanvas(&noiseImage, m_width, m_height, 0, 0, BLACK);
	ImageBlurGaussian(&noiseImage, m_worldSettings->mapGenNoiseBlur);
	ImageColorContrast(&noiseImage, m_worldSettings->mapGenNoiseContrast);
	Color *noiseColors = LoadImageColors(noiseImage);

	UnloadImage(noiseImage);

	for ( int y = 0; y < m_height; ++y )
	{
		const int rowIndex = y * m_width;
		for ( int x        = 0; x < m_width; ++x )
		{
			const int dx = x - m_homePos.x;
			const int dy = y - m_homePos.y;
			if ( dx * dx + dy * dy <= m_homeRadius * m_homeRadius * 10 )
			{
				continue;
			}

			const int   index      = rowIndex + x;
			const Color noisePixel = noiseColors[index];

			genFunc(x, y, noisePixel);
		}
	}

	UnloadImageColors(noiseColors);
}
