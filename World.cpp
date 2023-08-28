#include <algorithm>

#include "World.hpp"

#include "omp.h"
#include <raymath.h>

void World::Init(int width, int height, const WorldValueTable &valueTable)
{
	m_valueTable = &valueTable;

	m_width  = width;
	m_height = height;

	m_screenToWorldRatio        = static_cast<float>(GetScreenWidth()) / static_cast<float>(m_width);
	m_screenToWorldInverseRatio = 1.f / m_screenToWorldRatio;

	// -------------

	m_worldMap = new Cell *[m_height];

	m_homePheromoneMap = new double *[m_height];
	m_foodPheromoneMap = new double *[m_height];

	for ( int i = 0; i < m_height; ++i )
	{
		m_worldMap[i] = new Cell[m_width];

		m_homePheromoneMap[i] = new double[m_width];
		m_foodPheromoneMap[i] = new double[m_width];

		for ( int j = 0; j < m_width; ++j )
		{
			m_worldMap[i][j].type   = CellType::None;
			m_worldMap[i][j].amount = 0;

			m_homePheromoneMap[i][j] = 0;
			m_foodPheromoneMap[i][j] = 0;
		}
	}
	// -------------

	m_homePheromoneImage    = GenImageColor(m_width, m_height, m_valueTable->homePheromoneColor);
	m_homePheromoneTexture  = LoadTextureFromImage(m_homePheromoneImage);
	m_homePheromoneColorMap = LoadImageColors(m_homePheromoneImage);

	m_foodPheromoneImage    = GenImageColor(m_width, m_height, m_valueTable->foodPheromoneColor);
	m_foodPheromoneTexture  = LoadTextureFromImage(m_foodPheromoneImage);
	m_foodPheromoneColorMap = LoadImageColors(m_foodPheromoneImage);

	m_worldImage    = GenImageColor(m_width, m_height, BLACK);
	m_worldTexture  = LoadTextureFromImage(m_worldImage);
	m_worldColorMap = LoadImageColors(m_worldImage);

	UnloadImage(m_homePheromoneImage);
	UnloadImage(m_foodPheromoneImage);
	UnloadImage(m_worldImage);

	// -------------

	for ( int i = 0; i < k_cellsAmount; ++i )
	{
		m_cellColors[i]        = m_valueTable->cellColors[i];
		m_cellDefaultAmount[i] = m_valueTable->cellDefaultAmount[i];
	}

	// -------------

	m_homePheromoneEvaporationRate = m_valueTable->homePheromoneEvaporationRate;
	m_foodPheromoneEvaporationRate = m_valueTable->foodPheromoneEvaporationRate;

	// -------------

	m_homeColor = m_valueTable->homePheromoneColor;
	m_homeColor.a = 255;

	m_homeRadius = m_valueTable->homeRadius;
	if ( m_valueTable->centeredHomePos )
	{
		m_homePos = {m_width / 2, m_height / 2};
	}
	else
	{
		m_homePos = {m_valueTable->homePos[0], m_valueTable->homePos[1]};
	}

	m_screenHomePos    = {static_cast<float>(m_homePos.first) * m_screenToWorldRatio,
	                      static_cast<float>(m_homePos.second) * m_screenToWorldRatio};
	m_screenHomeRadius = static_cast<float>(m_homeRadius) * m_screenToWorldRatio;

	for ( int y = -m_homeRadius; y < m_homeRadius; ++y )
	{
		for ( int x = -m_homeRadius; x < m_homeRadius; ++x )
		{
			if ( x * x + y * y <= m_homeRadius * m_homeRadius )
			{
				m_homeCellPositions.emplace_back(m_homePos.first + x, m_homePos.second + y);
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
	for ( auto &pos: m_homeCellPositions )
	{
		m_homePheromoneMap[pos.second][pos.first] = 9998;
	}

#if 0
	for ( int y = -m_homeRadius; y < m_homeRadius; ++y )
	{
		for ( int x = -m_homeRadius; x < m_homeRadius; ++x )
		{
			if ( x * x + y * y <= m_homeRadius * m_homeRadius )
			{
				m_homePheromoneMap[m_homePos.second + y][m_homePos.first + x] = 1000;
			}
		}
	}
#endif

	const double homeEvapRate = m_homePheromoneEvaporationRate * delta;
	const double foodEvapRate = m_foodPheromoneEvaporationRate * delta;

#pragma omp parallel for default(none) shared(delta, homeEvapRate, foodEvapRate)
	for ( int y = 0; y < m_height; ++y )
	{
		double *homePheromoneRow = m_homePheromoneMap[y];
		double *foodPheromoneRow = m_foodPheromoneMap[y];

		const int rowIndex = y * m_width;

		for ( int x = 0; x < m_width; ++x )
		{
			double &homePheromone = homePheromoneRow[x];
			double &foodPheromone = foodPheromoneRow[x];

			homePheromone = std::max(homePheromone - homeEvapRate, 0.0);
			foodPheromone = std::max(foodPheromone - foodEvapRate, 0.0);

			const int index = rowIndex + x;
			m_homePheromoneColorMap[index].a = static_cast<unsigned char>(std::min(homePheromone, 255.0));
			m_foodPheromoneColorMap[index].a = static_cast<unsigned char>(std::min(foodPheromone, 255.0));
		}
	}


	UpdateTexture(m_homePheromoneTexture, m_homePheromoneColorMap);
	UpdateTexture(m_foodPheromoneTexture, m_foodPheromoneColorMap);
}

void World::SetCell(int x, int y, CellType type)
{
	if ( !IsInBounds(x, y))
	{
		return;
	}

	auto prevCell = m_worldMap[y][x];
	m_worldMap[y][x].type   = type;
	m_worldMap[y][x].amount = m_cellDefaultAmount[type];

	auto mapIndex = ToMapIndex(x, y);
	m_worldColorMap[mapIndex] = m_cellColors[type];
	if ( type != None )
	{
		m_homePheromoneMap[y][x] = 0;
		m_foodPheromoneMap[y][x] = 0;
	}

	if ( type == Food )
	{
		if ( prevCell.type == Food )
		{
			m_totalFoodAmount += m_cellDefaultAmount[Food] - prevCell.amount;
			m_remainingFoodAmount += m_cellDefaultAmount[Food] - prevCell.amount;
		}
		else
		{
			m_totalFoodAmount += m_cellDefaultAmount[Food];
			m_remainingFoodAmount += m_cellDefaultAmount[Food];
		}
	}

//	UpdateTexture(m_worldTexture, m_worldColorMap);
	UpdateTextureRec(m_worldTexture, {static_cast<float>(x), static_cast<float>(y), 1, 1}, &m_cellColors[type]);
}

void World::DecreaseCell(int x, int y)
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
		SetCell(x, y, None);
	}
}

void World::AddHomePheromone(int x, int y, double intensity)
{
	if ( !IsInBounds(x, y) || m_worldMap[y][x].type != None )
	{
		return;
	}
	m_homePheromoneMap[y][x] = std::min(m_homePheromoneMap[y][x] + intensity, 255.0);
}

void World::AddFoodPheromone(int x, int y, double intensity)
{
	if ( !IsInBounds(x, y) || m_worldMap[y][x].type != None )
	{
		return;
	}

	m_foodPheromoneMap[y][x] = std::min(m_foodPheromoneMap[y][x] + intensity, 255.0);
}

void World::ClearPheromones()
{
	for ( int y = 0; y < m_height; ++y )
	{
		for ( int x = 0; x < m_width; ++x )
		{
			m_homePheromoneMap[y][x] = 0;
			m_foodPheromoneMap[y][x] = 0;
		}
	}
}

void World::Draw(bool h, bool f) const
{
	Rectangle src  = {0, 0, (float) m_width, (float) m_height};
	Rectangle dest = {0, 0, (float) GetScreenWidth(), (float) GetScreenHeight()};
	DrawTexturePro(m_worldTexture, src, dest, {0, 0}, 0, WHITE);
	if ( h )
	{
		DrawTexturePro(m_homePheromoneTexture, src, dest, {0, 0}, 0, WHITE);
	}
	if ( f )
	{
		DrawTexturePro(m_foodPheromoneTexture, src, dest, {0, 0}, 0, WHITE);
	}

	DrawCircleSector(m_screenHomePos, m_screenHomeRadius, 0.f, 360.f, 18, m_homeColor);
}

void World::Erase()
{
	m_homeCellPositions.clear();

	for ( int y = 0; y < m_height; ++y )
	{
		delete[] m_worldMap[y];
		delete[] m_homePheromoneMap[y];
		delete[] m_foodPheromoneMap[y];
	}

	delete[] m_worldMap;
	delete[] m_homePheromoneMap;
	delete[] m_foodPheromoneMap;

//	UnloadImage(m_homePheromoneImage);
//	UnloadImage(m_foodPheromoneImage);
//	UnloadImage(m_worldImage);

	UnloadTexture(m_homePheromoneTexture);
	UnloadTexture(m_foodPheromoneTexture);
	UnloadTexture(m_worldTexture);

	UnloadImageColors(m_homePheromoneColorMap);
	UnloadImageColors(m_foodPheromoneColorMap);
	UnloadImageColors(m_worldColorMap);
}

void World::Reset(int width, int height)
{
	Erase();
	Init(width, height, *m_valueTable);
}

void World::GenerateMap()
{
	std::function<void(int, int, Color)> genFunc;

	auto foodOnly = [&](int x, int y, Color pixel) {
		if ( pixel.r >= m_valueTable->mapGenFoodLowThreshold &&
		     pixel.r <= m_valueTable->mapGenFoodHighThreshold )
		{
			SetCell(x, y, Food);
		}
	};

	auto wallsOnly = [&](int x, int y, Color pixel) {
		if ( pixel.r >= m_valueTable->mapGenWallLowThreshold &&
		     pixel.r <= m_valueTable->mapGenWallHighThreshold )
		{
			SetCell(x, y, Wall);
		}
	};

	auto foodAndWalls = [&](int x, int y, Color pixel) {
		foodOnly(x, y, pixel);
		wallsOnly(x, y, pixel);
	};

	switch ( m_valueTable->mapGenSettings )
	{
		case WorldValueTable::MapGenSettings::FoodOnly:
			genFunc = foodOnly;
			break;

		case WorldValueTable::MapGenSettings::WallsOnly:
			genFunc = wallsOnly;
			break;

		case WorldValueTable::MapGenSettings::FoodAndWalls:
			genFunc = foodAndWalls;
			break;

		default:
			return;
	}

	Image noiseImage = GenImagePerlinNoise(m_width, m_height, GetRandomValue(-1000, 1000),
	                                       GetRandomValue(-1000, 1000), m_valueTable->mapGenNoiseScale);
	ImageBlurGaussian(&noiseImage, m_valueTable->mapGenNoiseBlur);
	Color *noiseColors = LoadImageColors(noiseImage);
	UnloadImage(noiseImage);

	for ( int y = 0; y < m_height; ++y )
	{
		const int rowIndex = y * m_width;
		for ( int x        = 0; x < m_width; ++x )
		{
			const int dx = x - m_homePos.first;
			const int dy = y - m_homePos.second;
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