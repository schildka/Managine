#pragma once

#include <engine/renderer/shadow.h>

#include <engine/renderer/lightTypes.h>

#include <vector>

namespace engine::renderer {
	/**
	* Defines different light types and it effects on the world.
	* @param hasDirectionalLight holds information if a directionalLight is available in the scene.
	* @param calculateCascadeShadowMapping holds information if cascadeshadowmapping should be calculated if a directional light is available.
	*/
	class Lights {
	public:

		Lights(int width, int height) {
			shadow = new Shadow(width, height);
		};

		DirectionalLight CreateDirectionalLight(glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
		PointLight CreatePointLight(glm::vec3 position, float constant, float linear, float quadratic, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
		SpotLight CreateSpotLight(glm::vec3 position, glm::vec3 direction, float cutOff, float outerCutOff, float constant, float linear, float quadratic, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);

		glm::mat4 GetDirectionalLightView();

		unsigned int GetNumberOfPointLights();
		unsigned int GetNumberOfSpotLights();

		std::vector<PointLight> GetPointLights();
		std::vector<SpotLight> GetSpotLights();

		DirectionalLight* GetDirectionalLightP();
		DirectionalLight GetDirectionalLight();
		PointLight GetPointLight(unsigned int i);
		SpotLight GetSpotLight(unsigned int i);

		Shadow* GetShadow();

	public:

		// Todo make uniform...
		const unsigned int maxNumberOfPointLights = 5; // change manually if you need to.
		const unsigned int maxNumberOfSpotLights = 5; // change manually if you need to.

		bool hasDirectionalLight = false;
		bool calculateCascadeShadowMapping = false;

	private:

		DirectionalLight directionalLight;
		std::vector<PointLight> pointLights;
		std::vector<SpotLight> spotLights;

		Shadow* shadow = nullptr;
	};
}