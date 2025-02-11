<p align="center"><a href="https://ncine.github.io" target="_blank" rel="noopener noreferrer"><img width="192" src="https://github.com/nCine/nCine-data/blob/master/icons/icon192.png" alt="nCine logo"></a></p>

# nCine
nCine is a cross-platform 2D game engine with an emphasis on performance. It is written in C++11 but it can optionally be scripted with Lua.  
It is released under the MIT License and it has been in active development since June 2011.  

For additional information (like [features](https://ncine.github.io/features/), [gallery](https://ncine.github.io/gallery/), [videos](https://ncine.github.io/videos/)): https://ncine.github.io

Copyright (c) 2011-2025 Angelo Theodorou.

***

[![Linux](https://github.com/nCine/nCine/workflows/Linux/badge.svg)](https://github.com/nCine/nCine/actions?workflow=Linux)
[![macOS](https://github.com/nCine/nCine/workflows/macOS/badge.svg)](https://github.com/nCine/nCine/actions?workflow=macOS)
[![Windows](https://github.com/nCine/nCine/workflows/Windows/badge.svg)](https://github.com/nCine/nCine/actions?workflow=Windows)
[![MinGW](https://github.com/nCine/nCine/workflows/MinGW/badge.svg)](https://github.com/nCine/nCine/actions?workflow=MinGW)
[![Emscripten](https://github.com/nCine/nCine/workflows/Emscripten/badge.svg)](https://github.com/nCine/nCine/actions?workflow=Emscripten)
[![CodeQL](https://github.com/nCine/nCine/workflows/CodeQL/badge.svg)](https://github.com/nCine/nCine/actions?workflow=CodeQL)  
[![License](https://img.shields.io/github/license/nCine/nCine.svg)](https://github.com/nCine/nCine/blob/master/LICENSE)
[![Join the Discord server](https://discordapp.com/api/guilds/196619170165686273/embed.png "Join the Discord server")](https://discord.gg/495ab6Y)
[![Follow the project on Twitter](https://img.shields.io/twitter/follow/nCine2D.svg?style=social&label=Follow "Follow the project on Twitter")](https://twitter.com/intent/follow?screen_name=nCine2D)  
[![GitHub Stars](https://img.shields.io/github/stars/nCine/nCine.svg?style=social)](https://github.com/nCine/nCine/stargazers)
[![GitHub Watchers](https://img.shields.io/github/watchers/nCine/nCine.svg?style=social)](https://github.com/nCine/nCine/watchers)
[![GitHub Forks](https://img.shields.io/github/forks/nCine/nCine.svg?style=social)](https://github.com/nCine/nCine/network/members)
[![GitHub Issues](https://img.shields.io/github/issues/nCine/nCine?style=social)](https://github.com/nCine/nCine/issues)
[![GitHub Discussions](https://img.shields.io/github/discussions/nCine/nCine?style=social)](https://github.com/nCine/nCine/discussions)
![GitHub code size](https://img.shields.io/github/languages/code-size/nCine/nCine?style=social)  

***

## Documentation

- [Build Instructions](https://github.com/nCine/nCine/wiki/Build-Instructions)
- [Getting Started](https://github.com/nCine/nCine/wiki/Getting-Started)
- [Doxygen C++ API Documentation](https://ncine.github.io/docs/master/)
- [LDoc Lua API Documentation](https://ncine.github.io/docs/lua_master/)
- [Lua Tutorial](https://ncine.github.io/Lua_Tutorial.html)

## Technical Information

### Dependencies
- GLEW
- GLFW 3.x, SDL 2, Qt 5
- libpng, libwebp
- OpenAL-soft, libogg, libvorbis, libvorbisfile
- Lua 5.4
- Dear ImGui, Nuklear

### Supported Platforms and Compilers
- Windows (MSVC, MinGW-w64 on MSYS2)
- macOS (Apple Clang)
- Linux (GCC, Clang)
- Android (GCC, Clang)
- Emscripten

### Development Tools
- Git, CMake
- Qt Creator, Visual Studio Code
- Doxygen with GraphViz
- Valgrind, Cppcheck, clang-format
- Google Test and Gcovr, Google Benchmark
- Tracy frame profiler, RenderDoc graphics debugger
