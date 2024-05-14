
;Use space to toggle sprite view modes.

Include "start.bb"

CreateWindow(1280, 720, "Spritr Grass", 0)

CreateScene()

env = LoadTexture("sgd://envmaps/sunnysky-cube.png", 4, 56)

SetSceneEnvTexture env

skybox = CreateSkybox()
SetSkyboxTexture skybox,env
SetSkyboxRoughness skybox,.3

light = CreateDirectionalLight()
TurnEntity light,-45,0,0	; Tilt light down 45 degrees 

grassImage = LoadImage("sgd://misc/grass1.png", 4, 31, 1)

Const n=10000

For i=0 To n
	sprite = CreateSprite()
	SetSpriteImage(sprite, grassImage)
	TurnEntity(sprite,0,Rnd(360),0)
	MoveEntity(sprite,0,-1.5,Rnd(50))
Next

camera = CreatePerspectiveCamera()
pivot = CreateModel()
SetEntityParent camera, pivot
actor.Actor = CreateActor(pivot)

viewMode = 1 
SetImageSpriteViewMode grassImage, viewMode

While Not PollEvents()
	RenderScene()
	FlyActor(actor)
	SetEntityRotation camera,0,0,actor\yr * -15
	
	Clear2D()
	If KeyHit(KEY_SPACE)
		viewMode = viewMode + 1
		If viewMode=4 viewMode=1
		SetImageSpriteViewMode grassImage, viewMode
	EndIf
	Draw2DText "Sprite View mode:" + viewMode, 0, 0
	
	Present()
Wend