#ifndef ANTS_WORLD_HPP
#define ANTS_WORLD_HPP

#include <raylib.h>
#include <rlgl.h>

#include <memory>

#include "IntVec.hpp"
#include "BoundsChecker.hpp"

#include "TileMap.hpp"

class World
{
public:
	World();

	void Update();

	inline TileMap &GetTileMap() { return *m_tileMap; };
	inline const TileMap &GetTileMap() const { return *m_tileMap; };

	void ClearMap();

	void Draw() const;

	void Erase();

	const BoundsChecker2D &BoundsChecker() const { return *m_boundsChecker; }

private:
	void GenerateMap();

private:
	int m_screenWidth, m_screenHeight;

	std::unique_ptr<TileMap> m_tileMap;

	std::unique_ptr<BoundsChecker2D> m_boundsChecker;
};

#endif