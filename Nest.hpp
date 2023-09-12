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
	explicit Nest(World &world);
	Nest(World &world, const IntVec2& pos);

	void Update(float delta) {};
	void Draw() const;

	void Relocate(const IntVec2 &newPos, TileMap &tileMap);

	const IntVec2 &GetPos() const { return m_pos; };
	const Vector2 &GetScreenPos() const { return m_screenPos; };

private:


private:
	int m_colonyId;

	IntVec2 m_pos;
	Vector2 m_screenPos;

	int   m_size;
	float m_screenSize;

	Color m_color;

	int m_foodStored;
};


#endif //ANTS_NEST_HPP
