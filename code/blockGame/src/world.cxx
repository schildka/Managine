#include "world.h"

#include <engine/assets/text.h>
#include <engine/assets/model.h>

namespace blockGame {

	World::World(engine::Engine* engine) {
		//Lights
		engine->renderer->getLights()->CreateDirectionalLight(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.25f, 0.25f, 0.5f), glm::vec3(0.5f, 0.5f, 0.75f), glm::vec3(0.5f, 0.5f, 0.75f));
		engine->renderer->getLights()->calculateCascadeShadowMapping = false;	// No need for shadowMapping in this Scene

		//engine->renderer->getLights()->CreatePointLight(glm::vec3(-43.0f, 2.0f, 40.0f), 0.5f, 0.009f, 0.0032f, glm::vec3(0.0f, 0.25f, 0.25f), glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 1.0f));
		//engine->renderer->getLights()->CreatePointLight(glm::vec3(-2.0f, 0.25f, -2.0f), 0.0f, 0.09f, 0.032f, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		//engine->renderer->getLights()->CreatePointLight(glm::vec3(-2.0f, 0.25f, -2.0f), 0.0f, 0.09f, 0.032f, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		//engine->renderer->getLights()->CreateSpotLight(glm::vec3(10.0f, 10.0f, 10.0f), glm::vec3(-10.f, -10.0f, -10.0f), glm::cos(glm::radians(90.0f)), glm::cos(glm::radians(90.0f)), 0.0f, 0.09f, 0.032f, glm::vec3(0.1f, 0.0f, 0.1f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 1.0f));


		//SkyBox
		std::vector<std::string> faces
		{
			"textures/interstellar_skybox/right.png",
			"textures/interstellar_skybox/left.png",

			"textures/interstellar_skybox/top.png",
			"textures/interstellar_skybox/bot.png",

			"textures/interstellar_skybox/front.png",
			"textures/interstellar_skybox/back.png"
		};

		skyBox = new engine::renderer::SkyBox(faces);
		//skyBox->sun = engine->renderer->getLights()->GetDirectionalLightP();

		auto fragmentSource = engine::assets::Text("shader/skybox/fragmentSkyBoxShader.fs");
		auto vertexSource = engine::assets::Text("shader/skybox/vertexSkyBoxShader.vs");
		skyBox->shader = new engine::renderer::ShaderProgram(
			engine::renderer::Shader(vertexSource.get(), GL_VERTEX_SHADER),
			engine::renderer::Shader(fragmentSource.get(), GL_FRAGMENT_SHADER));

		engine->renderer->registerRenderable(skyBox);


		//Start Block
		vertexSource = engine::assets::Text("shader/object/vertexShader.vs");
		fragmentSource = engine::assets::Text("shader/object/fragmentMaterialShader.fs");

		material.ambient = glm::vec3(2, 2, 2);
		material.diffuse = glm::vec3(2, 2, 2);
		material.specular = glm::vec3(2, 2, 2);
		material.shininess = 0.16;

		start = new engine::renderer::WorldObject();
		start->addObject(new engine::renderer::Cube());
		start->getObject(0)->GetTransform()->ScaleBy(glm::vec3(12, 4, 12));
		start->getObject(0)->shader = new engine::renderer::ShaderProgram(
			engine::renderer::Shader(vertexSource.get(), GL_VERTEX_SHADER),
			engine::renderer::Shader(fragmentSource.get(), GL_FRAGMENT_SHADER));
		start->getObject(0)->setMaterial(material);
		start->registerAllRenderables(engine->renderer);
		start->initPhysics(glm::vec3(0,0,0), rp3d::Vector3(6, 2, 6), engine->physics);


		//Blocks
		engine::assets::Model blockModel("models/Cube.fbx");
		fragmentSource = engine::assets::Text("shader/object/fragmentMaterialGlowShader.fs");
		
		unsigned int length = sizeof(pos) / sizeof(pos[0]);
		for (unsigned int i = 0; i < length; i++)
		{
			blocks.push_back(engine::renderer::WorldObject());

			for (unsigned int j = 0; j < blockModel.numberOfMeshParts(); j++)
			{
				engine::renderer::Mesh* mesh = new engine::renderer::Mesh(pos[i], rot[j], glm::vec3(sca[j]));

				mesh->shader = new engine::renderer::ShaderProgram(
					engine::renderer::Shader(vertexSource.get(), GL_VERTEX_SHADER),
					engine::renderer::Shader(fragmentSource.get(), GL_FRAGMENT_SHADER));
				mesh->setBuffers(blockModel.vertices(j), blockModel.indices(j), blockModel.indices(j).size());
				mesh->setTextures(blockModel.textures(j));
				mesh->setMaterial(blockModel.material(j));
				blocks[i].addObject(mesh);
			}

			engine::assets::Material* m = blocks[i].getObject(1)->getMaterial();
			m->ambient = glm::vec3(10.0, 0.0, 0.0);

			blocks[i].registerAllRenderables(engine->renderer);
			blocks[i].initPhysics(pos[i]/2.0f, rp3d::Vector3(2, 2, 2), engine->physics);
		}

		blocks[5].getObject(1)->getMaterial()->ambient = glm::vec3(0.0, 0.0, 10.0);
		blocks[5].getObject(1)->setMaterialUniforms();
		blocks[5].isActive = true;
		blocks[6].getObject(1)->getMaterial()->ambient = glm::vec3(0.0, 0.0, 10.0);
		blocks[6].getObject(1)->setMaterialUniforms();
		blocks[6].isActive = true;
		blocks[16].getObject(1)->getMaterial()->ambient = glm::vec3(0.0, 0.0, 10.0);
		blocks[16].getObject(1)->setMaterialUniforms();
		blocks[16].isActive = true;
		blocks[17].getObject(1)->getMaterial()->ambient = glm::vec3(0.0, 0.0, 10.0);
		blocks[17].getObject(1)->setMaterialUniforms();
		blocks[17].isActive = true;

		blocks[34].getObject(1)->getMaterial()->ambient = glm::vec3(0.0, 0.0, 10.0);
		blocks[34].getObject(1)->setMaterialUniforms();
		blocks[34].isActive = true;

		engine->registerActor(this);
	}

	World::~World() {
		delete skyBox;
		delete start;
	}

	void World::update(engine::context::Context* context, engine::renderer::Renderer* renderer, engine::physics::Physics* physics) {
		skyBox->GetTransform()->RotateBy(glm::radians(skyBox->speed*3), glm::vec3(0, 1, 0));
	}

}