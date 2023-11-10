#pragma once

#include <glm/glm.hpp>

// Pad values for uniform buffer object offset!

struct DirectionalLight {
	glm::vec3 direction;
	float pad1;

	glm::vec3 ambient;
	float pad2;

	glm::vec3 diffuse;
	float pad3;
	
	glm::vec3 specular;
	float pad4;
};

struct PointLight {
	glm::vec3 position;
	float pad1;

	glm::vec3 ambient;
	float pad2;
	
	glm::vec3 diffuse;
	float pad3;

	glm::vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

struct SpotLight {
	glm::vec3 position;
	float pad1;
	
	glm::vec3 direction;
	float pad2;
	
	glm::vec3 ambient;
	float pad3;
	
	glm::vec3 diffuse;
	float pad4;
	
	glm::vec3 specular;

	float cutOff;
	float outerCutOff;

	float constant;
	float linear;
	float quadratic;
};