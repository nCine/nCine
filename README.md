# nCine
nCine is a multi-platform 2D game engine.  
It is released under the MIT License, Copyright (c) 2011-2016 Angelo Theodorou.  
For additional information: https://ncine.github.io/

## Information

### Dependencies
- GLEW
- GLFW 3.x
- SDL 1.2.x (deprecated)
- SDL_image 1.2.x (deprecated)
- libwebp
- libpng
- OpenAL-soft
- libogg, libvorbis, libvorbisfile

### Supported Platforms and Compilers
- Windows (MSVC, MinGW-w64 on MSYS2)
- Mac Os X (LLVM)
- Linux (GCC, LLVM)
- Android (GCC, LLVM)

### Development Tools
- git
- CMake
- Qt Creator
- Doxygen with GraphViz
- Valgrind
- Cppcheck
- Artistic Style

## Development

### Tool Notes

#### Cppcheck
`cppcheck --quiet --enable=all --inconclusive --std=c++03 -I include -I src/include src 2> cppcheck.log`

#### Artistic Style
`astyle --style=allman --indent=tab --attach-namespaces --indent-modifiers --indent-switches --indent-preproc-block --min-conditional-indent=0 --pad-oper --pad-header --add-brackets --align-pointer=name --align-reference=name --keep-one-line-blocks --keep-one-line-statements --suffix=none --recursive "src/*.h" "src/*.cpp" "include/*.h" "tests/*.h" "tests/*.cpp"`

### Conventions

#### Coding Conventions
- Indent with tabs but use spaces for continuation line alignment
- Allman brackets
  - Brackets around one line conditional statements
- Padding space after parenthesis headers (`if`, `do`, `while`, `for`, `switch`)
- Align pointer and reference operator to the variable or function name
- `inline` keyword always before `virtual` and `static`
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
