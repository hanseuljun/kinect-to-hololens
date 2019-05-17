New-Item .\build -ItemType directory
New-Item .\build\x86 -ItemType directory
New-Item .\build\x64 -ItemType directory

cmake -S . -B .\build\x86 -G "Visual Studio 15 2017" -A Win32 -DCMAKE_TOOLCHAIN_FILE=..\vcpkg\scripts\buildsystems\vcpkg.cmake
cmake -S . -B .\build\x64 -G "Visual Studio 15 2017" -A x64 -DCMAKE_TOOLCHAIN_FILE=D:..\vcpkg\scripts\buildsystems\vcpkg.cmake