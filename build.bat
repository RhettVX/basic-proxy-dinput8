@echo off


call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64

setlocal EnableDelayedExpansion
setlocal EnableExtensions


set OUTPUT_NAME=dinput8.dll
set SOURCES=..\src\main.cpp
set DEFINES=/D_CRT_SECURE_NO_WARNINGS /DX64 /DC_TARGET_3
rem /O1 is needed or else we get an access violation on x86
set COMPILATION_FLAGS=/Od /Zi /FC /W4
set LINK_FLAGS=/link /DEBUG:FULL /DLL kernel32.lib user32.lib vcruntime.lib


IF NOT EXIST build mkdir build
pushd build
cl /nologo /Fe:!OUTPUT_NAME! !DEFINES! !COMPILATION_FLAGS! !SOURCES! !LINK_FLAGS!
rem copy dinput8.dll "D:\WindowsUsers\Rhett\Desktop\PlanetSide 2 Beta Sept Late\"
rem copy dinput8.pdb "D:\WindowsUsers\Rhett\Desktop\PlanetSide 2 Beta Sept Late\"
rem copy dinput8.dll "D:\WindowsUsers\Rhett\Desktop\PlanetSide 2 Beta Sept 8\"
rem copy dinput8.dll "D:\WindowsUsers\Rhett\Desktop\PlanetSide 2 Beta July 19\"
rem copy dinput8.dll "D:\WindowsUsers\Rhett\Desktop\PlanetSide 2 Server Target 2"
copy dinput8.dll "D:\WindowsUsers\Rhett\Desktop\PlanetSide 2 Test Server Target 3"
rem copy dinput8.pdb "D:\WindowsUsers\Rhett\Desktop\PlanetSide 2 Beta Sept 8\"
rem copy dinput8.dll "D:\WindowsUsers\Rhett\Desktop\PlanetSide 2 Admin - Copy\"
rem copy dinput8.pdb "D:\WindowsUsers\Rhett\Desktop\PlanetSide 2 Admin\"
popd
