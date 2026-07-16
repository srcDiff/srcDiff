## Getting the Source Code

srcDiff relies on the [`srcSAX`](https://github.com/srcML/srcSAX) module. Therefore, you must clone the repository with its submodules.

```bash
git clone --recurse-submodules https://github.com/srcDiff/srcDiff
cd srcDiff
```

*(If you already cloned without submodules, you can fetch them by running `git submodule update --init --recursive` inside the repository root).*

## Building srcDiff

### Prerequisites
Before running CMake, ensure your system has all required build tools and project libraries installed.

#### Build Tools
* **CMake:** Version 3.28 or higher is required.
* **C++ Compiler:** Must support C++20.
* **Ninja:** Used as the primary build generator. 
* Note: **Git:** Is used by CMake to fetch internal dependencies like CLI11.

#### Libraries
* **srcML:** please refer to the official build instructions here: [https://github.com/srcML/srcML/blob/develop/BUILD.md](https://github.com/srcML/srcML/blob/develop/BUILD.md).
* **libxml2:** Required for XML parsing.
* **Boost (Optional):** Required only if you plan on running the unit test suite.

### Unix-based (Linux & macOS)

**macOS:**
*(Requires Xcode Command Line Tools and Homebrew)*
```bash
xcode-select --install
brew install cmake ninja libxml2
```

**Ubuntu or Debian-based OS:**
```bash
sudo apt update
sudo apt install -y cmake build-essential ninja-build libxml2-dev
```

**Fedora or RPM-based OS (RHEL/CentOS):**
```bash
dnf install -y cmake gcc gcc-c++ ninja-build libxml2-devel
```

#### Unix Build Instructions
Once the system dependencies and `srcML` are installed, use the built-in CMake presets to configure and build. Run these commands from the **root of the `srcDiff` repository**. The configuration preset will automatically create the `build` directory for you.

> **Note:** The presets below configure the standard build. If you want to build with testing enabled, you can prepend `ci-` to your preset (e.g., change `macos` to `ci-macos`).

**macOS:**
```bash
cmake --preset macos
cmake --build build
```

**Ubuntu or Debian-based OS:**
```bash
# Ensure you are in the srcDiff root directory
cmake --preset debian   # This generates the 'build' directory
cmake --build build
```

**Fedora or RPM-based OS (RHEL/CentOS):**
```bash
cmake --preset rhel
cmake --build build
```

### Windows
On Windows, `vcpkg` is heavily integrated into the CMake presets to handle dependencies (like libxml2 and CLI11). 

#### 1. Setup vcpkg
[This is the official guide to installing vcpkg](https://learn.microsoft.com/en-us/vcpkg/get_started/get-started?pivots=shell-cmd#1---set-up-vcpkg). Following these steps is sufficient to install vcpkg for this build:

```powershell
cd C:/your/chosen/install/location
git clone https://github.com/microsoft/vcpkg.git
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
cmake -S ./ -B build --preset windows
cmake --build build
```

The compiled executable will be located in `build/bin`:
```powershell
cd build/bin
./srcdiff -V
```

#### Alternative: Compile with MSVC
You can also build srcDiff with Microsoft's compiler and build tools. First, ensure you have installed Visual Studio 2026 with C++ development tools from [Microsoft's website](https://learn.microsoft.com/en-us/visualstudio/releases/2026/release-notes).

Then, configure and run the build using the MSVC preset:

```powershell
cmake -S ./ -B build --preset windows-msvc
cmake --build build
```

The compiled executable will be located in `build/bin/Debug/` (or `Release/` depending on your configuration).

## Running Tests (Optional)
Testing requires **Boost** (e.g., `boost` on macOS, `libboost-all-dev` on Debian/Ubuntu, `boost-devel` on Fedora/RHEL, or via `vcpkg` on Windows). 

To configure and build srcDiff with testing, run CMake with a `ci-*` preset (e.g., `ci-macos`).

srcDiff testing consists of:
* Delta Testsuite - A suite of tests that check correct delta generation.
* Unit Testsuite - A unit testing suite for srcDiff classes/methods/functions.

The following shows how to execute each type of srcDiff testing.

**Delta Testsuite:**
```bash
# Ensure you are in the srcDiff root directory
ninja -C build suite
```

**Unit Testsuite:**
```bash
cd build
ctest
```