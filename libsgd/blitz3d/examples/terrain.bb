Dialect "modern"

Include "start.bb"

CreateWindow GetDesktopWidth()/2,GetDesktopHeight()/2,"Canyon!",WINDOW_FLAGS_CENTERED

SetAmbientLightColor 1,.9,.8,.1

Local light = CreateDirectionalLight()
SetLightShadowsEnabled light,True
SetEntityRotation light,-45,-45,0

Local env =  LoadCubeTexture("sgd://envmaps/sunnysky-cube.png",TEXTURE_FORMAT_ANY,TEXTURE_FLAGS_DEFAULT)
SetEnvTexture env
CreateSkybox(env)

Local heightTexture = Load2DTexture("sgd://terrains/canyon/height.exr", TEXTURE_FORMAT_ANY, TEXTURE_FLAGS_DEFAULT)
Local normalTexture = Load2DTexture("sgd://terrains/canyon/normal.png", TEXTURE_FORMAT_RGBA8, TEXTURE_FLAGS_DEFAULT)
Local albedoTexture = Load2DTexture("sgd://terrains/canyon/albedo.png", TEXTURE_FORMAT_ANY, TEXTURE_FLAGS_DEFAULT)

;Local heightTexture = Load2DTexture("~/Desktop/rocky/height.exr", TEXTURE_FORMAT_ANY, TEXTURE_FLAGS_IMAGE)
;Local normalTexture = Load2DTexture("~/Desktop/rocky/normal.png", TEXTURE_FORMAT_RGBA8, TEXTURE_FLAGS_IMAGE)
;Local albedoTexture = Load2DTexture("~/Desktop/rocky/albedo.png", TEXTURE_FORMAT_ANY, TEXTURE_FLAGS_IMAGE)

Local material=CreatePBRMaterial()
SetMaterialTexture material,"albedo",albedoTexture

Local terrain = CreateTerrain()

SetTerrainSize terrain,4096
SetTerrainLODs terrain,5
SetTerrainMaterialSize terrain,4096
SetTerrainHeightTexture terrain,heightTexture
SetTerrainNormalTexture terrain,normalTexture
SetTerrainMaterial terrain,material

SetEntityScale terrain, 1,512,1

createPlayer(0)
MoveEntity player,0,512,0

Local debug=0

While (PollEvents() And 1)<>1

	Local speed#=.25
	If IsKeyDown(KEY_LEFT_SHIFT) speed = 2.5
	
	PlayerFly(speed)
	
	If IsKeyHit(KEY_SPACE)
		debug=1-debug
		SetTerrainDebugMode terrain, debug
	EndIf
	
	Clear2D()
	Draw2DText "Debug (space to toggle): "+debug,0,0
	Draw2DText "FPS: "+GetFPS(),0,16
	
	RenderScene()
	
	Present()
Wend