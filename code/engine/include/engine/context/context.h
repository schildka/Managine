#pragma once

#include "input.h"

#include <chrono>
#include <functional>
#include <vector>

#include <glm/glm.hpp>

// Forward definition
struct GLFWwindow;
namespace engine::context {
	/**
	* GLFW context.
	* @param window Current context window.
	*/
    class Context : public Input {
    public:
        Context();

        ~Context();

		/** returns indicator of given keyboard key */
        unsigned int getKey(unsigned int glfwKey) override;
		/** returns indicator of given mouse key */
		unsigned int getMouseKey(unsigned int glfwKey) override;

		void registerScrollCallback(void (*f)(GLFWwindow*, double, double));
		void registerMouseCallback(void(*f)(GLFWwindow*, double, double));
		void registerFramebufferCallback(void(*f)(GLFWwindow*, int, int));

        bool shouldWindowClose();

        GLFWwindow * getWindow() const { return window; }

        double getDeltaTime() const { return deltaTime; }

		double getCurrentFrameTime() const { return currentFrameTime; }

		double getLastFrameTime() const { return lastFrameTime; }

		double getStartFrameTime() const { return startFrameTime; }

        int getWidth() const { return width; }

        int getHeight() const { return height; }

        float getAspect() const { return width / (float) height; }

        void run(std::function<void(Context *)> updateCallback);
		
        void initialize();

        void beginFrame();

        void endFrame();


		glm::vec2 getMouseOffset();
		bool IsMouseMoving();
		void setMouseIsMoving(bool b);

		void mouse_callback(GLFWwindow* window, double xpos, double ypos);
		void framebuffer_size_callback(GLFWwindow* window, int width, int height);

    private:
        GLFWwindow *window;
		double startFrameTime;
		double currentFrameTime;
        double lastFrameTime = 0;
        double deltaTime = 0;
        int width = 0;
        int height = 0;

		glm::vec2 mouseOffset;
		float lastX;
		float lastY;
		bool firstMouse = true;
		bool isMouseMoving = false;
    };
}