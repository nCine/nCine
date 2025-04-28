<p align="center"><a href="https://ncine.github.io" target="_blank" rel="noopener noreferrer"><img width="192" src="https://github.com/nCine/nCine-data/blob/master/icons/icon192.png" alt="nCine logo"></a></p>

# nCine

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

[News](https://ncine.github.io/news/) •
[Features](https://ncine.github.io/features/) •
[Gallery](https://ncine.github.io/gallery/) •
[Videos](https://ncine.github.io/videos/) •
[Getting Started](https://github.com/nCine/nCine/wiki/Getting-Started) •
[Discord](https://discord.gg/495ab6Y) •
[Donate](https://ncine.github.io/donate/)

## Overview

nCine is a cross-platform 2D game engine with an emphasis on performance. It is written in C++11 but it can optionally be scripted with Lua.  
It is released under the MIT License and it has been in active development since June 2011.

For additional information: <https://ncine.github.io/>.

Copyright (c) 2011-2025 Angelo Theodorou.

## Screenshots

<table>
  <tr>
    <td align="center" width="50%">
      <figure>
        <img src="https://ncine.github.io/img/gallery/SpookyGhost.png" alt="SpookyGhost" style="max-width:100%; height:auto;">
        <figcaption><a href="https://encelo.itch.io/spookyghost">SpookyGhost</a>, a procedural sprite animation tool by <a href="https://github.com/encelo">Encelo</a></figcaption>
      </figure>
    </td>
    <td align="center" width="50%">
      <figure>
        <img src="https://ncine.github.io/img/gallery/ncJump.png" alt="ncJump" style="max-width:100%; height:auto;">
        <figcaption><a href="https://github.com/Fahien/ncJump">ncJump</a>, a platform game with Box2D physics and an ImGui editor by <a href="https://github.com/Fahien">Fahien</a></figcaption>
      </figure>
    </td>
  </tr>
  <tr>
    <td align="center" width="50%">
      <figure>
        <img src="https://ncine.github.io/img/gallery/jazz2.png" alt="Jazz² Resurrection" style="max-width:100%; height:auto;">
        <figcaption><a href="https://github.com/deathkiller/jazz2-native">Jazz² Resurrection</a>, the C++ version of the Jazz Jackrabbit 2 reimplementation project by <a href="https://github.com/deathkiller">DeathKiller</a></figcaption>
      </figure>
    </td>
    <td align="center" width="50%">
      <figure>
        <img src="https://ncine.github.io/img/gallery/ncJugiMapFrameworkDemo.png" alt="ncJugiMapFrameworkDemo" style="max-width:100%; height:auto;">
        <figcaption><a href="https://github.com/Jugilus/JugimapFramework">ncJugiMapFrameworkDemo</a>, a demo test for <a href="http://jugimap.com/">JugiMap</a> integration framework by <a href="https://github.com/Jugilus">Jugilus</a></figcaption>
      </figure>
    </td>
  </tr>
</table>

## Documentation

- [Build Instructions](https://github.com/nCine/nCine/wiki/Build-Instructions)
- [Getting Started](https://github.com/nCine/nCine/wiki/Getting-Started)
- [C++ API Documentation](https://ncine.github.io/docs/master/) (Doxygen)
- [Lua API Documentation](https://ncine.github.io/docs/lua_master/) (LDoc)
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
