# Peach-E
Hey! This is a game engine that you are free to do whatever with!

Peach-E is an open-source game engine licensed under the permissive MIT License. 

Currently you can target Windows and OSX.

## Building Peach-E From Source

If you want to build Peach-E for yourself:

0. This project is built using __C++20__, and you will need __CMake 3.20+__ and __conan2__ (scroll down to the resources section for links if you are unfamiliar)

1. Clone the repo

2. Run: __python init.py --debug or --release or --both -G [desired_generator]__ in your terminal and your done!

__Note:__ Fuck you

## Conan Profile Settings Successfully Tested

```ini
[settings]
arch=x86_64
build_type=Release
compiler=msvc
compiler.cppstd=20
compiler.runtime=dynamic
compiler.version=193
os=Windows
```

```ini
​​[settings]
arch=armv8
build_type=Release
compiler=apple-clang
compiler.cppstd=20
compiler.libcxx=libc++
compiler.version=16
os=Macos
```

## Resources:

[Latest CMake Download](https://cmake.org/download/)

[Latest Conan Download](https://conan.io/downloads)