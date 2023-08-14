#ifndef ANTS_SIMULATION_HPP
#define ANTS_SIMULATION_HPP

#include "Ant.hpp"
#include "World.hpp"

#include <list>

class Simulation
{
public:
	Simulation();
	~Simulation();
	void Start();

private:
	void HandleInput();
	void Update(double delta);
	void Draw();

	void Reset();

private:
	std::list<Ant> m_ants;
	World m_world;

	bool m_pause = false;

	bool m_drawHomePheromones = false;
	bool m_drawFoodPheromones = false;
	bool m_drawAnts = true;

	float m_gameSpeed = 1;
};


#endif
