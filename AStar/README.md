# Maze Visualiser

This project has been created for the COMPSYS301 design
project. It is meant to be able to visualise path finding
code. I created it so that pathfinding code could be written
in C and tested in C instead of porting C code over to
MATLAB.

To use this code, simply modify the algorithm inside
`algo.c`. Currently, the algorithm uses a modified version
of DFS, and is nowhere near optimal, so it shouldn't be
used.

## Requirements

**Windows**:
- `cmake`: A cross-platform build system ([link](cmake.org))
- A C compiler

*OR*

- `Visual Studio`: Microsofts IDE for C/C++ and other
  projects ([link](https://visualstudio.microsoft.com/#vs-section))

**macOS / Linux**
- `raylib`: C library for drawing visuals
  ([link](https://www.raylib.com/))
- A C compiler
-`pkg-config`: Finds the correct verisons of installed
  libraries ([link](https://www.freedesktop.org/wiki/Software/pkg-config/))

If you're using macOS, you should probably use `brew` or
`macports` to install the dependencies. i.e.
```
brew install pkg-config raylib
```

If you're using linux, you can just use your distros package
manager to install the dependencies.

If you're using Windows, you can install CMake from
`cmake.org`
***MAKE SURE TO ADD CMAKE TO PATH IN THE INSTALLER***.

## Building

If you're on macOS or Linux, you can simply just install the
dependencies listed above, and then run `./build.sh`.

### Building on Windows (Visual Studio)

You can open CMake projects in Visual Studio. I'm not sure
which button to click to do it, but I think it is simple
enough.

### Building on Windows (Command Line)

On Windows, to setup cmake, you need to run
```pwsh
mkdir build # Create a build directory
cd bin && cmake .. # Build from that directory so the build files are in one place
```

Once CMake is setup, to rebuild you code, you can just run
```
cmake --build . # Build the project
```
from inside the `bin` directory. You do not need to run
the CMake setup more than once.
