
CreateWindow(640, 480, "Hello World!", 0)
CreateScene()
SetSceneClearColor 1, .5, 0, 1

While (PollEvents() And 1)=0
	RenderScene()
	Present()
Wend
