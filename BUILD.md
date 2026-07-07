# srcDiff

srcDiff is a syntactical differencer built off of the srcML infrastructure.

## Getting the Source Code

Because srcDiff relies on the `srcSAX` module, you must clone the repository with its submodules.

```bash
git clone --recurse-submodules https://github.com/srcDiff/srcDiff
cd srcDiff
```

*(If you already cloned without submodules, you can fetch them by running `git submodule update --init --recursive` inside the repository root).*
## Building srcDiff

### Prerequisites
Before running CMake, ensure your system has all required build tools and project libraries installed.

* **srcML:** please refer to the official build instructions here: [https://github.com/srcML/srcML/blob/develop/BUILD.md](https://github.com/srcML/srcML/blob/develop/BUILD.md).
* **CMake:** Version 3.28 or higher is required.
* **C++ Compiler:** Must support C++20.
* **Ninja:** Used as the primary build generator for the CI presets.
* **Git:** Required for CMake to fetch internal dependencies like CLI11.
* **libxml2:** Required for XML parsing.
* **Boost:** Required for various utility functions.

### Unix-based (Linux & macOS)

**Ubuntu or Debian-based OS:**
```bash
sudo apt update
sudo apt install -y cmake git clang ninja-build libxml2-dev libboost-all-dev
```

**Fedora or RPM-based OS (RHEL/CentOS):**
```bash
sudo dnf install -y cmake git clang ninja-build libxml2-devel boost-devel
```

**macOS:**
*(Requires Homebrew)*
```bash
brew install cmake git llvm ninja libxml2 boost
```

#### Unix Build Instructions
Once the system dependencies and `srcML` are installed, use the built-in CMake presets to configure and build. In the repository root directory, run the commands corresponding to your OS. A `build` directory will be created automatically.

**Ubuntu or Debian-based OS:**
```bash
cmake --preset ci-debian
cmake --build ./build/
```

**Fedora or RPM-based OS (RHEL/CentOS):**
```bash
cmake --preset ci-rhel
cmake --build ./build/
```

**macOS:**
```bash
cmake --preset ci-macos
cmake --build ./build/
```

### Windows

On Windows, `vcpkg` is heavily integrated into the CMake presets to handle dependencies (like Boost, libxml2, and CLI11). 

#### 1. Setup vcpkg
[This is the official guide to installing vcpkg](https://learn.microsoft.com/en-us/vcpkg/get_started/get-started?pivots=shell-cmd#1---set-up-vcpkg). Following these steps is sufficient to install vcpkg for this build:

```powershell
cd C:/your/chosen/install/location
git clone [https://github.com/microsoft/vcpkg.git](https://github.com/microsoft/vcpkg.git)
cd vcpkg
./bootstrap-vcpkg.bat
# Set the required VCPKG_ROOT environment variable to the current directory.
# Note: this will not persist between PowerShell sessions unless you also add
# this as a system environment variable in the Windows Settings.
$Env:VCPKG_ROOT = (pwd).path
```

#### 2. Install Build Tools
CMake is required to configure the project. The easiest way to install CMake, Ninja, and Clang on Windows is with [Chocolatey](https://chocolatey.org/install):

```powershell
choco install cmake git ninja llvm -y
```

#### 3. Compile with Clang (Recommended)
The recommended way to build srcDiff on Windows is with Clang and Ninja. Once vcpkg and the build tools are installed, configure and run the build:

```powershell
# Return to the srcDiff repository root
cmake -S ./ -B ./build --preset windows
cmake --build ./build/
```

The compiled executable will be located in `build/bin`:
```powershell
cd build/bin
./srcdiff -V
```

#### Alternative: Compile with MSVC
You can also build srcDiff with Microsoft's compiler and build tools. First, ensure you have installed Visual Studio 17 (2022) with C++ development tools from [Microsoft's website](https://learn.microsoft.com/en-us/visualstudio/releases/2022/release-notes).

Then, configure and run the build using the MSVC preset:

```powershell
cmake -S ./ -B ./build --preset windows-msvc
cmake --build ./build/
```

The compiled executable will be located in `build/bin/Debug/` (or `Release/` depending on your configuration).