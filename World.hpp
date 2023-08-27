#ifndef ANTS_WORLD_HPP
#define ANTS_WORLD_HPP

#include <iostream>
#include <utility>

#include <vector>

#include "raylib.h"
#include "rlgl.h"
#include "ValueTable.hpp"

class World
{
public:
	enum CellType
	{
		None, Food, Wall, Amount
	};

	struct Cell
	{
		CellType type;
		int      amount;
	};

public:
	void Init(int width, int height);

	~World();

	void Update(double delta);

	void SetCell(int x, int y, CellType type);
	void DecreaseCell(int x, int y);

	void AddHomePheromone(int x, int y, double intensity);
	void AddFoodPheromone(int x, int y, double intensity);

	inline double GetFoodPheromone(int x, int y) const
	{
		return IsInBounds(x, y) ? m_foodPheromoneMap[y][x] : 0;
	}

	inline double GetHomePheromone(int x, int y) const
	{
		return IsInBounds(x, y) ? m_homePheromoneMap[y][x] : 0;
	}

	inline const Cell &GetCell(int x, int y) const
	{
		return IsInBounds(x, y) ? m_worldMap[y][x] : m_worldMap[0][0];
	}

	void Draw(bool h = true, bool f = true) const;

	inline std::pair<int, int> ScreenToWorld(float x, float y) const
	{
		return {x * m_screenToWorldInverseRatio, y * m_screenToWorldInverseRatio};
	}

	inline std::pair<int, int> ScreenToWorld(Vector2 pos) const
	{
		return ScreenToWorld(pos.x, pos.y);
	}

	inline Vector2 WorldToScreen(int x, int y) const
	{
		return {x * m_screenToWorldRatio, y * m_screenToWorldRatio};
	}

	inline float GetScreenToWorldRatio() const { return m_screenToWorldRatio; }
	inline float GetScreenToWorldInverseRatio() const { return m_screenToWorldInverseRatio; }

	inline Vector2 GetScreenHomePos() const { return m_screenHomePos; }
	inline float GetScreenHomeRadius() const { return m_screenHomeRadius; }

	inline bool IsInBounds(int x, int y) const { return x >= 0 && x < m_width && y >= 0 && y < m_height; }
	inline bool IsInBounds(std::pair<int, int> pos) const { return IsInBounds(pos.first, pos.second); }

	void Reset(int width, int height);
	void Erase();

	inline void IncDeliveredFoodCount() { ++m_deliveredFoodAmount; }

	int GetCollectedFoodAmount() const { return m_collectedFoodAmount; }
	int GetDeliveredFoodAmount() const { return m_deliveredFoodAmount; }

	int GetTotalFoodAmount() const { return m_totalFoodAmount; }
	int GetRemainingFoodAmount() const { return m_remaingingFoodAmount; }

private:
	inline int ToMapIndex(int x, int y) const { return ( y * m_width ) + x; }

	void GenerateMap();


private:
	const WorldValueTable *m_valueTable;

	int m_width;
	int m_height;

	float m_screenToWorldRatio;
	float m_screenToWorldInverseRatio;

	// -------------

	Cell **m_worldMap;

	Image   m_worldImage;
	Texture m_worldTexture;
	Color   *m_worldColorMap;

	double m_homePheromoneEvaporationRate;
	double m_foodPheromoneEvaporationRate;

	double **m_homePheromoneMap;
	double **m_foodPheromoneMap;

	Image   m_homePheromoneImage{};
	Texture m_homePheromoneTexture{};
	Color   *m_homePheromoneColorMap;

	Image   m_foodPheromoneImage{};
	Texture m_foodPheromoneTexture{};
	Color   *m_foodPheromoneColorMap;

	// -------------

	std::pair<int, int> m_homePos;
	int                 m_homeRadius;

	Vector2 m_screenHomePos;
	float   m_screenHomeRadius;

	// -------------

	Color m_homeColor;
	Color m_cellColors[k_cellsAmount];
	int   m_cellDefaultAmount[k_cellsAmount];

	int m_collectedFoodAmount = 0;
	int m_deliveredFoodAmount = 0;

	int m_totalFoodAmount      = 0;
	int m_remaingingFoodAmount = 0;

	std::vector<std::pair<int, int>> m_homeCellPositions;
};


#endif
