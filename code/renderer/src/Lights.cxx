#include<engine/renderer/lights.h>

namespace engine::renderer {

	DirectionalLight Lights::CreateDirectionalLight(glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) {

		DirectionalLight dirLight;

		dirLight.direction = direction;

		dirLight.ambient = ambient;
		dirLight.diffuse = diffuse;
		dirLight.specular = specular;

		directionalLight = dirLight;

		hasDirectionalLight = true;
		calculateCascadeShadowMapping = true;

		return dirLight;
	}

	PointLight Lights::CreatePointLight(glm::vec3 position, float constant, float linear, float quadratic, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) {

		PointLight pointLight;

		pointLight.position = position;
		pointLight.ambient = ambient;
		pointLight.diffuse = diffuse;
		pointLight.specular = specular;
		pointLight.constant = constant;
		pointLight.linear = linear;
		pointLight.quadratic = quadratic;

		pointLights.push_back(pointLight);
		
		shadow->createPointDepthMap(pointLights.size() - 1);

		return pointLight;
	}

	SpotLight Lights::CreateSpotLight(glm::vec3 position, glm::vec3 direction, float cutOff, float outerCutOff, float constant, float linear, float quadratic, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) {

		SpotLight spotLight;

		spotLight.position = position;
		spotLight.direction = direction;
		spotLight.ambient = ambient;
		spotLight.diffuse = diffuse;
		spotLight.specular = specular;
		spotLight.cutOff = cutOff;
		spotLight.outerCutOff = outerCutOff;
		spotLight.constant = constant;
		spotLight.linear = linear;
		spotLight.quadratic = quadratic;

		spotLights.push_back(spotLight);

		shadow->createSpotDepthMap(spotLights.size() - 1);

		return spotLight;
	}

	glm::mat4 Lights::GetDirectionalLightView() {
		return glm::lookAt(glm::vec3(0.0, 0.0, 0.0), -directionalLight.direction, glm::vec3(0.0, 1.0, 0.0));
	}


	unsigned int Lights::GetNumberOfPointLights() {
		return pointLights.size();
	}
	unsigned int Lights::GetNumberOfSpotLights() {
		return spotLights.size();
	}

	std::vector<PointLight> Lights::GetPointLights() {
		return pointLights;
	}
	std::vector<SpotLight> Lights::GetSpotLights() {
		return spotLights;
	}

	DirectionalLight* Lights::GetDirectionalLightP() {
		return &directionalLight;
	}

	DirectionalLight Lights::GetDirectionalLight() {
		return directionalLight;
	}
	PointLight Lights::GetPointLight(unsigned int i) {
		return pointLights[i];
	}
	SpotLight Lights::GetSpotLight(unsigned int i) {
		return spotLights[i];
	}

	Shadow* Lights::GetShadow() {
		return shadow;
	}

}
