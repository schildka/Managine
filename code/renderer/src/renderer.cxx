// GameLab 3 Tutorial 2019/20
// Andreas Knote <andreas.knote@uni-wuerzburg.de>

#include <engine/renderer/renderer.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cassert>
#include "error.h"

namespace engine::renderer {

	Renderer::Renderer(engine::context::Context* context) {
		this->context = context;
	}

    Renderer::Renderer(Camera* camera, engine::context::Context* context){
		this->camera = camera;
		this->context = context;
	}

	void Renderer::registerSharedUniformsRenderable(Renderable* renderable) {
		sharedUniforms = renderable;
	}

    void Renderer::registerRenderable(Renderable* renderable) {
		renderables.insert(renderable);
    }

	void Renderer::deRegisterRenderable(Renderable* renderable) {
		renderables.erase(renderable);
	}

	void Renderer::registerCamera(Camera* camera) {
		this->camera = camera;
	}

	void Renderer::registerLights(Lights* lights) {
		this->lights = lights;
	}

	void Renderer::registerWater(Water* water) {
		waterRenderables.insert(water);
		this->water = water;
	}

	void Renderer::registerUI(engine::ui::UI* ui) {
		this->ui = ui;
	}

    void Renderer::shutdown() {
        
    }

    void Renderer::startup() {

        // Load GL Extensions
        gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

        std::cout << "GPU: " << glGetString(GL_RENDERER) << std::endl
                  << " OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

        // Bind a single global vertex array
        {
            GLuint vao;
            glGenVertexArrays(1, &vao);
            glBindVertexArray(vao);
        }

        // Check for errors
        {
            const GLenum error = glGetError();
            assert(error == GL_NONE);
        }

		// configure global opengl state
		// -----------------------------
		glEnable(GL_DEPTH_TEST);

		lights = new Lights(context->getWidth(), context->getHeight());
		shadow = lights->GetShadow();

		gl::throwOnError();
    }

    void Renderer::lateUpdate() {

		// Cascade Shadow Mapping
		if (lights->calculateCascadeShadowMapping) {
			glm::mat4 lightView = glm::transpose(lights->GetDirectionalLightView());
			glm::mat4 cameraView = glm::transpose(camera->GetNegativeViewMatrix());

			shadow->CalculateOrthoProjections(cameraView, lightView);

			shadow->directionalDepthMapShader->use();

			for (unsigned int i = 0; i < NUM_CASCADES; i++)
			{
				shadow->BindDirectionalDepthBuffer(i);
				shadow->Clear();

				shadow->renderDirectional(cameraView, lightView, i);

				if (sharedUniforms != nullptr)sharedUniforms->renderShadow(shadow->directionalDepthMapShader);

				for each (Renderable* object in renderables)
				{
					object->renderShadow(shadow->directionalDepthMapShader);
				}
			}

			shadow->UnbindBuffer(context->getWidth(), context->getHeight());
		}
		

		// Point Lights Shadows
		for (unsigned int i = 0; i < lights->GetNumberOfPointLights(); i++)
		{
			shadow->BindPointDepthBuffer(i);
			shadow->Clear();

			shadow->renderPoint(lights->GetPointLight(i).position);

			if (sharedUniforms != nullptr)sharedUniforms->renderShadow(shadow->pointDepthMapShader);

			for each (Renderable* object in renderables)
			{
				object->renderShadow(shadow->pointDepthMapShader);
			}
		}

		shadow->UnbindBuffer(context->getWidth(), context->getHeight());


		// Spot Lights Shadows
		for (unsigned int i = 0; i < lights->GetNumberOfSpotLights(); i++)
		{
			shadow->BindSpotDepthBuffer(i);
			shadow->Clear();

			shadow->renderSpot(lights->GetSpotLight(i).position, lights->GetSpotLight(i).direction, i);

			if (sharedUniforms != nullptr)sharedUniforms->renderShadow(shadow->spotDepthMapShader);

			for each (Renderable* object in renderables)
			{
				object->renderShadow(shadow->spotDepthMapShader);
			}
		}

		shadow->UnbindBuffer(context->getWidth(), context->getHeight());


		if (water != nullptr) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			camera->toggleReflectionTransform();

			water->BindReflectionBuffer();
			water->Clear();

			camera->clipPlane = glm::vec4(0, 1, 0, 0);
			glEnable(GL_CLIP_DISTANCE0);

			if(sharedUniforms != nullptr)sharedUniforms->render(camera, lights, context);

			for each (Renderable* object in renderables)
			{
				object->render(camera, lights, context);
			}

			water->UnbindBuffer(context->getWidth(), context->getHeight());


			camera->toggleReflectionTransform();

			water->BindRefractionBuffer();
			water->Clear();

			camera->clipPlane = glm::vec4(0, -1, 0, 0);
			glEnable(GL_CLIP_DISTANCE0);

			if (sharedUniforms != nullptr)sharedUniforms->render(camera, lights, context);

			for each (Renderable* object in renderables)
			{
				object->render(camera, lights, context);
			}

			camera->clipPlane = glm::vec4(0, -1, 0, 10000);
			glDisable(GL_CLIP_DISTANCE0);

			water->UnbindBuffer(context->getWidth(), context->getHeight());
		}


		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (sharedUniforms != nullptr)sharedUniforms->render(camera, lights, context);

		for each (Renderable* object in renderables)
		{
			object->render(camera, lights, context);
		}


		for each (Renderable* object in waterRenderables)
		{
			object->render(camera, lights, context);
		}

		if (ui != nullptr)ui->drawGameUI(context->getDeltaTime(), context->getCurrentFrameTime()); // drawDebugUI(context->getDeltaTime());
    }
}
