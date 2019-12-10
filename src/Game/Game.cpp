#include "Game.h"

Game::Game(World* pWorld, Controller* pMainController){
	this->pWorld = pWorld;
	this->pMainController = pMainController;
}

void Game::Setup(){
	// Setup game scene.
	// Add main camera.
	wMainCamera.setPerspectiveProjection(FOV, WIDTH / HEIGHT);
	this->pWorld->addCamera(&this->wMainCamera);

	// Create box mesh.
	Mesh* boxMesh = new Mesh();
	boxMesh->createBoxGeometry({ 1,1,1 });

	// Create dynamic boxes.
	CollisionShape* dynamicBoxColShape = new CollisionShape();
	dynamicBoxColShape->createBoxGeometry({ 0.1f, 0.1f, 0.1f });

	Entity* box;
	CollisionActor* boxColActor;
	CollisionMaterial* boxColMat = new CollisionMaterial();
	Material* boxMaterial = new Material(
		NULL, Color(0, 1, 0)
	);

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
				{ 0.1f, 0.1f, 0.1f },
				{ bb*3.0f, 5 + (b * marginBetweenBoxes), 10+(bb * 3.0f) },
				{ 0,0,0,0 },
				boxMaterial,
				boxColMat,
				boxMesh,
				dynamicBoxColShape,
				boxColActor
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
		{ 2000, 0.1f, 2000 },
		{ 0, 0, 0 },
		{ 0,0,0,0 },
		NULL,
		NULL,
		boxMesh,
		groundBoxColShape,
		groundBoxColActor
	);
	this->pWorld->addEntity(groundBox);

	// Dynamic big box.
	// Create Texture and Normal Mapping
	Texture* brickTexture = new Texture(
		"brick",
		"D:\\VisualStudioProjects\\cpp_game_engine\\assets\\brick_wall_diffuse.dds",
		"D:\\VisualStudioProjects\\cpp_game_engine\\assets\\brick_wall_normal.dds",
		"D:\\VisualStudioProjects\\cpp_game_engine\\assets\\brick_wall_specular.dds"
		//"D:\\VisualStudioProjects\\cpp_game_engine\\assets\\brick_wall_alpha.dds"
	);
	this->pWorld->createTexture(brickTexture);

	Mesh* bigBoxMesh = new Mesh();
	bigBoxMesh->createBoxGeometry({ 1,1,1 });
	//bigBoxMesh->LoadFBX("C:\\VisualStudioProjects\\cpp_game_engine\\assets\\box.fbx", "");

	CollisionShape* bigBoxColShape = new CollisionShape();
	CollisionActor* bigBoxColActor = new CollisionActor(COLLISION_ACTOR_DYNAMIC);
	CollisionMaterial* bigBoxColMaterial = new CollisionMaterial(1, 1, 1);
	Material* bigBoxMaterial = new Material(brickTexture, { 0, 0, 0.66f, 1 }, 5, 25);
	Entity* bigBox = new Entity(
		{ 3, 3, 3 },
		{ -20, 10, 0 },
		{ 0,0,0,0 },
		bigBoxMaterial,
		bigBoxColMaterial,
		bigBoxMesh,
		bigBoxColShape,
		bigBoxColActor
	);
	this->pWorld->addEntity(bigBox);

	//// Load animated entity.
	// Collision.
	CollisionShape* mainCharacterCollisionShape = new CollisionShape();
	CollisionActor* mainCharacterCollisionActor = new CollisionActor(COLLISION_ACTOR_CCT);
	Material* mainCharacterMaterial = new Material(NULL, { 1, 1, 1, 1 }, 1, 5);

	Mesh* mainCharacterMesh = new Mesh();
	mainCharacter = new Character(
		{ 0.01f, 0.01f, 0.01f },
		{ 0,0,0 },
		{ 0,0,0,0 },
		mainCharacterMaterial,
		NULL,
		mainCharacterMesh,
		mainCharacterCollisionShape,
		mainCharacterCollisionActor,
		0.1f, {0, 0, 1}
	);

	// Load mesh.
	if (mainCharacterMesh->LoadFBX("D:\\VisualStudioProjects\\cpp_game_engine\\assets\\character01.fbx", "")) {
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
		pWorld->addCamera(pCharacterCamera);
		this->mainCharacter->setCamera(pCharacterCamera);

		VectorRelation* characterCam = new Vector3Relation(
			&(this->mainCharacter->gPosition),
			&(pCharacterCamera->gPosition),
			VECTOR_RELATION_TYPE::RLTV,
			&(this->mainCharacter->dataChanged),
			&(pCharacterCamera->dataChanged),
			true,
			Vector3(0, 2.5f, -2.5f)
		);
		this->pWorld->addVectorRelation(characterCam);
	}

	//// Add lights to scene.
	this->directionalSunLight = new DirectionalLight(Vector3(-1, -1, 1), 0.15f, Color(0.99f, 0.72f, 0.074f), true, 3);
	this->pWorld->addLight(this->directionalSunLight);

	this->wMainCameraSpotLight = new SpotLight(Vector3(), Vector3(), 1, Color(0.66f, 0.66f, 0.66f), dx::XM_PIDIV4);
	this->pWorld->addLight(this->wMainCameraSpotLight);

	this->pointLight = new PointLight(Vector3(15, 5, 3), 1, Color(1,1,1));
	this->pWorld->addLight(this->pointLight);

	//// Create secondary camera, attach spot light to it.
	Camera* pwSecondaryCamera = new Camera(
		Vector3(-20, 5, -12),
		Vector3::ZAxis(),
		WIDTH, HEIGHT,
		PROJECTION_TYPE::PERSPECTIVE
	);
	pwSecondaryCamera->setPerspectiveProjection(FOV, WIDTH / HEIGHT);
	pWorld->addCamera(pwSecondaryCamera, true);

	// Position relation, last dynamic box to camera.
	VectorRelation* boxPositionCam = new Vector3Relation(
		&(box->gPosition),
		&(pwSecondaryCamera->gPosition),
		VECTOR_RELATION_TYPE::RLTV,
		&(box->dataChanged),
		&(pwSecondaryCamera->dataChanged),
		true,
		Vector3(0, 2.5f, 2.5f)
	);
	this->pWorld->addVectorRelation(boxPositionCam);

	// Position relation, camera to spot light.
	VectorRelation* camPositionSL = new Vector3Relation(
		&(pwSecondaryCamera->gPosition),
		&(this->wMainCameraSpotLight->gPosition),
		VECTOR_RELATION_TYPE::COPY,
		&(pwSecondaryCamera->dataChanged),
		&(this->wMainCameraSpotLight->dataChanged)
	);
	this->pWorld->addVectorRelation(camPositionSL);

	// Direction relation, camera to spot light.
	VectorRelation* camDirectionSL = new Vector3Relation(
		&(pwSecondaryCamera->gDirection),
		&(this->wMainCameraSpotLight->gDirection),
		VECTOR_RELATION_TYPE::COPY,
		&(pwSecondaryCamera->dataChanged),
		&(this->wMainCameraSpotLight->dataChanged)
	);
	this->pWorld->addVectorRelation(camDirectionSL);
}

void Game::Update(){
	if (timer.Peek() > 1.0f) {
		timer.Reset();
	}

	// Circular motion
	float cosx = cos(timer.Peek() * dx::XM_2PI) * 5;
	float siny = sin(timer.Peek() * dx::XM_2PI) * 5;
}

void Game::Reset() {

}