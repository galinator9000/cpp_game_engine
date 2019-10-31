#include "Game.h"

Game::Game(World* pWorld, Controller* pMainController){
	this->pWorld = pWorld;
	this->pMainController = pMainController;
}

void Game::Setup(){
	// Setup game scene.
	// Add main camera.
	//wMainCamera.setPerspectiveProjection(FOV, WIDTH / HEIGHT);
	wMainCamera.setOrthographicProjection(25, 25);
	this->pWorld->addCamera(&this->wMainCamera);

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
					{{ 0.5f, 0, 0, 1 }},
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
			}
			prevBox = box;
		}
	}

	// Ground box.
	CollisionShape* groundBoxColShape = new CollisionShape();
	CollisionActor* groundBoxColActor = new CollisionActor(COLLISION_ACTOR_STATIC);
	Entity* groundBox = new Entity(
		{
			{ 2000, 0.1f, 2000 },
			{ 0, 0, 0 },
			{ 0,0,0,0 },
			{{ 1, 1, 1, 1 }},
			{},
			boxMesh,
			groundBoxColShape,
			groundBoxColActor
		}
	);
	this->pWorld->addEntity(groundBox);

	// Dynamic big box.
	// Create Texture and Normal Mapping
	Texture* texture = new Texture("diffuse", "C:\\VisualStudioProjects\\cpp_game_engine\\assets\\brick_wall_diffuse.dds");
	Texture* normalMapTexture = new Texture("normalmap", "C:\\VisualStudioProjects\\cpp_game_engine\\assets\\brick_wall_normal.dds");
	TextureSampler* textureSampler = new TextureSampler();

	this->pWorld->createTexture(texture);
	this->pWorld->createTexture(normalMapTexture);
	this->pWorld->createTextureSampler(textureSampler);

	Mesh* bigBoxMesh = new Mesh();
	bigBoxMesh->createBoxGeometry({ 1,1,1 });
	//bigBoxMesh->LoadFBX("C:\\VisualStudioProjects\\cpp_game_engine\\assets\\box.fbx", "");

	CollisionShape* bigBoxColShape = new CollisionShape();
	CollisionActor* bigBoxColActor = new CollisionActor(COLLISION_ACTOR_DYNAMIC);
	Entity* bigBox = new Entity(
		{
			{ 3, 3, 3 },
			{ -20, 10, 0 },
			{ 0,0,0,0 },
			{{ 0, 0, 0.66f, 1 }, 1, 5},
			{},
			bigBoxMesh,
			bigBoxColShape,
			bigBoxColActor
		}
	);
	bigBox->attachTextureAndSampler(texture, textureSampler);
	bigBox->attachNormalMappingTexture(normalMapTexture, textureSampler);
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
			{{ 1, 1, 1, 1 }, 1, 5},
			{},
			mainCharacterMesh,
			mainCharacterCollisionShape,
			mainCharacterCollisionActor
		},
		{}
	);

	// Load mesh.
	if (mainCharacterMesh->LoadFBX("C:\\VisualStudioProjects\\cpp_game_engine\\assets\\character01.fbx", "")) {
		// Set Animation
		MeshDeformer* mainCharacterMeshDeformer = new MeshDeformer();
		mainCharacter->attachMeshDeformer(mainCharacterMeshDeformer);
		this->mainCharacter->setAnimation("Take 001");

		// Add entity to world.
		this->pWorld->addEntity(mainCharacter);
		this->pMainController->setMainCharacter(mainCharacter);

		// Add character camera.
		Camera* pCharacterCamera = new Camera(
			Vector3(0.0f, 0.0f, 0.0f),
			Vector3::ZAxis(),
			WIDTH, HEIGHT,
			PROJECTION_TYPE::PERSPECTIVE
		);
		pCharacterCamera->setPerspectiveProjection(FOV, WIDTH / HEIGHT);
		pCharacterCamera->followEntity(mainCharacter, Vector3(0, 1.5f, 0), Vector3(0, 0, 2.5f));
		pWorld->addCamera(pCharacterCamera);
	}

	//// Add lights to scene.
	DirectionalLight* directionalLight = new DirectionalLight(Vector3(0.25f, -1, 0.25f), 1, Color(0.95f, 0.95f, 0.74f));
	//this->pWorld->addLight(directionalLight);

	SpotLight* wMainCameraSpotLight = new SpotLight(Vector3(), Vector3(), 1, Color(0.66f, 0.66f, 0.66f), dx::XM_PIDIV4, true);
	this->pWorld->addLight(wMainCameraSpotLight);

	PointLight* pointLight = new PointLight(Vector3(0, 5, 0), 1, Color(1,1,1));
	//this->pWorld->addLight(pointLight);

	//// Attach point light to main camera.
	// Position relation, camera to point light.
	VectorRelation* camPositionPL = new Vector3Relation(
		&(this->wMainCamera.camPosition),
		&(pointLight->gPosition),
		VECTOR_RELATION_TYPE::COPY,
		&(this->wMainCamera.dataChanged),
		&(pointLight->dataChanged)
	);
	this->pWorld->addVectorRelation(camPositionPL);

	//// Create secondary camera, attach spot light to it.
	Camera* pwSecondaryCamera = new Camera(
		Vector3(-10.0f, 10.0f, 0.0f),
		Vector3::ZAxis() + -Vector3::XAxis(),
		WIDTH, HEIGHT,
		PROJECTION_TYPE::PERSPECTIVE
	);
	pwSecondaryCamera->setPerspectiveProjection(FOV, WIDTH / HEIGHT);
	pWorld->addCamera(pwSecondaryCamera, true);

	// Position relation, camera to spot light.
	VectorRelation* camPositionSL = new Vector3Relation(
		&(pwSecondaryCamera->camPosition),
		&(wMainCameraSpotLight->gPosition),
		VECTOR_RELATION_TYPE::COPY,
		&(pwSecondaryCamera->dataChanged),
		&(wMainCameraSpotLight->dataChanged)
	);
	this->pWorld->addVectorRelation(camPositionSL);

	// Direction relation, camera to spot light.
	VectorRelation* camDirectionSL = new Vector3Relation(
		&(pwSecondaryCamera->lookDirection),
		&(wMainCameraSpotLight->gDirection),
		VECTOR_RELATION_TYPE::COPY,
		&(pwSecondaryCamera->dataChanged),
		&(wMainCameraSpotLight->dataChanged)
	);
	this->pWorld->addVectorRelation(camDirectionSL);
}

void Game::Update(){
	if (timer.Peek() > 1.0f) {
		timer.Reset();
	}

	// Circular motion
	float cosx = cos(timer.Peek() * dx::XM_2PI) * 5.0f;
	float siny = sin(timer.Peek() * dx::XM_2PI) * 5.0f;
}