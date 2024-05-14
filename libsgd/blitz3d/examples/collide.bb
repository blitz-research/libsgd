Include "start.bb"

CreateWindow 1280, 720, "Hello World", 4

CreateScene()

SetSceneAmbientLightColor 1,.9,.8,.2

Local env =  LoadTexture("sgd://envmaps/sunnysky-cube.png", 4, 56)
SetSceneEnvTexture env

Local skybox = CreateSkybox()
SetSkyboxTexture skybox,env

Local light = CreateDirectionalLight()
TurnEntity light,-45,225, 0

Local levelMesh = LoadMesh("sgd://models/ManurewaDuplex.glb")

FitMesh levelMesh,-50,-50,-50,50,50,50,True

Local levelModel = CreateModel()
SetModelMesh levelModel, levelMesh

Local levelCollider = CreateMeshCollider(levelModel, 0, levelMesh)

Local playerMesh = CreateSphereMesh(1,48,24,CreatePBRMaterial())

Local playerModel = CreateModel()
SetModelMesh playerModel, playerMesh
MoveEntity playerModel, 0, 2.5, -30

Local fpCamera = CreatePerspectiveCamera()
SetEntityParent fpCamera, playerModel

playerCollider = CreateSphereCollider(playerModel, 1, 1)

EnableCollisions 1, 0, 2

Local gravity# =  -9.81 / 60.0 / 60.0
Local vel# = 0
While PollEvents()<>1
	
	If KeyDown(KEY_LEFT)
		TurnEntity playerModel, 0, 1, 0
	Else If KeyDown(KEY_RIGHT)
		TurnEntity playerModel, 0, -1, 0
	EndIf
	If KeyDown(KEY_W)
		MoveEntity playerModel, 0, 0, .1
	Else If KeyDown(KEY_S)
		MoveEntity playerModel, 0, 0, -.1
	EndIf
	
	Local 	pos# = EntityY(playerModel)
	MoveEntity(playerModel, 0, vel + gravity, 0)
	
	UpdateColliders()
	
	vel = EntityY(playerModel) - pos
	
	RenderScene()
	Present()
	
Wend