#ifndef world_hpp
#define world_hpp

#include "bigint.hpp"
#include "render.hpp"

const int picture_width  = 128;
const int picture_height = 128;
const int bytes_per_pixels = (256+256+256)/8;
const int bytes_per_u64    = 8;

const int bytes_per_picture = picture_width*picture_height * bytes_per_pixels;
const int   u64_per_picture = bytes_per_picture / bytes_per_u64;

const int bytes_per_picture_coord = bytes_per_picture/2;
const int   u64_per_picture_coord =   u64_per_picture/2;

typedef BigUint<u64_per_picture> PictureCoords;


const float square_size = 8.f;
const float wall_size   = 5.f;
const float wall_height = 5.f;

const float loaded_squares_radius_float = 3.f;
const int   loaded_squares_radius_int = 3;
const int   total_loaded_squares = 2*loaded_squares_radius_int+1;

void load_walls_into_buffer(BufferAndCount *buffer);

#endif /* world_hpp */
