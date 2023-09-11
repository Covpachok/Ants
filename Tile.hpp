#ifndef ANTS_TILE_HPP
#define ANTS_TILE_HPP

#include <raylib.h>
#include <memory>

#include "IntVec.hpp"

class Tile
{
public:
	enum TileType
	{
		Empty, Wall, Food, Nest, Amount
	};

public:
	Tile(const IntVec2 &pos, TileType type);

	void UpdateColor(const std::array<TileType, 4> &neighborTypes);

	void ChangeType(TileType type);

	bool Take();

	inline IntVec2 GetPos() const { return m_pos; };
	inline TileType GetType() const { return m_type; };
	inline Color GetColor() const { return m_color; };
	inline Color GetDefaultColor() const { return m_defaultColor; };

	inline bool IsPassable() const { return m_passable; }

private:
	IntVec2  m_pos;
	TileType m_type;

	Color m_color;
	Color m_defaultColor;

	int m_amount;

	bool m_passable;
};

using TileType = Tile::TileType;

#endif //ANTS_TILE_HPP
