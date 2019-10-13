#include "Game.h"

Game::Game(World* pWorld, Controller* pMainController){
	this->pWorld = pWorld;
	this->pMainController = pMainController;
}

void Game::Setup(){
	// Setup game scene.
	// Add main camera.
	this->pWorld->addCamera(&this->wMainCamera);

	// Create Texture and Normal Mapping
	Texture* texture = new Texture("diffuse", "C:\\VisualStudioProjects\\cpp_game_engine\\assets\\brick_wall_diffuse.dds");
	Texture* normalMapTexture = new Texture("normalmap", "C:\\VisualStudioProjects\\cpp_game_engine\\assets\\brick_wall_normal.dds");
	TextureSampler* textureSampler = new TextureSampler();

	this->pWorld->createTexture(texture);
	this->pWorld->createTexture(normalMapTexture);
	this->pWorld->createTextureSampler(textureSampler);

	// Create box mesh.
	Mesh* boxMesh = new Mesh();
	boxMesh->createBoxGeometry({ 1,1,1 });

	// Create dynamic boxes.
	CollisionShape* dynamicBoxColShape = new CollisionShape();
	dynamicBoxColShape->createBoxGeometry({ 0.1f, 0.1f, 0.1f });

	Entity* box;
	CollisionActor* boxColActor;

	Entity* prevBox = NULL;

	float marginBetweenBoxes = 0.25f;
	for (int bb = 0; bb<3; bb++) {
		for (int b = 0; b < 25; b++) {
			if (b == 0) {
				prevBox = NULL;
				boxColActor = new CollisionActor(COLLISION_ACTOR_STATIC);
			}
			else {
				boxColActor = new CollisionActor(COLLISION_ACTOR_DYNAMIC);
			}
			
			box = new Entity(
				{
					{ 0.1f, 0.1f, 0.1f },
					{ bb*3.0f, 5 + (b * marginBetweenBoxes), 10+(bb * 3.0f) },
					{ 0,0,0,0 },
					{ 0.5f, 0, 0, 1 },
					{},
					boxMesh,
					dynamicBoxColShape,
					boxColActor
				}
			);

			this->pWorld->addEntity(box);

			// Create joint.
			if (prevBox != NULL) {
				this->pWorld->createSphericalJoint(
					prevBox, box,
					Vector3(0, marginBetweenBoxes / 2, 0),
					Vector3(0, -(marginBetweenBoxes / 2), 0)
				);

				box->attachTextureAndSampler(texture, textureSampler);
				box->attachNormalMappingTexture(normalMapTexture);
			}
			prevBox = box;
		}
	}

	// Ground box.
	CollisionShape* groundBoxColShape = new CollisionShape();
	groundBoxColShape->createBoxGeometry({ 2000, 0.1f, 2000 });
	CollisionActor* groundBoxColActor = new CollisionActor(COLLISION_ACTOR_STATIC);
	Entity* groundBox = new Entity(
		{
			{ 2000, 0.1f, 2000 },
			{ 0, 0, 0 },
			{ 0,0,0,0 },
			{ 0.1f, 0.1f, 0.1f, 1 },
			{},
			boxMesh,
			groundBoxColShape,
			groundBoxColActor
		}
	);
	this->pWorld->addEntity(groundBox);

	// Dynamic big box.
	CollisionShape* bigBoxColShape = new CollisionShape();
	bigBoxColShape->createBoxGeometry({ 3,2,3 });
	CollisionActor* bigBoxColActor = new CollisionActor(COLLISION_ACTOR_DYNAMIC);
	Entity* bigBox = new Entity(
		{
			{ 3,2,3 },
			{ -20, 10, 0 },
			{ 0,0,0,0 },
			{ 0, 1, 0, 1 },
			{},
			boxMesh,
			bigBoxColShape,
			bigBoxColActor
		}
	);
	this->pWorld->addEntity(bigBox);

	//// Load animated entity.
	// Collision.
	CollisionShape* mainCharacterCollisionShape = new CollisionShape();
	CollisionActor* mainCharacterCollisionActor = new CollisionActor(COLLISION_ACTOR_CCT);

	Mesh* mainCharacterMesh = new Mesh();
	mainCharacter = new Character(
		{
			{ 0.01f, 0.01f, 0.01f },
			{ 0,0,0 },
			{ 0,0,0,0 },
			{ 1,1,1,1 },
			{},
			mainCharacterMesh,
			mainCharacterCollisionShape,
			mainCharacterCollisionActor
		},
		{

		}
	);

	// Load mesh.
	if (mainCharacterMesh->LoadFBX("C:\\VisualStudioProjects\\cpp_game_engine\\assets\\character01_uv.fbx", "")) {
		// Set Animation
		MeshDeformer* mainCharacterMeshDeformer = new MeshDeformer();
		mainCharacter->attachMeshDeformer(mainCharacterMeshDeformer);
		this->mainCharacter->setAnimation("Take 001");

		this->mainCharacter->attachTextureAndSampler(texture, textureSampler);
		this->mainCharacter->attachNormalMappingTexture(normalMapTexture);

		// Add entity to world.
		this->pWorld->addEntity(mainCharacter);
		this->pMainController->setMainCharacter(mainCharacter);

		// Add secondary camera.
		Camera* pEntityCamera = new Camera(
			Vector3(0.0f, 0.0f, 0.0f),
			FOV,
			WIDTH / HEIGHT
		);
		pEntityCamera->followEntity(mainCharacter, Vector3(0, 1.5f, 0), Vector3(0, 0, 2.5f));
		pWorld->addCamera(pEntityCamera);
	}

	// Add lights to scene.
	PointLight* pointLight = new PointLight(Vector3(20, 5, -10.0f), 0.5f, Color(1, 0, 0));
	//this->pWorld->addLight(pointLight);

	DirectionalLight* directionalLight = new DirectionalLight(Vector3(1, -1, -1), 0.3f, Color(1, 1, 1));
	//this->pWorld->addLight(directionalLight);

	this->spotLight = new SpotLight(Vector3(0, 5, 0), Vector3(0, -1, 0), 1.0f, Color(1, 1, 1));
	this->pWorld->addLight(this->spotLight);
}

void Game::Update(){
	if (timer.Peek() > 1.0f) {
		timer.Reset();
	}

	// Circular motion
	float cosx = cos(timer.Peek() * dx::XM_2PI) * 5.0f;
	float siny = sin(timer.Peek() * dx::XM_2PI) * 5.0f;
}