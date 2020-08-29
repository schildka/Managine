#include "resources.h"

#include <engine/assets/text.h>
#include <engine/assets/model.h>

namespace resourceGame {
	
	Resources::Resources(engine::Engine* engine) {
		engine::assets::Model ourModel("models/Ressources/Rock.fbx");
		auto fragmentSource = engine::assets::Text("shader/object/fragmentShader.fs");
		auto vertexSource = engine::assets::Text("shader/object/vertexShader.vs");
		stone = new engine::renderer::Resource();

		stone->meshObject.initialize(&ourModel, glm::vec3(100, -9, 70), glm::vec3(0), glm::vec3(0.05), vertexSource, fragmentSource);
		stone->meshObject.registerAllRenderables(engine->renderer);

		stone->meshObject.initPhysics(glm::vec3(50, -4.5, 35), rp3d::Vector3(7, 10, 15), engine->physics);


		engine::assets::Model oreModel("models/Ressources/Ore.fbx");
		ore = new engine::renderer::Resource();

		ore->meshObject.initialize(&oreModel, glm::vec3(30, -4, 115), glm::vec3(glm::radians(-90.0), 0, glm::radians(90.0)), glm::vec3(30), vertexSource, fragmentSource);
		ore->meshObject.registerAllRenderables(engine->renderer);

		ore->meshObject.initPhysics(glm::vec3(15, -2, 57.5), rp3d::Vector3(5, 5, 5), engine->physics);
			

		// Tress 
		engine::assets::Model treeModel("models/Trees/Palm_Tree.obj");
		fragmentSource = engine::assets::Text("shader/object/fragmentMaterialShader.fs");

		for (unsigned int i = 0; i < numberOfTrees; i++)
		{
			trees.push_back(engine::renderer::Resource());
			float randomSize = (float)rand() / RAND_MAX;

			trees[i].meshObject.initialize(&treeModel, treePositions[i], glm::vec3(0, i, 0), glm::vec3(2.5f - randomSize, 2.5f - randomSize, 2.5f - randomSize), vertexSource, fragmentSource);
			trees[i].meshObject.registerAllRenderables(engine->renderer);
			
			trees[i].meshObject.initPhysics(treePositions[i] / 2.0f, rp3d::Vector3(1, 4, 1), engine->physics);
		}

	}

	Resources::~Resources() {

	}

	void Resources::update(engine::context::Context* context, engine::renderer::Renderer* renderer, engine::physics::Physics* physics) {

	}

	engine::renderer::Resource* Resources::getStone() {
		return stone;
	}
	engine::renderer::Resource* Resources::getOre() {
		return ore;
	}
	engine::renderer::Resource* Resources::getWood() {
		return &trees[0];
	}
}