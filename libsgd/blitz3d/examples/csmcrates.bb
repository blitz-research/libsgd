
Include "start.bb"

CreateWindow(1280, 720, "Primitives", 2)

env = LoadTexture("sgd://envmaps/sunnysky-cube.png", 4, 56)
SetEnvTexture env

Local skybox = CreateSkybox(env)
SetSkyboxRoughness skybox, .3

CreatePlayer(0)
MoveEntity player,0,15,-25

Local light = CreateDirectionalLight()
SetLightShadowMappingEnabled light,True
TurnEntity light,-75,15,0

Local sz#=32
Local groundMaterial = LoadPBRMaterial("sgd://misc/brownish-grass.jpg")
Local groundMesh = CreateBoxMesh(-sz, -1, -sz, sz, 0, sz, groundMaterial)
TFormMeshTexCoords groundMesh, sz, sz, 0,0
Local groundModel = CreateModel(groundMesh)

Local crateMesh = LoadMesh("sgd://models/sci-fi_crate.glb")
SetMeshShadowCastingEnabled crateMesh,True
FitMesh CrateMesh,-.4,-.4,-.4,.4,.4,.4,True
For x=-20 To 20
	For z=-20 To 20
		Local crateModel = CreateModel(crateMesh)
		MoveEntity crateModel,x*1.25,30-Sqr(x*x+z*z),z*1.25
	Next
Next

While (PollEvents() And 1)<>1

	PlayerFly(.1)

	RenderScene()
	Present()
Wend