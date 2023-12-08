#ifndef ANTS_ANT_HPP
#define ANTS_ANT_HPP

#include <cmath>
#include <raylib.h>

#include "PheromoneMap.hpp"
#include "TileMap.hpp"

#include "IntVec.hpp"
#include "Timer.hpp"

#include "Aliases.hpp"

class AntsSettings;

class Ant
{
	enum StateType
	{
		Roam, SearchForFood, SearchForNest
	};

public:
	Ant(AntId id, AntColonyId colonyId, const Vector2 &pos);

	void Update(const TileMap &tileMap, const PheromoneMap &pheromoneMap);
	void PostUpdate(TileMap &tileMap, PheromoneMap &pheromoneMap);

	void SetId(AntId newId) { m_id = newId; }

	AntId GetId() const { return m_id; }
	AntColonyId GetColonyId() const { return m_colonyId; }

	Vector2 GetPos() const { return m_pos; }
	bool IsGotFood() const { return m_gotFood; }

	void Draw();

private:
	void Rotate();
	void Move();
	void StayInBounds();

	void SpawnPheromone(PheromoneMap &pheromoneMap);
	void DecreasePheromone(PheromoneMap &pheromoneMap) const;

	void CheckInFov(const TileMap &tileMap, const PheromoneMap &pheromoneMap);
	void CheckCollisions(const TileMap &tileMap);

	void RandomizeRotation(float pi = M_PI);
	void RandomizeDesiredRotation(float pi = M_PI);

	void ChangeDesiredRotation(Vector2 desiredPos);
	void TurnBackward()
	{
		m_rotation -= M_PI;
		m_desiredRotation = m_rotation;
	}

	void CheckNestCollision(const TileMap &tileMap, const IntVec2 &mapPos);
	void CheckFoodCollision(const TileMap &tileMap, const IntVec2 &mapPos);

private:
	AntId       m_id;
	AntColonyId m_colonyId;

	Vector2 m_pos;
	Vector2 m_prevPos;

	StateType m_state;

	const AntsSettings &m_antsSettings;

	const Color *m_colorsPtr[2]{};

	float m_rotation;
	float m_desiredRotation;

	float m_pheromoneStrength = 1;

	Timer m_pheromoneSpawnTimer;
	Timer m_fovCheckTimer;
	Timer m_deviationTimer;
	Timer m_deviationResetTimer;
	Timer m_lostPheromoneTimer;

	bool m_gotFood          = false;
	bool m_takenFood        = false;
	bool m_deliveredFood    = false;
	bool m_ignorePheromones = false;
	bool m_decreasePheromones = false;
	bool m_spawnLostPheromone = false;

	IntVec2 m_takenFoodPos;
};


#endif
