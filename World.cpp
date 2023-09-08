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

	m_screenToWorldRatio        = m_worldSettings->screenToMapRatio;
	m_screenToWorldInverseRatio = 1.f / m_screenToWorldRatio;

	m_screenWidth  = m_width * m_screenToWorldRatio;
	m_screenHeight = m_height * m_screenToWorldRatio;

	// -------------

	m_tilesColorMap      = std::make_unique<ColorMap>(m_width, m_height, m_screenToWorldRatio,
	                                                  Color{0, 0, 0, 0});
	m_pheromonesColorMap = std::make_unique<ColorMap>(m_width, m_height, m_screenToWorldRatio,
	                                                  BLACK);
	m_worldMap = new Tile *[m_height];

	for ( int i = 0; i < m_height; ++i )
	{
		m_worldMap[i] = new Tile[m_width];
		for ( int j = 0; j < m_width; ++j )
		{
			m_worldMap[i][j].type   = TileType::None;
			m_worldMap[i][j].amount = 0;
		}
	}

	// -------------

	for ( int i = 0; i < k_tilesAmount; ++i )
	{
		m_tileColors[i]        = m_worldSettings->tileColors[i];
		m_tileDefaultAmount[i] = m_worldSettings->tileDefaultAmount[i];
	}

	// -------------

	m_homePheromoneEvaporationRate = m_worldSettings->homePheromoneEvaporationRate;
	m_foodPheromoneEvaporationRate = m_worldSettings->foodPheromoneEvaporationRate;


	m_homePheromoneMap = std::make_unique<PheromoneMap>(m_width, m_height, m_screenToWorldRatio,
	                                                    m_homePheromoneEvaporationRate, m_worldSettings->homePheromoneColor);
	m_foodPheromoneMap = std::make_unique<PheromoneMap>(m_width, m_height, m_screenToWorldRatio,
	                                                    m_foodPheromoneEvaporationRate, m_worldSettings->foodPheromoneColor);

	// -------------

	m_homeColor = m_worldSettings->homePheromoneColor;
	m_homeColor.a = 255;

	m_homeRadius = m_worldSettings->homeRadius;
	if ( m_worldSettings->centeredHomePos )
	{
		m_homePos = {m_width / 2, m_height / 2};
	}
	else
	{
		m_homePos = {m_worldSettings->homePos[0], m_worldSettings->homePos[1]};
	}

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

void World::SetTile(int x, int y, TileType type)
{
	if ( !IsInBounds(x, y))
	{
		return;
	}

	Tile prevTile = m_worldMap[y][x];
	m_worldMap[y][x].type   = type;
	m_worldMap[y][x].amount = m_tileDefaultAmount[type];

	int mapIndex = ToMapIndex(x, y);
//	m_worldColorMap[mapIndex] = m_tileColors[type];
	m_tilesColorMap->Set(mapIndex, m_tileColors[type]);
	if ( type != None )
	{
		m_homePheromoneMap->Set(x, y, 0);
		m_foodPheromoneMap->Set(x, y, 0);
	}

	if ( type == Food )
	{
		if ( prevTile.type == Food )
		{
			m_totalFoodAmount += m_tileDefaultAmount[Food] - prevTile.amount;
			m_remainingFoodAmount += m_tileDefaultAmount[Food] - prevTile.amount;
		}
		else
		{
			m_totalFoodAmount += m_tileDefaultAmount[Food];
			m_remainingFoodAmount += m_tileDefaultAmount[Food];
		}
	}

//	UpdateTexture(m_worldTexture, m_worldColorMap);
//	UpdateTextureRec(m_worldTexture, {static_cast<float>(x), static_cast<float>(y), 1, 1}, &m_tileColors[type]);
	m_tilesColorMap->UpdatePixel(x, y);
}

void World::DecreaseTile(int x, int y)
{
	if ( !IsInBounds(x, y))
	{
		return;
	}

	--m_worldMap[y][x].amount;

	if ( m_worldMap[y][x].type == Food && m_worldMap[y][x].amount >= 0 )
	{
		--m_remainingFoodAmount;
		++m_collectedFoodAmount;
	}

	if ( m_worldMap[y][x].amount <= 0 )
	{
		SetTile(x, y, None);
	}
}

void World::AddHomePheromone(int x, int y, float intensity)
{
	if ( m_worldMap[y][x].type != None )
	{
		return;
	}
	m_homePheromoneMap->Add(x, y, intensity);
}

void World::AddFoodPheromone(int x, int y, float intensity)
{
	if ( m_worldMap[y][x].type != None )
	{
		return;
	}

	m_foodPheromoneMap->Add(x, y, intensity);
}

void World::ClearPheromones()
{
	for ( int y = 0; y < m_height; ++y )
	{
		const int rowIndex = y * m_width;
		for ( int x        = 0; x < m_width; ++x )
		{
			m_homePheromoneMap->Set(x, y, 0);
			m_foodPheromoneMap->Set(x, y, 0);

			const int index = rowIndex + x;
			m_pheromonesColorMap->GetMutable(index).a = 0;
		}
	}

	m_pheromonesColorMap->Update();
}

void World::ClearMap()
{
	for ( int y = 0; y < m_height; ++y )
	{
		for ( int x = 0; x < m_width; ++x )
		{
			m_worldMap[y][x].type   = None;
			m_worldMap[y][x].amount = 0;
			m_tilesColorMap->Set(x, y, m_tileColors[None]);
//			m_worldColorMap[ToMapIndex(x, y)] = m_tileColors[None];
		}
	}

	m_tilesColorMap->Update();
}

void World::Draw(bool h, bool f) const
{
	DrawRectangle(-5, -5, m_screenWidth + 10, m_screenHeight + 10, RED);
	DrawRectangle(0, 0, m_screenWidth, m_screenHeight, BLACK);

	if ( h )
	{
		m_homePheromoneMap->Draw();
	}
	if ( f )
	{
		m_foodPheromoneMap->Draw();
	}
	m_tilesColorMap->Draw();

	DrawCircleSector(m_screenHomePos, m_screenHomeRadius, 0.f, 360.f, 18, m_homeColor);
}

void World::Erase()
{
	m_homeTilePositions.clear();

	for ( int y = 0; y < m_height; ++y )
	{
		delete[] m_worldMap[y];
	}

	delete[] m_worldMap;
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
			SetTile(x, y, Food);
		}
	};

	const auto wallsOnly = [&](int x, int y, Color pixel) {
		if ( pixel.r >= m_worldSettings->mapGenWallLowThreshold &&
		     pixel.r <= m_worldSettings->mapGenWallHighThreshold )
		{
			SetTile(x, y, Wall);
		}
	};

	const auto foodAndWalls = [&](int x, int y, Color pixel) {
		foodOnly(x, y, pixel);
		wallsOnly(x, y, pixel);
	};

	switch ( m_worldSettings->mapGenSettings )
	{
		case WorldSettings::MapGenSettings::FoodOnly:
			genFunc = foodOnly;
			break;

		case WorldSettings::MapGenSettings::WallsOnly:
			genFunc = wallsOnly;
			break;

		case WorldSettings::MapGenSettings::FoodAndWalls:
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
