#ifndef ANTS_ANT_HPP
#define ANTS_ANT_HPP

#include "raylib.h"
#include <cmath>
#include "ValueTable.hpp"

class World;

class Ant
{
public:
	void Init(float startX, float startY);

	void Update(float delta, World &world);

	void CheckCollisions(World &world);

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
private:
	void SpawnPheromone(World &world);

	void Move(float delta);

	void StayOnScreen();

	void CheckPheromones(World &world);

	void RandomizeAngle(float pi = M_PI);

	void RandomizeDesiredAngle(float pi = M_PI);

private:
	Vector2 m_prevPos;
	Vector2 m_pos;

	const AntsValueTable *m_table;

	float m_angle;
	float m_desiredAngle;

	float m_homeStrength = 1;
	float m_foodStrength = 0;

	bool m_gotFood = false;

	float m_lastPheromoneSpawnTime = 0;
	float m_lastPheromoneCheckTime = 0;
};


#endif
