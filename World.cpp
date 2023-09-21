#include "World.hpp"

#include <algorithm>

#include "Random.hpp"
#include "Settings.hpp"

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
	auto &genSettings    = Settings::Instance().GetMapGenerationSettings();

	int                                  width  = static_cast<int>(globalSettings.mapWidth);
	int                                  height = static_cast<int>(globalSettings.mapHeight);

	std::function<void(int, int, Color)> genFunc;

	const auto foodOnly = [&](int x, int y, Color pixel) {
		if ( pixel.r >= genSettings.mapGenFoodLowThreshold &&
		     pixel.r <= genSettings.mapGenFoodHighThreshold )
		{
			m_tileMap->SetTile({x, y}, TileType::eFood);
		}
	};

	const auto wallsOnly = [&](int x, int y, Color pixel) {
		if ( pixel.r >= genSettings.mapGenWallLowThreshold &&
		     pixel.r <= genSettings.mapGenWallHighThreshold )
		{
			m_tileMap->SetTile({x, y}, TileType::eWall);
		}
	};

	const auto foodAndWalls = [&](int x, int y, Color pixel) {
		foodOnly(x, y, pixel);
		wallsOnly(x, y, pixel);
	};

	switch ( genSettings.mapGenSettings )
	{
		case MapGenSettings::eFoodOnly:
			genFunc = foodOnly;
			break;

		case MapGenSettings::eWallsOnly:
			genFunc = wallsOnly;
			break;

		case MapGenSettings::eFoodAndWalls:
			genFunc = foodAndWalls;
			break;

		default:
			return;

	}

	Image noiseImage = GenImagePerlinNoise(width, width, Random::Int(-10000, 10000),
	                                       Random::Int(-10000, 10000), genSettings.mapGenNoiseScale);
	ImageResizeCanvas(&noiseImage, width, height, 0, 0, BLACK);
	ImageBlurGaussian(&noiseImage, genSettings.mapGenNoiseBlur);
	ImageColorContrast(&noiseImage, genSettings.mapGenNoiseContrast);
	Color *noiseColors = LoadImageColors(noiseImage);

	UnloadImage(noiseImage);

	for ( int y = 0; y < height; ++y )
	{
		const int rowIndex = y * width;
		for ( int x        = 0; x < width; ++x )
		{
			const int   index      = rowIndex + x;
			const Color noisePixel = noiseColors[index];

			genFunc(x, y, noisePixel);
		}
	}

	UnloadImageColors(noiseColors);
}
