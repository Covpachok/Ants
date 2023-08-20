#include <algorithm>
#include <iostream>
#include "World.hpp"

void World::Init(int width, int height)
{
	m_valueTable = &g_valueTable.GetWorldTable();

	m_width  = width;
	m_height = height;

	m_screenToWorldRatio = static_cast<float>(GetScreenWidth()) / static_cast<float>(m_width);

	// -------------

	m_worldMap         = new Cell[m_height * m_width];
	m_homePheromoneMap = new double[m_height * m_width];
	m_foodPheromoneMap = new double[m_height * m_width];

	for ( int i = 0; i < m_width * m_height; ++i )
	{
		m_worldMap[i].type   = CellType::None;
		m_worldMap[i].amount = 0;
		m_homePheromoneMap[i] = 0;
		m_foodPheromoneMap[i] = 0;
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
	for ( int y = -m_homeRadius; y < m_homeRadius; ++y )
	{
		for ( int x = -m_homeRadius; x < m_homeRadius; ++x )
		{
			if ( x * x + y * y <= m_homeRadius * m_homeRadius )
			{
				int index = ToMapIndex(m_homePos.first + x, m_homePos.second + y);
				m_homePheromoneMap[index] = 1000;
			}
		}
	}

	for ( int i = 0; i < m_height * m_width; ++i )
	{
		m_homePheromoneMap[i] = std::max(m_homePheromoneMap[i] - ( m_homePheromoneEvaporationRate * delta ), 0.0);
		m_foodPheromoneMap[i] = std::max(m_foodPheromoneMap[i] - ( m_foodPheromoneEvaporationRate * delta ), 0.0);
		m_homePheromoneColorMap[i].a = (unsigned char) std::min(m_homePheromoneMap[i], 255.0);
		m_foodPheromoneColorMap[i].a = (unsigned char) std::min(m_foodPheromoneMap[i], 255.0);
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

	int index = ToMapIndex(x, y);

	m_worldMap[index].type   = type;
	m_worldMap[index].amount = m_cellDefaultAmount[type];

	auto mapIndex = ToMapIndex(x, y);
	m_worldColorMap[mapIndex]    = m_cellColors[type];
	m_homePheromoneMap[mapIndex] = 0;
	m_foodPheromoneMap[mapIndex] = 0;

	UpdateTexture(m_worldTexture, m_worldColorMap);
}

void World::DecreaseCell(int x, int y)
{
	if ( !IsInBounds(x, y))
	{
		return;
	}

	int index = ToMapIndex(x, y);

	--m_worldMap[index].amount;

	if ( m_worldMap[index].amount <= 0 )
	{
		SetCell(x, y, None);
	}
}

const World::Cell &World::GetCell(int x, int y) const
{
	if ( !IsInBounds(x, y))
	{
		return m_worldMap[ToMapIndex(0, 0)];
	}

	return m_worldMap[ToMapIndex(x, y)];
}

void World::AddHomePheromone(int x, int y, double intensity)
{
	if ( !IsInBounds(x, y))
	{
		return;
	}

	int index = ToMapIndex(x, y);

	m_homePheromoneMap[index] = std::min(m_homePheromoneMap[index] + intensity, 255.0);
}

void World::AddFoodPheromone(int x, int y, double intensity)
{
	if ( !IsInBounds(x, y))
	{
		return;
	}

	int index = ToMapIndex(x, y);

	m_foodPheromoneMap[index] = std::min(m_foodPheromoneMap[index] + intensity, 255.0);
}

double World::GetFoodPheromone(int x, int y) const
{
	if ( !IsInBounds(x, y))
	{
		return 0;
	}

	return m_foodPheromoneMap[ToMapIndex(x, y)];
}

double World::GetHomePheromone(int x, int y) const
{
	if ( !IsInBounds(x, y))
	{
		return 0;
	}

	return m_homePheromoneMap[ToMapIndex(x, y)];
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

