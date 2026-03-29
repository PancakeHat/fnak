# Five Nights at Korniluk's


### Download:
Executables can be found in the [Releases Tab](https://github.com/PancakeHat/fnak/releases).

****

### A fair warning:
This project is slop. The code is slop, the assets are slop, the mechanics are slop, it's slop all the way down. It is a miracle that this runs at all in the first place and I am not going back to fix problems. If you want to build it yourself, the instructions are below.

### Build from source:

**Prerequisites:**
- [CMake](https://cmake.org/)
- A CMake supported build program, e.g. [Make](https://gnuwin32.sourceforge.net/packages/make.htm) or [Ninja](https://ninja-build.org/)
- A C/C++ 20 compatible Compiler like [MinGW-w64](https://www.mingw-w64.org/) (Windows only)

**Build Support:**
- Windows: Yes
- MacOS: Maybe (Untested)
- Linux: Yes (only tested on Debian)

**Linux Note**
- GCC 13 or higher is needed. **Make sure CMake is using GCC 13**

**Finally Building:**
*(Steps shown are for Windows)*
1. Create and enter build folder:  
`mkdir build`  
`cd build`  
2. CMake stuff:  
`cmake ..` (add `-G "MinGW Makefiles"` for Make build)
3. Compile:  
`ninja` or `make` depending on what build system you used
5. Copy over the asset folder into the build directory:  
`cp -r ../assets ./assets`  
4. Done. The exectuable should be ready.
