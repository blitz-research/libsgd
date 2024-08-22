Dialect "modern"

Include "start.bb"

Const COLLIDER_TYPE_LEVEL=0
Const COLLIDER_TYPE_SPHERE=1
Const COLLIDER_TYPE_PLAYER=2

CreateWindow GetDesktopWidth()/2,GetDesktopHeight()/2,"Collide Demo",WINDOW_FLAGS_RESIZABLE

EnableCollisions COLLIDER_TYPE_PLAYER,COLLIDER_TYPE_LEVEL,COLLISION_RESPONSE_SLIDEXZ
EnableCollisions COLLIDER_TYPE_PLAYER,COLLIDER_TYPE_SPHERE,COLLISION_RESPONSE_SLIDE

SetAmbientLightColor 1,.9,.8,.2

Local env =  LoadCubeTexture("sgd://envmaps/sunnysky-cube.png",TEXTURE_FORMAT_ANY,TEXTURE_FLAGS_DEFAULT)
SetEnvTexture env

Local skybox = CreateSkybox(env)

Local light = CreateDirectionalLight()
TurnEntity light,-45,-45,0

Local levelMesh = LoadMesh("sgd://models/ManurewaDuplex.glb")

Local sz#=50
FitMesh levelMesh,-sz,-sz,-sz,sz,sz,sz,True

Local levelModel = CreateModel(levelMesh)
Local levelCollider = CreateMeshCollider(levelModel, COLLIDER_TYPE_LEVEL, 0)

Local sphereMesh = CreateSphereMesh(2.5,48,23,CreatePBRMaterial());
SetMaterialVector4f(GetSurfaceMaterial(GetSurface(sphereMesh,0)),"albedoColor4f",1,.75,0,1)

Local sphereModel = CreateModel(sphereMesh)
MoveEntity sphereModel,0,-6,36
Local sphereCollider = CreateMeshCollider(sphereModel, COLLIDER_TYPE_SPHERE, 0)

CreatePlayer(0)
SetEntityPosition player,-8,0,36
SetEntityRotation player,0,-145.5,0
MoveEntity camera,0,.8,0

;Skinny dude!
Local playerCollider = CreateEllipsoidCollider(player, COLLIDER_TYPE_PLAYER, .2, 1.8)

Local gravity#=-9.81/60.0/60.0
Local vel#=0
Local jumping=False

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
		jumping=True
	EndIf
		
	vel = vel + gravity
	MoveEntity(player, 0, vel, 0)
	
	UpdateColliders()
	
	vel = GetEntityY(player) - py
	
	If jumping
		If vel<=0 jumping = False
	Else
		If vel>0 vel=0
	EndIf
	
	Clear2D
	Set2DTextColor 0,0,0,1
	
	Draw2DText "Player position: "+GetEntityX(player)+", "+GetEntityY(player)+", "+GetEntityZ(player),0,0
	Draw2DText "Player rotation: "+GetEntityRX(player)+", "+GetEntityRY(player)+", "+GetEntityRZ(player),0,16

	For i=0 To GetCollisionCount(playerCollider)-1
		Draw2DText "Collision "+i+" Type:"+GetColliderType(GetCollisionCollider(playerCollider,i)),0,i*48+32
		Draw2DText "Collision "+i+" Point : "+GetCollisionX(playerCollider,i)+", "+GetCollisionY(playerCollider,i)+", "+GetCollisionZ(playerCollider,i),0,i*48+48
		Draw2DText "Collision "+i+" Normal: "+GetCollisionNX(playerCollider,i)+", "+GetCollisionNY(playerCollider,i)+", "+GetCollisionNZ(playerCollider,i),0,i*48+64
	Next
	
	RenderScene()
	Present()
	
Wend