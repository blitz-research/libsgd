### Contents:

&bull; blitz3d/ : Contains Blitz3D replacement runtime.dll file and examples code. Copy runtime.dll to your existing V1.117 or later Blitz3D 'bin' directory to install.

&bull; blitzmax/ : Contains BlitzMax module file and examples.  

&bull; html/ : Contains Doxygen generated help files.

&bull; include/ : Contains header files for C/C++ apps. This directory should be added to your C/C++ compiler's 'include directories' setting.

&bull; lib/ : Contains dynamic library for C/C++ apps. This directory should be added to your C/C++ compiler's 'library directories' setting. To use LibSGD in your C/C++ app, your app must link with libsgd_dynamic.lib, and libsgd_dynamic.dll must be copied to your executable's path so it can be found at runtime.

&bull; python/ : Contains helloworld.py and examples. The python libsgd package can be installed using pip, eg: ``python -m pip install --upgrade libsgd``

&bull; nodejs/ : Contains node.js wrapper module. The libsgd.node file must be in the current directory when you run a node.js app that wants to use libsgd.

&bull; VERSIONS.txt : Summary of version changes.

&bull; README.md : This file.


### Notes

Most folders contain simple 'helloworld' demos to get you started. See source code for simple build/run instructions. Some folders also contain an 'examples' directories with more elaborate examples.


### Credits

Big shout out to my awesome Patreon supporters!
```
Martin
Shane
Simon Harrison
MarLupin
Medi98
Chaduke
Michael Denathorn
James Boyd
Dex
Mark Sibly
Andy Hewco
Dan
Markus Rosse
JaviCervera
Amon
```
