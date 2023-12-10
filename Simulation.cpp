#include <string>

#include <raylib.h>
#include <raymath.h>
#include <imgui.h>
#include <rlImGui.h>

#include "Simulation.hpp"
#include "Gui.hpp"

const int k_screenWidth  = 1280;
const int k_screenHeight = 720;

const float k_adaptiveSpeedStep = 0.005f;

const int k_fpsLowThreshold  = 30;
const int k_fpsHighThreshold = 120;

constexpr float k_fixedTimestep = ( 1000.0 / 60.0 ) / 1000.0;


Simulation::Simulation() :
		m_settings(), m_camera()
{
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(k_screenWidth, k_screenHeight, "Ants");
	rlImGuiSetup(true);

	ResetCamera();

	m_world           = std::make_unique<World>();
	m_coloniesManager = std::make_unique<ColoniesManager>(m_world->GetTileMap());
}

Simulation::~Simulation()
{
	CloseWindow();
	rlImGuiShutdown();
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
#if 0
	if ( m_choosingHomePos )
	{
		m_paintingEnabled = false;
		if ( IsMouseButtonDown(MOUSE_BUTTON_LEFT))
		{
			auto worldPos = m_world->ScreenToWorld(mouseWorldPos);
			if ( m_world->BoundsChecker().IsInBounds(worldPos))
			{
				auto &homePos = m_settings.GetMutableWorldSettings().homePos;
				homePos       = worldPos;

				if(m_spawnNewNest)
				{
					m_world->AddNest(homePos);
				}

				m_choosingHomePos = false;
			}
			return;
		}
	}
#endif

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

	if ( IsMouseButtonDown(MOUSE_BUTTON_LEFT))
	{
		IntVec2 pos = {mouseWorldPos.x, mouseWorldPos.y};//m_settings.GetGlobalSettings().ScreenToWorld(mouseWorldPos);

		m_brush.Paint(m_world->GetTileMap(), pos.x, pos.y);
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
		m_drawPheromones = !m_drawPheromones;
	}

	if ( IsKeyPressed(KEY_TWO))
	{
		m_drawAnts = !m_drawAnts;
	}

	if ( IsKeyPressed(KEY_EQUAL))
	{
		m_gameSpeed += 1;
		if ( m_gameSpeed > k_maxGameSpeed )
		{
			m_gameSpeed = k_maxGameSpeed;
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
		if ( fps < k_fpsLowThreshold )
		{
			m_gameSpeed = std::max(m_gameSpeed - k_adaptiveSpeedStep, k_minGameSpeed);
		}

		if ( fps > k_fpsHighThreshold )
		{
			m_gameSpeed = std::min(m_gameSpeed + k_adaptiveSpeedStep, k_maxGameSpeed);
		}
	}

	for ( auto &colony: m_coloniesManager->GetColonies())
	{
		colony->Update(m_world->GetTileMap());
	}
}

void Simulation::Draw()
{
	BeginDrawing();

	ClearBackground({64, 64, 64, 255});

	BeginMode2D(m_camera);
	{
		m_world->Draw();

		for ( auto &colony: m_coloniesManager->GetColonies())
		{
			if ( m_drawPheromones )
			{
				colony->DrawPheromones();
			}
			if ( m_drawAnts )
			{
				colony->DrawAnts();
			}
		}
	}
	EndMode2D();

	ShowGui();

	EndDrawing();
}

void Simulation::ResetCamera()
{
	float width  = static_cast<float>(m_settings.GetGlobalSettings().mapWidth);
	float height = static_cast<float>(m_settings.GetGlobalSettings().mapHeight);

	m_camera.rotation = 0;
	m_camera.zoom     = 1;
	m_camera.offset   = {GetScreenWidth() / 2.f, GetScreenHeight() / 2.f};
	m_camera.target   = {width / 2, height / 2};
}

void Simulation::ShowGui()
{
	if ( !m_showGui )
	{
		return;
	}

	m_shouldHandleInput = m_gui.ShouldHandleInput();

	rlImGuiBegin();

	m_gui.ShowMainSettings(*this);

	if ( m_showAdvancedSettings )
	{
		m_gui.ShowAdvancedSettings(m_settings);
	}

	rlImGuiEnd();
}

void Simulation::Reset()
{
	m_world           = std::make_unique<World>();
	m_coloniesManager = std::make_unique<ColoniesManager>(m_world->GetTileMap());
	ResetCamera();
}
