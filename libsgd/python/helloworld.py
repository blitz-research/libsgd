import sgd

sgd.Init()

sgd.CreateWindow(640, 480, "Hello World!", 0)
sgd.CreateScene()
sgd.SetSceneClearColor(1,.5,0,1)

while (sgd.PollEvents() & sgd.EVENT_MASK_CLOSE_CLICKED) == 0:
    if sgd.KeyHit(sgd.KEY_ESCAPE):
        break
    sgd.RenderScene()
    sgd.Present()

sgd.Terminate()
