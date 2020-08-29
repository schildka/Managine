#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <engine/assets/texture.h>

#include <map>

namespace engine::ui {

	enum MenuState{
		MainMenu, ResourceMenu, BuildMenu
	};

	class UI {
	public:
		UI() {};
		UI(GLFWwindow * window, double deltaTime);
		~UI();

		void drawDebugUI(double deltaTime);
		void drawGameUI(double deltaTime);

		void loadIcons(std::filesystem::path paths[11]);

	public:
		MenuState menuState = ResourceMenu;

		std::string questLog = "Don't starve!";
		bool isQuestLogActive = true;

		unsigned int stone;
		unsigned int ore;
		unsigned int wood;
		unsigned int corn;

		unsigned int neededStone;
		unsigned int neededOre;
		unsigned int neededWood;
		unsigned int neededCorn;


	private:
		engine::assets::Texture woodImage;
		engine::assets::Texture stoneImage;
		engine::assets::Texture oreImage;
		engine::assets::Texture cornImage;

		engine::assets::Texture escImage;
		engine::assets::Texture spaceImage;
		engine::assets::Texture backImage;
		engine::assets::Texture altImage;
		engine::assets::Texture mouseLeftImage;
		engine::assets::Texture mouseImage;
		engine::assets::Texture wasdImage;

		
	};
}