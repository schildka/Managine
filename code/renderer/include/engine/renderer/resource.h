#pragma once

#include <engine/renderer/worldObject.h>

namespace engine::renderer {
	/**
	* Resource class including world object and amount of resource available
	* @param meshObject World object possessing meshes and/or physics values
	* @param amount of resource available
	*/
	class Resource {
	public:
		Resource();

	public:
		WorldObject meshObject;

	private:
		unsigned int amount;

	};
}