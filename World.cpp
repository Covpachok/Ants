#include "World.hpp"

#include "Settings.hpp"

#include "WorldGenerator.hpp"

#include "omp.h"

World::World()
{
	auto &globalSettings = Settings::Instance().GetGlobalSettings();

	m_boundsChecker = std::make_unique<BoundsChecker2D>(0, globalSettings.mapWidth, 0, globalSettings.mapHeight);

	m_screenWidth  = static_cast<int>(globalSettings.windowWidth);
	m_screenHeight = static_cast<int>(globalSettings.windowHeight);

	m_tileMap = std::make_unique<TileMap>(globalSettings.mapWidth, globalSettings.mapHeight);

	GenerateMap();
}

void World::Update()
{

}

void World::ClearMap()
{
	m_tileMap->Clear();
}

void World::Draw() const
{
	DrawRectangle(-5, -5, m_screenWidth + 10, m_screenHeight + 10, RED);
	DrawRectangle(0, 0, m_screenWidth, m_screenHeight, BLACK);

	m_tileMap->Draw();
}

void World::Erase()
{
	m_tileMap->Clear();
}

void World::GenerateMap()
{
	auto &globalSettings = Settings::Instance().GetGlobalSettings();
	auto &genSettings    = Settings::Instance().GetWorldGenerationSettings();

	int                                  width  = static_cast<int>(globalSettings.mapWidth);
	int                                  height = static_cast<int>(globalSettings.mapHeight);

	float Flo      = 0.75f, Fhi = 1.f;
	float Wlo      = 0.0f, Whi = 0.15f;
	float Clo      = 0.6f, Chi = 0.65f;
	float size     = 7.f;
	bool  changed  = false;
	int   octaves  = 8.f;
	float contrast = 2.25f;
	float blur     = 2.f;

	WorldGenerator::Generate(*m_tileMap, size / 2.f, contrast, blur, {Wlo, Whi}, {Flo, Fhi}, {Clo, Chi}, octaves);
}
