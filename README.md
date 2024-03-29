# srcDiff

srcDiff is a syntactical differencer built off of the srcML infrastructure.

## Building srcDiff

### Windows

On Windows, due to the lack of a standard location or package manager for libraries, vcpkg should be used to install dependencies. [This is the official guide to installing vcpkg](https://learn.microsoft.com/en-us/vcpkg/get_started/get-started?pivots=shell-cmd#1---set-up-vcpkg). Following these steps is sufficient to install vcpkg for this build:

```powershell
cd C:/your/chosen/install/location
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.bat
# set the required VCPKG_ROOT environment variable to the current directory.
# note: this will not persist between PowerShell sessions unless you also add
# this as an environment variable in the Control Panel
$Env:VCPKG_ROOT = (pwd).path
```

Also, CMake is required to build srcDiff. The easiest way to install it on Windows is with [Chocolatey](https://chocolatey.org/install).

```powershell
choco install cmake -y
```

#### Compile with Clang

The recommended way to build srcDiff is with Clang, which can be used to compile srcDiff on a variety of platforms, and Ninja, which is a fast build automation tool. Ninja and Clang can also be installed with Chocolatey:

```powershell
choco install ninja -y
choco install llvm -y
```

After vcpkg, CMake, Ninja, and Clang are installed, you can configure and run the build with CMake:

```powershell
cmake -S ./ -B ./build --preset windows
cmake --build ./build/
```

srcDiff now exists in build/bin:

```powershell
cd build/bin
./srcdiff -V
```

#### Compile with MSVC

You can also build srcDiff with Microsoft's compiler and build tools. First, get Visual Studio 17 (2022) from [Microsoft's website](https://learn.microsoft.com/en-us/visualstudio/releases/2022/release-notes).

Then, configure and run the build with CMake:

```powershell
cmake -S ./ -B ./build --preset windows-msvc
cmake --build ./build/
```

srcDiff now exists in build/bin/Debug:

```powershell
cd build/bin/Debug
./srcdiff -V
```
