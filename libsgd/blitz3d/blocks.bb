
Const KEY_ESCAPE = 256
Const KEY_SPACE = 32
					
Const KEY_RIGHT = 262
Const KEY_LEFT = 263
Const KEY_DOWN = 264
Const KEY_UP = 265

Const KEY_W = 87
Const KEY_A = 65
Const KEY_S = 83
Const KEY_D = 68

Const NUM_BLOCKS = 5000
Const WORLD_SIZE = 100

Type Camera
	Field entity
	Field xr#,yr#,vz#,vx#
End Type

Type Block
	Field model
	Field xr#,yr#
End Type

Type Bullet
	Field model
	Field timeout
	Field vz#
End Type

Global camera.Camera
Global bulletMesh

CreateWindow(DesktopWidth(), DesktopHeight(), "スノー Blocks", 3)

SetWebGPUBackend "D3D11"

CreateScene()

LoadScene()

While (PollEvents() And 1) <> 1

	If KeyHit(KEY_ESCAPE)
		Delete Each Camera
		Delete Each Bullet
		Delete Each Block
		ClearScene()
		LoadScene()
	EndIf

	UpdateCamera()
	
	UpdateBullets()
	
	UpdateBlocks()
	
	Clear2D()
	
	Draw2DText "FPS:"+FPS(),0,0
	
	RenderScene()
	
	Present()
Wend

Function LoadScene() 

	Local env =  LoadTexture("sgd://envmaps/stormy-cube.jpg", 4, 56)
	SetSceneEnvTexture env
	
	Local skybox = CreateSkybox()
	SetSkyboxTexture skybox,env
	
	Local material = CreatePrelitMaterial()
	bulletMesh = CreateSphereMesh(.5,24,12,material)
	SetMeshCastsShadow bulletMesh,0

	Local light = CreateDirectionalLight()
	SetLightColor light,1,1,1,.2
	TurnEntity light,-60,0,0

	CreateGround()

	CreateBlocks()

	CreateCamera()

End Function

Function UpdateBlocks()

	For block.Block = Each Block
		TurnEntity block\model,block\xr,block\yr,0
	Next

End Function

Function UpdateBullets() 

	If KeyHit(KEY_SPACE)
		Local r#=Rnd(1), g# = Rnd(1), b#=Rnd(1)
		
		Local bullet.Bullet = New Bullet
		
		bullet\model = CreateModel()
		SetModelMesh bullet\model, bulletMesh
		SetModelColor bullet\model,r,g,b,1
		
		SetEntityParent bullet\model,camera\entity
		SetEntityParent bullet\model,0
		
		bullet\timeout = 180
		bullet\vz = camera\vz + 1
		If bullet\vz<1 bullet\vz=1
		
		Local light=CreatePointLight()
		SetLightCastsShadow light,True
		SetEntityParent light,bullet\model
		SetLightColor light,r,g,b,2
		SetLightRange light,50
	EndIf

	For bullet.Bullet = Each Bullet
		bullet\timeout = bullet\timeout - 1
		If bullet\timeout = 0
			DestroyEntity bullet\model
			Delete bullet
		Else
			MoveEntity bullet\model,0,0,bullet\vz
		EndIf
	Next

End Function

Function CreateGround()

;	Local material = LoadPBRMaterial("sgd://materials/Fabric050_1K-JPG")
	Local material = LoadPBRMaterial("sgd://materials/Gravel023_1K-JPG")

	Local mesh = CreateBoxMesh(-WORLD_SIZE * 2,-1,-WORLD_SIZE*2,WORLD_SIZE*2,0,WORLD_SIZE*2,material)
	TransformMeshTexCoords(mesh,100,100,0,0)

	Local model = CreateModel()
	SetModelMesh model, mesh
	
End Function

Function CreateBlocks()

	Local material = LoadPBRMaterial("sgd://materials/Fabric048_1K-JPG")
;	Local material = LoadPBRMaterial("sgd://materials/PaintedWood09C_1K-JPG")
	Local mesh = CreateBoxMesh(-1,-1,-1,1,1,1,material)
	SetMeshCastsShadow mesh,True
	
	For i=1 To NUM_BLOCKS
		block.Block = New Block
		block\model = CreateModel()
		SetModelMesh block\model, mesh
		SetModelColor block\model, Rnd(1), Rnd(1), Rnd(1), 1
		SetEntityPosition block\model, Rnd(-WORLD_SIZE,WORLD_SIZE), Rnd(1,WORLD_SIZE), Rnd(-WORLD_SIZE,WORLD_SIZE)
		TurnEntity block\model, Rnd(360), Rnd(360), 0
		block\xr = Rnd(3)
		block\yr = Rnd(3)
	Next
	
End Function

Function CreateCamera()

	camera = New Camera
	camera\entity = CreatePerspectiveCamera()
	SetCameraNear camera\entity, .1
	SetCameraFar camera\entity, 1000
	MoveEntity camera\entity, 0,50,-100
	
	Local light = CreateSpotLight()
	SetLightColor light,1,1,1,1
	SetLightRange light,50
	SetEntityParent light,camera\entity
	
End Function

Function UpdateCamera()
	
	If KeyDown(KEY_LEFT)
		camera\yr = camera\yr + (1.5-camera\yr) * .3
	Else If KeyDown(KEY_RIGHT)
		camera\yr = camera\yr + (-1.5-camera\yr) * .3
	Else
		camera\yr = camera\yr * .9
	EndIf
	RotateEntity camera\entity,0,camera\yr,0
	
	If KeyDown(KEY_UP)
		camera\xr = camera\xr + (-1.5-camera\xr) * .3
	ElseIf KeyDown(KEY_DOWN)
		camera\xr = camera\xr + (1.5-camera\xr) * .3
	Else 
		camera\xr = camera\xr * .9
	EndIf
	TurnEntity camera\entity,camera\xr,0,0

	If KeyDown(KEY_W)
		camera\vz = camera\vz + (.5-camera\vz) * .3
	Else If KeyDown(KEY_S)
		camera\vz = camera\vz + (-.5 - camera\vz) * .3
	Else
		camera\vz = camera\vz * .9
	EndIf
	MoveEntity camera\entity,0,0,camera\vz

	If KeyDown(KEY_A)
		camera\vx = camera\vx + (-.5-camera\vx) * .3
	Else If KeyDown(KEY_D)
		camera\vx = camera\vx + (.5-camera\vx) * .3
	Else
		camera\vx = camera\vx * .9
	EndIf
	MoveEntity camera\entity,camera\vx,0,0
	
End Function