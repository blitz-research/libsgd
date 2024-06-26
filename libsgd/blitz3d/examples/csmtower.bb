
Include "start.bb"

CreateWindow(GetDesktopWidth()/2, GetDesktopHeight()/2, "La Tour Eiffel!", 4)

Local light = CreateDirectionalLight()
SetLightShadowMappingEnabled light,True
SetEntityRotation light,-30,0,0

Local env =  LoadTexture("sgd://envmaps/sunnysky-cube.png", 4, 56)
SetEnvTexture env

Local skybox = CreateSkybox(env)

Local sz#=1000
Local groundMaterial = LoadPBRMaterial("sgd://misc/brownish-grass.jpg")
Local groundMesh = CreateBoxMesh(-sz/2,-1,-sz/2,sz/2,0,sz/2,groundMaterial)
TFormMeshTexCoords groundMesh,sz,sz,0,0
Local ground = CreateModel(groundMesh)
CreateMeshCollider(ground, 0, 0)

sz=330
Local towerMesh = LoadMesh("sgd://models/eiffel_tower.glb")
SetMeshShadowCastingEnabled towerMesh,True
FitMesh(towerMesh,-sz/2,0,-sz/2,sz/2,sz,sz/2,True)
Local tower = CreateModel(towerMesh)
CreateMeshCollider(tower, 0, 0)

createPlayer(0)
MoveEntity player,0,1,0
CreateSphereCollider(player, 1, 1);

EnableCollisions 1,0,2

While (PollEvents() And 1)<>1

	PlayerFly(.25)
	
	RotateEntity light,0,.025,0
	
	UpdateColliders()
	
	RenderScene()
	
	Clear2D()
	Draw2DText "FPS:"+GetFPS(),0,0
	
	Present()
Wend
