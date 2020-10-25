#include "blockGame.h"

namespace blockGame {
	
	BlockGame::BlockGame() : Engine() {

	}

	BlockGame::~BlockGame() {
		delete mainUI;
		delete camera;
		delete world;
		delete character;
	}

	void BlockGame::afterSystemStartup() {
		camera = new MainCamera(this);
		world = new World(this);
		character = new MainCharacter(this, camera);
		mainUI = new MainUI(this);

		character->setBlocks(world->getBlocks());
		character->setUI(mainUI);
	}

}