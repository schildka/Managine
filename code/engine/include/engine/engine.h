// GameLab 3 Tutorial 2019/20
// Andreas Knote <andreas.knote@uni-wuerzburg.de>

#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "actor.h"

#include <engine/physics/physics.h>


namespace engine {
	class Engine {

	public:
		Engine();

		virtual ~Engine() {
			delete context;
			delete renderer;
			delete physics;
		};

		context::Input &getInput() {
			return static_cast<context::Input &>(*context);
		}

		void registerActor(Actor* actor);

		void run();

		virtual void afterSystemStartup() {};

	public:
		context::Context* context;
		engine::renderer::Renderer* renderer;
		engine::physics::Physics* physics;

	private:
		void engineUpdate();

		std::set<Actor*> actors;
	};
}
