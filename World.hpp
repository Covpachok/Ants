#ifndef ANTS_WORLD_HPP
#define ANTS_WORLD_HPP

#include "raylib.h"
#include "ValueTable.hpp"

class World
{
public:
	enum CellType
	{
		None, Food, Wall
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
	const Cell &GetCell(int x, int y) const;

	void AddHomePheromone(int x, int y, double intensity);
	void AddFoodPheromone(int x, int y, double intensity);

	double GetFoodPheromone(int x, int y) const;
	double GetHomePheromone(int x, int y) const;

	void Draw(bool h = true, bool f = true) const;

	inline std::pair<int, int> ScreenToWorld(float x, float y) const
	{
		return {x / m_screenToWorldRatio, y / m_screenToWorldRatio};
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

	inline Vector2 GetScreenHomePos() const { return m_screenHomePos; }
	inline float GetScreenHomeRadius() const { return m_screenHomeRadius; }

	inline bool IsInBounds(int x, int y) const { return x >= 0 && x < m_width && y >= 0 && y < m_height; }
	inline bool IsInBounds(std::pair<int, int> pos) const { return IsInBounds(pos.first, pos.second); }

	void Reset(int width, int height);
	void Erase();

private:
	inline int ToMapIndex(int x, int y) const { return ( y * m_width ) + x; }

private:
	const WorldValueTable *m_valueTable;

	int m_width;
	int m_height;

	float m_screenToWorldRatio;

	// -------------

	Cell *m_worldMap;

	Image   m_worldImage;
	Texture m_worldTexture;
	Color   *m_worldColorMap;

	double m_homePheromoneEvaporationRate;
	double m_foodPheromoneEvaporationRate;

	double *m_homePheromoneMap;
	double *m_foodPheromoneMap;

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
};


#endif
