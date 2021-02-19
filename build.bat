@echo off

call "D:\WindowsPrograms\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64
mkdir build
pushd build
rem -O1 is needed or else we get an access violation on x86 for some reason.
cl -Fe:dinput8.dll -LD ..\src\dinput8.cpp user32.lib
copy dinput8.dll "D:\WindowsUsers\Rhett\Desktop\6220904-march2015\"
rem copy dinput8.pdb "D:\WindowsUsers\Rhett\Desktop\6220904-march2015\"
rem copy vc140.pdb "D:\WindowsUsers\Rhett\Desktop\6220904-march2015\"
popd
pushd "D:\WindowsUsers\Rhett\Desktop\6220904-march2015"
call _run_solo_x64.bat
popd