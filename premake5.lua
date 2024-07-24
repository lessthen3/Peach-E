include "dependencies/conandeps.premake5.lua"

-- premake5.lua
workspace "Peach-E"
    configurations { "Debug", "Release" }
    architecture "x64"

project "Peach Engine"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"

    targetdir   "build/%{cfg.buildcfg}/bin"
    objdir        "build/%{cfg.buildcfg}/obj"

    location "./src"
    files { "%{prj.location}/**.h", "%{prj.location}**.cpp" }

    includedirs {
        "%{prj.location}/Peach-core/GOATS/box2d/include",
        "%{prj.location}/Peach-core/GOATS/libsodium/include",
        "%{prj.location}/Peach-core/GOATS/python/include",
        "%{prj.location}/Peach-core/GOATS/ggpo/include"
    }

    libdirs {
        "%{prj.location}/Peach-core/GOATS/box2d/lib",
        "%{prj.location}/Peach-core/GOATS/libsodium/lib",
        "%{prj.location}/Peach-core/GOATS/python/lib",
        "%{prj.location}/Peach-core/GOATS/ggpo/lib"
    }

    links {
        "box2d",
        "libsodium",
        "python312",
        "GGPO"
    }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"
    filter{ }

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"  
    filter{ }

    conan_setup()