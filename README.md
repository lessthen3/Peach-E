# Peach-E
 Peach Game Engine
 
Hey! This is a game engine that you are free to do whatever with!

Peach-E is an open-source game engine that currently is functional for 2D only. 

Currently you can target Windows, Linux, and MacOS.



If you want to build Peach-E for yourself (assuming you have the appropriate cpp runtime tools):

0. You will need CMake 3.20+ and conan2 (scroll down to the resources section for links if you are unfamiliar)

1. Clone the repo

2. Run: python init.py and your done!

Note: here is the conan profile I am using for building Peach-E,

### Conan Profile Settings

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

Resources:

[Latest CMake Download](https://cmake.org/download/)

[Latest Conan Download](https://conan.io/downloads)