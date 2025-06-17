# Peach-E
Peach-E is a modern, cross-platform, open source 2D/3D game engine built with C++20 and licensed under the permissive MIT License. 

Peach-E is—and always will be—100% free. No stupid fucking licensing garbage, no royalties, no hidden fees. Built by a game developer, for game developers of any skill level.

Currently you can target Windows(x86_64), MacOS(ARM64) and Linux(x86_64). Support for FreeBSD, Haiku, Android, iOS, iPadOS(w/ apple pencil), tvOS, watchOS, Web, and PS Vita is in the works. Peach-E may work on other POSIX compliant systems, but your mileage may vary.

>[!WARNING]
>Peach-E is still in early alpha!
>Features are being added constantly, and the API can (and will) break as we sprint toward a complete 1.0 release.

## Documentation

Docs are coming! You’ll find them here by release 1.0

## Overall Design and Features

Peach-E uses a component-based, node-driven approach, heavily inspired by Godot—but with a lot of my own tweaks and improvements where Godot falls short in my opinion.

Features:

🍑 __Scripting support for C#, Python, Lua as well as C++ if that's really how you roll__ _(WIP)_

🍑 __Bundled editor__ so you can hit the ground running _(WIP)_

🍑 __Vulkan, OpenGL, OpenGL ES, and WebGL__ backends _(support varies depending on which platform is being targetted)_

🍑 __Visual scripting + shader graph__ _(WIP)_

🍑 __Plugin support__—C++ works; scripted plugins are coming

🍑 __Built-in rollback networking__ _(WIP)_

🍑 __Audio playback + capture__ _(WIP)_

🍑 __Physics:__ Box2D (v3.1.1) for 2D, Jolt (v5.3.0) for 3D

🍑 __Native support for Windows, MacOS, and Linux__ _(FreeBSD, Haiku, iOS, iPadOS, tvOS, Android, PS Vita, and web are a WIP)_

🍑 __Quality of life nodes__ for easily integrating in-game consoles and logging support out of the box

## Building Peach-E From Source

If you want to build Peach-E for yourself:

0. This project is built using __C++20__, and you will need __CMake 3.20+__ (scroll down to the resources section for links if you are unfamiliar)

1. Clone the repo

2. Run: __python init.py [--debug | --release | --both] -G [desired_generator]__ in your terminal and your done!

>[!TIP]
>For the complete list of generators and commands run __python init.py [-h | --help]__

> [!INFO]
>Build output:
-Peach_Editor (executable or bundle on MacOS)
-Peach_Engine (executable or bundle on MacOS)
-peach (static core library)
-peach_api (dynamic lib for API linking w/ C#)

## Why Another Game Engine

Because I wanted to make one... also none of the existing ones hit the sweet spot for me.

Unity was alright... but the whole TOS drama really turned me off the engine, and __I really just don't trust Unity as a company anymore.__

Unreal Engine feels like a mess to use, and I don't want to touch Blueprints. I like visual scripting, but the Unreal Engine implementation leaves a lot to be desired in my opinion. Also the lack of official scripting language support is a massive L. I like C++ and that's why I'm using it to build Peach-E—but using it to write an entire game? Way overkill and unnecessary for most use cases. Scripting languages offer better development velocity and experience(ily lua).

Godot feels good to use, has great plugin support, and amazing documentation to boot. In spite of that, Godot feels unfinished. Feature development is slow, and the ones that do get added often feel inconsequential or sometimes just make the engine more awkward to use. Development doesn't seem focused on pressing issues, driven by whatever contributors feel like doing. As a result, Godot still has weird jank in core systems, and they just keep releasing new versions without a single definitive design for the engine.

Bitching aside—yeah, all three inspired Peach-E, but Godot’s “almost there” feeling finally made me snap. 

__*I just want...*__

🍑🍑🍑 An engine that doesn’t feel awful to use 🍑🍑🍑

💥💥💥 Doesn’t explode when you breathe on it wrong 💥💥💥

That's it. Plain and simple.

#### READY TO BUILD SOME GAMES? WELCOME ABOARD THE PEACH ENGINE!

## Platforms Successfully Tested

```ini
os=Windows
arch=x86_64
compiler=msvc
compiler.runtime=dynamic
compiler.version=193
```

```ini
os=Macos
arch=armv8
compiler=apple-clang
compiler.version=16
```

```ini
os=Linux
arch=x86_64
compiler=clang
compiler.version=19
```

## Resources:

[Latest CMake Download](https://cmake.org/download/)
