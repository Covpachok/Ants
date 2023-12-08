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

		ShowGlobalSettings(settings.GetGlobalSettings());
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

		ImGui::TreePop();
	}
}

void Gui::ShowAntColonySettings(AntColonySettings &antColonySettings)
{
	if ( ImGui::TreeNode("Ant Colony"))
	{
		ImGui::TreePop();
	}
}

void Gui::ShowGlobalSettings(GlobalSettings &globalSettings)
{
	if ( ImGui::TreeNode("Global"))
	{
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

		ImGui::TreePop();
	}
}

void Gui::ShowTileMapSettings(TileMapSettings &tileMapSettings)
{
	if ( ImGui::TreeNode("Tiles"))
	{
		ImGui::TreePop();
	}
}

void Gui::ShowWorldGenerationSettings(WorldGenerationSettings &worldGenerationSettings)
{
	if ( ImGui::TreeNode("World Generation"))
	{
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
		             paintTitles, static_cast<int>(TileType::eAmount));
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
