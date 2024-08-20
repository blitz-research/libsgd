from libsgd import sgd

player = None
player_rvx = 0.0
player_rvy = 0.0
player_vx = 0.0
player_vy = 0.0
player_vz = 0.0

camera = None
camera_rx = 0.0
camera_ry = 0.0

def createPlayer(mesh):

	global player
	global camera
	
	player = sgd.CreateModel(mesh)
	camera = sgd.CreatePerspectiveCamera()
	sgd.SetEntityParent(camera, player)
	
	
def flyPlayer(speed):

	global player
	global player_rvx
	global player_rvy
	global player_vx
	global player_vy
	global player_vz
	global camera

	if sgd.IsKeyDown(sgd.KEY_LEFT):
		player_rvy = player_rvy + (1.5 - player_rvy) * .3
	elif sgd.IsKeyDown(sgd.KEY_RIGHT):
		player_rvy = player_rvy + (-1.5 - player_rvy) * .3
	else:
		player_rvy = player_rvy * .9
		
	sgd.RotateEntity(player, 0, player_rvy, 0)
	sgd.SetEntityRotation(camera, 0, 0, player_rvy * -15)
	
	if sgd.IsKeyDown(sgd.KEY_UP):
		player_rvx = player_rvx + (-1.5 - player_rvx) * .3
	elif sgd.IsKeyDown(sgd.KEY_DOWN):
		player_rvx = player_rvx + (1.5 - player_rvx) * .3
	else:
		player_rvx = player_rvx * .9
		
	sgd.TurnEntity(player,player_rvx, 0, 0)

	if sgd.IsKeyDown(sgd.KEY_W):
		player_vz = player_vz + (speed - player_vz) * .3
	elif sgd.IsKeyDown(sgd.KEY_S):
		player_vz = player_vz + (-speed - player_vz) * .3
	else:
		player_vz = player_vz * .9
		
	sgd.MoveEntity(player, 0, 0, player_vz)

	if sgd.IsKeyDown(sgd.KEY_A):
		player_vx = player_vx + (-speed - player_vx) * .3
	elif sgd.IsKeyDown(sgd.KEY_D):
		player_vx = player_vx + (speed - player_vx) * .3
	else:
		player_vx = player_vx * .9
		
	sgd.MoveEntity(player, player_vx, 0, 0)
