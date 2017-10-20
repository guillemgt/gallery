#ifndef world_hpp
#define world_hpp

#include "bigint.hpp"

const int picture_width  = 128;
const int picture_height = 128;
const int bytes_per_pixels = (256+256+256)/8;
const int bytes_per_u64    = 8;

const int bytes_per_picture = picture_width*picture_height * bytes_per_pixels;
const int   u64_per_picture = bytes_per_picture / bytes_per_u64;

const int bytes_per_picture_coord = bytes_per_picture/2;
const int   u64_per_picture_coord =   u64_per_picture/2;

typedef BigUint<u64_per_picture> PictureCoords;

#endif /* world_hpp */
