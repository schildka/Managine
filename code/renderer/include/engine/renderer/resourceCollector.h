#pragma once

#include <engine/renderer/worldObject.h>
#include <engine/renderer/resource.h>

namespace engine::renderer {
	class ResourceCollector {
	public:
		ResourceCollector();

		bool isAvailable();

		bool isCloseToResource(glm::vec3 p);

		void setAllowedDistance(float distance);

		void incrementNumberOfBuildings();

		void gatherResource();

		void updateResource(unsigned int r);

		unsigned int getAmountOfResource();

		void reset(unsigned int v);

	public:
		WorldObject meshObject;
		Resource* resource = nullptr;

	private:
		float allowedDistance = 10;

		const unsigned int maxBuildings = 10;
		unsigned int numberOfBuildings = 0;

		unsigned int amountOfResource = 0;

	};
}