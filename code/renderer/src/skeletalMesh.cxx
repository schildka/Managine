// GameLab 3 Tutorial 2019/20
// Andreas Knote <andreas.knote@uni-wuerzburg.de>

#include <engine/renderer/skeletalMesh.h>

#include "error.h"

namespace engine::renderer {

	SkeletalMesh::SkeletalMesh(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) {
		startFrameTime = std::chrono::high_resolution_clock::now();

		transform = Transform(position, rotation, scale);

		UpdateMeshVectors();
	}

	SkeletalMesh::~SkeletalMesh() {
		Mesh::~Mesh();
		if (tangentBuffer != GL_NONE) glDeleteBuffers(1, &tangentBuffer);
		if (bitangentBuffer != GL_NONE) glDeleteBuffers(1, &bitangentBuffer);
		if (boneBuffer != GL_NONE) glDeleteBuffers(1, &boneBuffer);
		gl::throwOnError();
	}

	void SkeletalMesh::setBuffers(const engine::assets::Vertices vertices, const std::vector<engine::assets::VertexBoneData> bones, size_t bonesCount, const std::vector<unsigned int> indices, size_t indicesCount) {
		Mesh::setBuffers(vertices, indices, indicesCount);

		setBuffer(vertices.Tangents.data(), vertices.Tangents.size(), tangentBuffer);
		setBuffer(vertices.Bitangents.data(), vertices.Bitangents.size(), bitangentBuffer);
		setBuffer(bones.data(), bonesCount, boneBuffer);
	}

	void SkeletalMesh::setBoneInformations(std::vector<engine::assets::BoneInfo> boneInfo, std::map<std::string, unsigned int> boneMapping, engine::assets::Node nodes) {
		this->boneInfo = boneInfo;
		this->boneMapping = boneMapping;
		this->nodes = nodes;
		this->numberOfBones = boneInfo.size();

		this->globalInverseTransformation = glm::transpose(glm::inverse(nodes.transformation));
	}

	void SkeletalMesh::setAnimations(std::vector<engine::assets::Animation> animations) {
		this->animations = animations;
	}

	void SkeletalMesh::unbind() {
		Mesh::unbind();
		
		if (tangentAttribute != -1) glDisableVertexAttribArray(tangentAttribute);
		if (bitangentAttribute != -1) glDisableVertexAttribArray(bitangentAttribute);
		if (boneIDsAttribute != -1) glDisableVertexAttribArray(boneIDsAttribute);
		if (boneSecondIDsAttribute != -1) glDisableVertexAttribArray(boneSecondIDsAttribute);
		if (boneWeightsAttribute != -1) glDisableVertexAttribArray(boneWeightsAttribute);
		if (boneSecondWeightsAttribute != -1) glDisableVertexAttribArray(boneSecondWeightsAttribute);
		gl::throwOnError();
	}

	void SkeletalMesh::bindBoneAttributeBuffer(GLuint &buffer, int size) {
		if (buffer != GL_NONE && boneIDsAttribute != -1) {
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			glEnableVertexAttribArray(boneIDsAttribute);
			glVertexAttribIPointer(boneIDsAttribute, size, GL_INT, sizeof(engine::assets::VertexBoneData), (void*)0);

			if (boneSecondIDsAttribute != -1) {
				glEnableVertexAttribArray(boneSecondIDsAttribute);
				glVertexAttribIPointer(boneSecondIDsAttribute, size, GL_INT, sizeof(engine::assets::VertexBoneData), (void*)(4 * sizeof(int)));
				gl::throwOnError();
			}

			if (boneWeightsAttribute != -1) {
				glEnableVertexAttribArray(boneWeightsAttribute);
				glVertexAttribPointer(boneWeightsAttribute, size, GL_FLOAT, GL_FALSE, sizeof(engine::assets::VertexBoneData), (void*)(8 * sizeof(int)));
				gl::throwOnError();
			}

			if (boneSecondWeightsAttribute != -1) {
				glEnableVertexAttribArray(boneSecondWeightsAttribute);
				glVertexAttribPointer(boneSecondWeightsAttribute, size, GL_FLOAT, GL_FALSE, sizeof(engine::assets::VertexBoneData), (void*)(8 * sizeof(int) + 4 * sizeof(float)));
				gl::throwOnError();
			}
			gl::throwOnError();
		}

	}

	void SkeletalMesh::animate(unsigned int animationIndex, ShaderProgram* shader){
		shader->use();
		// Animation
		std::vector<glm::mat4> transforms;
		transforms = BoneTransform(transforms, animationIndex);
		for (unsigned int i = 0; i < transforms.size(); i++) {
			shader->setMat4T("gBones[" + std::to_string(i) + "]", transforms[i]);
		}

		this->animationIndex = animationIndex;
	}

	void SkeletalMesh::setImmutableUniforms(Camera* camera, Lights* lights) {
		shader->use();
		// material properties
		shader->setFloat("material.shininess", 64.0f);
	}

	void SkeletalMesh::renderShadow(ShaderProgram* shader) {
		glm::mat4 model = getModelMatrix();
		shader->setMat4("model", model);
		shader->setBool("isSkeletalMesh", true);

		animate(animationIndex, shader);

		bind();
		glDrawElements(GL_TRIANGLES, NumIndices, GL_UNSIGNED_INT, 0);
	}

	glm::mat4 SkeletalMesh::getModelMatrix() {
		transform.TranslateTo(rigidBody->position);	// macht komischen bug mit physics engine
		transform.modelMatrix = transform.GetMatrix();// *GetOrientationMatrix();
		return transform.modelMatrix;
	}

	void SkeletalMesh::render(Camera* camera, Lights* lights, engine::context::Context* context) {
		renderUpdate(camera, lights, context);
		bind();
		draw(lights->GetShadow()->pointDepthMapTexture, lights->GetShadow()->directionalDepthMapTexture, lights->GetShadow()->spotDepthMapTexture, lights->hasDirectionalLight, lights->maxNumberOfPointLights, lights->maxNumberOfSpotLights);
	}

	void SkeletalMesh::renderUpdate(Camera* camera, Lights* lights, engine::context::Context* context) {
		shader->use();
		
		glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)context->getWidth() / (float)context->getHeight(), camera->Near, camera->Far);
		
		shader->bindSharedMatricesUniforms(projection, camera->GetViewMatrix());
		shader->bindSharedVectorsUniforms(camera->Position);

		glm::ivec3 numLights = glm::ivec3(lights->GetNumberOfPointLights(), lights->GetNumberOfSpotLights(), lights->hasDirectionalLight);
		shader->bindSharedLightsUniforms(lights->GetDirectionalLight(), lights->GetPointLights(), lights->GetSpotLights(), numLights);

		shader->bindSharedShadowsUniforms(lights->GetShadow()->lightSpaceMatrix, lights->GetShadow()->cascadeClipPlanes, lights->GetShadow()->spotLightSpaceMatrix);


		glm::mat4 model = getModelMatrix();
		shader->setMat4("model", model);
		shader->setVec4("clipPlane", camera->clipPlane);
	}
	
	void SkeletalMesh::bind() {

		Mesh::bind();

		tangentAttribute = shader->GetAttributeLocation("tangents");
		bitangentAttribute = shader->GetAttributeLocation("bitangent");
		boneIDsAttribute = shader->GetAttributeLocation("boneIDs");
		boneSecondIDsAttribute = shader->GetAttributeLocation("boneSecondIDs");
		boneWeightsAttribute = shader->GetAttributeLocation("boneWeights");
		boneSecondWeightsAttribute = shader->GetAttributeLocation("boneSecondWeights");
		gl::throwOnError();

		bindAttributeBuffer(tangentBuffer, tangentAttribute, 3);
		bindAttributeBuffer(bitangentBuffer, bitangentAttribute, 3);

		bindBoneAttributeBuffer(boneBuffer, 4);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		gl::throwOnError();
	}

	void SkeletalMesh::draw(std::vector<engine::assets::Texture> pointDepthTextures, engine::assets::Texture directionalDepthTextures[], std::vector<engine::assets::Texture> spotDepthTextures, int directionalLight, unsigned int numberOfPointLights, unsigned int numberOfSpotLights) {
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
			std::string name = "material."+ modelTextures[i].type;
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

		for (unsigned int i = 0; i < pointDepthTextures.size(); i++)
		{	glActiveTexture(GL_TEXTURE0 + bindNumber);
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

	std::vector<glm::mat4> SkeletalMesh::BoneTransform(std::vector<glm::mat4> transforms, unsigned int animationIndex)
	{
		glm::mat4 identity(1.0f);

		long double timeInSeconds = std::chrono::duration_cast<std::chrono::duration<long double>>(
			std::chrono::high_resolution_clock::now() - startFrameTime).count();

		if (animationIndex != lastAnimationIndex || isBlending) {
			Blend(animationIndex, identity);
		}
		else {
			double ticksPerSecond = animations[animationIndex].ticksPerSecond != 0 ? animations[animationIndex].ticksPerSecond : 25.0f;
			double timeInTicks = timeInSeconds * ticksPerSecond;
			animationTime = fmod(timeInTicks, animations[animationIndex].duration);

			ReadNodeHeirarchy(animationTime, 0, animationIndex, &nodes, identity);
		}

		transforms.resize(numberOfBones);

		for (unsigned int i = 0; i < numberOfBones; i++) {
			transforms[i] = boneInfo[i].finalTransformation;
		}

		return transforms;
	}

	void SkeletalMesh::Blend(unsigned int animationIndex, const glm::mat4 identity, float blendMaxTime, unsigned int blendMultiplier) {

		if (!isBlending) {
			isBlending = true;
			startFrameTime = std::chrono::high_resolution_clock::now();
		}

		float blendTime = std::chrono::duration_cast<std::chrono::duration<long double>>(
			std::chrono::high_resolution_clock::now() - startFrameTime).count();

		if (blendTime <= blendMaxTime) {
			ReadNodeHeirarchy(animationTime, blendTime * blendMultiplier, animationIndex, &nodes, identity);
		}
		else {
			lastAnimationIndex = animationIndex;
			startFrameTime = std::chrono::high_resolution_clock::now();
			isBlending = false;
		}
	}

	void SkeletalMesh::ReadNodeHeirarchy(double animationTime, float blendTime, unsigned int animationIndex, const engine::assets::Node* pNode, const glm::mat4 parentTransform)
	{
		std::string nodeName(pNode->name);

		glm::mat4 nodeTransformation = pNode->transformation;

		const engine::assets::NodeAnimation* nodeAnim = FindNodeAnim(nodeName, animationIndex);

		if (nodeAnim) {
			nodeTransformation = CalculateNodeTransformation(animationTime, blendTime, nodeAnim, nodeName, lastAnimationIndex);
		}

		glm::mat4 globalTransformation = nodeTransformation * parentTransform;

		if (boneMapping.find(nodeName) != boneMapping.end()) {
			unsigned int boneIndex = boneMapping[nodeName];
			boneInfo[boneIndex].finalTransformation = boneInfo[boneIndex].boneOffset * globalTransformation * globalInverseTransformation;
		}

		for (unsigned int i = 0; i < pNode->numberOfChildren; i++) {
			ReadNodeHeirarchy(animationTime, blendTime, animationIndex, &pNode->children[i], globalTransformation);
		}
	}

	const engine::assets::NodeAnimation* SkeletalMesh::FindNodeAnim(const std::string nodeName, unsigned int animationIndex)
	{
		if (animations[animationIndex].nodeAnimation.count(nodeName) > 0)
		{
			return &animations[animationIndex].nodeAnimation[nodeName];
		}
		return NULL;
	}

	glm::mat4 SkeletalMesh::CalculateNodeTransformation(float animationTime, float blendTime, const engine::assets::NodeAnimation* nodeAnim, std::string nodeName, unsigned int lastAnimationIndex) {

		glm::vec3 scaling;
		glm::quat rotation;
		glm::vec3 translation;

		if (isBlending) {
			const engine::assets::NodeAnimation* nodeLastAnim = FindNodeAnim(nodeName, lastAnimationIndex);

			scaling = CalcBlendScaling(animationTime, blendTime, nodeAnim, nodeLastAnim);
			rotation = CalcBlendRotation(animationTime, blendTime, nodeAnim, nodeLastAnim);
			translation = CalcBlendPosition(animationTime, blendTime, nodeAnim, nodeLastAnim);
		}
		else {
			scaling = CalcInterpolatedScaling(animationTime, nodeAnim);
			rotation = CalcInterpolatedRotation(animationTime, nodeAnim);
			translation = CalcInterpolatedPosition(animationTime, nodeAnim);
		}

		glm::mat4 scalingMatrix(1.0f);
		scalingMatrix = glm::scale(scalingMatrix, scaling);

		glm::mat4 rotationMatrix(1.0f);
		rotationMatrix = glm::transpose(glm::toMat4(rotation));

		glm::mat4 translationMatrix(1.0f);
		translationMatrix = glm::transpose(glm::translate(translationMatrix, translation));

		return scalingMatrix * rotationMatrix * translationMatrix;
	}

	glm::vec3 SkeletalMesh::CalcInterpolatedScaling(double animationTime, const engine::assets::NodeAnimation* nodeAnim)
	{
		if (nodeAnim->numberOfScalings == 1) {
			return nodeAnim->scalings[0];
		}

		unsigned int scalingIndex = FindScaling(animationTime, nodeAnim);
		unsigned int nextScalingIndex = (scalingIndex + 1);
		assert(nextScalingIndex < nodeAnim->numberOfScalings);

		float deltaTime = (float)(nodeAnim->scaleTime[nextScalingIndex] - nodeAnim->scaleTime[scalingIndex]);
		float factor = (float)((animationTime - nodeAnim->scaleTime[scalingIndex]) / deltaTime);
		assert(factor >= 0.0f && factor <= 1.0f);

		const glm::vec3& start = nodeAnim->scalings[scalingIndex];
		const glm::vec3& end = nodeAnim->scalings[nextScalingIndex];
		glm::vec3 delta = end - start;
		return start + factor * delta;
	}

	glm::quat SkeletalMesh::CalcInterpolatedRotation(double animationTime, const engine::assets::NodeAnimation* nodeAnim)
	{
		// we need at least two values to interpolate...
		if (nodeAnim->numberOfRotations == 1) {
			return nodeAnim->rotations[0];
		}

		unsigned int rotationIndex = FindRotation(animationTime, nodeAnim);
		unsigned int nextRotationIndex = (rotationIndex + 1);
		assert(nextRotationIndex < nodeAnim->numberOfRotations);

		float deltaTime = (float)(nodeAnim->rotationTime[nextRotationIndex] - nodeAnim->rotationTime[rotationIndex]);
		float factor = (float)((animationTime - nodeAnim->rotationTime[rotationIndex]) / deltaTime);
		assert(factor >= 0.0f && factor <= 1.0f);

		const glm::quat& startRotation = nodeAnim->rotations[rotationIndex];
		const glm::quat& endRotation = nodeAnim->rotations[nextRotationIndex];

		return glm::normalize(glm::slerp(startRotation, endRotation, factor));
	}

	glm::vec3 SkeletalMesh::CalcInterpolatedPosition(double animationTime, const engine::assets::NodeAnimation* nodeAnim)
	{
		if (nodeAnim->numberOfPositions == 1) {
			return nodeAnim->positions[0];
		}

		unsigned int positionIndex = FindPosition(animationTime, nodeAnim);
		unsigned int nextPositionIndex = (positionIndex + 1);
		assert(nextPositionIndex < nodeAnim->numberOfPositions);

		float deltaTime = (float)(nodeAnim->positionTime[nextPositionIndex] - nodeAnim->positionTime[positionIndex]);
		float factor = (float)((animationTime - nodeAnim->positionTime[positionIndex]) / deltaTime);
		assert(factor >= 0.0f && factor <= 1.0f);

		const glm::vec3& start = nodeAnim->positions[positionIndex];
		const glm::vec3& end = nodeAnim->positions[nextPositionIndex];

		glm::vec3 delta = end - start;
		return start + factor * delta;
	}

	glm::vec3 SkeletalMesh::CalcBlendScaling(double animationTime, float blendTime, const engine::assets::NodeAnimation* nodeAnim, const engine::assets::NodeAnimation* nodeLastAnim)
	{
		if (!nodeLastAnim) {
			return nodeAnim->scalings[0];
		}

		unsigned int scalingIndex = 0;
		unsigned int lastScalingIndex = FindScaling(animationTime, nodeLastAnim);

		const glm::vec3& end = nodeAnim->scalings[scalingIndex];
		const glm::vec3& start = nodeLastAnim->scalings[lastScalingIndex];
		glm::vec3 delta = end - start;
		return start + blendTime * delta;
	}

	glm::quat SkeletalMesh::CalcBlendRotation(double animationTime, float blendTime, const engine::assets::NodeAnimation* nodeAnim, const engine::assets::NodeAnimation* nodeLastAnim)
	{
		if (!nodeLastAnim) {
			return nodeAnim->rotations[0];
		}

		unsigned int rotationIndex = 0;
		unsigned int lastRotationIndex = FindRotation(animationTime, nodeLastAnim);

		const glm::quat& endRotation = nodeAnim->rotations[rotationIndex];
		const glm::quat& startRotation = nodeLastAnim->rotations[lastRotationIndex];

		return glm::normalize(glm::slerp(startRotation, endRotation, blendTime));
	}

	glm::vec3 SkeletalMesh::CalcBlendPosition(double animationTime, float blendTime, const engine::assets::NodeAnimation* nodeAnim, const engine::assets::NodeAnimation* nodeLastAnim)
	{
		if (!nodeLastAnim) {
			return nodeAnim->positions[0];
		}

		unsigned int positionIndex = 0;
		unsigned int lastPositionIndex = FindPosition(animationTime, nodeLastAnim);

		const glm::vec3& end = nodeAnim->positions[positionIndex];
		const glm::vec3& start = nodeLastAnim->positions[lastPositionIndex];

		glm::vec3 delta = end - start;
		return start + blendTime * delta;
	}

	unsigned int SkeletalMesh::FindScaling(double animationTime, const engine::assets::NodeAnimation* nodeAnim)
	{
		assert(nodeAnim->numberOfScalings > 0);

		for (unsigned int i = 0; i < nodeAnim->numberOfScalings - 1; i++) {
			if (animationTime < nodeAnim->scaleTime[i + 1]) {
				return i;
			}
		}

		assert(0);

		return 0;
	}

	unsigned int SkeletalMesh::FindRotation(double animationTime, const engine::assets::NodeAnimation* nodeAnim)
	{
		assert(nodeAnim->numberOfRotations > 0);

		for (unsigned int i = 0; i < nodeAnim->numberOfRotations - 1; i++) {
			if (animationTime < nodeAnim->rotationTime[i + 1]) {
				return i;
			}
		}

		assert(0);

		return 0;
	}

	unsigned int SkeletalMesh::FindPosition(double animationTime, const engine::assets::NodeAnimation* nodeAnim)
	{
		for (unsigned int i = 0; i < nodeAnim->numberOfPositions - 1; i++) {
			if (animationTime < nodeAnim->positionTime[i + 1]) {
				return i;
			}
		}

		assert(0);

		return 0;
	}

	void SkeletalMesh::ProcessMeshOrientation(double xOffset) {
		xOffset *= 0.1f;	//MouseSensitivity
		yaw += xOffset;

		transform.RotateBy(glm::radians(xOffset), worldUP);

		UpdateMeshVectors();
	}

	void SkeletalMesh::UpdateMeshVectors() {
		glm::vec3 tmpFront(0);
		tmpFront.x = -cos(glm::radians(yaw));
		tmpFront.z = -sin(glm::radians(yaw));
		front = glm::normalize(tmpFront);
		right = glm::normalize(glm::cross(front, worldUP));
		up = glm::normalize(glm::cross(right, front));
	}

	glm::mat4 SkeletalMesh::GetOrientationMatrix()
	{		
		return glm::lookAt(transform.GetPosition(), transform.GetPosition() + front, up);
	}

	glm::vec3 SkeletalMesh::getFront() {
		return front;
	}

	glm::vec3 SkeletalMesh::getRight() {
		return right;
	}

}
