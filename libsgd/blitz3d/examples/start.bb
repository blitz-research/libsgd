
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

Type Actor
	Field entity
	Field xr#,yr#,vz#,vx#
End Type

Function CreateActor.Actor(entity) 

	Local actor.Actor=New Actor
	
	actor\entity = entity
	
	Return actor
	
End Function

Function FlyActor(actor.Actor)

	If KeyDown(KEY_LEFT)
		actor\yr = actor\yr + (1.5-actor\yr) * .1
	Else If KeyDown(KEY_RIGHT)
		actor\yr = actor\yr + (-1.5-actor\yr) * .1
	Else
		actor\yr = actor\yr * .9
	EndIf
	RotateEntity actor\entity,0,actor\yr,0
	
	If KeyDown(KEY_UP)
		actor\xr = actor\xr + (-1.5-actor\xr) * .1
	ElseIf KeyDown(KEY_DOWN)
		actor\xr = actor\xr + (1.5-actor\xr) * .1
	Else 
		actor\xr = actor\xr * .9
	EndIf
	TurnEntity actor\entity,actor\xr,0,0

	If KeyDown(KEY_W)
		actor\vz = actor\vz + (.5-actor\vz) * .1
	Else If KeyDown(KEY_S)
		actor\vz = actor\vz + (-.5 - actor\vz) * .1
	Else
		actor\vz = actor\vz * .9
	EndIf
	MoveEntity actor\entity,0,0,actor\vz

	If KeyDown(KEY_A)
		actor\vx = actor\vx + (-.5-actor\vx) * .1
	Else If KeyDown(KEY_D)
		actor\vx = actor\vx + (.5-actor\vx) * .1
	Else
		actor\vx = actor\vx * .9
	EndIf
	MoveEntity actor\entity,actor\vx,0,0

End Function


