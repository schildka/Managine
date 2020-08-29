#include <engine/renderer/water.h>

#include<vector>

#include "error.h"

namespace engine::renderer {

	const int Water::reflectionWidth = 1024;
	const int Water::reflectionHeight = 768;
	const int Water::refractionWidth = 1280;
	const int Water::refractionHeight = 720;

	Water::Water(int width, int height, glm::vec2 size, glm::vec3 position) {
		GenerateMesh(size);
		
		transform = Transform(position);
		textureTiling = 2;
		moveFactor = 0;
		moveSpeed = 1.0f / 40.0f;
		distorsionStrength = 0.04f;
		specularPower = 20.0f;


		reflectionFrameBuffer = CreateFrameBuffer();
		reflectionTexture = engine::assets::Texture::CreateTextureAttachment(reflectionWidth, reflectionHeight);
		reflectionDepthBuffer = CreateDepthBufferAttachment(reflectionWidth, reflectionHeight);
		UnbindBuffer(width, height);

		refractionFrameBuffer = CreateFrameBuffer();
		refractionTexture = engine::assets::Texture::CreateTextureAttachment(refractionWidth, refractionHeight);
		refractionDepthTexture = CreateDepthTextureAttachment(refractionWidth, refractionHeight);
		UnbindBuffer(width, height);
	}

	Water::~Water() {
		Mesh::~Mesh();
		glDeleteFramebuffers(1, &reflectionFrameBuffer);
		glDeleteFramebuffers(1, &refractionFrameBuffer);
		glDeleteRenderbuffers(1, &reflectionDepthBuffer);
		glDeleteTextures(1, &refractionDepthTexture);
	}


	void Water::SetDUDV(engine::assets::Texture texture) {
		this->dudv = texture;
	}
	void Water::SetNormal(engine::assets::Texture texture) {
		this->normal = texture;
	}
	
	void Water::GenerateMesh(glm::vec2 size) {
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec2> texCoords;
		std::vector<glm::vec3> normals;
		std::vector<unsigned int> indices;

		for (int i = 0; i < 2; ++i) {
			float x = i * size.x;
			for (int j = 0; j < 2; ++j) {
				float z = j * size.y;

				vertices.push_back(glm::vec3(x, 0, z));
				texCoords.push_back(glm::vec2(i, j));
				normals.push_back(glm::vec3(0, 1, 0));
			}
		}
		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(2);
		indices.push_back(2);
		indices.push_back(1);
		indices.push_back(3);

		setPositions(vertices.data(), vertices.size());
		setNormals(normals.data(), normals.size());
		setTexCoords(texCoords.data(), texCoords.size());
		setIndices(indices.data(), indices.size());
	}


	void Water::Clear() {
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	}
	void Water::BindReflectionBuffer() {
		BindFrameBuffer(reflectionFrameBuffer, reflectionWidth, reflectionHeight);
	}
	void Water::BindRefractionBuffer() {
		BindFrameBuffer(refractionFrameBuffer, refractionWidth, refractionHeight);
	}
	void Water::UnbindBuffer(int width, int height) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, width, height);
	}
	void Water::BindFrameBuffer(GLuint buffer, int width, int height) {
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, buffer);
		glViewport(0, 0, width, height);
	}
	GLuint Water::CreateFrameBuffer() {
		GLuint frameBuffer;
		glGenFramebuffers(1, &frameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);

		return frameBuffer;
	}
	GLuint Water::CreateDepthTextureAttachment(int width, int height) {
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture, 0);

		return texture;
	}
	GLuint Water::CreateDepthBufferAttachment(int width, int height) {
		GLuint depthBuffer;
		glGenRenderbuffers(1, &depthBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

		return depthBuffer;
	}

	void Water::RegisterTexture() {
		shader->use();
		glUniform1i(glGetUniformLocation(shader->getID(), "reflectionSampler"), 0);
		glUniform1i(glGetUniformLocation(shader->getID(), "refractionSampler"), 1);
		glUniform1i(glGetUniformLocation(shader->getID(), "dudvSampler"), 2);
		glUniform1i(glGetUniformLocation(shader->getID(), "normalSampler"), 3);
		glUniform1i(glGetUniformLocation(shader->getID(), "depthSampler"), 4);
	}

	void Water::setImmutableUniforms(Camera* camera, Lights* lights) {
		shader->use();

		shader->setFloat("near", camera->Near);
		shader->setFloat("far", camera->Far);
		shader->setVec3("lightDirection", lights->GetDirectionalLight().direction);
		shader->setVec3("lightColor", lights->GetDirectionalLight().diffuse);

		shader->setFloat("textureTiling", textureTiling);
		shader->setFloat("distorsionStrength", distorsionStrength);
		shader->setFloat("specularPower", specularPower);
		shader->setMat4("model", transform.GetMatrix());
	}

	void Water::renderShadow(ShaderProgram* shader) {
		shader->setMat4("model", transform.GetMatrix());
		shader->setBool("isSkeletalMesh", false);
		bind();
		glDrawElements(GL_TRIANGLES, NumIndices, GL_UNSIGNED_INT, 0);
	}

	void Water::render(Camera* camera, Lights* lights, engine::context::Context* context) {
		renderUpdate(camera, lights, context);
		bind();
		draw();
	}

	void Water::renderUpdate(Camera* camera, Lights* lights, engine::context::Context* context) {
		shader->use();

		shader->setFloat("moveFactor", moveFactor);
	}

	void Water::bind() {
		Mesh::bind();
	}

	void Water::draw(){
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, reflectionTexture.id());
		
		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, refractionTexture.id());

		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_2D, dudv.id());

		glActiveTexture(GL_TEXTURE0 + 3);
		glBindTexture(GL_TEXTURE_2D, normal.id());

		glActiveTexture(GL_TEXTURE0 + 4);
		glBindTexture(GL_TEXTURE_2D, refractionDepthTexture);

		glActiveTexture(GL_TEXTURE0);

		glDrawElements(GL_TRIANGLES, getNumIndices(), GL_UNSIGNED_INT, 0);

		glDisable(GL_BLEND);

		gl::throwOnError();
	}
}