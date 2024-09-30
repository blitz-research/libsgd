from libsgd import sgd

import random
import start

NUM_BLOCKS = 25000
WORLD_SIZE = 200

class Block:
	model = None
	xr = random.uniform(-3, 3)
	yr = random.uniform(-3, 3)

class Bullet:
	entity = None
	timeout = 180
	vz = 1

bulletImage = None
slimeBall = None

blocks = []
bullets = []
	
def resetScene():

	global bulletImage
	global slimeBall
	global blocks
	global bullets

	sgd.clearScene()
	
	env = sgd.loadCubeTexture("sgd://envmaps/stormy-cube.jpg", sgd.TEXTURE_FORMAT_ANY,sgd.TEXTURE_FLAGS_DEFAULT)
	sgd.setEnvTexture(env)
	
	sgd.createSkybox(env)
	
	light = sgd.createDirectionalLight()
	sgd.setLightColor(light, 1.0, 1.0, 1.0, .2)
	sgd.turnEntity(light, -60.0, 0.0, 0.0)
	
	start.createPlayer(0)
	sgd.moveEntity(start.player, 0.0, 50.0, -100.0)
	
	light = sgd.createSpotLight()
	sgd.setEntityParent(light,start.player)
	sgd.setLightColor(light, 1.0, 1.0, 1.0, 1.0)
	sgd.setLightRange(light, 50.0)
	
	bulletImage = sgd.loadImage("sgd://misc/light.png")
	
	slimeBall = sgd.loadSound("sgd://audio/slimeball.wav")

	# Create ground model
	
	material = sgd.loadPBRMaterial("sgd://materials/Gravel023_1K-JPG")

	mesh = sgd.createBoxMesh(-WORLD_SIZE * 2, -1, -WORLD_SIZE * 2, WORLD_SIZE * 2, 0, WORLD_SIZE * 2, material)
	sgd.transformTexCoords(mesh,100,100,0,0)

	sgd.createModel(mesh)
	
	# Create Blocks
	
	material = sgd.loadPBRMaterial("sgd://materials/Fabric048_1K-JPG")
	mesh = sgd.createBoxMesh(-1, -1, -1, 1, 1, 1, material)
	sgd.setMeshShadowsEnabled(mesh,True)
	
	blocks = []
	
	for i in range(NUM_BLOCKS):
		block = Block()
		block.model = sgd.createModel(mesh)
		#block.xr = random.uniform(-3,3)
		#block.yr = random.uniform(-3,3)
		sgd.setModelColor(block.model, random.random(), random.random(), random.random(), 1)
		sgd.setEntityPosition(block.model, random.uniform(-WORLD_SIZE, WORLD_SIZE), random.uniform(1, WORLD_SIZE), random.uniform(-WORLD_SIZE, WORLD_SIZE))
		sgd.turnEntity(block.model, random.uniform(0, 360), random.uniform(0, 360), 0)
		blocks.append(block)
		
	bullets = []
		
		
def updateScene():

	global bulletImage
	global slimeBall
	global blocks
	global bullets

	# Add bullet?
	if sgd.isKeyHit(sgd.KEY_SPACE):
		r = random.random()
		g = random.random()
		b = random.random()
		
		i = random.randint(1,3)
		if i == 1:
			r = 1
		elif i == 2:
			g = 1
		elif i == 3:
			b = 1
		
		bullet = Bullet()
		bullet.entity = sgd.createSprite(bulletImage)
		sgd.setSpriteColor(bullet.entity, r, g, b, 1)
		sgd.setEntityParent(bullet.entity, start.player)
		sgd.setEntityParent(bullet.entity, 0)
		bullet.timeout = 180
		bullet.vz = max(start.player_vz + .5, .5)
		bullets.append(bullet)
		
		light = sgd.createPointLight()
		sgd.setLightShadowsEnabled(light, True)
		sgd.setEntityParent(light, bullet.entity)
		sgd.setLightColor(light, r, g, b, 1)
		sgd.setLightRange(light, 50)
		
		sgd.playSound(slimeBall)

	# Update blocks
	for block in blocks:
		sgd.turnEntity(block.model, block.xr, block.yr, 0)
	
	# Update bullets
	alive = []
	for bullet in bullets:
		bullet.timeout = bullet.timeout - 1
		if bullet.timeout:
			sgd.moveEntity(bullet.entity, 0, 0, bullet.vz)
			alive.append(bullet)
		else:
			sgd.destroyEntity(bullet.entity)

	bullets = alive
	
		
sgd.init()

sgd.createWindow( sgd.getDesktopWidth() // 2, sgd.getDesktopHeight() // 2, "Spinning Blocks For Some Reason!", sgd.WINDOW_FLAGS_CENTERED)

resetScene()

while not (sgd.pollEvents() & sgd.EVENT_MASK_CLOSE_CLICKED):

	start.flyPlayer(.25)

	updateScene()

	sgd.renderScene()
	
	sgd.present()
