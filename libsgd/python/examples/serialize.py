from libsgd import sgd

import random
import start
import math

sgd.init()

sgd.createWindow(sgd.getDesktopWidth() // 2, sgd.getDesktopHeight() // 2, "Serialize!",
				 sgd.WINDOW_FLAGS_CENTERED)

light = sgd.createDirectionalLight()
sgd.rotateEntity(light, -45, 0, 0)

env = sgd.loadCubeTexture("sgd://envmaps/sunnysky-cube.png", sgd.TEXTURE_FORMAT_ANY, sgd.TEXTURE_FLAGS_DEFAULT)
sgd.setEnvTexture(env)

skybox = sgd.createSkybox(env)

groundMaterial = sgd.loadPBRMaterial("sgd://misc/brownish-grass.jpg")
groundPlane = sgd.createPlane(groundMaterial)

treeMeshes = [None, None, None]
treeMeshes[0] = sgd.loadMesh("sgd://models/tree1.glb")
treeMeshes[1] = sgd.loadMesh("sgd://models/palm_tree1.glb")
treeMeshes[2] = sgd.loadMesh("sgd://models/birch_tree1.glb")

WORLD_SIZE = 100
NUM_TREES = 5000

for i in range(NUM_TREES):
	treeModel = sgd.createModel(treeMeshes[random.randint(0, 2)])
	sgd.moveEntity(treeModel, random.uniform(-WORLD_SIZE, WORLD_SIZE), 0, random.uniform(-WORLD_SIZE, WORLD_SIZE))
	sgd.turnEntity(treeModel, 0, random.uniform(-math.pi, math.pi), 0)

start.createPlayer(0)
sgd.moveEntity(start.player, 0.0, 10.0, 0)

sgd.setEntityName(start.player, "Player")
sgd.setEntityName(start.camera, "Camera")

sgd.saveScene("~/Desktop/test-scene.json")
sgd.resetScene(True)
sgd.loadScene("~/Desktop/test-scene.json")

start.player = sgd.findEntityChild(0,"Player")
start.camera = sgd.findEntityChild(0,"Camera")

while not (sgd.pollEvents() & sgd.EVENT_MASK_CLOSE_CLICKED):
	start.flyPlayer(.25)

	sgd.renderScene()

	sgd.present()
