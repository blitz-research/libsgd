Dialect "modern"

Include "start.bb"

CreateWindow GetDesktopWidth()/2,GetDesktopHeight()/2,"Some nice trees",WINDOW_FLAGS_CENTERED

Local light = CreateDirectionalLight()
RotateEntity light,-45,0,0

Local env =  LoadCubeTexture("sgd://envmaps/sunnysky-cube.png",TEXTURE_FORMAT_ANY,TEXTURE_FLAGS_DEFAULT)
SetEnvTexture env

Local skybox = CreateSkybox(env)

Local groundMaterial = LoadPBRMaterial("sgd://misc/brownish-grass.jpg")
Local groundPlane = CreatePlane(groundMaterial)

Local sz#=200

Dim treeMeshes(3)
treeMeshes(0) = LoadMesh("sgd://models/tree1.glb")
treeMeshes(1) = LoadMesh("sgd://models/palm_tree1.glb")
treeMeshes(2) = LoadMesh("sgd://models/birch_tree1.glb")

Local n=10000
For i=1 To n
	Local treeModel = CreateModel(treeMeshes(Rand(0,2)))
	MoveEntity treeModel,Rnd(-sz,sz),0,Rnd(-sz,sz)
Next

createPlayer(0)
MoveEntity player,0,10,0
SetEntityName player,"Player"
SetEntityName camera,"Camera"

;Save scene to desktop!
SaveScene "~/Desktop/test-scene.json"

;Destroy scene
ResetScene True

;Load scene from desktop!
LoadScene "~/Desktop/test-scene.json"

;createPlayer(0)
;MoveEntity player,0,10,0
player=FindEntityChild(0,"Player")
camera=FindEntityChild(0,"Camera")

While PollEvents()<>1
	PlayerFly(.05)
	RenderScene()
	Clear2D()
	Draw2DText "FPS:"+GetFPS(),0,0
	Present()
Wend