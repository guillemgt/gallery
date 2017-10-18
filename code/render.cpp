#include "Engine/engine.hpp"

#include "render.hpp"

struct {
    GLuint id;
    GLuint position, color, normal, matrix, light_direction, directional_light_color, ambient_light_color;
} wall_program;

GLuint wall_buffer;
GLuint wall_vao;

void init_openGL(){
    // We read the wall shaders
    const char wall_shader[] =
#include "Shaders/wall_shader.glsl"
    ;
    wall_program.id = loadShadersByText(wall_shader);
    wall_program.position = glGetAttribLocation (wall_program.id, "a_position");
    wall_program.normal   = glGetAttribLocation (wall_program.id, "a_normal");
    wall_program.color    = glGetAttribLocation (wall_program.id, "a_color");
    wall_program.matrix   = glGetUniformLocation(wall_program.id, "u_matrix");
    wall_program.light_direction           = glGetUniformLocation(wall_program.id, "u_light_direction");
    wall_program.directional_light_color   = glGetUniformLocation(wall_program.id, "u_directional_light_color");
    wall_program.ambient_light_color       = glGetUniformLocation(wall_program.id, "u_ambient_light_color");
    
    // We initialize the buffers
    glGenBuffers(1, &wall_buffer);
    
    // We initialize the vertex array objects
    glGenVertexArrays(1, &wall_vao);
    glBindVertexArray(wall_vao);
    
    glEnableVertexAttribArray(wall_program.position);
    glEnableVertexAttribArray(wall_program.normal);
    glEnableVertexAttribArray(wall_program.color);
    
    glBindBuffer(GL_ARRAY_BUFFER, wall_buffer);
    glVertexAttribPointer(wall_program.position, 3, GL_FLOAT,         GL_FALSE, sizeof(Vertex_PNC), (void *)0);
    glVertexAttribPointer(wall_program.normal,   3, GL_FLOAT,         GL_FALSE, sizeof(Vertex_PNC), (void *)(sizeof(Vec3)));
    glVertexAttribPointer(wall_program.color,    3, GL_UNSIGNED_BYTE, GL_TRUE,  sizeof(Vertex_PNC), (void *)(2*sizeof(Vec3)));
    
    glBindVertexArray(0);
    
    // We define the positions, normals and colors of the triangles of the cube
    Vec3 positions[] = {
        Vec3(-1.f, -1.f, +0.f),
        Vec3(-1.f, +1.f, +0.f),
        Vec3(+1.f, -1.f, +0.f),
        
        Vec3(+1.f, -1.f, +0.f),
        Vec3(-1.f, +1.f, +0.f),
        Vec3(+1.f, +1.f, +0.f),
    };
    Vec3 normals[] = {
        Vec3(0.f, 0.f, -1.f),
        Vec3(0.f, 0.f, -1.f),
        Vec3(0.f, 0.f, -1.f),
        
        Vec3(0.f, 0.f, -1.f),
        Vec3(0.f, 0.f, -1.f),
        Vec3(0.f, 0.f, -1.f),
    };
    RgbColor colors[] = {
        {255, 255, 255},
        {255, 255, 255},
        {255, 255, 255},
        
        {255, 255, 255},
        {255, 255, 255},
        {255, 255, 255},
    };
    
    // We put everything into one big vector, interleaved
    Vertex_PNC vertices[6]; // PNC means position, normal, color
    for(int i=0; i<6; i++)
        vertices[i] = {positions[i], normals[i], colors[i]};
    
    // We put all the info in the buffer
    setVectorStaticBuffer(wall_buffer, vertices, 6);
    
}

void draw_scene(){
    glClearColor(0.4f, 0.4f, 0.4f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(wall_program.id);
    
    
    GLfloat matrix_uniform[16] = {
        0.5f,  0.f,  0.f, 0.f,
        0.f,  0.5f,  0.f, 0.f,
        0.f,   0.f, 0.5f,  0.f,
        0.f,   0.f,  0.f, 1.f
    };
    checkOpenGLError();
    glUniformMatrix4fv(wall_program.matrix, 1, GL_FALSE, matrix_uniform);
    glUniform3f(wall_program.light_direction, 0.f, 0.f, -1.f);
    glUniform3f(wall_program.directional_light_color, 0.1f, 0.1f, 0.1f);
    glUniform3f(wall_program.ambient_light_color,     0.9f, 0.9f, 0.9f);
    checkOpenGLError();
    
    glBindVertexArray(wall_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
