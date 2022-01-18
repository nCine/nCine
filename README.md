[![Linux](https://github.com/nCine/nCine/workflows/Linux/badge.svg)](https://github.com/nCine/nCine/actions?workflow=Linux)
[![macOS](https://github.com/nCine/nCine/workflows/macOS/badge.svg)](https://github.com/nCine/nCine/actions?workflow=macOS)
[![Windows](https://github.com/nCine/nCine/workflows/Windows/badge.svg)](https://github.com/nCine/nCine/actions?workflow=Windows)
[![MinGW](https://github.com/nCine/nCine/workflows/MinGW/badge.svg)](https://github.com/nCine/nCine/actions?workflow=MinGW)
[![Emscripten](https://github.com/nCine/nCine/workflows/Emscripten/badge.svg)](https://github.com/nCine/nCine/actions?workflow=Emscripten)
[![CodeQL](https://github.com/nCine/nCine/workflows/CodeQL/badge.svg)](https://github.com/nCine/nCine/actions?workflow=CodeQL)

# nCine
nCine is a cross-platform 2D game engine.  
It is released under the MIT License, Copyright (c) 2011-2022 Angelo Theodorou.  
For additional information: https://ncine.github.io

## Information

### Dependencies
- GLEW
- GLFW 3.x
- SDL 2
- libpng
- libwebp
- OpenAL-soft
- libogg, libvorbis, libvorbisfile
- Lua 5.4
- Dear ImGui
- Nuklear

### Supported Platforms and Compilers
- Windows (MSVC, MinGW-w64 on MSYS2)
- macOS (Clang)
- Linux (GCC, Clang)
- Android (GCC, Clang)
- Emscripten

### Development Tools
- git
- CMake
- Qt Creator
- Doxygen with GraphViz
- Valgrind
- Cppcheck
- clang-format
- Google Test and Gcovr
- Tracy frame profiler
- Google Benchmark
- RenderDoc graphics debugger

## Development

### Conventions

#### Coding Conventions
- Indent with tabs (4 spaces) but use spaces for continuation line alignment
- Allman brackets
  - No brackets around one line conditional statements
- Padding space after parenthesis headers (`if`, `do`, `while`, `for`, `switch`)
- Align pointer and reference operator to the variable or function name
- `inline` keyword always before `virtual` and `static`
- Mark an override method with the `override` specifier and remove `virtual`
- Access specifiers order: `public`, `protected`, `private`
  - One half indent for access specifiers (2 spaces)
- Declarations order: typedefs and enums, data members, constructors, destructors, methods
  - Friends defined in the private section of a class, after everything else
- One space padding around operators
- Use of the `explicit` keyword all the times it makes sense

#### Naming Conventions
- Pascal case for classes, enumerations, typedefs and constants
- Camel case for variables, functions and parameters
- All upper case for enumerators in an enumeration

#### Documenting Conventions
- Put Doxygen brief descriptions in header files and additional documentation in implementation ones
- Use Qt style for Doxygen detailed descriptions (`/*! */`) and end them with a period
