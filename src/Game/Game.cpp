#include "Game.h"

// Utils
#include "utils/OBJ_Loader.h"
objl::Loader Loader;

Game::Game(World* pWorld){
	this->pWorld = pWorld;
}

void Game::Setup(){
	// Add static ground.
	Plane* ground = new Plane(
		PxVec3(10.0f, 10.0f, 0)
	);
	this->pWorld->addEntity(ground);

	// Add dynamic cube.
	Box* box = new Box(
		PxVec3(1.5f, 0.05f, 1.5f),
		PxVec3(0.0f, 2.0f, 6.0f),
		PxVec3(10.0f, 0.0f, 10.0f),
		PxVec3(0, 0, 0)
	);
	this->pWorld->addEntity(box);

	// Add small cubes
	for (int i = 1; i < 500; i++) {
		Box* box2 = new Box(
			PxVec3(0.01f, 1.0f, 0.01f),
			PxVec3(i * 0.01f, i * 5.0f, 6.0f),
			PxVec3(0.0f, 0.0f, 0.0f),
			PxVec3(0, 0, 0)
		);
		this->pWorld->addEntity(box2);
	}

	// Load OBJ file
	bool loadResult = Loader.LoadFile("C:\\VisualStudioProjects\\cpp_game_engine\\assets\\deer.obj");
	if(loadResult){
		objl::Mesh mesh = Loader.LoadedMeshes[0];
		mesh.MeshName;
	}
}
void Game::Update(){

}