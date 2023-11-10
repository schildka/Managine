#include "resourceGame.h"

namespace resourceGame {

	ResourceGame::ResourceGame() : Engine() {

	}

	ResourceGame::~ResourceGame() {
		delete mainCamera;
		delete world;
		delete resources;
		delete collectables;
		delete buildings;
		delete mainCharacter;
		delete mainUI;
	}

	void ResourceGame::afterSystemStartup() {
		mainCamera = new MainCamera(this);
		world = new World(this);
		resources = new Resources(this);
		collectables = new Collectables(this);
		buildings = new ConstructiveBuildings(this);
		mainCharacter = new MainCharacter(this, mainCamera);
		mainUI = new MainUI(this);

		buildings->getResources(resources);

		mainCharacter->setCollectables(collectables);
		mainCharacter->setConstructives(buildings);
		mainCharacter->setTerrainCollider(world->getTerrain()->heightFieldShape);
		mainCharacter->setUI(mainUI);
		
		mainUI->getResources(buildings);
	}

}