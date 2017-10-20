#include "Engine/engine.hpp"
#include "Engine/window.hpp"
#include "Engine/opengl.hpp"

#include "main.hpp"
#include "render.hpp"

Player player;

int initGame(){
    player.looking_direction = Vec3(0.f, 1.f, 0.f);
    player.position_relative_to_coords = Vec2(0.f);
    init_openGL();
    return 1;
}

extern f32 TIME_STEP;

int gameLogic(bool keys[KEYS_NUM], StaticArray<Event, MAX_EVENTS_PER_LOOP> events){
    Vec2 flat_looking_direction = Vec2(player.looking_direction.x, player.looking_direction.y);
    if(flat_looking_direction != Vec2(0.f)){
        flat_looking_direction = normalize(flat_looking_direction);
        if(keys[0]){
            player.position_relative_to_coords += 5.f*flat_looking_direction*TIME_STEP;
        }
        if(keys[2]){
            player.position_relative_to_coords -= 5.f*flat_looking_direction*TIME_STEP;
        }
        if(keys[1]){
            player.position_relative_to_coords += 5.f*rotate(flat_looking_direction, Angle(0.f, +1.f))*TIME_STEP;
        }
        if(keys[3]){
            player.position_relative_to_coords += 5.f*rotate(flat_looking_direction, Angle(0.f, -1.f))*TIME_STEP;
        }
    }
    return 1;
}
void gameDraw(){
    draw_scene();
}
void cleanUpGame(){
    
}
