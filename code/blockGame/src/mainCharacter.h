#pragma once

#include <engine/engine.h>

#include <engine/assets/sound.h>

#include <engine/renderer/skeletalMesh.h>
#include <engine/renderer/worldObject.h>

#include "mainCamera.h"
#include "mainUI.h"

namespace blockGame {

	class MainCharacter : public engine::Actor {
	public:
		MainCharacter() : Actor() {};
		MainCharacter(engine::Engine* engine, MainCamera* camera);

		~MainCharacter() {
			soundEngine->drop();
			delete skeletalMesh;
		};

		void update(engine::context::Context* context, engine::renderer::Renderer* renderer, engine::physics::Physics* physics) override;

		void initializeSounds();

		void initializeQuests();
		void handleQuests();

		unsigned int handleMovement(engine::context::Context* context, glm::vec3 position);

		void handleBlocks(rp3d::Vector3 position);

		void handleMouse(glm::vec2 offset);

		void handleUI(engine::context::Context* context, engine::renderer::Renderer* renderer, engine::physics::Physics* physics);

		void setUI(MainUI* ui) { mainUI = ui; };

		void setBlocks(std::vector<engine::renderer::WorldObject> *b) { blocks = b; };

		void transformBlock(int y);

	private:
		engine::renderer::SkeletalMesh* skeletalMesh;
		glm::vec3* position;
		float movementSpeed = 2.5f;
		unsigned int animationIndex = 3;

		MainCamera *mainCamera;
		MainUI * mainUI;

		std::map<unsigned int, Quest> quests;
		std::string restartQuestText = "Turn all boxes to blue!";
		unsigned int quest = 0;

		std::vector<engine::renderer::WorldObject> *blocks;
		engine::renderer::WorldObject* currentBlock = nullptr;
		engine::renderer::WorldObject* barrierBlock = nullptr;
		unsigned int count = 5;

		// Sounds to play
		irrklang::ISoundEngine* soundEngine;
		irrklang::ISound *stepTrack = nullptr;
		engine::assets::Sound music;
		engine::assets::Sound error;
		engine::assets::Sound right;
		engine::assets::Sound spawn;
		engine::assets::Sound lose;
	};
}