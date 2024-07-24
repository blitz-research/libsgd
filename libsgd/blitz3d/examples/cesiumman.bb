
Global camera
Global model

CreateWindow(1280, 720, "Cesium Man", 0)

LoadScene()

;Cheesy sepia-tone render effect!
Local monocolor = CreateMonocolorEffect()
SetMonocolorEffectColor monocolor, 1,1,.75,1

atime#=0
While Not PollEvents()
	If IsKeyDown(263)
		TurnEntity model,0,3,0
	Else If IsKeyDown(262)
		TurnEntity model,0,-3,0
	EndIf
	
	If IsKeyDown(264)
		MoveEntity model,0,0,.03
	Else If IsKeyDown(265)
		MoveEntity model,0,0,-.03
	EndIf

	atime = atime + .03
	AnimateModel model,0,atime,2,1
	
	RenderScene()
	Present()
Wend

Function LoadScene()

	Local env = LoadTexture("sgd://envmaps/sunnysky-cube.png", 4, 56)
	SetEnvTexture env

	Local skybox = CreateSkybox(env)
	SetSkyboxRoughness skybox,.3

	Local light = CreateDirectionalLight()
	SetLightShadowMappingEnabled light, True
	TurnEntity light,-30,0,0	; Tilt light down 30 degrees 

	camera = CreatePerspectiveCamera()
	MoveEntity camera,0,1,-5
	
	Local material = LoadPBRMaterial("sgd://materials/PavingStones065_1K-JPG")
	Local mesh = CreateBoxMesh(-10, -1, -10, 10, 0, 10, material)
	TFormMeshTexCoords mesh, 4,4,0,0
	Local ground = CreateModel(mesh)
	
	model = LoadBonedModel("sgd://models/cesiumman.glb", True)
	SetMeshShadowCastingEnabled GetModelMesh(model),True

End Function