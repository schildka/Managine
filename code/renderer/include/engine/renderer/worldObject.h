#pragma once

#include<engine/assets/text.h>

#include<engine/physics/physics.h>

#include<engine/renderer/renderer.h>
#include<engine/renderer/mesh.h>

namespace engine::renderer {
	/** 
	* WorldObject combines mesh informations also mesh compositions and physic informations. 
	* @param objects Vector of mesh objects the world object is composed of.
	*/
	class WorldObject {
	public:
		WorldObject();
		WorldObject(std::vector<Mesh*> meshes);
		~WorldObject();

		void initialize(engine::assets::Model* model, glm::vec3 pos, glm::vec3 rot, glm::vec3 scale, engine::assets::Text vertShader, engine::assets::Text fragShader);
		/** Registers the vector of meshobjects to the renderer. */
		void registerAllRenderables(Renderer* renderer);

		void initPhysics(glm::vec3 pos, float radius, engine::physics::Physics* physics);
		void initPhysics(glm::vec3 pos, rp3d::Vector3 bounds, engine::physics::Physics* physics);
		void resetPhysics(engine::physics::Physics* physics);

		void registerRenderables(Renderer* renderer, Transform* transform);
		void deRegisterRenderables(Renderer* renderer);

		void addObject(Mesh* mesh);
		std::vector<Mesh*> getObjects();
		Mesh* getObject(unsigned int i);

		void setRigidbody(engine::physics::RigidBody* body);
		engine::physics::RigidBody* getRigidbody();
		void setBoxShape(rp3d::BoxShape* box);
		rp3d::BoxShape* getBoxShape();
		void setSphereShape(rp3d::SphereShape* sphere);
		rp3d::SphereShape* getSphereShape();
		void setBoxShapeBounds(rp3d::Vector3 bounds);
		rp3d::Vector3 getBoxShapeBounds();
		void setSphereShapeRadius(float radius);
		float getSphereShapeRadius();

		rp3d::ProxyShape* getShape();
		float getPointRadius();

		void setCollision(engine::physics::Physics* p);

		bool getHasBoxShape() { return hasBoxShape; };

	private:	
		std::vector<Mesh*> objects;

		engine::physics::RigidBody* rigidBody;
		rp3d::BoxShape* boxShape;
		rp3d::SphereShape* sphereShape;
		rp3d::Vector3 boxShapeBounds;
		float sphereRadius;

		bool hasSphereShape = false;
		bool hasBoxShape = false;

	public:
		rp3d::Vector3 staticPosition;

		bool waterBuild = false;
		bool isActive = false;

		float baseHeight = 0;
	};
}