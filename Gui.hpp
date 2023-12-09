#ifndef ANTS_GUI_HPP
#define ANTS_GUI_HPP

#include "Settings.hpp"

class Simulation;
class Brush;

class Gui
{
public:
	void ShowMainSettings(Simulation &simulation);
	void ShowStatistics();
	void ShowAdvancedSettings(Settings &settings);

	bool ShouldHandleInput();

private:
	void ShowAntsSettings(AntsSettings &antsSettings);
	void ShowAntColonySettings(AntColonySettings &antColonySettings);
	void ShowGlobalSettings(GlobalSettings &globalSettings, PheromoneMapSettings& pheromoneMapSettings);
	void ShowTileMapSettings(TileMapSettings &tileMapSettings);
	void ShowWorldGenerationSettings(WorldGenerationSettings &worldGenerationSettings);
	void ShowSaveLoadSettings(Settings &settings);

	void ShowBrushSettings(Brush &brush);

};


#endif //ANTS_GUI_HPP
