
CreateWindow(1280, 720, "Helmet", 0)

CreateScene()

env = LoadTexture("sgd://envmaps/sunnysky-cube.png", 4, 56)

SetSceneEnvTexture env

skybox = CreateSkybox()
SetSkyboxTexture skybox,env
SetSkyboxRoughness skybox,.3

light = CreateDirectionalLight()
TurnEntity light,-45,0,0	; Tilt light down 45 degrees 

material = LoadPrelitMaterial("sgd://misc/grass1.png")
SetMaterialBlendMode material,2

Const n=10000
For i=0 To n
	sprite = CreateSprite()
	SetSpriteMaterial(sprite, material)
	TurnEntity(sprite,0,Rnd(360),0)
	MoveEntity(sprite,0,-1.5,Rnd(50))
Next

While Not PollEvents()
	RenderScene()
	Present()
Wend