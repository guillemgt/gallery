#include "Engine/engine.hpp"
#include "Engine/window.hpp"
#include "Engine/opengl.hpp"

#include "main.hpp"
#include "render.hpp"

Player player;

int initGame(){
    player.looking_direction = Vec3(0.f, 1.f, 0.f);
    player.looking_angle = Angle(1.f, 0.f);
    player.position_relative_to_coords = Vec2(0.f);
    load_world();
    init_openGL();
    return 1;
}

extern f32 TIME_STEP;

int gameLogic(bool keys[KEYS_NUM], StaticArray<Event, MAX_EVENTS_PER_LOOP> events){
    for(int i=0; i<events.size; i++){
        switch(events[i].id){
            case EVENT_LEFTMOUSEMOVE: {
                float dx = 0.01f*events[i].data.move.v.x;
                float l = sqrtf(1.f+dx*dx);
                Angle a = Angle(1.f/l, dx/l);
                player.looking_direction = rotate(player.looking_direction, a);
                player.looking_angle = normalize(Vec2(-player.looking_direction.y, player.looking_direction.x));
            } break;
        }
    }
    
    // Modify player velocity
    Vec2 flat_looking_direction = Vec2(player.looking_direction.x, player.looking_direction.y);
    if(flat_looking_direction != Vec2(0.f)){
        flat_looking_direction = normalize(flat_looking_direction);
        player.v = Vec2(0.f);
        if(keys[0]){
            player.v = 5.f*flat_looking_direction;
        }
        if(keys[2]){
            player.v = -5.f*flat_looking_direction;
        }
        if(keys[1]){
            player.v = 5.f*rotate(flat_looking_direction, Angle(0.f, +1.f));
        }
        if(keys[3]){
            player.v = 5.f*rotate(flat_looking_direction, Angle(0.f, -1.f));
        }
    }
    
    // Calculate next player position
    // First, check if (and when) the player will collide with a wall
    // Recall that the walls are at ([-player_size, player_size+wall_size]u[-player_size+square_size, player_size+square_size+wall_size])^2
    const float player_size = 0.7f;
    const float walls[4] = {
        -player_size,
        player_size+wall_size,
        -player_size+square_size,
        player_size+square_size+wall_size
    };
    
    float time_left = TIME_STEP;
    do{
        float time_x[4], time_y[4];
        if(player.v.x > 0.f){
            for(int i=0; i<4; i++)
                time_x[i] = (walls[i]-player.position_relative_to_coords.x)/player.v.x;
        }else if(player.v.x < 0.f){
            for(int i=0; i<4; i++)
                time_x[i] = (walls[3-i]-player.position_relative_to_coords.x)/player.v.x;
        }else{
            time_x[0] = -INFINITY;
            for(int i=1; i<4; i++)
                time_x[i] = INFINITY;
        }
        if(player.v.y > 0.f){
            for(int i=0; i<4; i++)
                time_y[i] = (walls[i]-player.position_relative_to_coords.y)/player.v.y;
        }else if(player.v.y < 0.f){
            for(int i=0; i<4; i++)
                time_y[i] = (walls[3-i]-player.position_relative_to_coords.y)/player.v.y;
        }else{
            time_y[0] = -INFINITY;
            for(int i=1; i<4; i++)
                time_y[i] = INFINITY;
        }
        
        float min_time = time_left;
        int collision_axis = 0;
        for(int i=0; i<4; i+=2){
            if(time_x[i] >= 0.f){
                if(time_y[0] <= time_x[i] && time_x[i] <= time_y[1]){
                    if(time_x[i] < min_time){
                        min_time = time_x[i];
                        collision_axis = 1;
                    }
                }else if(time_y[2] <= time_x[i] && time_x[i] <= time_y[3]){
                    if(time_x[i] < min_time){
                        min_time = time_x[i];
                        collision_axis = 1;
                    }
                }
            }
            
            if(time_y[i] >= 0.f){
                if(time_x[0] <= time_y[i] && time_y[i] <= time_x[1]){
                    if(time_y[i] < min_time){
                        min_time = time_y[i];
                        collision_axis = 2;
                    }
                }else if(time_x[2] <= time_y[i] && time_y[i] <= time_x[3]){
                    if(time_y[i] < min_time){
                        min_time = time_y[i];
                        collision_axis = 2;
                    }
                }
            }
        }
        
        time_left -= min_time;
        player.position_relative_to_coords += min_time*player.v;
        if(collision_axis == 1){
            player.v.x = 0.f;
        }else if(collision_axis == 2){
            player.v.y = 0.f;
        }
    }while(time_left > 0.f);
    
    if(player.position_relative_to_coords.x < 0.f)
        player.position_relative_to_coords.x += square_size;
    if(player.position_relative_to_coords.x >= square_size)
        player.position_relative_to_coords.x -= square_size;
    if(player.position_relative_to_coords.y < 0.f)
        player.position_relative_to_coords.y += square_size;
    if(player.position_relative_to_coords.y >= square_size)
        player.position_relative_to_coords.y -= square_size;
    
    return 1;
}
void gameDraw(){
    draw_scene();
}
void cleanUpGame(){
    
}
