#ifndef ANTS_NEST_HPP
#define ANTS_NEST_HPP

#include <raylib.h>

#include "IntVec.hpp"

class World;
class PheromoneMap;
class Ant;
class TileMap;

class Nest
{
public:
	Nest(const IntVec2 &pos, const World &world);

	void Update(float delta);
	void Draw() const;

	void Relocate(const IntVec2 &newPos, TileMap &tileMap);

	IntVec2 GetPos() const;
	IntVec2 GetScreenPos() const;

private:
	int m_id;

	IntVec2 m_pos;
	int     m_size;

	int m_foodStoraged;

//	std::vector<
};


#endif //ANTS_NEST_HPP
