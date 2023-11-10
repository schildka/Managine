#include<engine/renderer/skyBox.h>

#include "error.h"

namespace engine::renderer {

	static const float vertices[] = {
		-1.0,  1.0,  -1.0,
		-1.0, -1.0,  -1.0,
		1.0, -1.0,  -1.0,
		1.0, -1.0,  -1.0,
		1.0,  1.0,  -1.0,
		-1.0,  1.0,  -1.0,

		-1.0, -1.0,   1.0,
		-1.0, -1.0,  -1.0,
		-1.0,  1.0,  -1.0,
		-1.0,  1.0,  -1.0,
		-1.0,  1.0,   1.0,
		-1.0, -1.0,   1.0,

		1.0, -1.0,  -1.0,
		1.0, -1.0,   1.0,
		1.0,  1.0,   1.0,
		1.0,  1.0,   1.0,
		1.0,  1.0,  -1.0,
		1.0, -1.0,  -1.0,

		-1.0, -1.0,   1.0,
		-1.0,  1.0,   1.0,
		1.0,  1.0,   1.0,
		1.0,  1.0,   1.0,
		1.0, -1.0,   1.0,
		-1.0, -1.0,   1.0,

		-1.0,  1.0,  -1.0,
		1.0,  1.0,  -1.0,
		1.0,  1.0,   1.0,
		1.0,  1.0,   1.0,
		-1.0,  1.0,   1.0,
		-1.0,  1.0,  -1.0,

		-1.0, -1.0,  -1.0,
		-1.0, -1.0,   1.0,
		1.0, -1.0,  -1.0,
		1.0, -1.0,  -1.0,
		-1.0, -1.0,   1.0,
		1.0, -1.0,   1.0,
	};

	SkyBox::SkyBox(std::vector<std::string> faces) {
		transform = Transform();
		transform.ScaleBy(glm::vec3(size));

		texture = engine::assets::Texture(faces);

		setPositions(vertices, sizeof(vertices) / sizeof(vertices[0]));
	}

	SkyBox::~SkyBox() {
		Mesh::~Mesh();
	}

	void SkyBox::setImmutableUniforms(Camera* camera, Lights* lights) {

	}

	void SkyBox::renderShadow(ShaderProgram* shader) {

	}

	void SkyBox::render(Camera* camera, Lights* lights, engine::context::Context* context) {
		renderUpdate(camera, lights, context);
		bind();
		draw();
	}

	void SkyBox::renderUpdate(Camera* camera, Lights* lights, engine::context::Context* context) {
				
		shader->use();

		glm::mat4 view = glm::mat4(glm::mat3(camera->GetViewMatrix())); // remove translation from the view matrix
		glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)context->getWidth() / (float)context->getHeight(), camera->Near, camera->Far);
		shader->setMat4("view", view);
		glm::mat4 model = transform.GetMatrix();
		shader->setMat4("model", model);
		glm::mat4 fixedView = glm::translate(view, camera->Position);
		glm::mat4 currentMVP = projection * fixedView * model;

		if (!isValid) {
			shader->setMat4("prevMVP", currentMVP);
			isValid = true;
		}
		else {
			shader->setMat4("prevMVP", prevMVP);
		}
		prevMVP = currentMVP;
	}

	void SkyBox::bind() {
		Mesh::bind();
	}

	void SkyBox::draw() {
		// draw skybox as last
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content

		glActiveTexture(GL_TEXTURE0); // active proper texture unit before binding
		
		// now set the sampler to the correct texture unit
		glUniform1i(glGetUniformLocation(shader->getID(), "skybox"), 0);
		// and finally bind the texture
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture.id());
		
		// always good practice to set everything back to defaults once configured.
		glActiveTexture(GL_TEXTURE0);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		glDepthFunc(GL_LESS); // set depth function back to default

		gl::throwOnError();
	}
}