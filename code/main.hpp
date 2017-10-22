#ifndef main_h
#define main_h

#include "world.hpp"

struct Player {
    PictureCoords *coords[2];
    Vec2 position_relative_to_coords;
    Vec2 v;
    Vec3 looking_direction;
    Angle looking_angle;
    bool falling;
};

extern Player player;

#endif /* main_h */
