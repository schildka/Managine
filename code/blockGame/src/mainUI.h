#pragma once

#include <engine/engine.h>

#include <engine/ui/ui.h>

namespace blockGame {

	struct Quest {
		std::string questText;
		bool done;

		Quest() {

		}

		Quest(std::string s) {
			questText = s;
			done = false;
		}
	};

	class MainUI : public engine::Actor {
	public:
		MainUI() : Actor() {};
		MainUI(engine::Engine* engine);

		~MainUI() {
			delete ui;
		}

		void update(engine::context::Context* context, engine::renderer::Renderer* renderer, engine::physics::Physics* physics) override;
		
		void handleTime();

		void setState(engine::ui::MenuState newState);

		engine::ui::UI* getUI() { return ui; };

	public:
		bool restartGame = false;

	private:
		engine::ui::UI* ui;
		
		engine::ui::MenuState oldState = engine::ui::ScoreMenu;
		bool isEscReleased = true;

		bool once = true;
	};
}