// GameLab 3 Tutorial 2019/20
// Andreas Knote <andreas.knote@uni-wuerzburg.de>


#include <engine/physics/physics.h>

#include <functional>
#include <algorithm>

namespace engine::physics {

    Physics::Physics() : accumulator(0) {
	
		// Gravity vector
		rp3d::Vector3 gravity(0.0, -9.81, 0.0);

		// Create the dynamics world
		dynamicsWorld = std::make_unique<rp3d::DynamicsWorld>(gravity);

		// Change the number of iterations of the velocity solver
		dynamicsWorld->setNbIterationsVelocitySolver(15);

		// Change the number of iterations of the position solver
		dynamicsWorld->setNbIterationsPositionSolver(8);
	
	};

	Physics::~Physics() {

	}
	
    void Physics::lateUpdate() {
        updateWorld();
        updateTransformations();
    }

    RigidBody* Physics::createRigidBody(glm::vec3 position, bool isSleeping, bool simulateGravity, float damping, float mass, rp3d::BodyType type) {
        
		RigidBody* body = new RigidBody();

        rp3d::Transform rp3dTransform;
		
		// Initial position and orientation of the rigid body 
		rp3d::Vector3 initPosition(position.x, position.y, position.z);
		rp3d::Quaternion initOrientation = rp3d::Quaternion::identity();
		rp3d::Transform transform(initPosition, initOrientation);

		// Create a rigid body in the world 
		body->rigidBody = dynamicsWorld->createRigidBody(transform);
        body->rigidBody->setIsSleeping(isSleeping);
		body->rigidBody->enableGravity(simulateGravity);
		body->rigidBody->setLinearDamping(damping);
		body->rigidBody->setMass(mass);
		body->rigidBody->setType(type);
		body->rigidBody->setAngularDamping(damping);
		body->rigidBody->setLinearVelocity(rp3d::Vector3(0,0,0));
		body->rigidBody->setAngularVelocity(rp3d::Vector3(0, 0, 0));

		copy(body->rigidBody->getTransform(), body->transform);
        
		rigidbodies.insert(body);

		return body;
    }

    void Physics::updateWorld() {
        using namespace std::chrono;

        // Get the current system time
        high_resolution_clock::time_point currentFrameTime = high_resolution_clock::now();

        // Compute the time difference between the two frames
        long double mDeltaTime = duration_cast<duration<long double>>(
                currentFrameTime - previousFrameTime).count();
        mDeltaTime = std::min(mDeltaTime, (long double) (10.0f * timeStep));

        // Update the previous time
        previousFrameTime = currentFrameTime;

        // Add the time difference in the accumulator
        accumulator += mDeltaTime;

        // While there is enough accumulated time to take
        // one or several physics steps
        while (accumulator >= timeStep) {
            // Update the Dynamics world with a constant time step
            dynamicsWorld->update(timeStep);

            // Decrease the accumulated time
            accumulator -= timeStep;
        }
    }

    void Physics::updateTransformations() {
        // Compute the time interpolation factor
        float factor = accumulator / timeStep;

        for (auto &rb : rigidbodies) {
            
            // Get the updated transform of the body
            rp3d::Transform currTransform = rb->rigidBody->getTransform();

            // Compute the interpolated transform of the rigid body
            rp3d::Transform interpolatedTransform = rp3d::Transform::interpolateTransforms(rb->previousTransform, currTransform,
                                                                                           factor);
            // Now you can render your body using the interpolated transform here
			copy(interpolatedTransform, rb->transform);
			rb->position = glm::vec3(interpolatedTransform.getPosition().x, interpolatedTransform.getPosition().y, interpolatedTransform.getPosition().z);


			rb->movingDistance = (currTransform.getPosition() - rb->previousTransform.getPosition()).length();


			// Update the previous transform
			rb->previousTransform = currTransform;
        }
    }
}
