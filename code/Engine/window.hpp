#ifndef engine_window_h
#define engine_window_h

#include "engine.hpp"

#define KEYS_UP    0
#define KEYS_RIGHT 1
#define KEYS_DOWN  2
#define KEYS_LEFT  3
#define KEYS_SPACE 4
#define KEYS_ESC   5
#define KEYS_SHIFT 6
#define KEYS_ENTER 7
#define KEYS_Z     8
#define KEYS_R     9
#define KEYS_P     10 // Pause
#define KEYS_N     11 // Next iteration

#define KEYS_NUM   12

#define MAX_EVENTS_PER_LOOP  256
#define EVENT_KEYDOWN        0
#define EVENT_KEYUP          1
#define EVENT_LEFTMOUSEDOWN  2
#define EVENT_LEFTMOUSEMOVE  3
#define EVENT_LEFTMOUSEUP    4
#define EVENT_RIGHTMOUSEDOWN 5
#define EVENT_RIGHTMOUSEMOVE 6
#define EVENT_RIGHTMOUSEUP   7
#define EVENT_RESIZE         8
#define EVENT_CLOSE          9
#define EVENT_FOCUS_LOST     10

struct MouseMovement{
    Vec2i r, v;
};

union EventData {
    char key;
    Vec2i size;
    Vec2i coords;
    MouseMovement move;
    inline EventData(){ // It needs this initializer somewhere...
    }
    inline EventData(char k) : key(k) {}
	inline EventData(int x, int y){
		size = { x, y };
	}
	inline EventData(int x, int y, int z, int w){
		move.r = { x, y };
		move.v = { z, w };
	}
};

struct Event{
    uint8 id;
    EventData data;
};

int initGame();
int gameLogic(bool keys[KEYS_NUM], StaticArray<Event, MAX_EVENTS_PER_LOOP> events);
void gameDraw();
void cleanUpGame();

#endif /* main_hpp */
