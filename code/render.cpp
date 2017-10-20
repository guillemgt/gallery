#include "Engine/engine.hpp"

#include "render.hpp"
#include "main.hpp"

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
        // Front face
        Vec3(-1.f, +1.f, -1.f),
        Vec3(-1.f, +1.f, +1.f),
        Vec3(+1.f, +1.f, -1.f),
        
        Vec3(+1.f, +1.f, -1.f),
        Vec3(-1.f, +1.f, +1.f),
        Vec3(+1.f, +1.f, +1.f),
        
        // Back face
        Vec3(-1.f, -1.f, -1.f),
        Vec3(-1.f, -1.f, +1.f),
        Vec3(+1.f, -1.f, -1.f),
        
        Vec3(+1.f, -1.f, -1.f),
        Vec3(-1.f, -1.f, +1.f),
        Vec3(+1.f, -1.f, +1.f),
        
        // Left face
        Vec3(-1.f, -1.f, +1.f),
        Vec3(-1.f, +1.f, -1.f),
        Vec3(-1.f, +1.f, +1.f),
        
        Vec3(-1.f, -1.f, -1.f),
        Vec3(-1.f, +1.f, -1.f),
        Vec3(-1.f, -1.f, +1.f),
        
        // Right face
        Vec3(+1.f, -1.f, +1.f),
        Vec3(+1.f, +1.f, -1.f),
        Vec3(+1.f, +1.f, +1.f),
        
        Vec3(+1.f, -1.f, -1.f),
        Vec3(+1.f, +1.f, -1.f),
        Vec3(+1.f, -1.f, +1.f),
    };
    Vec3 normals[] = {
        // Front face
        Vec3(0.f, +1.f, 0.f),
        Vec3(0.f, +1.f, 0.f),
        Vec3(0.f, +1.f, 0.f),
        
        Vec3(0.f, +1.f, 0.f),
        Vec3(0.f, +1.f, 0.f),
        Vec3(0.f, +1.f, 0.f),
        
        // Back face
        Vec3(0.f, -1.f, 0.f),
        Vec3(0.f, -1.f, 0.f),
        Vec3(0.f, -1.f, 0.f),
        
        Vec3(0.f, -1.f, 0.f),
        Vec3(0.f, -1.f, 0.f),
        Vec3(0.f, -1.f, 0.f),
        
        // Left face
        Vec3(-1.f, 0.f, 0.f),
        Vec3(-1.f, 0.f, 0.f),
        Vec3(-1.f, 0.f, 0.f),
        
        Vec3(-1.f, 0.f, 0.f),
        Vec3(-1.f, 0.f, 0.f),
        Vec3(-1.f, 0.f, 0.f),
        
        // Right face
        Vec3(+1.f, 0.f, 0.f),
        Vec3(+1.f, 0.f, 0.f),
        Vec3(+1.f, 0.f, 0.f),
        
        Vec3(+1.f, 0.f, 0.f),
        Vec3(+1.f, 0.f, 0.f),
        Vec3(+1.f, 0.f, 0.f),
    };
    RgbColor colors[] = {
        // Front face
        {255, 255, 255},
        {255, 255, 255},
        {255, 255, 255},
        
        {255, 255, 255},
        {255, 255, 255},
        {255, 255, 255},
        
        // Back face
        {255, 255, 255},
        {255, 255, 255},
        {255, 255, 255},
        
        {255, 255, 255},
        {255, 255, 255},
        {255, 255, 255},
        
        // Left face
        {255, 255, 255},
        {255, 255, 255},
        {255, 255, 255},
        
        {255, 255, 255},
        {255, 255, 255},
        {255, 255, 255},
        
        // Right face
        {255, 255, 255},
        {255, 255, 255},
        {255, 255, 255},
        
        {255, 255, 255},
        {255, 255, 255},
        {255, 255, 255},
    };
    
    // We put everything into one big vector, interleaved
    Vertex_PNC vertices[24]; // PNC means position, normal, color
    for(int i=0; i<24; i++)
        vertices[i] = {positions[i], normals[i], colors[i]};
    
    // We put all the info in the buffer
    setVectorStaticBuffer(wall_buffer, vertices, 24);
    
}

void draw_scene(){
    static float t = 0.f;
    glClearColor(0.4f, 0.4f, 0.4f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(wall_program.id);
    
    const Angle half_pov = Angle(M_PI_4);
    Mat4 perspective_matrix = get_perspective_matrix(half_pov, 0.5f, 10.5f);
    Mat4 translation_matrix = get_translation_matrix(Vec3(player.position_relative_to_coords, -2.f));
    Mat4 rotation_matrix = get_rotation_matrix_z(Angle(t));
    t += 0.03f;
    
    Mat4 final_matrix = perspective_matrix * switch_y_and_z * translation_matrix * rotation_matrix;
    
    
    checkOpenGLError();
    glUniformMatrix4fv(wall_program.matrix, 1, GL_FALSE, &final_matrix.values[0][0]);
    glUniform3f(wall_program.light_direction, 0.f, 0.f, -1.f);
    glUniform3f(wall_program.directional_light_color, 0.2f, 0.2f, 0.2f);
    glUniform3f(wall_program.ambient_light_color,     0.8f, 0.8f, 0.8f);
    checkOpenGLError();
    
    glBindVertexArray(wall_vao);
    glDrawArrays(GL_TRIANGLES, 0, 24);
}
