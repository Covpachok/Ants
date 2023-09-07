#ifndef ANTS_WORLD_HPP
#define ANTS_WORLD_HPP

#include <iostream>
#include <utility>

#include <vector>

#include <raylib.h>
#include <rlgl.h>
#include <memory>

#include "Settings.hpp"
#include "IntVec.hpp"
#include "ColorMap.hpp"
#include "PheromoneMap.hpp"

class World
{
public:
	enum TileType
	{
		None, Food, Wall, Amount
	};

	struct Tile
	{
		TileType type   = None;
		int      amount = 0;
	};

public:
	World(const Settings &settings);
	~World();

	void Init();

	void Update(double delta);

	void SetTile(int x, int y, TileType type);
	void DecreaseTile(int x, int y);

	void AddHomePheromone(int x, int y, double intensity);
	void AddFoodPheromone(int x, int y, double intensity);

	void ClearPheromones();
	void ClearMap();

	inline double GetFoodPheromone(int x, int y) const;

	inline double GetHomePheromone(int x, int y) const;

	inline double UnsafeGetFoodPheromone(int x, int y) const;

	inline double UnsafeGetHomePheromone(int x, int y) const;

	inline const Tile &GetTile(int x, int y) const;

	inline const Tile &UnsafeGetTile(int x, int y) const;

	void Draw(bool h = true, bool f = true) const;

	inline IntVec2 ScreenToWorld(float x, float y) const;

	inline IntVec2 ScreenToWorld(Vector2 pos) const;

	inline Vector2 WorldToScreen(int x, int y) const;

	inline float GetScreenToWorldRatio() const;
	inline float GetScreenToWorldInverseRatio() const;

	inline Vector2 GetScreenHomePos() const;
	inline float GetScreenHomeRadius() const;

	inline bool IsInBounds(int x, int y) const;
	inline bool IsInBounds(IntVec2 pos) const;

	void Reset(int width, int height);
	void Erase();

	inline void IncDeliveredFoodCount();

	int GetCollectedFoodAmount() const;
	int GetDeliveredFoodAmount() const;

	int GetTotalFoodAmount() const;
	int GetRemainingFoodAmount() const;

	int GetWidth() const;
	int GetHeight() const;

	float GetScreenWidth() const;
	float GetScreenHeight() const;

private:
	inline int ToMapIndex(int x, int y) const;

	void GenerateMap();

private:
	const WorldSettings *m_worldSettings;

	int m_width;
	int m_height;

	float m_screenWidth;
	float m_screenHeight;

	float m_screenToWorldRatio;
	float m_screenToWorldInverseRatio;

	// -------------

	Tile **m_worldMap;

	Texture m_worldTexture;
	Color   *m_worldColorMap;

	double m_homePheromoneEvaporationRate;
	double m_foodPheromoneEvaporationRate;

	double **m_homePheromoneMap;
	double **m_foodPheromoneMap;

//	Texture m_homePheromoneTexture{};
//	Color   *m_pheromonesColorMap;

	Texture m_foodPheromoneTexture{};
	Color   *m_foodPheromoneColorMap;

	std::unique_ptr<ColorMap> m_tilesColorMap;
	std::unique_ptr<ColorMap> m_pheromonesColorMap;

	std::unique_ptr<PheromoneMap> m_homePheromoneMap;

	// -------------

	IntVec2 m_homePos;
	int     m_homeRadius;

	Vector2 m_screenHomePos;
	float   m_screenHomeRadius;

	// -------------

	Color m_homeColor;
	Color m_tileColors[k_tilesAmount];
	int   m_tileDefaultAmount[k_tilesAmount];

	int m_collectedFoodAmount = 0;
	int m_deliveredFoodAmount = 0;

	int m_totalFoodAmount     = 0;
	int m_remainingFoodAmount = 0;

	std::vector<IntVec2> m_homeTilePositions;
};

double World::GetFoodPheromone(int x, int y) const
{
	return IsInBounds(x, y) ? m_foodPheromoneMap[y][x] : 0;
}

double World::GetHomePheromone(int x, int y) const
{
	return IsInBounds(x, y) ? m_homePheromoneMap[y][x] : 0;
}


double World::UnsafeGetFoodPheromone(int x, int y) const
{
	return m_foodPheromoneMap[y][x];
}

double World::UnsafeGetHomePheromone(int x, int y) const
{
	return m_homePheromoneMap[y][x];
}

const World::Tile &World::GetTile(int x, int y) const
{
	return IsInBounds(x, y) ? m_worldMap[y][x] : m_worldMap[0][0];
}

const World::Tile &World::UnsafeGetTile(int x, int y) const
{
	return m_worldMap[y][x];
}

IntVec2 World::ScreenToWorld(float x, float y) const
{
	return {x * m_screenToWorldInverseRatio, y * m_screenToWorldInverseRatio};
}

IntVec2 World::ScreenToWorld(Vector2 pos) const
{
	return ScreenToWorld(pos.x, pos.y);
}

Vector2 World::WorldToScreen(int x, int y) const
{
	return {x * m_screenToWorldRatio, y * m_screenToWorldRatio};
}

float World::GetScreenToWorldRatio() const
{
	return m_screenToWorldRatio;
}

float World::GetScreenToWorldInverseRatio() const
{
	return m_screenToWorldInverseRatio;
}

Vector2 World::GetScreenHomePos() const
{
	return m_screenHomePos;
}

float World::GetScreenHomeRadius() const
{
	return m_screenHomeRadius;
}

bool World::IsInBounds(int x, int y) const
{
	return x >= 0 && x < m_width && y >= 0 && y < m_height;
}

bool World::IsInBounds(IntVec2 pos) const
{
	return IsInBounds(pos.x, pos.y);
}

void World::IncDeliveredFoodCount()
{
	++m_deliveredFoodAmount;
}

int World::GetCollectedFoodAmount() const
{
	return m_collectedFoodAmount;
}

int World::GetDeliveredFoodAmount() const
{
	return m_deliveredFoodAmount;
}

int World::GetTotalFoodAmount() const
{
	return m_totalFoodAmount;
}

int World::GetRemainingFoodAmount() const
{
	return m_remainingFoodAmount;
}

int World::GetWidth() const
{
	return m_width;
}

int World::GetHeight() const
{
	return m_height;
}

float World::GetScreenWidth() const
{
	return m_screenWidth;
}

float World::GetScreenHeight() const
{
	return m_screenHeight;
}

int World::ToMapIndex(int x, int y) const
{
	return ( y * m_width ) + x;
}

#endif
