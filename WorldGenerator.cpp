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
GenerateNoiseMatrix(int width, int height, float scale, float contrast, float threshold, int octaves = 6)
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

	std::vector<std::vector<float>> noiseMatrix(height, std::vector<float>(width));

#pragma omp parallel for collapse(2) default(none) shared(width, height, noiseMatrix, scale, contrast, octaves, perlin, ridges, threshold)
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

			noiseMatrix[y][x] = noise;
		}
	}

	return noiseMatrix;
}

void
WorldGenerator::Generate(TileMap &tileMap, float scale, float contrast, float blurIntensity, Range wall, Range food,
                         Range clear, int octaves, float threshold)
{
	int width  = tileMap.GetWidth();
	int height = tileMap.GetHeight();

	auto noiseMatrix = GenerateNoiseMatrix(width, height, scale, contrast, threshold, octaves);

	if ( blurIntensity > 0.f )
	{
		for ( int i = 0; i < 4; ++i )
		{
			noiseMatrix = BlurNoise(noiseMatrix, blurIntensity);
		}
	}

#pragma omp parallel for collapse(2) default(none) shared(width, height, noiseMatrix, tileMap, wall, food)
	for ( int y = 0; y < height; ++y )
	{
		for ( int x = 0; x < width; ++x )
		{
			const auto &noise = noiseMatrix[y][x];

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