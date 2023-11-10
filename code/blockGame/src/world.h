#pragma once

#include <engine/engine.h>

#include <engine/renderer/skyBox.h>
#include <engine/renderer/lights.h>
#include <engine/renderer/worldObject.h>

namespace blockGame {

	class World : public engine::Actor {
	public:
		World() : Actor() {};
		World(engine::Engine* engine);
		~World();

		void update(engine::context::Context* context, engine::renderer::Renderer* renderer, engine::physics::Physics* physics) override;

		std::vector<engine::renderer::WorldObject>* getBlocks() { return &blocks; };

	private:
		engine::renderer::SkyBox* skyBox;

		engine::renderer::WorldObject* start;
		engine::assets::Material material;

		// vector data structure.. mabey struct? and of course position array?
		std::vector<engine::renderer::WorldObject> blocks;
		glm::vec3 pos[51] = { glm::vec3(8,0,0), glm::vec3(8,0,4), glm::vec3(8,0,-4), glm::vec3(12,0,4), glm::vec3(12,0,-4), glm::vec3(16,0,4), glm::vec3(16,0,-4), glm::vec3(16,0,12), glm::vec3(16,0,8), glm::vec3(16,0,0), glm::vec3(16,0, -8), glm::vec3(16,0,-12), glm::vec3(20,0,4), glm::vec3(20,0,-4), glm::vec3(20,0,12), glm::vec3(20,0,-12), glm::vec3(24,0,4), glm::vec3(24,0,-4), glm::vec3(24,0,12), glm::vec3(24,0,8), glm::vec3(24,0,0), glm::vec3(24,0, -8), glm::vec3(24,0,-12), glm::vec3(28,0,4), glm::vec3(28,0,-4), glm::vec3(32,0,4), glm::vec3(32,0,-4), glm::vec3(32,0,0), glm::vec3(-4,0,8), glm::vec3(-4,0,12), glm::vec3(0,0,12), glm::vec3(4,0,12), glm::vec3(4,0,8), glm::vec3(-4,0,-8), glm::vec3(-4,0,-12), glm::vec3(-4,0,-16), glm::vec3(-4,0,-20), glm::vec3(-8,0,-20), glm::vec3(-12,0,-20), glm::vec3(-12,0,-16), glm::vec3(-12,0,-12), glm::vec3(-8,0,-12), glm::vec3(0,0,-12), glm::vec3(4,0,-12), glm::vec3(4,0,-8), glm::vec3(-8,0,0), glm::vec3(-12,0,0), glm::vec3(-12,0,4), glm::vec3(-12,0,8), glm::vec3(-12,0,12), glm::vec3(-8,0,12) };
		glm::vec3 rot[7] = { glm::vec3(glm::radians(0.0f),0,0), glm::vec3(0,0,0), glm::vec3(glm::radians(0.0f),0,0), glm::vec3(glm::radians(90.0f),0,0), glm::vec3(glm::radians(90.0f),0,0), glm::vec3(0,glm::radians(90.0f),0), glm::vec3(0,glm::radians(90.0f),0) };
		float sca[7] = { 1.764f, 2.0f, 1.764f, 1.764f, 1.764f, 1.764f, 1.764f };

	};
}