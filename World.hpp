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

	void Draw() const;

	void ClearMap();
	void Erase();

	void GenerateMap();
	bool LoadWorldFromImage(class Settings &settings, const std::string &imageName);

	inline const BoundsChecker2D &BoundsChecker() const { return *m_boundsChecker; }
	inline TileMap &GetTileMap() { return *m_tileMap; };
	inline const TileMap &GetTileMap() const { return *m_tileMap; };

private:
//	float m_screenToWorld;
//	int m_screenWidth, m_screenHeight;
	int m_worldWidth, m_worldHeight;

	std::unique_ptr<TileMap> m_tileMap;

	std::unique_ptr<BoundsChecker2D> m_boundsChecker;
};

#endif