#ifndef ANTS_WORLD_HPP
#define ANTS_WORLD_HPP

#include "raylib.h"

class World
{
public:
	enum CellType
	{
		None, Food
	};

	struct Cell
	{
		CellType type;
		int      amount;
	};

public:
	void Init(int width, int height, double homeEvaporationRate, double foodEvaporationRate);

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

	std::pair<int, int> ScreenToMap(float x, float y) const
	{
		return {x / m_screenToMapRatio, y / m_screenToMapRatio};
	}

	Vector2 MapToScreen(int x, int y) const
	{
		return {x * m_screenToMapRatio, y * m_screenToMapRatio};
	}

	float GetScreenToMapRatio() const { return m_screenToMapRatio; }

	std::pair<int, int> GetHomePos() const { return {m_width / 2, m_height / 2}; }
	int GetHomeRadius() const { return 5; }

	Vector2 GetScreenHomePos() const { return {GetScreenWidth() / 2.f, GetScreenHeight() / 2.f}; }
	float GetScreenHomeRadius() const { return GetHomeRadius() * m_screenToMapRatio; }

	void Reset(int width, int height, double homeEvaporationRate, double foodEvaporationRate);

private:
	void Erase();

	int ToMapIndex(int x, int y) const { return ( y * m_width ) + x; }
	bool InBounds(int x, int y) const { return x >= 0 && x < m_width && y >= 0 && y < m_height; }

private:
	int m_width;
	int m_height;

	float m_screenToMapRatio;

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
};


#endif
