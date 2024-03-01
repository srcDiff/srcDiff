# srcDiff
[![Build Status](https://img.shields.io/circleci/project/github/RedSparr0w/node-csgo-parser.svg)](https://circleci.com/gh/srcML/srcDiff)

srcDiff is a syntactical differencer built off of the srcML infrastructure.

## Building srcDiff

### Windows

#### Clang

Install CMake, Ninja, and Clang. The easiest way to do that is with Chocolatey:

```powershell
choco install cmake -y
choco install ninja -y
choco install llvm -y
```

Install vcpkg: [Guide](https://learn.microsoft.com/en-us/vcpkg/get_started/get-started?pivots=shell-cmd#1---set-up-vcpkg). Short version:

```powershell
cd C:/your/chosen/install/location
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.bat
$Env:VCPKG_ROOT = "C:/your/chosen/install/location"
```

Configure and run with CMake:

```powershell
cmake -S ./ -B ./build --preset windows-clang
cmake --build ./build/
```
