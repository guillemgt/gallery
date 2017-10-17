#ifndef engine_freetype_h
#define engine_freetype_h

#include "opengl.hpp"

int initFreetype();
int cleanupFreetype();
int loadFont(char *path);

#ifdef TA_DEVMODE
void _cleanScreenLog();
void _screenLog(const char *ftext, ...);
#define cleanScreenLog(...) _cleanScreenLog(__VA_ARGS__)
#define print(...) _screenLog(__VA_ARGS__)
#else
#define cleanScreenLog(...)
#define print(...)
#endif

void renderText(float x, float y, float size, char *printLog);
unsigned int renderText(float x, float y, uint8 size, String &s, Vec3 *pos, RgbaColor *col, Vec2* coords, float *width, float *height);
unsigned int renderText(float x, float y, uint8 size, String &s, Vec3 *pos, RgbaColor *col, Vec2* coords, float *width, float *height, RgbaColor color);
float renderTextLength(uint8 size, String &s);
float renderTextLength(uint8 size, String &s, int length);
int textVertNum(char *printLog);
int textVertNum(String &s);

#define FONT_SIZES 4
#define FONT_SIZE_12 0
#define FONT_SIZE_16 1
#define FONT_SIZE_32 2
#define FONT_SIZE_64 3

#endif /* defined(__ta__files__) */
