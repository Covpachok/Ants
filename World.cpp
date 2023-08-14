#include <cassert>
#include <algorithm>
#include "World.hpp"

const Color kFoodPheromoneColor = {127, 0, 0, 0};
const Color kHomePheromoneColor = {0, 0, 127, 0};

const Color kCellColors[2]        = {{0, 0,   0, 255},
                                     {0, 127, 0, 255}};
const int   kCellDefaultAmount[2] = {0, 100};

void World::Init(int width, int height, double homeEvaporationRate, double foodEvaporationRate)
{
	m_width  = width;
	m_height = height;

	m_worldMap = new Cell[m_height * m_width];

	m_homePheromoneEvaporationRate = homeEvaporationRate;
	m_foodPheromoneEvaporationRate = foodEvaporationRate;

	m_homePheromoneMap = new double[m_height * m_width];
	m_foodPheromoneMap = new double[m_height * m_width];

	for ( int i = 0; i < m_width * m_height; ++i )
	{
		m_worldMap[i].type   = CellType::None;
		m_worldMap[i].amount = 0;
		m_homePheromoneMap[i] = 0;
		m_foodPheromoneMap[i] = 0;
	}

	m_homePheromoneImage    = GenImageColor(m_width, m_height, kHomePheromoneColor);
	m_homePheromoneTexture  = LoadTextureFromImage(m_homePheromoneImage);
	m_homePheromoneColorMap = LoadImageColors(m_homePheromoneImage);

	m_foodPheromoneImage    = GenImageColor(m_width, m_height, kFoodPheromoneColor);
	m_foodPheromoneTexture  = LoadTextureFromImage(m_foodPheromoneImage);
	m_foodPheromoneColorMap = LoadImageColors(m_foodPheromoneImage);

	m_worldImage    = GenImageColor(m_width, m_height, BLACK);
	m_worldTexture  = LoadTextureFromImage(m_worldImage);
	m_worldColorMap = LoadImageColors(m_worldImage);

	m_screenToMapRatio = (float) GetScreenWidth() / (float) m_width;
}

World::~World()
{
	Erase();
}

void World::Update(double delta)
{
	int       homeRadius = GetHomeRadius() * 1;
	for ( int y          = -homeRadius; y < homeRadius; ++y )
	{
		for ( int x = -homeRadius; x < homeRadius; ++x )
		{
			if ( x * x + y * y <= homeRadius * homeRadius )
			{
				int index = ToMapIndex(GetHomePos().first + x, GetHomePos().second + y);
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
	if ( !InBounds(x, y))
	{
		return;
	}

	int index = ToMapIndex(x, y);

	m_worldMap[index].type   = type;
	m_worldMap[index].amount = kCellDefaultAmount[type];

	m_worldColorMap[ToMapIndex(x, y)] = kCellColors[type];

	UpdateTexture(m_worldTexture, m_worldColorMap);
}

void World::DecreaseCell(int x, int y)
{
	if ( !InBounds(x, y))
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
	if ( !InBounds(x, y))
	{
		return m_worldMap[ToMapIndex(0, 0)];
	}

	return m_worldMap[ToMapIndex(x, y)];
}

void World::AddHomePheromone(int x, int y, double intensity)
{
	if ( !InBounds(x, y))
	{
		return;
	}

	int index = ToMapIndex(x, y);

	m_homePheromoneMap[index] = std::min(m_homePheromoneMap[index] + intensity, 255.0);
}

void World::AddFoodPheromone(int x, int y, double intensity)
{
	if ( !InBounds(x, y))
	{
		return;
	}

	int index = ToMapIndex(x, y);

	m_foodPheromoneMap[index] = std::min(m_foodPheromoneMap[index] + intensity, 255.0);
}

double World::GetFoodPheromone(int x, int y) const
{
	if ( !InBounds(x, y))
	{
		return 0;
	}

	return m_foodPheromoneMap[ToMapIndex(x, y)];
}

double World::GetHomePheromone(int x, int y) const
{
	if ( !InBounds(x, y))
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
void World::Reset(int width, int height, double homeEvaporationRate, double foodEvaporationRate)
{
	Erase();
	Init(width, height, homeEvaporationRate, foodEvaporationRate);
}

