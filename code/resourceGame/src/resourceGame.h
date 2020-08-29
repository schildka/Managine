#pragma once

#include <engine/engine.h>

#include "mainCamera.h"
#include "mainCharacter.h"
#include "world.h"
#include "resources.h"
#include "collectables.h"
#include "mainUI.h"

namespace resourceGame {
	
	class ResourceGame : public engine::Engine {
	public:
		ResourceGame();
		~ResourceGame();

		void afterSystemStartup();

	private:
		MainCamera* mainCamera;
		World* world;
		Resources* resources;
		Collectables* collectables;
		ConstructiveBuildings* buildings;
		MainCharacter* mainCharacter;
		MainUI* mainUI;
	};
}