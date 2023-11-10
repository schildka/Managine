#pragma once

#include <engine/context/context.h>

#include <engine/renderer/shader.h>
#include <engine/renderer/camera.h>
#include <engine/renderer/lights.h>

namespace engine::renderer {
	/**
	* Abstract Renderable class
	* @param shader Shaderporgram used by the renderable
	*/
	class Renderable {
	public:
		virtual void render(Camera* camera, Lights* lights, engine::context::Context* context) = 0;
		virtual void renderShadow(ShaderProgram* shader) = 0;

		virtual void setImmutableUniforms(Camera* camera, Lights* lights) = 0;

	public:
		ShaderProgram* shader = nullptr;
	};
}