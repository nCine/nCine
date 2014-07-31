# nCine
nCine is a multi-platform 2d game engine

### Supported platforms
- Windows
- Mac Os X
- Linux
- Android

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

### Checklist
- compiler warnings
    - GCC + address sanitizer
    - LLVM
    - MSVC
- static analysis with Cppcheck
- memory analysis with Valgrind
- deploy and test on all platforms
