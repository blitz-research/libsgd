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
SetEntityPosition player,-8,-4,36
SetEntityRotation player,0,-145.5,0
MoveEntity camera,0,.8,0

; I am NOT this skinny!
playerCollider = CreateEllipsoidCollider(player, 1, .2, 1.8)

EnableCollisions 1, 0, 2

Local gravity# =  -9.81/60.0/60.0
Local vel# = 0

While PollEvents()<>1

	Local sp#=3.2
	If KeyDown(KEY_LEFT_SHIFT) sp=5.6

	PlayerWalk(sp/60.0)
	
	Local 	py# = EntityY(player)
	
	If KeyHit(KEY_SPACE)
		vel = vel + .1
	EndIf
		
	vel = vel + gravity
	MoveEntity(player, 0, vel, 0)
	
	UpdateColliders()
	
	Local newvel# = EntityY(player) - py
;	If newvel>vel newvel = vel
	vel = newvel
	
	Clear2D
	Draw2DText EntityX(camera)+", "+EntityY(camera)+", "+EntityZ(camera),0,0
	Draw2DText EntityRX(camera)+", "+EntityRY(camera)+", "+EntityRZ(camera),0,16
	
	
	RenderScene()
	Present()
	
Wend