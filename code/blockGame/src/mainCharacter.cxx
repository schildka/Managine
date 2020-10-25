#include "mainCharacter.h"

#include <engine/assets/text.h>

namespace blockGame {

	MainCharacter::MainCharacter(engine::Engine* engine, MainCamera* camera) {
		skeletalMesh = new engine::renderer::SkeletalMesh(glm::vec3(), glm::vec3(), glm::vec3(0.025f, 0.025f, 0.025f));

		auto fragmentSource = engine::assets::Text("shader/skeletalMesh/fragmentSkeletalMeshShader.fs");
		auto vertexSource = engine::assets::Text("shader/skeletalMesh/vertexSkeletalMeshShader.vs");
		skeletalMesh->shader = new engine::renderer::ShaderProgram(
			engine::renderer::Shader(vertexSource.get(), GL_VERTEX_SHADER),
			engine::renderer::Shader(fragmentSource.get(), GL_FRAGMENT_SHADER));

		engine::assets::Model ourModel("models/Player/Player2.fbx");
		skeletalMesh->setBuffers(ourModel.vertices(0), ourModel.bones(0), ourModel.bones(0).size(), ourModel.indices(0), ourModel.indices(0).size());
		skeletalMesh->setTextures(ourModel.textures(0));
		skeletalMesh->setBoneInformations(ourModel.boneInfo(), ourModel.boneMapping(), ourModel.nodes());
		skeletalMesh->setAnimations(ourModel.animations());
		skeletalMesh->setImmutableUniforms(engine->renderer->getCamera(), engine->renderer->getLights());

		//Physics
		skeletalMesh->rigidBody = engine->physics->createRigidBody(glm::vec3(0.0, 1.25, 0.0), false, true, 0.99, 10.0, rp3d::BodyType::DYNAMIC);
		skeletalMesh->rigidBody->rigidBody->setCenterOfMassLocal(rp3d::Vector3(0.0f, 0.0f, 0.0f));
		skeletalMesh->capsuleShape = new rp3d::CapsuleShape(0.25, 2.0);
		skeletalMesh->rigidBody->rigidBody->addCollisionShape(skeletalMesh->capsuleShape, skeletalMesh->rigidBody->rigidBody->getTransform(), 10.0);

		// Get the current material of the body 
		rp3d::Material& material = skeletalMesh->rigidBody->rigidBody->getMaterial();
		material.setBounciness(rp3d::decimal(0.1));
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
		float x = skeletalMesh->GetTransform()->GetPosition().x;
		float y = skeletalMesh->GetTransform()->GetPosition().y;
		float z = skeletalMesh->GetTransform()->GetPosition().z;

		animationIndex = handleMovement(context, glm::vec3(x,y,z));

		skeletalMesh->animate(animationIndex, skeletalMesh->shader);

		handleBlocks(rp3d::Vector3(x,0,z));

		handleUI(context, renderer, physics);
	}

	void MainCharacter::initializeSounds() {
		soundEngine = irrklang::createIrrKlangDevice();
		music = engine::assets::Sound("sounds/musicSpace.wav", soundEngine);
		error = engine::assets::Sound("sounds/craftError.wav", soundEngine);
		right = engine::assets::Sound("sounds/findItem.wav", soundEngine);
		spawn = engine::assets::Sound("sounds/spawn.wav", soundEngine);
		lose = engine::assets::Sound("sounds/spawnLose.wav", soundEngine);

		soundEngine->play2D(spawn.getSound());
		soundEngine->play2D(music.getSound(), true);
	}

	void MainCharacter::initializeQuests() {
		quests.clear();
		quests.insert({ 1, Quest("Turn all boxes to blue!") });
		quests.insert({ 2, Quest("Yay you did it :D") });
	}

	void  MainCharacter::handleQuests() {
		if (quest != 0 && !quests[quest].done) {
			quests[quest].done = true;
			for (auto q : quests)
			{
				if (!q.second.done) {
					mainUI->getUI()->questLog = q.second.questText;
					break;
				}
			}
		}
	}

	unsigned int MainCharacter::handleMovement(engine::context::Context* context, glm::vec3 position) {

		float yVel = skeletalMesh->rigidBody->rigidBody->getLinearVelocity().y;

		if (abs(position.x) > 250 || abs(position.z) > 250 || position.y < -4.5) {	// Check if in WorldBounds!
			skeletalMesh->rigidBody->rigidBody->setTransform(rp3d::Transform(rp3d::Vector3(0, 0, 0), rp3d::Quaternion::identity()));
			skeletalMesh->GetTransform()->TranslateTo(glm::vec3(0, 0, 0));
			position.x = 0;
			position.z = 0;
			soundEngine->play2D(lose.getSound());
		}
		
		animationIndex = 0;

		if (context->getKey(GLFW_KEY_W) == GLFW_PRESS) {
			skeletalMesh->rigidBody->rigidBody->setLinearVelocity(rp3d::Vector3(skeletalMesh->getFront().x * movementSpeed * 2.0, 0, -skeletalMesh->getFront().z * movementSpeed * 2.0));

			mainCamera->getCamera()->ProcessMouseMovement(skeletalMesh->rigidBody->position, skeletalMesh->getFront(), 0, 0);
			animationIndex = 4;
		}
		if (context->getKey(GLFW_KEY_A) == GLFW_PRESS) {
			skeletalMesh->rigidBody->rigidBody->setLinearVelocity(rp3d::Vector3(skeletalMesh->getRight().x * movementSpeed, 0, -skeletalMesh->getRight().z * movementSpeed));

			mainCamera->getCamera()->ProcessMouseMovement(skeletalMesh->rigidBody->position, skeletalMesh->getFront(), 0, 0);
			animationIndex = 2;
		}
		if (context->getKey(GLFW_KEY_D) == GLFW_PRESS) {
			skeletalMesh->rigidBody->rigidBody->setLinearVelocity(rp3d::Vector3(-skeletalMesh->getRight().x * movementSpeed, 0, skeletalMesh->getRight().z * movementSpeed));

			mainCamera->getCamera()->ProcessMouseMovement(skeletalMesh->rigidBody->position, skeletalMesh->getFront(), 0, 0);
			animationIndex = 1;
		}
		if (context->getKey(GLFW_KEY_S) == GLFW_PRESS) {
			skeletalMesh->rigidBody->rigidBody->setLinearVelocity(rp3d::Vector3(-skeletalMesh->getFront().x * movementSpeed, 0, skeletalMesh->getFront().z * movementSpeed));

			mainCamera->getCamera()->ProcessMouseMovement(skeletalMesh->rigidBody->position, skeletalMesh->getFront(), 0, 0);
			animationIndex = 3;
		}

		if(animationIndex == 0)skeletalMesh->rigidBody->rigidBody->setLinearVelocity(rp3d::Vector3(0, 0, 0));
		if(yVel < -0.1)skeletalMesh->rigidBody->rigidBody->setLinearVelocity(rp3d::Vector3(0, -9.81, 0));

		if (context->IsMouseMoving()) {
			handleMouse(context->getMouseOffset());
			context->setMouseIsMoving(false);
		}

		return animationIndex;
	}

	void MainCharacter::handleBlocks(rp3d::Vector3 position) {
		if (currentBlock == nullptr) {
			for (unsigned int i = 0; i < blocks->size(); i++)
			{
				engine::renderer::WorldObject* b = &blocks->at(i);
				if (b->getRigidbody()->rigidBody->testPointInside(position)) {
					if (b->isActive) {
						b->isActive = false;
						b->getObject(1)->getMaterial()->ambient = glm::vec3(10, 0, 0);
						b->getObject(1)->setMaterialUniforms();
						currentBlock = b;
						--count;
						mainUI->getUI()->blocks = count;
						soundEngine->play2D(error.getSound());
					}
					else {
						b->isActive = true;
						b->getObject(1)->getMaterial()->ambient = glm::vec3(0, 0, 10);
						b->getObject(1)->setMaterialUniforms();
						currentBlock = b;
						soundEngine->play2D(right.getSound());

						if (++count == 51) {
							quest = 1;
							handleQuests();
							mainUI->handleTime();
						}
						mainUI->getUI()->blocks = count;
					}
					break;
				}
			}	
		}
		else {
			if (!currentBlock->getRigidbody()->rigidBody->testPointInside(position)) {
				transformBlock(-2);
				barrierBlock = currentBlock;
				transformBlock(2);
				currentBlock = nullptr;
			}
		}
	}

	void MainCharacter::transformBlock(int y) {
		if (barrierBlock != nullptr) {
			rp3d::Vector3 v = barrierBlock->getRigidbody()->rigidBody->getTransform().getPosition();
			barrierBlock->getRigidbody()->rigidBody->setTransform(rp3d::Transform(rp3d::Vector3(v.x, v.y + y, v.z), rp3d::Quaternion::identity()));
			barrierBlock->getObject(1)->GetTransform()->TranslateBy(glm::vec3(0, y, 0));
		}
	}

	void MainCharacter::handleMouse(glm::vec2 offset)
	{
		skeletalMesh->ProcessMeshOrientation(offset.x);

		mainCamera->getCamera()->ProcessMouseMovement(skeletalMesh->rigidBody->position, skeletalMesh->getFront(), offset.x, offset.y);
	}

	
	void MainCharacter::handleUI(engine::context::Context* context, engine::renderer::Renderer* renderer, engine::physics::Physics* physics) {
		if (mainUI->restartGame) {
			skeletalMesh->rigidBody->rigidBody->setTransform(rp3d::Transform(rp3d::Vector3(0, 1, 0), rp3d::Quaternion::identity()));
			skeletalMesh->GetTransform()->TranslateTo(glm::vec3(0, 1, 0));

			if (count == 51) {
				soundEngine->play2D(spawn.getSound());
			}
			else {
				soundEngine->play2D(lose.getSound());
			}

			initializeQuests();
			mainUI->getUI()->questLog = restartQuestText;

			for (unsigned int i = 0; i < blocks->size(); i++)
			{
				engine::renderer::WorldObject* b = &blocks->at(i);

				b->isActive = false;
				b->getObject(1)->getMaterial()->ambient = glm::vec3(10, 0, 0);
				b->getObject(1)->setMaterialUniforms();
			}

			blocks->at(5).getObject(1)->getMaterial()->ambient = glm::vec3(0.0, 0.0, 10.0);
			blocks->at(5).getObject(1)->setMaterialUniforms();
			blocks->at(5).isActive = true;
			blocks->at(6).getObject(1)->getMaterial()->ambient = glm::vec3(0.0, 0.0, 10.0);
			blocks->at(6).getObject(1)->setMaterialUniforms();
			blocks->at(6).isActive = true;
			blocks->at(16).getObject(1)->getMaterial()->ambient = glm::vec3(0.0, 0.0, 10.0);
			blocks->at(16).getObject(1)->setMaterialUniforms();
			blocks->at(16).isActive = true;
			blocks->at(17).getObject(1)->getMaterial()->ambient = glm::vec3(0.0, 0.0, 10.0);
			blocks->at(17).getObject(1)->setMaterialUniforms();
			blocks->at(17).isActive = true;
			blocks->at(34).getObject(1)->getMaterial()->ambient = glm::vec3(0.0, 0.0, 10.0);
			blocks->at(34).getObject(1)->setMaterialUniforms();
			blocks->at(34).isActive = true;

			transformBlock(-2);

			currentBlock = nullptr;
			barrierBlock = nullptr;
			count = 5;

			mainUI->getUI()->blocks = 5;
			mainUI->getUI()->startTime = context->getCurrentFrameTime();
			mainUI->restartGame = false;
		}
	}

}