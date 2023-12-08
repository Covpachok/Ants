#ifndef ANTS_SIMULATION_HPP
#define ANTS_SIMULATION_HPP

#include "ColoniesManager.hpp"
#include "World.hpp"
#include "Settings.hpp"
#include "Brush.hpp"
#include "Gui.hpp"

#include <string>

class Gui;

class Simulation
{
	friend class Gui;

	const float k_minGameSpeed = 0.1f;
	const float k_maxGameSpeed = 30.f;

public:
	Simulation();
	~Simulation();

	void Start();

private:
	void HandleInput();
	void Update();
	void Draw();

	void ResetCamera();

	void ShowGui();

	void Reset();
private:
	Settings m_settings;
	Gui m_gui;

	std::unique_ptr<World> m_world;
	std::unique_ptr<ColoniesManager> m_coloniesManager;

	float m_gameSpeed = 1;

	Camera2D m_camera{};

	Brush m_brush;

	// Mess of variables
	bool m_pause = false;

	bool m_drawPheromones = false;
	bool m_drawAnts = true;

	bool m_shouldHandleInput = true;
	bool m_adaptiveSpeed     = true;

	bool m_showGui = true;
	bool m_showAdvancedSettings = false;

	std::string m_saveFilename = "save";
};


#endif
