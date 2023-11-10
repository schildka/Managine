#pragma once

#include <engine/renderer/renderable.h>

#include <glad/glad.h>

#include<vector>

#include <engine/assets/model.h>

namespace engine::renderer {
	/**
	* Mesh class inheriting from Renderable including vertices, normals, texture coordinates, materials or textures.
	*/
    class Mesh : public Renderable  {
    public:
        Mesh();

		Mesh(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);

		Mesh(std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, std::vector<glm::vec2> texCoords, std::vector<unsigned int> indices);

		Mesh* Clone();

        virtual ~Mesh();

    public:

		void setBuffers(const engine::assets::Vertices vertices, const std::vector<unsigned int> indices, size_t indicesCount);

		void setTextures(std::vector<engine::assets::ModelTexture> textures);

		void setMaterial(engine::assets::Material material);

        void setPositions(const glm::vec3 *positions, size_t count);

        void setNormals(const glm::vec3 *normals, size_t count);

        void setTexCoords(const glm::vec2 *texCoords, size_t count);

		void setPositions(const float *positions, size_t count);

		void setNormals(const float *normals, size_t count);

		void setTexCoords(const float *texCoords, size_t count);

        void setIndices(const unsigned int *indices, size_t count);

		engine::assets::Material* getMaterial() { return &material; };

		void setMaterialUniforms();

		unsigned int getNumIndices();

		glm::mat4 getModelMatrix();

		Transform* GetTransform();
		void SetParentTransform(Transform* t);

		void render(Camera* camera, Lights* lights, engine::context::Context* context) override;
		void renderShadow(ShaderProgram* shader) override;

		void setImmutableUniforms(Camera* camera, Lights* lights) override;
		
		void bind();

		void draw(std::vector<engine::assets::Texture> pointDepthTextures, engine::assets::Texture directionalDepthTextures[], std::vector<engine::assets::Texture> spotDepthTextures, int directionalLight, unsigned int numberOfPointLights, unsigned int numberOfSpotLights);

        void unbind();

    protected:
        void bindAttributeBuffer(GLuint &buffer, GLuint attribute, int stride);

        template<typename T>
        void setBuffer(const T *data, size_t count, GLuint &buffer, GLenum target = GL_ARRAY_BUFFER) {
            if (buffer == GL_NONE)
                glGenBuffers(1, &buffer);

            glBindBuffer(target, buffer);
            glBufferData(target, sizeof(T) * count, data, GL_STATIC_DRAW);
        }

    protected:
		Transform transform;
		Transform *parentTransform = nullptr;

		std::vector<engine::assets::ModelTexture> modelTextures;
		std::vector<engine::assets::Texture> textures;
		engine::assets::Material material;

        GLuint indexBuffer = GL_NONE;
        GLuint positionBuffer = GL_NONE;
        GLuint texCoordBuffer = GL_NONE;
        GLuint normalBuffer = GL_NONE;

        GLint positionAttribute = 0;
        GLint normalAttribute = 0;
        GLint texCoordAttribute = 0;

        GLsizei NumIndices = 0;
    };

    class Quad : public Mesh {
    public:
        Quad();
    };

    class Triangle : public Mesh {
    public:
        Triangle();
    };

    class Cube : public Mesh {
    public:
        Cube();
    };
}
