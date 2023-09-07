#ifndef ANTS_ANT_HPP
#define ANTS_ANT_HPP

#include <cmath>
#include <raylib.h>

#include "Settings.hpp"
#include "IntVec.hpp"
#include "Timer.hpp"

class World;

class Ant
{
public:
	void Init(float startX, float startY, const AntsSettings &valueTable);

	void Update(float delta, const World &world);
	void PostUpdate(float delta, World &world);

	void CheckCollisions(const World &world);

	void SetGotFood(bool val) { m_gotFood = val; }

	void TurnBackward()
	{
		m_rotation -= M_PI;
		m_desiredRotation = m_rotation; /* - M_PI;*/ }

	void ChangeDesiredRotation(Vector2 desiredPos);

	Vector2 GetPos() const { return m_pos; }
	bool IsGotFood() const { return m_gotFood; }

	float GetFoodStrength() const { return m_foodStrength; }
	float GetHomeStrength() const { return m_homeStrength; }

	void Draw();

	void Rotate(float delta);

	void SpawnPheromone(World &world);

private:

	void Move(float delta, const World &world);

	void StayInBounds(const World& world);

	void CheckInFov(const World &world);

	void RandomizeRotation(float pi = M_PI);

	void RandomizeDesiredRotation(float pi = M_PI);

private:
	Vector2 m_prevPos;
	Vector2 m_pos;

	const AntsSettings *m_antsSettings;

	const Color *m_colorsPtr[2];

	float m_rotation;
	float m_desiredRotation;

	float m_homeStrength = 1;
	float m_foodStrength = 0;

	Timer m_pheromoneSpawnTimer;
	Timer m_fovCheckTimer;
	Timer m_deviationTimer;

	bool m_gotFood            = false;
	bool m_shouldDecreaseTile = false;
	bool m_deliveredFood      = false;
	bool m_ignorePheromones   = false;

	IntVec2 m_tileToDecreasePos;
};


#endif
