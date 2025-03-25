# Peach-E
Hey! This is a game engine that you are free to do whatever with!

Peach-E is an open-source and cross-platform game engine licensed under the permissive MIT License. 

Currently you can target Windows and OSX.

### PEACH-E IS STILL IN EARLY ALPHA AND IS A WORK IN PROGRESS AND STILL REQUIRES EXTENSIVE TESTING 
### WORK IS BEING DONE TO ADD FEATURES CONSTANTLY AND THE API IS SUBJECT TO BREAKING CHANGES AT ANY MOMENT

## Documentation

Docs are still a work in progress, but expect them to be here by the time Peach Engine 1.0 is released.

## Why Another Game Engine

Because I wanted to make one.

Also I'm not a huge fan of whats on the market currently. Unity was alright... but the whole TOS drama really turned me off the engine, and __I really just don't trust Unity as a company anymore.__

Unreal Engine feels like a mess to use, and I don't want to touch Blueprints. I like visual scripting, but the Unreal Engine implementation leaves a lot to be desired in my opinion. Also the lack of scripting language support is a massive L. I like C++ and that's why I'm using it to build Peach-E—but using it to write an entire game? Way overkill and unnecessary for most use cases. Scripting languages offer better development velocity in my experience.

Godot feels good to use, has great plugin support, and amazing documentation to boot. In spite of that, Godot feels unfinished. Feature development is slow, and the ones that do get added often feel inconsequential or sometimes just make the engine more awkward to use. Development doesn't seem focused on pressing issues, driven by whatever contributors feel like doing. As a result, Godot doesn't feel production ready to me. 

Godot has so many insane bugs I encountered while using it. Like how I completely broke my project by trying to edit it without realizing I had a game instance running, and now I get errors everytime I try to run my Godot project.

The engine still has memory leaks and weird jank in core systems, and they just keep releasing new versions without a single stable, truly bug-free milestone. To say Godot is out of beta is wishful thinking.

Bitching aside—yeah, all these engines contributed in one way or another to the creation of Peach-E. However Godot was the straw that broke the camel's back grumble grumble. 

My goal with Peach-E is simple:

🍑🍑🍑 Make a game engine that doesn’t feel awful to use 🍑🍑🍑

💥💥💥 Doesn’t explode when you breathe on it wrong 💥💥💥

That's it. Plain and simple.

## Building Peach-E From Source

If you want to build Peach-E for yourself:

0. This project is built using __C++20__, and you will need __CMake 3.20+__ and __conan2__ (scroll down to the resources section for links if you are unfamiliar)

1. Clone the repo

2. Run: __python init.py [--debug or --release or --both] -G [desired_generator]__ in your terminal and your done!

__Note:__ The build output will be generated in __/build/(Debug or Release)__ as an executable for Peach_Editor and Peach_Engine and a static lib for Peach_Core

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