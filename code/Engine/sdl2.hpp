#ifndef engine_sdl2_h
#define engine_sdl2_h

#include <stdio.h>
#include "engine.hpp"

#if OS != OS_WASM
#include <SDL2/SDL.h>
#else
#include <SDL/SDL.h>
#endif

#include "window.hpp"

#endif /* sdl2_hpp */
