
Include "start.bb"

Const NUM_BLOCKS = 10000
Const WORLD_SIZE = 100

Type Block
	Field model
	Field xr#,yr#
End Type

Type Bullet
	Field model
	Field timeout
	Field vz#
End Type

Global bulletMesh
Global slimeball

CreateWindow(DesktopWidth(), DesktopHeight(), "スノー Blocks", 1)

CreateScene()

LoadScene()

While (PollEvents() And 1) <> 1

	If KeyHit(KEY_ESCAPE)
		End
		Delete Each Bullet
		Delete Each Block
		ClearScene()
		LoadScene()
	EndIf
	
	PlayerFly(1)
	
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
	
	Local skybox = CreateSkybox(env)
	
	Local material = CreatePrelitMaterial()
	bulletMesh = CreateSphereMesh(.5,24,12,material)
	SetMeshCastsShadow bulletMesh,0

	Local light = CreateDirectionalLight()
	SetLightColor light,1,1,1,.2
	TurnEntity light,-60,0,0
	
	CreatePlayer(0)
	MoveEntity player, 0,50,-100
	
	light = CreateSpotLight()
	SetEntityParent light,player
	SetLightColor light,1,1,1,1
	SetLightRange light,50
	
	slimeball=LoadSound("sgd://audio/slimeball.wav")
	
	CreateGround()

	CreateBlocks()
	
End Function

Function UpdateBlocks()

	For block.Block = Each Block
		TurnEntity block\model,block\xr,block\yr,0
	Next

End Function

Function UpdateBullets() 

	If KeyHit(KEY_SPACE)
		Local r#=Rnd(1), g# = Rnd(1), b#=Rnd(1)
		
		;Fake some NEON baby!
		Select(Rand(3))
		Case 1 r=1
		Case 2 g=1
		Case 3 b=1
		End Select
		
		Local bullet.Bullet = New Bullet
		
		bullet\model = CreateModel(bulletMesh)
		SetModelColor bullet\model,r,g,b,1
		
		SetEntityParent bullet\model,player
		SetEntityParent bullet\model,0
		
		bullet\timeout = 180
		bullet\vz = player_vz + 1
		If bullet\vz<1 bullet\vz=1
		
		Local light=CreatePointLight()
		SetLightCastsShadow light,True
		SetEntityParent light,bullet\model
		SetLightColor light,r,g,b,2
		SetLightRange light,50
		
		PlaySound slimeball
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

	Local model = CreateModel(mesh)
	
End Function

Function CreateBlocks()

	Local material = LoadPBRMaterial("sgd://materials/Fabric048_1K-JPG")
;	Local material = LoadPBRMaterial("sgd://materials/PaintedWood09C_1K-JPG")
	Local mesh = CreateBoxMesh(-1,-1,-1,1,1,1,material)
	SetMeshCastsShadow mesh,True
	
	For i=1 To NUM_BLOCKS
		block.Block = New Block
		block\model = CreateModel(mesh)
		SetModelColor block\model, Rnd(1), Rnd(1), Rnd(1), 1
		SetEntityPosition block\model, Rnd(-WORLD_SIZE,WORLD_SIZE), Rnd(1,WORLD_SIZE), Rnd(-WORLD_SIZE,WORLD_SIZE)
		TurnEntity block\model, Rnd(360), Rnd(360), 0
		block\xr = Rnd(3)
		block\yr = Rnd(3)
	Next
	
End Function