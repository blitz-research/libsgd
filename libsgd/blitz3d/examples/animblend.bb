; sequence# | frames |	description 
; ---------	 ------   -----------
; 0				 72 	 	death
; 1 			 60 		idle	
; 2     		 27 	   jump
; 3 			 17			run_fast
; 4				 21 		run_left
; 5 			 21			run_right
; 6 			 23 		run_slow
; 7 			 36 		standard_walk
; 8 			 32 		walk
; 9 			 32 		walk_left
; 10 		    29			walk_right				 	  			

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

Local seq0,time0#,time0Step#	;base animation
Local seq1,time1#,time1Step#	;blend target
Local blend#, blendStep#
Local useBlend=1

seq0 = 1	;initially idle
time0Step = .02

CreateWindow 800,600,"Animation Blender",256
CreateScene()
camera = CreatePerspectiveCamera()
MoveEntity camera,0,1,-3
light = CreateDirectionalLight()
TurnEntity light,-35,0,0
model = LoadBonedModel ("sgd://models/base_male_animated.glb",True)

While (PollEvents() And 1)=0
	
	If GetKeyHit(KEY_SPACE) useBlend = 1 - useBlend
	
	;Select new anim
	Local nseq=1
	If GetKeyDown(KEY_LEFT_SHIFT)
		;Run
		If GetKeyDown(KEY_UP)
			nseq=6
		ElseIf GetKeyDown(KEY_LEFT)
			nseq=5
		ElseIf GetKeyDown(KEY_RIGHT)
			nseq=4
		EndIf
	Else
		;Walk
		If GetKeyDown(KEY_UP)
			nseq=8
		ElseIf GetKeyDown(KEY_LEFT)
			nseq=10
		ElseIf GetKeyDown(KEY_RIGHT)
			nseq=9
		EndIf
	EndIf
	
	;Change to new anim
	If blendStep = 0 And seq0 <> nseq
		If useBlend
			time0Step = 0
			seq1 = nseq
			time1 = 0
			time1Step = 0.02
			blendStep = .1
		Else
			seq0 = nseq
			time0 = 0
			time0Step = 0.02
		EndIf
	EndIf
	
	;Update blend state
	blend = blend + blendStep
	If blend >= 1
		seq0 = seq1
		time0  = time1
		time0Step = time1Step
		time1 = 0
		time1Step = 0
		blend = 0
		blendStep = 0
	EndIf
	
	;Set base animation
	time0 = time0 + time0Step
	AnimateModel model,seq0,time0,2,1
	
	If blend>0 
		;Appy blend
		time1 = time1 + time1Step
		AnimateModel model,seq1,time1,2,blend
	EndIf
	
	Clear2D()
	Draw2DText "Arrow keys/Shift to run. Using blend:"+useBlend+" (space to toggle)",0,0
	
	RenderScene()
	
	Present()
Wend 