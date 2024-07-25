# Peach-E
 Peach Game Engine
 
Hey! This is a game engine that you are free to do whatever with!

Peach-E is an open-source game engine that currently is functional for 2D only. 

Currently you can target Windows, Linux, and MacOS.



If you want to build Peach-E for yourself:

0. This project is built using __C++20__, and you will need __CMake 3.20+__ and __conan2__ (scroll down to the resources section for links if you are unfamiliar)

1. Clone the repo

2. Run: __python init.py__ in your terminal and your done!

__Note:__ You'll have to change some directory specific code in PeachEngineManager. Particularly, SetupLogManagers() and SetupInternalLogManagers(). Whatever directory you wish to use will be auto-populated with relevant log files.

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

### Resources:

[Latest CMake Download](https://cmake.org/download/)

[Latest Conan Download](https://conan.io/downloads)