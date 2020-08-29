#pragma once

#include <engine/renderer/worldObject.h>

namespace engine::renderer {
	class Resource {
	public:
		Resource();

	public:
		WorldObject meshObject;

	private:
		unsigned int amount;

	};
}