#include "Game.h"

#include "Light/DirectionalLight.h"

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
		{ 0.0f, 0.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f, 1.0f },
		{ 0, 0, 0 }
	);
	this->pWorld->addEntity(box);

	// Add small cubes
	for (int i = 1; i < 150; i++) {
		Box* box2 = new Box(
			{ 0.1f, 0.1f, 0.1f },
			{ i * 0.01f, 5.0f + (i * 0.5f), 6.0f },
			{ 0.0f, 0.0f, 0.0f },
			{ 0.0f, 0.0f, 0.5f, 1.0f },
			{ 0, 0, 0 }
		);
		this->pWorld->addEntity(box2);
	}

	TriangleMesh* tMesh = new TriangleMesh(
		{ 1.0f, 1.0f, 1.0f },
		{ 0, 0, 0 },
		{ 0.0f, 0.0f, 0.0f },
		{ 0.2f, 0.2f, 0.2f, 1.0f },
		{ 0, 0, 0 }
	);
	if (tMesh->LoadFBX("C:\\VisualStudioProjects\\cpp_game_engine\\assets\\deer_small.fbx")){
		this->pWorld->addEntity(tMesh);
	}

	DirectionalLight* dirLight = new DirectionalLight(Vector3(1.0f, 1.0f, 1.0f), 1.0f);
	this->pWorld->addLight(dirLight);
}

void Game::Update(){

}