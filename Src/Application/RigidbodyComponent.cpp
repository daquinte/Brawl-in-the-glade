#include "RigidbodyComponent.h"
#include "GameObject.h"
#include "Error.h"

#include <Ogre.h>

RigidbodyComponent::RigidbodyComponent(bool kinematic, float density, float friction, float restitution) :
kinematic(kinematic), density(density), friction(friction), restitution(restitution)
{
	
}

RigidbodyComponent::~RigidbodyComponent(){
}

void RigidbodyComponent::Start(){

	ColliderComponent * collider = (ColliderComponent*)(gameObject->GetComponent(COLLIDER));

	if (collider == nullptr){

		Error errorE("\n\n\n\n\nError al crear el Rigidbody. Necesita un Collider ");
		throw errorE;
	}

	body = collider->GetBody();

	if (!kinematic)
		body->SetType(b2BodyType::b2_dynamicBody);
	else
		body->SetType(b2BodyType::b2_kinematicBody);

	//TODO: meter m�s fixtures al body
	b2Fixture fixture = body->GetFixtureList()[0];
	fixture.SetDensity(density);
	fixture.SetFriction(friction);
	fixture.SetRestitution(restitution);

}

void RigidbodyComponent::Update(double elapsed){

	Ogre::SceneNode * node = gameObject->GetNode();
	b2Vec2 pos = body->GetPosition();
	float angle = body->GetAngle();

	Ogre::Radian angleRadians(angle / (2 * 3.14));
	Ogre::Quaternion quat;
	quat.FromAngleAxis(angleRadians, Ogre::Vector3(0, 1, 0));

	node->setPosition(Ogre::Vector3(pos.x, pos.y, 0));
	node->setOrientation(quat);
}
