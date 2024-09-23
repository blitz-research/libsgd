Dialect "modern"

Include "start.bb"

CreateWindow GetDesktopWidth()/2,GetDesktopHeight()/2,"Primitives",WINDOW_FLAGS_CENTERED

env = LoadCubeTexture("sgd://envmaps/sunnysky-cube.png",TEXTURE_FORMAT_ANY,TEXTURE_FLAGS_DEFAULT)
SetEnvTexture env

Local skybox = CreateSkybox(env)
SetSkyboxRoughness skybox, .3

SetPSMTextureSize 4096
SetMaxPSMLights 6

Local light0 = CreatePointLight()
SetEntityPosition light0,0,5,-2.5
SetLightRange light0, 10
SetLightShadowsEnabled light0,True

Local groundMaterial = CreatePBRMaterial()
SetMaterialVector4f groundMaterial,"albedo",.6,.5,.5,1

Local groundMesh = CreateBoxMesh(-5,-.1,-5,5,0,5,groundMaterial)
SetMeshShadowsEnabled groundMesh, True

Local groundModel = CreateModel(groundMesh)

Local material = LoadPBRMaterial("sgd://materials/Fabric050_1K-JPG")

Local r# = .5
Local y# = 1.5

CreatePlayer(0)
MoveEntity player,0,y,-5

Local mesh0 = CreateSphereMesh(r, 96, 48, material)
SetMeshShadowsEnabled mesh0, True
Local model0 = CreateModel(mesh0)
MoveEntity model0, -2.5, y, 0

Local r2#=r*.7071
Local mesh1 = CreateBoxMesh(-r2, -r2, -r2, r2, r2, r2, material)
SetMeshShadowsEnabled mesh1, True
Local model1 = CreateModel(mesh1)
MoveEntity model1, -1.25, y, 0

Local mesh2 = CreateCylinderMesh(r * 2, r / 2, 96, material)
SetMeshShadowsEnabled mesh2, True
Local model2 = CreateModel(mesh2)
MoveEntity model2, 0, y, 0

Local mesh3 = CreateConeMesh(r * 2, r / 2, 96, material)
SetMeshShadowsEnabled mesh3, True
Local model3 = CreateModel(mesh3)
MoveEntity model3, 1.25, y, 0

Local mesh4 = CreateTorusMesh(r * .75, r * .25, 96, 48, material)
SetMeshShadowsEnabled mesh4, True
Local model4 = CreateModel(mesh4)
MoveEntity model4, 2.5, y, 0

Local collider0=CreateMeshCollider(model0,0,0)
Local collider1=CreateMeshCollider(model1,0,0)
Local collider2=CreateMeshCollider(model2,0,0)
Local collider3=CreateMeshCollider(model3,0,0)
Local collider4=CreateMeshCollider(model4,0,0)

Set2DTextColor 0,0,0,1

Local cursorMesh = CreateSphereMesh(.01, 32,16,CreatePBRMaterial())
Local cursor= CreateModel(cursorMesh)
SetModelColor cursor, 0, 1, 0, 1

While (PollEvents() And 1) <> 1

	TurnEntity model0, .3, .4, .5
	TurnEntity model1, .3, .4, .5
	TurnEntity model2, .3, .4, .5
	TurnEntity model3, .3, .4, .5
	TurnEntity model4, .3, .4, .5
	
	If IsKeyHit(49) SetEntityVisible model0, Not IsEntityVisible(model0)
	If IsKeyHit(50) SetEntityVisible model1, Not IsEntityVisible(model1)
	If IsKeyHit(51) SetEntityVisible model2, Not IsEntityVisible(model2)
	If IsKeyHit(52) SetEntityVisible model3, Not IsEntityVisible(model3)
	If IsKeyHit(53) SetEntityVisible model4, Not IsEntityVisible(model4)
	
	PlayerFly(.05)
	
	Local collider = CameraPick(camera,GetMouseX(),GetMouseY(),1)
	
	Clear2D()
	Draw2DText "Picked collider:" + collider,0,0
	
	CameraUnproject camera, GetMouseX(), GetMouseY(), 1
	SetEntityPosition cursor, GetUnprojectedX(), GetUnprojectedY(), GetUnprojectedZ()
	
	RenderScene()
	Present()
Wend
