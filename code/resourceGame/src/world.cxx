#include "world.h"

#include <engine/assets/text.h>
#include <engine/assets/model.h>

namespace resourceGame {

	World::World(engine::Engine* engine) {
		//Lights
		engine->renderer->getLights()->CreateDirectionalLight(glm::vec3(-0.5f, -0.25f, 0.5f), glm::vec3(0.25f, 0.25f, 0.25f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.5f, 0.5f, 0.5f));

		//engine->renderer->getLights()->CreatePointLight(glm::vec3(-43.0f, 2.0f, 40.0f), 0.5f, 0.009f, 0.0032f, glm::vec3(0.0f, 0.25f, 0.25f), glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 1.0f));
		//engine->renderer->getLights()->CreatePointLight(glm::vec3(-2.0f, 0.25f, -2.0f), 0.0f, 0.09f, 0.032f, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		//engine->renderer->getLights()->CreatePointLight(glm::vec3(-2.0f, 0.25f, -2.0f), 0.0f, 0.09f, 0.032f, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		//engine->renderer->getLights()->CreateSpotLight(glm::vec3(15.0f, 2.0f, 15.0f), glm::vec3(0.1f, -1.0f, 0.1f), glm::cos(glm::radians(60.0f)), glm::cos(glm::radians(60.0f)), 0.0f, 0.09f, 0.032f, glm::vec3(0.1f, 0.0f, 0.1f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 1.0f));


		//SkyBox
		std::vector<std::string> faces
		{
			"textures/skybox/right.png",
			"textures/skybox/left.png",
			"textures/skybox/top.png",
			"textures/skybox/bottom.png",
			"textures/skybox/back.png",
			"textures/skybox/front.png"
		};

		skyBox = new engine::renderer::SkyBox(faces);
		skyBox->sun = engine->renderer->getLights()->GetDirectionalLightP();

		auto fragmentSource = engine::assets::Text("shader/skybox/fragmentSkyBoxShader.fs");
		auto vertexSource = engine::assets::Text("shader/skybox/vertexSkyBoxShader.vs");
		skyBox->shader = new engine::renderer::ShaderProgram(
			engine::renderer::Shader(vertexSource.get(), GL_VERTEX_SHADER),
			engine::renderer::Shader(fragmentSource.get(), GL_FRAGMENT_SHADER));

		engine->renderer->registerRenderable(skyBox);


		//Terrain
		terrain = new engine::renderer::Terrain(15, 1, glm::vec3(0.0f, -7.5f, 0.0f));
		//terrain->generateHeightMap("terrain/", "Test.png", 257, 257, 237);

		fragmentSource = engine::assets::Text("shader/terrain/fragmentTerrainShader.fs");
		vertexSource = engine::assets::Text("shader/terrain/vertexTerrainShader.vs");

		terrain->shader = new engine::renderer::ShaderProgram(
			engine::renderer::Shader(vertexSource.get(), GL_VERTEX_SHADER),
			engine::renderer::Shader(fragmentSource.get(), GL_FRAGMENT_SHADER));

		terrain->LoadTexture("textures/grass.jpg", 0);
		terrain->LoadTexture("textures/rock.jpg", 1);
		terrain->LoadTexture("textures/snow.jpg", 2);
		terrain->LoadHeightmap("terrain/island.png");


		terrain->rigidBody = engine->physics->createRigidBody(glm::vec3(0.0, 0.0, 0.0), false, false, 1.0, 1.0, rp3d::BodyType::STATIC);
		terrain->heightFieldShape = new rp3d::HeightFieldShape(terrain->heightmapDimensions.x, terrain->heightmapDimensions.y, rp3d::decimal(0), rp3d::decimal(15), terrain->heightValues.data(), rp3d::HeightFieldShape::HeightDataType::HEIGHT_FLOAT_TYPE);
		terrain->rigidBody->rigidBody->addCollisionShape(terrain->heightFieldShape, terrain->rigidBody->rigidBody->getTransform(), 1.0);

		// Get the current material of the body 
		rp3d::Material& material = terrain->rigidBody->rigidBody->getMaterial();
		material.setBounciness(rp3d::decimal(0.0));
		material.setFrictionCoefficient(1.0f);
		material.setRollingResistance(1.0f);

		terrain->rigidBody->rigidBody->setMaterial(material);

		engine->renderer->registerRenderable(terrain);


		//Water
		water = new engine::renderer::Water(engine->context->getWidth(), engine->context->getHeight(), glm::vec2(terrain->heightmapDimensions.x*2, terrain->heightmapDimensions.y*2), glm::vec3(-terrain->heightmapDimensions.x, -3.5, -terrain->heightmapDimensions.y));

		fragmentSource = engine::assets::Text("shader/water/fragmentWaterShader.fs");
		vertexSource = engine::assets::Text("shader/water/vertexWaterShader.vs");
		water->shader = new engine::renderer::ShaderProgram(
			engine::renderer::Shader(vertexSource.get(), GL_VERTEX_SHADER),
			engine::renderer::Shader(fragmentSource.get(), GL_FRAGMENT_SHADER));

		engine::assets::Texture waterDudv = engine::assets::Texture("textures/water/dudv.png");
		engine::assets::Texture waterNormal = engine::assets::Texture("textures/water/normal.png");
		water->SetDUDV(waterDudv);
		water->SetNormal(waterNormal);
		water->RegisterTexture();

		water->setImmutableUniforms(engine->renderer->getCamera(), engine->renderer->getLights());

		engine->renderer->registerWater(water);


		// Numbers
		fragmentSource = engine::assets::Text("shader/object/fragmentMaterialShader.fs");
		vertexSource = engine::assets::Text("shader/object/vertexShader.vs");

		engine::assets::Model one("models/Numbers/one.fbx");
		signs[0] = engine::renderer::WorldObject();
		signs[0].initialize(&one, glm::vec3(0, -3.25, 0), glm::vec3(0, 0, 0), glm::vec3(-5, 5, 5), vertexSource, fragmentSource);
		signs[0].registerAllRenderables(engine->renderer);

		engine::assets::Model two("models/Numbers/two.fbx");
		signs[1] = engine::renderer::WorldObject();
		signs[1].initialize(&two, glm::vec3(94, -2.5, 72), glm::vec3(glm::radians(-90.0f), 0, glm::radians(90.0f)), glm::vec3(-5, 5, 5), vertexSource, fragmentSource);
		signs[1].registerAllRenderables(engine->renderer);

		engine::assets::Model three("models/Numbers/three.fbx");
		signs[2] = engine::renderer::WorldObject();
		signs[2].initialize(&three, glm::vec3(30, -3.25, 105), glm::vec3(0, 0, 0), glm::vec3(-5, 5, 5), vertexSource, fragmentSource);
		signs[2].registerAllRenderables(engine->renderer);

		engine::assets::Model four("models/Numbers/four.fbx");
		signs[3] = engine::renderer::WorldObject();
		signs[3].initialize(&four, glm::vec3(-40.5, -3.0, 40), glm::vec3(0, 0, 0), glm::vec3(-5, 5, 5), vertexSource, fragmentSource);
		signs[3].registerAllRenderables(engine->renderer);

		engine::assets::Model five("models/Numbers/five.fbx");
		signs[4] = engine::renderer::WorldObject();
		signs[4].initialize(&five, glm::vec3(22, -3, 85.25), glm::vec3(glm::radians(-45.0f), 0, 0), glm::vec3(-7, 7, 7), vertexSource, fragmentSource);
		signs[4].registerAllRenderables(engine->renderer);
		

		engine->registerActor(this);
	}

	World::~World() {
		delete skyBox;
		delete terrain;
		delete water;
	}

	void World::update(engine::context::Context* context, engine::renderer::Renderer* renderer, engine::physics::Physics* physics) {
		water->moveFactor += context->getDeltaTime() * water->moveSpeed;
		water->moveFactor = fmod(water->moveFactor, 1.0f);

		skyBox->GetTransform()->RotateBy(glm::radians(skyBox->speed), glm::vec3(0, 1, 0));
		skyBox->sun->direction.x = skyBox->sun->direction.x * glm::cos(glm::radians(skyBox->speed)) + skyBox->sun->direction.z * glm::sin(glm::radians(skyBox->speed));
		skyBox->sun->direction.z = -skyBox->sun->direction.x * glm::sin(glm::radians(skyBox->speed)) + skyBox->sun->direction.z * glm::cos(glm::radians(skyBox->speed));
	}

}