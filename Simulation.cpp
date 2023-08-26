#include <iostream>
#include <string>

#include <raylib.h>
#include <raymath.h>
#include <imgui.h>
#include <rlImGui.h>

#include "Simulation.hpp"
#include "ColorConvert.hpp"

#include "omp.h"

const int k_screenWidth  = 1280;
const int k_screenHeight = 720;

constexpr float k_fixedTimestep = ( 1000.0 / 60.0 ) / 1000.0;


Simulation::Simulation() :
		m_ants()
{
	InitWindow(k_screenWidth, k_screenHeight, "Ants");

	Init();

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
	float delta = 0;
	while ( !WindowShouldClose())
	{
		Draw();
		HandleInput();
		if ( !m_pause )
		{
			delta += GetFrameTime() * m_gameSpeed;

			while ( delta >= k_fixedTimestep )
			{
				delta -= k_fixedTimestep;
				Update();
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
	if ( m_choosingHomePos )
	{
		if ( IsMouseButtonDown(MOUSE_BUTTON_LEFT))
		{
			auto worldPos = m_world.ScreenToWorld(mouseWorldPos);
			if ( m_world.IsInBounds(worldPos))
			{
				auto homePosRef = g_valueTable.GetMutableWorldTable().homePos;
				homePosRef[0] = worldPos.first;
				homePosRef[1] = worldPos.second;

				m_choosingHomePos = false;
			}

			return;
		}
	}

	float wheel = GetMouseWheelMove();
	if ( wheel != 0.f )
	{
		m_camera.offset = GetMousePosition();
		m_camera.target = mouseWorldPos;
		m_camera.zoom += wheel * 0.1f;
		m_camera.zoom   = std::fmax(m_camera.zoom, 0.1f);
	}

	if ( IsMouseButtonDown(MOUSE_BUTTON_LEFT))
	{
		Vector2 delta = Vector2Scale(GetMouseDelta(), -1.0f / m_camera.zoom);
		m_camera.target = Vector2Add(m_camera.target, delta);
	}

	if ( IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
	{
		auto pos = m_world.ScreenToWorld(mouseWorldPos.x, mouseWorldPos.y);

		int       radius        = 5;
		const int radiusSquared = radius * radius;

		for ( int y = -radius; y <= radius; ++y )
		{
			for ( int x = -radius; x <= radius; ++x )
			{
				if ( x * x + y * y <= radiusSquared )
				{
					m_world.SetCell(pos.first + x, pos.second + y, World::Food);
				}
			}
		}
	}

	if ( IsKeyDown(KEY_W))
	{
		auto      pos    = m_world.ScreenToWorld(mouseWorldPos.x, mouseWorldPos.y);
		int       radius = 5;
		for ( int y      = -radius; y <= radius; ++y )
		{
			for ( int x = -radius; x <= radius; ++x )
			{
				m_world.SetCell(pos.first + x, pos.second + y, World::Wall);
			}
		}
	}

	if ( IsKeyDown(KEY_E))
	{
		auto      pos    = m_world.ScreenToWorld(mouseWorldPos.x, mouseWorldPos.y);
		int       radius = 5;
		for ( int y      = -radius; y <= radius; ++y )
		{
			for ( int x = -radius; x <= radius; ++x )
			{
				m_world.SetCell(pos.first + x, pos.second + y, World::None);
			}
		}
	}

	if ( IsKeyPressed(KEY_SPACE))
	{
		m_pause = !m_pause;
	}

	if ( IsKeyPressed(KEY_A))
	{
		m_adaptiveSpeed = !m_adaptiveSpeed;
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

void Simulation::Update()
{
	SetWindowTitle(( "Ants FPS:" + std::to_string(GetFPS())).c_str());

	if ( m_adaptiveSpeed )
	{
		const int fps = GetFPS();
		if ( fps < 10 )
		{
			m_gameSpeed = std::max(m_gameSpeed - 0.5f * k_fixedTimestep, 1.f);
		}
		else if ( fps < 30 )
		{
			m_gameSpeed = std::max(m_gameSpeed - 0.1f * k_fixedTimestep, 1.f);
		}

		if ( fps > 144 )
		{
			m_gameSpeed = std::min(m_gameSpeed + 0.5f * k_fixedTimestep, 20.f);
		}
		else if ( fps > 60 )
		{
			m_gameSpeed = std::min(m_gameSpeed + 0.1f * k_fixedTimestep, 20.f);
		}
	}

	m_world.Update(k_fixedTimestep);


#pragma omp parallel for default(none) shared(k_fixedTimestep)
	for ( auto &ant: m_ants )
	{
		ant.Update(k_fixedTimestep, m_world);
	}

	// Thing that can't be parallelized are in here
	for ( auto &ant: m_ants )
	{
		ant.PostUpdate(k_fixedTimestep, m_world);
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

void Simulation::Init()
{
	m_ants.resize(g_valueTable.GetWorldTable().antsAmount);

	m_world.Init(k_screenWidth / 3, k_screenHeight / 3);
	auto       homePos = m_world.GetScreenHomePos();
	for ( auto &ant: m_ants )
	{
		ant.Init(homePos.x, homePos.y);
	}
}

void Simulation::Reset()
{
	m_world.Erase();
	m_ants.clear();

	Init();
}

void Simulation::DebugGui()
{
	ImGuiIO &io = ImGui::GetIO();
	m_shouldHandleInput = !( io.WantCaptureMouse || io.WantCaptureKeyboard );

	rlImGuiBegin();

	ImGui::Begin("Ants");
	{
		if ( ImGui::CollapsingHeader("Statistics"))
		{
			ImGui::Value("Collected food ", m_world.GetCollectedFoodAmount());
			ImGui::Value("Delivered food ", m_world.GetDeliveredFoodAmount());
			ImGui::Value("Not delivered  ", m_world.GetCollectedFoodAmount() - m_world.GetDeliveredFoodAmount());

			ImGui::Separator();

			ImGui::Value("Remaining food ", m_world.GetRemainingFoodAmount());
			ImGui::Value("Total food     ", m_world.GetTotalFoodAmount());

			ImGui::Separator();

			ImGui::Text("Delivered/Total : %d/%d", m_world.GetDeliveredFoodAmount(), m_world.GetTotalFoodAmount());
		}

		ImGui::Separator();


		ImGui::SeparatorText("Draw settings");

		ImGui::Checkbox("[1] Home", &m_drawHomePheromones);
		ImGui::SameLine();
		ImGui::Checkbox("[2] Food", &m_drawFoodPheromones);
		ImGui::SameLine();
		ImGui::Checkbox("[3] Ants", &m_drawAnts);

		ImGui::SeparatorText("Speed settings");

		ImGui::Checkbox("[SPACE] Pause", &m_pause);
		ImGui::SameLine();
		ImGui::Checkbox("[A] Adaptive speed", &m_adaptiveSpeed);

		ImGui::SliderFloat("Simulation speed", &m_gameSpeed, 0.1f, 20.f);

		ImGui::SeparatorText("Other");

		ImGui::Checkbox("Show advanced settings", &m_showSimulationVariables);

		ImGui::Separator();

		if ( ImGui::Button("Restart simulation"))
		{
			Reset();
		}

		if ( ImGui::Button("Reset settings values"))
		{
			g_valueTable = g_defaultValueTable;
		}
		ImGui::Text("(Some values will change only after simulation restart)");
	}
	ImGui::End();

	if ( m_showSimulationVariables )
	{
		DebugVarsGui();
	}

	rlImGuiEnd();
}

void Simulation::DebugVarsGui()
{
	ImGui::Begin("Simulation variables");
	{
		auto &antsValueTable  = g_valueTable.GetMutableAntsTable();
		auto &worldValueTable = g_valueTable.GetMutableWorldTable();

		ImGui::SeparatorText("REALTIME CHANGE");

		if ( ImGui::TreeNode("Ants behaviour settings"))
		{
			ImGui::SeparatorText("Colors");

			auto imguiColor = ColorConvert::RayColorToFloat4(antsValueTable.antDefaultColor);
			ImGui::ColorEdit4("Default", imguiColor.color);
			antsValueTable.antDefaultColor = ColorConvert::Float4ToRayColor(imguiColor.color);

			imguiColor = ColorConvert::RayColorToFloat4(antsValueTable.antWithFoodColor);
			ImGui::ColorEdit4("With food", imguiColor.color);
			antsValueTable.antWithFoodColor = ColorConvert::Float4ToRayColor(imguiColor.color);

			ImGui::SeparatorText("Movement");

			ImGui::InputFloat("Movement speed", &antsValueTable.antMovementSpeed);
			ImGui::InputFloat("Rotation speed", &antsValueTable.antRotationSpeed);

			ImGui::Separator();

			ImGui::InputFloat("Random angle", &antsValueTable.antRandomAngle);

			ImGui::SeparatorText("Perception");

			ImGui::SliderInt("FOV range", &antsValueTable.antFovRange, 2, 16);

			ImGui::SeparatorText("Pheromones");

			ImGui::InputFloat("Home strength loss", &antsValueTable.homePheromoneStrengthLoss);
			ImGui::InputFloat("Food strength loss", &antsValueTable.foodPheromoneStrengthLoss);

			ImGui::Separator();

			ImGui::InputFloat("Home intensity", &antsValueTable.foodPheromoneIntensity);
			ImGui::InputFloat("Food intensity", &antsValueTable.homePheromoneIntensity);

			ImGui::TreePop();
		}

		ImGui::Text(" ");

		ImGui::SeparatorText("WILL CHANGE ONLY AFTER A RESET");

		ImGui::InputInt("Ants amount", &worldValueTable.antsAmount);

		if ( ImGui::TreeNode("World settings"))
		{
			ImGui::SeparatorText("World colors");
			{
				using namespace ColorConvert;

				ImGuiRlColorEdit4("Food", worldValueTable.cellColors[World::Food]);
				ImGuiRlColorEdit4("Wall", worldValueTable.cellColors[World::Wall]);

				ImGui::Separator();

				ImGuiRlColorEdit4("Home pheromone", worldValueTable.homePheromoneColor);
				ImGuiRlColorEdit4("Food pheromone", worldValueTable.foodPheromoneColor);
			}

			ImGui::SeparatorText("Cells");

			ImGui::SliderInt("Food amount per cell", &worldValueTable.cellDefaultAmount[1], 1, 512);

			ImGui::SeparatorText("Pheromones");

			ImGui::InputFloat("Home evaporation rate", &worldValueTable.homePheromoneEvaporationRate);
			ImGui::InputFloat("Food evaporation rate", &worldValueTable.foodPheromoneEvaporationRate);

			ImGui::SeparatorText("Home");

			ImGui::SliderInt("Radius", &worldValueTable.homeRadius, 2, 10);
			ImGui::Checkbox("Centered position", &worldValueTable.centeredHomePos);
			if ( !worldValueTable.centeredHomePos )
			{
				ImGui::Separator();
				ImGui::InputInt2("Pos", worldValueTable.homePos);
				ImGui::Checkbox("Choose by mouse click", &m_choosingHomePos);
			}

			ImGui::TreePop();
		}

		ImGui::Text(" ");

		if ( ImGui::TreeNode("Map generation settings"))
		{
			ImGui::SeparatorText("Generation");

			const char *titles[] = {"None", "Food only", "Walls only", "Food and Walls"};

			ImGui::Combo("Cells to generate", reinterpret_cast<int *>(&worldValueTable.mapGenSettings),
			             titles, static_cast<int>(MapGenSettings::Amount));

			ImGui::SeparatorText("Noise gen");

			ImGui::SliderFloat("Scale", &worldValueTable.mapGenNoiseScale, 1.f, 128.f);
			ImGui::SliderInt("Blur", &worldValueTable.mapGenNoiseBlur, 0, 32);

			ImGui::SeparatorText("Cells thresholds");

			ImGui::SliderInt("Food low", &worldValueTable.mapGenFoodLowThreshold, 0, 255);
			ImGui::SliderInt("Food high", &worldValueTable.mapGenFoodHighThreshold, 0, 255);

			ImGui::Separator();

			ImGui::SliderInt("Wall low", &worldValueTable.mapGenWallLowThreshold, 0, 255);
			ImGui::SliderInt("Wall high", &worldValueTable.mapGenWallHighThreshold, 0, 255);

			ImGui::TreePop();
		}
	}
	ImGui::End();
}