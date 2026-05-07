package com.starlightbrew.peach;

import org.libsdl.app.SDLActivity;

public class PeachActivity extends SDLActivity
{
    // By default SDLActivity tries to load "libmain.so" AND "libSDL3.so".
    // We statically link SDL3 into peach_core, so override both.
    
    @Override
    protected String[] getLibraries()
    {
        return new String[] { "peach_core" }; // just the one .so, SDL3 is baked in
    }
    
}