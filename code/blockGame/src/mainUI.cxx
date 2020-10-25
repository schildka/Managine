#include "mainUI.h"

#include <engine/assets/text.h>

namespace blockGame {

	MainUI::MainUI(engine::Engine* engine) {
		ui = new engine::ui::UI(engine->context->getWindow(), engine->context->getDeltaTime(), engine::ui::ScoreMenu, "Turn all boxes to blue!");

		std::filesystem::path paths[11] = { "icons/wood.png", "icons/stone.png", "icons/ore.png", "icons/corn.png", "icons/esc.png", "icons/space.png", "icons/back.png", "icons/alt.png", "icons/mouseKey.png", "icons/mouse.png", "icons/wasd.png" };
		ui->loadIcons(paths);

		engine->renderer->registerUI(ui);

		engine->registerActor(this);
	}

	void MainUI::update(engine::context::Context* context, engine::renderer::Renderer* renderer, engine::physics::Physics* physics) {
		
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
			ui->menuState = engine::ui::ScoreMenu;
			restartGame = true;
		}

		if (ui->menuState == engine::ui::MainMenu && context->getKey(GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
			ui->isQuestLogActive = !ui->isQuestLogActive;
			ui->menuState = oldState;
		}

		if (once) {
			ui->startTime = context->getCurrentFrameTime();
			once = false;
		}

	}

	void MainUI::handleTime() {
		if (getUI()->bestTime > getUI()->actualTime || getUI()->bestTime == 0.0) {
			getUI()->bestTime = getUI()->actualTime;
		}
		ui->menuState = engine::ui::ResultMenu;
		oldState = engine::ui::ResultMenu;
	}

	void MainUI::setState(engine::ui::MenuState newState) {
		ui->menuState = newState;
		oldState = newState;
	}

}