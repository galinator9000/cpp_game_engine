#include "Game.h"

Game::Game(World* pWorld, Controller* pMainController){
	this->pWorld = pWorld;
	this->pMainController = pMainController;
}

void Game::Setup(){
	// Setup game scene.
	// Add main camera.
	this->pWorld->addCamera(&this->wMainCamera);

	// Create box mesh for graphics side.
	Mesh* boxMesh = new Mesh();
	boxMesh->createBoxGeometry({ 1,1,1 });

	// Ground box.
	CollisionShape* groundBoxColShape = new CollisionShape();
	groundBoxColShape->createBoxGeometry({ 2000, 0.1f, 2000 });
	CollisionActor* groundBoxColActor = new CollisionActor(false);
	Entity* groundBox = new Entity(
		{ 20, 0.1f, 20 },
		{ 0, 0, 0 },
		{ 0,0,0,0 },
		{ 0.5f, 0.5f, 0.5f, 1 },
		{ 1,1,0 },
		boxMesh,
		groundBoxColShape,
		groundBoxColActor
	);
	this->pWorld->addEntity(groundBox);

	// Dynamic boxes.
	CollisionShape* dynamicBoxColShape = new CollisionShape();
	dynamicBoxColShape->createBoxGeometry({ 1,1,1 });
	Entity* box;
	for (int b = 0; b<50; b++) {
		CollisionActor* boxColActor = new CollisionActor(true);
		box = new Entity(
			{ 1,1,1 },
			{ 0, b*5.0f, 0 },
			{ 0,0,0,0 },
			{ 0.5f, 0.5f, 0.5f, 1 },
			{ 1,1,0 },
			boxMesh,
			dynamicBoxColShape,
			boxColActor
		);
		this->pWorld->addEntity(box);
	}

	//// Load animated entity.
	Mesh* animatedMesh = new Mesh();
	MeshDeformer* animatedMeshDeformer = new MeshDeformer();
	animatedEntity = new Entity(
		{0.025f, 0.025f, 0.025f},
		{ 0, 0, 10},
		{0,0,0,0},
		{0.66f, 0.66f, 0.66f, 1},
		{ 1,1,1 },
		animatedMesh
	);
	if (animatedMesh->LoadFBX("C:\\VisualStudioProjects\\cpp_game_engine\\assets\\BaseMesh_Anim_Triangle.fbx")) {
		animatedEntity->attachMeshDeformer(animatedMeshDeformer);
		animatedEntity->setAnimation("Root|Root|Root|Idle|Root|Idle");
		this->pWorld->addEntity(animatedEntity);

		// Create character for animated entity.
		Character* mainCharacter = new Character(animatedEntity);
		this->pMainController->setMainCharacter(mainCharacter);

		// Add secondary camera.
		Camera* pEntityCamera = new Camera(
			Vector3(0.0f, 0.0f, 0.0f),
			FOV,
			WIDTH / HEIGHT
		);
		pEntityCamera->followEntity(animatedEntity, Vector3(0, 5, 0), Vector3(0, 0, 6));
		pWorld->addCamera(pEntityCamera);
	}

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
}