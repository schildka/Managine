#include "collectables.h"

#include <engine/assets/text.h>
#include <engine/assets/model.h>

namespace resourceGame {

	Collectables::Collectables(engine::Engine* engine) {
		auto fragmentSource = engine::assets::Text("shader/object/fragmentShader.fs");
		auto vertexSource = engine::assets::Text("shader/object/vertexShader.vs");

		engine::assets::Model woodModel("models/Ressources/Wood.fbx");
		
		engine::renderer::WorldObject* wO = new engine::renderer::WorldObject();
		wO->initialize(&woodModel, glm::vec3(0, -1, 25), glm::vec3(0, 0, glm::radians(145.0f)), glm::vec3(0.5, 0.5, 1.588), vertexSource, fragmentSource);
		wO->staticPosition = rp3d::Vector3(0, -1, 25);
		wO->registerAllRenderables(engine->renderer);
		wood.insert(wO);

		engine::renderer::WorldObject* wO2 = new engine::renderer::WorldObject();
		wO2->initialize(&woodModel, glm::vec3(-35, -1, 20), glm::vec3(0, 0, glm::radians(145.0f)), glm::vec3(0.5, 0.5, 1.588), vertexSource, fragmentSource);
		wO2->staticPosition = rp3d::Vector3(-35, -1, 20);
		wO2->registerAllRenderables(engine->renderer);
		wood.insert(wO2);

		engine::renderer::WorldObject* wO3 = new engine::renderer::WorldObject();
		wO3->initialize(&woodModel, glm::vec3(0, 8, -75), glm::vec3(0, 0, glm::radians(145.0f)), glm::vec3(0.5, 0.5, 1.588), vertexSource, fragmentSource);
		wO3->staticPosition = rp3d::Vector3(0, 8, -75);
		wO3->registerAllRenderables(engine->renderer);
		wood.insert(wO3);

		engine::renderer::WorldObject* wO4 = new engine::renderer::WorldObject();
		wO4->initialize(&woodModel, glm::vec3(65, -2, 115), glm::vec3(0, 0, glm::radians(145.0f)), glm::vec3(0.5, 0.5, 1.588), vertexSource, fragmentSource);
		wO4->staticPosition = rp3d::Vector3(65, -2, 115);
		wO4->registerAllRenderables(engine->renderer);
		wood.insert(wO4);

		engine::renderer::WorldObject* wO5 = new engine::renderer::WorldObject();
		wO5->initialize(&woodModel, glm::vec3(-20, -1, 170), glm::vec3(0, 0, glm::radians(145.0f)), glm::vec3(0.5, 0.5, 1.588), vertexSource, fragmentSource);
		wO5->staticPosition = rp3d::Vector3(-20, -1, 170);
		wO5->registerAllRenderables(engine->renderer);
		wood.insert(wO5);

		reStartwood = wood;
	}


	Collectables::~Collectables() {

	}

	void Collectables::update(engine::context::Context* context, engine::renderer::Renderer* renderer, engine::physics::Physics* physics) {
	}

	bool Collectables::isCollectableInside(rp3d::RigidBody* body, engine::renderer::Renderer* renderer) {
		bool isInside = false;
		engine::renderer::WorldObject* tmp;
		for each (engine::renderer::WorldObject* w in wood)
		{
			if (body->testPointInside(w->staticPosition)) {
				tmp = w;
				isInside = true;
				break;
			}
		}

		if (isInside) {
			tmp->deRegisterRenderables(renderer);
			wood.erase(tmp);
		}

		return isInside;
	}

	void Collectables::reset(engine::renderer::Renderer* renderer) {
		for each (engine::renderer::WorldObject* w in reStartwood)
		{
			if (wood.find(w) == wood.end()) {
				w->registerAllRenderables(renderer);
			}
		}
		wood = reStartwood;
	}

}