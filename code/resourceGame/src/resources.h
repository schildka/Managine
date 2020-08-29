#pragma once

#include <engine/engine.h>

#include <engine/renderer/resource.h>

namespace resourceGame {

	class Resources : public engine::Actor {
	public:
		Resources() : Actor() {};
		Resources(engine::Engine* engine);
		~Resources();

		void update(engine::context::Context* context, engine::renderer::Renderer* renderer, engine::physics::Physics* physics) override;

		engine::renderer::Resource* getStone();
		engine::renderer::Resource* getOre();
		engine::renderer::Resource* getWood();

	private:
		engine::renderer::Resource* stone;
		engine::renderer::Resource* ore;
		std::vector<engine::renderer::Resource> trees;

		glm::vec3 treePositions[12] = { { -44,-2,35 },{ -42,-2,42 },{ -47,-2,42 },{ -45,-2,47 },{ -37,-2,47 },{ -120,-4,120 },{ 75,-4,-100 },{ -150,-4,-100 },{ 155,-4,155 },{ 0,-4,195 },{ 0,-4,-180 },{ 55,-4,140 } };
		const int numberOfTrees = sizeof(treePositions) / sizeof(treePositions[0]);
	};
}