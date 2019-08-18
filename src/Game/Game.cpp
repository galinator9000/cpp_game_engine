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
		Vector3(1.5f, 0.05f, 1.5f),
		Vector3(0.0f, 2.0f, 6.0f),
		Vector3(10.0f, 0.0f, 10.0f),
		Vector3(0, 0, 0)
	);
	this->pWorld->addEntity(box);

	// Add small cubes
	for (int i = 1; i < 500; i++) {
		Box* box2 = new Box(
			Vector3(0.01f, 1.0f, 0.01f),
			Vector3(i * 0.01f, i * 5.0f, 6.0f),
			Vector3(0.0f, 0.0f, 0.0f),
			Vector3(0, 0, 0)
		);
		this->pWorld->addEntity(box2);
	}
}

void Game::Update(){

}