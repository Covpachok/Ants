#include "Ant.hpp"
#include <cmath>
#include <random>
#include <iostream>
#include "World.hpp"
#include "raymath.h"

const float kAntRotationSpeed    = 12;
const float kAntRandomAngle      = 0.3;
const float kAntFoodStrengthLoss = 0.005;//15f;
const float kAntHomeStrengthLoss = 0.001;//15f;
int         kAntFovRange         = 8;
const Color kAntColor            = {255, 255, 0, 127};
const Color kAntWithFoodColor    = {0, 255, 0, 127};

void Ant::Init(float startX, float startY, float startSpeed)
{
	m_pos          = {startX, startY};
	m_speed        = startSpeed;
	m_angle        = GetRandomValue(M_PI * -100, M_PI * 100) / 100.0;
	m_desiredAngle = m_angle;
	m_color        = kAntColor;
}

void Ant::Update(float delta)
{
	m_foodStrength = std::max(m_foodStrength - delta * kAntFoodStrengthLoss, 0.f);
	m_homeStrength = std::max(m_homeStrength - delta * kAntHomeStrengthLoss, 0.f);

	Rotate(delta);
	Move(delta);
}

void Ant::Move(float delta)
{
	m_pos.x += m_speed * std::cos(m_angle) * delta;
	m_pos.y += m_speed * std::sin(m_angle) * delta;

	StayOnScreen();
}

void Ant::Rotate(float delta)
{
	static std::random_device               rd;
	static std::mt19937                     gen(rd());
	static std::uniform_real_distribution<> dis(-1.0, 1.0);
	float                                   randomAngle = dis(gen) * kAntRandomAngle;

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
	m_angle += angleDiff * kAntRotationSpeed * delta;
}

void Ant::CheckCollisions(World &world)
{
	const auto  homePos    = world.GetScreenHomePos();
	const float homeRadius = world.GetScreenHomeRadius();

	if ( CheckCollisionPointCircle(m_pos, homePos, homeRadius))
	{
		m_homeStrength = 1;

		if ( m_gotFood )
		{
			m_gotFood = false;
			m_color   = kAntColor;
			TurnBackward();
			return;
		}
	}

	const auto checkMapPos = world.ScreenToMap(m_pos.x, m_pos.y);
	if ( world.GetCell(checkMapPos.first, checkMapPos.second).type == World::Food )
	{
		m_foodStrength = 1;
//		TurnBackward();

		if ( !m_gotFood )
		{
			world.DecreaseCell(checkMapPos.first, checkMapPos.second);
			m_gotFood = true;
			m_color   = kAntWithFoodColor;
			TurnBackward();
		}
	}

	CheckPheromones(world);
}

void Ant::CheckPheromones(World &world)
{
	Vector2 checkPos;

	double strongestPheromone = 0;
	double checkedPheromone;

	float turnSide = 0;

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

	int       prevSign  = 0;
	bool      foundFood = false;
	for ( int j         = 1; j <= kAntFovRange && !foundFood; ++j )
	{
		for ( int i = -j / 2 - 1; i <= j / 2 + 1; ++i )
		{
			foundFood = false;
			int         sign       = ( i > 0 ) - ( i < 0 );
			const float multiplier = world.GetScreenToMapRatio() * j;

			checkPos.x = m_pos.x + checkAngles[sign + 1][0] * multiplier;
			checkPos.y = m_pos.y + checkAngles[sign + 1][1] * multiplier;

			const auto checkMapPos = world.ScreenToMap(checkPos.x, checkPos.y);
			if ( m_gotFood || m_homeStrength < 0.1 )
			{
				checkedPheromone = world.GetHomePheromone(checkMapPos.first, checkMapPos.second);
			}
			else
			{
				if ( world.GetCell(checkMapPos.first, checkMapPos.second).type == World::Food )
				{
					checkedPheromone = 9999;
					foundFood        = true;
				}
				else
				{
					checkedPheromone = world.GetFoodPheromone(checkMapPos.first, checkMapPos.second);
				}
			}

			if ( checkedPheromone > strongestPheromone )
			{
				strongestPheromone = checkedPheromone;
				turnSide           = sign * M_PI_4;
			}
			else if ( checkedPheromone == strongestPheromone )
			{
				if ( sign == 0 || prevSign == 0 )
				{
					turnSide = 0;
				}
				else
				{
					turnSide = sign * M_PI_4;
				}
			}

			prevSign = sign;

			if ( foundFood )
			{
				break;
			}
		}
	}

	if ( strongestPheromone <= 0.01 )
	{
		return;
	}

	m_desiredAngle = m_angle + turnSide;
}

void Ant::ChangeDesiredAngle(Vector2 desiredPos)
{
	float dx = desiredPos.x - m_pos.x;
	float dy = desiredPos.y - m_pos.y;
	m_desiredAngle = std::atan2(dy, dx);
}


void Ant::Draw()
{
//	DrawCircle(m_pos.x, m_pos.y, 2, m_gotFood ? kAntWithFoodColor : kAntColor);
	DrawCircleSector(m_pos, 1, 0, 360, 1, m_color);
//	auto forward  = Vector2Rotate({10, 0}, m_angle);
//	auto dforward = Vector2Rotate({10, 0}, m_desiredAngle);
//	DrawLineV(m_pos, Vector2Add(m_pos, forward), BLUE);
//	DrawLineV(m_pos, Vector2Add(m_pos, dforward), GREEN);
}

void Ant::StayOnScreen()
{
	const float width  = (float) GetScreenWidth();
	const float height = (float) GetScreenHeight();

	if ( m_pos.x >= width )
	{
		m_pos.x = 0;
	}
	else if ( m_pos.x < 0 )
	{
		m_pos.x = width;
	}

	if ( m_pos.y >= height )
	{
		m_pos.y = 0;
	}
	else if ( m_pos.y < 0 )
	{
		m_pos.y = height;
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

