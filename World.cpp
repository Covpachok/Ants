#include "World.hpp"

#include "Settings.hpp"

#include "WorldGenerator.hpp"

#include "omp.h"
#include <iostream>

World::World()
{
	auto &globalSettings = Settings::Instance().GetGlobalSettings();

	m_boundsChecker = std::make_unique<BoundsChecker2D>(0, globalSettings.mapWidth, 0, globalSettings.mapHeight);

	m_worldWidth  = static_cast<int>(globalSettings.mapWidth);
	m_worldHeight = static_cast<int>(globalSettings.mapHeight);
//	m_screenToWorld = globalSettings.screenToMapRatio;

	std::cout << "World size: " << m_worldWidth << "x" << m_worldHeight << std::endl;

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
//	DrawRectangle(-5 * m_screenToWorld, -5 * m_screenToWorld, (m_worldWidth + 10) * m_screenToWorld, (m_worldHeight + 10) * m_screenToWorld, RED);
//	DrawRectangle(0, 0, m_worldWidth * m_screenToWorld, m_worldHeight * m_screenToWorld, BLACK);
	DrawRectangle(-5, -5, ( m_worldWidth + 10 ), ( m_worldHeight + 10 ), RED);
	DrawRectangle(0, 0, m_worldWidth, m_worldHeight, BLACK);

	m_tileMap->Draw();
}

void World::Erase()
{
	m_tileMap->Clear();
}

void World::GenerateMap()
{
	auto &genSettings = Settings::Instance().GetWorldGenerationSettings();

	WorldGenerator::Generate(*m_tileMap, genSettings.noiseScale / 2.f, genSettings.noiseContrast, genSettings.noiseBlur,
	                         genSettings.wallRange, genSettings.foodRange, genSettings.emptyRange,
	                         genSettings.noiseOctaves, genSettings.ridgesIntensity);
}

bool CheckColor(Color checked, Color sample)
{
	return checked.r == sample.r && checked.g == sample.g && checked.b == sample.b;
}

bool World::LoadWorldFromImage(Settings &settings, const std::string &imageName)
{
	Image image = LoadImage(imageName.c_str());
	if ( image.data == nullptr )
	{
		std::cout << "Image is null" << std::endl;
		return false;
	}
	Color *colors = LoadImageColors(image);
	if ( colors == nullptr )
	{
		std::cout << "Colors is null" << std::endl;
		return false;
	}

	settings.GetGlobalSettings().mapWidth  = image.width;
	settings.GetGlobalSettings().mapHeight = image.height;
	m_worldWidth  = image.width;
	m_worldHeight = image.height;

	m_tileMap       = std::make_unique<TileMap>(image.width, image.height);
	m_boundsChecker = std::make_unique<BoundsChecker2D>(0, image.width, 0, image.height);

	std::cout << "Updating tileMap";
	for ( int y = 0; y < image.height; ++y )
	{
		for ( int x = 0; x < image.width; ++x )
		{
			if ( CheckColor(colors[y * image.width + x], {0, 255, 0, 255}))
			{
				m_tileMap->UnsafeSetTile(x, y, TileType::eFood);
			}
			else if ( CheckColor(colors[y * image.width + x], {255, 255, 255, 255}))
			{
				m_tileMap->UnsafeSetTile(x, y, TileType::eWall);
			}
			else
			{
				m_tileMap->UnsafeSetTile(x, y, TileType::eEmpty);
			}
		}
	}
	m_tileMap->Update();
	std::cout << std::endl;
	std::cout << "Success" << std::endl;

	UnloadImage(image);
	UnloadImageColors(colors);
	return true;
}