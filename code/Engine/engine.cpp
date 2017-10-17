#include "engine.hpp"
#include "opengl.hpp"
#include "freetype.hpp"
#include "sound.hpp"
#include "ui.hpp"
#include "memory.hpp"

float TIME_STEP;

void initEngine(){
    initEngineMemory();
#if ENGINE_ENABLE_OPENGL
    initEngineOpenGL();
#endif
#if ENGINE_ENABLE_FREETYPE
    initFreetype();
#endif
#if ENGINE_ENABLE_OPENAL
    initSound();
#endif
#if ENGINE_ENABLE_UITHING
    initUIThing();
#endif
}

void cleanupEngine(){
#if ENGINE_ENABLE_UITHING
    cleanupUIThing();
#endif
#if ENGINE_ENABLE_OPENAL
    cleanupSound();
#endif
#if ENGINE_ENABLE_FREETYPE
    cleanupFreetype();
#endif
#if ENGINE_ENABLE_OPENGL
    cleanupEngineOpenGL();
#endif
    cleanupEngineMemory();
}
