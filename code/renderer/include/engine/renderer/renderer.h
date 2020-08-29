// GameLab 3 Tutorial 2019/20
// Andreas Knote <andreas.knote@uni-wuerzburg.de>

#pragma once


#include <engine/ui/ui.h>

#include <engine/renderer/renderable.h>
#include <engine/renderer/water.h>
#include <engine/renderer/camera.h>
#include <engine/renderer/lights.h>

#include <set>

namespace engine::renderer {

    class Renderer {

    public:

		Renderer() {};
		Renderer(engine::context::Context* context);
        Renderer(Camera* camera, engine::context::Context* context);


		void registerSharedUniformsRenderable(Renderable* renderable);
		void registerRenderable(Renderable* renderable);
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
        
		//Engine *engine
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
