#include <engine\renderer\worldObject.h>

namespace engine::renderer {
	
	WorldObject::WorldObject() {

	}

	WorldObject::WorldObject(std::vector<Mesh*> meshes) {
		objects = meshes;
	}

	WorldObject::~WorldObject() {
		auto const to_be_removed = std::partition(begin(objects), end(objects), [](auto p) { return p != nullptr; });
		std::for_each(to_be_removed, end(objects), [](auto p) {
			delete p;
		});
		objects.clear();
	}

	void WorldObject::initialize(engine::assets::Model* model, glm::vec3 pos, glm::vec3 rot, glm::vec3 scale, engine::assets::Text vertShader, engine::assets::Text fragShader) {
		for (unsigned int j = 0; j < model->numberOfMeshParts(); j++)
		{
			Mesh* mesh = new Mesh(pos, rot, scale);

			mesh->shader = new engine::renderer::ShaderProgram(
				engine::renderer::Shader(vertShader.get(), GL_VERTEX_SHADER),
				engine::renderer::Shader(fragShader.get(), GL_FRAGMENT_SHADER));
			mesh->setBuffers(model->vertices(j), model->indices(j), model->indices(j).size());
			mesh->setTextures(model->textures(j));
			mesh->setMaterial(model->material(j));
			addObject(mesh);
		}
	}

	void WorldObject::registerAllRenderables(Renderer* renderer) {
		for (unsigned int i = 0; i < objects.size(); i++)
		{
			objects[i]->setImmutableUniforms(renderer->getCamera(), renderer->getLights());
			renderer->registerRenderable(objects[i]);
		}
	}

	void WorldObject::initPhysics(glm::vec3 pos, float radius, engine::physics::Physics* physics) {
		setRigidbody(physics->createRigidBody(pos, true, false, 0.0, 1.0, rp3d::BodyType::STATIC));
		setSphereShape(new rp3d::SphereShape(radius));
		getRigidbody()->rigidBody->addCollisionShape(getSphereShape(), getRigidbody()->rigidBody->getTransform(), 1.0);
		physics->collisionMap.push_back(engine::physics::PointCollision(pos, radius));
	}

	void WorldObject::initPhysics(glm::vec3 pos, rp3d::Vector3 bounds, engine::physics::Physics* physics) {
		setRigidbody(physics->createRigidBody(pos, true, false, 0.0, 1.0, rp3d::BodyType::STATIC));
		setBoxShape(new rp3d::BoxShape(bounds));
		getRigidbody()->rigidBody->addCollisionShape(getBoxShape(), getRigidbody()->rigidBody->getTransform(), 1.0);
		physics->collisionMap.push_back(engine::physics::PointCollision(pos, (float)bounds.x));
	}

	void WorldObject::resetPhysics(engine::physics::Physics* physics) {
		rigidBody->rigidBody->removeCollisionShape(getShape());
		physics->deleteRigidbody(rigidBody);
		physics->collisionMap.pop_back();
		rigidBody->rigidBody = nullptr;
	}

	void WorldObject::registerRenderables(Renderer* renderer, Transform* transform) {
		for (unsigned int i = 0; i < getObjects().size(); i++)
		{
			engine::renderer::Mesh *m = getObject(i);
			m->SetParentTransform(transform);
			renderer->registerRenderable(m);
		}
	}
	void WorldObject::deRegisterRenderables(Renderer* renderer) {
		for (unsigned int i = 0; i < getObjects().size(); i++)
		{
			renderer->deRegisterRenderable(getObject(i));
		}
	}

	void WorldObject::addObject(Mesh* mesh) {
		objects.push_back(mesh);
	}

	std::vector<Mesh*> WorldObject::getObjects() {
		return objects;
	}
	
	Mesh* WorldObject::getObject(unsigned int i) {
		return objects[i];
	}

	void WorldObject::setRigidbody(engine::physics::RigidBody* body) {
		rigidBody = body;
	}

	engine::physics::RigidBody* WorldObject::getRigidbody() {
		return rigidBody;
	}

	void WorldObject::setBoxShape(rp3d::BoxShape* box) {
		boxShape = box;
	}

	rp3d::BoxShape* WorldObject::getBoxShape() {
		return boxShape;
	}

	void WorldObject::setSphereShape(rp3d::SphereShape* sphere) {
		sphereShape = sphere;
	}

	rp3d::SphereShape* WorldObject::getSphereShape() {
		return sphereShape;
	}

	void WorldObject::setBoxShapeBounds(rp3d::Vector3 bounds) {
		boxShapeBounds = bounds;
		hasBoxShape = true;
	}

	rp3d::Vector3 WorldObject::getBoxShapeBounds() {
		return boxShapeBounds;
	}

	void WorldObject::setSphereShapeRadius(float radius) {
		sphereRadius = radius;
		hasSphereShape = true;
	}
	
	float WorldObject::getSphereShapeRadius() {
		return sphereRadius;
	}

	rp3d::ProxyShape* WorldObject::getShape() {
		if (hasBoxShape) return (rp3d::ProxyShape*)boxShape;
		else return (rp3d::ProxyShape*)sphereShape;
	}

	float WorldObject::getPointRadius() {
		if (hasBoxShape) return boxShapeBounds.x;
		else return sphereRadius;
	}

	void WorldObject::setCollision(engine::physics::Physics* p) {
		setRigidbody(p->createRigidBody(glm::vec3(0, 0, 0), true, false, 0.0, 1.0, rp3d::BodyType::STATIC));
		engine::physics::copy(getObject(0)->GetTransform()->modelMatrix, getRigidbody()->previousTransform);
		
		if (hasBoxShape) {
			setBoxShape(new rp3d::BoxShape(getBoxShapeBounds()));
			getRigidbody()->rigidBody->addCollisionShape(getBoxShape(), getRigidbody()->previousTransform, 1.0);
			p->collisionMap.push_back(engine::physics::PointCollision(getObject(0)->GetTransform()->GetModelPosition(), getBoxShapeBounds().x ));
		}
		else {
			setSphereShape(new rp3d::SphereShape(getSphereShapeRadius()));
			getRigidbody()->rigidBody->addCollisionShape(getSphereShape(), getRigidbody()->previousTransform, 1.0);
			p->collisionMap.push_back(engine::physics::PointCollision(getObject(0)->GetTransform()->GetModelPosition(), getSphereShapeRadius() ));
		}
	}

}