#ifndef ANTS_SIMULATION_HPP
#define ANTS_SIMULATION_HPP

#include "ColoniesManager.hpp"
#include "World.hpp"
#include "Settings.hpp"
#include "Brush.hpp"

#include <string>

class Simulation
{
public:
	Simulation();
	~Simulation();

	void Start();

private:
	void HandleInput();
	void Update();
	void Draw();

	void ResetCamera();

	// Should put it in a different class
	void ShowGui();
	void StatisticsGui();
	void SettingsGui();
	void AdvancedSettingsGui();

	void Reset();
private:
	Settings m_settings;

	std::unique_ptr<World> m_world;
	std::unique_ptr<ColoniesManager> m_coloniesManager;

	float m_gameSpeed = 1;

	Camera2D m_camera;

	Brush m_brush;

	// Mess of variables
	bool m_pause = false;

	bool m_drawPheromones = false;
	bool m_drawAnts = true;

	bool m_shouldHandleInput = true;
	bool m_adaptiveSpeed     = false;

	bool m_showAdvancedSettings = false;

	std::string m_saveFilename = "save";
};


#endif
