import sgd

sgd.CreateWindow(640, 480, "Hello World!", 0)
sgd.CreateScene()
sgd.SetSceneClearColor(1,.5,0,1)

while(not sgd.PollEvents()):
    sgd.RenderScene()
    sgd.Present()
