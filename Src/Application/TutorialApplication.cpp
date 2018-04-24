/*
-----------------------------------------------------------------------------
Filename:    TutorialApplication.cpp
-----------------------------------------------------------------------------
This source file is part of the
___                 __    __ _ _    _
/___\__ _ _ __ ___  / / /\ \ (_) | _(_)
//  // _` | '__/ _ \ \ \/  \/ / | |/ / |
/ \_// (_| | | |  __/  \  /\  /| |   <| |
\___/ \__, |_|  \___|   \/  \/ |_|_|\_\_|
|___/
Tutorial Framework
http://www.ogre3d.org/tikiwiki/
-----------------------------------------------------------------------------
*/

#include "TutorialApplication.h"
#include <OgreSceneNode.h>
#include <OgreEntity.h>
//#include "GameComponent.h"
#include "GameObject.h"
#include "EntityComponent.h"
#include "MoveComponent.h"
#include "MoveCameraComponent.h"
#include <stdio.h>
#include "AnimationComponent.h"


#include <time.h>
#include <iostream>
#include "EnemyComponent.h"
#include "RigidbodyComponent.h"
#include "DynamicRigidbodyComponent.h"
using namespace Ogre;

//#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll

//-------------------------------------------------------------------------------------
TutorialApplication::TutorialApplication(void)
{
}
//-------------------------------------------------------------------------------------

/*TutorialApplication::~TutorialApplication(void)
{
}
*/
//-------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------

void TutorialApplication::createLights(void)
{
	//Creamos luz ambiental
	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));

	//Creamos una luz
	Light* light = mSceneMgr->createLight("MainLight");
	SceneNode* lightNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	lightNode->attachObject(light);

	//Damos posici�n al nodo de la luz
	lightNode->setPosition(20, 80, 50);
}

//-------------------------------------------------------------------------------------

void TutorialApplication::createCameras(void)
{
	//Creamos camara
	GameObject * cam = new GameObject(mSceneMgr);
	SceneNode* camNode = cam->getNode();
	camNode->attachObject(mCamera);
	camNode->setPosition(0, 47, 222);
	cam->addComponent(new MoveCameraComponent(BaseApplication::mWindow, mSceneMgr));
	actors_.push_back(cam);
	
	/*
	SceneNode* camNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	// create the camera
	Camera* cam = mSceneMgr->createCamera("myCam");
	cam->setNearClipDistance(5); // specific to this sample
	cam->setAutoAspectRatio(true);
	camNode->attachObject(cam);
	camNode->setPosition(0, 0, 140);
	// and tell it to render into the main window
	getRenderWindow()->addViewport(cam);
	*/
}

//-------------------------------------------------------------------------------------

void TutorialApplication::createEntities(void)
{

	

	//Creamos un plano de terreno (Rigidbody est�tico) y una Esfera que cae al plano (dynamic rigidbody)

	//Todos los rigidbody necesitan una referencia al collision shape.
	//Collision shape es UNICAMENTE	para colisiones. No tiene masa, inercia,etc.
	//Si hay muchos rigidbodies con la misma colision, es bueno utilizar el mismo collision shape.

	//---------------------PLANO---------------------------------

	//COMENTADO YA QUE SE HACE ABAJO CON TILES. LO HE DEJADO PARA PODER CONSULTAR LA ESTRUCTURA
	
	//Crear el plano en Ogre
	//Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);
	//Ogre::MeshManager::getSingleton().createPlane("ground", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 1500, 1500, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Z);

	GameObject *planito = new GameObject(mSceneMgr,"suelo");

	//PhysicsComponent

	//MOTION STATE
	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(0, -1, 0));

	btDefaultMotionState *groundMotionState = new btDefaultMotionState(groundTransform);

	//COLLISION SHAPE
	//Creamos un plano en el origen.
	btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);//Offset de 1
	//Masa 0 -> Objeto est�tico. Masa infinita
	btScalar groundMass(0.); //the mass is 0, because the ground is immovable (static)
	btVector3 localGroundInertia(0, 0, 0);

	planito->addComponent(new RigidbodyComponent(groundMotionState, groundShape, groundMass, localGroundInertia));

	actors_.push_back(planito);

	//---------------------PLANO---------------------------------

	//---------------------ESFERA---------------------------------
	GameObject *esfera = new GameObject(mSceneMgr,"esfera");
	esfera->addComponent(new EntityComponent("ogrehead.mesh"));
	esfera->getNode()->setScale(Ogre::Real(0.2), Ogre::Real(0.2), Ogre::Real(0.2));

	//Motion state
	//set the initial position and transform. For this demo, we set the tranform to be none
	btVector3 initialPosition(0, 100, 0);
	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(initialPosition);


	//actually contruvc the body and add it to the dynamics world
	//Esfera a 50 metros de altura
	btDefaultMotionState *fallMotionState = new btDefaultMotionState(startTransform);

	//Colision shape
	//Creamos la esfera de radio 1
	btCollisionShape* fallShape = new btSphereShape(1);
	//btCollisionShape *newRigidShape = new btBoxShape(btVector3(5.0f, 1.0f, 5.0f));

	//set the mass of the object. a mass of "0" means that it is an immovable object
	btScalar mass(10.0f);
	btVector3 fallInertia(0, 0, 0);

	DynamicRigidbodyComponent* rbComponent = new DynamicRigidbodyComponent(fallMotionState, fallShape, mass, fallInertia);
	esfera->addComponent(rbComponent);

	rbComponent->getRigidbody()->setRestitution(1);

	actors_.push_back(esfera);

	/*
	EXPLICACI�N DE BTRIGIDBODY::btRigidBodyConstructionInfo:
	SI QUEREMOS CREAR OBJETOS SIMILARES, UTILIZAMOS EL MISMO BTRIGIDBODYCONSTRUCTIONINFO, YA QUE
	SE COPIAN AL OBJETO QUE SE LO DAMOS
	*/

	//---------------------ESFERA---------------------------------

	srand((unsigned int)time(NULL));

	int random = 0;
	int cont = 200;

	for (int i = 0; i < 40; i++){
		for (int j = 0; j < 40; j++){
			random = rand() % 101;
			//Game Component ahora es Game Object
			GameObject *suelo = new GameObject(mSceneMgr);
			suelo->getNode()->setPosition(Ogre::Vector3((i * 50) - 300, -20, (j * 50) - 300));
			suelo->getNode()->setScale(Ogre::Vector3(5, 5, 5));

			if (j == 20 && i == 20)
			{
				suelo->addComponent(new EntityComponent("Torre.mesh"));
				std::cout << "Posicion de la torre: " << suelo->getNode()->getPosition().x << ", " <<
					suelo->getNode()->getPosition().y << ", " << suelo->getNode()->getPosition().z << "\n";
				actors_.push_back(planito);
			}
			else{
				if (random % 6 == 0)
				{
					suelo->addComponent(new EntityComponent("arbol.mesh"));
					actors_.push_back(planito);
				}
				else
				{
					suelo->addComponent(new EntityComponent("suelo.mesh"));
					actors_.push_back(suelo);

				}
			}
		}
	}
	
	
	EnemyFactory::initialize(mSceneMgr);

	EnemyPrototype * enemigui;//Prototipo del enemigo
	//Super �til
	for (int i = 0; i < 100; i++){
		enemigui = EnemyFactory::getTypeEnemy();
		enemigui->getNode()->setPosition(Ogre::Vector3((rand() % 40 * 50) - 300, 100, (rand() % 40 * 50) - 300));
		enemigui->getNode()->setScale(Ogre::Real(0.2), Ogre::Real(0.2), Ogre::Real(0.2));
		actors_.push_back(enemigui);
	}


	

	//for (int i = 0; i < 2; i++){
	//	for (int j = 0; j < 20; j++){
	//		GameObject * enemigo = new GameObject(mSceneMgr);
	//		enemigo->getNode()->setScale(0.5, 0.5, 0.5);

	//		enemigo->addComponent(new EntityComponent("ogrehead.mesh")); //Ninja.mesh
	//		//enemigo->addComponent(new CollisionComponent());		//Da un lag de la hostia cuando los enemigos colisionan contra el suelo.
	//		//enemigo->addComponent(new AnimationComponent("Idle1")); //Le pasas una inicial, luego la cambias desde el input.
	//		//enemigo->addComponent(new MoveComponent());			//Debajo del animation porque lo usa ->Asumo que el enemy prototype tiene MoveComponent
	//		btVector3 enemyInitialPosition((rand() % 40 * 50) - 300, 100, (rand() % 40 * 50) - 300);
	//		btTransform enemyTransform;
	//		enemyTransform.setIdentity();
	//		enemyTransform.setOrigin(enemyInitialPosition);


	//		//actually contruvc the body and add it to the dynamics world
	//		//Esfera a 50 metros de altura
	//		btDefaultMotionState *enemyMotionState = new btDefaultMotionState(enemyTransform);

	//		//Colision shape
	//		btCollisionShape *newRigidShape = new btBoxShape(btVector3(5.0f, 5.0f, 5.0f));

	//		//set the mass of the object. a mass of "0" means that it is an immovable object
	//		btScalar enemyMass(10.0f);
	//		btVector3 enemyInertia(0, 0, 0);

	//		DynamicRigidbodyComponent* enemyRbComponent = new DynamicRigidbodyComponent(enemyMotionState, newRigidShape, enemyMass, enemyInertia);
	//		enemigo->addComponent(enemyRbComponent);
	//		//enemigo->addComponent(new EnemyComponent());

	//		actors_.push_back(enemigo);
	//	}
	//}
	
	GameObject* ninja = new GameObject(mSceneMgr);
	ninja->getNode()->setScale(Ogre::Real(1.4), Ogre::Real(1.4), Ogre::Real(1.4));
	
	ninja->addComponent(new EntityComponent("ninja.mesh")); //Ninja.mesh
	ninja->addComponent(new AnimationComponent("Idle1")); //Le pasas una inicial, luego la cambias desde el input.
	ninja->addComponent(new MoveComponent());			//Debajo del animation porque lo usa ->Asumo que el enemy prototype tiene MoveComponent
	//ninja->getNode()->setPosition(Ogre::Vector3((rand() % 40 * 50) - 300, 0, (rand() % 40 * 50) - 300));

	//Motion state
	//set the initial position and transform. For this demo, we set the tranform to be none
	btVector3 ninjaInitialPosition(0, 200, 0);
	btTransform ninjaTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(ninjaInitialPosition);


	//actually contruvc the body and add it to the dynamics world
	//Esfera a 50 metros de altura
	btDefaultMotionState *ninjaMotionState = new btDefaultMotionState(ninjaTransform);

	//Colision shape
	btCollisionShape *newRigidShapeNinja = new btBoxShape(btVector3(5.0f, 5.0f, 5.0f));

	//set the mass of the object. a mass of "0" means that it is an immovable object
	btScalar ninjaMass(10.0f);
	btVector3 ninjaInertia(0, 0, 0);

	DynamicRigidbodyComponent* ninjaRbComponent = new DynamicRigidbodyComponent(ninjaMotionState, newRigidShapeNinja, ninjaMass, ninjaInertia);
	ninja->addComponent(ninjaRbComponent);

	ninjaRbComponent->getRigidbody()->setRestitution(1);

	
	actors_.push_back(ninja);
	
}

//-------------------------------------------------------------------------------------
void TutorialApplication::createScene(void)
{
	// create your scene here :)

	createLights();

	createCameras();

	createEntities();
}

TutorialApplication *TutorialApplication::instance = 0;

TutorialApplication *TutorialApplication::getInstance()
{
	if (!instance)
		instance = new TutorialApplication;
	return instance;

}

