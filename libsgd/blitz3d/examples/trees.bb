
Include "start.bb"

CreateWindow(GetDesktopWidth()/2, GetDesktopHeight()/2, "La Tour Eiffel!", 4)

Local light = CreateDirectionalLight()
RotateEntity light,-45,0,0

Local env =  LoadTexture("sgd://envmaps/sunnysky-cube.png", 4, 56)
SetEnvTexture env

Local skybox = CreateSkybox(env)

Local sz#=1000
Local groundMaterial = LoadPBRMaterial("sgd://misc/brownish-grass.jpg")
Local groundMesh = CreateBoxMesh(-sz/2,-1,-sz/2,sz/2,0,sz/2,groundMaterial)
TFormMeshTexCoords groundMesh,sz,sz,0,0
Local ground = CreateModel(groundMesh)

sz=3
Local towerMesh = LoadMesh("sgd://models/eiffel_tower.glb")
FitMesh(towerMesh,-sz/2,0,-sz/2,sz/2,sz,sz/2,True)

Dim treeMeshes(3)

treeMeshes(0) = LoadMesh("sgd://models/tree1.glb")
treeMeshes(1) = LoadMesh("sgd://models/palm_tree1.glb")
treeMeshes(2) = LoadMesh("sgd://models/birch_tree1.glb")

For i=1 To 1000
	Local treeModel = CreateModel(treeMeshes(Rand(0,2)))
	Local fx = Rnd(-sz,sz)
	Local fz = Rnd(-sz,sz)
	MoveEntity treeModel,fx,0,fz
Next

createPlayer(0)
MoveEntity player,0,1,0

While PollEvents()<>1
	PlayerFly(.125)
	RenderScene()
	Clear2D()
	Draw2DText "FPS:"+GetFPS(),0,0
	Present()
Wend