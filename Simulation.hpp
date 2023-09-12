#ifndef ANTS_SIMULATION_HPP
#define ANTS_SIMULATION_HPP

#include "Ant.hpp"
#include "World.hpp"
#include "Settings.hpp"
#include "Brush.hpp"

#include <vector>
#include <execution>
#include <string>
#include <list>

class Simulation
{
public:
	Simulation();
	~Simulation();
	void Start();

private:
	void Init();
	void InitAnts();

	void HandleInput();
	void Update();
	void Draw();

	void Reset();
	void ResetAnts();
	void ResetCamera();

	// Should put it in a different class
	void ShowGui();
	void StatisticsGui();
	void SettingsGui();
	void AdvancedSettingsGui();

private:
	Settings m_settings;

	// Really should put ants in World class
	std::vector<Ant>       m_ants;
	std::unique_ptr<World> m_world;

	float m_gameSpeed = 1;

	Camera2D m_camera;

	Brush m_brush;

	// Mess of variables
	bool m_pause = false;

	bool m_drawHomePheromones = false;
	bool m_drawFoodPheromones = false;
	bool m_drawAnts           = true;

	bool m_shouldHandleInput = true;
	bool m_adaptiveSpeed     = false;

	bool m_showAdvancedSettings = false;

	bool m_choosingHomePos = false;
	bool m_showGui         = true;
	bool m_paintingEnabled = true;
	bool m_spawnNewNest    = false;

	std::string m_saveFilename = "save";
};


#endif
