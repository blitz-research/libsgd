#
# The libsgd package must be installed first, eg:
#
# python -m install libsgd
#
from libsgd import sgd

sgd.init()

sgd.createWindow(640, 480, "Hello World!", sgd.WINDOW_FLAGS_RESIZABLE)

sgd.setClearColor(1,.5,0,1)

while not (sgd.pollEvents() & sgd.EVENT_MASK_CLOSE_CLICKED):

    if sgd.isKeyHit(sgd.KEY_ESCAPE):
        break

    sgd.renderScene()

    sgd.present()

sgd.terminate()
