#pragma once

#include <engine/engine.h>

#include <engine/ui/ui.h>

#include "ConstructiveBuildings.h"

namespace resourceGame {

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

		void getResources(ConstructiveBuildings *b);

		void setState(engine::ui::MenuState newState);

		engine::ui::UI* getUI() { return ui; };

	public:
		bool restartGame = false;

		BuildValues values;

	private:
		engine::ui::UI* ui;
		
		engine::renderer::ResourceCollector* stone;
		engine::renderer::ResourceCollector* mine;
		engine::renderer::ResourceCollector* wood;
		engine::renderer::ResourceCollector* farm;

		engine::ui::MenuState oldState = engine::ui::ResourceMenu;
		bool isEscReleased = true;
	};
}