#ifndef ANTS_TILE_HPP
#define ANTS_TILE_HPP

#include <raylib.h>
#include <memory>
#include <bitset>

#include "IntVec.hpp"

class Nest;

class Tile
{
public:
	enum TileType
	{
		eEmpty, eWall, eFood, eNest, eAmount
	};

private:
	inline static constexpr std::bitset<Tile::eAmount> s_tilesPassability = 0b1001;

public:
	Tile(const IntVec2 &pos, TileType type);

	void UpdateColorByNeighbors(const std::array<TileType, 4> &neighborTypes);

	void ChangeType(TileType type, Nest *nest = nullptr);

	inline bool Take() { return --m_amount <= 0; };

	inline Nest *GetNest() const { return m_nest; };
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

	int  m_amount;
	Nest *m_nest;
};

using TileType = Tile::TileType;

#endif //ANTS_TILE_HPP
