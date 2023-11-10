# ResourceGame

A game using modern c++ that shows many of the basic features, patterns, and approaches that can be used in building your own game engine.

## Building this Project

The project layout is based on [Modern CMake](https://cliutils.gitlab.io/modern-cmake/chapters/basics/structure.html) and [It's time to do CMake right](https://pabloariasal.github.io/2018/02/19/its-time-to-do-cmake-right/).

### Command Line
To configure this project using CMake, use:

```bash
cmake -S . -B build
```

To build the project, use:

```bash
cmake --build build
```

To generate project files for an IDE, e.g. for XCode, use:

```bash
cmake -S . -B xbuild -GXcode
cmake --open xbuild
``` 

#### MacOS < 10.14

Install llvm/clang (for extended C++17 stdlib support) using brew.

```bash
brew install llvm
```

### GUI

There is also the graphical CMake-GUI available, installed by default with CMake on windows.

### IDEs

We also recommend using an IDE such as [CLion](https://www.jetbrains.com) or [VisualStudio](https://www.visualstudio.com), both of which are available in our laboratories and for free for educational use (registration required).

### Building the Documentation

The project uses Doxygen to build the generation, if available.
