#ifndef ANTS_ANT_HPP
#define ANTS_ANT_HPP

#include <cmath>

#include <raylib.h>
#include "ValueTable.hpp"

class World;

class Ant
{
public:
	void Init(float startX, float startY);

	void Update(float delta, const World &world);
	void PostUpdate(float delta, World &world);

	void CheckCollisions(const World &world);

	void SetGotFood(bool val) { m_gotFood = val; }

	void TurnBackward()
	{
		m_angle -= M_PI;
		m_desiredAngle = m_angle; /* - M_PI;*/ }

	void ChangeDesiredAngle(Vector2 desiredPos);

	Vector2 GetPos() const { return m_pos; }
	bool IsGotFood() const { return m_gotFood; }

	float GetFoodStrength() const { return m_foodStrength; }
	float GetHomeStrength() const { return m_homeStrength; }

	void Draw();

	void Rotate(float delta);

	void SpawnPheromone(World &world);

private:

	void Move(float delta);

	void StayOnScreen();

	void CheckInFov(const World &world);

	void RandomizeAngle(float pi = M_PI);

	void RandomizeDesiredAngle(float pi = M_PI);

private:
	Vector2 m_prevPos;
	Vector2 m_pos;

	const AntsValueTable *m_table;

	const Color *m_colorsPtr[2];

	float m_angle;
	float m_desiredAngle;

	float m_homeStrength = 1;
	float m_foodStrength = 0;

	float m_lastPheromoneSpawnTime = 0;
	float m_lastPheromoneCheckTime = 0;

	bool m_gotFood            = false;
	bool m_shouldDecreaseCell = false;
	bool m_deliveredFood = false;

	std::pair<int, int> m_cellToDecreasePos;
};


#endif
