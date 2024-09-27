from libsgd import sgd

# animblend.bb to animblend.py

"""
sequence# | frames |	description 
---------	 ------   -----------
0		72 	 	death
1 		 60 		idle	
2     		 27 	        jump
3 		 17		run_fast
4		 21 		run_left
5 		 21		run_right
6 		 23 		run_slow
7 		 36 		standard_walk
8 		 32 		walk
9 		 32 		walk_left
10 		 29		walk_right		
"""

# app from here

sgd.init()

seq0 = 0
time0 = 0.0
time0Step = 0.0	
seq1 = 0
time1 = 0.0
time1Step = 0.0 
blend = 0.0
blendStep = 0.0
useBlend = 1

# window
sgd.createWindow(800, 600, "libsgd", 0)
#sgd.SetClearColor(.1,.2,.3,1)

# camera
camera = sgd.createPerspectiveCamera()
sgd.moveEntity(camera, 0,1,-3)

# light
light = sgd.createDirectionalLight()
sgd.turnEntity(light, -35, 0, 0)
model = sgd.loadBonedModel("sgd://models/base_male_animated.glb",True)

###### main loop #######
while (sgd.pollEvents() & sgd.EVENT_MASK_CLOSE_CLICKED) == 0:
    if sgd.isKeyHit(sgd.KEY_ESCAPE):
        break

    # blend select toggle
    if sgd.isKeyHit(sgd.KEY_SPACE):
        useBlend = 1 - useBlend
    
    # select new anim
    nseq = 1
    if(sgd.isKeyDown(sgd.KEY_LEFT_SHIFT)):
        # run
        if (sgd.isKeyDown(sgd.KEY_UP)):
            nseq = 6
        elif(sgd.isKeyDown(sgd.KEY_LEFT)):
            nseq = 5
        elif(sgd.isKeyDown(sgd.KEY_RIGHT)):
            nseq = 4
    else:    
        # walk
        if (sgd.isKeyDown(sgd.KEY_UP)):
            nseq = 8
        elif(sgd.isKeyDown(sgd.KEY_LEFT)):
            nseq = 10
        elif(sgd.isKeyDown(sgd.KEY_RIGHT)):
            nseq = 9
    
    # change to new anim
    if blendStep == 0 and seq0 is not nseq:
        if useBlend:
            time0Step = 0
            seq1 = nseq
            time1 = 0
            time1Step = 0.02
            blendStep = .1
        else:
            seq0 = nseq
            time0 = 0
            time0Step = 0.02

    # update blend state
    blend = blend + blendStep
    if blend >= 1:
        seq0 = seq1
        time0  = time1
        time0Step = time1Step
        time1 = 0
        time1Step = 0
        blend = 0
        blendStep = 0
	
    #Set base animation
    time0 = time0 + time0Step
    sgd.animateModel(model,seq0,time0,2,1)

    if blend > 0: 
		#Appy blend
        time1 = time1 + time1Step
        sgd.animateModel(model,seq1,time1,2,blend)
		
    sgd.clear2D()
    sgd.draw2DText("Arrow keys/Shift to run. Using blend:" + str(useBlend) + " (space to toggle)",0,0)
    sgd.renderScene()
    sgd.present()

sgd.terminate()

##### bye #####
