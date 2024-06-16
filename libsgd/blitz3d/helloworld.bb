Const KEY_ESCAPE = 256

CreateWindow(640, 480, "Hello World!", 0)
SetClearColor 1, .5, 0, 1

While (PollEvents() And 1)=0
	If IsKeyHit(KEY_ESCAPE) Exit
	RenderScene()
	Present()
Wend
