#ifndef engine_globals_h
#define engine_globals_h

#include <stdio.h>
#include <stdlib.h>

#include <math.h>
//#include <glm/glm.hpp>

#define OS_WINDOWS 1
#define OS_MAC     2
#define OS_IOS     3
#define OS_LINUX   4
#define OS_WASM    5

#define OS 0

#if defined(__EMSCRIPTEN__) || defined(EMSCRIPTEN)

#undef OS
#define OS OS_WASM

#elif defined(_WIN32)

#undef OS
#define OS OS_WINDOWS

#elif defined __APPLE__

#include <TargetConditionals.h>
#if TARGET_OS_IPHONE

#undef OS
#define OS OS_IOS

#else

#undef OS
#define OS OS_MAC

#endif

#else

#undef OS
#define OS OS_LINUX

#endif

#if OS == OS_WINDOWS

// Stop sscanf_s, sprintf_s warnings
#define _CRT_SECURE_NO_WARNINGS 1


#define fileopen(fp, fn, m) fopen_s(&fp, fn, m)

#include <stdio.h>

//#define sprintf(dst, format, ) sprintf_s
#define sscanpf sscanf_s
#define vsprintf vsprintf_s

#define makedir(path) CreateDirectory(path, NULL)

#define DEBUG_PRINTF 0
#if DEBUG_PRINTF
#define printf(...) printf_windows(__VA_ARGS__)
#endif

int __cdecl printf_windows(const char *format, ...);

#else

#define fileopen(fp, fn, m) fp = fopen(fn, m)

#include <sys/stat.h>
#include <sys/types.h>

#define makedir(path) mkdir(path, 0777)



#endif



#if OS == OS_WINDOWS
#define USE_SDL 1

#if USE_SDL
#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")
#endif
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "freetype27d.lib")
#pragma comment(lib, "OpenAL32.lib")
#endif

#include "config.hpp"

#include "structures.hpp"


void initEngine();
void cleanupEngine();

#endif
