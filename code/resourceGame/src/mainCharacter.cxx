#include "mainCharacter.h"

#include <engine/assets/text.h>

namespace resourceGame {

	MainCharacter::MainCharacter(engine::Engine* engine, MainCamera* camera)  {
		skeletalMesh = new engine::renderer::SkeletalMesh(glm::vec3(), glm::vec3(), glm::vec3(0.025f, 0.025f, 0.025f));
		
		auto fragmentSource = engine::assets::Text("shader/skeletalMesh/fragmentSkeletalMeshShader.fs");
		auto vertexSource = engine::assets::Text("shader/skeletalMesh/vertexSkeletalMeshShader.vs");
		skeletalMesh->shader = new engine::renderer::ShaderProgram(
			engine::renderer::Shader(vertexSource.get(), GL_VERTEX_SHADER),
			engine::renderer::Shader(fragmentSource.get(), GL_FRAGMENT_SHADER));
		
		engine::assets::Model ourModel("models/Player/Player.fbx");
		skeletalMesh->setBuffers(ourModel.vertices(0), ourModel.bones(0), ourModel.bones(0).size(), ourModel.indices(0), ourModel.indices(0).size());
		skeletalMesh->setTextures(ourModel.textures(0));
		skeletalMesh->setBoneInformations(ourModel.boneInfo(), ourModel.boneMapping(), ourModel.nodes());
		skeletalMesh->setAnimations(ourModel.animations());
		skeletalMesh->setImmutableUniforms(engine->renderer->getCamera(), engine->renderer->getLights());

		//Physics
		skeletalMesh->rigidBody = engine->physics->createRigidBody(glm::vec3(0.0, 2.0, 0.0), false, false, 0.99, 10.0, rp3d::BodyType::DYNAMIC);
		skeletalMesh->rigidBody->rigidBody->setCenterOfMassLocal(rp3d::Vector3(0.0f, 0.0f, 0.0f));
		skeletalMesh->capsuleShape = new rp3d::CapsuleShape(1.0, 2.0);
		skeletalMesh->sphereShape = new rp3d::SphereShape(0.5);
		skeletalMesh->rigidBody->rigidBody->addCollisionShape(skeletalMesh->capsuleShape, skeletalMesh->rigidBody->rigidBody->getTransform(), 10.0);

		// Get the current material of the body 
		rp3d::Material& material = skeletalMesh->rigidBody->rigidBody->getMaterial();
		material.setBounciness(rp3d::decimal(0.0));
		material.setFrictionCoefficient(1.0f);
		material.setRollingResistance(std::numeric_limits<float>::max());
		skeletalMesh->rigidBody->rigidBody->setMaterial(material);

		//Shared Uniforms
		skeletalMesh->shader->bufferSharedLightsUniforms(engine->renderer->getLights()->hasDirectionalLight, engine->renderer->getLights()->GetNumberOfPointLights(), engine->renderer->getLights()->GetNumberOfSpotLights());
		skeletalMesh->shader->bufferSharedMatricesUniforms();
		skeletalMesh->shader->bufferSharedVectorsUniforms();
		skeletalMesh->shader->bufferSharedShadowsUniforms(engine->renderer->getLights()->GetNumberOfSpotLights());
		engine->renderer->registerSharedUniformsRenderable(skeletalMesh);

		//glfwSetWindowUserPointer(engine->context->getWindow(), this);
		//engine->context->registerMouseCallback([](GLFWwindow* window, double xpos, double ypos) {static_cast<MainCharacter*>(glfwGetWindowUserPointer(window))->mouse_callback(window, xpos, ypos); });

		engine->registerActor(this);

		mainCamera = camera;
		mainCamera->getCamera()->Target = skeletalMesh->GetTransform()->GetPosition();

		initializeQuests();

		initializeSounds();
	}

	void MainCharacter::update(engine::context::Context* context, engine::renderer::Renderer* renderer, engine::physics::Physics* physics) {
		animationIndex = handleMovement(context);

		skeletalMesh->animate(animationIndex, skeletalMesh->shader);

		handleBuildings(context, renderer, physics);

		handleCollectables(renderer);

		handleUI(context, renderer, physics);
	}

	void MainCharacter::initializeSounds() {
		soundEngine = irrklang::createIrrKlangDevice();
		music = engine::assets::Sound("sounds/music.wav", soundEngine);
		craft = engine::assets::Sound("sounds/craft.wav", soundEngine);
		buildError = engine::assets::Sound("sounds/craftError.wav", soundEngine);
		findItem = engine::assets::Sound("sounds/findItem.wav", soundEngine);
		waterStep = engine::assets::Sound("sounds/waterStep.wav", soundEngine);
		spawn = engine::assets::Sound("sounds/spawn.wav", soundEngine);
		lose = engine::assets::Sound("sounds/spawnLose.wav", soundEngine);

		soundEngine->play2D(spawn.getSound());
		soundEngine->play2D(music.getSound(), true);
	}

	void MainCharacter::initializeQuests() {
		quests.clear();
		quests.insert({ 1, Quest("Don't starve!") });
		quests.insert({ 2, Quest("Build a House to live in.") });
		quests.insert({ 3, Quest("Build a Port to trade.") });
		quests.insert({ 4, Quest("Congratulations you finished the Demo. \n Now do your thing...") });
	}

	void  MainCharacter::handleQuests() {
		if (quest != 0 && !quests[quest].done) {
			quests[quest].done = true;
			for(auto q : quests)
			{
				if (!q.second.done) {
					mainUI->getUI()->questLog = q.second.questText;
					break;
				}
			}
		}
	}

	unsigned int MainCharacter::handleMovement(engine::context::Context* context) {
		float gravity = -9.81;

		float x = skeletalMesh->GetTransform()->GetPosition().x;
		float y = skeletalMesh->GetTransform()->GetPosition().y;
		float z = skeletalMesh->GetTransform()->GetPosition().z;

		if (abs(x) > 250 || abs(z) > 250 || y < -4.5) {	// Check if in WorldBounds!
			skeletalMesh->rigidBody->rigidBody->setTransform(rp3d::Transform(rp3d::Vector3(0,0,0), rp3d::Quaternion::identity()));
			skeletalMesh->GetTransform()->TranslateTo(glm::vec3(0,0,0));
			x = 0;
			z = 0;
		}

		float height = terrainCollider->getHeightAt(x + 256, z + 256);
		if (animationIndex == 3) {
			if (height - 7.5 >= y)gravity = 0;
			skeletalMesh->rigidBody->rigidBody->setLinearVelocity(rp3d::Vector3(0, gravity, 0));
		}

		animationIndex = 3;

		if (context->getKey(GLFW_KEY_W) == GLFW_PRESS) {
			skeletalMesh->rigidBody->rigidBody->setLinearVelocity(rp3d::Vector3(skeletalMesh->getFront().x * movementSpeed * 1.25, gravity, -skeletalMesh->getFront().z * movementSpeed * 1.25));

			mainCamera->getCamera()->ProcessMouseMovement(skeletalMesh->rigidBody->position, skeletalMesh->getFront(), 0, 0);
			animationIndex = 4;
		}
		if (context->getKey(GLFW_KEY_A) == GLFW_PRESS) {
			skeletalMesh->rigidBody->rigidBody->setLinearVelocity(rp3d::Vector3(skeletalMesh->getRight().x * movementSpeed, gravity, -skeletalMesh->getRight().z * movementSpeed));

			mainCamera->getCamera()->ProcessMouseMovement(skeletalMesh->rigidBody->position, skeletalMesh->getFront(), 0, 0);
			animationIndex = 2;
		}
		if (context->getKey(GLFW_KEY_D) == GLFW_PRESS) {
			skeletalMesh->rigidBody->rigidBody->setLinearVelocity(rp3d::Vector3(-skeletalMesh->getRight().x * movementSpeed, gravity, skeletalMesh->getRight().z * movementSpeed));

			mainCamera->getCamera()->ProcessMouseMovement(skeletalMesh->rigidBody->position, skeletalMesh->getFront(), 0, 0);
			animationIndex = 1;
		}
		if (context->getKey(GLFW_KEY_S) == GLFW_PRESS) {
			skeletalMesh->rigidBody->rigidBody->setLinearVelocity(rp3d::Vector3(-skeletalMesh->getFront().x * movementSpeed, gravity, skeletalMesh->getFront().z * movementSpeed));

			mainCamera->getCamera()->ProcessMouseMovement(skeletalMesh->rigidBody->position, skeletalMesh->getFront(), 0, 0);
			animationIndex = 0;
		}
		
		if (context->IsMouseMoving()) {
			handleMouse(context->getMouseOffset());
			context->setMouseIsMoving(false);
		}

		if (animationIndex != 3 && y <= -3.5 && stepTrack == nullptr) {
			stepTrack = soundEngine->play2D(waterStep.getSound(), true, false, true);
		}
		else if (stepTrack != nullptr && (animationIndex == 3 || y > -3.5)) {
			stepTrack->stop();
			stepTrack->drop();
			stepTrack = nullptr;
		}

		return animationIndex;
	}

	void MainCharacter::handleMouse(glm::vec2 offset)
	{
		skeletalMesh->ProcessMeshOrientation(offset.x);

		mainCamera->getCamera()->ProcessMouseMovement(skeletalMesh->rigidBody->position, skeletalMesh->getFront(), offset.x, offset.y);
	}

	void MainCharacter::handleBuildings(engine::context::Context* context, engine::renderer::Renderer* renderer, engine::physics::Physics* physics) {
		
		switch (buildState)
		{
		case Idle:
			if (!isReleased && context->getKey(key) == GLFW_RELEASE) {
				isReleased = true;
			}

			if (context->getKey(GLFW_KEY_1) == GLFW_PRESS && isReleased) {
				buildings->addObject(buildings->house);
				creationObject = buildings->getLastConstructiveObject();
				
				isReleased = false;
				key = GLFW_KEY_1;
				quest = 2;

				buildValues = buildings->buildResourceMap.find(1)->second;

				buildState = Prepare;
			}

			if (context->getKey(GLFW_KEY_2) == GLFW_PRESS && isReleased) {
				buildings->addObject(&buildings->stone->meshObject);
				creationObject = buildings->getLastConstructiveObject();
				collector = buildings->stone;

				isReleased = false;
				key = GLFW_KEY_2;

				buildValues = buildings->buildResourceMap.find(2)->second;

				buildState = PrepareCollector;
			}

			if (context->getKey(GLFW_KEY_3) == GLFW_PRESS && isReleased) {
				buildings->addObject(&buildings->mine->meshObject);
				creationObject = buildings->getLastConstructiveObject();
				collector = buildings->mine;

				isReleased = false;
				key = GLFW_KEY_3;

				buildValues = buildings->buildResourceMap.find(3)->second;

				buildState = PrepareCollector;
			}

			if (context->getKey(GLFW_KEY_4) == GLFW_PRESS && isReleased) {
				buildings->addObject(&buildings->wood->meshObject);
				creationObject = buildings->getLastConstructiveObject();
				collector = buildings->wood;

				isReleased = false;
				key = GLFW_KEY_4;

				buildValues = buildings->buildResourceMap.find(4)->second;

				buildState = PrepareCollector;
			}

			if (context->getKey(GLFW_KEY_5) == GLFW_PRESS && isReleased) {
				buildings->addObject(&buildings->farm->meshObject);
				creationObject = buildings->getLastConstructiveObject();
				collector = buildings->farm;

				isReleased = false;
				key = GLFW_KEY_5;
				quest = 1;
				
				buildValues = buildings->buildResourceMap.find(5)->second;

				buildState = PrepareCollector;
			}

			if (context->getKey(GLFW_KEY_6) == GLFW_PRESS && isReleased) {
				buildings->addObject(buildings->port);
				creationObject = buildings->getLastConstructiveObject();

				isReleased = false;
				key = GLFW_KEY_6;
				quest = 3;

				buildValues = buildings->buildResourceMap.find(6)->second;

				buildState = Prepare;
			}

			break;
		case Prepare:
			
			creationObject->registerRenderables(renderer, skeletalMesh->GetTransform());

			mainUI->values = buildValues;
			mainUI->setState(engine::ui::BuildMenu);

			buildState = Building;
			
			break;

		case PrepareCollector:
			if (!collector->isAvailable()) {
				collector = nullptr;
				creationObject = nullptr;
				buildState = Idle;
			}
			else {
				creationObject->registerRenderables(renderer, skeletalMesh->GetTransform());

				mainUI->values = buildValues;
				mainUI->setState(engine::ui::BuildMenu);

				buildState = BuildingCollector;
			}

			break;

		case Building:
			if (!isReleased && context->getKey(key) == GLFW_RELEASE) {
				isReleased = true;
			}

			if (context->getKey(key) == GLFW_PRESS && isReleased) {
				creationObject->deRegisterRenderables(renderer);
				
				creationObject = nullptr;

				buildings->deleteLastConstructiveObject();

				isReleased = false;
				quest = 0;

				mainUI->setState(engine::ui::ResourceMenu);
				buildState = Idle;
			}

			if (context->getMouseKey(GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
				bool canBeBuild = buildings->hasResource(buildValues) && buildings->canBuildHere(creationObject, terrainCollider, physics->getCollisionMap());

				if (canBeBuild) {
					glm::vec3 worldPosition = creationObject->getObject(0)->GetTransform()->GetModelPosition();
					float height = terrainCollider->getHeightAt(worldPosition.x + 256, worldPosition.z + 256) - 7.5;
					buildings->build(physics, creationObject, height, buildValues);
					
					creationObject = nullptr;

					mainUI->setState(engine::ui::ResourceMenu);
					buildState = Idle;

					handleQuests();

					soundEngine->play2D(craft.getSound());
				}
				else {
					soundEngine->play2D(buildError.getSound());
				}
				//ToDo Auditive feedback!
			}
			break;

		case BuildingCollector:
			if (!isReleased && context->getKey(key) == GLFW_RELEASE) {
				isReleased = true;
			}

			if (context->getKey(key) == GLFW_PRESS && isReleased) {
				creationObject->deRegisterRenderables(renderer);

				creationObject = nullptr;
				collector = nullptr;

				buildings->deleteLastConstructiveObject();

				isReleased = false;
				quest = 0;

				mainUI->setState(engine::ui::ResourceMenu);
				buildState = Idle;
			}

			if (context->getMouseKey(GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
				bool canBeBuild = buildings->hasResource(buildValues) && buildings->canBuildHere(creationObject, terrainCollider, physics->getCollisionMap()) && buildings->canBuildCollector(collector, creationObject->getObject(0)->GetTransform()->GetModelPosition());

				if (canBeBuild) {
					glm::vec3 worldPosition = creationObject->getObject(0)->GetTransform()->GetModelPosition();
					float height = terrainCollider->getHeightAt(worldPosition.x + 256, worldPosition.z + 256) - 7.5;
					buildings->build(physics, creationObject, height, buildValues);

					creationObject = nullptr;
					
					collector->incrementNumberOfBuildings();
					collector = nullptr;

					mainUI->setState(engine::ui::ResourceMenu);
					buildState = Idle;

					handleQuests();
					
					soundEngine->play2D(craft.getSound());
				}
				else {
					soundEngine->play2D(buildError.getSound());
				}
				//ToDo Auditive feedback!
			}
			break;

		default:
			break;
		}
		
	}

	void MainCharacter::handleCollectables(engine::renderer::Renderer* renderer) {
		if (collectables->isCollectableInside(skeletalMesh->rigidBody->rigidBody, renderer)) {
			buildings->wood->updateResource(5);
			soundEngine->play2D(findItem.getSound());
		}
	}

	void MainCharacter::handleUI(engine::context::Context* context, engine::renderer::Renderer* renderer, engine::physics::Physics* physics) {

		if (buildings->farm->getAmountOfResource() <= 0) {
			restartQuestTest = "You starved... so now don't";
			mainUI->restartGame = true;
		}

		if (mainUI->restartGame) {

			for each (engine::renderer::WorldObject o in buildings->constructiveObjects)
			{
				o.resetPhysics(physics);
				for each (engine::renderer::Mesh* m in o.getObjects())
				{
					renderer->deRegisterRenderable(m);
				}
			}
			buildings->constructiveObjects.clear();

			collectables->reset(renderer);
			
			if (buildings->farm->getAmountOfResource() > 0) {
				restartQuestTest = "Don't starve!";
				soundEngine->play2D(spawn.getSound());
			}
			else {
				soundEngine->play2D(lose.getSound());
			}

			initializeQuests();
			mainUI->getUI()->questLog = restartQuestTest;

			skeletalMesh->rigidBody->rigidBody->setTransform(rp3d::Transform(rp3d::Vector3(0, 0, 0), rp3d::Quaternion::identity()));
			skeletalMesh->GetTransform()->TranslateTo(glm::vec3(0, 0, 0));

			buildings->farm->reset(25);
			buildings->mine->reset(0);
			buildings->stone->reset(0);
			buildings->wood->reset(5);

			for each (engine::renderer::Mesh* m in buildings->farm->meshObject.getObjects())
			{
				renderer->deRegisterRenderable(m);
			}

			for each (engine::renderer::Mesh* m in buildings->mine->meshObject.getObjects())
			{
				renderer->deRegisterRenderable(m);
			}

			for each (engine::renderer::Mesh* m in buildings->stone->meshObject.getObjects())
			{
				renderer->deRegisterRenderable(m);
			}

			for each (engine::renderer::Mesh* m in buildings->wood->meshObject.getObjects())
			{
				renderer->deRegisterRenderable(m);
			}
			mainUI->restartGame = false;
		}
	}

}