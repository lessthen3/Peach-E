# Peach-E
Hey! This is a game engine that you are free to do whatever with!

Peach-E is an open-source and cross-platform game engine built with modern C++ 20 licensed under the permissive MIT License. 

Currently you can target Windows, macOS and Linux. Peach-E may work on UNIX systems, but support isn't guaranteed.

>[!WARNING]
>Peach-E is still in early alpha and extensive testing is still required. Work is being done to add features constantly, and the API is subject to breaking changes at any moment while work is being done to get Peach-E to a complete 1.0 release.

## Documentation

Docs are still a work in progress, but expect them to be here by the time Peach Engine 1.0 is released.

## Overall Design and Features

Peach-E is designed around the PeachCore library and utilizes a component based approach to building games with nodes. The API design is heavily inspired by how Godot works, but the implementation is entirely original—no Godot code is used.

Features:

🍑 Vulkan and OpenGL rendering backends

🍑 Scripting support for C#, Python, and Lua

🍑 Planned support for visual scripting and a ShaderGraph

🍑 Built-in rollback networking

🍑 Audio playback and capture

🍑 Physics support via Box2D (v3.0) for 2D and Jolt (v5.2.0) for 3D

🍑 Native support for Windows, macOS, and Linux (web and mobile support planned)

🍑 External C++ plugin support via dynamic linking (scripted plugin support WIP)

🍑 QOL nodes for easily integrating in-game consoles and logging support out of the box

## Building Peach-E From Source

If you want to build Peach-E for yourself:

0. This project is built using __C++20__, and you will need __CMake 3.20+__ and __conan2__ (scroll down to the resources section for links if you are unfamiliar)

1. Clone the repo

2. Run: __python init.py [--debug or --release or --both] -G [desired_generator] -P [conan_profile]__ in your terminal and your done!
>[!TIP]
>For the complete list of generators run __python init.py [-h or --help]__. Also -P isn't required, if no profile is specified init.py will use the default profile

> [!NOTE]
>The build output will be generated in __/build/(Debug or Release)__ as an executable for Peach_Editor and Peach_Engine and a static lib for Peach_Core

## Why Another Game Engine

Because I wanted to make one.

Also I'm not a huge fan of whats on the market currently. Unity was alright... but the whole TOS drama really turned me off the engine, and __I really just don't trust Unity as a company anymore.__

Unreal Engine feels like a mess to use, and I don't want to touch Blueprints. I like visual scripting, but the Unreal Engine implementation leaves a lot to be desired in my opinion. Also the lack of scripting language support is a massive L. I like C++ and that's why I'm using it to build Peach-E—but using it to write an entire game? Way overkill and unnecessary for most use cases. Scripting languages offer better development velocity in my experience.

Godot feels good to use, has great plugin support, and amazing documentation to boot. In spite of that, Godot feels unfinished. Feature development is slow, and the ones that do get added often feel inconsequential or sometimes just make the engine more awkward to use. Development doesn't seem focused on pressing issues, driven by whatever contributors feel like doing. As a result, Godot doesn't feel production ready to me. 

Furthermore, Godot still has memory leaks and weird jank in core systems, and they just keep releasing new versions without a single stable, truly bug-free milestone.

Bitching aside—yeah, all these engines contributed in one way or another to the creation of Peach-E. However Godot was the straw that broke the camel's back grumble grumble. 

My goal with Peach-E is simple:

🍑🍑🍑 Make a game engine that doesn’t feel awful to use 🍑🍑🍑

💥💥💥 Doesn’t explode when you breathe on it wrong 💥💥💥

That's it. Plain and simple.

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
[settings]
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