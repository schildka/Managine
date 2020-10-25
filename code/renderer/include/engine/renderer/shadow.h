#pragma once

#include <engine/assets/texture.h>

#include <engine/renderer/shader.h>

#include <engine/renderer/lightTypes.h>

#define NUM_CASCADES 3
#define NUM_FRUSTUM_CORNERS 8

namespace engine::renderer {

	struct OrthoProjection
	{
		float l;        // left
		float r;        // right
		float b;        // bottom
		float t;        // top
		float n;        // z near
		float f;        // z far
	};
	/**
	* Shadow Calculating different shadow maps for spot, point and directional lights.
	*/
	class Shadow {

	public:

		Shadow(int width, int height);

		void createSpotDepthMapShader();
		void createSpotDepthMap(unsigned int i);
		void createPointDepthMapShader();
		void createPointDepthMap(unsigned int i);
		void createDirectionalDepthMap();

		GLuint CreateFrameBuffer(bool isDirectional);

		void BindSpotDepthBuffer(unsigned int i);
		void BindPointDepthBuffer(unsigned int i);
		void BindDirectionalDepthBuffer(unsigned int i);

		void UnbindBuffer(int width, int height);
		
		void Clear();

		void renderSpot(glm::vec3 lightPos, glm::vec3 lightDir, unsigned int i);
		void renderPoint(glm::vec3 lightPos);
		void renderDirectional(glm::mat4 cameraView, glm::mat4 lightView, unsigned int i);

		void CalculateOrthoProjections(glm::mat4 cameraView, glm::mat4 lightView);

		ShaderProgram * spotDepthMapShader = nullptr;
		ShaderProgram * pointDepthMapShader = nullptr;
		ShaderProgram * directionalDepthMapShader = nullptr;

		std::vector<engine::assets::Texture> spotDepthMapTexture;
		std::vector<engine::assets::Texture> pointDepthMapTexture;
		engine::assets::Texture directionalDepthMapTexture[3];	// Cascade

		GLuint spotDepthMapFBO;
		GLuint pointDepthMapFBO;
		GLuint directionalDepthMapFBO;


		const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
		float windowWidth, windowheight;

		glm::mat4 spotLightProjection;
		glm::mat4 shadowProj;
		std::vector<glm::mat4> shadowTransforms;

		glm::mat4 lightSpaceMatrix[NUM_CASCADES];
		std::vector<glm::mat4> spotLightSpaceMatrix;

		float nearPlane = 1.0f;
		float farPlane = 25.0f;

		float cascadePlanes[NUM_CASCADES + 1] = { 0.1f, 50.0f, 150.0f, 300.0f };
		float cascadeClipPlanes[NUM_CASCADES];
		OrthoProjection shadowOrthoProjections[NUM_CASCADES];
	};
}