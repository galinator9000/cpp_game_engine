Basic 3D Game Engine with usage of DirectX 11 &amp; PhysX on C++

# Features
- DirectX 11 as graphics engine
- PhysX as physics engine
- Basic lighting (point light, directional light, spot light)
- Basic shadows (shadow mapping using depth)
- Basic event timer
- Entity system
- Camera system (field-of-view setting, perspective & orthogonal projection)
- Textures, normal maps while rendering
- Skeleton animation system
- Being able to load FBX models (Autodesk format) with animation

# Initialization
### Main window class
```
Window* pMainWnd = new Window(hInstance, WND_TITLE, WIDTH, HEIGHT);
```

### Create input handler objects
InputHandler class is a wrapper for keyboard and mouse classes, also create Keyboard & Mouse objects.
```
EventHandler* pEventHandler = new EventHandler();
InputHandler* pInputHandler = new InputHandler();

Keyboard* pKeyb = new Keyboard();
Mouse* pMouse = new Mouse();
```

### Create graphics and physics component
```
Graphics* pGfx = new Graphics(pMainWnd->GetHandler(), WIDTH, HEIGHT, GRAPHICS_REFRESH_RATE);
Physics* pPhy = new Physics(GRAVITY, PHYSICS_REFRESH_RATE);
Character::worldGravity = GRAVITY / PHYSICS_REFRESH_RATE;
```

### Create world and game objects
World integrates physics and graphics components.

Game setups the game scene.
```
World* pWorld = new World(pGfx, pPhy, pAud);
pWorld->Setup();

Game* pGame = new Game(pWorld, pInputHandler);
pGame->Setup();
```

### Connect window and controller with keyboard & mouse
```
pMainWnd->Setup(pKeyb, pMouse);
pInputHandler->Setup(pKeyb, pMouse, pEventHandler, pWorld);
```

# Setting the scene
## Setup game scene

### Add main camera
```
wMainCamera.setPerspectiveProjection(FOV, WIDTH / HEIGHT);
this->pWorld->addCamera(&this->wMainCamera);
```

### Create box mesh dynamically
```
Mesh* boxMesh = new Mesh();
boxMesh->createBoxGeometry({ 1,1,1 });
```

### Adding ground
#### Create static box object and add to world as an entity for ground.
```
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
```

### Load animated character
```
// Collision.
CollisionShape* mainCharacterCollisionShape = new CollisionShape();
CollisionActor* mainCharacterCollisionActor = new CollisionActor(COLLISION_ACTOR_CCT);
Material* mainCharacterMaterial = new Material(NULL, { 1, 1, 1, 1 }, 1, 5);

// Create bash mesh object
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
if (mainCharacterMesh->LoadFBX("CHARACTER_FBX_PATH", "")) {
	// Set Animation
	MeshDeformer* mainCharacterMeshDeformer = new MeshDeformer();
	mainCharacter->attachMeshDeformer(mainCharacterMeshDeformer);
	this->mainCharacter->setAnimation("Take 001");

	// Add entity to world.
	this->pWorld->addEntity(mainCharacter);
	this->pInputHandler->setMainCharacter(mainCharacter);

	// Add character camera.
	Camera* pCharacterCamera = new Camera(
		Vector3(0.0f, 0.0f, 0.0f),
		Vector3::ZAxis(),
		WIDTH, HEIGHT,
		PROJECTION_TYPE::PERSPECTIVE
	);
	pCharacterCamera->setPerspectiveProjection(FOV, WIDTH / HEIGHT);
	pWorld->addCamera(pCharacterCamera, true);
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
```

### Add lights to scene
#### Directional Sun Light
```
this->directionalSunLight = new DirectionalLight(Vector3(-1, -1, 1), 0.15f, Color(0.99f, 0.72f, 0.074f), true, 3);
this->pWorld->addLight(this->directionalSunLight);
```

#### Spot Light
```
this->wMainCameraSpotLight = new SpotLight(Vector3(), Vector3(), 1, Color(0.66f, 0.66f, 0.66f), dx::XM_PIDIV4);
this->pWorld->addLight(this->wMainCameraSpotLight);
```

#### Point Light
```
this->pointLight = new PointLight(Vector3(15, 5, 3), 1, Color(1,1,1));
this->pWorld->addLight(this->pointLight);
```

### Add one more camera to scene
```
//// Create secondary camera, attach spot light to it.
Camera* pwSecondaryCamera = new Camera(
	Vector3(-20, 5, -12),
	Vector3::ZAxis(),
	WIDTH, HEIGHT,
	PROJECTION_TYPE::PERSPECTIVE
);
pwSecondaryCamera->setPerspectiveProjection(FOV, WIDTH / HEIGHT);
pWorld->addCamera(pwSecondaryCamera);
```

### Attaching vectors
Attach camera position&direction vectors with spot light position&direction vector.

So they can navigate the scene together.
```
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
```
