#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include <engine/renderer/transform.h>

namespace engine::renderer {

	/** Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods */
	enum Camera_Movement {
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT
	};

	enum Camera_MODE {
		FREEFLIGHT,
		THIRDPERSON
	};

	// Default camera values
	const float YAW = 90.0f;
	const float PITCH = 0.0f;
	const float SPEED = 2.5f;
	const float SENSITIVITY = 0.1f;
	const float ZOOM = 45.0f;
	const float NEARCLIP = 0.1f;
	const float FARCLIP = 1000.0f;


	/**
	* An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
	* @param cameraMode Selects camera mode e.g. third person camera mode
	*/
	class Camera
	{
	public:
		// Camera Attributes
		glm::vec3 Position;
		glm::vec3 Target;
		glm::vec3 Front;
		glm::vec3 Up;
		glm::vec3 Right;
		glm::vec3 WorldUp;
		glm::vec4 clipPlane = glm::vec4(0, -1, 0, 10000);
		// Euler Angles
		float Yaw;
		float Pitch;
		// Camera options
		float MovementSpeed;
		float MouseSensitivity;
		float Zoom;
		float Near;
		float Far;
		Camera_MODE cameraMode = FREEFLIGHT;

		// Constructor with vectors
		Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH, float nearClip = NEARCLIP, float farClip = FARCLIP);
		// Constructor with scalar values
		Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

		// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
		glm::mat4 GetViewMatrix();
		glm::mat4 GetNegativeViewMatrix();

		/** Changes the camera position and pitch to render reflections */
		void toggleReflectionTransform();

		/** Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems) */
		void ProcessKeyboard(Camera_Movement direction, float deltaTime);

		/** Processes input received from a mouse input system. Expects the offset value in both the x and y direction. */
		void ProcessMouseMovement(glm::vec3 parentPosition, glm::vec3 parentFront, float xoffset, float yoffset, GLboolean constrainPitch = true);

		/** Calculates the front vector from the Camera's (updated) Euler Angles */
		void updateCameraVectors();
	};
}
#endif