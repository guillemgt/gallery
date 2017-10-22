#ifndef engine_opengl_h
#define engine_opengl_h

#define OPENGL_OLD 1

#if OS == OS_WINDOWS

#define OPENGL_ES 0
#include <Windows.h>
#include <GL/glew.h>
#include <GL/gl.h>

#elif OS == OS_IOS

#define OPENGL_ES 1
#include <OpenGLES/EAGL.h>
#include <OpenGLES/ES3/gl.h>

#elif OS == OS_WASM

#define OPENGL_ES 1
#include <GL/glew.h>
//#include <GLES2/gl2.h>
#include <SDL/SDL.h> // ??

#else

#define OPENGL_ES 0
#include <OpenGL/gl.h>
#if !OPENGL_OLD
#include <OpenGL/gl3.h>
#endif

#endif

#if OS == OS_MAC && OPENGL_OLD
#define glGenVertexArrays    glGenVertexArraysAPPLE
#define glBindVertexArray    glBindVertexArrayAPPLE
#define glDeleteVertexArrays glDeleteVertexArraysAPPLE
#endif

#define checkOpenGLError() _checkOpenGLError(__LINE__, __FILE__)

struct GlBuffer{
    GLuint positions, colors, coords;
    unsigned int verts;
};

inline void initPositionsColors(GlBuffer *t){
    glGenBuffers(1, &t->positions);
    glGenBuffers(1, &t->colors);
    t->verts = 0;
}inline void cleanupPositionsColors(GlBuffer *t){
    glDeleteBuffers(1, &t->positions);
    glDeleteBuffers(1, &t->colors);
    t->verts = 0;
}

inline void initPositionsCoords(GlBuffer *t){
    glGenBuffers(1, &t->positions);
    glGenBuffers(1, &t->coords);
    t->verts = 0;
}
inline void cleanupPositionsCoords(GlBuffer *t){
    glDeleteBuffers(1, &t->positions);
    glDeleteBuffers(1, &t->coords);
    t->verts = 0;
}

inline void initPositionsColorsCoords(GlBuffer *t){
    glGenBuffers(1, &t->positions);
    glGenBuffers(1, &t->colors);
    glGenBuffers(1, &t->coords);
    t->verts = 0;
}
inline void cleanupPositionsColorsCoords(GlBuffer *t){
    glDeleteBuffers(1, &t->positions);
    glDeleteBuffers(1, &t->colors);
    glDeleteBuffers(1, &t->coords);
    t->verts = 0;
}

bool initEngineOpenGL();
bool cleanupEngineOpenGL();

struct Vertex_PC {
    Vec3 p;
    RgbColor c;
};
struct Vertex_PT {
    Vec3 p;
    Vec2 t;
};
struct Vertex_PNC {
    Vec3 p, n;
    RgbColor c;
};
struct Vertex_PNT {
    Vec3 p, n;
    Vec2 t;
};


void setVectorStaticBuffer(GLuint buffer, GLfloat *vert, unsigned int size);
void setVectorDynamicBuffer(GLuint buffer, GLfloat *vert, unsigned int size);
void setVectorStaticBuffer(GLuint buffer, GLubyte *vert, unsigned int size);
void setVectorDynamicBuffer(GLuint buffer, GLubyte *vert, unsigned int size);
void setVectorDynamicBuffer(GLuint buffer, Vertex_PC *vert, unsigned int size);
void setVectorStreamBuffer(GLuint buffer, Vertex_PC *vert, unsigned int size);
void setVectorDynamicBuffer(GLuint buffer, Vertex_PT *vert, unsigned int size);
void setVectorStaticBuffer(GLuint buffer, Vertex_PNC *vert, unsigned int size);
void setVectorStaticBuffer(GLuint buffer, Vertex_PNT *vert, unsigned int size);

void _checkOpenGLError(int line, const char *file);

GLuint loadShaders(const char *file_path);
GLuint loadShaders(const char *file_path, const char *prefix);
GLuint loadShadersByText(const char *text);

void loadTexture(GLuint texture, const char *path);

#endif /* defined(__topo__render__) */
