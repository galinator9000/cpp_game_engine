#include "Game.h"

Game::Game(World* pWorld){
	this->pWorld = pWorld;
}

void Game::Setup(){
	// Add main camera.
	Camera* pCamera = new Camera(
		Vector3(0.0f, 3.0f, 1.0f),
		FOV,
		WIDTH / HEIGHT
	);
	pWorld->addCamera(pCamera, true);

	// Add static ground.
	Plane* ground = new Plane(
		Vector3(10.0f, 10.0f, 0)
	);
	this->pWorld->addEntity(ground);

	// Add dynamic cube.
	Box* box = new Box(
		{ 1.5f, 0.05f, 1.5f },
		{ 0.0f, 2.0f, 6.0f },
		{ 0, 0, 0 },
		{ 0.0f, 1.0f, 0.0f, 1.0f },
		{ 0, 0, 0 }
	);
	this->pWorld->addEntity(box);

	// Add small cubes
	for (int i = 1; i < 150; i++) {
		Box* box2 = new Box(
			{ 0.1f, 0.1f, 0.1f },
			{ i * 0.01f, 5.0f + (i * 0.5f), 6.0f },
			{ 0, 0, 0 },
			{ 0, 0, 1, 1 },
			{ 0, 0, 0 }
		);
		this->pWorld->addEntity(box2);
	}

	// Suzanne model
	TriangleMesh* tMesh = new TriangleMesh(
		{ 1, 1, 1 },
		{ 0, 0, 0 },
		{ 0, 0, 0 },
		{ 1, 1, 1, 1 },
		{ 0, 0, 0 }
	);
	if (tMesh->LoadFBX("C:\\VisualStudioProjects\\cpp_game_engine\\assets\\suzanne.fbx")){
		this->pWorld->addEntity(tMesh);
	}

	// Deer
	TriangleMesh* tMesh2 = new TriangleMesh(
		{ 1, 1, 1 },
		{ 0, 0, 10 },
		{ 0, 0, 0 },
		{ 1, 1, 1, 1 },
		{ 0, 0, 0 }
	);
	if (tMesh2->LoadFBX("C:\\VisualStudioProjects\\cpp_game_engine\\assets\\deer_small.fbx")) {
		this->pWorld->addEntity(tMesh2);
	}

	// Cube
	TriangleMesh* tMesh3 = new TriangleMesh(
		{ 1, 1, 1 },
		{ 10, 0, 0 },
		{ 0, 0, 0 },
		{ 1, 1, 1, 1 },
		{ 0, 0, 0 }
	);
	if (tMesh3->LoadFBX("C:\\VisualStudioProjects\\cpp_game_engine\\assets\\cube.fbx")) {
		this->pWorld->addEntity(tMesh3);
	}

	// Cube but sharp
	TriangleMesh* tMesh4 = new TriangleMesh(
		{ 1, 1, 1 },
		{ -10, 0, 0 },
		{ 0, 0, 0 },
		{ 1, 1, 1, 1 },
		{ 0, 0, 0 }
	);
	if (tMesh4->LoadFBX("C:\\VisualStudioProjects\\cpp_game_engine\\assets\\cube_sharp.fbx")) {
		this->pWorld->addEntity(tMesh4);
	}

	PointLight* pointLight = new PointLight(Vector3(-3.0f, 3.0f, -3.0f), 1.0f);
	this->pWorld->addLight(pointLight);

	//DirectionalLight* directionalLight = new DirectionalLight(Vector3(1.0f, -1.0f, 1.0f), 1.0f);
	//this->pWorld->addLight(directionalLight);
}

void Game::Update(){
	float cosx = cos(timer.Peek() * 6.28f) * 5.0f;
	float siny = sin(-timer.Peek() * 6.28f) * 5.0f;
	
	this->pWorld->allLights.at(0)->setPosition(
		{ cosx, siny, siny }
	);

	if (timer.Peek() > 1.0f) {
		timer.Reset();
	}
}