#include <engine/renderer/shadow.h>

#include <engine/assets/text.h>

#include "error.h"

#define _USE_MATH_DEFINES // for C++
#include <math.h>

namespace engine::renderer {

	Shadow::Shadow(int width, int height) {

		windowWidth = width;
		windowheight = height;

		createDirectionalDepthMap();
		directionalDepthMapFBO = CreateFrameBuffer(true);
		UnbindBuffer(width, height);

		createSpotDepthMapShader();
		spotDepthMapFBO = CreateFrameBuffer(false);
		UnbindBuffer(width, height);
		spotLightProjection = glm::perspective(glm::radians(120.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, nearPlane, farPlane);

		createPointDepthMapShader();
		pointDepthMapFBO = CreateFrameBuffer(false);
		UnbindBuffer(width, height);
		shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, nearPlane, farPlane);
	}

	void Shadow::createSpotDepthMapShader() {
		auto fragmentSource = engine::assets::Text("shader/shadow/fragmentDirectionalShadowShader.fs");
		auto vertexSource = engine::assets::Text("shader/shadow/vertexDirectionalShadowShader.vs");
		spotDepthMapShader = new engine::renderer::ShaderProgram(
			engine::renderer::Shader(vertexSource.get(), GL_VERTEX_SHADER),
			engine::renderer::Shader(fragmentSource.get(), GL_FRAGMENT_SHADER));
	}

	void Shadow::createSpotDepthMap(unsigned int i) {
		spotDepthMapTexture.push_back(engine::assets::Texture(SHADOW_WIDTH, SHADOW_HEIGHT, true));
		spotLightSpaceMatrix.resize(i + 1);
		glBindFramebuffer(GL_FRAMEBUFFER, spotDepthMapFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, spotDepthMapTexture[i].id(), 0);
		UnbindBuffer(windowWidth, windowheight);
	}

	void Shadow::createPointDepthMapShader() {
		auto geometrySource = engine::assets::Text("shader/shadow/geometryShadowShader.gs");
		auto fragmentSource = engine::assets::Text("shader/shadow/fragmentShadowShader.fs");
		auto vertexSource = engine::assets::Text("shader/shadow/vertexShadowShader.vs");
		pointDepthMapShader = new engine::renderer::ShaderProgram(
			engine::renderer::Shader(vertexSource.get(), GL_VERTEX_SHADER),
			engine::renderer::Shader(fragmentSource.get(), GL_FRAGMENT_SHADER),
			&engine::renderer::Shader(geometrySource.get(), GL_GEOMETRY_SHADER));

		pointDepthMapShader->use();
		pointDepthMapShader->setFloat("farPlane", farPlane);
	}

	void Shadow::createPointDepthMap(unsigned int i) {
		pointDepthMapTexture.push_back(engine::assets::Texture(SHADOW_WIDTH, SHADOW_HEIGHT, false));
		glBindFramebuffer(GL_FRAMEBUFFER, pointDepthMapFBO);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, pointDepthMapTexture[i].id(), 0);
		UnbindBuffer(windowWidth, windowheight);
	}

	void Shadow::createDirectionalDepthMap() {
		auto fragmentSource = engine::assets::Text("shader/shadow/fragmentDirectionalShadowShader.fs");
		auto vertexSource = engine::assets::Text("shader/shadow/vertexDirectionalShadowShader.vs");
		directionalDepthMapShader = new engine::renderer::ShaderProgram(
			engine::renderer::Shader(vertexSource.get(), GL_VERTEX_SHADER),
			engine::renderer::Shader(fragmentSource.get(), GL_FRAGMENT_SHADER));

		// create depth cubemap texture
		for (unsigned int i = 0; i < 3; i++)
		{
			directionalDepthMapTexture[i] = engine::assets::Texture(SHADOW_WIDTH, SHADOW_HEIGHT, true);
		}


		glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)windowWidth / (float)windowheight, cascadePlanes[0], cascadePlanes[3]);

		for (unsigned int i = 0; i < NUM_CASCADES; i++) {

			glm::vec4 view = glm::vec4(0.0f, 0.0f, cascadePlanes[i + 1], 1.0f);

			cascadeClipPlanes[i] = (proj * view).z * (-1);
		}
	}

	GLuint Shadow::CreateFrameBuffer(bool isDirectional) {
		GLuint frameBuffer;
		glGenFramebuffers(1, &frameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
		if (isDirectional) {
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, directionalDepthMapTexture[0].id(), 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, directionalDepthMapTexture[1].id(), 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, directionalDepthMapTexture[2].id(), 0);	// ToDo 1,2
		}

		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		return frameBuffer;
	}

	void Shadow::BindSpotDepthBuffer(unsigned int i) {
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, spotDepthMapFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, spotDepthMapTexture[i].id(), 0);
	}

	void Shadow::BindPointDepthBuffer(unsigned int i) {
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, pointDepthMapFBO);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, pointDepthMapTexture[i].id(), 0);
	}

	void Shadow::BindDirectionalDepthBuffer(unsigned int i) {
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, directionalDepthMapFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, directionalDepthMapTexture[i].id(), 0);
	}

	void Shadow::Clear() {
		glClear(GL_DEPTH_BUFFER_BIT);
	}

	void Shadow::UnbindBuffer(int width, int height) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, width, height);
	}

	void Shadow::renderSpot(glm::vec3 lightPos, glm::vec3 lightDir, unsigned int i) {

		glm::mat4 lightView = glm::lookAt(lightPos, lightPos - -lightDir, glm::vec3(0.0, 1.0, 0.0));
		spotLightSpaceMatrix[i] = spotLightProjection * lightView;
		
		spotDepthMapShader->use();
		spotDepthMapShader->setMat4("lightSpaceMatrix", spotLightSpaceMatrix[i]);
	}

	void Shadow::renderPoint(glm::vec3 lightPos) {
		pointDepthMapShader->use();

		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

		for (unsigned int i = 0; i < 6; ++i)
			pointDepthMapShader->setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);

		pointDepthMapShader->setVec3("lightPos", lightPos);
	}

	void Shadow::renderDirectional(glm::mat4 cameraView, glm::mat4 lightView, unsigned int i) {
		
		//glm::mat4 lightProjection = glm::transpose(glm::ortho(shadowOrthoProjections[i].l, shadowOrthoProjections[i].r, shadowOrthoProjections[i].b, shadowOrthoProjections[i].t, shadowOrthoProjections[i].n, shadowOrthoProjections[i].f));
		glm::mat4 lightProjection2;

		float l = shadowOrthoProjections[i].l;
		float r = shadowOrthoProjections[i].r;
		float b = shadowOrthoProjections[i].b;
		float t = shadowOrthoProjections[i].t;
		float n = shadowOrthoProjections[i].n;
		float f = shadowOrthoProjections[i].f;

		lightProjection2[0][0] = 2.0f / (r - l); lightProjection2[0][1] = 0.0f;         lightProjection2[0][2] = 0.0f;         lightProjection2[0][3] = -(r + l) / (r - l);
		lightProjection2[1][0] = 0.0f;         lightProjection2[1][1] = 2.0f / (t - b); lightProjection2[1][2] = 0.0f;         lightProjection2[1][3] = -(t + b) / (t - b);
		lightProjection2[2][0] = 0.0f;         lightProjection2[2][1] = 0.0f;         lightProjection2[2][2] = 2.0f / (f - n); lightProjection2[2][3] = -(f + n) / (f - n);
		lightProjection2[3][0] = 0.0f;         lightProjection2[3][1] = 0.0f;         lightProjection2[3][2] = 0.0f;         lightProjection2[3][3] = 1.0;


		//std::cout << shadowOrthoProjections[i].l << "   " << shadowOrthoProjections[i].r << "   " << shadowOrthoProjections[i].b << "   " << shadowOrthoProjections[i].t << "   " << shadowOrthoProjections[i].n << "   " << shadowOrthoProjections[i].f << std::endl;


		lightSpaceMatrix[i] = glm::transpose(lightView * lightProjection2);	// ToDo was ist hier los?

		directionalDepthMapShader->setMat4("lightSpaceMatrix", lightSpaceMatrix[i]);
	}

	void Shadow::CalculateOrthoProjections(glm::mat4 cameraView, glm::mat4 lightView)
	{
		glm::mat4 camInv = glm::inverse(cameraView);
				
		float ar = windowheight / windowWidth;
		float tanHalfHFOV = glm::tan(glm::radians(90.0f / 2.0f));
		float tanHalfVFOV = glm::tan((glm::radians(90.0f * ar) / 2.0f));

		for (unsigned int i = 0; i < NUM_CASCADES; i++) {
			float xn = cascadePlanes[i] * tanHalfHFOV;
			float xf = cascadePlanes[i + 1] * tanHalfHFOV;
			float yn = cascadePlanes[i] * tanHalfVFOV;
			float yf = cascadePlanes[i + 1] * tanHalfVFOV;

			glm::vec4 frustumCorners[NUM_FRUSTUM_CORNERS] = {
				// near face
				glm::vec4(xn,   yn, cascadePlanes[i], 1.0),
				glm::vec4(-xn,  yn, cascadePlanes[i], 1.0),
				glm::vec4(xn,  -yn, cascadePlanes[i], 1.0),
				glm::vec4(-xn, -yn, cascadePlanes[i], 1.0),

				// far face
				glm::vec4(xf,   yf, cascadePlanes[i + 1], 1.0),
				glm::vec4(-xf,  yf, cascadePlanes[i + 1], 1.0),
				glm::vec4(xf,  -yf, cascadePlanes[i + 1], 1.0),
				glm::vec4(-xf, -yf, cascadePlanes[i + 1], 1.0)
			};

			float minX = std::numeric_limits<float>::max();
			float maxX = std::numeric_limits<float>::min();
			float minY = std::numeric_limits<float>::max();
			float maxY = std::numeric_limits<float>::min();
			float minZ = std::numeric_limits<float>::max();
			float maxZ = std::numeric_limits<float>::min();

			for (unsigned int j = 0; j < NUM_FRUSTUM_CORNERS; j++) {

				glm::vec4 lightSpace = frustumCorners[j] * camInv;
				
				glm::vec4 corner = lightSpace * lightView;

				minX = glm::min(minX, corner.x);
				maxX = glm::max(maxX, corner.x);
				minY = glm::min(minY, corner.y);
				maxY = glm::max(maxY, corner.y);
				minZ = glm::min(minZ, corner.z);
				maxZ = glm::max(maxZ, corner.z);
			}
			
			shadowOrthoProjections[i].l = minX;
			shadowOrthoProjections[i].r = maxX;
			shadowOrthoProjections[i].b = minY;
			shadowOrthoProjections[i].t = maxY;
			shadowOrthoProjections[i].n = minZ;
			shadowOrthoProjections[i].f = maxZ;
		}
	}

}