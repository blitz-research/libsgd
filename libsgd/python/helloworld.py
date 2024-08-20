#
# The libsgd package must be installed first, eg:
#
# python -m install libsgd
#
from libsgd import sgd

sgd.Init()

sgd.CreateWindow(640, 480, "Hello World!", sgd.WINDOW_FLAGS_RESIZABLE)

sgd.SetClearColor(1,.5,0,1)

while not (sgd.PollEvents() & sgd.EVENT_MASK_CLOSE_CLICKED):

    if sgd.IsKeyHit(sgd.KEY_ESCAPE):
        break

    sgd.RenderScene()

    sgd.Present()

sgd.Terminate()
