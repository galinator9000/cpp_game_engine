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
	/*Plane* ground = new Plane(
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
	}*/

	// Create texture objects.
	Texture* texture = new Texture("texture0", "C:\\VisualStudioProjects\\cpp_game_engine\\assets\\texture.dds");
	TextureSampler* textureSampler = new TextureSampler();

	// Create texture on GPU side.
	this->pWorld->createTexture(texture);
	this->pWorld->createTextureSampler(textureSampler);

	TriangleMesh* tMesh = new TriangleMesh(
		{ 1,1,1 },
		{ 0,0,10 },
		{ 0,0,0 },
		{ 0.66f, 0.66f, 0.66f, 1 },
		{ 1,1,1 }
	);
	if (tMesh->LoadFBX("C:\\VisualStudioProjects\\cpp_game_engine\\assets\\BaseMesh_Triangle.fbx")) {
		tMesh->attachTextureAndSampler(texture, textureSampler);
		this->pWorld->addEntity(tMesh);
	}

	DirectionalLight* directionalLight = new DirectionalLight(Vector3(-1.0f, 1.0f, -1.0f), 1.0f);
	this->pWorld->addLight(directionalLight);

	PointLight* pointLight = new PointLight(Vector3(-3.0f, 3.0f, -3.0f), 1.0f);
	this->pWorld->addLight(pointLight);
}

void Game::Update(){
	// Circular motion around 0,0,0 for point light.
	float cosx = cos(timer.Peek() * 6.28f) * 5.0f;
	float siny = sin(-timer.Peek() * 6.28f) * 5.0f;

	this->pWorld->allEntities.at(0)->Translate({ cosx * 0.1f, siny * 0.1f, 0.0f});
	this->pWorld->allEntities.at(0)->Scale({ siny * 0.01f, 0.0f, siny * 0.05f });
	
	//this->pWorld->allLights.at(0)->setDirection({ cosx, siny, cosx });
	this->pWorld->allLights.at(1)->setPosition({ cosx, siny, 0.0f });

	if (timer.Peek() > 1.0f) {
		timer.Reset();
	}
}