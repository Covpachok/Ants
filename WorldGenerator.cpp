#include "WorldGenerator.hpp"
#include <cmath>
#include <iostream>
#include <FastNoiseLite.h>

#include <rlImGui.h>
#include <raylib.h>
#include <raymath.h>

#include "Settings.hpp"
#include "Random.hpp"

#include "Timer.hpp"

#include "omp.h"

std::vector<std::vector<float>> GenerateGaussianKernel(int scale, float sigma)
{
	std::vector<std::vector<float>> kernel(scale, std::vector<float>(scale));

	float sum      = 0.f;
	int   halfSize = scale / 2;

	for ( int i = -halfSize; i <= halfSize; ++i )
	{
		for ( int j = -halfSize; j <= halfSize; ++j )
		{
			kernel[i + halfSize][j + halfSize] =
					std::exp(-( i * i + j * j ) / ( 2 * sigma * sigma )) / ( 2 * M_PI * sigma * sigma );
			sum += kernel[i + halfSize][j + halfSize];
		}
	}

#pragma omp parallel for collapse(2) default(none) shared(scale, kernel, sum)
	for ( int i = 0; i < scale; ++i )
	{
		for ( int j = 0; j < scale; ++j )
		{
			kernel[i][j] /= sum;
		}
	}

	return kernel;
}

std::vector<std::vector<float>> BlurNoise(const std::vector<std::vector<float>> &noise, float blurIntensity)
{
	int noiseWidth  = static_cast<int>(noise[0].size());
	int noiseHeight = static_cast<int>( noise.size());

	std::vector<std::vector<float>> blurred(noiseHeight, std::vector<float>(noiseWidth));
	std::vector<std::vector<float>> kernel = GenerateGaussianKernel(5, blurIntensity);

	int kernelWidth  = static_cast<int>(kernel[0].size());
	int kernelHeight = static_cast<int>(kernel.size());

#pragma omp parallel for collapse(2) default(none) shared(noiseWidth, noiseHeight, kernelWidth, kernelHeight, blurred, kernel, noise)
	for ( int y = 0; y < noiseHeight; ++y )
	{
		for ( int x = 0; x < noiseWidth; ++x )
		{

			for ( int ny = -kernelHeight / 2; ny <= kernelHeight / 2; ++ny )
			{
				for ( int nx = -kernelWidth / 2; nx <= kernelWidth / 2; ++nx )
				{
					if ( y + ny < 0 || y + ny >= noiseHeight || x + nx < 0 || x + nx >= noiseWidth )
					{
						continue;
					}

					blurred[y][x] += kernel[ny + kernelHeight / 2][nx + kernelWidth / 2] * noise[y + ny][x + nx];
				}
			}

		}
	}

	return blurred;
}

std::vector<std::vector<float>>
GenerateNoiseArray(int width, int height, float scale, float contrast, float threshold, int octaves = 6)
{
	FastNoiseLite perlin(Random::Int(0, INT32_MAX));
	perlin.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
	perlin.SetFractalType(FastNoiseLite::FractalType_FBm);
	perlin.SetFractalOctaves(octaves);
	perlin.SetFractalGain(0.5f);
	perlin.SetFractalLacunarity(2);
	perlin.SetFrequency(0.01);

	FastNoiseLite ridges(Random::Int(0, INT32_MAX));
	ridges.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
	ridges.SetFractalType(FastNoiseLite::FractalType_Ridged);
	ridges.SetFractalOctaves(octaves);
	ridges.SetFractalGain(0.5f);
	ridges.SetFractalLacunarity(0);
	ridges.SetFrequency(0.004);
	ridges.SetCellularDistanceFunction(FastNoiseLite::CellularDistanceFunction_EuclideanSq);

	std::vector<std::vector<float>> noiseArray(height, std::vector<float>(width));

#pragma omp parallel for collapse(2) default(none) shared(width, height, noiseArray, scale, contrast, octaves, perlin, ridges, threshold)
	for ( int y = 0; y < height; ++y )
	{
		for ( int x = 0; x < width; ++x )
		{
			float nx = x * scale;
			float ny = y * scale;

			float per = ( perlin.GetNoise(nx, ny) + 1.f ) / 2.f;
			float cel = ( ridges.GetNoise(nx, ny) + 1.f ) / 2.f;
			if ( cel >= 0.75f )
			{
				cel = std::min(cel * 2.f, 1.f);
			}
			else
			{
				cel = 0.f;
			}

			float noise = cel * threshold + per * ( 1 - threshold );
			noise = pow(noise, contrast);

			noiseArray[y][x] = noise;
		}
	}

	return noiseArray;
}

void
WorldGenerator::Generate(TileMap &tileMap, float scale, float contrast, float blurIntensity, Range wall, Range food,
                         Range clear, int octaves)
{
	int width  = tileMap.GetWidth();
	int height = tileMap.GetHeight();

	auto noiseArray = GenerateNoiseArray(width, height, scale, contrast, clear.low, octaves);

	if ( blurIntensity > 0.f )
	{
		for ( int i = 0; i < 4; ++i )
		{
			noiseArray = BlurNoise(noiseArray, blurIntensity);
		}
	}

#pragma omp parallel for collapse(2) default(none) shared(width, height, noiseArray, tileMap, wall, food)
	for ( int y = 0; y < height; ++y )
	{
		for ( int x = 0; x < width; ++x )
		{
			const auto &noise = noiseArray[y][x];

			if ( wall.IsInRange(noise))
			{
				tileMap.UnsafeSetTile(x, y, TileType::eWall);
			}
			else if ( food.IsInRange(noise))
			{
				tileMap.UnsafeSetTile(x, y, TileType::eFood);
			}
			else
			{
				tileMap.UnsafeSetTile(x, y, TileType::eEmpty);
			}
		}
	}
	tileMap.Update();
}

void
WorldGenerator::Generate(ColorMap &colorMap, float scale, float contrast, float blurIntensity, float Wlo, float Whi,
                         float Flo, float Fhi,
                         int octaves)
{
	int width  = colorMap.GetWidth();
	int height = colorMap.GetHeight();

	auto noiseArray = GenerateNoiseArray(width, height, scale, contrast, Wlo, octaves);

	if ( blurIntensity > 0.f )
	{
		noiseArray = BlurNoise(noiseArray, blurIntensity);
	}

#pragma omp parallel for collapse(2) default(none) shared(width, height, noiseArray, colorMap)
	for ( int y = 0; y < height; ++y )
	{
		for ( int x = 0; x < width; ++x )
		{
			auto  n     = static_cast<unsigned char>(noiseArray[y][x] * 255.f);
			Color color = {n, n, n, 255};
			colorMap.UnsafeSet(x, y, color);
		}
	}

	colorMap.Update();
}


void WorldGenerator::WorldGenTest()
{
	InitWindow(1280, 720, "Ants");
	rlImGuiSetup(true);

	Settings settings;

	Camera2D camera;
	camera.rotation = 0;
	camera.zoom     = 1;
	camera.offset   = {0, 0};
	camera.target   = {0, 0};

	TileMap  tileMap(settings.GetGlobalSettings().mapWidth, settings.GetGlobalSettings().mapHeight);
	ColorMap colorMap(settings.GetGlobalSettings().mapWidth, settings.GetGlobalSettings().mapHeight, BLACK);

	float Flo      = 0.75f, Fhi = 1.f;
	float Wlo      = 0.0f, Whi = 0.15f;
	float Clo      = 0.6f, Chi = 0.65f;
	float scale    = 7.f;
	bool  changed  = false;
	int   octaves  = 8.f;
	float contrast = 2.25f;
	float blur     = 2.f;

	Timer timer{0.1f};

	while ( !WindowShouldClose())
	{
		timer.Update(GetFrameTime());
		if ( IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
		{
			Vector2 delta = Vector2Scale(GetMouseDelta(), -1.0f / camera.zoom);
			camera.target = Vector2Add(camera.target, delta);
		}

		BeginDrawing();
		ClearBackground(BLACK);

		BeginMode2D(camera);
		tileMap.Draw();
//		colorMap.Draw();
		EndMode2D();

		rlImGuiBegin();
		ImGui::Begin("A");
		{
			changed |= ImGui::SliderFloat("Size", &scale, 1.f, 16.f);
			changed |= ImGui::SliderFloat("Flo", &Flo, 0.f, 1.f);
			changed |= ImGui::SliderFloat("Fhi", &Fhi, 0.f, 1.f);
			changed |= ImGui::SliderFloat("Wlo", &Wlo, 0.f, 1.f);
			changed |= ImGui::SliderFloat("Whi", &Whi, 0.f, 1.f);
			changed |= ImGui::SliderFloat("Clo", &Clo, 0.f, 1.f);
			changed |= ImGui::SliderFloat("Chi", &Chi, 0.f, 1.f);
			changed |= ImGui::SliderFloat("Contrast", &contrast, 0.1f, 4.f);
			changed |= ImGui::SliderFloat("Blur intensity", &blur, 0.f, 2.f);
			changed |= ImGui::SliderInt("Octaves", &octaves, 1, 6);
			if ( changed && timer.IsElapsed() || ImGui::Button("Gen"))
			{
				changed = false;
				timer.Reset();
				WorldGenerator::Generate(tileMap, scale / 2.f, contrast, blur, {Wlo, Whi}, {Flo, Fhi}, {Clo, Chi},
				                         octaves);
//				WorldGenerator::Generate(colorMap, scale, contrast, blur, Wlo, Whi, Flo, Fhi, octaves);
			}
		}
		ImGui::End();
		rlImGuiEnd();

		EndDrawing();
	}
}
