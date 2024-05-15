
Include "start.bb"

CreateWindow(1280, 720, "Primitives", 2)

CreateScene()

env = LoadTexture("sgd://envmaps/sunnysky-cube.png", 4, 56)
SetSceneEnvTexture env

Local skybox = CreateSkybox(env)
SetSkyboxRoughness skybox, .3

Local light0 = CreatePointLight()
SetLightRange light0, 50

CreatePlayer(0)
MoveEntity player,0,0,-4
Local light = CreatePointLight()
SetEntityParent light, player
SetLightRange light, 50

Local material = LoadPBRMaterial("sgd://materials/Fabric050_1K-JPG")

Local r# = .5

Local mesh0 = CreateSphereMesh(r, 96, 48, material)
Local model0 = CreateModel(mesh0)
MoveEntity model0, -2.5, 0, 0

Local r2#=r*.7071
Local mesh1 = CreateBoxMesh(-r2, -r2, -r2, r2, r2, r2, material)
Local model1 = CreateModel(mesh1)
MoveEntity model1, -1.25, 0, 0

Local mesh2 = CreateCylinderMesh(r * 2, r / 2, 96, material)
Local model2 = CreateModel(mesh2)
MoveEntity model2, 0, 0, 0

Local mesh3 = CreateConeMesh(r * 2, r / 2, 96, material)
Local model3 = CreateModel(mesh3)
MoveEntity model3, 1.25, 0, 0

Local mesh4 = CreateTorusMesh(r * .75, r * .25, 96, 48, material)
Local model4 = CreateModel(mesh4)
MoveEntity model4, 2.5, 0, 0

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
	
	PlayerFly(.05)
	
	Local collider = CameraPick(camera,MouseX(),MouseY(),1)
	
	Clear2D()
	Draw2DText "Picked collider:" + collider,0,0
	
	RenderScene()
	
	Present()
Wend