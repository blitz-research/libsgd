### LibSGD

LibSGD is a simple game development library that provides a high level, easy to use 'scene graph' style API for writing games and apps.

LibSGD also support loading and playing audio and will eventually include network functionality.

The library is coded in C++, but the primary public API is pure 'C', making it highly compatible with a wide range of platforms and languages.

Currently supported languages include C/C++, python and nodejs. Support for more languages  is on the way.

LibSGD is licensed under the Zlib/PNG license.

Joins the LibSFD discussion at https://skirmish-dev.net/fourm

Help support LibSGD at https://patreon.com/libsgd


### CMake build instuctions for 64bit Linux.

* You will need at least cmake, ninja and python3.

* CD to repos checkout dir and...

* Configure: ``cmake -S . -B cmake-build-linux-release -DCMAKE_BUILD_TYPE=MinSizeRel -G Ninja``

* Build: ``cmake --build cmake-build-linux-release``

* Install: ``cmake --install cmake-build-linux-release``

After install output files should end up in cmake-build-linux-release/LIBSGD_INSTALL dir.

You may also need to install dev packages for things like X11 and so on.


### CMake build instuctions for 64bit Windows 10.

* Install cmake, python3 and MSVC (I use the free 2022 Community Edition).

* Git-bash or msys2 etc also highly recommended for bash compatible shell.

* CD to repos checkout dir and...

* Configure: ``cmake -S . -B cmake-build-windows-release -G "Visual Studio 17 2022"``

* Build: ``cmake --build cmake-build-windows-release --config MinSizeRel``

* Install: ``cmake --install cmake-build-windows-release --config MinSizeRel``

After install output files should end up in cmake-build-windows-release/LIBSGD_INSTALL dir.
