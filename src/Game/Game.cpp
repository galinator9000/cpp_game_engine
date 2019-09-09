#include "Game.h"

Game::Game(World* pWorld, Controller* pMainController){
	this->pWorld = pWorld;
	this->pMainController = pMainController;
}

void Game::Setup(){
	// Setup game scene.
	// Add main camera.
	this->pWorld->addCamera(&this->wMainCamera);

	//// Load animated entity.
	Mesh* animatedMesh = new Mesh();
	MeshDeformer* animatedMeshDeformer = new MeshDeformer();
	animatedEntity = new Entity(
		{0.025f, 0.025f, 0.025f},
		{ 0, 0, 10},
		{0,0,0,0},
		{0.66f, 0.66f, 0.66f, 1},
		{1,1,1},
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