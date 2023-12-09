#include "Gui.hpp"

#include "rlImGui.h"
#include <misc/cpp/imgui_stdlib.h>

#include "Simulation.hpp"
#include "Brush.hpp"
#include "ColorConvert.hpp"

void HelpTooltip(const std::string &text)
{
	ImGui::SameLine();
	ImGui::TextDisabled("(?)");
	if ( ImGui::IsItemHovered())
	{
		ImGui::SetTooltip(text.c_str());
	}
}

void Gui::ShowMainSettings(Simulation &simulation)
{
	/* Violation of encapsulation, but I DON'T CARE
	 * because that's just way faster and simpler */

	Brush &brush = simulation.m_brush;

	bool &drawPheromones       = simulation.m_drawPheromones;
	bool &drawAnts             = simulation.m_drawAnts;
	bool &pause                = simulation.m_pause;
	bool &adaptiveSpeed        = simulation.m_adaptiveSpeed;
	bool &showAdvancedSettings = simulation.m_showAdvancedSettings;

	float &gameSpeed = simulation.m_gameSpeed;

	ImGui::Begin("Main settings");
	{
		ShowBrushSettings(brush);

		ImGui::SeparatorText("Draw settings");
		{
			ImGui::Checkbox("[1] Pheromones", &drawPheromones);
			ImGui::SameLine();
			ImGui::Checkbox("[2] Ants", &drawAnts);
		}

		ImGui::SeparatorText("Speed settings");
		{
			ImGui::Checkbox("[SPACE] Pause", &pause);
			ImGui::SameLine();
			ImGui::Checkbox("[A] Adaptive speed", &adaptiveSpeed);

			ImGui::SliderFloat("[+][-] Simulation speed", &gameSpeed,
			                   simulation.k_minGameSpeed, simulation.k_maxGameSpeed);

			if ( ImGui::Button("Reset speed"))
			{
				simulation.m_gameSpeed = 1;
			}
		}

		ImGui::SeparatorText("Other");

		ImGui::Separator();

		if ( ImGui::Button("[R] Restart simulation"))
		{
			simulation.Reset();
		}
		ImGui::SameLine();

		if ( ImGui::Button("Clear map"))
		{
			simulation.m_world->ClearMap();
		}

		if ( ImGui::Button("Reset camera"))
		{
			simulation.ResetCamera();
		}

		ImGui::Separator();

		ImGui::Checkbox("Show advanced settings", &showAdvancedSettings);

		static std::string fileName;
		ImGui::InputText("image", &fileName);
		if ( ImGui::Button("load world from image"))
		{
			// Very ugly
			if ( simulation.m_world->LoadWorldFromImage(simulation.m_settings, fileName))
			{
				simulation.m_coloniesManager = std::make_unique<ColoniesManager>(simulation.m_world->GetTileMap());
			}
		}
	}
	ImGui::End();
}

void Gui::ShowStatistics()
{
//	ImGui::Value("Ants amount", static_cast<int>(coloniesManager->GetColonies()[0]->GetAntsAmount()));
}

void Gui::ShowAdvancedSettings(Settings &settings)
{
	ImGui::Begin("Advanced settings");
	{
		ImGui::Separator();

		ShowGlobalSettings(settings.GetGlobalSettings(), settings.GetPheromoneMapSettings());
		ShowAntsSettings(settings.GetAntsSettings());
		ShowAntColonySettings(settings.GetAntColonySettings());
		ShowTileMapSettings(settings.GetTileMapSettings());
		ShowWorldGenerationSettings(settings.GetWorldGenerationSettings());

		ImGui::Text(" ");
		ImGui::Separator();

		ShowSaveLoadSettings(settings);

		if ( ImGui::Button("Reset settings values"))
		{
			settings.Reset();
		}
	}
	ImGui::End();
}

void Gui::ShowAntsSettings(AntsSettings &antsSettings)
{
	if ( ImGui::TreeNode("Ants"))
	{
		auto imguiColor = ColorConvert::RayColorToFloat4(antsSettings.antDefaultColor);
		ImGui::ColorEdit4("Default", imguiColor.color);
		antsSettings.antDefaultColor = ColorConvert::Float4ToRayColor(imguiColor.color);

		imguiColor = ColorConvert::RayColorToFloat4(antsSettings.antWithFoodColor);
		ImGui::ColorEdit4("With food", imguiColor.color);
		antsSettings.antWithFoodColor = ColorConvert::Float4ToRayColor(imguiColor.color);

		ImGui::PushItemWidth(200);

		ImGui::SeparatorText("Movement");

		ImGui::InputFloat("Movement speed", &antsSettings.antMovementSpeed);
		ImGui::InputFloat("Rotation speed", &antsSettings.antRotationSpeed);

		ImGui::SliderFloat("Random angle", &antsSettings.antRandomRotation, 0.f, 1.f);
		HelpTooltip("When ants wander, they choose a random angle\n"
		            "in which to look and move in that direction");

		ImGui::SeparatorText("Perception");

		ImGui::SliderInt("FOV range", &antsSettings.antFovRange, 2, 16);
		HelpTooltip("Ants can perceive objects in a cone in front of them,\n"
		            "this variable affects size of this cone.\n"
		            "Heavily affects performance");

		ImGui::SeparatorText("Pheromones");

		ImGui::InputFloat("Strength loss", &antsSettings.pheromoneStrengthLoss);
		HelpTooltip("Each time ant takes food/returns to the nest, pheromone \n"
		            "strength resets, and then slowly deteriorate to 0.\n"
		            "If value is too high, ants may not form path to distant food source.");

		ImGui::SeparatorText("Deviation");

		ImGui::DragIntRange2("Deviation delay range", &antsSettings.deviationDelayMin, &antsSettings.deviationDelayMax,
		                     10, 10, 10000);

		ImGui::InputInt("Deviation time", &antsSettings.deviationTime);


		ImGui::PopItemWidth();
		ImGui::TreePop();
	}
}

void Gui::ShowAntColonySettings(AntColonySettings &antColonySettings)
{
	if ( ImGui::TreeNode("Ant Colony"))
	{
		ImGui::PushItemWidth(200);
		if ( ImGui::InputInt("Ants start amount", &antColonySettings.antsStartAmount))
		{
			antColonySettings.antsStartAmount = std::max(antColonySettings.antsStartAmount, 1);
		}

		if ( ImGui::InputInt("Ants max amount", &antColonySettings.antsMaxAmount))
		{
			antColonySettings.antsMaxAmount = std::max(antColonySettings.antsMaxAmount, 1);
		}

		ImGui::Checkbox("Dynamic life", &antColonySettings.dynamicLife);
		HelpTooltip("If true, ants will die and spawn depending of food in the colony.");

		if ( antColonySettings.dynamicLife )
		{
			if ( ImGui::InputInt("Ant death delay", &antColonySettings.antDeathDelay))
			{
				antColonySettings.antDeathDelay = std::max(antColonySettings.antDeathDelay, 1);
			}

			if ( ImGui::InputInt("Food to spawn ant", &antColonySettings.foodToSpawnAnt))
			{
				antColonySettings.foodToSpawnAnt = std::max(antColonySettings.foodToSpawnAnt, 1);
			}
		}


		ImGui::PopItemWidth();
		ImGui::TreePop();
	}
}

void Gui::ShowGlobalSettings(GlobalSettings &globalSettings, PheromoneMapSettings &pheromoneMapSettings)
{
	if ( ImGui::TreeNode("Map"))
	{
		ImGui::PushItemWidth(200);
		ImGui::Text("Restart to apply these");

		int width  = static_cast<int>(globalSettings.mapWidth);
		int height = static_cast<int>(globalSettings.mapHeight);

		ImGui::InputInt("MapWidth", &width);
		ImGui::InputInt("MapHeight", &height);

		if ( width > 0 && height > 0 )
		{
			globalSettings.mapWidth  = static_cast<size_t>(width);
			globalSettings.mapHeight = static_cast<size_t>(height);
		}

		ImGui::InputFloat("Pheromone evaporation rate", &pheromoneMapSettings.pheromoneEvaporationRate);

		ImGui::PopItemWidth();
		ImGui::TreePop();
	}
}

void Gui::ShowTileMapSettings(TileMapSettings &tileMapSettings)
{
	if ( ImGui::TreeNode("Tiles"))
	{
		ImGui::PushItemWidth(200);
		if ( ImGui::InputInt("Food amount on FoodTile", &tileMapSettings.foodDefaultAmount))
		{
			tileMapSettings.foodDefaultAmount = std::max(tileMapSettings.foodDefaultAmount, 1);
		}

		ImGui::PopItemWidth();
		ImGui::TreePop();
	}
}

void Gui::ShowWorldGenerationSettings(WorldGenerationSettings &worldGenerationSettings)
{
	if ( ImGui::TreeNode("World Generation"))
	{
		ImGui::PushItemWidth(200);

		ImGui::DragFloatRange2("Food range", &worldGenerationSettings.foodRange.low,
		                       &worldGenerationSettings.foodRange.high, 0.01f, 0.f, 1.f);
		ImGui::DragFloatRange2("Wall range", &worldGenerationSettings.wallRange.low,
		                       &worldGenerationSettings.wallRange.high, 0.01f, 0.f, 1.f);
		ImGui::DragFloatRange2("Empty range", &worldGenerationSettings.emptyRange.low,
		                       &worldGenerationSettings.emptyRange.high, 0.01f, 0.f, 1.f);

		ImGui::Separator();

		ImGui::SliderFloat("Noise scale", &worldGenerationSettings.noiseScale, 1.f, 16.f);
		ImGui::SliderFloat("Noise contrast", &worldGenerationSettings.noiseContrast, 0.1f, 4.f);
		ImGui::SliderFloat("Noise blur", &worldGenerationSettings.noiseBlur, 0.f, 2.f);
		ImGui::SliderFloat("Ridges intensity", &worldGenerationSettings.ridgesIntensity, 0.f, 1.f);
		ImGui::SliderInt("Noise octaves", &worldGenerationSettings.noiseOctaves, 1, 6);

		ImGui::PopItemWidth();
		ImGui::TreePop();
	}
}

bool Gui::ShouldHandleInput()
{
	ImGuiIO &io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	return !( io.WantCaptureMouse || io.WantCaptureKeyboard );
}

void Gui::ShowBrushSettings(Brush &brush)
{
	if ( ImGui::TreeNode("Brush settings"))
	{
		const char *brushTitles[Brush::BrushType::Amount] = {"Point", "Square", "Round"};
		const char *paintTitles[TileType::eAmount]        = {"Empty", "Wall", "Food", "Nest"};

		int              size      = brush.GetBrushSize();
		Brush::BrushType brushType = brush.GetBrushType();
		TileType         paintType = brush.GetPaintType();

		ImGui::Text("Press Left Mouse Button to paint");

		ImGui::SliderInt("Brush size", &size, 1, 100);

		ImGui::Combo("Paint type", reinterpret_cast<int *>(&paintType),
		             paintTitles, static_cast<int>(TileType::eNest));
		ImGui::Combo("Brush type", reinterpret_cast<int *>(&brushType),
		             brushTitles, static_cast<int>(Brush::BrushType::Amount));

		brush.SetBrushSize(size);
		brush.SetBrushType(brushType);
		brush.SetPaintType(paintType);

		ImGui::TreePop();
	}
}

void Gui::ShowSaveLoadSettings(Settings &settings)
{
	if ( ImGui::TreeNode("Save/Load settings"))
	{
		static std::string              saveFilename;
		static std::vector<std::string> savedSettingsFiles = Settings::FindSavedSettings();

		ImGui::PushItemWidth(200);

		ImGui::Text("Filename");
		ImGui::InputText("S", &saveFilename);

		ImGui::SameLine();

		if ( ImGui::Button("Save"))
		{
			settings.Save(saveFilename);
			savedSettingsFiles = Settings::FindSavedSettings();
		}

		static size_t selectedIndex = 0;
		if ( savedSettingsFiles.empty())
		{
			ImGui::TextDisabled("Not found any save files");
		}
		else
		{
			const char *previewValue = savedSettingsFiles[selectedIndex].c_str();
			if ( ImGui::BeginCombo("L", previewValue))
			{
				for ( size_t i = 0; i < savedSettingsFiles.size(); ++i )
				{
					const bool isSelected = ( selectedIndex == i );
					if ( ImGui::Selectable(savedSettingsFiles[i].c_str(), isSelected))
					{
						selectedIndex = i;
					}

					if ( isSelected )
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
		}

		ImGui::SameLine();

		if ( ImGui::Button("Load"))
		{
			settings.Load(savedSettingsFiles[selectedIndex]);
		}

		ImGui::SameLine();

		if ( ImGui::Button("Update list"))
		{
			savedSettingsFiles = Settings::FindSavedSettings();
		}

		ImGui::PopItemWidth();
		ImGui::TreePop();
	}
}
