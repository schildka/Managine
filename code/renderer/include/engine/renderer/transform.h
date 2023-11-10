#pragma once

#include <glm\glm.hpp>

namespace engine::renderer {
	/**
	* Transform component holding position, rotation and scale informations in 3D.
	* @param matrix Holds model matrix including translation, rotation and scaling.
	*/
	class Transform {
	private:
		glm::vec3 position, rotation, scale;
		glm::mat4 translationMatrix, rotationMatrix, scalingMatrix;
		glm::mat4 matrix;

	public:
		glm::mat4 modelMatrix;

		Transform(glm::vec3 position = glm::vec3(), glm::vec3 rotation = glm::vec3(), glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f));
		~Transform();

		void update();
		glm::vec3 GetModelPosition();
		glm::vec3 GetModelScale();
		glm::vec3 GetPosition();
		glm::vec3 GetRotation();
		glm::vec3 GetScale();
		glm::mat4 GetMatrix();
		void SetMatrix(glm::mat4 m);

		void TranslateTo(const glm::vec3& position);
		void ScaleTo(const glm::vec3& scale);
		void RotateTo(const glm::vec3& rotation);

		void TranslateBy(const glm::vec3& distance);
		void ScaleBy(const glm::vec3& scale);
		void RotateBy(float angle, const glm::vec3& axis);
	};
}