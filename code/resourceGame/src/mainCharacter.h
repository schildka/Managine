#pragma once

#include <engine/engine.h>

#include <engine/assets/sound.h>

#include <engine/renderer/skeletalMesh.h>

#include "mainCamera.h"
#include "mainUI.h"
#include "collectables.h"
#include "constructiveBuildings.h"

namespace resourceGame {

	class MainCharacter : public engine::Actor {
	public:
		MainCharacter() : Actor() {};
		MainCharacter(engine::Engine* engine, MainCamera* camera);

		~MainCharacter(){
			soundEngine->drop();
			delete skeletalMesh;
		};

		void update(engine::context::Context* context, engine::renderer::Renderer* renderer, engine::physics::Physics* physics) override;

		void initializeSounds();

		void initializeQuests();
		void handleQuests();

		unsigned int handleMovement(engine::context::Context* context);

		void handleMouse(glm::vec2 offset);

		void handleBuildings(engine::context::Context* context, engine::renderer::Renderer* renderer, engine::physics::Physics* physics);

		void handleCollectables(engine::renderer::Renderer* renderer);

		void handleUI(engine::context::Context* context, engine::renderer::Renderer* renderer, engine::physics::Physics* physics);

		void setConstructives(ConstructiveBuildings *buildings) {
			this->buildings = buildings;
		}

		void setCollectables(Collectables *collect) {
			this->collectables = collect;
		}
		
		void setTerrainCollider(rp3d::HeightFieldShape* collider) {
			terrainCollider = collider;
		}

		void setUI(MainUI *ui) {
			mainUI = ui;
		}
		
	private:
		engine::renderer::SkeletalMesh* skeletalMesh;
		glm::vec3* position;
		float movementSpeed = 15.0f;
		unsigned int animationIndex = 3;

		rp3d::HeightFieldShape* terrainCollider;

		MainCamera *mainCamera;
		MainUI *mainUI;

		Collectables* collectables;

		// Buildings to build
		ConstructiveBuildings* buildings;
		BuildValues buildValues;
		engine::renderer::ResourceCollector * collector = nullptr;
		engine::renderer::WorldObject *creationObject = nullptr;
		BuildState buildState = Idle;
		unsigned int key;
		bool isReleased = true;

		std::map<unsigned int, Quest> quests;
		std::string restartQuestTest = "Don't starve!";
		unsigned int quest = 0;

		// Sounds to play
		irrklang::ISoundEngine* soundEngine;
		irrklang::ISound *stepTrack = nullptr;
		engine::assets::Sound music;
		engine::assets::Sound craft;
		engine::assets::Sound buildError;
		engine::assets::Sound findItem;
		engine::assets::Sound waterStep;
		engine::assets::Sound spawn;
		engine::assets::Sound lose;
	};
}