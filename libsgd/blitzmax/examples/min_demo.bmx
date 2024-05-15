
'	KEYS:

' 	WASD				- forward/back/strafe
' 	CURSORS				- turn/look around
'	LEFT-SHIFT			- speed boost

' Set CAMERA_PITCH_INVERT to True to invert pitch control

Include "../sgd.bmx"

Const FULL_SCREEN:Int = False
'Const FULL_SCREEN:Int = True ' Keyboard control too twitchy. Vsync??

Const CAMERA_PITCH_INVERT:Int = False ' True to flip look-up/down on Xbox pad

' Normally place sgd_dynamic.dll in same folder and just
' call SGD.LoadSGD to import LibSGD. Distribution demo requires
' custom path, hence:

SGD.LoadSGD "../../lib/sgd_dynamic.dll"

SGD.Init ' Normal SGD setup call

' --------
' WINDOW:
' --------

Local flags:Int = SGD.WINDOW_FLAGS_RESIZABLE

If FULL_SCREEN
	flags = SGD.WINDOW_FLAGS_FULLSCREEN
EndIf

SGD.CreateWindow (1280, 720, "Min Demo - WASD/Shift plus CURSORS", flags)

' -----------------------------------------------------------------------------
' Ground materials -- uncomment one only:
' -----------------------------------------------------------------------------

'Local ground_texture_file:String = "Tiles019_1K-JPG"
'Local ground_texture_file:String = "Tiles093_1K-JPG"
'Local ground_texture_file:String = "Bricks076C_1K-JPG"
'Local ground_texture_file:String = "PavingStones119_1K-JPG"
Local ground_texture_file:String = "PavingStones131_1K-JPG"

' -----------------------------------------------------------------------------
' Envmaps -- uncomment one only:
' -----------------------------------------------------------------------------

'Local sky_texture_file:String = "nightsky-cube.png"
'Local sky_texture_file:String = "stormy-cube.jpg"
Local sky_texture_file:String = "sunnysky-cube.png"

' -----------------------------------------------------------------------------
' Selections from above...
' -----------------------------------------------------------------------------

Local ground_material_path:String	= "sgd://materials/"	+ ground_texture_file
Local sky_map_path:String			= "sgd://envmaps/"		+ sky_texture_file

' -------
' SCENE:
' -------

SGD.CreateScene ()
SGD.SetSceneAmbientLightColor 0, 0, 0, 1'0.025, 0.05, 0.0375, 1.0

' --------
' SKYBOX:
' --------
 
Local sky_texture:Int = SGD.LoadTexture (sky_map_path, SGD.TEXTURE_FORMAT_SRGBA8, SGD.TEXTURE_FLAGS_ENVMAP_DEFAULT)

SGD.SetSceneEnvTexture sky_texture

Local skybox:Int = SGD.CreateSkybox (sky_texture)
SGD.SetSkyboxRoughness skybox, 0.3

' -------
' LIGHT:
' -------

' Create a point light -- LibSGD doesn't yet support spotlight shadows:

Local light:Int = SGD.CreatePointLight ()

' Light details:

SGD.SetLightRange light, 25.0
SGD.SetLightCastsShadow (light, True) ' CASTING SHADOW
SGD.SetLightColor light, 1.0, 1.0, 1.0, 1.0

' Positioning:

SGD.MoveEntity light, -5, 5.0, 7.5
SGD.TurnEntity light, -22.5, -120, 0

' --------
' GROUND:
' --------

Local ground_size:Float = 250.0
Local ground_thickness:Float = 0.1

Local ground_material:Int	= SGD.LoadPBRMaterial (ground_material_path)
Local ground_mesh:Int		= SGD.CreateBoxMesh (-ground_size * 0.5, -ground_thickness * 0.5, -ground_size * 0.5, ground_size * 0.5, ground_thickness * 0.5, ground_size * 0.5, ground_material)
Local ground:Int			= SGD.CreateModel (ground_mesh)

' Scale texture...

Local ground_scale:Float = 50.0

SGD.TransformMeshTexCoords (ground_mesh, ground_scale, ground_scale, 0.0, 0.0)

' ------
' CUBE:
' ------

Local cube:Int			= SGD.CreateModel (0)
Local cube_material:Int	= SGD.CreatePBRMaterial ()
Local cube_size:Float	= 1.0
Local cube_mesh			= SGD.CreateBoxMesh (-cube_size * 0.5, -cube_size * 0.5, -cube_size * 0.5, cube_size * 0.5, cube_size * 0.5, cube_size * 0.5, cube_material)

SGD.SetMeshCastsShadow cube_mesh, True ' CUBE CASTS SHADOW

' Change colour here!

Local r:Float = 1.0	' Red	0.0 - 1.0
Local g:Float = 0.0 ' Green	0.0 - 1.0
Local b:Float = 0.0 ' Blue	0.0 - 1.0
Local a:Float = 1.0 ' Alpha	0.0 - 1.0

' Model details:

SGD.SetModelMesh cube, cube_mesh
SGD.SetModelColor cube, r, g, b, a

' Positioning:

SGD.MoveEntity cube, 0, cube_size * 0.5 + ground_thickness * 0.5, 5

' SGD.TurnEntity cube, -5, 0, 0 ' For axis direction checking!

' --------
' CAMERA:
' --------

Local camera:Int = SGD.CreatePerspectiveCamera ()

SGD.SetCameraNear camera, 0.05		' Allow extreme closeups!
SGD.MoveEntity camera, 0, 1.5, 0	' Positioning

' Window events variables...

Local events:Int
Local quit:Int = False

' Camera movement variables...

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
	
	Local shifted:Float = 1.0
	
	If SGD.KeyDown (SGD.KEY_LEFT_SHIFT)
		shifted = 2.0
	EndIf
	
	' A/D keys: strafe left/right...

	If SGD.KeyDown (SGD.KEY_A)
		strafe = strafe - 0.01 * shifted
		If strafe < -0.1 * shifted Then strafe = -0.1 * shifted
	Else
		If SGD.KeyDown (SGD.KEY_D)
			strafe = strafe + 0.1 * shifted
			If strafe > 0.1 * shifted Then strafe = 0.1 * shifted
		EndIf
	EndIf

	' W/S keys: forward/back...

	If SGD.KeyDown (SGD.KEY_W)
		z = z + 0.01 * shifted
		If z > 0.1 * shifted Then z = 0.1 * shifted
	Else
		If SGD.KeyDown (SGD.KEY_S)
			z = z - 0.01 * shifted
			If z < -0.1 * shifted Then z = -0.1 * shifted
		EndIf
	EndIf

	' Diminish all movement towards 0...
	
	pitch	= Util.Diminish (pitch, 0.03)
	yaw		= Util.Diminish (yaw, 0.03)
	strafe	= Util.Diminish (strafe, 0.0015)
	rise	= Util.Diminish (rise, 0.0015)
	z		= Util.Diminish (z, 0.0015)

	' Update camera...
	
	SGD.TurnEntity camera, pitch, 0, 0
	SGD.RotateEntity camera, 0, yaw, 0
	SGD.MoveEntity camera, strafe, rise, z
	
	' Render/show!
	
	SGD.RenderScene
	SGD.Present
	
Until quit

End

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
