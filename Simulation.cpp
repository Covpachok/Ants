#include <iostream>
#include "Simulation.hpp"
#include <raylib.h>
#include <imgui.h>
#include <rlImGui.h>

const int kScreenWidth  = 1280;
const int kScreenHeight = 720;

const float kFoodPheromoneIntensity = 30;
const float kHomePheromoneIntensity = 30;

const double kFixedTimestep = (1000.0 / 60.0) / 1000.0;

const int kAntsAmount = 2000;

Simulation::Simulation() :
		m_ants(2000)
{
	InitWindow(kScreenWidth, kScreenHeight, "Ants");

	m_world.Init(kScreenWidth / 4, kScreenHeight / 4, 5, 5);

	for ( auto &ant: m_ants )
	{
		ant.Init(kScreenWidth / 2.f, kScreenHeight / 2.f, 40);
	}

//	SetTargetFPS(60);

	std::cout << "FIXED TIMESTEP: " << kFixedTimestep << std::endl;

	rlImGuiSetup(true);
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

			while ( deltaTime >= kFixedTimestep )
			{
				deltaTime -= kFixedTimestep;
				Update(kFixedTimestep);
			}
		}
	}
}

void Simulation::HandleInput()
{
	if ( IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
	{
		auto      pos    = m_world.ScreenToMap(GetMousePosition().x, GetMousePosition().y);
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
//		m_world.SetCell(pos.first, pos.second, World::Food);
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
		if(m_gameSpeed > 5)
			m_gameSpeed = 5;
	}

	if ( IsKeyPressed(KEY_MINUS))
	{
		m_gameSpeed -= 1;
		if(m_gameSpeed < 1)
			m_gameSpeed = 1;
	}
}

void Simulation::Update(double delta)
{
	if(GetFPS() < 30)
	{
		m_gameSpeed = std::max(m_gameSpeed - 0.1 * delta, 1.0);
	}
	else if (GetFPS() > 60)
	{
		m_gameSpeed = std::min(m_gameSpeed + 0.1 * delta, 10.0);
	}

	m_world.Update(delta);

	for ( auto &ant: m_ants )
	{
		ant.CheckCollisions(m_world);

		ant.Update(delta);

		auto pos = ant.GetPos();
//		int  y   = floor(pos.y), x = floor(pos.x);
		auto p   = m_world.ScreenToMap(pos.x, pos.y);

		if ( ant.IsGotFood())
		{
			m_world.AddFoodPheromone(p.first, p.second, kFoodPheromoneIntensity * delta * ant.GetFoodStrength());
		}
		else
		{
			m_world.AddHomePheromone(p.first, p.second, kHomePheromoneIntensity * delta * ant.GetHomeStrength());
		}
	}
}

void Simulation::Draw()
{
	BeginDrawing();

	ClearBackground(BLACK);

	m_world.Draw(m_drawHomePheromones, m_drawFoodPheromones);

	if ( m_drawAnts )
	{
		for ( auto &ant: m_ants )
		{
			ant.Draw();
		}
	}

	DrawFPS(1, 1);
	DrawText(TextFormat("%.0f", m_gameSpeed), 1, 21, 20, BLUE);

	rlImGuiBegin();

	ImGui::Begin("Ants");
	bool pressed = ImGui::Button("Reset");
	ImGui::SliderFloat("Simulation speed", &m_gameSpeed, 0.1f, 8.f);
	ImGui::End();

	rlImGuiEnd();

	EndDrawing();

	if(pressed)
	{
		Reset();
	}
}

void Simulation::Reset()
{
	m_world.Reset(kScreenWidth / 4, kScreenHeight / 4, 5, 5);

	m_ants.clear();
	m_ants.resize(kAntsAmount);

	for ( auto &ant: m_ants )
	{
		ant.Init(kScreenWidth / 2.f, kScreenHeight / 2.f, 40);
	}
}
