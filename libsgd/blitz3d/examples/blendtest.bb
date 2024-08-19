Dialect "modern"

Include "start.bb"

CreateWindow GetDesktopWidth()/2, GetDesktopHeight()/2, "Blend test", WINDOW_FLAGS_CENTERED

SetAmbientLightColor 1,1,1,.2

Local env =  LoadCubeTexture("sgd://envmaps/stormy-cube.jpg", TEXTURE_FORMAT_ANY, TEXTURE_FLAGS_DEFAULT)
SetEnvTexture env
CreateSkybox env

Local light = CreateDirectionalLight()
SetEntityRotation light,-45,-45,0

Local mesh = LoadMesh("sgd://models/blendtest.glb")
FitMesh mesh,-.5,-.5,-.5,.5,.5,.5,True

Local cells = 5
Local spc#= 2
Local halfsz# = cells*spc/2

Type Model
	Field entity
End Type

For x = 0 To cells-1
	For y=0 To cells-1
		For z=0 To cells-1
			Local model:Model=New Model
			model.entity=CreateModel(mesh)
			MoveEntity model.entity,x*spc-halfsz, y*spc-halfsz,z*spc-halfsz
		Next
	Next
Next

CreatePlayer(0)
MoveEntity player,0,0,-halfsz

While Not (PollEvents() And EVENT_MASK_CLOSE_CLICKED)

	SeedRnd 1234
	
	For model:model = Each Model
		TurnEntity model.entity,Rnd(-.7,.7),Rnd(-.3,.3),0
	Next
	
	PlayerFly(.125)
	
	RenderScene
	Present

Wend