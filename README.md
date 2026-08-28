# duGL

First attempt at an OpenGL rendering engine. 

Prerequisites: 
- Python 3 (for generating GLAD). 
- C++20 or newer.
- CMake 3.26 or newer.
- Ninja.
- A GPU with OpenGL support.

Other dependencies are automatically installed with CMake scripts when generating the buildsystem.

## Building

The build is defined by `CMakePresets.json`, so the same configuration is used everywhere: from the command line, from Visual Studio (which reads the presets directly), and from CI.

On Linux and macOS:

```
cmake --preset x64-Debug
cmake --build --preset x64-Debug
```

On Windows, run `scripts/build.ps1` instead, or build from Visual Studio. MSVC needs its developer environment set before the compiler can find the standard and Windows SDK headers; Visual Studio applies that itself, and the script applies it for command-line builds. Substitute `x64-Release` (or pass `-Preset x64-Release`) for an optimised build.