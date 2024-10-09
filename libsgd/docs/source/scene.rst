Scene
=====

The scene is a collection of entities stored as a directed, acyclic graph, and a numer of rendering properties.

The Scene rendering properties are:

* Clear Color - a color value used to clear the scene. Use :any:`sgd_SetClearColor` to set this property.

* Environment Texture - a cube or equirect 2d texture that represents the scene environment for the purpose of specular reflection when lighting the scene. Use :any:`sgd_SetEnvTexture` to set this property.

* Ambient Light Color - a color value used to represent the scene's diffuse ambient lighting. Use :any:`sgd_SetAmbientLightColor` to set this property.

To render a scene, use the :any:`sgd_RenderScene function`.

Once the scene has been rendered, use the :any:`sgd_Present` function to copy the render result to the app window.

Example code
------------

.. tabs::

    .. code-tab:: c

        #include <sgd/sgd.h>

        sgd_Init();

        sgd_CreateWindow("Window",GetDesktopWidth()/2, GetDesktopHeight()/2, SGD_WINDOW_FLAGS_CENTERED);

        sgd_SetClearColor(1, .5f, 0, 1);

        while((sgd_PollEvents() & SGD_EVENT_MASK_CLOSE_CLICKED) == 0) {

            sgd_RenderScene();

            sgd_Present();
        }

        sgd_Terminate();

    .. code-tab:: py

        from libsgd import sgd

        sgd.init()

        sgd.createWindow("Window", GetDesktopWidth() // 2, GetDesktopHeight() // 2, sgd.WINDOW_FLAGS_CENTERED)

        sgd.setClearColor(1, .5, 0, 1)

        while (sgd.pollEvents() & sgd.EVENT_MASK_CLOSE_CLICKED) == 0:

            sgd.renderScene()

            sgd.present()

        sgd.terminate()

    .. code-tab:: blitzmax

        sgd.Init()

        sgd.CreateWindow "Window", GetDesktopWidth()/2, GetDesktopHeight()/2, sgd.WINDOW_FLAGS_CENTERED

        sgd.SetClearColor 1, .5, 0, 1

        while (PollEvents() & sgd.EVENT_MASK_CLOSE_CLICKED) = 0

            sgd.RenderScene()

            sgd.Present()

        wend

        sgd.Terminate()

    .. code-tab:: b3d

        CreateWindow "Window", GetDesktopWidth()/2, GetDesktopHeight()/2, WINDOW_FLAGS_CENTERED

        SetClearColor 1, .5, 0, 1

        While (PollEvents() And EVENT_MASK_CLOSE_CLICKED) = 0

            RenderScene()

            Present()

        Wend

Functions
---------

.. doxygengroup:: Scene
    :content-only:
