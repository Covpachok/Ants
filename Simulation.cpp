#include <iostream>
#include <string>

#include <raylib.h>
#include <raymath.h>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <rlImGui.h>
#include <filesystem>

#include "Simulation.hpp"
#include "ColorConvert.hpp"

#include "omp.h"

const int k_screenWidth  = 1280;
const int k_screenHeight = 720;

constexpr float k_fixedTimestep = ( 1000.0 / 60.0 ) / 1000.0;


Simulation::Simulation() :
		m_settings(), m_ants()
{
	InitWindow(k_screenWidth, k_screenHeight, "Ants");

	m_world = std::make_unique<World>(m_settings);
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

void Simulation::Init()
{
	m_world->Init();
	InitAnts();
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
		m_shouldHandleInput = true;
		return;
	}

	Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), m_camera);
	if ( m_choosingHomePos )
	{
		m_paintingEnabled = false;
		if ( IsMouseButtonDown(MOUSE_BUTTON_LEFT))
		{
			auto worldPos = m_world->ScreenToWorld(mouseWorldPos);
			if ( m_world->IsInBounds(worldPos))
			{
				auto homePosRef = m_settings.GetMutableWorldSettings().homePos;
				homePosRef[0] = worldPos.x;
				homePosRef[1] = worldPos.y;

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

	if ( IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
	{
		Vector2 delta = Vector2Scale(GetMouseDelta(), -1.0f / m_camera.zoom);
		m_camera.target = Vector2Add(m_camera.target, delta);
	}

	if ( IsMouseButtonDown(MOUSE_BUTTON_LEFT) && m_paintingEnabled )
	{
		auto pos = m_world->ScreenToWorld(mouseWorldPos.x, mouseWorldPos.y);

		m_brush.Paint(*m_world, pos.x, pos.y);
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

	if ( IsKeyPressed(KEY_F11))
	{
		m_showGui = !m_showGui;
	}

	if ( IsKeyPressed(KEY_R))
	{
		Reset();
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
			m_gameSpeed = std::min(m_gameSpeed + 0.05f * k_fixedTimestep, 20.f);
		}
	}

	m_world->Update(k_fixedTimestep);


#pragma omp parallel for default(none) shared(k_fixedTimestep)
	for ( auto &ant: m_ants )
	{
		ant.Update(k_fixedTimestep, *m_world);
	}

	// Thing that can't be parallelized are in here
	for ( auto &ant: m_ants )
	{
		ant.PostUpdate(k_fixedTimestep, *m_world);
	}
}

void Simulation::Draw()
{
	BeginDrawing();

	ClearBackground({64, 64, 64, 255});

	BeginMode2D(m_camera);
	m_world->Draw(m_drawHomePheromones, m_drawFoodPheromones);

	if ( m_drawAnts )
	{
		for ( auto &ant: m_ants )
		{
			ant.Draw();
		}
	}
	EndMode2D();

	if ( m_showGui )
	{
		ShowGui();
	}

	EndDrawing();
}


void Simulation::Reset()
{
	m_world->Erase();
	ResetAnts();
	Init();
}

void Simulation::InitAnts()
{
	m_ants.resize(m_settings.GetWorldSettings().antsAmount);

	auto       homePos = m_world->GetScreenHomePos();
	for ( auto &ant: m_ants )
	{
		ant.Init(homePos.x, homePos.y, m_settings.GetAntsSettings());
	}
}

void Simulation::ResetAnts()
{
	m_ants.clear();
	InitAnts();
}

void Simulation::ResetCamera()
{
	m_camera.rotation = 0;
	m_camera.zoom     = 1;
	m_camera.offset   = {0, 0};
	m_camera.target   = {0, 0};
}

void HelpTooltip(const std::string &text)
{
	ImGui::SameLine();
	ImGui::TextDisabled("(?)");
	if ( ImGui::IsItemHovered())
	{
		ImGui::SetTooltip(text.c_str());
	}
}

void Simulation::ShowGui()
{
	ImGuiIO &io = ImGui::GetIO();
	m_shouldHandleInput = !( io.WantCaptureMouse || io.WantCaptureKeyboard );

	rlImGuiBegin();

	StatisticsGui();
	SettingsGui();

	if ( m_showAdvancedSettings )
	{
		AdvancedSettingsGui();
	}

	rlImGuiEnd();
}

void Simulation::StatisticsGui()
{
	ImGui::Begin("Statistics");
	{
		ImGui::Value("Ants amount", static_cast<int>(m_ants.size()));

		ImGui::Separator();

		ImGui::Value("Collected food ", m_world->GetCollectedFoodAmount());
		ImGui::Value("Delivered food ", m_world->GetDeliveredFoodAmount());
		ImGui::Value("Not delivered  ", m_world->GetCollectedFoodAmount() - m_world->GetDeliveredFoodAmount());

		ImGui::Separator();

		ImGui::Value("Remaining food ", m_world->GetRemainingFoodAmount());
		ImGui::Value("Total food     ", m_world->GetTotalFoodAmount());

		ImGui::Separator();

		ImGui::Text("Delivered/Total : %d/%d", m_world->GetDeliveredFoodAmount(), m_world->GetTotalFoodAmount());
	}
	ImGui::End();
}

void Simulation::SettingsGui()
{
	ImGui::Begin("Main settings");
	{
		ImGui::Checkbox("Painting enabled", &m_paintingEnabled);
		if ( m_paintingEnabled )
		{
			ImGui::SeparatorText("Brush settings");
			{
				const char *brushTitles[Brush::BrushType::Amount] = {"Point", "Square", "Round"};
				const char *paintTitles[Brush::BrushType::Amount] = {"Erase", "Food", "Wall"};

				int              size      = m_brush.GetBrushSize();
				Brush::BrushType brushType = m_brush.GetBrushType();
				World::TileType  paintType = m_brush.GetPaintType();

				ImGui::Text("Press Left Mouse Button to paint");

				ImGui::SliderInt("Brush size", &size, 1, 100);

				ImGui::Combo("Paint type", reinterpret_cast<int *>(&paintType),
				             paintTitles, static_cast<int>(World::TileType::Amount));
				ImGui::Combo("Brush type", reinterpret_cast<int *>(&brushType),
				             brushTitles, static_cast<int>(Brush::BrushType::Amount));

				m_brush.SetBrushSize(size);
				m_brush.SetBrushType(brushType);
				m_brush.SetPaintType(paintType);
			}
		}

		ImGui::SeparatorText("Draw settings");
		{
			ImGui::Checkbox("[1] Home", &m_drawHomePheromones);
			ImGui::SameLine();
			ImGui::Checkbox("[2] Food", &m_drawFoodPheromones);
			ImGui::SameLine();
			ImGui::Checkbox("[3] Ants", &m_drawAnts);
			ImGui::SameLine();
			ImGui::Checkbox("[F11] Show gui", &m_showGui);
		}

		ImGui::SeparatorText("Speed settings");
		{
			ImGui::Checkbox("[SPACE] Pause", &m_pause);
			ImGui::SameLine();
			ImGui::Checkbox("[A] Adaptive speed", &m_adaptiveSpeed);

			ImGui::SliderFloat("[+][-] Simulation speed", &m_gameSpeed, 0.1f, 20.f);
		}

		ImGui::SeparatorText("Other");

		ImGui::Separator();

		if ( ImGui::Button("[R] Restart simulation"))
		{
			Reset();
		}

		if ( ImGui::Button("Reset ants"))
		{
			ResetAnts();
		}

		ImGui::SameLine();

		if ( ImGui::Button("Clear pheromones"))
		{
			m_world->ClearPheromones();
		}

		ImGui::SameLine();

		if ( ImGui::Button("Clear map"))
		{
			m_world->ClearMap();
		}

		if ( ImGui::Button("Reset camera"))
		{
			ResetCamera();
		}

		ImGui::Separator();

		ImGui::Checkbox("Show advanced settings", &m_showAdvancedSettings);

		ImGui::Text("(Some values will change only after simulation restart)");

	}
	ImGui::End();
}

void Simulation::AdvancedSettingsGui()
{
	ImGui::Begin("Advanced settings");
	{
		auto &antsSettings  = m_settings.GetMutableAntsSettings();
		auto &worldSettings = m_settings.GetMutableWorldSettings();

		ImGui::SeparatorText("REALTIME CHANGE");

		if ( ImGui::TreeNode("Ants behaviour settings"))
		{
			ImGui::SeparatorText("Colors");

			auto imguiColor = ColorConvert::RayColorToFloat4(antsSettings.antDefaultColor);
			ImGui::ColorEdit4("Default", imguiColor.color);
			antsSettings.antDefaultColor = ColorConvert::Float4ToRayColor(imguiColor.color);

			imguiColor = ColorConvert::RayColorToFloat4(antsSettings.antWithFoodColor);
			ImGui::ColorEdit4("With food", imguiColor.color);
			antsSettings.antWithFoodColor = ColorConvert::Float4ToRayColor(imguiColor.color);

			ImGui::PushItemWidth(200);

			ImGui::SeparatorText("Movement");

			ImGui::InputFloat("Movement speed", &antsSettings.antMovementSpeed);
			ImGui::InputFloat("Rotation speed", &antsSettings.antRotationSpeed);

			ImGui::SliderFloat("Random angle", &antsSettings.antRandomRotation, 0.f, 1.f);
			HelpTooltip("When ants wander, they choose a random angle\n"
			            "in which to look and move in that direction");

			ImGui::SeparatorText("Perception");

			ImGui::SliderInt("FOV range", &antsSettings.antFovRange, 2, 16);
			HelpTooltip("Ants can perceive objects in a cone in front of them,\n"
			            "this variable affects size of this cone.\n"
			            "Heavily affects performance");


			ImGui::SeparatorText("Pheromones");

			ImGui::InputFloat("Home strength loss", &antsSettings.homePheromoneStrengthLoss);
			ImGui::InputFloat("Food strength loss", &antsSettings.foodPheromoneStrengthLoss);
			HelpTooltip("Over time, ants lose the strength of their pheromones.\n"
			            "This variable prevents the ants from going in circles forever\n"
			            "and also allows them to take shorter paths.");

			ImGui::InputFloat("Home spawn intensity", &antsSettings.foodPheromoneIntensity);
			ImGui::InputFloat("Food spawn intensity", &antsSettings.homePheromoneIntensity);
			HelpTooltip("Amount of pheromones spawned each time");

			ImGui::SeparatorText("Other");

			ImGui::InputFloat("Food spawn delay", &antsSettings.pheromoneSpawnDelay);
			ImGui::InputFloat("Deviation chance", &antsSettings.deviationChance);

			ImGui::PopItemWidth();
			ImGui::TreePop();
		}

		ImGui::Text(" ");

		ImGui::SeparatorText("WILL CHANGE ONLY AFTER A RESET");
		{
			ImGui::InputInt("Ants amount", &worldSettings.antsAmount);

			if ( ImGui::TreeNode("World settings"))
			{
				ImGui::SeparatorText("World colors");
				{
					using namespace ColorConvert;

					ImGuiRlColorEdit4("Food", worldSettings.tileColors[World::Food]);
					ImGuiRlColorEdit4("Wall", worldSettings.tileColors[World::Wall]);

					ImGuiRlColorEdit4("Home pheromone", worldSettings.homePheromoneColor);
					ImGuiRlColorEdit4("Food pheromone", worldSettings.foodPheromoneColor);
				}

				ImGui::PushItemWidth(200);

				ImGui::SeparatorText("Tiles");

				ImGui::SliderInt("Food amount per tile", &worldSettings.tileDefaultAmount[1], 1, 512);

				ImGui::SeparatorText("Pheromones");

				ImGui::InputFloat("Home evaporation rate", &worldSettings.homePheromoneEvaporationRate, 0.f, 0.f,
				                  "%.3f");
				ImGui::InputFloat("Food evaporation rate", &worldSettings.foodPheromoneEvaporationRate, 0.f, 0.f,
				                  "%.3f");

				worldSettings.homePheromoneEvaporationRate = std::clamp(worldSettings.homePheromoneEvaporationRate,
				                                                        0.f, 255.f);
				worldSettings.foodPheromoneEvaporationRate = std::clamp(worldSettings.foodPheromoneEvaporationRate,
				                                                        0.f, 255.f);

				ImGui::SeparatorText("Home");

				ImGui::SliderInt("Radius", &worldSettings.homeRadius, 2, 10);
				ImGui::Checkbox("Centered position", &worldSettings.centeredHomePos);
				if ( !worldSettings.centeredHomePos )
				{
					ImGui::Separator();
					ImGui::InputInt2("Pos", worldSettings.homePos);
					ImGui::Checkbox("Choose by mouse click", &m_choosingHomePos);
				}

				ImGui::PopItemWidth();

				ImGui::TreePop();
			}

			if ( ImGui::TreeNode("Map generation settings"))
			{
				ImGui::PushItemWidth(200);

				ImGui::SeparatorText("Generation");

				const char *titles[] = {"None", "Food only", "Walls only", "Food and Walls"};

				ImGui::Combo("Tiles to generate", reinterpret_cast<int *>(&worldSettings.mapGenSettings),
				             titles, static_cast<int>(WorldSettings::MapGenSettings::Amount));

				ImGui::SeparatorText("Noise gen");

				ImGui::SliderFloat("Scale", &worldSettings.mapGenNoiseScale, 1.f, 32.f);
				HelpTooltip("The larger the scale, the more detailed the noise will be.");
				ImGui::SliderInt("Blur", &worldSettings.mapGenNoiseBlur, 1, 8);
				HelpTooltip("Makes world smoother.");
				ImGui::SliderFloat("Contrast", &worldSettings.mapGenNoiseContrast, 1.f, 128.f);

				ImGui::SeparatorText("Tiles thresholds");

				ImGui::DragIntRange2("Food spawn range",
				                     &worldSettings.mapGenFoodLowThreshold,
				                     &worldSettings.mapGenFoodHighThreshold,
				                     1, 0, 255,
				                     "From: %d", "To: %d", ImGuiSliderFlags_AlwaysClamp);

				ImGui::DragIntRange2("Wall spawn range",
				                     &worldSettings.mapGenWallLowThreshold,
				                     &worldSettings.mapGenWallHighThreshold,
				                     1, 0, 255,
				                     "From: %d", "To: %d", ImGuiSliderFlags_AlwaysClamp);

				ImGui::PopItemWidth();

				ImGui::TreePop();
			}
		}

		ImGui::Text(" ");

		ImGui::SeparatorText("OTHER");

		if ( ImGui::TreeNode("Save/Load"))
		{
			static std::vector<std::string> savedSettingsFiles = Settings::FindSavedSettings();

			ImGui::PushItemWidth(200);

			ImGui::Text("Filename");
			ImGui::InputText("S", &m_saveFilename);

			ImGui::SameLine();

			if ( ImGui::Button("Save"))
			{
				m_settings.Save(m_saveFilename);
				savedSettingsFiles = Settings::FindSavedSettings();
			}

			static size_t selectedIndex = 0;
			if ( savedSettingsFiles.empty())
			{
				ImGui::TextDisabled("Not found any save files");
			}
			else
			{
				const char *previewValue = savedSettingsFiles[selectedIndex].c_str();
				if ( ImGui::BeginCombo("L", previewValue))
				{
					for ( size_t i = 0; i < savedSettingsFiles.size(); ++i )
					{
						const bool isSelected = ( selectedIndex == i );
						if ( ImGui::Selectable(savedSettingsFiles[i].c_str(), isSelected))
						{
							selectedIndex = i;
						}

						if ( isSelected )
						{
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
			}

			ImGui::SameLine();

			if ( ImGui::Button("Load"))
			{
				m_settings.Load(savedSettingsFiles[selectedIndex]);
			}

			ImGui::SameLine();

			if ( ImGui::Button("Update list"))
			{
				savedSettingsFiles = Settings::FindSavedSettings();
			}

			ImGui::PopItemWidth();
			ImGui::TreePop();
		}

		ImGui::Text(" ");
		ImGui::Separator();

		if ( ImGui::Button("Reset settings values"))
		{
			m_settings.Reset();
		}

	}
	ImGui::End();
}