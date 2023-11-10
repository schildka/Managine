#include <engine/renderer/terrain.h>

#include <engine/assets/heightMapGenerator.h>

#include <iostream>

#include "error.h"

// Enable mutitexture blending across the terrain
#ifndef ENABLE_MULTITEXTURE
#define ENABLE_MULTITEXTURE 1
#endif

// Enable the blend constants based on the slope of the terrain
#ifndef ENABLE_SLOPE_BASED_BLEND
#define ENABLE_SLOPE_BASED_BLEND 1
#endif 

inline float GetPercentage(float value, const float min, const float max)
{
	value = glm::clamp(value, min, max);
	return (value - min) / (max - min);
}

// Convert data from the char buffer to a floating point value between 0..1
// depending on the storage value of the original data
// NOTE: This only works with (LSB,MSB) data storage.
inline float GetHeightValue(const unsigned char* data, unsigned char numBytes)
{
	switch (numBytes)
	{
	case 1:
	{
		return (unsigned char)(data[0]) / (float)0xff;
	}
	break;
	case 2:
	{
		return (unsigned short)(data[1] << 8 | data[0]) / (float)0xffff;
	}
	break;
	case 3:
	{
		return (unsigned int)(data[2] << 16 | data[1] << 8 | data[0]) / (float)0xffffff;
	}
	break;
	case 4:
	{
		return (unsigned int)(data[3] << 24 | data[2] << 16 | data[1] << 8 | data[0]) / (float)0xffffffff;
	}
	break;
	default:
	{
		assert(false);  // Height field with non standard pixel sizes
	}
	break;
	}

	return 0.0f;
}

namespace engine::renderer {

	Terrain::Terrain(float heightScale /* = 500.0f */, float blockScale /* = 2.0f */, glm::vec3 position)
		: localToWorldMatrix(1)
		, inverseLocalToWorldMatrix(1)
		, heightmapDimensions(0, 0)
		, heightScale(heightScale)
		, blockScale(blockScale)
	{
		transform = Transform(position);
	}

	Terrain::~Terrain()
	{
		Terminate();
	}

	void Terrain::Terminate()
	{
		Mesh::~Mesh();

		if (colorBuffer != GL_NONE) glDeleteBuffers(1, &colorBuffer);

		gl::throwOnError();
	}

	void Terrain::generateHeightMap(std::filesystem::path relativeAssetPath, std::string fileName, int width, int height, int seed) {
		engine::assets::HeightMapGenerator gen;
		gen.Generate(relativeAssetPath, fileName, width, height, seed);
	}

	bool Terrain::LoadTexture(const std::string& filename, unsigned int textureStage)
	{
		engine::assets::Texture texture = engine::assets::Texture(filename);

		textures.push_back(texture);

		return (textures.back().id() != 0);
	}

	bool Terrain::LoadHeightmap(const std::string& filename)
	{
		engine::assets::Bitmap heightMap = engine::assets::Bitmap(filename);

		int width = heightMap.width();
		int height = heightMap.height();
		
		const unsigned int bytesPerPixel = heightMap.channels();
		const unsigned int fileSize = (bytesPerPixel * width * height);

		unsigned int numVerts = width * height;
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> texCoords;
		std::vector<unsigned int> indices;
		vertices.resize(numVerts);
		normals.resize(numVerts);
		texCoords.resize(numVerts);

		colors.resize(numVerts);
		heightValues.resize(numVerts);

		heightmapDimensions = glm::vec2(width, height);
		
		// Size of the terrain in world units
		float terrainWidth = (width - 1) * blockScale;
		float terrainHeight = (height - 1) * blockScale;

		float halfTerrainWidth = terrainWidth * 0.5f;
		float halfTerrainHeight = terrainHeight * 0.5f;

		for (unsigned int j = 0; j < height; j++)
		{
			for (unsigned i = 0; i < width; i++)
			{
				unsigned int index = (j * width) + i; //(i * height) + j

				float heightValue = GetHeightValue(&heightMap.data()[index * bytesPerPixel], bytesPerPixel);

				float S = (i / (float)(width - 1));
				float T = (j / (float)(height - 1));

				float X = (S * terrainWidth) - halfTerrainWidth;
				float Y = heightValue * heightScale;
				float Z = (T * terrainHeight) - halfTerrainHeight;

				// Blend 3 textures depending on the height of the terrain
				float tex0Contribution = 1.0f - GetPercentage(heightValue, 0.0f, 0.35f);
				float tex2Contribution = 1.0f - GetPercentage(heightValue, 0.35f, 1.0f);

				heightValues[index] = Y;

				normals[index] = glm::vec3(0);
				vertices[index] = glm::vec3(X, Y, Z);
				texCoords[index] = glm::vec2(S*32, T*32);
#if ENABLE_MULTITEXTURE
				colors[index] = glm::vec4(tex0Contribution, tex0Contribution, tex0Contribution, tex2Contribution);
#else
				colors[index] = glm::vec4(1.0f);
#endif
			}
		}

		std::cout << "Terrain has been loaded!" << std::endl;

		indices = GenerateIndexBuffer(indices);
		normals = GenerateNormals(normals, indices, vertices);
		GenerateBuffers(vertices, normals, texCoords, indices);

		return true;
	}

	std::vector<unsigned int> Terrain::GenerateIndexBuffer(std::vector<unsigned int> indices)
	{

		const unsigned int terrainWidth = heightmapDimensions.x;
		const unsigned int terrainHeight = heightmapDimensions.y;

		// 2 triangles for every quad of the terrain mesh
		const unsigned int numTriangles = (terrainWidth - 1) * (terrainHeight - 1) * 2;

		// 3 indices for each triangle in the terrain mesh
		indices.resize(numTriangles * 3);

		unsigned int index = 0; // Index in the index buffer
		for (unsigned int j = 0; j < (terrainHeight - 1); ++j)
		{
			for (unsigned int i = 0; i < (terrainWidth - 1); ++i)
			{
				int vertexIndex = (j * terrainWidth) + i;
				// Top triangle (T0)
				indices[index++] = vertexIndex;                           // V0
				indices[index++] = vertexIndex + terrainWidth + 1;        // V3
				indices[index++] = vertexIndex + 1;                       // V1
																				// Bottom triangle (T1)
				indices[index++] = vertexIndex;                           // V0
				indices[index++] = vertexIndex + terrainWidth;            // V2
				indices[index++] = vertexIndex + terrainWidth + 1;        // V3
			}
		}
		return indices;
	}

	std::vector<glm::vec3> Terrain::GenerateNormals(std::vector<glm::vec3> normals, std::vector<unsigned int> indices, std::vector<glm::vec3> vertices)
	{
		for (unsigned int i = 0; i < indices.size(); i += 3)
		{
			glm::vec3 v0 = vertices[indices[i + 0]];
			glm::vec3 v1 = vertices[indices[i + 1]];
			glm::vec3 v2 = vertices[indices[i + 2]];

			glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

			normals[indices[i + 0]] += normal;
			normals[indices[i + 1]] += normal;
			normals[indices[i + 2]] += normal;
		}

		const glm::vec3 UP(0.0f, 1.0f, 0.0f);
		for (unsigned int i = 0; i < normals.size(); ++i)
		{
			normals[i] = glm::normalize(normals[i]);

#if ENABLE_SLOPE_BASED_BLEND
			float fTexture0Contribution = glm::clamp(glm::dot(normals[i], UP) - 0.45f, 0.0f, 1.0f);	// ToDo Saturate???
			colors[i] = glm::vec4(fTexture0Contribution, fTexture0Contribution, fTexture0Contribution, colors[i].w);
#endif

		}
		return normals;
	}

	void Terrain::GenerateBuffers(std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, std::vector<glm::vec2> texCoords, std::vector<unsigned int> indices)
	{
		setPositions(vertices.data(), vertices.size());
		setNormals(normals.data(), normals.size());
		setTexCoords(texCoords.data(), texCoords.size());
		setIndices(indices.data(), indices.size());

		setBuffer(colors.data(), colors.size(), colorBuffer);

		bind();
	}

	void Terrain::unbind() {

		Mesh::unbind();

		if (colorAttribute != -1) glDisableVertexAttribArray(colorAttribute);
		gl::throwOnError();
	}

	void Terrain::bindAttributeBuffer(GLuint &buffer, GLuint attribute, int size) {
		if (buffer != GL_NONE && attribute != -1) {
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			glVertexAttribPointer(attribute, size, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(attribute);
			gl::throwOnError();
		}
	}

	void Terrain::setImmutableUniforms(Camera* camera, Lights* lights) {
	}

	void Terrain::renderShadow(ShaderProgram* shader) {
		glm::mat4 model = getModelMatrix();
		shader->setMat4("model", model);
		shader->setBool("isSkeletalMesh", false);
		bind();
		glDrawElements(GL_TRIANGLES, NumIndices, GL_UNSIGNED_INT, 0);
	}

	glm::mat4 Terrain::getModelMatrix() {
		glm::mat4 model = rigidBody->transform;
		return model * transform.GetMatrix();
	}

	void Terrain::render(Camera* camera, Lights* lights, engine::context::Context* context) {
		renderUpdate(camera, lights, context);
		bind();
		draw(lights->GetShadow()->pointDepthMapTexture, lights->GetShadow()->directionalDepthMapTexture, lights->GetShadow()->spotDepthMapTexture, lights->hasDirectionalLight, lights->maxNumberOfPointLights, lights->maxNumberOfSpotLights);
	}

	void Terrain::renderUpdate(Camera* camera, Lights* lights, engine::context::Context* context) {
		shader->use();

		shader->setVec4("clipPlane", camera->clipPlane);

		glm::mat4 model = getModelMatrix();
		shader->setMat4("model", model);
	}

	void Terrain::bind() {

		Mesh::bind();

		colorAttribute = shader->GetAttributeLocation("color");
		bindAttributeBuffer(colorBuffer, colorAttribute, 4);

		gl::throwOnError();
	}

	void Terrain::draw(std::vector<engine::assets::Texture> pointDepthTextures, engine::assets::Texture directionalDepthTextures[], std::vector<engine::assets::Texture> spotDepthTextures, int directionalLight, unsigned int numberOfPointLights, unsigned int numberOfSpotLights) {
		unsigned int bindNumber = 0;

		// bind appropriate textures
		unsigned int textureNr = 0;
		for (unsigned int i = 0; i < textures.size(); i++)
		{
			textureNr++;
			glActiveTexture(GL_TEXTURE0 + bindNumber); // active proper texture unit before binding
											  								  
			// retrieve texture number (the N in diffuse_textureN)
			std::string name = "material.texture" + std::to_string(textureNr);

			// now set the sampler to the correct texture unit
			glUniform1i(glGetUniformLocation(shader->getID(), name.c_str()), bindNumber);
			// and finally bind the texture
			glBindTexture(GL_TEXTURE_2D, textures[i].id());

			bindNumber++;
		}

		for (unsigned int i = 0; i < NUM_CASCADES * directionalLight; i++)
		{
			glActiveTexture(GL_TEXTURE0 + bindNumber);
			glUniform1i(glGetUniformLocation(shader->getID(), ("shadowMap[" + std::to_string(i) + "]").c_str()), bindNumber);
			glBindTexture(GL_TEXTURE_2D, directionalDepthTextures[i].id());

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
}