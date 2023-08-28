#include <random>

#include "Ant.hpp"
#include "World.hpp"
#include "ValueTable.hpp"

#include "omp.h"

constexpr float k_antFovCheckDelay = 0.05f;

bool CheckPointCircleCollision(Vector2 center1, Vector2 center2, float radius2)
{
	const float dx = center2.x - center1.x;
	const float dy = center2.y - center1.y;

	const float distanceSquared = dx * dx + dy * dy;

	return distanceSquared <= radius2 * radius2;
}

void Ant::Init(float startX, float startY, const AntsValueTable &valueTable)
{
	m_valueTable = &valueTable;

	m_pos          = {startX, startY};
	m_prevPos      = m_pos;
	m_angle        = static_cast<float>(GetRandomValue(M_PI * -100, M_PI * 100)) / 100.f;
	m_desiredAngle = m_angle;

	m_colorsPtr[0] = &m_valueTable->antDefaultColor;
	m_colorsPtr[1] = &m_valueTable->antWithFoodColor;
}

void Ant::Update(const float delta, const World &world)
{
	m_lastPheromoneSpawnTime += delta;
	m_lastFovCheckTime += delta;

	CheckCollisions(world);
	if ( m_lastFovCheckTime > k_antFovCheckDelay )
	{
		CheckInFov(world);
		m_lastFovCheckTime = 0;
	}

	m_foodStrength = std::max(m_foodStrength - delta * m_valueTable->foodPheromoneStrengthLoss, 0.f);
	m_homeStrength = std::max(m_homeStrength - delta * m_valueTable->homePheromoneStrengthLoss, 0.f);

	Rotate(delta);
	Move(delta);
}

void Ant::PostUpdate(const float delta, World &world)
{
	if ( m_lastPheromoneSpawnTime > m_valueTable->pheromoneSpawnDelay )
	{
		SpawnPheromone(world);
		m_lastPheromoneSpawnTime = 0;
	}

	if ( m_shouldDecreaseCell )
	{
		if ( world.GetCell(m_cellToDecreasePos.first, m_cellToDecreasePos.second).type == World::Food )
		{
			m_gotFood = true;
		}

		m_shouldDecreaseCell = false;
		world.DecreaseCell(m_cellToDecreasePos.first, m_cellToDecreasePos.second);
	}

	if ( m_deliveredFood )
	{
		m_deliveredFood = false;
		world.IncDeliveredFoodCount();
	}
}

void Ant::Move(float delta)
{
	m_prevPos = m_pos;

	const float speed = m_valueTable->antMovementSpeed * delta;

	m_pos.x += speed * std::cos(m_angle);
	m_pos.y += speed * std::sin(m_angle);

	StayOnScreen();
}

void Ant::Rotate(float delta)
{
	static std::random_device                    rd;
	static std::mt19937                          gen(rd());
	static std::uniform_real_distribution<float> dis(-1.f, 1.f);

	const float deviation   = ( GetRandomValue(0, 1000) <= m_valueTable->deviationChance ) ? dis(gen) : 0.0;
	const float randomAngle = dis(gen) * m_valueTable->antRandomAngle;

	m_desiredAngle += randomAngle + deviation;

	/*
	if ( angleDiff > M_PI )
	{
		angleDiff -= 2 * M_PI;
	}
	else if ( angleDiff < -M_PI )
	{
		angleDiff += 2 * M_PI;
	}
	 */

	float angleDiff = m_desiredAngle - m_angle;
	angleDiff = std::remainder(angleDiff, 2.0f * static_cast<float>(M_PI));

	m_angle += angleDiff * m_valueTable->antRotationSpeed * delta;
}

void Ant::SpawnPheromone(World &world)
{
	auto pos = world.ScreenToWorld(m_pos.x, m_pos.y);

	if ( m_gotFood )
	{
		world.AddFoodPheromone(pos.first, pos.second, m_valueTable->foodPheromoneIntensity * m_foodStrength);
	}
	else
	{
		world.AddHomePheromone(pos.first, pos.second, m_valueTable->homePheromoneIntensity * m_homeStrength);
	}
}

void Ant::CheckCollisions(const World &world)
{
	/* Home collision */
	const auto  homePos    = world.GetScreenHomePos();
	const float homeRadius = world.GetScreenHomeRadius();

	if ( CheckPointCircleCollision(m_pos, homePos, homeRadius))
	{
		m_homeStrength = 1;
		if ( m_gotFood )
		{
			m_deliveredFood = true;
			m_gotFood       = false;
			TurnBackward();
			return;
		}
	}

	/* Food collision */
	const auto checkMapPos = world.ScreenToWorld(m_pos.x, m_pos.y);
	auto       cellType    = world.GetCell(checkMapPos.first, checkMapPos.second).type;
	if ( cellType == World::Food )
	{
		m_pos = m_prevPos;

		TurnBackward();

		m_foodStrength = 1;

		if ( !m_gotFood )
		{
			m_shouldDecreaseCell = true;
			m_cellToDecreasePos  = checkMapPos;
		}
	}

	/* Wall collision */
	if ( cellType == World::Wall )
	{
		m_pos = m_prevPos;

		const auto prevMapPos = world.ScreenToWorld(m_prevPos.x, m_prevPos.y);
		auto       prevCellType    = world.GetCell(prevMapPos.first, prevMapPos.second).type;

		if(prevCellType == World::Wall)
		{
			m_pos = homePos;
		}

		RandomizeAngle(M_PI);
	}
}

void Ant::CheckInFov(const World &world)
{
	float     checkAngles[3][2];
	for ( int i = -1; i <= 1; ++i )
	{
		const float angleX = m_angle + static_cast<float>(i) * M_PI_4;

		float cosValue = std::cos(angleX);
		float sinValue = std::sin(angleX);

		if ( angleX > M_PI )
		{
			cosValue = -cosValue;
			sinValue = -sinValue;
		}

		checkAngles[i + 1][0] = cosValue;
		checkAngles[i + 1][1] = sinValue;
	}
#if 0
	for ( int i = -1; i <= 1; ++i )
	{
		float &checkAngleX = checkAngles[i + 1][0];
		checkAngleX = m_angle + i * M_PI_4;
		if ( checkAngleX > M_PI )
		{
			checkAngleX -= 2 * M_PI;
		}
		else if ( checkAngleX < -M_PI )
		{
			checkAngleX += 2 * M_PI;
		}

		float &checkAngleY = checkAngles[i + 1][1];
		checkAngleY = std::sin(checkAngleX);
		checkAngleX = std::cos(checkAngleX);
	}
#endif

	Vector2 checkPos = {0, 0};

	double strongestPheromone = 0;
	double checkedPheromone   = 0;

	bool foundThing = false;
	int  prevSide   = 0;
	int  turnSide   = 0;

	World::CellType cellType = World::None;

	float screenToMapRatio = world.GetScreenToWorldRatio();

	for ( int j = 1; j <= m_valueTable->antFovRange && !foundThing; ++j )
	{
		const float multiplier = screenToMapRatio * j;
		for ( int   i          = -j / 2 - 1; i <= j / 2 + 1; ++i )
		{
			foundThing = false;
			// -1 left; 0 forward; 1 right
			int side = ( i > 0 ) - ( i < 0 );

			checkPos.x = m_pos.x + checkAngles[side + 1][0] * multiplier;
			checkPos.y = m_pos.y + checkAngles[side + 1][1] * multiplier;

			const auto checkMapPos = world.ScreenToWorld(checkPos.x, checkPos.y);
			cellType = world.GetCell(checkMapPos.first, checkMapPos.second).type;

			if ( cellType == World::Wall && j < 3 )
			{
				turnSide   = -side;
				foundThing = true;
				break;
			}

			if ( m_gotFood )
			{
				checkedPheromone = world.GetHomePheromone(checkMapPos.first, checkMapPos.second);
			}
			else if ( cellType == World::Food )
			{
				turnSide   = side;
				foundThing = true;
				break;
			}
			else
			{
				checkedPheromone = world.GetFoodPheromone(checkMapPos.first, checkMapPos.second);
			}

			if ( checkedPheromone > strongestPheromone )
			{
				strongestPheromone = checkedPheromone;
				turnSide           = side;
			}
			else if ( checkedPheromone == strongestPheromone )
			{
				turnSide = ( side == 0 || prevSide == 0 ) ? 0 : side;
			}

			prevSide = side;
		}
	}

	if ( strongestPheromone > 0.01 || foundThing )
	{
		m_desiredAngle = m_angle + turnSide * M_PI_4;
	}
}

void Ant::ChangeDesiredAngle(Vector2 desiredPos)
{
	float dx = desiredPos.x - m_pos.x;
	float dy = desiredPos.y - m_pos.y;
	m_desiredAngle = std::atan2(dy, dx);
}

void Ant::Draw()
{
//	DrawCircle(m_pos.x, m_pos.y, 2, m_gotFood ? k_antWithFoodColor : k_antColor);
	DrawCircleSector(m_pos, 2, 0, 360, 1, *m_colorsPtr[m_gotFood]);
//	auto forward  = Vector2Rotate({10, 0}, m_angle);
//	auto dforward = Vector2Rotate({10, 0}, m_desiredAngle);
//	DrawLineV(m_pos, Vector2Add(m_pos, forward), BLUE);
//	DrawLineV(m_pos, Vector2Add(m_pos, dforward), GREEN);
}

void Ant::StayOnScreen()
{
	const auto width  = static_cast<float>(GetScreenWidth());
	const auto height = static_cast<float>(GetScreenHeight());

	bool out = false;

	if ( m_pos.x >= width )
	{
		m_pos.x = width;
		out = true;
	}
	else if ( m_pos.x < 0 )
	{
		m_pos.x = 0;
		out = true;
	}

	if ( m_pos.y >= height )
	{
		m_pos.y = height;
		out = true;
	}
	else if ( m_pos.y < 0 )
	{
		m_pos.y = 0;
		out = true;
	}

	if ( out )
	{
		m_pos = m_prevPos;
		TurnBackward();
	}
}

void Ant::RandomizeAngle(float pi)
{
	// TODO: Replace GetRandomValue to c++ random functions
	m_angle += ( pi * static_cast<float>(GetRandomValue(-100, 100))) / 100.f;
}

void Ant::RandomizeDesiredAngle(float pi)
{
	m_desiredAngle += ( pi * static_cast<float>(GetRandomValue(-100, 100))) / 100.f;
}
