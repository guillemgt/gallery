#ifndef engine_ui_h
#define engine_ui_h

#include "engine.hpp"

#define CURSOR_ARROW   0
#define CURSOR_POINTER 1
#define CURSOR_TEXT    2

void renderUi();

typedef enum {
    DIV,
    LABEL,
    TEXTAREA,
    BUTTON,
    TEXTINPUT,
    CHECKBOX,
    SELECT,
    SELECT_DROPDOWN,
    SELECT_OPTION,
    TAB_VIEW,
    TAB_TITLES,
    TAB_TITLE,
    TABS,
    TAB,
    NEW_LINE
} TagType;

struct Tag {
    TagType type;
    String text;
    Tag **children, *parent;
    Vec2 p, size;
    void (*onClick)(Tag *);
    unsigned int childrenNum, parentNum;
    bool shown;
    
    Tag(){}
    Tag(TagType t){
        type = t;
        //text.allocator = &somePermanentStorage.allocator;
        text = String();
        childrenNum = 0;
        children = nullptr;
        shown = true;
        onClick = nullptr;
    }
    
    Tag(TagType t, const char *s){
        type = t;
        //text.allocator = &somePermanentStorage.allocator;
        text = s;
        childrenNum = 0;
        children = nullptr;
        shown = true;
        onClick = nullptr;
    }
    
    Tag *addChild(TagType t){
        childrenNum++;
        if(childrenNum > 1)
            children = (Tag **)realloc(children, childrenNum*sizeof(Tag *));
        else
            children = (Tag **)malloc(childrenNum*sizeof(Tag *));
        Tag *c;
        c = (Tag *)malloc(sizeof(Tag));
        *c = Tag(t);
        c->parent = this;
        c->parentNum = childrenNum-1;
        
        children[childrenNum-1] = c;
        return c;
    }
    Tag *addChild(TagType t, const char *s){
        childrenNum++;
        if(childrenNum > 1)
            children = (Tag **)realloc(children, childrenNum*sizeof(Tag *));
        else
            children = (Tag **)malloc(childrenNum*sizeof(Tag *));
        Tag *c;
        c = (Tag *)malloc(sizeof(Tag));
        *c = Tag(t, s);
        c->parent = this;
        c->parentNum = childrenNum-1;
        
        children[childrenNum-1] = c;
        
        if(c->type == SELECT_DROPDOWN || c->type == TAB_VIEW)
            c->shown = false;
        
        return c;
    }
    void removeAllChildren(){
        for(int i=0; i<childrenNum; i++){
            children[i]->removeAllChildren();
            free(children[i]);
        }
        free(children);
        children = nullptr;
        childrenNum = 0;
    }
    
    float floatValue(){
        float a;
        sscanf(text.text, "%f", &a);
        return a;
    }
    void setFloatValue(float a){
        char s[20];
        sprintf(s, "%g", a);
        text = s;
    }
    int intValue(){
        int a;
        sscanf(text.text, "%i", &a);
        return a;
    }
    int hexValue(){
        int a;
        sscanf(text.text, "%x", &a);
        return a;
    }
    void setHexValue(int a){
        char s[20];
        sprintf(s, "%x", a);
        text = s;
    }
};

void initUIThing();
void cleanupUIThing();

void leftClick(Vec2 p);
void rightClick(Vec2 p);
int mouseMoved(Vec2 p, Vec2 v);

void keyDown(char c, bool shift);
void moveFocusPointer(int amount);

void changeTagValueALittle(float st);

#endif /* defined(__ta__ui__) */
