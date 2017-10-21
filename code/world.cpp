#include "world.hpp"
#include "bigint.hpp"
#include "render.hpp"

PictureCoords _loaded_pictures_x[3];
PictureCoords _loaded_pictures_y[3];

void load_world(){
    
}


const RgbColor wall_color = {200, 200, 200};
const RgbColor floor_color = {255, 255, 255};
void load_walls_into_buffer(BufferAndCount *buffer){
    Vertex_PNC o_vertices[total_loaded_squares*total_loaded_squares*24 + 12];
    Vertex_PNC *vertices = o_vertices;
    float fx = -loaded_squares_radius_float*square_size;
    for(int x=-loaded_squares_radius_int; x<=loaded_squares_radius_int; x++){
        fx += square_size;
        float fy = -loaded_squares_radius_float*square_size;
        for(int y=-loaded_squares_radius_int; y<=loaded_squares_radius_int; y++){
            fy += square_size;
            
            // Front face
            *(vertices++) = { Vec3(fx+      0.f, fy+wall_size,         0.f), Vec3(0.f, +1.f, 0.f), wall_color };
            *(vertices++) = { Vec3(fx+      0.f, fy+wall_size, wall_height), Vec3(0.f, +1.f, 0.f), wall_color };
            *(vertices++) = { Vec3(fx+wall_size, fy+wall_size,         0.f), Vec3(0.f, +1.f, 0.f), wall_color };
            
            *(vertices++) = { Vec3(fx+wall_size, fy+wall_size,         0.f), Vec3(0.f, +1.f, 0.f), wall_color };
            *(vertices++) = { Vec3(fx+      0.f, fy+wall_size, wall_height), Vec3(0.f, +1.f, 0.f), wall_color };
            *(vertices++) = { Vec3(fx+wall_size, fy+wall_size, wall_height), Vec3(0.f, +1.f, 0.f), wall_color };
            
            // Back face
            *(vertices++) = { Vec3(fx+      0.f, fy+      0.f,         0.f), Vec3(0.f, +1.f, 0.f), wall_color };
            *(vertices++) = { Vec3(fx+      0.f, fy+      0.f, wall_height), Vec3(0.f, +1.f, 0.f), wall_color };
            *(vertices++) = { Vec3(fx+wall_size, fy+      0.f,         0.f), Vec3(0.f, +1.f, 0.f), wall_color };
            
            *(vertices++) = { Vec3(fx+wall_size, fy+      0.f,         0.f), Vec3(0.f, +1.f, 0.f), wall_color };
            *(vertices++) = { Vec3(fx+      0.f, fy+      0.f, wall_height), Vec3(0.f, +1.f, 0.f), wall_color };
            *(vertices++) = { Vec3(fx+wall_size, fy+      0.f, wall_height), Vec3(0.f, +1.f, 0.f), wall_color };
            
            // Right face
            *(vertices++) = { Vec3(fx+wall_size, fy+      0.f,         0.f), Vec3(+1.f, 0.f, 0.f), wall_color };
            *(vertices++) = { Vec3(fx+wall_size, fy+      0.f, wall_height), Vec3(+1.f, 0.f, 0.f), wall_color };
            *(vertices++) = { Vec3(fx+wall_size, fy+wall_size,         0.f), Vec3(+1.f, 0.f, 0.f), wall_color };
            
            *(vertices++) = { Vec3(fx+wall_size, fy+wall_size,         0.f), Vec3(+1.f, 0.f, 0.f), wall_color };
            *(vertices++) = { Vec3(fx+wall_size, fy+      0.f, wall_height), Vec3(+1.f, 0.f, 0.f), wall_color };
            *(vertices++) = { Vec3(fx+wall_size, fy+wall_size, wall_height), Vec3(+1.f, 0.f, 0.f), wall_color };
            
            // Left face
            *(vertices++) = { Vec3(fx+      0.f, fy+      0.f,         0.f), Vec3(-1.f, 0.f, 0.f), wall_color };
            *(vertices++) = { Vec3(fx+      0.f, fy+      0.f, wall_height), Vec3(-1.f, 0.f, 0.f), wall_color };
            *(vertices++) = { Vec3(fx+      0.f, fy+wall_size,         0.f), Vec3(-1.f, 0.f, 0.f), wall_color };
            
            *(vertices++) = { Vec3(fx+      0.f, fy+wall_size,         0.f), Vec3(-1.f, 0.f, 0.f), wall_color };
            *(vertices++) = { Vec3(fx+      0.f, fy+      0.f, wall_height), Vec3(-1.f, 0.f, 0.f), wall_color };
            *(vertices++) = { Vec3(fx+      0.f, fy+wall_size, wall_height), Vec3(-1.f, 0.f, 0.f), wall_color };
        }
    }
    
    // Floor
    *(vertices++) = { Vec3(-loaded_squares_radius_float*square_size, -loaded_squares_radius_float*square_size, 0.f), Vec3(0.f, 0.f, 1.f), floor_color };
    *(vertices++) = { Vec3(-loaded_squares_radius_float*square_size, +loaded_squares_radius_float*square_size, 0.f), Vec3(0.f, 0.f, 1.f), floor_color };
    *(vertices++) = { Vec3(+loaded_squares_radius_float*square_size, -loaded_squares_radius_float*square_size, 0.f), Vec3(0.f, 0.f, 1.f), floor_color };
    
    *(vertices++) = { Vec3(+loaded_squares_radius_float*square_size, -loaded_squares_radius_float*square_size, 0.f), Vec3(0.f, 0.f, 1.f), floor_color };
    *(vertices++) = { Vec3(-loaded_squares_radius_float*square_size, +loaded_squares_radius_float*square_size, 0.f), Vec3(0.f, 0.f, 1.f), floor_color };
    *(vertices++) = { Vec3(+loaded_squares_radius_float*square_size, +loaded_squares_radius_float*square_size, 0.f), Vec3(0.f, 0.f, 1.f), floor_color };
    
    // Ceiling
    *(vertices++) = { Vec3(-loaded_squares_radius_float*square_size, -loaded_squares_radius_float*square_size, wall_height), Vec3(0.f, 0.f, -1.f), floor_color };
    *(vertices++) = { Vec3(-loaded_squares_radius_float*square_size, +loaded_squares_radius_float*square_size, wall_height), Vec3(0.f, 0.f, -1.f), floor_color };
    *(vertices++) = { Vec3(+loaded_squares_radius_float*square_size, -loaded_squares_radius_float*square_size, wall_height), Vec3(0.f, 0.f, -1.f), floor_color };
    
    *(vertices++) = { Vec3(+loaded_squares_radius_float*square_size, -loaded_squares_radius_float*square_size, wall_height), Vec3(0.f, 0.f, -1.f), floor_color };
    *(vertices++) = { Vec3(-loaded_squares_radius_float*square_size, +loaded_squares_radius_float*square_size, wall_height), Vec3(0.f, 0.f, -1.f), floor_color };
    *(vertices++) = { Vec3(+loaded_squares_radius_float*square_size, +loaded_squares_radius_float*square_size, wall_height), Vec3(0.f, 0.f, -1.f), floor_color };
    
    setVectorStaticBuffer(buffer->buffer, o_vertices, total_loaded_squares*total_loaded_squares*24 + 12);
    buffer->count = total_loaded_squares*total_loaded_squares*24 + 12;
}

void load_pictures_into_texture(){
    for(int x=0; x<3; x++){
        for(int y=0; y<3; y++){
            
        }
    }
}
