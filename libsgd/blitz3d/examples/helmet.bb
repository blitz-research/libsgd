
CreateWindow(1280, 720, "Helmet", 0)

env = LoadTexture("sgd://envmaps/sunnysky-cube.png", 4, 56)
SetEnvTexture env

skybox = CreateSkybox(env)
SetSkyboxRoughness skybox, .3

light = CreateDirectionalLight()
TurnEntity light,-45,0,0	; Tilt light down 45 degrees 

material = LoadPBRMaterial("sgd://materials/Tiles019_1K-JPG")
mesh = CreateBoxMesh(-10,-3,-10,10,-2,10,material)
TFormTexCoords mesh,3,3,0,0
ground = CreateModel(mesh)

mesh = LoadMesh("sgd://models/helmet.glb")
model=CreateModel(mesh)
MoveEntity model,0,0,3

While Not PollEvents()

	If GetKeyDown(263)
		TurnEntity model,0,3,0
	Else If GetKeyDown(262)
		TurnEntity model,0,-3,0
	EndIf
	
	If GetKeyDown(264)
		TurnEntity model,3,0,0
	Else If GetKeyDown(265)
		TurnEntity model,-3,0,0
	EndIf
	
	RenderScene()
	
	Present()
	
Wend