#include <engine/engine.h>

#include <iostream>

namespace engine {

	Engine::Engine() {
		context = new context::Context();
		renderer = new engine::renderer::Renderer(context);
		physics = new engine::physics::Physics();
	}

	void Engine::registerActor(Actor* actor) {
		actors.insert(actor);
	}

    void Engine::run() {
        // Start up the systems
		context->initialize();
		renderer->startup();
		
        // Start up the game
		afterSystemStartup();

		// Update Loop
		while (!context->shouldWindowClose()) {
			context->beginFrame();

			engineUpdate();

			context->endFrame();
		}

        // Shut down the game
        //before_shutdown.fire(this);

        // Shut down the systems
        //shutdown.fire(this);
    }

    void Engine::engineUpdate() {
        //before_update.fire(this);

        //early_update.fire(this);


        //update.fire(this);
		for each (Actor* actor in actors)
		{
			actor->update(context, renderer, physics);
		}


        //late_update.fire(this);
		renderer->lateUpdate();
		physics->lateUpdate();

        //after_update.fire(this);
    }
}
