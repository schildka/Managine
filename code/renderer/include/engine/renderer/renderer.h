#pragma once


#include <engine/ui/ui.h>

#include <engine/renderer/renderable.h>
#include <engine/renderer/water.h>
#include <engine/renderer/camera.h>
#include <engine/renderer/lights.h>

#include <set>

namespace engine::renderer {
	/**
	* Renderer renders all renderables to screen.
	* @param sharedUniform otional shared uniform to draw.
	* @param renderables Set of renderables to draw.
	* @param waterRenderables Set of water objects to draw.
	* @param ui optional graphical user interface to draw.
	*/
    class Renderer {

    public:

		Renderer() {};
		Renderer(engine::context::Context* context);
        Renderer(Camera* camera, engine::context::Context* context);


		void registerSharedUniformsRenderable(Renderable* renderable);
		/** Register renderable which should be drawn */
		void registerRenderable(Renderable* renderable);
		/** Deregister renderable which should not be drawn */
		void deRegisterRenderable(Renderable* renderable);
		void registerCamera(Camera* camera);
		void registerLights(Lights* light);
		void registerWater(Water* water);
		void registerUI(engine::ui::UI* ui);

		Camera* getCamera() {
			return camera;
		}

		Lights* getLights() {
			return lights;
		}

		void lateUpdate();

		void startup();

    private:
        void shutdown();

    private:
		Renderable * sharedUniforms = nullptr;

		std::set<Renderable*> renderables;
		std::set<Renderable*> waterRenderables;

		engine::ui::UI* ui = nullptr;

		Water* water = nullptr;

		Camera* camera;
		
		Lights* lights;
		Shadow* shadow;

		engine::context::Context* context;
        
    };
}
