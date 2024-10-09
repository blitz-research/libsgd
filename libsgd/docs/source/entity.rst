Entity
======

An entity is a visual element in the scene. Every entity has the following properties:

* Name: name of the entity, as read from file or set by user. Use :any:`sgd_SetEntityName` and :any:`sgd_GetEntityName` to get and set this property. Use :any:`sgd_FindEntityChild` to search an entity and its descendants for an entity with a specific name.

* Position - position of the entity, stored as x, y, z coordinates. Use :any:`sgd_SetEntityPosition`, :any:`sgd_GetEntityX`, :any:`sgd_GetEntityY` and :any:`sgd_GetEntityZ` to set and get this property.

* Rotation - rotation of the entity, stored as a 3 X 3 orthogonal, normalized 'basis' matrix. Use :any:`sgd_SetEntityRotation`, :any:`sgd_GetEntityRX`, :any:`sgd_GetEntityRY` and :any:`sgd_GetEntityRZ` to get and set entity rotation using euler angles in the range 0..360.

* Scale - scale of the entity, stored as x, y, z scale values. Use :any:`sgd_SetEntityScale`, :any:`sgd_GetEntitySX`, :any:`sgd_GetEntitySY` and :any:`sgd_GetEntitySZ` to get and set this property.

* Parent - reference to parent entity. Use :any:`sgd_SetEntityParent` and :any:`sgd_GetEntityParent` to set and get this property.

* Children - child entities. Use :any:`sgd_GetEntityChildCount` and :any:`sgd_GetEntityChild` to visit the children of an entity.

* Visible flag - flag indicating whther the entity is visible or not. Use :aNY:`sgd_SetEntityVisible` and :any:`sgd_IsEntityVisible` to set and get this property. For an entity to be rendered by :any:`sgd_RenderScene`, it and its ancestor's visible flags must be set to true.

* Enabled flag - flag indicating when the entity is enabled or not. Use :any:`sgd_SetEntityEnabled` and :any:`sgd_IsEntityEnabled` to set and get this property. For an entity to be collided with or picked, it and it's ancestor's enabled flags must be set to true.


Types
-----

.. doxygengroup:: EntityTypes
    :content-only:

Functions
---------

.. doxygengroup:: Entity
    :content-only:
