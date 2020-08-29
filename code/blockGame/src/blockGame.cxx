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
		mainUI = new MainUI(this);
		camera = new MainCamera(this);
		world = new World(this);
		character = new MainCharacter(this, camera);

		character->setBlocks(world->getBlocks());
	}

}