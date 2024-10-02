# LibSGD 

## Simple Game Development Library

LibSGD is a game development library that provides a high level, easy to use 'scene graph' style API for writing games and apps.

LibSGD is open source software licensed under the Zlib/PNG license.

Feel free to drop in to the LibSGD community here: https://skirmish-dev.net/forum

The API reference can be found here: https://skirmish-dev.net/libsgd/help/html/index.html

You can support the LibSGD project through its Patreon page: https://www.patreon.com/libsgd

The LibSGD github repository is here: https://github.com/blitz-research/libsgd

Here is an example of a minimal LibSGD app:

```python
from libsgd import sgd

sgd.init()

sgd.createWindow(640, 480, "Hello World!", sgd.WINDOW_FLAGS_CENTERED | sgd.WINDOW_FLAGS_RESIZABLE)

sgd.setClearColor(1,.5,0,1)

while not (sgd.pollEvents() & sgd.EVENT_MASK_CLOSE_CLICKED):

	if sgd.isKeyHit(sgd.KEY_ESCAPE):
		break

	sgd.renderScene()

	sgd.present()

sgd.terminate()
```

LibSGD is not yet at V1.0 so please be aware that there will be changes made before then, although hopefully nothing major!
