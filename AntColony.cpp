#include "AntColony.hpp"
#include "Ant.hpp"

AntColony::AntColony(const IntVec2 &nestPos) :
		m_id(s_colonyIds++)
{
	auto &settings = Settings::Instance();

	m_antsAmount = settings.GetWorldSettings().antsAmount;
	m_ants.resize(m_antsAmount);

	Vector2 antSpawnPos = {
			static_cast<float>(nestPos.x) * settings.GetWorldSettings().screenToMapRatio,
			static_cast<float>(nestPos.y) * settings.GetWorldSettings().screenToMapRatio
	};

	for ( auto &ant: m_ants )
	{
		ant = std::make_unique<Ant>(m_id, antSpawnPos);
	}
}

void AntColony::Update(const TileMap &tileMap)
{
//	UpdateTimers();

	for(size_t i = 0; i < m_antsAmount; ++i)
	{
//		m_ants[i]->Update(tileMap);
	}

	for(size_t i = 0; i < m_antsAmount; ++i)
	{
//		m_ants[i]->PostUpdate(tileMap);
	}
}

void AntColony::SpawnAnt(const Nest &spawnNest)
{
	if(m_antsAmount >= m_antsMaxAmount)
	{
		return;
	}

	++m_antsAmount;
}
