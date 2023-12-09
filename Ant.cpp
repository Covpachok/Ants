#include "Ant.hpp"

#include "World.hpp"
#include "Random.hpp"
#include "Settings.hpp"

#include <raymath.h>

#include "omp.h"

constexpr float k_pheromoneSpawnIntensity = 128;

constexpr float k_pheromoneSpawnDelay = 20;
constexpr float k_fovCheckDelay       = 3;

constexpr float k_decreasingPheromonesMultiplier = 1;

Ant::Ant(AntId id, AntColonyId colonyId, const Vector2 &pos) :
		m_id(id),
		m_colonyId(colonyId),
		m_pos(pos), m_prevPos(m_pos),
		m_state(SearchForFood),
		m_antsSettings(Settings::Instance().GetAntsSettings())
{
	m_rotation        = Random::Float(-M_PI, M_PI);
	m_desiredRotation = m_rotation;

	m_colorsPtr[0] = &m_antsSettings.antDefaultColor;
	m_colorsPtr[1] = &m_antsSettings.antWithFoodColor;

	m_pheromoneSpawnTimer.SetDelay(k_pheromoneSpawnDelay);
	m_fovCheckTimer.SetDelay(k_fovCheckDelay);
	RandomizeDeviationDelay();
	m_deviationResetTimer.SetDelay(m_antsSettings.deviationTime);
	m_lostPheromoneTimer.SetDelay(1000);
}

void Ant::Update(const TileMap &tileMap, const PheromoneMap &pheromoneMap)
{
	m_pheromoneSpawnTimer.Update(1);
	m_fovCheckTimer.Update(1);
	m_deviationTimer.Update(1);
	m_deviationResetTimer.Update(1);

	if ( m_deviationTimer.IsElapsed())
	{
		m_ignorePheromones = true;
		m_deviationTimer.Reset();
		m_deviationResetTimer.Reset();
		RandomizeDeviationDelay();
	}

	if ( m_deviationResetTimer.IsElapsed())
	{
		m_ignorePheromones   = false;
		m_decreasePheromones = false;
		m_deviationResetTimer.Reset();
	}

	Rotate();
	Move();

	CheckCollisions(tileMap);
	if ( m_fovCheckTimer.IsElapsed())
	{
		CheckInFov(tileMap, pheromoneMap);
		m_fovCheckTimer.Reset();
	}

	m_pheromoneStrength = std::max(m_pheromoneStrength - m_antsSettings.pheromoneStrengthLoss, 0.f);
}

void Ant::PostUpdate(TileMap &tileMap, PheromoneMap &pheromoneMap)
{
	if ( m_takenFood && tileMap.GetTileType(m_takenFoodPos) == TileType::eFood )
	{
		m_pheromoneStrength = 1;
		m_gotFood           = true;
		m_state             = SearchForNest;
		m_takenFood         = false;
		if ( tileMap.TakeFood(m_takenFoodPos))
		{
			pheromoneMap.Set(PheromoneType::Lost, m_takenFoodPos, 255);
			m_lostPheromoneTimer.Reset();
			m_spawnLostPheromone = true;
		}
	}

	if ( m_deliveredFood )
	{
		m_deliveredFood = false;
		m_decreasePheromones = false;
//		world.IncDeliveredFoodCount();
	}

	if ( m_pheromoneSpawnTimer.IsElapsed())
	{
		SpawnPheromone(pheromoneMap);
		m_pheromoneSpawnTimer.Reset();
	}

	if ( m_decreasePheromones )
	{
		DecreasePheromone(pheromoneMap);
	}
}

void Ant::Move()
{
	m_prevPos = m_pos;

	const float speed = m_antsSettings.antMovementSpeed;

	m_pos.x += speed * std::cos(m_rotation);
	m_pos.y += speed * std::sin(m_rotation);

	StayInBounds();
}

void Ant::Rotate()
{
	static std::mt19937                          gen{std::random_device()()};
	static std::uniform_real_distribution<float> def(-1.f, 1.f);

	const float randomRotation = def(gen) * m_antsSettings.antRandomRotation;

	m_desiredRotation += randomRotation;

	float rotationDiff = m_desiredRotation - m_rotation;
	rotationDiff = std::remainder(rotationDiff, 2.0f * static_cast<float>(M_PI));

	m_rotation += rotationDiff * m_antsSettings.antRotationSpeed;
}

void Ant::SpawnPheromone(PheromoneMap &pheromoneMap)
{
	const IntVec2 pos = {m_pos.x, m_pos.y};//Settings::Instance().GetGlobalSettings().ScreenToWorld(m_prevPos);

	if ( m_spawnLostPheromone )
	{
		pheromoneMap.Add(PheromoneType::Lost, pos.x, pos.y,
		                 ( k_pheromoneSpawnIntensity ) * m_pheromoneStrength);
	}
	else if ( m_gotFood )
	{
		pheromoneMap.Add(PheromoneType::Food, pos.x, pos.y,
		                 k_pheromoneSpawnIntensity * m_pheromoneStrength);
	}
	else
	{
		pheromoneMap.Add(PheromoneType::Nest, pos.x, pos.y,
		                 k_pheromoneSpawnIntensity * m_pheromoneStrength);
	}
}

void Ant::DecreasePheromone(PheromoneMap &pheromoneMap) const
{
	auto          settings = Settings::Instance();
	const IntVec2 pos      = {m_pos.x, m_pos.y};
	pheromoneMap.Substract(PheromoneType::Food, pos.x, pos.y,
	                       settings.GetPheromoneMapSettings().pheromoneEvaporationRate *
	                       k_decreasingPheromonesMultiplier);
}

void Ant::CheckCollisions(const TileMap &tileMap)
{
	const IntVec2 checkMapPos = {m_pos.x, m_pos.y};//Settings::Instance().GetGlobalSettings().ScreenToWorld(m_pos);

	switch ( m_state )
	{
		case SearchForFood:
			CheckFoodCollision(tileMap, checkMapPos);
			break;
		case SearchForNest:
			CheckNestCollision(tileMap, checkMapPos);
			break;
		default:
			break;
	}

	const Tile &tile = tileMap.GetTile(checkMapPos);

	if ( !tile.IsPassable())
	{
		m_pos = m_prevPos;

		const IntVec2 prevMapPos = {m_prevPos.x,
		                            m_prevPos.y};//Settings::Instance().GetGlobalSettings().ScreenToWorld(m_prevPos);

		if ( !tileMap.GetTile(prevMapPos).IsPassable())
		{
			//			m_pos = world.GetScreenHomePos();
			// FIND THE NEAREST NEST AND TELEPORT THERE
		}

		TurnBackward();
		RandomizeRotation(M_PI_4);
	}
}

void Ant::CheckInFov(const TileMap &tileMap, const PheromoneMap &pheromoneMap)
{
	// Caching rotations to improve performance
	float     checkRotations[3][2];
	for ( int i = -1; i <= 1; ++i )
	{
		const float rotation = m_rotation + i * M_PI_4;

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

	int turnSide = 0;
	int prevSide = 0;

//	float screenToMapRatio        = Settings::Instance().GetGlobalSettings().screenToMapRatio;
//	float screenToMapInverseRatio = Settings::Instance().GetGlobalSettings().screenToMapInverseRatio;

	PheromoneType searchForPheromoneType = PheromoneType::Nest;

	switch ( m_state )
	{
		case SearchForFood:
			searchForPheromoneType = PheromoneType::Food;
			break;
		case SearchForNest:
			searchForPheromoneType = PheromoneType::Nest;
			break;
		default:
			break;
	}

	for ( int j = 1; j <= m_antsSettings.antFovRange && !foundObject; ++j )
	{
		const float multiplier = static_cast<float>(j); //* screenToMapRatio;
		for ( int   i          = -j / 2 - 1; i <= j / 2 + 1; ++i )
		{
			foundObject = false;
			// -1 left; 0 forward; 1 right
			int side = ( i > 0 ) - ( i < 0 );

			checkPos.x = m_pos.x + checkRotations[side + 1][0] * multiplier;
			checkPos.y = m_pos.y + checkRotations[side + 1][1] * multiplier;

			IntVec2    checkMapPos = {checkPos.x, checkPos.y};
			const Tile &tile       = tileMap.GetTile(checkMapPos);

			if ( tile.GetType() == TileType::eFood && m_state == SearchForFood )
			{
				turnSide    = side;
				foundObject = true;
				break;
			}
			else if ( tile.GetType() == TileType::eWall && j < 3 )
			{
				turnSide    = -side;
				foundObject = true;
				break;
			}

			if ( m_ignorePheromones )
			{
				continue;
			}

			checkedPheromone = pheromoneMap.Get(searchForPheromoneType, checkMapPos);

			if ( m_state == SearchForFood &&
			     pheromoneMap.Get(PheromoneType::Lost, checkMapPos) > checkedPheromone )
			{
				m_decreasePheromones = true;
				return;
			}

			if ( checkedPheromone > strongestPheromone )
			{
				strongestPheromone = checkedPheromone;
				turnSide           = side;
				foundPheromone     = true;
			}
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
	DrawRectanglePro(Rectangle{m_pos.x, m_pos.y, 2.5f, 1.25f}, {1.25f, 0.625f}, m_rotation * ( 180.0 / M_PI ),
	                 *m_colorsPtr[m_gotFood]);
}

void Ant::StayInBounds()
{
	const auto width  = static_cast<float>(Settings::Instance().GetGlobalSettings().mapWidth);
	//* Settings::Instance().GetGlobalSettings().screenToMapRatio;
	const auto height = static_cast<float>(Settings::Instance().GetGlobalSettings().mapHeight);
	//* Settings::Instance().GetGlobalSettings().screenToMapRatio;

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
	m_rotation += ( pi * Random::Float(-1.f, 1.f));
	m_desiredRotation = m_rotation;
}

void Ant::RandomizeDesiredRotation(float pi)
{
	m_desiredRotation += ( pi * Random::Float(-1.f, 1.f));
}

void Ant::CheckNestCollision(const TileMap &tileMap, const IntVec2 &mapPos)
{
	TileType tileType = tileMap.GetTileType(mapPos);

	if ( tileType == TileType::eNest )
	{
		if ( m_gotFood )
		{
			// remake this
			auto nest       = tileMap.GetTile(mapPos).GetNest();
			auto nestColony = nest->GetColony();
			if ( nestColony && nestColony->GetId() == m_colonyId )
			{
				nest->AddFoodToStorage();
			}
		}

		m_pheromoneStrength  = 1;
		m_deliveredFood      = true;
		m_gotFood            = false;
		m_spawnLostPheromone = false;
		m_state              = SearchForFood;
		TurnBackward();
	}
}

void Ant::CheckFoodCollision(const TileMap &tileMap, const IntVec2 &mapPos)
{
	TileType tileType = tileMap.GetTileType(mapPos);

	if ( tileType == TileType::eFood )
	{
		m_takenFood    = true;
		m_takenFoodPos = mapPos;
		m_state        = SearchForNest;
	}
}

void Ant::RandomizeDeviationDelay()
{
	m_deviationTimer.SetDelay(Random::Int(m_antsSettings.deviationDelayMin, m_antsSettings.deviationDelayMax));
}
