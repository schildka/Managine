#include "mainUI.h"

#include <engine/assets/text.h>

namespace resourceGame {

	MainUI::MainUI(engine::Engine* engine)  {
		ui = new engine::ui::UI(engine->context->getWindow(), engine->context->getDeltaTime(), engine::ui::ResourceMenu, "Don't starve!");

		std::filesystem::path paths[11] = { "icons/wood.png", "icons/stone.png", "icons/ore.png", "icons/corn.png", "icons/esc.png", "icons/space.png", "icons/back.png", "icons/alt.png", "icons/mouseKey.png", "icons/mouse.png", "icons/wasd.png" };
		ui->loadIcons(paths);

		engine->renderer->registerUI(ui);

		engine->registerActor(this);
	}

	void MainUI::update(engine::context::Context* context, engine::renderer::Renderer* renderer, engine::physics::Physics* physics) {
		ui->wood = wood->getAmountOfResource();
		ui->stone = stone->getAmountOfResource();
		ui->ore = mine->getAmountOfResource();
		ui->corn = farm->getAmountOfResource();

		if (!isEscReleased && context->getKey(GLFW_KEY_ESCAPE) == GLFW_RELEASE) {
			isEscReleased = true;
		}

		if (isEscReleased && context->getKey(GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			isEscReleased = false;
			if (ui->menuState == engine::ui::MainMenu)glfwSetWindowShouldClose(context->getWindow(), true);
			else ui->menuState = engine::ui::MainMenu;
		}

		if (ui->menuState == engine::ui::MainMenu && context->getKey(GLFW_KEY_BACKSPACE) == GLFW_PRESS) {
			ui->menuState = oldState;
		}

		if (ui->menuState == engine::ui::MainMenu && oldState != engine::ui::BuildMenu && context->getKey(GLFW_KEY_SPACE) == GLFW_PRESS) {
			ui->menuState = engine::ui::ResourceMenu;
			restartGame = true;
		}

		if (ui->menuState == engine::ui::MainMenu && context->getKey(GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
			ui->isQuestLogActive = !ui->isQuestLogActive;
			ui->menuState = oldState;
		}

	}

	void MainUI::getResources(ConstructiveBuildings *b) {
		wood = b->wood;
		stone = b->stone;
		mine = b->mine;
		farm = b->farm;
	}

	void MainUI::setState(engine::ui::MenuState newState) {
		ui->menuState = newState;
		oldState = newState;
		ui->neededWood = values.woodValue;
		ui->neededStone = values.stoneValue;
		ui->neededOre = values.oreValue;
		ui->neededCorn = values.foodValue;
	}

}