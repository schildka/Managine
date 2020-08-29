// GameLab 3 Tutorial 2019/20
// Andreas Knote <andreas.knote@uni-wuerzburg.de>

#pragma once

#include "maths.h"

#include <chrono>

#include <set>
#include <map>

namespace engine::physics {

	struct RigidBody {
		rp3d::RigidBody *rigidBody = nullptr;
		rp3d::Transform previousTransform;
		glm::mat4 transform;
		glm::vec3 position;
		float movingDistance;
	};

	struct PointCollision {
		glm::vec3 worldPosition;
		float radius;

		PointCollision(glm::vec3 p, float r) {
			worldPosition = p;
			radius = r;
		}
	};

    class Physics {
    public:

        Physics();

		~Physics();

		void lateUpdate();

		RigidBody* createRigidBody(glm::vec3 position, bool isSleeping, bool simulateGravity, float damping, float mass, rp3d::BodyType type);

		void deleteRigidbody(RigidBody* rigidbody) {
			dynamicsWorld->destroyRigidBody(rigidbody->rigidBody);
			rigidbodies.erase(rigidbody);
		};

		std::vector<PointCollision>* getCollisionMap() { return &collisionMap; };
		
    private:

        void updateWorld();

        void updateTransformations();

	public:

		std::vector<PointCollision> collisionMap;

    private:

		std::set<RigidBody*> rigidbodies;

        std::unique_ptr<rp3d::DynamicsWorld> dynamicsWorld;

        std::chrono::high_resolution_clock::time_point previousFrameTime;

        float accumulator = 0.0f;
        const float timeStep = 1.0f / 90.0f;
    };
}
