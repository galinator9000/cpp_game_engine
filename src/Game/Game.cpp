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

	// Create texture objects.
	Texture* texture = new Texture("texture0", "C:\\VisualStudioProjects\\cpp_game_engine\\assets\\texture.dds");
	TextureSampler* textureSampler = new TextureSampler();
	this->pWorld->createTexture(texture);
	this->pWorld->createTextureSampler(textureSampler);

	// Add boxes.
	/*
	Box* box;
	Mesh* boxMesh = new Mesh();
	boxMesh->createBoxShape();
	for (int i = 0; i < 50; i++) {
		box = new Box(
			{ 0.1f,0.1f,0.1f },
			{ i*0.1f,i*0.5f, -10 + (i*0.3f) },
			{ 0,0,0 },
			{ 0.0f, 0.66f, 1, 1 },
			{ 1,1,1 }
		);
		box->attachMesh(boxMesh);
		box->attachTextureAndSampler(texture, textureSampler);
		this->pWorld->addEntity(box);
	}
	*/

	Entity* entity = new Entity(
		{ 0.025f,0.025f,0.025f },
		{ 0,0,10 },
		{ 0,0,0 },
		{ 0.66f, 0.66f, 0.66f, 1 },
		{ 1,1,1 }
	);
	Mesh* tMesh = new Mesh();
	MeshDeformer* tMeshDeformer = new MeshDeformer();

	if (tMesh->LoadFBX("C:\\VisualStudioProjects\\cpp_game_engine\\assets\\BaseMesh_Anim_Triangle.fbx")) {
		// Attach texture and mesh to entities.
		tMesh->attachMeshDeformer(tMeshDeformer);

		entity->attachMesh(tMesh);
		entity->attachTextureAndSampler(texture, textureSampler);

		this->pWorld->addEntity(entity);
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
	
	//this->pWorld->allLights.at(0)->setDirection({ cosx, siny, cosx });
	this->pWorld->allLights.at(1)->setPosition({ cosx, siny, 0.0f });

	if (timer.Peek() > 1.0f) {
		timer.Reset();
	}
}