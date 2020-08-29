#pragma once

#include <engine/context/context.h>
#include <engine/physics/physics.h>
#include <engine\renderer\renderer.h>

namespace engine {

	class Actor {
	public:

		Actor();
		virtual ~Actor();

		virtual void update(context::Context* context, renderer::Renderer* renderer, physics::Physics* physics);
	};
}