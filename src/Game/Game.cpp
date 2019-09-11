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
	CollisionActor* groundBoxColActor = new CollisionActor(COLLISION_ACTOR_STATIC);
	Entity* groundBox = new Entity(
		{
			{ 2000, 0.1f, 2000 },
			{ 0, 0, 0 },
			{ 0,0,0,0 },
			{ 0.5f, 0.5f, 0.5f, 1 },
			{},
			boxMesh,
			groundBoxColShape,
			groundBoxColActor
		}
	);
	this->pWorld->addEntity(groundBox);

	// Dynamic boxes.
	CollisionShape* dynamicBoxColShape = new CollisionShape();
	dynamicBoxColShape->createBoxGeometry({ 1, 1, 1 });
	Entity* box;
	for (int b = 0; b<500; b++) {
		CollisionActor* dynamicBoxColActor = new CollisionActor(COLLISION_ACTOR_DYNAMIC);
		box = new Entity(
			{
				{ 1, 1, 1 },
				{ 0, b * 2.0f, 0 },
				{ 0,0,0,0 },
				{ 0.5f, 0.5f, 0.5f, 1 },
				{},
				boxMesh,
				dynamicBoxColShape,
				dynamicBoxColActor
			}
		);
		this->pWorld->addEntity(box);
	}

	//// Load animated entity.
	Mesh* mainCharacterMesh = new Mesh();
	MeshDeformer* mainCharacterMeshDeformer = new MeshDeformer();

	CollisionShape* mainCharacterCollisionShape = new CollisionShape();
	mainCharacterCollisionShape->createBoxGeometry({ 1,3,1 });
	CollisionActor* mainCharacterCollisionActor = new CollisionActor(COLLISION_ACTOR_CCT);

	this->mainCharacter = new Character(
		{
			{0.025f, 0.025f, 0.025f},
			{ 0, 0, 0},
			{ 0,0,0,0 },
			{0.66f, 0.66f, 0.66f, 1},
			{},
			mainCharacterMesh,
			mainCharacterCollisionShape,
			mainCharacterCollisionActor
		}
	);
	if (mainCharacterMesh->LoadFBX("C:\\VisualStudioProjects\\cpp_game_engine\\assets\\BaseMesh_Anim_Triangle.fbx")) {
		this->mainCharacter->attachMeshDeformer(mainCharacterMeshDeformer);
		this->mainCharacter->setAnimation("Root|Root|Root|Idle|Root|Idle");
		this->pWorld->addEntity(this->mainCharacter);

		this->pMainController->setMainCharacter(this->mainCharacter);

		// Add secondary camera.
		Camera* pEntityCamera = new Camera(
			Vector3(0.0f, 0.0f, 0.0f),
			FOV,
			WIDTH / HEIGHT
		);
		pEntityCamera->followEntity(this->mainCharacter, Vector3(0, 5, 0), Vector3(0, 0, 6));
		pWorld->addCamera(pEntityCamera, true);
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