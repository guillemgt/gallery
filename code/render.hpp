#ifndef render_hpp
#define render_hpp

#include "Engine/engine.hpp"
#include "Engine/opengl.hpp"

struct BufferAndCount {
    GLuint buffer;
    u32 count;
};

void init_openGL();
void draw_scene();

#endif /* render_hpp */
