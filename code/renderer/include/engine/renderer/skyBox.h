#pragma once

#include <engine/assets/texture.h>

#include <engine/renderer/mesh.h>
#include <engine/renderer/transform.h>

#define GLM_FORCE_RADIANS 1
#include <glm/glm.hpp>

#include<vector>

namespace engine::renderer {
	/**
	* SkyBox cubeMap sourrunds scene
	* @param texture CubeMap texture.
	* @param sun DirectionalLight can be set to rotate with cubeMap.
	*/
	class SkyBox : public Mesh {
	public:
		SkyBox() {};
		SkyBox(std::vector<std::string> faces);
		
		~SkyBox();

		void setImmutableUniforms(Camera* camera, Lights* lights) override;
		void renderShadow(ShaderProgram* shader) override;
		void render(Camera* camera, Lights* lights, engine::context::Context* context) override;
		void renderUpdate(Camera* camera, Lights* lights, engine::context::Context* context);
		void bind();
		void draw();

	public:
		engine::assets::Texture texture;

		glm::mat4 prevMVP;
		float speed = 0.005f;
		bool isValid = false;
		unsigned size = 2000.0f;

		DirectionalLight *sun = nullptr;
	};
}