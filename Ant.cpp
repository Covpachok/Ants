#include <random>

#include "Ant.hpp"
#include "World.hpp"
#include "ValueTable.hpp"

//const float k_antRotationSpeed    = 12; // 12
//const float k_antRandomAngle      = 0.3; // 0.3
//const float k_antFoodStrengthLoss = 0.025; // 0.02
//const float k_antHomeStrengthLoss = 0.025; // 0.02
//const int   k_antFovRange         = 8;

void Ant::Init(float startX, float startY)
{
	m_table = &g_valueTable.GetAntsTable();

	m_pos          = {startX, startY};
	m_prevPos      = m_pos;
	m_angle        = static_cast<float>(GetRandomValue(M_PI * -100, M_PI * 100)) / 100.f;
	m_desiredAngle = m_angle;
}

void Ant::Update(float delta, World &world)
{
	m_lastPheromoneSpawnTime += delta;
	m_lastPheromoneCheckTime += delta;

	CheckCollisions(world);
	if ( m_lastPheromoneCheckTime > m_table->antPheromoneCheckDelay )
	{
		CheckPheromones(world);
		m_lastPheromoneCheckTime = 0;
	}

	m_foodStrength = std::max(m_foodStrength - delta * m_table->foodPheromoneStrengthLoss, 0.f);
	m_homeStrength = std::max(m_homeStrength - delta * m_table->homePheromoneStrengthLoss, 0.f);

	Rotate(delta);
	Move(delta);

	if ( m_lastPheromoneSpawnTime > m_table->pheromoneSpawnDelay )
	{
		SpawnPheromone(world);
		m_lastPheromoneSpawnTime = 0;
	}
}

void Ant::Move(float delta)
{
	m_prevPos = m_pos;

	auto speed = m_table->antMovementSpeed * delta;

	m_pos.x += speed * std::cos(m_angle);
	m_pos.y += speed * std::sin(m_angle);

	StayOnScreen();
}

void Ant::Rotate(float delta)
{
	static std::random_device               rd;
	static std::mt19937                     gen(rd());
	static std::uniform_real_distribution<> dis(-1.0, 1.0);

	int deviation = ( GetRandomValue(0, 100) <= 1 );

	float randomAngle = dis(gen) * m_table->antRandomAngle + deviation * dis(gen);

	m_desiredAngle += randomAngle;

	float angleDiff = m_desiredAngle - m_angle;
	if ( angleDiff > M_PI )
	{
		angleDiff -= 2 * M_PI;
	}
	else if ( angleDiff < -M_PI )
	{
		angleDiff += 2 * M_PI;
	}
	m_angle += angleDiff * m_table->antRotationSpeed * delta;
}

void Ant::SpawnPheromone(World &world)
{
	auto pos = world.ScreenToWorld(m_pos.x, m_pos.y);

	if ( m_gotFood )
	{
		world.AddFoodPheromone(pos.first, pos.second, m_table->foodPheromoneIntensity * m_foodStrength);
	}
	else
	{
		world.AddHomePheromone(pos.first, pos.second, m_table->homePheromoneIntensity * m_homeStrength);
	}
}

void Ant::CheckCollisions(World &world)
{
	const auto  homePos    = world.GetScreenHomePos();
	const float homeRadius = world.GetScreenHomeRadius();

	if ( CheckCollisionPointCircle(m_pos, homePos, homeRadius))
	{
		if ( m_gotFood )
		{
			m_homeStrength = 1;
			m_gotFood      = false;
			TurnBackward();
			return;
		}
	}

	const auto checkMapPos = world.ScreenToWorld(m_pos.x, m_pos.y);
	auto       cellType    = world.GetCell(checkMapPos.first, checkMapPos.second).type;
	if ( cellType == World::Food )
	{
		m_pos = m_prevPos;

		TurnBackward();

		if ( !m_gotFood )
		{
			m_foodStrength = 1;
			world.DecreaseCell(checkMapPos.first, checkMapPos.second);
			m_gotFood = true;
//			TurnBackward();
		}
	}

	if ( cellType == World::Wall )
	{
		m_pos = m_prevPos;

		RandomizeAngle(M_PI_2);

		CheckPheromones(world);
	}
}

void Ant::CheckPheromones(World &world)
{
	Vector2 checkPos;

	double strongestPheromone = 0;
	double checkedPheromone;

	float     checkAngles[3][2];
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

	bool foundThing = false;
	int  prevSide   = 0;
	int  turnSide   = 0;

	World::CellType cellType;

	float screenToMapRatio = world.GetScreenToWorldRatio();

	for ( int j = 1; j <= m_table->antFovRange && !foundThing; ++j )
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

			if ( cellType == World::Wall && j < 2 )
			{
				turnSide   = -side;
				foundThing = true;
				break;
			}

//				if ( m_gotFood || m_homeStrength < 0.1 )
			if ( m_gotFood )
			{
				checkedPheromone = world.GetHomePheromone(checkMapPos.first, checkMapPos.second);
			}
			else if ( cellType == World::Food )
			{
				checkedPheromone = 9999;
				turnSide         = side;
				foundThing       = true;
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

	if ( strongestPheromone <= 0.01 )
	{
		return;
	}

	m_desiredAngle = m_angle + turnSide * M_PI_4;
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
	DrawCircleSector(m_pos, 2, 0, 360, 1, *m_table->antColorPtr[m_gotFood]);
//	auto forward  = Vector2Rotate({10, 0}, m_angle);
//	auto dforward = Vector2Rotate({10, 0}, m_desiredAngle);
//	DrawLineV(m_pos, Vector2Add(m_pos, forward), BLUE);
//	DrawLineV(m_pos, Vector2Add(m_pos, dforward), GREEN);
}

void Ant::StayOnScreen()
{
	const float width  = (float) GetScreenWidth();
	const float height = (float) GetScreenHeight();

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

#if 0
	const int offset       = 3;
	const int doubleOffset = offset * 2;

	if ( m_pos.x >= GetScreenWidth() - doubleOffset || m_pos.y >= GetScreenHeight() - doubleOffset ||
		 m_pos.x < doubleOffset || m_pos.y < doubleOffset )
	{
		TurnBackward();
	}

	if ( m_pos.x >= GetScreenWidth() - offset )
	{
		m_pos.x = GetScreenWidth() - offset;
	}
	if ( m_pos.y >= GetScreenHeight() - offset )
	{
		m_pos.y = GetScreenHeight() - offset;
	}
	if ( m_pos.x < offset )
	{
		m_pos.x = offset;
	}
	if ( m_pos.y < offset )
	{
		m_pos.y = offset;
	}
#endif
}

void Ant::RandomizeAngle(float pi)
{
	m_angle += ( pi * static_cast<float>(GetRandomValue(-100, 100))) / 100.0;
}

void Ant::RandomizeDesiredAngle(float pi)
{
	m_desiredAngle += ( pi * static_cast<float>(GetRandomValue(-100, 100))) / 100.0;
}

