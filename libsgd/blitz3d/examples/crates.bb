Dialect "modern"

Include "start.bb"

CreateWindow GetDesktopWidth()/2,GetDesktopHeight()/2,"Crates",WINDOW_FLAGS_CENTERED

env = LoadCubeTexture("sgd://envmaps/sunnysky-cube.png", TEXTURE_FORMAT_ANY, TEXTURE_FLAGS_DEFAULT)
SetEnvTexture env

Local skybox = CreateSkybox(env)
SetSkyboxRoughness skybox, .3

CreatePlayer(0)
MoveEntity player,0,15,-25

Local light = CreateDirectionalLight()
SetLightShadowsEnabled light,True
TurnEntity light,-90,0,0

Local sz#=32
Local groundMaterial = LoadPBRMaterial("sgd://misc/brownish-grass.jpg")
Local groundMesh = CreateBoxMesh(-sz, -1, -sz, sz, 0, sz, groundMaterial)
TransformTexCoords groundMesh, sz, sz, 0,0
Local groundModel = CreateModel(groundMesh)

Local crateMesh = LoadMesh("sgd://models/sci-fi_crate.glb")
SetMeshShadowsEnabled crateMesh,True
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