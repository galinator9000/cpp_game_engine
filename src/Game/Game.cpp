#include "Game.h"

Game::Game(World* pWorld){
	this->pWorld = pWorld;
}

void Game::Setup(){
	// Setup game scene.
	//// Create box entities.
	// This mesh will be shared between boxes.
	Mesh* boxMesh = new Mesh();
	boxMesh->createBoxShape();

	// Parent box
	this->parentBoxEntity = new Entity(
		{ 1,1,1 },
		{ 0,0,0 },
		{ 0,0,0,0 },
		{ 0.66f, 0.66f, 0.66f, 1 },
		{ 1,1,1 },
		boxMesh
	);
	this->pWorld->addEntity(this->parentBoxEntity);

	// Child box 1
	Entity* childBoxEntity1 = new Entity(
		{ 1,1,1 },
		{ 0,0,5 },
		{ 0,0,0,0 },
		{ 0.66f, 0.66f, 0.66f, 1 },
		{ 1,1,1 },
		boxMesh
	);
	this->pWorld->addEntity(childBoxEntity1);
	this->parentBoxEntity->attachChild(childBoxEntity1);

	// Child box 2
	Entity* childBoxEntity2 = new Entity(
		{ 1,1,1 },
		{ 0,0,-5 },
		{ 0,0,0,0 },
		{ 0.66f, 0.66f, 0.66f, 1 },
		{ 1,1,1 },
		boxMesh
	);
	this->pWorld->addEntity(childBoxEntity2);
	this->parentBoxEntity->attachChild(childBoxEntity2);

	// Child box 3
	Entity* childBoxEntity3 = new Entity(
		{ 1,1,1 },
		{ 0,5,0 },
		{ 0,0,0,0 },
		{ 0.66f, 0.66f, 0.66f, 1 },
		{ 1,1,1 },
		boxMesh
	);
	this->pWorld->addEntity(childBoxEntity3);
	this->parentBoxEntity->attachChild(childBoxEntity3);

	// Child box 4
	Entity* childBoxEntity4 = new Entity(
		{ 1,1,1 },
		{ 0,-5,0 },
		{ 0,0,0,0 },
		{ 0.66f, 0.66f, 0.66f, 1 },
		{ 1,1,1 },
		boxMesh
	);
	this->pWorld->addEntity(childBoxEntity4);
	this->parentBoxEntity->attachChild(childBoxEntity4);

	//// Animated entity.
	Mesh* animatedMesh = new Mesh();
	MeshDeformer* animatedMeshDeformer = new MeshDeformer();

	Entity* animatedEntity = new Entity(
		{0.025f, 0.025f, 0.025f},
		{ 0, 0, 10},
		{0,0,0,0},
		{0.66f, 0.66f, 0.66f, 1},
		{1,1,1},
		animatedMesh
	);

	if (animatedMesh->LoadFBX("C:\\VisualStudioProjects\\cpp_game_engine\\assets\\BaseMesh_Anim_Triangle.fbx")) {
		animatedEntity->attachMeshDeformer(animatedMeshDeformer);
		animatedEntity->setAnimation("Root|Root|Root|Root|Root|Idle|Root|Idle|Root|Root|Root|Idle|Root|Idl");
		this->pWorld->addEntity(animatedEntity);
	}

	// Add main camera to world.
	pWorld->addCamera(&wMainCamera);

	// Add secondary camera.
	Camera* pEntityCamera = new Camera(
		Vector3(0.0f, 0.0f, 0.0f),
		FOV,
		WIDTH / HEIGHT
	);
	pEntityCamera->followEntity(animatedEntity, Vector3(0, 5, 0), Vector3(0, 0, 6));
	pWorld->addCamera(pEntityCamera, true);

	// Add lights to scene.
	PointLight* pointLight = new PointLight(Vector3(0, 5, -10.0f), 1.0f);
	this->pWorld->addLight(pointLight);
}

void Game::Update(){
	if (timer.Peek() > 1.0f) {
		timer.Reset();
	}

	// Circular motion around 0,0,0 for point light.
	float cosx = cos(timer.Peek() * 6.28f) * 5.0f;
	float siny = sin(-timer.Peek() * 6.28f) * 5.0f;

	// Transform parent box.
	this->parentBoxEntity->Translate({ 0.01f, 0, 0});
	this->parentBoxEntity->rotateQuaternion({ 1, 0.5f, 0, timer.Peek() * 6.28f });
}