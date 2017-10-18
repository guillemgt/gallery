#include "Engine/engine.hpp"
#include "Engine/window.hpp"
#include "Engine/opengl.hpp"

#include "render.hpp"

int initGame(){
    init_openGL();
    return 1;
}
int gameLogic(bool keys[KEYS_NUM], StaticArray<Event, MAX_EVENTS_PER_LOOP> events){
    
    return 1;
}
void gameDraw(){
    draw_scene();
}
void cleanUpGame(){
    
}
