
Global camera
Global model

CreateWindow(1280, 720, "Cesium Man", 0)

CreateScene()

LoadScene()

atime#=0
While Not PollEvents()
	If KeyDown(263)
		TurnEntity model,0,3,0
	Else If KeyDown(262)
		TurnEntity model,0,-3,0
	EndIf
	
	If KeyDown(264)
		MoveEntity model,0,0,.03
	Else If KeyDown(265)
		MoveEntity model,0,0,-.03
	EndIf

	atime = atime + .03
	AnimateModel model,0,atime,2,1
	
	RenderScene()
	Present()
Wend

Function LoadScene()

	Local env = LoadTexture("sgd://envmaps/sunnysky-cube.png", 4, 56)
	SetSceneEnvTexture env

	Local skybox = CreateSkybox(env)
	SetSkyboxRoughness skybox,.3

	Local light = CreateDirectionalLight()
	SetLightCastsShadow light, True
	TurnEntity light,-30,0,0	; Tilt light down 30 degrees 

	camera = CreatePerspectiveCamera()
	MoveEntity camera,0,1,-5
	
	Local material = LoadPBRMaterial("sgd://materials/PavingStones065_1K-JPG")
	Local mesh = CreateBoxMesh(-10, -1, -10, 10, 0, 10, material)
	TransformMeshTexCoords mesh, 4,4,0,0
	Local ground = CreateModel(mesh)
	
	model = LoadBonedModel("sgd://models/cesiumman.glb", True)
	SetMeshCastsShadow ModelMesh(model),True

End Function