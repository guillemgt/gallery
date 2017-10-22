#include "Engine/os.hpp"

#include "world.hpp"
#include "bigint.hpp"
#include "render.hpp"

PictureCoords _loaded_pictures_x[3];
PictureCoords _loaded_pictures_y[3];

#define MAX_MODEL_TRIANGLES 104
StaticArray<Vertex_PNC[3], MAX_MODEL_TRIANGLES> wall_model;

void load_world(){
    // We load the wall model
    OsFile fp = openGameFile("walls.model", "rb");
    if(fp != nullptr){
        readFile(&wall_model.size, sizeof(u32), 1, fp);
        assert(wall_model.size <= MAX_MODEL_TRIANGLES);
        
        Vec3     positions[3*MAX_MODEL_TRIANGLES];
        Vec3     normals[MAX_MODEL_TRIANGLES];
        RgbColor colors[MAX_MODEL_TRIANGLES];
        readFile(positions, sizeof(Vec3), 3*wall_model.size, fp);
        readFile(colors, sizeof(RgbColor), wall_model.size, fp);
        readFile(normals, sizeof(Vec3), wall_model.size, fp);
        closeFile(fp);
        
        for(int i=0, j=0; i<wall_model.size; i++){
            wall_model[i][0] = {positions[j++], normals[i], colors[i]};
            wall_model[i][1] = {positions[j++], normals[i], colors[i]};
            wall_model[i][2] = {positions[j++], normals[i], colors[i]};
        }
    }else{
        printf("Couldn't load model 'walls.model'\n");
    }
}


const RgbColor floor_color = {255, 255, 255};
void load_walls_into_buffer(BufferAndCount *buffer){
    int vert_num = total_loaded_squares*total_loaded_squares*3*wall_model.size + 12;
    Vertex_PNC o_vertices[vert_num];
    Vertex_PNC *vertices = o_vertices;
    float fx = -loaded_squares_radius_float*square_size;
    for(int x=-loaded_squares_radius_int; x<=loaded_squares_radius_int; x++){
        fx += square_size;
        float fy = -loaded_squares_radius_float*square_size;
        for(int y=-loaded_squares_radius_int; y<=loaded_squares_radius_int; y++){
            fy += square_size;
            Vec3 p = Vec3(fx, fy, 0.f);
            
            for(int i=0; i<wall_model.size; i++){
                *(vertices++) = {p+wall_model[i][0].p, wall_model[i][0].n, wall_model[i][0].c};
                *(vertices++) = {p+wall_model[i][1].p, wall_model[i][1].n, wall_model[i][1].c};
                *(vertices++) = {p+wall_model[i][2].p, wall_model[i][2].n, wall_model[i][2].c};
            }
        }
    }
    
    const f32 loaded_squares_radius_float_plus = loaded_squares_radius_float+1.f;
    // Floor
    *(vertices++) = { Vec3(-loaded_squares_radius_float_plus*square_size, -loaded_squares_radius_float_plus*square_size, 0.f), Vec3(0.f, 0.f, 1.f), floor_color };
    *(vertices++) = { Vec3(-loaded_squares_radius_float_plus*square_size, +loaded_squares_radius_float_plus*square_size, 0.f), Vec3(0.f, 0.f, 1.f), floor_color };
    *(vertices++) = { Vec3(+loaded_squares_radius_float_plus*square_size, -loaded_squares_radius_float_plus*square_size, 0.f), Vec3(0.f, 0.f, 1.f), floor_color };
    
    *(vertices++) = { Vec3(+loaded_squares_radius_float_plus*square_size, -loaded_squares_radius_float_plus*square_size, 0.f), Vec3(0.f, 0.f, 1.f), floor_color };
    *(vertices++) = { Vec3(-loaded_squares_radius_float_plus*square_size, +loaded_squares_radius_float_plus*square_size, 0.f), Vec3(0.f, 0.f, 1.f), floor_color };
    *(vertices++) = { Vec3(+loaded_squares_radius_float_plus*square_size, +loaded_squares_radius_float_plus*square_size, 0.f), Vec3(0.f, 0.f, 1.f), floor_color };
    
    // Ceiling
    *(vertices++) = { Vec3(-loaded_squares_radius_float_plus*square_size, -loaded_squares_radius_float_plus*square_size, wall_height), Vec3(0.f, 0.f, -1.f), floor_color };
    *(vertices++) = { Vec3(-loaded_squares_radius_float_plus*square_size, +loaded_squares_radius_float_plus*square_size, wall_height), Vec3(0.f, 0.f, -1.f), floor_color };
    *(vertices++) = { Vec3(+loaded_squares_radius_float_plus*square_size, -loaded_squares_radius_float_plus*square_size, wall_height), Vec3(0.f, 0.f, -1.f), floor_color };
    
    *(vertices++) = { Vec3(+loaded_squares_radius_float_plus*square_size, -loaded_squares_radius_float_plus*square_size, wall_height), Vec3(0.f, 0.f, -1.f), floor_color };
    *(vertices++) = { Vec3(-loaded_squares_radius_float_plus*square_size, +loaded_squares_radius_float_plus*square_size, wall_height), Vec3(0.f, 0.f, -1.f), floor_color };
    *(vertices++) = { Vec3(+loaded_squares_radius_float_plus*square_size, +loaded_squares_radius_float_plus*square_size, wall_height), Vec3(0.f, 0.f, -1.f), floor_color };
    
    setVectorStaticBuffer(buffer->buffer, o_vertices, vert_num);
    buffer->count = vert_num;
}

void load_pictures_into_buffer(BufferAndCount *buffer){
    Vertex_PNT o_vertices[9*2*24];
    Vertex_PNT *vertices = o_vertices;
    float fx = -2*square_size;
    for(int x=-1; x<=1; x++){
        fx += square_size;
        float fy = -2*square_size;
        for(int y=-1; y<=1; y++){
            fy += square_size;
            
            Vec2 tex_coords_x[] = {
                {  0.f, 0.f},
                {0.25f, 0.f},
                {  0.f, 0.25f},
                {0.25f, 0.25f}
            };
            Vec2 tex_coords_y[] = {
                {  0.f, 0.f},
                {0.25f, 0.f},
                {  0.f, 0.25f},
                {0.25f, 0.25f}
            };
            
            Vec3 p;
            p = Vec3(fx+picture_margin, fy+0.01f, picture_margin);
            
            // Front faces
            *(vertices++) = { p+Vec3(         0.f, wall_size,          0.f), Vec3(0.f, +1.f, 0.f), tex_coords_x[0]};
            *(vertices++) = { p+Vec3(         0.f, wall_size, picture_size), Vec3(0.f, +1.f, 0.f), tex_coords_x[1]};
            *(vertices++) = { p+Vec3(picture_size, wall_size,          0.f), Vec3(0.f, +1.f, 0.f), tex_coords_x[2]};
            
            *(vertices++) = { p+Vec3(picture_size, wall_size,          0.f), Vec3(0.f, +1.f, 0.f), tex_coords_x[2]};
            *(vertices++) = { p+Vec3(         0.f, wall_size, picture_size), Vec3(0.f, +1.f, 0.f), tex_coords_x[1]};
            *(vertices++) = { p+Vec3(picture_size, wall_size, picture_size), Vec3(0.f, +1.f, 0.f), tex_coords_x[3]};
            
            *(vertices++) = { p+Vec3(         0.f, wall_size,          0.f), Vec3(0.f, +1.f, 0.f), tex_coords_y[0]};
            *(vertices++) = { p+Vec3(         0.f, wall_size, picture_size), Vec3(0.f, +1.f, 0.f), tex_coords_y[1]};
            *(vertices++) = { p+Vec3(picture_size, wall_size,          0.f), Vec3(0.f, +1.f, 0.f), tex_coords_y[2]};
            
            *(vertices++) = { p+Vec3(picture_size, wall_size,          0.f), Vec3(0.f, +1.f, 0.f), tex_coords_y[2]};
            *(vertices++) = { p+Vec3(         0.f, wall_size, picture_size), Vec3(0.f, +1.f, 0.f), tex_coords_y[1]};
            *(vertices++) = { p+Vec3(picture_size, wall_size, picture_size), Vec3(0.f, +1.f, 0.f), tex_coords_y[3]};
            
            p.y -= 0.02f;
            
            // Back faces
            *(vertices++) = { p+Vec3(         0.f,       0.f,          0.f), Vec3(0.f, +1.f, 0.f), tex_coords_x[0]};
            *(vertices++) = { p+Vec3(         0.f,       0.f, picture_size), Vec3(0.f, +1.f, 0.f), tex_coords_x[1]};
            *(vertices++) = { p+Vec3(picture_size,       0.f,          0.f), Vec3(0.f, +1.f, 0.f), tex_coords_x[2]};
            
            *(vertices++) = { p+Vec3(picture_size,       0.f,          0.f), Vec3(0.f, +1.f, 0.f), tex_coords_x[2]};
            *(vertices++) = { p+Vec3(         0.f,       0.f, picture_size), Vec3(0.f, +1.f, 0.f), tex_coords_x[1]};
            *(vertices++) = { p+Vec3(picture_size,       0.f, picture_size), Vec3(0.f, +1.f, 0.f), tex_coords_x[3]};
            
            *(vertices++) = { p+Vec3(         0.f,       0.f,          0.f), Vec3(0.f, +1.f, 0.f), tex_coords_y[0]};
            *(vertices++) = { p+Vec3(         0.f,       0.f, picture_size), Vec3(0.f, +1.f, 0.f), tex_coords_y[1]};
            *(vertices++) = { p+Vec3(picture_size,       0.f,          0.f), Vec3(0.f, +1.f, 0.f), tex_coords_y[2]};
            
            *(vertices++) = { p+Vec3(picture_size,       0.f,          0.f), Vec3(0.f, +1.f, 0.f), tex_coords_y[2]};
            *(vertices++) = { p+Vec3(         0.f,       0.f, picture_size), Vec3(0.f, +1.f, 0.f), tex_coords_y[1]};
            *(vertices++) = { p+Vec3(picture_size,       0.f, picture_size), Vec3(0.f, +1.f, 0.f), tex_coords_y[3]};
            
            p = Vec3(fx+0.01f, fy+picture_margin, picture_margin);
            
            // Right face
            *(vertices++) = { p+Vec3(wall_size,          0.f,          0.f), Vec3(+1.f, 0.f, 0.f), tex_coords_x[0]};
            *(vertices++) = { p+Vec3(wall_size,          0.f, picture_size), Vec3(+1.f, 0.f, 0.f), tex_coords_x[1]};
            *(vertices++) = { p+Vec3(wall_size, picture_size,          0.f), Vec3(+1.f, 0.f, 0.f), tex_coords_x[2]};
            
            *(vertices++) = { p+Vec3(wall_size, picture_size,          0.f), Vec3(+1.f, 0.f, 0.f), tex_coords_x[2]};
            *(vertices++) = { p+Vec3(wall_size,          0.f, picture_size), Vec3(+1.f, 0.f, 0.f), tex_coords_x[1]};
            *(vertices++) = { p+Vec3(wall_size, picture_size, picture_size), Vec3(+1.f, 0.f, 0.f), tex_coords_x[3]};
            
            *(vertices++) = { p+Vec3(wall_size,          0.f,          0.f), Vec3(+1.f, 0.f, 0.f), tex_coords_y[0]};
            *(vertices++) = { p+Vec3(wall_size,          0.f, picture_size), Vec3(+1.f, 0.f, 0.f), tex_coords_y[1]};
            *(vertices++) = { p+Vec3(wall_size, picture_size,          0.f), Vec3(+1.f, 0.f, 0.f), tex_coords_y[2]};
            
            *(vertices++) = { p+Vec3(wall_size, picture_size,          0.f), Vec3(+1.f, 0.f, 0.f), tex_coords_y[2]};
            *(vertices++) = { p+Vec3(wall_size,          0.f, picture_size), Vec3(+1.f, 0.f, 0.f), tex_coords_y[1]};
            *(vertices++) = { p+Vec3(wall_size, picture_size, picture_size), Vec3(+1.f, 0.f, 0.f), tex_coords_y[3]};
            
            p.x -= 0.02f;
            
            // Left face
            *(vertices++) = { p+Vec3(      0.f,          0.f,          0.f), Vec3(-1.f, 0.f, 0.f), tex_coords_x[0]};
            *(vertices++) = { p+Vec3(      0.f,          0.f, picture_size), Vec3(-1.f, 0.f, 0.f), tex_coords_x[1]};
            *(vertices++) = { p+Vec3(      0.f, picture_size,          0.f), Vec3(-1.f, 0.f, 0.f), tex_coords_x[2]};
            
            *(vertices++) = { p+Vec3(      0.f, picture_size,          0.f), Vec3(-1.f, 0.f, 0.f), tex_coords_x[2]};
            *(vertices++) = { p+Vec3(      0.f,          0.f, picture_size), Vec3(-1.f, 0.f, 0.f), tex_coords_x[1]};
            *(vertices++) = { p+Vec3(      0.f, picture_size, picture_size), Vec3(-1.f, 0.f, 0.f), tex_coords_x[3]};
            
            *(vertices++) = { p+Vec3(      0.f,          0.f,          0.f), Vec3(-1.f, 0.f, 0.f), tex_coords_y[0]};
            *(vertices++) = { p+Vec3(      0.f,          0.f, picture_size), Vec3(-1.f, 0.f, 0.f), tex_coords_y[1]};
            *(vertices++) = { p+Vec3(      0.f, picture_size,          0.f), Vec3(-1.f, 0.f, 0.f), tex_coords_y[2]};
            
            *(vertices++) = { p+Vec3(      0.f, picture_size,          0.f), Vec3(-1.f, 0.f, 0.f), tex_coords_y[2]};
            *(vertices++) = { p+Vec3(      0.f,          0.f, picture_size), Vec3(-1.f, 0.f, 0.f), tex_coords_y[1]};
            *(vertices++) = { p+Vec3(      0.f, picture_size, picture_size), Vec3(-1.f, 0.f, 0.f), tex_coords_y[3]};
        }
    }
    
    setVectorStaticBuffer(buffer->buffer, o_vertices, 9*2*24);
    buffer->count = 9*2*24;
}

void load_pictures_into_texture(){
    for(int x=0; x<3; x++){
        for(int y=0; y<3; y++){
            
        }
    }
}
