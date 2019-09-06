#include "Game.h"

Game::Game(World* pWorld){
	this->pWorld = pWorld;
}

void Game::Setup(){
	// Setup game scene.
	// Add main camera.
	Camera* pCamera = new Camera(
		Vector3(0.0f, 0.0f, 0.0f),
		FOV,
		WIDTH / HEIGHT
	);
	pWorld->addCamera(pCamera, true);

	// Add static ground to world.
	Plane* ground = new Plane({ 0,1,0 }, { 1,1,0 });
	this->pWorld->addEntity(ground);

	// Create box mesh.
	// This mesh will be shared between all boxes.
	Mesh* boxMesh = new Mesh();
	boxMesh->createBoxShape();

	// Create big box.
	Box* box;
	box = new Box(
		{ 1, 1, 1 },
		{ 0, 2, 10 },
		{ 0,0,0,0 },
		{ 1,1,1,1 },
		{ 1,1,0 },
		boxMesh
	);
	this->pWorld->addEntity(box);

	// Create small boxes.
	for (int i = 0; i < 25; i++) {
		box = new Box(
			{ 0.05f, 1.0f, 0.05f },
			{ 0, (i*5.0f) + 30.0f, 10 },
			{ 0,0,0,0 },
			{ 1,1,0,1 },
			{ 1,1,0 },
			boxMesh
		);
		this->pWorld->addEntity(box);
	}

	PointLight* pointLight = new PointLight(Vector3(0, 0, -10.0f), 1.0f);
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