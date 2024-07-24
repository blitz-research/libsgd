
Function createSphere(radius#, xSegs, ySegs, material)

	Local mesh = CreateMesh(0,0)
	
	Local fxSegs# = 1/Float(xSegs), fySegs# = 1/Float(ySegs)
	
	For i=0 To xSegs-1
		AddMeshVertex mesh, 0, radius, 0, 0, 1, 0, (Float(i) + .5) * 2 * fxSegs, 0
	Next
	For j = 1 To ySegs-1
		Local pitch# = HALF_PI - Float(j) * Pi * fySegs;
		For i = 0 To xSegs
			Local yaw# = Float(i) * TWO_PI / fxSegs;
			Local r# = Cos(pitch);
			Local y# = Sin(pitch);
			Local x# = Cos(yaw) * r;
			Local z# = Sin(yaw) * r;
			AddMeshVertex mesh, x * radius, y * radius, z * radius, x, y, z, Float(i) * 2 * fxSegs, Float(j) * fySegs
		Next
	Next
	For i = 0 To xSegs-1
		AddMeshVertex mesh, 0, -radius, 0, 0, -1, 0, (Float(i) + .5) * 2 * fxSegs, 1
	Next
	
	Local surf = CreateSurface(mesh, 0, material);
	
	For i = 0 To xSegs-1
		AddSurfaceTriangle surf, i, i + xSegs, i + xSegs + 1
	Next
	
	For j = 1 To ySegs-2
		For i = 0 To xSegs-1
			Local v0 = j * (xSegs + 1) + i - 1
			Local v2 = v0 + xSegs + 2
			AddSurfaceTriangle surf, v0, v2, v0 + 1
			AddSurfaceTriangle surf, v0, v2 - 1, v2
		Next
	Next
	For i = 0 To xSegs-1
		v0 = (xSegs + 1) * (ySegs - 1) + i - 1
		AddSurfaceTriangle surf, v0, v0 + xSegs + 1, v0 + 1
	Next
	
	Return mesh
	
End Function

CreateWindow(1280, 720, "Custom mesh demo", 0)

env = LoadTexture("sgd://envmaps/sunnysky-cube.png", 4, 56)

SetEnvTexture env

skybox = CreateSkybox(env)
SetSkyboxRoughness skybox, .3

light = CreateDirectionalLight()
;TurnEntity light,-45,0,0	; Tilt light down 45 degrees 

material = LoadPBRMaterial("sgd://misc/test-texture.png")
SetMaterialFloat material, "roughnessFactor1f", .5

mesh = CreateSphereMesh(1,96,48,material)
model=CreateModel(mesh)
MoveEntity model,0,0,3

While Not PollEvents()

	If IsKeyDown(263)
		TurnEntity model,0,3,0
	Else If IsKeyDown(262)
		TurnEntity model,0,-3,0
	EndIf
	
	If IsKeyDown(264)
		TurnEntity model,3,0,0
	Else If IsKeyDown(265)
		TurnEntity model,-3,0,0
	EndIf
	
	RenderScene()
	
	Present()
	
Wend