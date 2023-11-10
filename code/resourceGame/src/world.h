#pragma once

#include <engine/engine.h>

#include <engine/renderer/skyBox.h>
#include <engine/renderer/terrain.h>
#include <engine/renderer/water.h>
#include <engine/renderer/lights.h>
#include <engine/renderer/worldObject.h>

namespace resourceGame {
	
	class World : public engine::Actor {
	public:
		World() : Actor() {};
		World(engine::Engine* engine);
		~World();

		void update(engine::context::Context* context, engine::renderer::Renderer* renderer, engine::physics::Physics* physics) override;

		engine::renderer::Terrain* getTerrain() {
			return terrain;
		}

	private:
		engine::renderer::SkyBox* skyBox;
		engine::renderer::Terrain* terrain;
		engine::renderer::Water* water;

		engine::renderer::WorldObject signs[5];
	};
}