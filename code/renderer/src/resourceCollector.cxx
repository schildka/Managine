#include <engine/renderer/resourceCollector.h>

namespace engine::renderer {

	ResourceCollector::ResourceCollector() {
		meshObject = WorldObject();
	}

	bool ResourceCollector::isAvailable() {
		if (numberOfBuildings == maxBuildings)return false;
		else return true;
	}

	bool ResourceCollector::isCloseToResource(glm::vec3 p) {
		if (resource == nullptr || glm::distance(p, resource->meshObject.getObject(0)->GetTransform()->GetPosition()) < allowedDistance) {
			return true;
		}
		else {
			return false;
		}
	}

	void ResourceCollector::setAllowedDistance(float distance) {
		allowedDistance = distance;
	}

	void ResourceCollector::incrementNumberOfBuildings() {
		numberOfBuildings++;
	}

	void ResourceCollector::gatherResource() {
		amountOfResource += 1 * numberOfBuildings;
	}

	void ResourceCollector::updateResource(unsigned int r) {
		amountOfResource += r;
	}

	unsigned int ResourceCollector::getAmountOfResource() {
		return amountOfResource;
	}

	void ResourceCollector::reset(unsigned int v) {
		amountOfResource = v;
		numberOfBuildings = 0;
	}

}