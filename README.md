# Peach-E
Peach-E is a modern, cross-platform, open source 2D/3D game engine built with C++20 and licensed under the permissive MIT License. 

Peach-E is and __will always be 100% free__. No stupid licensing garbage, no royalties, no hidden fees. Built by a game developer, for game developers of any skill level.

Currently you can target Windows(x64), MacOS(ARM64) and Linux(x64). Native support for FreeBSD, Haiku, Android, iOS, iPadOS(w/ apple pencil), tvOS, watchOS, Web, Windows ARM and PS Vita are in the works. Peach-E may work on other POSIX compliant systems, but your mileage may vary.

Peach-E can be used headless via the peach_api static lib, the api uses a C calling convention so linking against C, C++ or rust is supported.

>[!WARNING]
>Peach-E is still in early alpha!
>Features are being added constantly, and the API can (and will) break as I work towards a complete 1.0 release.

## Documentation

Docs are coming! You’ll find them [here](https://www.youtube.com/watch?v=dQw4w9WgXcQ) fully complete by release 1.0

## Overall Design and Features

Peach-E uses a component-based, node-driven approach, heavily inspired by Godot—but with a lot of my own tweaks and improvements where Godot falls short in my opinion.

Features:

🍑 __Scripting support for C#, Python, Lua__ _(WIP)_

🍑 __Bundled editor__ so you can hit the ground running _(WIP)_

🍑 __Vulkan, OpenGL, OpenGL ES, and WebGL__ backends _(support varies depending on which platform is being targetted)_

🍑 __Visual scripting + shader graph__ _(WIP)_

🍑 __Plugin support__ C++ works; scripted plugins are coming

🍑 __Built-in rollback and/or delay based UDP/TCP networking__ _(WIP)_

🍑 __Audio playback + capture__ _(WIP)_

🍑 __Physics:__ Box2D (v3.1.1) for 2D, Jolt (v5.3.0) for 3D

🍑 __Native support for Windows(x64), MacOS, and Linux__ _(Windows ARM, FreeBSD, Haiku, iOS, iPadOS, tvOS, Android, PS Vita, and web are a WIP)_

🍑 __Quality of life nodes__ for easily integrating in-game consoles and logging support out of the box

## Building Peach-E From Source

If you want to build Peach-E for yourself:

0. This project is built using __C++20__, and you will need __CMake 3.20+__ 

1. Clone the repo

2. Run: __python init.py [--debug | --release | --both] -G [desired_generator]__ in your terminal and your done!

>[!TIP]
>For the complete list of generators and commands run __python init.py [-h | --help]__

> [!NOTE]
>Build output:
> * Peach_Editor (executable or bundle on MacOS)
> * Peach_Engine (executable or bundle on MacOS)
> * peach (static core library)
> * peach_api (dynamic lib for API linking w/ C#)

## Why Another Game Engine

Because I wanted to make one... also I don't like any of the current options:

Fuck Unity.

Unreal Engine is powerful but feels awful to use and only supporting C++ in my opinion is stupid.

Godot feels good to use, although in my experience pretty buggy.

LWJGL is nice, but who wants to program a game in Java.

what is O3DE.

Bevy is technically a game engine, but like where's the editor?_?

Bitching aside—all of my experiences with game engines and frameworks inspired Peach-E.

__*I just want an engine...*__

🍑🍑🍑 that doesn’t feel awful to use 🍑🍑🍑

💥💥💥 that doesn’t explode 💥💥💥

That's it. Plain and simple.

#### READY TO BUILD SOME GAMES? WELCOME ABOARD THE PEACH ENGINE!

## Platforms Successfully Tested

```ini
os=Windows
arch=x86_64
compiler=msvc
compiler.runtime=static
compiler.version=193
```

```ini
os=Macos
arch=armv8
compiler=apple-clang
compiler.version=16-17
```

```ini
os=Linux
arch=x86_64
compiler=clang
compiler.version=19
```