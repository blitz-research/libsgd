Materials
=========

Materials are used when creating mesh surfaces, and affect how the surface appears when rendered by lighting.

There are several kinds of materials available, and each material supports different parameters that are set using one of :any:`sgd_SetMaterialTexture`, :any:`sgd_SetMaterialColor` and :any:`sgd_SetMaterialFloat`.

Material's also have a :any:`SGD_BlendMode`, :any:`SGD_DepthFunc` and :any:`SGD_CullMode`.


PBR Materials
-------------

PBR (physically based rendering) materials are the most physically accurate type of material available, but also the slowest to render.

To load a PBR material from file use :any:`sgd_LoadPBRMaterial`, and to create a PBR material from scratch use :any:`sgd_CreatePBRMaterial`.

The material parameters available for PBR materials are:

..  list-table:: PBR Material Parameters
    :widths: 25 25 50
    :header-rows: 1

    * - Parameter
      - Tyes(s)
      - Description
    * - "albedo"
      - texture, color
      - Base color used for PBR lighting
    * - "emissive"
      - texture, color
      - Emissive color applied after PBR lighting
    * - "roughness"
      - texture, float
      - Roughness value used for lighting. In the case of texture parameters, this value is read from the texture green component.
    * - "metalness"
      - texture, float
      - Metalness value used for lighting. In the case of texture parameters, this value is read from the texture blue component.
    * - "occlusion"
      - texture
      - Ambient occlusion value, read from the texture red component.
    * - "normal"
      - texture
      - Surface normal map.


Prelit Materials
----------------

Prelit materials are the simplest type of material you can use. Prelit materials support a single "emissive" texture and a single "emissive" color, which are multiplied together to produce a final surface color. Prelit materials are completed unaffecrted by scene lighting.


Types
-----

.. doxygengroup:: MaterialTypes
    :content-only:


Functions
---------

.. doxygengroup:: Material
    :content-only:
