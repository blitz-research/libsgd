### Contents:

&bull; blitz3d/ : Contains Blitz3D replacement runtime.dll file and examples code. Copy runtime.dll to your existing V1.117 or later Blitz3D 'bin' directory to install.

&bull; blitzmax/ : Contains BlitzMax module file and examples.  

&bull; html/ : Contains Sphinx generated htmlhelp files.

&bull; include/ : Contains header files for C/C++ apps. This directory should be added to your C/C++ compiler's 'include directories' setting.

&bull; lib/ : Contains dynamic library for C/C++ apps. This directory should be added to your C/C++ compiler's 'library directories' setting. To use LibSGD in your C/C++ app, your app must link with libsgd_dynamic.lib, and libsgd_dynamic.dll must be copied to your executable's path so it can be found at runtime.

&bull; lua/ : Contains WIP sgd.lua wrapper for the Lua language using the 'luaJIT' environment. 

&bull; nodejs/ : Contains node.js wrapper module. The libsgd.node file must be in the current directory when you run a node.js app that wants to use libsgd.

&bull; python/ : Contains helloworld.py and examples. The Python libsgd package can be installed using pip on Windows, eg: ``python -m pip install --upgrade libsgd``

&bull; zig/ : Contains WIP sgd.zig wrapper for the Zig language.

&bull; README.md : This file.

&bull; VERSIONS.txt : Summary of version changes.


### Notes

Most folders contain simple 'helloworld' demos to get you started. See source code for simple build/run instructions. Some folders also contain an 'examples' directories with more elaborate examples.


### Credits

Big shout out to my awesome Patreon supporters!
```
Shane
Martin
Simon Harrison
MarLupin
Medi98
Chaduke
Michael Denathorn
James Boyd
Dex
Andy Hewco
Dan
Markus Rosse
Amon
JaviCervera
William Soennichsen
```
