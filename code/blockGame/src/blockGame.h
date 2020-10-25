#pragma once

#include <engine/engine.h>

#include "mainUI.h"
#include "mainCamera.h"
#include "world.h"
#include "mainCharacter.h"

namespace blockGame {

	class BlockGame : public engine::Engine {
	public:
		BlockGame();
		~BlockGame();

		void afterSystemStartup();

	private:
		MainUI * mainUI = nullptr;
		MainCamera* camera = nullptr;
		World* world = nullptr;
		MainCharacter* character = nullptr;
	};
}