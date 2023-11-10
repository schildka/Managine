#pragma once

#include <engine/renderer/mesh.h>
#include<engine/renderer/transform.h>

#include <engine/physics/physics.h>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace engine::renderer {
	/**
	* Skeletal Mesh inherits from mesh and holds skeletal mesh model and it's animation as well as handling them.
	*/
	class SkeletalMesh : public Mesh{
	public:
		SkeletalMesh(glm::vec3 position = glm::vec3(), glm::vec3 rotation = glm::vec3(), glm::vec3 scale = glm::vec3(1.0f,1.0f,1.0f));
		
		virtual ~SkeletalMesh();

	public:

		void setBuffers(const engine::assets::Vertices vertices, const std::vector<engine::assets::VertexBoneData> bones, size_t bonesCount, const std::vector<unsigned int> indices, size_t indicesCount);

		void setBoneInformations(std::vector<engine::assets::BoneInfo> boneInfo, std::map<std::string, unsigned int> boneMapping, engine::assets::Node nodes);
		void setAnimations(std::vector<engine::assets::Animation> animations);

		void animate(unsigned int animationIndex, ShaderProgram* shader);

		void setImmutableUniforms(Camera* camera, Lights* lights) override;
		void renderShadow(ShaderProgram* shader) override;
		void render(Camera* camera, Lights* lights, engine::context::Context* context) override;
		void renderUpdate(Camera* camera, Lights* lights, engine::context::Context* context);
		void bind();
		void draw(std::vector<engine::assets::Texture> pointDepthTextures, engine::assets::Texture directionalDepthTextures[], std::vector<engine::assets::Texture> spotDepthTextures, int directionalLight, unsigned int numberOfPointLights, unsigned int numberOfSpotLights);

		// Animtaion Methods
		std::vector<glm::mat4> BoneTransform(std::vector<glm::mat4> transforms, unsigned int animationIndex);
		/** Traverse node hirarchy recurively to apply all bone transforms. */
		void ReadNodeHeirarchy(double animationTime, float blendTime, unsigned int animationIndex, const engine::assets::Node* pNode, const glm::mat4 parentTransform);
		/** Is called if new animation interupts playing animation. */
		void Blend(unsigned int animationIndex, const glm::mat4 parentTransform, float blendMaxTime = 0.25, unsigned int blendMultiplier = 4);
		const engine::assets::NodeAnimation* FindNodeAnim(const std::string nodeName, unsigned int animationIndex);
		glm::mat4 CalculateNodeTransformation(float animationTime, float blendTime, const engine::assets::NodeAnimation* nodeAnim, std::string nodeName, unsigned int lastAnimationIndex);
		glm::vec3 CalcInterpolatedScaling(double animationTime, const engine::assets::NodeAnimation* nodeAnim);
		glm::quat CalcInterpolatedRotation(double animationTime, const engine::assets::NodeAnimation* nodeAnim);
		glm::vec3 CalcInterpolatedPosition(double animationTime, const engine::assets::NodeAnimation* nodeAnim);
		glm::vec3 CalcBlendScaling(double animationTime, float blendTime, const engine::assets::NodeAnimation* nodeAnim, const engine::assets::NodeAnimation* nodeLastAnim);
		glm::quat CalcBlendRotation(double animationTime, float blendTime, const engine::assets::NodeAnimation* nodeAnim, const engine::assets::NodeAnimation* nodeLastAnim);
		glm::vec3 CalcBlendPosition(double animationTime, float blendTime, const engine::assets::NodeAnimation* nodeAnim, const engine::assets::NodeAnimation* nodeLastAnim);
		unsigned int FindScaling(double animationTime, const engine::assets::NodeAnimation* nodeAnim);
		unsigned int FindRotation(double animationTime, const engine::assets::NodeAnimation* nodeAnim);
		unsigned int FindPosition(double animationTime, const engine::assets::NodeAnimation* nodeAnim);
		void ProcessMeshOrientation(double xOffset);
		void UpdateMeshVectors();

		glm::mat4 GetOrientationMatrix();
		glm::vec3 getFront();
		glm::vec3 getRight();

		void unbind();

	private:
		glm::mat4 getModelMatrix();

		void bindBoneAttributeBuffer(GLuint &buffer, int size);

	public:

		engine::physics::RigidBody *rigidBody = nullptr;

		rp3d::CapsuleShape *capsuleShape;
		rp3d::SphereShape *sphereShape;

	private:
		
		GLuint tangentBuffer = GL_NONE;
		GLuint bitangentBuffer = GL_NONE;
		GLuint boneBuffer = GL_NONE;

		GLint tangentAttribute = 0;
		GLint bitangentAttribute = 0;
		GLint boneIDsAttribute = 0;
		GLint boneSecondIDsAttribute = 0;
		GLint boneWeightsAttribute = 0;
		GLint boneSecondWeightsAttribute = 0;
		
		unsigned int numberOfBones = 0;

		glm::mat4 globalInverseTransformation;
		
		std::vector<engine::assets::BoneInfo> boneInfo;
		std::map<std::string, unsigned int> boneMapping;
		std::vector<engine::assets::Animation> animations;
		engine::assets::Node nodes;

		double animationTime = 0;
		unsigned int lastAnimationIndex = 0;
		std::chrono::high_resolution_clock::time_point startFrameTime;
		bool isBlending = false;

		glm::vec3 front;
		glm::vec3 up;
		glm::vec3 worldUP = glm::vec3(0, 1, 0);
		glm::vec3 right;
		float yaw = 90;

		unsigned int animationIndex = 0;
	};
}
