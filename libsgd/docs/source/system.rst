System
======

The system API allows you to control and monitor various low level aspects of LibSGD.

To startup and shutdown LibSGD, use the :any:`sgd_Init` and :any:`sgd_Terminate` functions. sgd_Init should be called before any other SGD function, except for sgd_SetConfigVar and sgd_SetErrorCallback which may be called earlier.

Use :any:`sgd_PollEvents` to poll for user input and system events. You need to call this fairly regularly or the OS may start displaying a 'busy' icon or pointer, or even think your app has crash and terminate it completely.

Types
-----

.. doxygengroup:: SystemTypes
    :content-only:

Functions
---------

.. doxygengroup:: System
    :content-only:
