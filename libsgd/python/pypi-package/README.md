# LibSGD 

## Simple Game Development Library

LibSGD is a game development library that provides a high level, easy to use 'scene graph' style API for writing games and apps.

LibSGD is open source software licensed under the Zlib/PNG license.

The LibSGD github repository is here: https://github.com/blitz-research/libsgd

The API reference can be found here: https://skirmish-dev.net/libsgd/help/html/sgd_8h.html

You can support the LibSGD project through its Patreon page: https://www.patreon.com/libsgd

Feel free to drop in to the LibSGD community here: https://skirmish-dev.net/forum

Here is an example of a minimal LibSGD app:

```python
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
```

LibSGD is not yet at V1.0 so please be aware that there will be changes made before then, although hopefully nothing major!
