# nCine
nCine is a multi-platform 2d game engine

## Information

### Features
- OpenGL / OpenGL ES renderer
- Custom array and list containers with iterators
- Thread pool
- Streaming of Vorbis Audio
- Android assets
- Joystick support
- SDL 1.2 / GLFW 3 support
- Font rendering (AngelCode's BMFont format)
- Particle system

### Supported platforms
- Windows (MSVC, MinGW)
- Mac Os X (LLVM)
- Linux (GCC, LLVM)
- Android (GCC)

### Development Tools
- CMake
- Qt Creator
- Doxygen with GraphViz
- Valgrind
- Cppcheck
- Mercurial first, then git

### Libraries
- GLEW
- GLFW 3
- SDL 1.2
- SDL_image
- libwebp
- libpng
- OpenAL
- libogg, libvorbis, libvorbisfile

## Notes

### Cppcheck
cppcheck --quiet --enable=all --inconclusive --std=c++03 -I include/ src/ 2> cppcheck.log

### ADB
adb push libs/armeabi/libncine.so /data/data/com.encelo.ncine/lib/
adb pull /sdcard/ncine_log.txt

### MinGW32
cmake -DCROSS_MINGW32=1 nCine-git

### bitbucket.org
git push https://bitbucket.org/encelo/ncine

## Developing

### Checklist
- compiler warnings
  - GCC + address sanitizer
  - LLVM
  - MSVC
- static analysis with Cppcheck
- memory analysis with Valgrind
- deploy and test on all platforms
  - check both GLFW 3 and SDL 1.2 support
