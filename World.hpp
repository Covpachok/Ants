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
#include "TileMap.hpp"
#include "BoundsChecker.hpp"

class World
{
public:
//	enum TileType
//	{
//		None, Food, Wall, Amount
//	};
//
//	struct Tile
//	{
//		TileType type   = None;
//		int      amount = 0;
//	};

public:
	World(const Settings &settings);
	~World();

	void Init();

	void Update(double delta);

	inline TileMap &GetTileMap() { return *m_tileMap; };
	inline PheromoneMap &GetFoodPheromoneMap() { return *m_foodPheromoneMap; };
	inline PheromoneMap &GetHomePheromoneMap() { return *m_homePheromoneMap; };

	inline const TileMap &GetTileMap() const { return *m_tileMap; };
	inline const PheromoneMap &GetFoodPheromoneMap() const { return *m_foodPheromoneMap; };
	inline const PheromoneMap &GetHomePheromoneMap() const { return *m_homePheromoneMap; };

	void AddHomePheromone(int x, int y, float intensity);
	void AddFoodPheromone(int x, int y, float intensity);

	void ClearPheromones();
	void ClearMap();

	void Draw(bool h = true, bool f = true) const;

	inline IntVec2 ScreenToWorld(float x, float y) const;
	inline IntVec2 ScreenToWorld(Vector2 pos) const;
	inline Vector2 WorldToScreen(int x, int y) const;

	inline float GetScreenToWorldRatio() const;
	inline float GetScreenToWorldInverseRatio() const;

	inline Vector2 GetScreenHomePos() const;
	inline float GetScreenHomeRadius() const;

	void Reset(int width, int height);
	void Erase();

	inline void IncDeliveredFoodCount();

	inline int GetCollectedFoodAmount() const;
	inline int GetDeliveredFoodAmount() const;

	inline int GetTotalFoodAmount() const;
	inline int GetRemainingFoodAmount() const;

	inline int GetWidth() const;
	inline int GetHeight() const;

	inline float GetScreenWidth() const;
	inline float GetScreenHeight() const;

	const BoundsChecker2D &BoundsChecker() const { return *m_boundsChecker; }

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

	double m_homePheromoneEvaporationRate;
	double m_foodPheromoneEvaporationRate;

	std::unique_ptr<TileMap> m_tileMap;

	std::unique_ptr<PheromoneMap> m_homePheromoneMap;
	std::unique_ptr<PheromoneMap> m_foodPheromoneMap;

	// -------------

	IntVec2 m_homePos;
	int     m_homeRadius;

	Vector2 m_screenHomePos;
	float   m_screenHomeRadius;

	// -------------

	Color m_homeColor;

	int m_collectedFoodAmount = 0;
	int m_deliveredFoodAmount = 0;

	int m_totalFoodAmount     = 0;
	int m_remainingFoodAmount = 0;

	std::vector<IntVec2> m_homeTilePositions;

	std::unique_ptr<BoundsChecker2D> m_boundsChecker;
};

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
