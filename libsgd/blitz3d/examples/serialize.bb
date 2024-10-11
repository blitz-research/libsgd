Dialect "modern"

Include "start.bb"

CreateWindow GetDesktopWidth()/2,GetDesktopHeight()/2,"Some nice trees",WINDOW_FLAGS_CENTERED

Local light = CreateDirectionalLight()
RotateEntity light,-45,0,0

Local env =  LoadCubeTexture("sgd://envmaps/sunnysky-cube.png",TEXTURE_FORMAT_ANY,TEXTURE_FLAGS_DEFAULT)
SetEnvTexture env

Local skybox = CreateSkybox(env)

Local sz#=200
;Local groundMaterial = LoadPBRMaterial("sgd://misc/brownish-grass.jpg")
;Local groundMesh = CreateBoxMesh(-sz,-1,-sz,sz,0,sz,groundMaterial)
;TransformTexCoords groundMesh,sz,sz,0,0
;Local ground = CreateModel(groundMesh)

Dim treeMeshes(3)
treeMeshes(0) = LoadMesh("sgd://models/tree1.glb")
treeMeshes(1) = LoadMesh("sgd://models/palm_tree1.glb")
treeMeshes(2) = LoadMesh("sgd://models/birch_tree1.glb")

Local n=10000
For i=1 To n
	Local treeModel = CreateModel(treeMeshes(Rand(0,2)))
	MoveEntity treeModel,Rnd(-sz,sz),0,Rnd(-sz,sz)
Next

;Save scene to desktop!
SaveScene "~/Desktop/test-scene.json"

;Destroy scene
ResetScene True

;Load scene from desktop!
LoadScene "~/Desktop/test-scene.json"

createPlayer(0)
MoveEntity player,0,10,0

While PollEvents()<>1
	PlayerFly(.125)
	RenderScene()
	Clear2D()
	Draw2DText "FPS:"+GetFPS(),0,0
	Present()
Wend