#include <iostream>

#include "AntColony.hpp"
#include "Ant.hpp"

#include "Settings.hpp"

#include "omp.h"

AntColony::AntColony(AntColonyId id, const Vector2 &antsSpawnPos) :
		m_id(id)
{
	auto &settings = Settings::Instance();

	m_antsAmount    = settings.GetAntColonySettings().antsStartAmount;
	m_maxAntsAmount = Settings::Instance().GetAntColonySettings().antsMaxAmount;

	m_ants.resize(m_maxAntsAmount);
	for ( size_t i = 0; i < m_maxAntsAmount; ++i )
	{
		m_ants[i] = std::make_unique<Ant>(i, m_id, antsSpawnPos);
	}

	auto &globalSettings = settings.GetGlobalSettings();
	m_pheromoneMap = std::make_unique<PheromoneMap>(globalSettings.mapWidth, globalSettings.mapHeight,
	                                                settings.GetPheromoneMapSettings().pheromoneEvaporationRate);

	m_pheromoneSpawnTimer.SetDelay(settings.GetAntsSettings().pheromoneSpawnDelay);
	m_fovCheckTimer.SetDelay(settings.GetAntsSettings().fovCheckDelay);
	m_antDeathTimer.SetDelay(settings.GetAntColonySettings().antDeathDelay);

	std::cout << m_antsAmount << std::endl;
	std::cout << antsSpawnPos.x << " " << antsSpawnPos.y << std::endl;
}

void AntColony::Update(TileMap &tileMap)
{
//	UpdateTimers();

#pragma omp parallel for default(none) shared(m_ants, tileMap, m_pheromoneMap)
	for ( size_t i = 0; i < m_antsAmount; ++i )
	{
		m_ants[i]->Update(tileMap, *m_pheromoneMap);
	}

	for ( size_t i = 0; i < m_antsAmount; ++i )
	{
		m_ants[i]->PostUpdate(tileMap, *m_pheromoneMap);
	}

	m_pheromoneMap->Update();
}

void AntColony::SpawnAnt(const Vector2 &pos)
{
	if ( m_antsAmount >= Settings::Instance().GetAntColonySettings().antsMaxAmount )
	{
		return;
	}
//	++m_antsAmount;
}

void AntColony::RemoveAnt(AntId id)
{
	if ( id > m_antsAmount )
	{
		return;
	}

	std::unique_ptr<Ant> &antToRemove = m_ants[id];
	std::unique_ptr<Ant> &antToSwap   = m_ants[m_antsAmount];

	// Will it work?
	// Idk
	antToRemove = nullptr;
	antToSwap->SetId(id);
	antToSwap.swap(antToRemove);
	--m_antsAmount;
}

void AntColony::UpdateTimers()
{
	m_pheromoneSpawnTimer.Update(1);
	m_antDeathTimer.Update(1);
	m_fovCheckTimer.Update(1);
}

void AntColony::DrawAnts() const
{
	for ( size_t i = 0; i < m_antsAmount; ++i )
	{
		m_ants[i]->Draw();
	}
}

void AntColony::DrawPheromones() const
{
	m_pheromoneMap->Draw();
}
