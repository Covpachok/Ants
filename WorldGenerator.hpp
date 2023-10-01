#ifndef ANTS_WORLDGENERATOR_HPP
#define ANTS_WORLDGENERATOR_HPP

#include <raylib.h>

#include "TileMap.hpp"

class WorldGenerationSettings;


struct Range
{
	float low  = 0;
	float high = 0;

	Range &operator=(const Range &) = default;

	inline bool IsInRange(float n) const { return n >= low && n <= high; }
};

namespace WorldGenerator
{
	void Generate(TileMap &tileMap, float size, float contrast,float blurIntensity, Range wall, Range food, Range clear, int octaves = 6);
	void Generate(ColorMap &colorMap, float size, float contrast, float blurIntensity, float Wlo, float Whi, float Flo, float Fhi,
	              int octaves = 6);


	void WorldGenTest();
}

#endif //ANTS_WORLDGENERATOR_HPP

