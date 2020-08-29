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
		MainUI * mainUI;
		MainCamera* camera;
		World* world;
		MainCharacter* character;
	};
}