#ifndef __PlayerComponent_h_
#define __PlayerComponent_h_

#include "Component.h"
#include "RigidbodyComponent.h"

const float _LINEARVELOCITY = 500.f;
const float _ANGULARVELOCITY = 150.f;

class PlayerComponent : public Component{

#pragma region Attributes  
private:
	RigidbodyComponent * rigidbody;
	b2Vec2 velocity;
	float angle;

#pragma endregion Attributes

#pragma region Methods  

public:
	PlayerComponent();
	virtual ~PlayerComponent();

	virtual void Start();
	virtual void Update(double elapsed);

	virtual void OnCollisionEnter(ColliderComponent* collider);		//Es llamado cuando dos gameObject colisionan.
	virtual void OnCollisionExit(ColliderComponent* collider);		//Es llamado cuando dos gameObject dejan de colisionar. 

#pragma endregion Methods

};

#endif /* __PlayerComponent_h_ */