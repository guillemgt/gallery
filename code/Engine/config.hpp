#ifndef engine_config_h
#define engine_config_h

#define ENGINE_ENABLE_ANGLE      1
#define ENGINE_ENABLE_VEC2       1
#define ENGINE_ENABLE_VEC3       1
#define ENGINE_ENABLE_VEC4       1
#define ENGINE_ENABLE_MAT2       1
#define ENGINE_ENABLE_MAT3       1
#define ENGINE_ENABLE_MAT4       1
#define ENGINE_ENABLE_QUATERNION 1
#define ENGINE_ENABLE_ARRAYS     1
#define ENGINE_ENABLE_S_ARRAYS   1
#define ENGINE_ENABLE_MATRICES   1
#define ENGINE_ENABLE_S_MATRICES 1
#define ENGINE_ENABLE_STRINGS    1
#define ENGINE_ENABLE_STACKS     1
#define ENGINE_ENABLE_FILES      1
#define ENGINE_ENABLE_OPENGL     1
#define ENGINE_ENABLE_FREETYPE   1
#if OS != OS_IOS
#define ENGINE_ENABLE_OPENAL     1
#endif
#define ENGINE_ENABLE_UITHING    1

#endif
