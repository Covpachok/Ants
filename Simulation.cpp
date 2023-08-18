#include <iostream>
#include "Simulation.hpp"
#include <raylib.h>
#include <raymath.h>
#include <imgui.h>
#include <rlImGui.h>
#include "ColorConvert.hpp"

const int k_screenWidth  = 1280;
const int k_screenHeight = 720;

/*
const float k_foodPheromoneIntensity = 10;
const float k_homePheromoneIntensity = 10;

const float k_foodPheromoneEvaporationRate = 2;
const float k_homePheromoneEvaporationRate = 2;
 */

const float k_foodPheromoneIntensity = 800;
const float k_homePheromoneIntensity = 800;

const float k_foodPheromoneEvaporationRate = 10;
const float k_homePheromoneEvaporationRate = 10;

const double k_fixedTimestep = ( 1000.0 / 60.0 ) / 1000.0;

//const int k_antsAmount = 2000;
const int k_antsAmount = 500;

Simulation::Simulation() :
		m_ants(k_antsAmount)
{
	InitWindow(k_screenWidth, k_screenHeight, "Ants");

	m_world.Init(k_screenWidth / 3, k_screenHeight / 3, k_homePheromoneEvaporationRate, k_foodPheromoneEvaporationRate);

	for ( auto &ant: m_ants )
	{
		ant.Init(k_screenWidth / 2.f, k_screenHeight / 2.f, 40);
	}

//	SetTargetFPS(60);

	std::cout << "FIXED TIMESTEP: " << k_fixedTimestep << std::endl;

	rlImGuiSetup(true);

	m_camera.rotation = 0;
	m_camera.zoom     = 1;
	m_camera.offset   = {0, 0};
	m_camera.target   = {0, 0};
}

Simulation::~Simulation()
{
	rlImGuiShutdown();
	CloseWindow();
}

void Simulation::Start()
{
	double deltaTime = 0;
	while ( !WindowShouldClose())
	{
		Draw();
		HandleInput();
		if ( !m_pause )
		{
			deltaTime += GetFrameTime() * m_gameSpeed;

			while ( deltaTime >= k_fixedTimestep )
			{
				deltaTime -= k_fixedTimestep;
				Update(k_fixedTimestep);
			}
		}
	}
}

void Simulation::HandleInput()
{
	if ( !m_shouldHandleInput )
	{
		return;
	}

	Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), m_camera);
	float   wheel         = GetMouseWheelMove();
	if ( wheel != 0.f )
	{
		m_camera.offset = GetMousePosition();
		m_camera.target = mouseWorldPos;
		m_camera.zoom += wheel * 0.1f;
		m_camera.zoom   = std::max(m_camera.zoom, 0.1f);
	}

	if ( IsMouseButtonDown(MOUSE_BUTTON_LEFT))
	{
		Vector2 delta = Vector2Scale(GetMouseDelta(), -1.0f / m_camera.zoom);
		m_camera.target = Vector2Add(m_camera.target, delta);
	}

	if ( IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
	{
		auto      pos    = m_world.ScreenToMap(mouseWorldPos.x, mouseWorldPos.y);
		int       radius = 5;
		for ( int y      = -radius; y <= radius; ++y )
		{
			for ( int x = -radius; x <= radius; ++x )
			{
				if ( x * x + y * y <= radius * radius + 0.5f )
				{
					m_world.SetCell(pos.first + x, pos.second + y, World::Food);
				}
			}
		}
	}

	if ( IsKeyPressed(KEY_SPACE))
	{
		m_pause = !m_pause;
	}

	if ( IsKeyPressed(KEY_ONE))
	{
		m_drawHomePheromones = !m_drawHomePheromones;
	}
	if ( IsKeyPressed(KEY_TWO))
	{
		m_drawFoodPheromones = !m_drawFoodPheromones;
	}
	if ( IsKeyPressed(KEY_THREE))
	{
		m_drawAnts = !m_drawAnts;
	}

	if ( IsKeyPressed(KEY_EQUAL))
	{
		m_gameSpeed += 1;
		if ( m_gameSpeed > 5 )
		{
			m_gameSpeed = 5;
		}
	}

	if ( IsKeyPressed(KEY_MINUS))
	{
		m_gameSpeed -= 1;
		if ( m_gameSpeed < 1 )
		{
			m_gameSpeed = 1;
		}
	}
}

void Simulation::Update(double delta)
{
	SetWindowTitle(( "Ants FPS:" + std::to_string(GetFPS())).c_str());

	if ( m_adaptiveSpeed )
	{
		const int fps = GetFPS();
		if ( fps < 10 )
		{
			m_gameSpeed = std::max(m_gameSpeed - 0.5 * delta, 1.0);
		}
		else if ( fps < 30 )
		{
			m_gameSpeed = std::max(m_gameSpeed - 0.1 * delta, 1.0);
		}

		if ( fps > 144 )
		{
			m_gameSpeed = std::min(m_gameSpeed + 0.5 * delta, 20.0);
		}
		else if ( fps > 60 )
		{
			m_gameSpeed = std::min(m_gameSpeed + 0.1 * delta, 20.0);
		}
	}

	m_world.Update(delta);

	for ( auto &ant: m_ants )
	{
		ant.Update(delta, m_world);

		/*
			auto pos = ant.GetPos();
	//		int  y   = floor(pos.y), x = floor(pos.x);
			auto p   = m_world.ScreenToMap(pos.x, pos.y);

			if ( ant.IsGotFood())
			{
				m_world.AddFoodPheromone(p.first, p.second, k_foodPheromoneIntensity * delta * ant.GetFoodStrength());
			}
			else
			{
				m_world.AddHomePheromone(p.first, p.second, k_homePheromoneIntensity * delta * ant.GetHomeStrength());
			}
		 */
		}
}

void Simulation::Draw()
{
	BeginDrawing();

	ClearBackground({64, 64, 64, 255});

	BeginMode2D(m_camera);
	m_world.Draw(m_drawHomePheromones, m_drawFoodPheromones);

	if ( m_drawAnts )
	{
		for ( auto &ant: m_ants )
		{
			ant.Draw();
		}
	}
	EndMode2D();

	DebugGui();

	EndDrawing();
}

void Simulation::Reset()
{
	m_world.Reset(k_screenWidth / 4, k_screenHeight / 4, 5, 5);

	m_ants.clear();
	m_ants.resize(k_antsAmount);

	for ( auto &ant: m_ants )
	{
		ant.Init(k_screenWidth / 2.f, k_screenHeight / 2.f, 40);
	}
}

void Simulation::DebugGui()
{
	ImGuiIO &io = ImGui::GetIO();
	m_shouldHandleInput = !( io.WantCaptureMouse || io.WantCaptureKeyboard );

	rlImGuiBegin();

	ImGui::Begin("Ants");

	ImGui::SeparatorText("Draw settings");

	ImGui::Checkbox("Home", &m_drawHomePheromones);
	ImGui::SameLine();
	ImGui::Checkbox("Food", &m_drawFoodPheromones);
	ImGui::SameLine();
	ImGui::Checkbox("Ants", &m_drawAnts);

	ImGui::SeparatorText("Simulation settings");

	ImGui::Checkbox("Adaptive simulation speed", &m_adaptiveSpeed);
	ImGui::SliderFloat("Simulation speed", &m_gameSpeed, 0.1f, 20.f);

	ImGui::SeparatorText("Other");

	ImGui::Checkbox("Show simulation variables", &m_showSimulationVariables);

	bool pressed = ImGui::Button("Reset");


	ImGui::End();

	if ( pressed )
	{
		Reset();
	}

	if ( !m_showSimulationVariables )
	{
		rlImGuiEnd();
		return;
	}

	ImGui::Begin("Simulation variables");

	auto &antsValueTable  = g_valueTable.GetMutableAntsTable();
	auto &worldValueTable = g_valueTable.GetMutableWorldTable();


	ImGui::SeparatorText("Realtime change");

	if(ImGui::CollapsingHeader("Colors"))
	{
		ImGui::SeparatorText("Ant colors");

		auto imguiColor = ColorConvert::RayColorToFloat4(antsValueTable.antDefaultColor);
		ImGui::ColorEdit4("Default", imguiColor.color);
		antsValueTable.antDefaultColor = ColorConvert::Float4ToRayColor(imguiColor.color);

		imguiColor = ColorConvert::RayColorToFloat4(antsValueTable.antWithFoodColor);
		ImGui::ColorEdit4("With food", imguiColor.color);
		antsValueTable.antWithFoodColor = ColorConvert::Float4ToRayColor(imguiColor.color);

		ImGui::SeparatorText("World colors");

		imguiColor = ColorConvert::RayColorToFloat4(worldValueTable.homePheromoneColor);
		ImGui::ColorEdit4("Home pheromone", imguiColor.color);
		worldValueTable.homePheromoneColor = ColorConvert::Float4ToRayColor(imguiColor.color);

		imguiColor = ColorConvert::RayColorToFloat4(worldValueTable.foodPheromoneColor);
		ImGui::ColorEdit4("Food pheromone", imguiColor.color);
		worldValueTable.foodPheromoneColor = ColorConvert::Float4ToRayColor(imguiColor.color);
	}

	ImGui::SeparatorText("Change only after reset");

	if(ImGui::CollapsingHeader("Ants behaviour"))
	{
		ImGui::SeparatorText("Movement");

		ImGui::InputFloat("Movement speed", &antsValueTable.antMovementSpeed);
		ImGui::InputFloat("Rotation speed", &antsValueTable.antRotationSpeed);
		ImGui::InputFloat("Random angle", &antsValueTable.antRandomAngle);

		ImGui::SeparatorText("Perception");

		ImGui::InputInt("FOV range", &antsValueTable.antFovRange);

		ImGui::SeparatorText("Other");

		ImGui::InputFloat("Home pheromone strength loss", &antsValueTable.antHomeStrengthLoss);
		ImGui::InputFloat("Food pheromone strength loss", &antsValueTable.antFoodStrengthLoss);
	}

	if(ImGui::CollapsingHeader("World variables"))
	{

	}

	ImGui::End();

	rlImGuiEnd();
}
