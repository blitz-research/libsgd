
Include "start.bb"

CreateWindow(GetDesktopWidth()/2, GetDesktopHeight()/2, "La Tour Eiffel!", 4)

;Create fog
Local fog = CreateFogEffect()
SetFogEffectColor fog,.3,.6,1,1
SetFogEffectPower fog,2

Local light = CreateDirectionalLight()
SetLightShadowsEnabled light,True
SetEntityRotation light,-30,0,0

Local env =  LoadTexture("sgd://envmaps/sunnysky-cube.png", 4, 56)
SetEnvTexture env

SetClearColor .3,.6,1,1
;Local skybox = CreateSkybox(env)

; Height of eiffel tower, fact fans!
Local sz#=330

SetCSMClipRange sz * 2

Local groundMaterial = LoadPBRMaterial("sgd://misc/grass1K.jpg")
Local groundMesh = CreateBoxMesh(-sz,0,-sz,sz,0,sz,groundMaterial)
TFormMeshTexCoords groundMesh,sz,sz,0,0
Local ground = CreateModel(groundMesh)
CreateMeshCollider(ground, 0, 0)

Local towerMesh = LoadMesh("sgd://models/eiffel_tower.glb")
SetMeshShadowsEnabled towerMesh,True
FitMesh(towerMesh,-sz/2,0,-sz/2,sz/2,sz,sz/2,True)
Local tower = CreateModel(towerMesh)
CreateMeshCollider(tower, 0, 0)

createPlayer(0)
MoveEntity player,0,1,0
CreateSphereCollider(player, 1, 1);

EnableCollisions 1,0,2

While (PollEvents() And 1)<>1

	PlayerFly(1.25)
	
	RotateEntity light,0,.025,0
	
	UpdateColliders()
	
	RenderScene()
	
	Clear2D()
	Draw2DText "FPS:"+GetFPS(),0,0
	
	Present()
Wend