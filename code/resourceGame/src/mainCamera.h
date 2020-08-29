#pragma once

#include <engine/engine.h>

#include <engine/renderer/camera.h>

namespace resourceGame {

	class MainCamera : public engine::Actor {
	public:
		MainCamera() : Actor() {};
		MainCamera(engine::Engine* engine);

		~MainCamera(){
			delete camera;
		};

		void update(engine::context::Context* context, engine::renderer::Renderer* renderer, engine::physics::Physics* physics) override;

		engine::renderer::Camera* getCamera() {
			return camera;
		};

		//void mouse_callback(GLFWwindow* window, double xpos, double ypos);
		void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	private:
		engine::renderer::Camera* camera;
		
		float lastX;
		float lastY;
		bool firstMouse;
	};
}