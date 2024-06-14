
Include "start.bb"

CreateWindow(1280, 720, "Primitives", 2)

env = LoadTexture("sgd://envmaps/sunnysky-cube.png", 4, 56)
SetEnvTexture env

Local skybox = CreateSkybox(env)
SetSkyboxRoughness skybox, .3

SetPSMTextureSize 4096
SetMaxPSMLights 6

Local light0 = CreatePointLight()
SetEntityPosition light0,0,5,-2.5
SetLightRange light0, 10
SetLightCastsShadow light0,True 
;Local light = CreatePointLight()
;SetEntityParent light, player
;'SetLightRange light, 50

Local groundMaterial = CreatePBRMaterial()
SetMaterialVector4f groundMaterial, "albedoColor4f",.6,.5,.5,1
SetMaterialFloat groundMaterial, "roughnessFactor1f",0.5
Local groundMesh = CreateBoxMesh(-5,-.1,-5,5,0,5,groundMaterial)
SetMeshCastsShadow groundMesh, True
Local groundModel = CreateModel(groundMesh)

Local material = LoadPBRMaterial("sgd://materials/Fabric050_1K-JPG")

Local r# = .5
Local y# = 1.5

CreatePlayer(0)
MoveEntity player,0,y,-5

Local mesh0 = CreateSphereMesh(r, 96, 48, material)
SetMeshCastsShadow mesh0, True
Local model0 = CreateModel(mesh0)
MoveEntity model0, -2.5, y, 0

Local r2#=r*.7071
Local mesh1 = CreateBoxMesh(-r2, -r2, -r2, r2, r2, r2, material)
SetMeshCastsShadow mesh1, True
Local model1 = CreateModel(mesh1)
MoveEntity model1, -1.25, y, 0

Local mesh2 = CreateCylinderMesh(r * 2, r / 2, 96, material)
SetMeshCastsShadow mesh2, True
Local model2 = CreateModel(mesh2)
MoveEntity model2, 0, y, 0

Local mesh3 = CreateConeMesh(r * 2, r / 2, 96, material)
SetMeshCastsShadow mesh3, True
Local model3 = CreateModel(mesh3)
MoveEntity model3, 1.25, y, 0

Local mesh4 = CreateTorusMesh(r * .75, r * .25, 96, 48, material)
SetMeshCastsShadow mesh4, True
Local model4 = CreateModel(mesh4)
MoveEntity model4, 2.5, y, 0

Local collider0=CreateMeshCollider(model0,0,0)
Local collider1=CreateMeshCollider(model1,0,0)
Local collider2=CreateMeshCollider(model2,0,0)
Local collider3=CreateMeshCollider(model3,0,0)
Local collider4=CreateMeshCollider(model4,0,0)

Set2DTextColor 0,0,0,1

While (PollEvents() And 1) <> 1

	TurnEntity model0, .3, .4, .5
	TurnEntity model1, .3, .4, .5
	TurnEntity model2, .3, .4, .5
	TurnEntity model3, .3, .4, .5
	TurnEntity model4, .3, .4, .5
	
	If GetKeyHit(49) SetEntityVisible model0, Not GetEntityVisible(model0)
	If GetKeyHit(50) SetEntityVisible model1, Not GetEntityVisible(model1)
	If GetKeyHit(51) SetEntityVisible model2, Not GetEntityVisible(model2)
	If GetKeyHit(52) SetEntityVisible model3, Not GetEntityVisible(model3)
	If GetKeyHit(53) SetEntityVisible model4, Not GetEntityVisible(model4)
	
	PlayerFly(.05)
	
	Local collider = CameraPick(camera,GetMouseX(),GetMouseY(),1)
	
	Clear2D()
	Draw2DText "Picked collider:" + collider,0,0
	
	RenderScene()
	Present()
Wend
