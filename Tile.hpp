#ifndef ANTS_TILE_HPP
#define ANTS_TILE_HPP

enum class TileType
{
	Empty, Food, Wall, Nest
};

class Tile
{
public:
	Tile();

private:
	TileType m_type;
};


#endif //ANTS_TILE_HPP
