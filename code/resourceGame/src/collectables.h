#pragma once

#include <engine/engine.h>

#include <engine/renderer/worldObject.h>

namespace resourceGame {

	class Collectables : public engine::Actor {
	public:
		Collectables() : Actor() {};
		Collectables(engine::Engine* engine);
		~Collectables();

		void update(engine::context::Context* context, engine::renderer::Renderer* renderer, engine::physics::Physics* physics) override;

		bool isCollectableInside(rp3d::RigidBody* body, engine::renderer::Renderer* renderer);

		void reset(engine::renderer::Renderer* renderer);

	private:
		std::set<engine::renderer::WorldObject*> wood;
		std::set<engine::renderer::WorldObject*> reStartwood;
	};
}