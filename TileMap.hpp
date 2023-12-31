#ifndef ANTS_TILEMAP_HPP
#define ANTS_TILEMAP_HPP

#include <vector>
#include <memory>

#include "BoundsChecker.hpp"
#include "IntVec.hpp"

#include "Tile.hpp"
#include "ColorMap.hpp"

class Nest;

class TileMap
{
public:
	TileMap(int width, int height);

	void SetTile(const IntVec2 &pos, TileType newType);
	void UnsafeSetTile(int x, int y, TileType newType);

	void Update();

	void PlaceNest(Nest &nest);

	bool TakeFood(const IntVec2 &pos);

	void Clear();

	inline const Tile &GetTile(const IntVec2 &pos) const;
	inline TileType GetTileType(const IntVec2 &pos) const { return GetTile(pos).GetType(); }

	void Draw() const;

	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }

private:
	void UpdateColorMap(const IntVec2 &pos);
	void UpdateTileColor(const IntVec2 &pos);

private:
	int m_width, m_height;

	std::vector<std::vector<std::unique_ptr<Tile>>> m_tiles;
	std::unique_ptr<ColorMap>                       m_colorMap;

	BoundsChecker2D m_boundsChecker;

	Tile m_errorTile{TileType::eWall};

	Nest *m_nest = nullptr;
};

const Tile &TileMap::GetTile(const IntVec2 &pos) const
{
	return m_boundsChecker.IsInBounds(pos) ? *m_tiles[pos.y][pos.x] : m_errorTile;
}

#endif //ANTS_TILEMAP_HPP
