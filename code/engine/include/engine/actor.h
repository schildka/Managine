#pragma once

#include <engine/context/context.h>
#include <engine/physics/physics.h>
#include <engine\renderer\renderer.h>

namespace engine {
	/**
	* Actor class.
	*/
	class Actor {
	public:

		Actor();
		virtual ~Actor();

		/**
		* Update call handled by engine.
		* @param context.
		* @param renderer.
		* @param physics.
		*/
		virtual void update(context::Context* context, renderer::Renderer* renderer, physics::Physics* physics);
	};
}