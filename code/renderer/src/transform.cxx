#include <engine/renderer/transform.h>
#include <glm\gtc\matrix_transform.hpp>

#define _USE_MATH_DEFINES
#include <math.h>


namespace engine::renderer {

	Transform::Transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) {
		this->position = this->rotation = glm::vec3();
		this->scale = glm::vec3(1.0f, 1.0f, 1.0f);
		matrix = translationMatrix = rotationMatrix = scalingMatrix = glm::mat4(1.0f);

		TranslateTo(position);
		RotateTo(rotation);
		ScaleTo(scale);
	}

	Transform::~Transform() {
	}

	void Transform::update() {
	
		TranslateTo(GetModelPosition());
		
		glm::vec3 scale = GetModelScale();
		
		/*
		glm::vec3 rot0 = glm::vec3(modelMatrix[0][0] / scale.x, modelMatrix[0][1] / scale.x, modelMatrix[0][2] / scale.x);
		glm::vec3 rot1 = glm::vec3(modelMatrix[1][0] / scale.y, modelMatrix[1][1] / scale.y, modelMatrix[1][2] / scale.y);
		glm::vec3 rot2 = glm::vec3(modelMatrix[2][0] / scale.z, modelMatrix[2][1] / scale.z, modelMatrix[2][2] / scale.z);

		float x = atan2f(rot1.z, rot2.z) - M_PI;
		float y = atan2f(-rot0.z, sqrtf(powf(rot1.z,2) + powf(rot2.z,2))) - M_PI/2;
		float z = atan2f(rot0.y, rot0.x) - M_PI;


		float x = 0;
		float y = 0;
		float z = 0;
		if (modelMatrix[0][2] == 1.0f)
		{
			x = -M_PI/2;
			y = atan2(-modelMatrix[1][0], -modelMatrix[2][0]);
			z = 0;

		}
		else if (modelMatrix[0][2] == -1.0f)
		{
			x = M_PI / 2;
			y = atan2(modelMatrix[1][0], modelMatrix[2][0]);
			z = 0;
		}
		else
		{
			x = asin(modelMatrix[0][2]);
			y = atan2(-modelMatrix[1][2] / cos(x), modelMatrix[2][2] / cos(x));
			z = atan2(-modelMatrix[0][1] / cos(x), modelMatrix[0][0] / cos(x));
		}


		RotateTo(glm::vec3(glm::degrees(x-M_PI), glm::degrees(y - M_PI / 2), glm::degrees(z - M_PI)));
		*/

		ScaleTo(scale);
	}

	glm::vec3 Transform::GetModelPosition() {
		return glm::vec3(modelMatrix[3][0], modelMatrix[3][1], modelMatrix[3][2]);
	}

	glm::vec3 Transform::GetModelScale() {
		return glm::vec3(glm::length(glm::vec3(modelMatrix[0][0], modelMatrix[0][1], modelMatrix[0][2])), glm::length(glm::vec3(modelMatrix[1][0], modelMatrix[1][1], modelMatrix[1][2])), glm::length(glm::vec3(modelMatrix[2][0], modelMatrix[2][1], modelMatrix[2][2])));
	}

	glm::vec3 Transform::GetPosition() {
		return position;
	}
	glm::vec3 Transform::GetRotation() {
		return rotation;
	}
	glm::vec3 Transform::GetScale() {
		return scale;
	}
	glm::mat4 Transform::GetMatrix() {
		return matrix;
	}
	void Transform::SetMatrix(glm::mat4 m) {
		matrix = m;
	}

	void Transform::TranslateTo(const glm::vec3& position) {
		TranslateBy(position - this->position);
	}

	void Transform::ScaleTo(const glm::vec3& scale) {
		ScaleBy(scale / this->scale);
	}

	void Transform::RotateTo(const glm::vec3& rotation) {
		RotateBy(rotation.x - this->rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		RotateBy(rotation.y - this->rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		RotateBy(rotation.z - this->rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	}

	void Transform::TranslateBy(const glm::vec3& distance) {
		translationMatrix = glm::translate(translationMatrix, distance);
		matrix = translationMatrix * rotationMatrix * scalingMatrix;
		this->position += distance;
	}

	void Transform::ScaleBy(const glm::vec3& scale) {
		scalingMatrix = glm::scale(scalingMatrix, scale);
		matrix = translationMatrix * rotationMatrix * scalingMatrix;
		this->scale *= scale;
	}

	void Transform::RotateBy(float angle, const glm::vec3& axis) {
		rotationMatrix = glm::rotate(rotationMatrix, angle, axis);
		matrix = translationMatrix * rotationMatrix * scalingMatrix;
		this->rotation += axis * angle;

		this->rotation.x = fmod(this->rotation.x + 360.0f, 360.0f);
		this->rotation.y = fmod(this->rotation.y + 360.0f, 360.0f);
		this->rotation.z = fmod(this->rotation.z + 360.0f, 360.0f);
	}
}