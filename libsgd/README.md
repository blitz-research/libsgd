### LibSGD release V0.03

#### Contents:

``include``: LibSGD Header files for C/C++ programs.

``lib/windows-x64``: Windows 64 bit dynamic lib for C/C++ apps. To use LibSGD in your C/C++ app, your app must link with libsgd_dynamic.lib, and libsgd_dynamic.dll must be copied to your executable's path so it can be found at runtime.

``lib/linux-x64``: Linux 64 bit dynamic (shared) lib for C/C++ apps. To use LibSGD in yourt C/C++ app, your app must link with libsgd_dynamic.so, and libsgd_dynamic.so must also be copied to your executable's path so it can be found at runtime.

``nodejs/windows-x64``: Windows 64 bit node.js module. The libsgd.node file must be in the current directory when you run a node.js app that wants to use libsgd.

``python/windows-x64``: Windows 64 bit python module. Both the _libsgd.py and _libsgd.pyd files must be in the current directory when you run a python app that wants to use libsgd.

``help/html``: Doxygen generated help files.

``blitz3d``: Blitz3d replacement runtime.dll and sample code.

#### Notes

Most folders contain very short 'helloworld' demos to get you started. See source code for simple build/run instructions.

This release only contains the 'dll' version of the sgd library, as the static version currently weighs in at over several hundred megabytes. We're working on improving this, but in the meantime if you need the static lib for some reason (eg: both the node.js and blitz3d targets link with it, even though they only produce 8M libs) please get in touch at the Blitz World forum: skirmish-dev.net/forum

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
