#include "mainCamera.h"

#include <engine/assets/text.h>

namespace blockGame {

	MainCamera::MainCamera(engine::Engine* engine) {
		// camera
		camera = new engine::renderer::Camera(glm::vec3(0.0f, 8.0f, 8.0f));
		camera->cameraMode = engine::renderer::Camera_MODE::THIRDPERSON;
		lastX = engine->context->getWidth() / 2.0f;
		lastY = engine->context->getHeight() / 2.0f;
		firstMouse = true;
		
		engine->renderer->registerCamera(camera);

		engine->registerActor(this);
	}

	void MainCamera::update(engine::context::Context* context, engine::renderer::Renderer* renderer, engine::physics::Physics* physics) {
		if (camera->cameraMode == engine::renderer::Camera_MODE::FREEFLIGHT) {
			if (context->getKey(GLFW_KEY_W) == GLFW_PRESS)
				camera->ProcessKeyboard(engine::renderer::FORWARD, context->getDeltaTime() * 5);
			if (context->getKey(GLFW_KEY_S) == GLFW_PRESS)
				camera->ProcessKeyboard(engine::renderer::BACKWARD, context->getDeltaTime() * 5);
			if (context->getKey(GLFW_KEY_A) == GLFW_PRESS)
				camera->ProcessKeyboard(engine::renderer::LEFT, context->getDeltaTime() * 5);
			if (context->getKey(GLFW_KEY_D) == GLFW_PRESS)
				camera->ProcessKeyboard(engine::renderer::RIGHT, context->getDeltaTime() * 5);
		}
	}


	// glfw: whenever the mouse scroll wheel scrolls, this callback is called
	// ----------------------------------------------------------------------
	void MainCamera::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		//camera->ProcessMouseScroll(yoffset);
	}

}