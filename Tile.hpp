#ifndef ANTS_TILE_HPP
#define ANTS_TILE_HPP

#include <raylib.h>
#include <memory>
#include <bitset>

#include "IntVec.hpp"

class Tile
{
public:
	enum TileType
	{
		Empty, Wall, Food, Nest, Amount
	};

private:
	inline static constexpr std::bitset<Tile::Amount> s_tilesPassability = 0b1001;

public:
	Tile(const IntVec2 &pos, TileType type);

	void UpdateColorByNeighbors(const std::array<TileType, 4> &neighborTypes);

	void ChangeType(TileType type);

	bool Take();

	inline IntVec2 GetPos() const { return m_pos; };
	inline TileType GetType() const { return m_type; };
	inline Color GetColor() const { return m_color; };
	inline Color GetDefaultColor() const { return m_defaultColor; };

	inline bool IsPassable() const { return s_tilesPassability[m_type]; }

private:
	IntVec2  m_pos;
	TileType m_type;

	Color m_color;
	Color m_defaultColor;

	int m_amount;
};

using TileType = Tile::TileType;

#endif //ANTS_TILE_HPP
