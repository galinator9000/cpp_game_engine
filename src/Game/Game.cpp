#include "Game.h"

Game::Game(World* pWorld, Controller* pMainController){
	this->pWorld = pWorld;
	this->pMainController = pMainController;
}

void Game::Setup(){
	// Setup game scene.
	// Add main camera.
	this->pWorld->addCamera(&this->wMainCamera);

	//// Create boxes.
	Mesh* boxMesh = new Mesh();
	boxMesh->createBoxGeometry({ 1,1,1 });

	// Dynamic boxes.
	/*CollisionShape* dynamicBoxColShape = new CollisionShape();
	dynamicBoxColShape->createBoxGeometry({ 1, 1, 1 });
	Entity* box;
	for (int b = 0; b < 500; b++) {
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
	}*/

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
			NULL,
			groundBoxColShape,
			groundBoxColActor
		}
	);
	this->pWorld->addEntity(groundBox);

	//// Load animated entity.
	// Collision.
	CollisionShape* mainCharacterCollisionShape = new CollisionShape();
	CollisionActor* mainCharacterCollisionActor = new CollisionActor(COLLISION_ACTOR_CCT);

	Mesh* mainCharacterMesh = new Mesh();
	mainCharacter = new Character(
		{
			{ 0.1f, 0.1f, 0.1f },
			{ 0, 0, 0},
			{ 0,0,0,0 },
			{0.66f, 0.66f, 0.66f, 1},
			{},
			mainCharacterMesh,
			mainCharacterCollisionShape,
			mainCharacterCollisionActor
		},
		{
			{0, 0, 1}
		}
	);

	// Load mesh.
	//if (mainCharacterMesh->LoadFBX("C:\\VisualStudioProjects\\cpp_game_engine\\assets\\attackcombo01.fbx", "")) {
	//if (mainCharacterMesh->LoadFBX("C:\\VisualStudioProjects\\cpp_game_engine\\assets\\BaseMesh_Animated.fbx", "")) {
	if (mainCharacterMesh->LoadFBX("C:\\VisualStudioProjects\\cpp_game_engine\\assets\\BaseMesh_Animated_blender.fbx", "Mesh")) {
		// Set Animation
		MeshDeformer* mainCharacterMeshDeformer = new MeshDeformer();
		mainCharacter->attachMeshDeformer(mainCharacterMeshDeformer);
		this->mainCharacter->setAnimation("Root|Root|Take 001|BaseLayer");

		// Set Texture
		/*Texture* mainCharacterTexture = new Texture("dying", "C:\\VisualStudioProjects\\cpp_game_engine\\assets\\texture.dds");
		TextureSampler* mainCharacterTextureSampler = new TextureSampler();
		this->pWorld->createTexture(mainCharacterTexture);
		this->pWorld->createTextureSampler(mainCharacterTextureSampler);
		this->mainCharacter->attachTextureAndSampler(mainCharacterTexture, mainCharacterTextureSampler);*/

		// Add entity to world.
		this->pWorld->addEntity(mainCharacter);
		this->pMainController->setMainCharacter(mainCharacter);

		// Add secondary camera.
		Camera* pEntityCamera = new Camera(
			Vector3(0.0f, 0.0f, 0.0f),
			FOV,
			WIDTH / HEIGHT
		);
		pEntityCamera->followEntity(mainCharacter, Vector3(0, 5, 0), Vector3(0, 0, 6));
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