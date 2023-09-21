#ifndef ANTS_NEST_HPP
#define ANTS_NEST_HPP

#include <raylib.h>
#include "IntVec.hpp"

#include "TileMap.hpp"

#include "Aliases.hpp"

class AntColony;

class Nest
{
public:
	Nest(NestId id, AntColony *colony, const IntVec2 &pos, TileMap &tileMap);

	inline void AddFoodToStorage();
	void Relocate(const IntVec2 &newPos, TileMap &tileMap);

	void SetColony(AntColony *colony) { m_colony = colony; }

	NestId GetId() const { return m_id; }
	AntColony *GetColony() const { return m_colony; }

	int GetSize() const { return m_size; };
	const IntVec2 &GetPos() const { return m_pos; };
	const Vector2 &GetScreenPos() const { return m_screenPos; };

private:
	void OnFoodStoredIncrease();

private:
	AntColony *m_colony;
	NestId    m_id;

	IntVec2 m_pos;
	Vector2 m_screenPos;

	int m_size;

	int m_foodStored;
};

void Nest::AddFoodToStorage()
{
	++m_foodStored;
	OnFoodStoredIncrease();
}

#endif //ANTS_NEST_HPP
