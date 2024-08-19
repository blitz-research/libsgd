Dialect "modern"

Include "start.bb"

CreateWindow GetDesktopWidth()/2,GetDesktopHeight()/2,"Collide Demo",WINDOW_FLAGS_RESIZABLE

;SetAmbientLightColor 1,.9,.8,.2

Local env =  LoadCubeTexture("sgd://envmaps/sunnysky-cube.png",TEXTURE_FORMAT_ANY,TEXTURE_FLAGS_DEFAULT)
SetEnvTexture env

Local skybox = CreateSkybox(env)

Local light = CreateDirectionalLight()
TurnEntity light,-45,225, 0

Local levelMesh = LoadMesh("sgd://models/ManurewaDuplex.glb")

Local sz#=50
FitMesh levelMesh,-sz,-sz,-sz,sz,sz,sz,True

Local levelModel = CreateModel(levelMesh)
Local levelCollider = CreateMeshCollider(levelModel, 0, 0)

CreatePlayer(0)
SetEntityPosition player,-8,0,36
SetEntityRotation player,0,-145.5,0
MoveEntity camera,0,.8,0

;Skinny dude!
Local playerCollider = CreateEllipsoidCollider(player, 1, .2, 1.8)

EnableCollisions 1, 0, 2

Local gravity# =  -9.81/60.0/60.0
Local vel# = 0

While PollEvents()<>1

	Local sp#=3.2
	If IsKeyDown(KEY_LEFT_SHIFT) sp=5.6
	
	If IsKeyHit(KEY_F11)
		If GetWindowState()<>4
			SetWindowState 4
		Else
			SetWindowState 2
		EndIf
	EndIf
	
	PlayerWalk(sp/60.0)
	
	Local 	py# = GetEntityY(player)
	
	If IsKeyHit(KEY_SPACE)
		vel = vel + .1
	EndIf
		
	vel = vel + gravity
	MoveEntity(player, 0, vel, 0)
	
	UpdateColliders()
	
	Local newvel# = GetEntityY(player) - py
;	If newvel>vel newvel = vel
	vel = newvel
	
	Clear2D
	Set2DTextColor 0,0,0,1
	Draw2DText "Player position: "+GetEntityX(player)+", "+GetEntityY(player)+", "+GetEntityZ(player),0,0
	Draw2DText "Player rotation: "+GetEntityRX(player)+", "+GetEntityRY(player)+", "+GetEntityRZ(player),0,16

	If GetCollisionCount(playerCollider)>0
		Draw2DText "Collision Point : "+GetCollisionX(playerCollider,0)+", "+GetCollisionY(playerCollider,0)+", "+GetCollisionZ(playerCollider,0),0,32
		Draw2DText "Collision Normal: "+GetCollisionNX(playerCollider,0)+", "+GetCollisionNY(playerCollider,0)+", "+GetCollisionNZ(playerCollider,0),0,48
	EndIf
	
	RenderScene()
	Present()
	
Wend