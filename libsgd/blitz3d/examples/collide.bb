Include "start.bb"

CreateWindow 1280, 720, "Hello World", 4

CreateScene()

SetSceneAmbientLightColor 1,.9,.8,.2

Local env =  LoadTexture("sgd://envmaps/sunnysky-cube.png", 4, 56)
SetSceneEnvTexture env

Local skybox = CreateSkybox(env)

Local light = CreateDirectionalLight()
TurnEntity light,-45,225, 0

Local levelMesh = LoadMesh("sgd://models/ManurewaDuplex.glb")

Local sz#=50
FitMesh levelMesh,-sz,-sz,-sz,sz,sz,sz,True

Local levelModel = CreateModel(levelMesh)
Local levelCollider = CreateMeshCollider(levelModel, 0, 0)

Local playerMesh = CreateSphereMesh(1,48,24,CreatePBRMaterial())

CreatePlayer(playerMesh)
MoveEntity player, 0, 2.5, -sz/2
MoveEntity camera, 0, .75, 0

playerCollider = CreateSphereCollider(player, 1, 1)

EnableCollisions 1, 0, 2

Local gravity# =  -9.81/60.0/60.0
Local vel# = 0

While PollEvents()<>1

	Local sp#=1.6
	If KeyDown(KEY_LEFT_SHIFT) sp=4.8

	PlayerWalk(sp/60.0)
	
	Local 	py# = EntityY(player)
	vel = vel + gravity
	MoveEntity(player, 0, vel, 0)
	
	UpdateColliders()
	
	vel = EntityY(player) - py
	If vel>0 vel=0
	
	RenderScene()
	Present()
	
Wend