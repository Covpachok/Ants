#ifndef ANTS_NEST_HPP
#define ANTS_NEST_HPP

#include <raylib.h>
#include "IntVec.hpp"

#include "TileMap.hpp"
#include "AntColony.hpp"

using NestId = uint32_t;

class Nest
{
public:
	Nest(NestId id, TileMap &tileMap, const IntVec2 &pos);

	inline void AddFoodToStorage();
	void Relocate(const IntVec2 &newPos, TileMap &tileMap);

	NestId GetId() const { return m_id; }
	AntColonyId GetColonyId() const { return m_colonyId; }

	const IntVec2 &GetPos() const { return m_pos; };
	const Vector2 &GetScreenPos() const { return m_screenPos; };

private:
	void OnFoodStoredIncrease();

private:
	AntColonyId m_colonyId;
	NestId      m_id;

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
