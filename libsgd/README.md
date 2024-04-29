### Contents:

``blitz3d/``: Contains Blitz3D replacement runtime.dll file and examples code. Copy runtime.dll to your existing Blitz3D 'bin' directory to install.

``blitzmax/``: Contains BlitzMax module file and examples.  

``html/``: Contains Doxygen generated help files.

``include/``: Contains header files for C/C++ apps. This directory should be added to your C/C++ compiler's include directories.

``lib/``: Contains dynamic library for C/C++ apps. To use LibSGD in your C/C++ app, your app must link with libsgd_dynamic.lib, and libsgd_dynamic.dll must be copied to your executable's path so it can be found at runtime.

``nodejs/``: Contains node.js wrapper module. The libsgd.node file must be in the current directory when you run a node.js app that wants to use libsgd.

``python/``: Contains python wrapper module. Both the _libsgd.py and _libsgd.pyd files must be in the current directory when you run a python app that wants to use libsgd.

``README.md``: This file.

``VERSIONS.txt``: Summary of version changes.


### Notes

Most folders contain very short 'helloworld' demos to get you started. See source code for simple build/run instructions.

This release only contains the 'dll' version of the sgd library, as the static version currently weighs in at over several hundred megabytes. We're working on improving this, but for now releases will be dll only.


### Credits

Big shout out to my awesome Patreon supporters!

Martin
Shane
Simon Harrison
Michael Hartlef
MarLupin
James Boyd
Michael Denathorn
Dex
Andy Hewco
Dan
Markus Rosse
Panayiotis Yianni
JaviCervera
Chaduke
