
' Movement -- keys or Xbox pad. Keys are actually nicer...

' KEYS:

' 	WASD				- forward/back/strafe
' 	CURSORS				- turn/look around
'	SPACE				- fire bullet!
'	LEFT-SHIFT			- speed boost

' 	RIGHT-CTRL/SHIFT	- rise up/down

' XBOX PAD (bit WIP!):

' 	Triggers	- forward/back
' 	Left stick	- turn/look around
'	Button A	- fire bullet

' 	Right stick	- best ignored... strafe and rise up/down (rise requires more pressure to activate)

' 	Bit weird, just use triggers/left stick most of the time!

' Set CAMERA_PITCH_INVERT to True to invert pitch control
' Set SOUND_ON to False to turn off shot sounds

Import "../sgd.bmx"

Const FULL_SCREEN:Int = False
'Const FULL_SCREEN:Int = True ' Keyboard control too twitchy. Vsync??

Const NUM_BLOCKS:Int = 10000
Const CAMERA_PITCH_INVERT:Int = False ' True to flip look-up/down on Xbox pad
Const SOUND_ON:Int = False
Const SHADOWS_ON:Int = True

' Boilerplate!

' Normal usage with sgd_dynamic.dll in same folder:

'SGD.LoadSGD		'Loads the BlitzMax DLL wrapper

' LibSGD demo distribution path:

SGD.LoadSGD "../../lib/sgd_dynamic.dll"

SGD.Init		' Standard LibSGD Init call

Local flags:Int = SGD.WINDOW_FLAGS_RESIZABLE

If FULL_SCREEN
	flags = SGD.WINDOW_FLAGS_FULLSCREEN
EndIf

SGD.CreateWindow (1280, 720, "Demo - WASD/Shift plus CURSORS plus SPACE. UTF-8 text bonus! 雪の東京・高円寺 散歩", flags)

' Should probably stick in a Type...

Global CubeMesh:Int = demo_GetStaticCubeMesh ()
Global BulletMesh:Int = demo_GetStaticBulletMesh ()

Global Shot1:TSound' = LoadSound ("shot1.ogg")
Global Shot2:TSound' = LoadSound ("shot2.ogg")

' Can't do this until scene exists!
' Global BulletSprite:Int = CreateBulletSprite ()

' Scene init...

' Textures -- loaded from https://skirmish-dev.net/assets via "sgd://" pathname...

' -----------------------------------------------------------------------------
' Ground materials -- uncomment one:
' -----------------------------------------------------------------------------

Local ground_textures:String = "Tiles019_1K-JPG"
'Local ground_textures:String = "Tiles093_1K-JPG"
'Local ground_textures:String = "Bricks076C_1K-JPG"
'Local ground_textures:String = "PavingStones119_1K-JPG"
'Local ground_textures:String = "PavingStones131_1K-JPG"

' -----------------------------------------------------------------------------
' Envmaps -- uncomment one:
' -----------------------------------------------------------------------------

'Local sky_texture:String = "nightsky-cube.png"
Local sky_texture:String = "stormy-cube.jpg"
'Local sky_texture:String = "sunnysky-cube.png"

' -----------------------------------------------------------------------------
' Selections from above...
' -----------------------------------------------------------------------------

Local ground_material:String	= "sgd://materials/"	+ ground_textures
Local sky_map:String			= "sgd://envmaps/"		+ sky_texture

'Local sky_map:String = "散歩.jpg"
'Local sky_map:String = "test.jpg"

demo_CreateScene NUM_BLOCKS, 50.0, ground_material, sky_map
' Can't do this until scene exists!

Global BulletSprite:Int = CreateBulletSprite ()

' Scene camera...

Global Camera:Int = demo_CreateCamera ()

SGD.MoveEntity Camera, 0, 0, -20

' Window events variables...

Local events:Int
Local quit:Int = False

' WIP: Bit of a weird setup -- XBCameraControl has its own versions! To be wrapped...

Local pitch:Double = 0.0
Local yaw:Double = 0.0
Local strafe:Double = 0.0
Local rise:Double = 0.0
Local z:Double = 0.0

Repeat

	' Get window events...
	
	events = SGD.PollEvents ()
	
	' Check for ESC key or window close gadget-hit...
	
	If SGD.KeyHit (SGD.KEY_ESCAPE) Or events & SGD.EVENT_MASK_CLOSE_CLICKED
		quit = True
	EndIf
	
	' Update cubes...
	
	For Local c:Cube = EachIn Cube.CubeList
		SGD.TurnEntity c.entity, 0.5, 0.5, 0.5
	Next
	
	' TEMP: Stick keys into separate function...
	
	If Not demo_XBCameraControl ()
	
		' No gamepad input, use keys (er, which turned out better):
		
		' LEFT/RIGHT cursors: camera strafe...
		
		If SGD.KeyDown (SGD.KEY_LEFT)
			yaw = yaw + 0.1
			If yaw > 1.5 Then yaw = 1.5
		Else
			If SGD.KeyDown (SGD.KEY_RIGHT)
				yaw = yaw - 0.1
				If yaw < -1.5 Then yaw = -1.5
			EndIf
		EndIf

		' Pitch inversion option...
		
		Local pitcher:Float = 1.0
		
		If CAMERA_PITCH_INVERT Then pitcher = -1.0
		
		' UP/DOWN cursors: camera tilt...
		
		If SGD.KeyDown (SGD.KEY_UP)
			pitch = pitch - 0.1 * pitcher
			If pitch < -1.5 Then pitch = -1.5
		Else
			If SGD.KeyDown (SGD.KEY_DOWN)
				pitch = pitch + 0.1 * pitcher
				If pitch > 1.5 Then pitch = 1.5
			EndIf
		EndIf

		' Camera boost with LEFT SHIFT...
		
		Local shifted:Int = False
		
		If SGD.KeyDown (SGD.KEY_LEFT_SHIFT)
			shifted = True
		EndIf
		
		' A/D keys: strafe left/right...

		If SGD.KeyDown (SGD.KEY_A)
			If Not shifted
				strafe = strafe - 0.01
				If strafe < -0.1 Then strafe = -0.1
			Else
				strafe = strafe - 0.5
				If strafe < -0.5 Then strafe = -0.5
			EndIf
		Else
			If SGD.KeyDown (SGD.KEY_D)
				If Not shifted
					strafe = strafe + 0.1
					If strafe > 0.1 Then strafe = 0.1
				Else
					strafe = strafe + 0.5
					If strafe > 0.5 Then strafe = 0.5
				EndIf
			EndIf
		EndIf

		' W/S keys: forward/back...

		If SGD.KeyDown (SGD.KEY_W)
			If Not shifted
				z = z + 0.01
				If z > 0.1 Then z = 0.1
			Else
				z = z + 0.05
				If z > 0.5 Then z = 0.5
			EndIf
		Else
			If SGD.KeyDown (SGD.KEY_S)
				If Not shifted
					z = z - 0.01
					If z < -0.1 Then z = -0.1
				Else
					z = z - 0.05
					If z < -0.5 Then z = -0.5
				EndIf
			EndIf
		EndIf

		' RIGHT SHIFT/CTRL keys: raise/lower camera...

		If SGD.KeyDown (SGD.KEY_RIGHT_SHIFT)
			rise = rise + 0.01
			If rise > 0.1 Then rise = 0.1
		Else
			If SGD.KeyDown (SGD.KEY_RIGHT_CONTROL)
				rise = rise - 0.01
				If rise < -0.1 Then rise = -0.1
			EndIf
		EndIf

		' SPACE: shoot!
		
		' New Bullet z creates a new Bullet, passing current z speed to add to Bullet.BulletSpeed...
		
		If SGD.KeyHit (SGD.KEY_SPACE) Then New Bullet z
		
		' Diminish all movement towards 0...
		
		pitch	= Util.Diminish (pitch, 0.03)
		yaw		= Util.Diminish (yaw, 0.03)
		strafe	= Util.Diminish (strafe, 0.0015)
		rise	= Util.Diminish (rise, 0.0015)
		z		= Util.Diminish (z, 0.0015)

		' Update camera...
		
		SGD.TurnEntity Camera, pitch, 0, 0
		SGD.RotateEntity Camera, 0, yaw, 0
		SGD.MoveEntity Camera, strafe, rise, z
		
	EndIf
	
	' Update bullets...
	
	Bullet.UpdateBullets
	
	' Render/show!
	
	SGD.RenderScene
	SGD.Present
	
Until quit

End

' Cube management...

Type Cube

	Global CubeList:TList
	
	Field entity:Int
	
	Method New ()
	
		If Not CubeList Then CubeList = CreateList ()
		ListAddLast CubeList, Self
		
	End Method
	
End Type

Type Bullet

	Global BulletList:TList
	Global BulletSpeed:Float = 0.25
	Global BulletDist:Float = 100.0
	
	Field entity:Int
	Field light:Int
	Field init_speed:Float
	
	Method New (speed:Double = 0.0)
	
		If Not BulletList
			BulletList = CreateList ()
		EndIf
		
'		If Not light
			light = SGD.CreatePointLight ()
			SGD.SetLightRange light, 25
			If SHADOWS_ON
				SGD.SetLightCastsShadow light, True
			EndIf
'		EndIf
		
		init_speed = Max (0, speed)
		
		ListAddLast BulletList, Self

		' Primary!
		
		Local r:Int = Rand (0, 1)
		Local g:Int = Rand (0, 1)
		Local b:Int = Rand (0, 1)

		' Convert black to white!
		
		If Not r And Not g And Not b Then r = 1; g = 1; b = 1
		
'		entity = demo_CreateBullet (0.1)
		entity = SGD.CopyEntity (BulletSprite)
		SGD.SetSpriteColor entity, r, g, b, 1
		
		SGD.SetEntityParent entity, Camera		' Pick up camera position/orientation
		SGD.SetEntityParent entity, Null		' Now use world as parent or we follow the camera!

'		SGD.SetModelColor entity, r, g, b, 1.0
		
		' Attach light to bullet...
		
		SGD.SetEntityParent light, entity
		SGD.SetLightColor light, r, g, b, 1.0
		
		If SOUND_ON
			Select Rand (0, 1)
				Case 0
					If Shot1 Then PlaySound Shot1
				Case 1
					If Shot2 Then PlaySound Shot2
			End Select
		EndIf
		
	End Method
	
	Function UpdateBullets ()
	
		If BulletList
			For Local b:Bullet = EachIn BulletList
				SGD.MoveEntity b.entity, 0, 0, b.init_speed + BulletSpeed
				If temp_EntityDistance (Camera, b.entity) > BulletDist
					SGD.DestroyEntity b.entity
					ListRemove BulletList, b
				EndIf
			Next
		EndIf
		
	End Function
	
End Type

' Camera! Xbox One pad... returns True for movement over movement_trigger on any axis

Function demo_XBCameraControl:Int (movement_trigger:Float = 0.1)

	Local movement:Int = False
	
	For Local gamepad:Int = 0 To 3 ' No count facility yet

		If SGD.GamepadConnected (gamepad)

			' Left stick -- camera look

			Local pitcher:Float = 1.0
			If CAMERA_PITCH_INVERT Then pitcher = -1.0

			Local pitch:Float	= SGD.GamepadAxis (gamepad, 1) * pitcher
			Local yaw:Float		= -SGD.GamepadAxis (gamepad, 0)

			' Triggers - forward/reverse

			Local left_trig:Float	= SGD.GamepadAxis (gamepad, 4)
			Local right_trig:Float	= SGD.GamepadAxis (gamepad, 5)

			' Right stick - left/right: strafe

			Local strafe:Float = SGD.GamepadAxis (gamepad, 2)

			' Right stick - up/down: raise/lower

			Local rise:Float = SGD.GamepadAxis (gamepad, 3)

			If Abs (pitch)	< movement_trigger Then pitch = 0 Else movement = True
			If Abs (yaw)	< movement_trigger Then yaw = 0 Else movement = True
			If Abs (strafe)	< movement_trigger Then strafe = 0 Else movement = True

			' Special case... want raise/lower only when pushed hard
			
			If Abs (rise) >= movement_trigger
				movement = True
			EndIf

			If Abs (rise) < 0.85
				rise = 0
			EndIf
			
			' These are -1.0 at rest, going from -1.0 to 1.0 as pressure increases:

			If left_trig < -(1.0 - movement_trigger) Then left_trig = -1.0 Else movement = True
			If right_trig < -(1.0 - movement_trigger) Then right_trig = -1.0 Else movement = True

			Local z:Float = (-left_trig + right_trig) * 0.05
			
			SGD.TurnEntity Camera, pitch, 0, 0
			SGD.RotateEntity Camera, 0, yaw, 0
			SGD.MoveEntity Camera, strafe * 0.1, -rise * 0.025, z
			
			' TMP: Meant to be GamepadButtonHIT
			
			If SGD.GamepadButtonHit (gamepad, 0)
				New Bullet z ' New Bullet z creates a new Bullet, passing current z speed to add to Bullet.BulletSpeed...
			EndIf
			
		EndIf
	
	Next

	Return movement
	
End Function

' Utils!

Function demo_CreateCube:Int (size:Float)

	Local box:Int = SGD.CreateModel ()
	
	SGD.SetModelMesh box, demo_GetStaticCubeMesh ()
	
	Return box
	
End Function

Function demo_CreateBullet:Int (size:Float)

	Local box:Int = SGD.CreateModel ()
	
	SGD.SetModelMesh box, demo_GetStaticBulletMesh ()
	
	Return box
	
End Function

Function demo_CreateCamera:Int ()

	Local camera:Int = SGD.CreatePerspectiveCamera ()

	SGD.SetCameraNear camera, 0.05

	' Light attached to camera -- improves lighting of cubes when looking into sun direction (ie. when in shadow of cubes)...

	Local camera_light:Int = SGD.CreateSpotLight ()
	SGD.SetEntityParent camera_light, camera
	If SHADOWS_ON
		SGD.SetLightCastsShadow camera_light, True ' Working? Can't see shadows from it, even offset:
	EndIf
'	SGD.MoveEntity camera_light, -2, 2, 2
'	SGD.TurnEntity camera_light, 0, -10, 0

	Return camera
	
End Function

Function demo_CreateSkyBox:Int (texture:String, format:Int = SGD.TEXTURE_FORMAT_SRGBA8, flags:Int = SGD.TEXTURE_FLAGS_CUBE_MIPMAP | SGD.TEXTURE_FLAGS_FILTER)

	Local sky_texture:Int = SGD.LoadTexture (texture, format, flags)


		SGD.SetSceneEnvTexture sky_texture

	Local skybox:Int = SGD.CreateSkybox ()

		SGD.SetSkyboxTexture skybox, sky_texture
		SGD.SetSkyboxRoughness skybox, 0.3

	Return skybox
	
End Function

Function demo_CreateScene (num_cubes:Int = 10000, ground_scale:Float = 1.0, ground_material_path:String = "sgd://materials/PavingStones131_1K-JPG", skybox_path:String = "sgd://envmaps/stormy-cube.jpg")

	' Basic scene init...

	SGD.CreateScene ()
	SGD.SetSceneClearColor 0.1, 0.25, 0.5, 1.0
	SGD.SetSceneAmbientLightColor 0.025, 0.05, 0.0375, 1.0

	' Skybox...
	
	Local skybox:Int = demo_CreateSkyBox (skybox_path)
	
	' Sunlight...
	
	Local sun:Int = SGD.CreateDirectionalLight ()
	SGD.SetLightColor sun, 0.1, 0.1, 0.1, 1.0
	SGD.TurnEntity sun, 0, 35, 0

	For Local loop:Int = 1 To num_cubes

		Local c:Cube = New Cube
		
		c.entity = demo_CreateCube (Util.RndF (0.25, 3.0))
		
		Local size:Float = Util.RndF (0.25, 4.0)
		
		SGD.MoveEntity c.entity, Util.RndF (-20, 20), Util.RndF (-20, 20), Util.RndF (40)
		SGD.SetEntityRotation c.entity, Util.RndF (360), Util.RndF (360), Util.RndF (360)
		
		SGD.SetModelColor c.entity, Util.RndF (1.0), Util.RndF (1.0), Util.RndF (1.0), 1.0
		
	Next

	Local ground_size:Float = 250.0

	Local ground_material:Int	= SGD.LoadPBRMaterial (ground_material_path)
	Local ground:Int			= SGD.CreateModel ()
	Local ground_mesh:Int		= SGD.CreateBoxMesh (-ground_size * 0.5, -0.5, -ground_size * 0.5, ground_size * 0.5, 0.5, ground_size * 0.5, ground_material)

	SGD.SetModelMesh ground, ground_mesh

	' Scale texture...
	
	SGD.TransformMeshTexCoords (ground_mesh, ground_scale, ground_scale, 0.0, 0.0)

	SGD.MoveEntity ground, 0, -21, 0

End Function

Function demo_GetStaticCubeMesh:Int (cube_material_path:String = "sgd://materials/Tiles019_1K-JPG")

	If Not CubeMesh

		' Material applied to cubes...

'Print cube_material_path
'Print String.FromUTF8String (cube_material_path.ToUTF8String ())

		Local cube_material:Int	= SGD.LoadPBRMaterial (cube_material_path)

		CubeMesh = SGD.CreateBoxMesh (-0.5, -0.5, -0.5, 0.5, 0.5, 0.5, cube_material)

		If SHADOWS_ON
			SGD.SetMeshCastsShadow CubeMesh, True
		EndIf

	EndIf
	
	Return CubeMesh
	
End Function

Function CreateBulletSprite (sprite_path:String = "sgd://misc/light.png")

	Local sprite:Int = SGD.CreateSprite ()
	Local mat:Int = SGD.LoadPrelitMaterial (sprite_path)
'	If Not mat RuntimeError "Hi"
	
	SGD.SetMaterialBlendMode mat, SGD.BLEND_MODE_ALPHA
	SGD.SetSpriteMaterial sprite, mat
	
	' No HideEntity yet!
	SGD.MoveEntity sprite, 0, -50, 0
	
	Return sprite

End Function

Function demo_GetStaticBulletMesh:Int ()'cube_material_path:String = "sgd://materials/Tiles019_1K-JPG")

	If Not BulletMesh

		' Material applied to cubes...

'Print cube_material_path
'Print String.FromUTF8String (cube_material_path.ToUTF8String ())

		Local cube_material:Int	= SGD.CreatePBRMaterial ()'LoadPBRMaterial (cube_material_path)

		BulletMesh = SGD.CreateBoxMesh (-0.5, -0.5, -0.5, 0.5, 0.5, 0.5, cube_material)
		If SHADOWS_ON
			SGD.SetMeshCastsShadow BulletMesh, False
		EndIf
		
	EndIf
	
	Return BulletMesh
	
End Function

' Utility functions...

Type Util

	' Just to tidy up Rnd Double warnings!

	Function RndF:Float (minValue:Float = 1, maxValue:Float = 0)
		Return Float (Rnd (minValue, maxValue))
	End Function

	' Reduce value towards 0 by amount (eg. 0.01)...
	
	Function Diminish:Double (value:Double, amount:Double)
		If Abs (value) <= Abs (amount) Then Return 0.0
		value = value - (amount * Sgn (value))
		Return value
	End Function

End Type

' No doubt to come!

Function temp_EntityDistance:Float (src:Int, dest:Int)
	Return Sqr ((SGD.EntityX (src) - SGD.EntityX (dest)) ^ 2 + (SGD.EntityY (src) - SGD.EntityY (dest)) ^ 2 + (SGD.EntityZ (src) - SGD.EntityZ (dest)) ^ 2)
End Function
