
Const KEY_ESCAPE = 256
Const KEY_SPACE = 32
					
Const KEY_RIGHT = 262
Const KEY_LEFT = 263
Const KEY_DOWN = 264
Const KEY_UP = 265

Const KEY_W = 87
Const KEY_A = 65
Const KEY_S = 83
Const KEY_D = 68

Const KEY_LEFT_SHIFT = 340
Const KEY_LEFT_CONTROL = 341
Const KEY_LEFT_ALT = 342
Const KEY_LEFT_SUPER = 343
Const KEY_RIGHT_SHIFT = 344
Const KEY_RIGHT_CONTROL = 345
Const KEY_RIGHT_ALT = 346
Const KEY_RIGHT_SUPER = 347

Global player
Global player_rvx#,player_rvy#
Global player_vx#,player_vy#,player_vz#

Global camera
Global camera_rx#,camera_ry#

Function CreatePlayer(mesh)

	player = CreateModel(mesh)
	
	camera = CreatePerspectiveCamera()
	SetEntityParent camera,player
	
End Function

Function PlayerWalk(speed#)

	If IsKeyDown(KEY_LEFT)
		player_rvy = player_rvy + (1.5-player_rvy) * .3
	Else If IsKeyDown(KEY_RIGHT)
		player_rvy = player_rvy + (-1.5-player_rvy) * .3
	Else
		player_rvy = player_rvy * .9
	EndIf
	RotateEntity player,0,player_rvy,0

	If IsKeyDown(KEY_DOWN)
		camera_rx = camera_rx + (-45-camera_rx) * .025
	Else If IsKeyDown(KEY_UP)
		camera_rx = camera_rx + (45-camera_rx) * .025
	Else 
		camera_rx = camera_rx * .9
	EndIf
	SetEntityRotation camera,camera_rx,0,0
	
	If IsKeyDown(KEY_W)
		player_vz = player_vz + (speed-player_vz) * .3
	Else If IsKeyDown(KEY_S)
		player_vz = player_vz + (-speed-player_vz) * .3
	Else
		player_vz = player_vz * .9
	EndIf
	
	If IsKeyDown(KEY_A)
		player_vx = player_vx + (-speed-player_vx) * .3
	Else If IsKeyDown(KEY_D)
		player_vx = player_vx + (speed-player_vx) * .3
	Else
		player_vx = player_vx * .9
	EndIf
	
	MoveEntity player,player_vx,0,player_vz
	
End Function

Function PlayerFly(speed#)

	If IsKeyDown(KEY_LEFT)
		player_rvy = player_rvy + (1.5-player_rvy) * .3
	Else If IsKeyDown(KEY_RIGHT)
		player_rvy = player_rvy + (-1.5-player_rvy) * .3
	Else
		player_rvy = player_rvy * .9
	EndIf
	RotateEntity player,0,player_rvy,0
	SetEntityRotation camera,0,0,player_rvy*-15
	
	If IsKeyDown(KEY_UP)
		player_rvx = player_rvx + (-1.5-player_rvx) * .3
	ElseIf IsKeyDown(KEY_DOWN)
		player_rvx = player_rvx + (1.5-player_rvx) * .3
	Else 
		player_rvx = player_rvx * .9
	EndIf
	TurnEntity player,player_rvx,0,0

	If IsKeyDown(KEY_W)
		player_vz = player_vz + (speed-player_vz) * .3
	Else If IsKeyDown(KEY_S)
		player_vz = player_vz + (-speed-player_vz) * .3
	Else
		player_vz = player_vz * .9
	EndIf
	MoveEntity player,0,0,player_vz

	If IsKeyDown(KEY_A)
		player_vx = player_vx + (-speed-player_vx) * .3
	Else If IsKeyDown(KEY_D)
		player_vx = player_vx + (speed-player_vx) * .3
	Else
		player_vx = player_vx * .9
	EndIf
	MoveEntity player,player_vx,0,0

End Function