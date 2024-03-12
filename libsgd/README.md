### LibSGD release V24.03

#### Contents:

``include``: LibSGD Header files for C/C++ programs.

``lib/windows-x64``: Windows 64 bit dynamic lib for C/C++ apps. To use LibSGD in your C/C++ app, your app must link with libsgd_dynamic.lib, and libsgd_dynamic.dll must be copied to your executable's path so it can be found at runtime.

``lib/linux-x64``: Linux 64 bit dynamic (shared) lib for C/C++ apps. To use LibSGD in yourt C/C++ app, your app must link with libsgd_dynamic.so, and libsgd_dynamic.so must also be copied to your executable's path so it can be found at runtime.

``nodejs/windows-x64``: Windows 64 bit node.js module. The libsgd.node file must be in the current directory when you run a node.js app that wants to use libsgd.

``python/windows-x64``: Windows 64 bit python module. Both the _libsgd.py and _libsgd.pyd files must be in the current directory when you run a python app that wants to use libsgd.

``blitz3d/runtime.dll``: Runtime lib for Blitz3d. This should be copied to your Blitz3D's 'bin' directory. Note that it entirely replaces the existing 2d/3d command set, so you should back up your current version first.

``examples``: a bunch of simple examples.

#### Notes

Most folders contain very short 'helloworld' demos to get you started.

This release only contains the 'dll' versions of LibSGD. This is because the 64 bit release version of the static lib currently comes it at around 500M(!) and I'm not sure why as yet (The 32 bit version is 'only' 157M) so to keep the release tidier I'm leaving it out at this stage.

Ditto, this releases contains 64 bit releases only, except for Blitz3D. If you need a static or 32 bit lib for Windows/Linux, please contact me.

I am not yet sure how to 'package' node.js and python modules correctly so using them is currently a bit untidy.

##### Credits

Big shout out to my awesome Patreon supporters!
	
MarLupin
Dan
Martin
Michael Denathorn
Dex
Michael Hartlef
Andy Hewco
James Boyd
Shane
Markus Rosse
