Scene
=====

The scene is a collection of entities stored as a directed, acyclic graph, and a numer of rendering properties.

The Scene rendering properties are:

* Clear Color - a color value used to clear the scene. Use :any:`sgd_SetClearColor` to set this property.

* Environment Texture - a cube or equirect 2d texture that represents the scene environment for the purpose of specular reflection when lighting the scene. Use :any:`sgd_SetEnvTexture` to set this property.

* Ambient Light Color - a color value used to represent the scene's diffuse ambient lighting. Use :any:`sgd_SetAmbientLightColor` to set this property.


Rendering and presenting
------------------------

To render a scene, use the :any:`sgd_RenderScene function`.

Once the scene has been rendered, use the :any:`sgd_Present` function to copy the render result to the app window.

.. doxygengroup:: Scene
