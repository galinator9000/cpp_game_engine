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
Main window class
```
Window* pMainWnd = new Window(hInstance, WND_TITLE, WIDTH, HEIGHT);
```

InputHandler class is a wrapper for keyboard and mouse classes, also create Keyboard & Mouse objects.
```
EventHandler* pEventHandler = new EventHandler();
InputHandler* pInputHandler = new InputHandler();

Keyboard* pKeyb = new Keyboard();
Mouse* pMouse = new Mouse();
```

Create graphics and physics component
```
Graphics* pGfx = new Graphics(pMainWnd->GetHandler(), WIDTH, HEIGHT, GRAPHICS_REFRESH_RATE);
Physics* pPhy = new Physics(GRAVITY, PHYSICS_REFRESH_RATE);
Character::worldGravity = GRAVITY / PHYSICS_REFRESH_RATE;
```

Create world and game classes.
World integrates physics and graphics components.
Game setups the game scene.
```
World* pWorld = new World(pGfx, pPhy, pAud);
pWorld->Setup();

Game* pGame = new Game(pWorld, pInputHandler);
pGame->Setup();
```

Connect window and controller with keyboard and mouse.
```
pMainWnd->Setup(pKeyb, pMouse);
pInputHandler->Setup(pKeyb, pMouse, pEventHandler, pWorld);
```

# Setting the scene with Game calls
