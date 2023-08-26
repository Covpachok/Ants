#include <algorithm>
#include <iostream>
#include "World.hpp"

void World::Init(int width, int height)
{
	m_valueTable = &g_valueTable.GetWorldTable();

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
}

World::~World()
{
	Erase();
}

void World::Update(double delta)
{
	int       index = 0;
	for ( int y     = -m_homeRadius; y < m_homeRadius; ++y )
	{
		for ( int x = -m_homeRadius; x < m_homeRadius; ++x )
		{
			if ( x * x + y * y <= m_homeRadius * m_homeRadius )
			{
				m_homePheromoneMap[m_homePos.second + y][m_homePos.first + x] = 1000;
			}
		}
	}

	for ( int y = 0; y < m_height; ++y )
	{
		for ( int x = 0; x < m_width; ++x )
		{
			index = ToMapIndex(x, y);
			m_homePheromoneMap[y][x] = std::max(m_homePheromoneMap[y][x] - ( m_homePheromoneEvaporationRate * delta ),
			                                    0.0);
			m_foodPheromoneMap[y][x] = std::max(m_foodPheromoneMap[y][x] - ( m_foodPheromoneEvaporationRate * delta ),
			                                    0.0);
			m_homePheromoneColorMap[index].a = (unsigned char) std::min(m_homePheromoneMap[y][x], 255.0);
			m_foodPheromoneColorMap[index].a = (unsigned char) std::min(m_foodPheromoneMap[y][x], 255.0);
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

	m_worldMap[y][x].type   = type;
	m_worldMap[y][x].amount = m_cellDefaultAmount[type];

	auto mapIndex = ToMapIndex(x, y);
	m_worldColorMap[mapIndex] = m_cellColors[type];
	m_homePheromoneMap[y][x]  = 0;
	m_foodPheromoneMap[y][x]  = 0;

	UpdateTexture(m_worldTexture, m_worldColorMap);
}

void World::DecreaseCell(int x, int y)
{
	if ( !IsInBounds(x, y))
	{
		return;
	}

	--m_worldMap[y][x].amount;

	if ( m_worldMap[y][x].amount <= 0 )
	{
		SetCell(x, y, None);
	}
}

void World::AddHomePheromone(int x, int y, double intensity)
{
	if ( !IsInBounds(x, y))
	{
		return;
	}

	m_homePheromoneMap[y][x] = std::min(m_homePheromoneMap[y][x] + intensity, 255.0);
}

void World::AddFoodPheromone(int x, int y, double intensity)
{
	if ( !IsInBounds(x, y))
	{
		return;
	}

	m_foodPheromoneMap[y][x] = std::min(m_foodPheromoneMap[y][x] + intensity, 255.0);
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
	for ( int y = 0; y < m_height; ++y )
	{
		delete[] m_worldMap[y];
		delete[] m_homePheromoneMap[y];
		delete[] m_foodPheromoneMap[y];
	}

	delete[] m_worldMap;
	delete[] m_homePheromoneMap;
	delete[] m_foodPheromoneMap;

	UnloadImage(m_homePheromoneImage);
	UnloadImage(m_foodPheromoneImage);
	UnloadImage(m_worldImage);

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
	Init(width, height);
}

