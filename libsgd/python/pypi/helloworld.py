#
# Simple helloworld example.
#
# The libsgd package must be built and installed to your local python environ first, eg:
#
# cd libsgd
# python311 -m build
# python311 -m pip install .
# python311 helloworld.py
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
