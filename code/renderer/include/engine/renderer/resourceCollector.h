#pragma once

#include <engine/renderer/worldObject.h>
#include <engine/renderer/resource.h>

namespace engine::renderer {
	/**
	* Resource collector class including world object and resource it should collect from.
	* @param meshObject World object possessing meshes and/or physics values
	* @param resource Resource it should collect.
	*/
	class ResourceCollector {
	public:
		ResourceCollector();

		bool isAvailable();

		/**
		* Checks if resourceCollector is build near it's resource.
		* @param p world position
		* @return true if it's near by it's resource.
		*/
		bool isCloseToResource(glm::vec3 p);

		/**
		* Sets distance collector can be build from resource.
		* @param distance 
		*/
		void setAllowedDistance(float distance);

		void incrementNumberOfBuildings();

		/** Gather resource over time. */
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