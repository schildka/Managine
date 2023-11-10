#pragma once;

#include <engine/assets/texture.h>

#include <engine/renderer/mesh.h>
#include <engine/renderer/transform.h>

#include <engine/physics/physics.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/std_based_type.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <vector>

namespace engine::renderer {
	/**
	* Terrain Holds all relevant terrain data and creates terrain mesh out of heihtmap data.
	* @param heightFieldShape set by reactphysics
	*/
	class Terrain : public Mesh
	{
	public:

		Terrain(float heightScale = 500.0f, float blockScale = 2.0f, glm::vec3 position = glm::vec3(0.0f, -7.5f, 0.0f));
		virtual ~Terrain();

		void Terminate();
		void generateHeightMap(std::filesystem::path relativeAssetPath, std::string fileName, int width, int height, int seed = 0);
		bool LoadHeightmap(const std::string& filename);
		bool LoadTexture(const std::string& filename, unsigned int textureStage = 0);

		void setImmutableUniforms(Camera* camera, Lights* lights) override;
		void renderShadow(ShaderProgram* shader) override;
		void render(Camera* camera, Lights* lights, engine::context::Context* context) override;
		void renderUpdate(Camera* camera, Lights* lights, engine::context::Context* context);
		void bind();
		void draw(std::vector<engine::assets::Texture> pointDepthTextures, engine::assets::Texture directionalDepthTextures[], std::vector<engine::assets::Texture> spotDepthTextures, int directionalLight, unsigned int numberOfPointLights, unsigned int numberOfSpotLights);

	protected:
		std::vector<unsigned int> GenerateIndexBuffer(std::vector<unsigned int> indices);
		std::vector<glm::vec3> GenerateNormals(std::vector<glm::vec3> normals, std::vector<unsigned int> indices, std::vector<glm::vec3> vertices);
		// Generates the vertex buffer objects from the position, normal, texture, and color buffers
		void GenerateBuffers(std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, std::vector<glm::vec2> texCoords, std::vector<unsigned int> indices);

		void unbind();

	private:
		glm::mat4 getModelMatrix();

		void bindAttributeBuffer(GLuint &buffer, GLuint attribute, int size);

		template<typename T>
		void setBuffer(const T *data, size_t count, GLuint &buffer, GLenum target = GL_ARRAY_BUFFER) {
			if (buffer == GL_NONE)
				glGenBuffers(1, &buffer);

			glBindBuffer(target, buffer);
			glBufferData(target, sizeof(T) * count, data, GL_STATIC_DRAW);
		}

	public:

		engine::physics::RigidBody *rigidBody = nullptr;
		rp3d::HeightFieldShape* heightFieldShape;
		std::vector<float> heightValues;

		// The dimensions of the heightmap texture
		glm::vec2 heightmapDimensions;

	private:

		std::vector<glm::vec4> colors;
		
		// ID's for the VBO's
		GLuint  colorBuffer = GL_NONE;

		GLint colorAttribute = 0;

		glm::mat4x4 localToWorldMatrix;
		glm::mat4x4 inverseLocalToWorldMatrix;

		// The height-map value will be multiplied by this value
		// before it is assigned to the vertex's Y-coordinate
		float   heightScale;
		// The vertex's X and Z coordinates will be multiplied by this
		// for each step when building the terrain
		float   blockScale;
	};
}