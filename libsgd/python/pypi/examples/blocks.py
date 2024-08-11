from libsgd import sgd

import random
import start

NUM_BLOCKS = 25000
WORLD_SIZE = 200

class Block:
	model = None
	xr = random.uniform(-1, 1)
	yr = random.uniform(-1, 1)

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

	sgd.ClearScene()
	
	env =  sgd.LoadTexture("sgd://envmaps/stormy-cube.jpg", 4, 56)
	sgd.SetEnvTexture(env)
	
	sgd.CreateSkybox(env)
	
#	sgd.SetBloomEffectRadius(sgd.CreateBloomEffect(), 5)
	
	light = sgd.CreateDirectionalLight()
	sgd.SetLightColor(light, 1.0, 1.0, 1.0, .2)
	sgd.TurnEntity(light, -60.0, 0.0, 0.0)
	
	start.createPlayer(0)
	sgd.MoveEntity(start.player, 0.0, 50.0, -100.0)
	
	light = sgd.CreateSpotLight()
	sgd.SetEntityParent(light,start.player)
	sgd.SetLightColor(light, 1.0, 1.0, 1.0, 1.0)
	sgd.SetLightRange(light, 50.0)
	
	bulletImage = sgd.LoadImage("sgd://misc/light.png", 1)
	
	slimeBall = sgd.LoadSound("sgd://audio/slimeball.wav")

	# Create ground model
	
	material = sgd.LoadPBRMaterial("sgd://materials/Gravel023_1K-JPG")

	mesh = sgd.CreateBoxMesh(-WORLD_SIZE * 2, -1, -WORLD_SIZE * 2, WORLD_SIZE * 2, 0, WORLD_SIZE * 2, material)
	sgd.TFormMeshTexCoords(mesh,100,100,0,0)

	sgd.CreateModel(mesh)
	
	# Create Blocks
	
	material = sgd.LoadPBRMaterial("sgd://materials/Fabric048_1K-JPG")
	mesh = sgd.CreateBoxMesh(-1, -1, -1, 1, 1, 1, material)
	sgd.SetMeshShadowsEnabled(mesh,True)
	
	blocks = []
	
	for i in range(NUM_BLOCKS):
		block = Block()
		block.model = sgd.CreateModel(mesh)
		#block.xr = random.uniform(-3,3)
		#block.yr = random.uniform(-3,3)
		sgd.SetModelColor(block.model, random.random(), random.random(), random.random(), 1)
		sgd.SetEntityPosition(block.model, random.uniform(-WORLD_SIZE, WORLD_SIZE), random.uniform(1, WORLD_SIZE), random.uniform(-WORLD_SIZE, WORLD_SIZE))
		sgd.TurnEntity(block.model, random.uniform(0, 360), random.uniform(0, 360), 0)
		blocks.append(block)
		
	bullets = []
		
		
def updateScene():

	global bulletImage
	global slimeBall
	global blocks
	global bullets

	# Add bullet?
	if sgd.IsKeyHit(sgd.KEY_SPACE):
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
		bullet.entity = sgd.CreateSprite(bulletImage)
		sgd.SetSpriteColor(bullet.entity, r, g, b, 1)
		sgd.SetEntityParent(bullet.entity, start.player)
		sgd.SetEntityParent(bullet.entity, 0)
		bullet.timeout = 180
		bullet.vz = max(start.player_vz + .5, .5)
		bullets.append(bullet)
		
		light = sgd.CreatePointLight()
		sgd.SetLightShadowsEnabled(light, True)
		sgd.SetEntityParent(light, bullet.entity)
		sgd.SetLightColor(light, r, g, b, 1)
		sgd.SetLightRange(light, 50)
		
		sgd.PlaySound(slimeBall)

	# Update blocks
	for block in blocks:
		sgd.TurnEntity(block.model, block.xr, block.yr, 0)
	
	# Update bullets
	alive = []
	for bullet in bullets:
		bullet.timeout = bullet.timeout - 1
		if bullet.timeout:
			sgd.MoveEntity(bullet.entity, 0, 0, bullet.vz)
			alive.append(bullet)
		else:
			sgd.DestroyEntity(bullet.entity)

	bullets = alive
	
		
sgd.Init()

sgd.CreateWindow( sgd.GetDesktopWidth() // 2, sgd.GetDesktopHeight() // 2, "Spinning Blocks For Some Reason!", sgd.WINDOW_FLAGS_RESIZABLE | sgd.WINDOW_FLAGS_CENTERED)

resetScene()

while not (sgd.PollEvents() & sgd.EVENT_MASK_CLOSE_CLICKED):

	start.flyPlayer(.25)

	updateScene()

	sgd.RenderScene()
	
	sgd.Present()
