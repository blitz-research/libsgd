CreateWindow 1280,720,"Hello World!",2

CreateScene()

env = LoadTexture("sgd://envmaps/sunnysky-cube.png",4,56)

SetSceneEnvTexture env

skybox = CreateSkybox()
SetSkyboxTexture skybox,env
SetSkyboxRoughness skybox, .3

Clear2D

Draw2DText "Hello World!",0,0

; Draw some lines
;Set2DLineWidth 5

Local i

For i=1 To 100
	Set2DFillColor Rnd(1),Rnd(1),Rnd(1),1
	Draw2DLine Rnd(1920), Rnd(1080), Rnd(1920), Rnd(1080)
Next

Set2DOutlineEnabled True
;Set2DOutlineWidth 3

; Draw some outlined rects
For i=1 To 100
	w=Rnd(32,64)
	h=Rnd(32,64)
	x=Rnd(1920-w)
	y=Rnd(1080-h)
	Set2DFillColor Rnd(1),Rnd(1),Rnd(1),1
	Draw2DRect x, y, x+w, y+h
Next

Set2DOutlineColor 0,255,0,1

; Draw some ovals
For i=1 To 100
	w=Rnd(32,64)
	h=Rnd(32,64)
	x=Rnd(1920-w)
	y=Rnd(1080-h)
	Set2DFillColor Rnd(1),Rnd(1),Rnd(1),1
	Draw2DOval x, y, x+w, y+h
Next

While (PollEvents() And 1) = 0
	
	RenderScene()
	
	Present()

Wend