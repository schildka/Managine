#pragma once

#include <engine/assets/texture.h>

#include <engine/renderer/mesh.h>
#include <engine/renderer/transform.h>

#include <glm\glm.hpp>


namespace engine::renderer {
	/**
	* Water which generates mesh and render water reflection and refractions.
	*/
	class Water : public Mesh {

	public:
		Water() {};
		Water(int width, int height, glm::vec2 size, glm::vec3 position);
		~Water();
		
		float textureTiling;
		float distorsionStrength;
		float moveFactor;
		float moveSpeed;
		float specularPower;

		void SetDUDV(engine::assets::Texture texture);
		void SetNormal(engine::assets::Texture normal);
		
		void GenerateMesh(glm::vec2 size);


		void Clear();
		void BindReflectionBuffer();
		void BindRefractionBuffer();
		void UnbindBuffer(int width, int height);


		void RegisterTexture();

		void setImmutableUniforms(Camera* camera, Lights* lights) override;
		void renderShadow(ShaderProgram* shader) override;
		void render(Camera* camera, Lights* lights, engine::context::Context* context) override;
		void renderUpdate(Camera* camera, Lights* lights, engine::context::Context* context);
		void bind();
		void draw();

	private:

		engine::assets::Texture dudv;
		engine::assets::Texture normal;

		static const int reflectionWidth, reflectionHeight;
		static const int refractionWidth, refractionHeight;

		GLuint reflectionFrameBuffer, reflectionDepthBuffer;
		GLuint refractionFrameBuffer, refractionDepthTexture;
		engine::assets::Texture reflectionTexture, refractionTexture;

		GLuint CreateFrameBuffer();
		GLuint CreateDepthTextureAttachment(int width, int height);
		GLuint CreateDepthBufferAttachment(int width, int height);
		void BindFrameBuffer(GLuint buffer, int width, int height);

	};

}