// GameLab 3 Tutorial 2019/20
// Andreas Knote <andreas.knote@uni-wuerzburg.de>

#include <engine/renderer/mesh.h>

#include "error.h"

namespace engine::renderer {

    Mesh::Mesh() {}

	Mesh::Mesh(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) {
		transform = Transform(position, rotation, scale);
	}

	Mesh::Mesh(std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, std::vector<glm::vec2> texCoords, std::vector<unsigned int> indices) {
		setPositions(vertices.data(), vertices.size());
		setNormals(normals.data(), normals.size());
		setTexCoords(texCoords.data(), texCoords.size());
		setIndices(indices.data(), indices.size());
	}

	Mesh* Mesh::Clone() {
		Mesh* m = new Mesh();
		m->transform = this->transform;
		m->modelTextures = this->modelTextures;
		m->textures = this->textures;
		m->material = this->material;

		m->indexBuffer = this->indexBuffer;
		m->positionBuffer = this->positionBuffer;
		m->texCoordBuffer = this->texCoordBuffer;
		m->normalBuffer = this->normalBuffer;

		m->positionAttribute = this->positionAttribute;
		m->normalAttribute = this->normalAttribute;
		m->texCoordAttribute = this->texCoordAttribute;

		m->NumIndices = this->NumIndices;

		m->shader = this->shader;

		return m;
	}

    Mesh::~Mesh() {
		if (positionBuffer != GL_NONE) glDeleteBuffers(1, &positionBuffer);
        if (indexBuffer != GL_NONE) glDeleteBuffers(1, &indexBuffer);
        if (texCoordBuffer != GL_NONE) glDeleteBuffers(1, &texCoordBuffer);
        if (normalBuffer != GL_NONE) glDeleteBuffers(1, &normalBuffer);
        gl::throwOnError();
    }

	void Mesh::setBuffers(const engine::assets::Vertices vertices, const std::vector<unsigned int> indices, size_t indicesCount) {
		setPositions(vertices.Positions.data(), vertices.Positions.size());
		setNormals(vertices.Normals.data(), vertices.Normals.size());
		setTexCoords(vertices.TexCoords.data(), vertices.TexCoords.size());
		setIndices(indices.data(), indices.size());
	}

	void Mesh::setTextures(std::vector<engine::assets::ModelTexture> textures) {
		modelTextures = textures;
	}

	void Mesh::setMaterial(engine::assets::Material material) {
		this->material = material;
	}

    void Mesh::setPositions(const glm::vec3 *positions, size_t count) {
        setBuffer(positions, count, positionBuffer);
    }

    void Mesh::setNormals(const glm::vec3 *normals, size_t count) {
        setBuffer(normals, count, normalBuffer);
    }

    void Mesh::setTexCoords(const glm::vec2 *texCoords, size_t count) {
        setBuffer(texCoords, count, texCoordBuffer);
    }

	void Mesh::setPositions(const float *positions, size_t count) {
		setBuffer(positions, count, positionBuffer);
	}

	void Mesh::setNormals(const float *normals, size_t count) {
		setBuffer(normals, count, normalBuffer);
	}

	void Mesh::setTexCoords(const float *texCoords, size_t count) {
		setBuffer(texCoords, count, texCoordBuffer);
	}

    void Mesh::setIndices(const unsigned int *indices, size_t count) {
        NumIndices = (GLsizei) count;
        setBuffer(indices, count, indexBuffer, GL_ELEMENT_ARRAY_BUFFER);
    };

	unsigned int Mesh::getNumIndices() {
		return NumIndices;
	}

	glm::mat4 Mesh::getModelMatrix() {
		if (parentTransform != nullptr) {
			transform.modelMatrix = parentTransform->GetMatrix() * transform.GetMatrix();
			return transform.modelMatrix;
		}
		else {
			return transform.GetMatrix();
		}
	}

	Transform* Mesh::GetTransform() {
		return &transform;
	}

	void Mesh::SetParentTransform(Transform* t) {
		parentTransform = t;
	}

	void Mesh::setMaterialUniforms() {
		shader->use();
		shader->setVec3("material.ambient", material.ambient);
		shader->setVec3("material.diffuse", material.diffuse);
		shader->setVec3("material.specular", material.specular);
		shader->setFloat("material.shininess", material.shininess);
	}

	void Mesh::render(Camera* camera, Lights* lights, engine::context::Context* context) {
		shader->use();

		shader->setMat4("model", getModelMatrix());
		shader->setVec4("clipPlane", camera->clipPlane);

		bind();
		draw(lights->GetShadow()->pointDepthMapTexture, lights->GetShadow()->directionalDepthMapTexture, lights->GetShadow()->spotDepthMapTexture, lights->hasDirectionalLight, lights->maxNumberOfPointLights, lights->maxNumberOfSpotLights);
	}

	void Mesh::renderShadow(ShaderProgram* shader) {
		shader->setMat4("model", getModelMatrix());
		shader->setBool("isSkeletalMesh", false);

		bind();
		glDrawElements(GL_TRIANGLES, NumIndices, GL_UNSIGNED_INT, 0);
	}

	void Mesh::setImmutableUniforms(Camera* camera, Lights* lights) {
		shader->use();
		shader->setVec3("material.ambient", material.ambient);
		shader->setVec3("material.diffuse", material.diffuse);
		shader->setVec3("material.specular", material.specular);
		shader->setFloat("material.shininess", material.shininess);
	}

    void Mesh::bind() {
        positionAttribute = shader->GetAttributeLocation("position");
        normalAttribute = shader->GetAttributeLocation("normal");
        texCoordAttribute = shader->GetAttributeLocation("texCoord");
        gl::throwOnError();

        bindAttributeBuffer(positionBuffer, positionAttribute, 3);
        bindAttributeBuffer(normalBuffer, normalAttribute, 3);
        bindAttributeBuffer(texCoordBuffer, texCoordAttribute, 2);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        gl::throwOnError();
    }

    void Mesh::draw(std::vector<engine::assets::Texture> pointDepthTextures, engine::assets::Texture directionalDepthTextures[], std::vector<engine::assets::Texture> spotDepthTextures, int directionalLight, unsigned int numberOfPointLights, unsigned int numberOfSpotLights) {
		unsigned int bindNumber = 0;

		// bind appropriate textures
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		unsigned int normalNr = 1;
		unsigned int heightNr = 1;
		for (unsigned int i = 0; i < modelTextures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + bindNumber); // active proper texture unit before binding
													   // retrieve texture number (the N in diffuse_textureN)
			std::string number;
			std::string name = "material." + modelTextures[i].type;
			if (name == "material.texture_diffuse")
				number = std::to_string(diffuseNr++);
			else if (name == "material.texture_specular")
				number = std::to_string(specularNr++); // transfer unsigned int to stream
			else if (name == "material.texture_normal")
				number = std::to_string(normalNr++); // transfer unsigned int to stream

													 // now set the sampler to the correct texture unit
			glUniform1i(glGetUniformLocation(shader->getID(), (name + number).c_str()), bindNumber);
			// and finally bind the texture
			glBindTexture(GL_TEXTURE_2D, modelTextures[i].id);

			bindNumber++;
		}

		for (unsigned int i = 0; i < NUM_CASCADES * directionalLight; i++)
		{
			glActiveTexture(GL_TEXTURE0 + bindNumber);
			glUniform1i(glGetUniformLocation(shader->getID(), ("shadowMap[" + std::to_string(i) + "]").c_str()), bindNumber);
			glBindTexture(GL_TEXTURE_2D, directionalDepthTextures[i].id());

			bindNumber++;
		}
		for (unsigned int i = NUM_CASCADES * directionalLight; i < NUM_CASCADES; i++)
		{
			glActiveTexture(GL_TEXTURE0 + bindNumber);
			glUniform1i(glGetUniformLocation(shader->getID(), ("shadowMap[" + std::to_string(i) + "]").c_str()), bindNumber);
			glBindTexture(GL_TEXTURE_2D, 0);

			bindNumber++;
		}

		for (unsigned int i = 0; i < pointDepthTextures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + bindNumber);
			glUniform1i(glGetUniformLocation(shader->getID(), ("pointDepthMap[" + std::to_string(i) + "]").c_str()), bindNumber);
			glBindTexture(GL_TEXTURE_CUBE_MAP, pointDepthTextures[i].id());

			bindNumber++;
		}
		for (unsigned int i = pointDepthTextures.size(); i < numberOfPointLights; i++)
		{
			glActiveTexture(GL_TEXTURE0 + bindNumber);
			glUniform1i(glGetUniformLocation(shader->getID(), ("pointDepthMap[" + std::to_string(i) + "]").c_str()), bindNumber);
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

			bindNumber++;
		}

		for (unsigned int i = 0; i < spotDepthTextures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + bindNumber);
			glUniform1i(glGetUniformLocation(shader->getID(), ("spotDepthMap[" + std::to_string(i) + "]").c_str()), bindNumber);
			glBindTexture(GL_TEXTURE_2D, spotDepthTextures[i].id());

			bindNumber++;
		}
		for (unsigned int i = spotDepthTextures.size(); i < numberOfSpotLights; i++)
		{
			glActiveTexture(GL_TEXTURE0 + bindNumber);
			glUniform1i(glGetUniformLocation(shader->getID(), ("spotDepthMap[" + std::to_string(i) + "]").c_str()), bindNumber);
			glBindTexture(GL_TEXTURE_2D, 0);

			bindNumber++;
		}

		// always good practice to set everything back to defaults once configured.
		glActiveTexture(GL_TEXTURE0);

		glDrawElements(GL_TRIANGLES, getNumIndices(), GL_UNSIGNED_INT, 0);

        gl::throwOnError();
    }

    void Mesh::unbind() {
        if (positionAttribute != -1) glDisableVertexAttribArray(positionAttribute);
        if (normalAttribute != -1) glDisableVertexAttribArray(normalAttribute);
        if (texCoordAttribute != -1) glDisableVertexAttribArray(texCoordAttribute);
        gl::throwOnError();
    }

    void Mesh::bindAttributeBuffer(GLuint &buffer, GLuint attribute, int stride) {
        if (buffer != GL_NONE && attribute != -1) {
            glBindBuffer(GL_ARRAY_BUFFER, buffer);
            glVertexAttribPointer(attribute, stride, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(attribute);
            gl::throwOnError();
        }
    }

    Quad::Quad() : Mesh() {
        unsigned int indices[] = {
                0, 1, 2,
                0, 2, 3
        };

        float vertices[] = {
                -1, 1, 0,
                -1, -1, 0,
                1, -1, 0,
                1, 1, 0
        };

        float uv[] = {
                0, 1,
                0, 0,
                1, 0,
                1, 1
        };

        setIndices(indices, 9);
        setPositions(vertices, 12);
        setTexCoords(uv, 8);
    };

    Triangle::Triangle() : Mesh() {
        unsigned int indices[] = {
                2, 1, 0
        };

        float vertices[] = {
                0.5f, -0.5f, 0.0f,
                -0.5f, -0.5f, 0.0f,
                -0.5f, 0.5f, 0.0f
        };

        setIndices(indices, 3);
        setPositions(vertices, 9);
    };

    Cube::Cube() : Mesh() {
        float position[] = {-.5f, .5f, -.5f, -.5f, .5f, .5f, .5f, .5f, .5f, .5f, .5f, -.5f, -.5f, .5f, -.5f, -.5f, -.5f,
                            -.5f, -.5f, -.5f, .5f, -.5f, .5f, .5f, .5f, .5f, .5f, .5f, -.5f, .5f, .5f, -.5f, -.5f, .5f,
                            .5f, -.5f, .5f, .5f, -.5f, .5f, -.5f, -.5f, -.5f, -.5f, -.5f, -.5f, .5f, -.5f, -.5f, .5f,
                            .5f, -.5f, -.5f, .5f, .5f, -.5f, .5f, .5f, .5f, .5f, -.5f, -.5f, .5f, -.5f, -.5f, -.5f, .5f,
                            -.5f, -.5f, .5f, -.5f, .5f};
        float normal[] = {0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, -1.f, 0.f, 0.f, -1.f, 0.f, 0.f,
                          -1.f, 0.f, 0.f, -1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f,
                          0.f, 0.f, -1.f, 0.f, 0.f, -1.f, 0.f, 0.f, -1.f, 0.f, 0.f, -1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f,
                          0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, -1.f, 0.f, 0.f, -1.f, 0.f, 0.f, -1.f, 0.f, 0.f, -1.f, 0.f};
        float texCoord[] = {0.f, 0.f, 0.f, 1.f, 1.f, 1.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f, 1.f, 0.f, 0.f, 0.f,
                            0.f, 1.f, 1.f, 1.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f,
                            1.f, 1.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f, 1.f, 0.f};
        unsigned int index[] = {0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 8, 9, 10, 8, 10, 11, 12, 13, 14, 12, 14, 15, 16, 17,
                                18, 16, 18, 19, 20, 21, 22, 20, 22, 23};

        setIndices(index, sizeof(index) / sizeof(index[0]));
        setPositions(position, sizeof(position) / sizeof(position[0]));
        setTexCoords(texCoord, sizeof(texCoord) / sizeof(texCoord[0]));
        setNormals(normal, sizeof(normal) / sizeof(normal[0]));
    }
}
