#ifndef ANTS_ANT_HPP
#define ANTS_ANT_HPP

#include "raylib.h"
#include <cmath>

class World;

class Ant
{
public:
	void Init(float startX, float startY, float startSpeed);

	void Update(float delta);

	void CheckCollisions(World &world);

	void SetGotFood(bool val) { m_gotFood = val; }

	void TurnBackward() { m_angle -= M_PI;
		m_desiredAngle = m_angle; /* - M_PI;*/ }

	void ChangeDesiredAngle(Vector2 desiredPos);

	Vector2 GetPos() const { return m_pos; }
	bool IsGotFood() const { return m_gotFood; }

	float GetFoodStrength() const {return m_foodStrength;}
	float GetHomeStrength() const {return m_homeStrength;}

	void Draw();

	void Rotate(float delta);
private:
	void Move(float delta);

	void StayOnScreen();

	void CheckPheromones(World&world);

private:
	Vector2 m_pos;

	float m_speed;

	float m_angle;
	float m_desiredAngle;

	float m_homeStrength = 1;
	float m_foodStrength = 0;

	bool m_gotFood = false;

	Color m_color;
};


#endif
