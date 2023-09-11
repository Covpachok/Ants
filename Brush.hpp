#ifndef ANTS_BRUSH_HPP
#define ANTS_BRUSH_HPP

#include <functional>
#include "World.hpp"

class Brush
{
public:
	enum BrushType
	{
		Point, Square, Round, Amount
	};

public:
	explicit Brush(TileType paintType = TileType::Food, BrushType brushType = Round, int brushSize = 5) :
			m_paintType(paintType), m_brushType(brushType), m_brushSize(brushSize)
	{
		OnBrushChanged();
	}

	void Paint(World &world, int x, int y);

	void SetPaintType(TileType type) { m_paintType = type; }
	void SetBrushType(BrushType type) { m_brushType = type; OnBrushChanged(); };
	void SetBrushSize(int size) { m_brushSize = size; }

	TileType GetPaintType() const { return m_paintType; }
	BrushType GetBrushType() const { return m_brushType; }
	int GetBrushSize() const { return m_brushSize; }

private:
	void OnBrushChanged();

private:
	TileType m_paintType;
	BrushType       m_brushType;

	int m_brushSize;

	std::function<void(TileMap &, Brush &, int x, int y)> m_paintFunc;
};

#endif //ANTS_BRUSH_HPP
