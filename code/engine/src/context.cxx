// GameLab 3 Tutorial 2019/20
// Andreas Knote <andreas.knote@uni-wuerzburg.de>

#include <engine/context/context.h>

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <cassert>
#include <iostream>

namespace engine::context {

    Context::Context() {
        // Initialize GLFW library
        if (!glfwInit())
            throw std::runtime_error("Failed to initialize GLFW!");

        // Make sure we get an OpenGL 4.6 context
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

        // Window Size
        width = 1920;
        height = 1080;
		
        glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

        // Create a windowed mode window and its OpenGL context
        window = glfwCreateWindow(width, height, "Window", glfwGetPrimaryMonitor(), NULL);	// For FullScreen use -> glfwGetPrimaryMonitor()
        if (!window) {
            glfwTerminate();
            throw std::runtime_error("Failed to initialize GLFW!");
        }

        // Make the window's context current
        glfwMakeContextCurrent(window);

		// Capture Mouse
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        // enable VSync
        glfwSwapInterval(1);


		lastX = width / 2.0f;
		lastY = height / 2.0f;

		// ToDo wenn das alles wieder hier drinne ist, kann man das wieder wie vorher machen ^^
		glfwSetWindowUserPointer(window, this);
		registerMouseCallback([](GLFWwindow* window, double xpos, double ypos) {static_cast<Context*>(glfwGetWindowUserPointer(window))->mouse_callback(window, xpos, ypos); });
		registerFramebufferCallback([](GLFWwindow* window, int width, int height) {static_cast<Context*>(glfwGetWindowUserPointer(window))->framebuffer_size_callback(window, width, height); });
    }

    Context::~Context() {
        glfwTerminate();
    }

	void Context::registerScrollCallback(void (*f)(GLFWwindow*, double, double)) {
		glfwSetScrollCallback(window, (*f));
	}

	void Context::registerMouseCallback(void(*f)(GLFWwindow*, double, double)) {
		glfwSetCursorPosCallback(window, (*f));
	}

	void Context::registerFramebufferCallback(void(*f)(GLFWwindow*, int, int)) {
		glfwSetFramebufferSizeCallback(window, (*f));
	}
	
    void Context::initialize() {
        // Frame time  (initialize to sane values)
        glfwSetTime(1.0 / 60);

		startFrameTime = glfwGetTime();
    }

    void Context::beginFrame() {
        // Update the window size
        int newWidth, newHeight;
        glfwGetFramebufferSize(window, &newWidth, &newHeight);
        if (newWidth != width || newHeight != height) {
            height = newHeight;
            width = newWidth;
			//glViewport(0, 0, width, height);
        }

		currentFrameTime = glfwGetTime();
		deltaTime = currentFrameTime - lastFrameTime;
		lastFrameTime = currentFrameTime;
    }

    void Context::endFrame() {
        // Poll for and process events
        glfwPollEvents();

        // Swap front and back buffers
        glfwSwapBuffers(window);
    }

	void Context::run(std::function<void(Context *)> updateCallback) {

		// Update Loop
		while (!shouldWindowClose()) {
			beginFrame();

			updateCallback(this);

			endFrame();
		}
	}

    unsigned int Context::getKey(unsigned int keyCode) {
        return glfwGetKey(window, keyCode);
    }

	unsigned int Context::getMouseKey(unsigned int keyCode) {
		return glfwGetMouseButton(window, keyCode);
	}

    bool Context::shouldWindowClose() {
        return glfwWindowShouldClose(window);
    }

	glm::vec2 Context::getMouseOffset() {
		return mouseOffset;
	}

	bool Context::IsMouseMoving() {
		return isMouseMoving;
	}

	void Context::setMouseIsMoving(bool b) {
		isMouseMoving = b;
	}

	// glfw: whenever the mouse moves, this callback is called
	// -------------------------------------------------------
	void Context::mouse_callback(GLFWwindow* window, double xpos, double ypos)
	{
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		mouseOffset.x = lastX - xpos;
		mouseOffset.y = lastY - ypos;

		lastX = xpos;
		lastY = ypos;

		isMouseMoving = true;
	}

	// glfw: whenever the window size changed (by OS or user resize) this callback function executes
	// ---------------------------------------------------------------------------------------------
	void Context::framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		// make sure the viewport matches the new window dimensions; note that width and 
		// height will be significantly larger than specified on retina displays.
		glViewport(0, 0, width, height);
		this->width = width;
		this->height = height;
	}
}
