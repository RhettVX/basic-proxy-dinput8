@echo off

call "D:\WindowsPrograms\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x86
mkdir build
pushd build
rem -O1 is needed or else we get an access violation on x86 for some reason.
cl -Fe:dinput8.dll -O1 -LD ..\src\dinput8.cpp user32.lib
popd