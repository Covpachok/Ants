#include "Ant.hpp"

#include "World.hpp"
#include "Random.hpp"

#include "omp.h"

constexpr float k_antFovCheckDelay = 0.04f;

bool CheckPointCircleCollision(Vector2 center1, Vector2 center2, float radius2)
{
	const float dx = center2.x - center1.x;
	const float dy = center2.y - center1.y;

	const float distanceSquared = dx * dx + dy * dy;

	return distanceSquared <= radius2 * radius2;
}

void Ant::Init(float startX, float startY, const AntsSettings &valueTable)
{
	m_antsSettings = &valueTable;

	m_pos             = {startX, startY};
	m_prevPos         = m_pos;
	m_rotation        = Random::Float(-M_PI, M_PI);
//	m_rotation        = static_cast<float>(GetRandomValue(M_PI * -100, M_PI * 100)) / 100.f;
	m_desiredRotation = m_rotation;

	m_colorsPtr[0] = &m_antsSettings->antDefaultColor;
	m_colorsPtr[1] = &m_antsSettings->antWithFoodColor;

	m_pheromoneSpawnTimer.SetDelay(m_antsSettings->pheromoneSpawnDelay);
	m_fovCheckTimer.SetDelay(k_antFovCheckDelay);
	m_deviationTimer.SetDelay(1.f);

	m_state = SearchForFood;
}

void Ant::Update(const float delta, const World &world)
{
	m_pheromoneSpawnTimer.Update(delta);
	m_fovCheckTimer.Update(delta);
	m_deviationTimer.Update(delta);

	if ( Random::Bool(m_antsSettings->deviationChance))
	{
		m_ignorePheromones = true;
		m_deviationTimer.Reset();
	}

	if ( m_deviationTimer.IsElapsed())
	{
		m_ignorePheromones = false;
	}

	CheckCollisions(world);
	if ( m_fovCheckTimer.IsElapsed())
	{
		CheckInFov(world);
		m_fovCheckTimer.Reset();
	}

	m_foodStrength = std::max(m_foodStrength - delta * m_antsSettings->foodPheromoneStrengthLoss, 0.f);
	m_homeStrength = std::max(m_homeStrength - delta * m_antsSettings->homePheromoneStrengthLoss, 0.f);

	Rotate(delta);
	Move(delta, world);
}

void Ant::PostUpdate(const float delta, World &world)
{
	TileMap &tileMap = world.GetTileMap();

	if ( m_pheromoneSpawnTimer.IsElapsed())
	{
		SpawnPheromone(world);
		m_pheromoneSpawnTimer.Reset();
	}

	if ( m_takenFood )
	{
		if ( tileMap.GetTileType(m_takenFoodPos) == TileType::Food )
		{
			m_gotFood = true;
			m_state   = SearchForNest;
		}

		m_takenFood = false;
		tileMap.TakeFood(m_takenFoodPos);
	}

	if ( m_deliveredFood )
	{
		m_deliveredFood = false;
		world.IncDeliveredFoodCount();
	}
}

void Ant::Move(float delta, const World &world)
{
	m_prevPos = m_pos;

	const float speed = m_antsSettings->antMovementSpeed * delta;

	m_pos.x += speed * std::cos(m_rotation);
	m_pos.y += speed * std::sin(m_rotation);

	StayInBounds(world);
}

void Ant::Rotate(float delta)
{
//	static std::random_device                    rd;
//	static std::mt19937                          gen(rd());
//	static std::uniform_real_distribution<float> def(-1.f, 1.f);

	const float randomRotation = Random::Float(-1.f, 1.f) * m_antsSettings->antRandomRotation;

	m_desiredRotation += randomRotation;

	float rotationDiff = m_desiredRotation - m_rotation;
	rotationDiff = std::remainder(rotationDiff, 2.0f * static_cast<float>(M_PI));

	m_rotation += rotationDiff * m_antsSettings->antRotationSpeed * delta;
}

void Ant::SpawnPheromone(World &world)
{
	const IntVec2 pos = world.ScreenToWorld(m_pos.x, m_pos.y);

	if ( m_gotFood )
	{
		auto &foodPheromoneMap = world.GetFoodPheromoneMap();
		foodPheromoneMap.Add(pos.x, pos.y, m_antsSettings->foodPheromoneIntensity * m_foodStrength);
	}
	else
	{
		auto &homePheromoneMap = world.GetHomePheromoneMap();
		homePheromoneMap.Add(pos.x, pos.y, m_antsSettings->homePheromoneIntensity * m_homeStrength);
	}
}

void Ant::CheckCollisions(const World &world)
{
	const TileMap &tileMap    = world.GetTileMap();
	const IntVec2 checkMapPos = world.ScreenToWorld(m_pos.x, m_pos.y);

	switch ( m_state )
	{
		case SearchForFood:
			CheckFoodCollision(tileMap, checkMapPos);
			break;
		case SearchForNest:
			CheckHomeCollision(tileMap, checkMapPos);
			break;
		default:
			break;
	}

	const Tile &tile = tileMap.GetTile(checkMapPos);

	if ( !tile.IsPassable())
	{
		m_pos = m_prevPos;

		const IntVec2 prevMapPos = world.ScreenToWorld(m_prevPos.x, m_prevPos.y);

		if ( !tileMap.GetTile(prevMapPos).IsPassable())
		{
			m_pos = world.GetScreenHomePos();
		}

		TurnBackward();
		RandomizeRotation(M_PI_4);
	}
}

void Ant::CheckInFov(const World &world)
{
	auto &tileMap          = world.GetTileMap();
	auto &foodPheromoneMap = world.GetFoodPheromoneMap();
	auto &homePheromoneMap = world.GetHomePheromoneMap();

	// Caching rotations to improve performance
	float     checkRotations[3][2];
	for ( int i = -1; i <= 1; ++i )
	{
		const float rotation = m_rotation + static_cast<float>(i) * M_PI_4;

		float cosValue = std::cos(rotation);
		float sinValue = std::sin(rotation);

		if ( rotation > M_PI )
		{
			cosValue = -cosValue;
			sinValue = -sinValue;
		}

		checkRotations[i + 1][0] = cosValue;
		checkRotations[i + 1][1] = sinValue;
	}

	Vector2 checkPos = {0, 0};

	double strongestPheromone = 0;
	double checkedPheromone   = 0;

	bool foundObject    = false;
	bool foundPheromone = false;

	int prevSide = 0;
	int turnSide = 0;

	TileType tileType = TileType::Empty;

	float screenToMapRatio = world.GetScreenToWorldRatio();

	for ( int j = 1; j <= m_antsSettings->antFovRange && !foundObject; ++j )
	{
		const float multiplier = screenToMapRatio * static_cast<float>(j);
		for ( int   i          = -j / 2 - 1; i <= j / 2 + 1; ++i )
		{
			foundObject = false;
			// -1 left; 0 forward; 1 right
			int side = ( i > 0 ) - ( i < 0 );

			checkPos.x = m_pos.x + checkRotations[side + 1][0] * multiplier;
			checkPos.y = m_pos.y + checkRotations[side + 1][1] * multiplier;

			const IntVec2 checkMapPos = world.ScreenToWorld(checkPos.x, checkPos.y);

			tileType = tileMap.GetTileType(checkMapPos);

			// Avoid walls if they are close
			if ( tileType == TileType::Wall && j < 3 )
			{
				turnSide    = -side;
				foundObject = true;
				break;
			}
				// Go for the food
			else if ( tileType == TileType::Food && !m_gotFood )
			{
				turnSide    = side;
				foundObject = true;
				break;
			}

			if ( m_ignorePheromones )
			{
				continue;
			}

			// Look for home pheromones if ant have food
			if ( m_gotFood )
			{
				checkedPheromone = homePheromoneMap.Get(checkMapPos);
			}
				// Look for food pheromones if ant doesn't have food
			else
			{
				checkedPheromone = foodPheromoneMap.Get(checkMapPos);
			}

			if ( checkedPheromone > strongestPheromone )
			{
				strongestPheromone = checkedPheromone;
				turnSide           = side;
				foundPheromone     = true;
			}
				// Fixes ants going left when they have spotted two strong pheromones
			else if ( checkedPheromone == strongestPheromone )
			{
				turnSide = ( side == 0 || prevSide == 0 ) ? 0 : side;
			}

			prevSide = side;
		}
	}

	if ( foundPheromone || foundObject )
	{
		m_desiredRotation = m_rotation + turnSide * M_PI_4;
	}
}

void Ant::ChangeDesiredRotation(Vector2 desiredPos)
{
	float dx = desiredPos.x - m_pos.x;
	float dy = desiredPos.y - m_pos.y;
	m_desiredRotation = std::atan2(dy, dx);
}

void Ant::Draw()
{
	DrawCircleSector(m_pos, 2, 0, 360, 1, *m_colorsPtr[m_gotFood]);
}

void Ant::StayInBounds(const World &world)
{
	const float width  = world.GetScreenWidth();
	const float height = world.GetScreenHeight();

	bool outOfBounds = false;

	if ( m_pos.x >= width )
	{
		m_pos.x = width;
		outOfBounds = true;
	}
	else if ( m_pos.x < 0 )
	{
		m_pos.x = 0;
		outOfBounds = true;
	}

	if ( m_pos.y >= height )
	{
		m_pos.y = height;
		outOfBounds = true;
	}
	else if ( m_pos.y < 0 )
	{
		m_pos.y = 0;
		outOfBounds = true;
	}

	if ( outOfBounds )
	{
		m_pos = m_prevPos;
		TurnBackward();
		RandomizeRotation(M_PI_4);
	}
}

void Ant::RandomizeRotation(float pi)
{
	// TODO: Replace GetRandomValue to c++ random functions
//	m_rotation += ( pi * static_cast<float>(GetRandomValue(-100, 100))) / 100.f;
	m_rotation += ( pi * Random::Float(-1.f, 1.f));
}

void Ant::RandomizeDesiredRotation(float pi)
{
//	m_desiredRotation += ( pi * static_cast<float>(GetRandomValue(-100, 100))) / 100.f;
	m_desiredRotation += ( pi * Random::Float(-1.f, 1.f));
}

void Ant::CheckHomeCollision(const TileMap &tileMap, const IntVec2 &mapPos)
{
	TileType tileType = tileMap.GetTileType(mapPos);

	if ( tileType == TileType::Nest )
	{
		m_homeStrength  = 1;
		m_deliveredFood = true;
		m_gotFood       = false;
		m_state         = SearchForFood;
		TurnBackward();
	}
}

void Ant::CheckFoodCollision(const TileMap &tileMap, const IntVec2 &mapPos)
{
	TileType tileType = tileMap.GetTileType(mapPos);

	if ( tileType == TileType::Food )
	{
		m_foodStrength = 1;
		m_takenFood    = true;
		m_takenFoodPos = mapPos;
	}
}
