#ifndef ANTS_NEST_HPP
#define ANTS_NEST_HPP

#include <raylib.h>

#include "IntVec.hpp"
#include "World.hpp"

class Nest
{
public:
	Nest(const IntVec2 &pos, const World &world);

	void Update(float delta);
	void Draw() const;

	void Relocate(const IntVec2 &newPos);

	IntVec2 GetPos() const;
	IntVec2 GetScreenPos() const;

private:
	uint32_t m_id;

	IntVec2 m_pos;
	int     m_size;

	uint32_t m_foodAmount;
};


#endif //ANTS_NEST_HPP
