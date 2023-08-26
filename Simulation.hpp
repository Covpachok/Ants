#ifndef ANTS_SIMULATION_HPP
#define ANTS_SIMULATION_HPP

#include "Ant.hpp"
#include "World.hpp"
#include "ValueTable.hpp"

#include <vector>
#include <execution>

class Simulation
{
public:
	Simulation();
	~Simulation();
	void Start();

private:
	void Init();

	void HandleInput();
	void Update();
	void Draw();

	void Reset();

	void DebugGui();

private:
	std::vector<Ant> m_ants;
	World m_world;

	bool m_pause = false;

	bool m_drawHomePheromones = false;
	bool m_drawFoodPheromones = false;
	bool m_drawAnts = true;

	float m_gameSpeed = 1;

	Camera2D m_camera;

	bool m_shouldHandleInput = true;
	bool m_adaptiveSpeed = false;

	bool m_showSimulationVariables = false;

	bool m_choosingHomePos = false;
};


#endif
