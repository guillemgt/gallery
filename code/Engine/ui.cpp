#include <assert.h>
#include <string.h>

#include "engine.hpp"
#include "opengl.hpp"
#include "freetype.hpp"
#include "ui.hpp"

#define GOOD_FONT_SIZE FONT_SIZE_16
#define SMALL_FONT_SIZE FONT_SIZE_12

#define MAX_SELECT_OPTIONS_SHOWED   20
#define MAX_SELECT_OPTIONS_SHOWEDf  20.f
#define MAX_SELECT_OPTIONS_SHOWED_2 10

bool taEditing = true;

extern Vec2i windowSize;

extern char printLog[];

Tag uiRoot;

Tag *tagFocus = nullptr;
int focusTextPointer = 0;
int selectFocusStart = 0;

void (*uiLeftMouseDown)(Vec2 p) = nullptr;
void (*uiRightMouseDown)(Vec2 p) = nullptr;
void (*uiMouseMoved)(Vec2 p, Vec2 v) = nullptr;

struct {
    GLuint id;
    GLuint positionLocation, texCoordLocation, colorLocation, infoLocation, samplerLocation;
} uiLetterProgram;

struct {
    GLuint id;
    GLuint positionLocation, texCoordLocation, colorLocation, infoLocation, samplerLocation;
} uiColorProgram;

GlBuffer uiLetterBuffer;
GlBuffer uiColorBuffer;

GLuint uiLetterVertexArray;
GLuint uiColorVertexArray;
extern GLuint dummyVAO;

void initUIThing(){
    uiRoot = Tag(DIV, "");
    
    uiRoot.p = {0.f, 0.f};
    uiRoot.size = {0.f, 0.f};
    
    initPositionsColorsCoords(&uiLetterBuffer);
    initPositionsColors(&uiColorBuffer);
    
    const char letterShader[] =
#include "Shaders/letterShader.glsl"
    ;
    uiLetterProgram.id = loadShadersByText(letterShader);
    uiLetterProgram.positionLocation = glGetAttribLocation(uiLetterProgram.id, "a_position");
    uiLetterProgram.texCoordLocation = glGetAttribLocation(uiLetterProgram.id, "a_texCoords");
    uiLetterProgram.colorLocation    = glGetAttribLocation(uiLetterProgram.id, "a_color");
    uiLetterProgram.infoLocation     = glGetUniformLocation(uiLetterProgram.id, "u_info");
    uiLetterProgram.samplerLocation  = glGetUniformLocation(uiLetterProgram.id, "u_sampler");
    
    const char colorShader[] =
#include "Shaders/uiShader.glsl"
    ;
    uiColorProgram.id = loadShadersByText(colorShader);
    uiColorProgram.positionLocation = glGetAttribLocation(uiColorProgram.id, "a_position");
    uiColorProgram.colorLocation    = glGetAttribLocation(uiColorProgram.id, "a_color");
    uiColorProgram.infoLocation     = glGetUniformLocation(uiColorProgram.id, "u_info");
    
    glGenVertexArrays(1, &uiLetterVertexArray);
    glBindVertexArray(uiLetterVertexArray);
    
    glEnableVertexAttribArray(uiLetterProgram.positionLocation);
    glEnableVertexAttribArray(uiLetterProgram.texCoordLocation);
    glEnableVertexAttribArray(uiLetterProgram.colorLocation);
    
    glBindBuffer(GL_ARRAY_BUFFER, uiLetterBuffer.positions);
    glVertexAttribPointer(uiLetterProgram.positionLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, uiLetterBuffer.coords);
    glVertexAttribPointer(uiLetterProgram.texCoordLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, uiLetterBuffer.colors);
    glVertexAttribPointer(uiLetterProgram.colorLocation, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
    
    glGenVertexArrays(1, &uiColorVertexArray);
    glBindVertexArray(uiColorVertexArray);
    
    glEnableVertexAttribArray(uiColorProgram.positionLocation);
    glEnableVertexAttribArray(uiColorProgram.colorLocation);
    
    glBindBuffer(GL_ARRAY_BUFFER, uiColorBuffer.positions);
    glVertexAttribPointer(uiColorProgram.positionLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, uiColorBuffer.colors);
    glVertexAttribPointer(uiColorProgram.colorLocation, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
}

void cleanupUIThing(){
    cleanupPositionsColorsCoords(&uiLetterBuffer);
    cleanupPositionsColors(&uiColorBuffer);
    
    glDeleteVertexArrays(1, &uiLetterVertexArray);
    glDeleteVertexArrays(1, &uiColorVertexArray);
    // @Incomplete: delete programs
}

void renderTagCount(Tag &t, Vec2i &count){
    if(t.shown){
        if(t.text.length > 0){
            count.y += textVertNum(t.text);
        }
        if(t.type == BUTTON || t.type == TAB_TITLE || t.type == TEXTINPUT || t.type == SELECT || t.type == SELECT_DROPDOWN || t.type == CHECKBOX || t.type == DIV){
            count.x += 6;
        }
        if(&t == tagFocus && t.type == TEXTINPUT){
            count.x += 6;
        }
        if(t.type == SELECT_DROPDOWN){
            for(int i=selectFocusStart; i<t.childrenNum && i<selectFocusStart+20; i++)
                renderTagCount(*t.children[i], count);
        }else{
            for(int i=0; i<t.childrenNum; i++){
                renderTagCount(*t.children[i], count);
            }
        }
    }
}

void renderTag(Tag &t, float x0, float &x, float &y, Vec3 *&squaresPos, RgbaColor *&squaresCol, Vec3 *&textPos, RgbaColor *&textCol, Vec2 *&textCoords){
    if(t.shown){
        switch(t.type){
            case BUTTON:
            {
                float width = 0, height = 0;
                if(t.text.length > 0){
                    unsigned int l = renderText(x+9.f, y-1.f, GOOD_FONT_SIZE, t.text, textPos, textCol, textCoords, &width, &height);
                    textPos += l;
                    textCol += l;
                    textCoords += l;
                }
                t.p = {x, y+1.f};
                t.size = {width+18.f, height-6.f};
                
                squaresPos[0] = {x,            y+1.f,        -0.995f};
                squaresPos[1] = {x+width+18.f, y+1.f,        -0.995f};
                squaresPos[2] = {x,            y-height+6.f, -0.995f};
                squaresPos[3] = {x+width+18.f, y+1.f,        -0.995f};
                squaresPos[4] = {x,            y-height+6.f, -0.995f};
                squaresPos[5] = {x+width+18.f, y-height+6.f, -0.995f};
                
                x += width+30.f;
                
                squaresCol[0] = {150, 155, 155, 200};
                squaresCol[1] = {150, 155, 155, 200};
                squaresCol[2] = {150, 155, 155, 200};
                squaresCol[3] = {150, 155, 155, 200};
                squaresCol[4] = {150, 155, 155, 200};
                squaresCol[5] = {150, 155, 155, 200};
                
                squaresPos += 6;
                squaresCol += 6;
            }
                break;
            case TEXTINPUT:
            {
                float width = 0, height = 0;
                if(t.text.length > 0){
                    unsigned int l = renderText(x+9.f, y-1.f, GOOD_FONT_SIZE, t.text, textPos, textCol, textCoords, &width, &height);
                    textPos += l;
                    textCol += l;
                    textCoords += l;
                }
                if(width < 100.f)
                    width = 100.f;
                t.p = {x, y+1.f};
                t.size = {width+18.f, height-6.f};
                
                squaresPos[0] = {x,            y+1.f,        -0.995f};
                squaresPos[1] = {x+width+18.f, y+1.f,        -0.995f};
                squaresPos[2] = {x,            y-height+6.f, -0.995f};
                squaresPos[3] = {x+width+18.f, y+1.f,        -0.995f};
                squaresPos[4] = {x,            y-height+6.f, -0.995f};
                squaresPos[5] = {x+width+18.f, y-height+6.f, -0.995f};
                
                x += width+30.f;
                
                if(&t == tagFocus){
                    squaresCol[0] = {255, 255, 205, 200};
                    squaresCol[1] = {255, 255, 205, 200};
                    squaresCol[2] = {255, 255, 205, 200};
                    squaresCol[3] = {255, 255, 205, 200};
                    squaresCol[4] = {255, 255, 205, 200};
                    squaresCol[5] = {255, 255, 205, 200};
                }else{
                    squaresCol[0] = {250, 255, 255, 200};
                    squaresCol[1] = {250, 255, 255, 200};
                    squaresCol[2] = {250, 255, 255, 200};
                    squaresCol[3] = {250, 255, 255, 200};
                    squaresCol[4] = {250, 255, 255, 200};
                    squaresCol[5] = {250, 255, 255, 200};
                }
                
                squaresPos += 6;
                squaresCol += 6;
                
                if(&t == tagFocus){
                    float xp = x-(width+30.f)+9.f+renderTextLength(GOOD_FONT_SIZE, t.text, focusTextPointer);
                    squaresPos[0] = {xp,     y-2.f,         -0.996f};
                    squaresPos[1] = {xp+2.f, y-2.f,         -0.996f};
                    squaresPos[2] = {xp,     y-height+12.f, -0.996f};
                    squaresPos[3] = {xp+2.f, y-2.f,         -0.996f};
                    squaresPos[4] = {xp,     y-height+12.f, -0.996f};
                    squaresPos[5] = {xp+2.f, y-height+12.f, -0.996f};
                    
                    squaresCol[0] = {0, 0, 0, 200};
                    squaresCol[1] = {0, 0, 0, 200};
                    squaresCol[2] = {0, 0, 0, 200};
                    squaresCol[3] = {0, 0, 0, 200};
                    squaresCol[4] = {0, 0, 0, 200};
                    squaresCol[5] = {0, 0, 0, 200};
                    
                    squaresPos += 6;
                    squaresCol += 6;
                    
                }
            }
                break;
            case SELECT:
            {
                float width = 0, height = 0;
                if(t.text.length > 0){
                    unsigned int l = renderText(x+9.f, y-1.f, GOOD_FONT_SIZE, t.text, textPos, textCol, textCoords, &width, &height);
                    textPos += l;
                    textCol += l;
                    textCoords += l;
                }
                t.p = {x, y+1.f};
                t.size = {width+18.f, height-6.f};
                
                squaresPos[0] = {x,            y+1.f,        -0.995f};
                squaresPos[1] = {x+width+18.f, y+1.f,        -0.995f};
                squaresPos[2] = {x,            y-height+6.f, -0.995f};
                squaresPos[3] = {x+width+18.f, y+1.f,        -0.995f};
                squaresPos[4] = {x,            y-height+6.f, -0.995f};
                squaresPos[5] = {x+width+18.f, y-height+6.f, -0.995f};
                
                x += width+30.f;
                
                squaresCol[0] = {195, 195, 195, 200};
                squaresCol[1] = {195, 195, 195, 200};
                squaresCol[2] = {195, 195, 195, 200};
                squaresCol[3] = {195, 195, 195, 200};
                squaresCol[4] = {195, 195, 195, 200};
                squaresCol[5] = {195, 195, 195, 200};
                
                squaresPos += 6;
                squaresCol += 6;
            }
                break;
            case SELECT_DROPDOWN:
            {
                float width = 100.f, height = 5.f+17.f*(fmin(t.childrenNum-selectFocusStart, 20.f));
                
                squaresPos[0] = {x,            y+1.f,        -0.995f};
                squaresPos[1] = {x+width+18.f, y+1.f,        -0.995f};
                squaresPos[2] = {x,            y-height+6.f, -0.995f};
                squaresPos[3] = {x+width+18.f, y+1.f,        -0.995f};
                squaresPos[4] = {x,            y-height+6.f, -0.995f};
                squaresPos[5] = {x+width+18.f, y-height+6.f, -0.995f};
                
                x += 5.f;
                x0 = x;
                
                squaresCol[0] = {190, 195, 195, 200};
                squaresCol[1] = {190, 195, 195, 200};
                squaresCol[2] = {190, 195, 195, 200};
                squaresCol[3] = {190, 195, 195, 200};
                squaresCol[4] = {190, 195, 195, 200};
                squaresCol[5] = {190, 195, 195, 200};
                
                squaresPos += 6;
                squaresCol += 6;
            }
                break;
            case CHECKBOX:
            {
                float width = 0, height = 0;
                if(t.text.length > 0){
                    unsigned int l = renderText(x+9.f, y-1.f, GOOD_FONT_SIZE, t.text, textPos, textCol, textCoords, &width, &height);
                    textPos += l;
                    textCol += l;
                    textCoords += l;
                }
                width = 30.f;
                t.p = {x, y+1.f};
                t.size = {width+18.f, height-6.f};
                
                squaresPos[0] = {x,            y+1.f,        -0.995f};
                squaresPos[1] = {x+width, y+1.f,        -0.995f};
                squaresPos[2] = {x,            y-height+6.f, -0.995f};
                squaresPos[3] = {x+width, y+1.f,        -0.995f};
                squaresPos[4] = {x,            y-height+6.f, -0.995f};
                squaresPos[5] = {x+width, y-height+6.f, -0.995f};
                
                x += width+5.f;
                
                squaresCol[0] = {190, 195, 195, 200};
                squaresCol[1] = {190, 195, 195, 200};
                squaresCol[2] = {190, 195, 195, 200};
                squaresCol[3] = {190, 195, 195, 200};
                squaresCol[4] = {190, 195, 195, 200};
                squaresCol[5] = {190, 195, 195, 200};
                
                squaresPos += 6;
                squaresCol += 6;
            }
                break;
            case TAB_TITLE:
            {
                float width = 0, height = 0;
                if(t.text.length > 0){
                    unsigned int l = renderText(x+9.f, y-1.f, GOOD_FONT_SIZE, t.text, textPos, textCol, textCoords, &width, &height);
                    if(x+width+30.f > 430.f){
                        y -= 35.f;
                        x = x0;
                        l = renderText(x+9.f, y-1.f, GOOD_FONT_SIZE, t.text, textPos, textCol, textCoords, &width, &height);
                    }
                    textPos += l;
                    textCol += l;
                    textCoords += l;
                }
                t.p = {x, y+1.f};
                t.size = {width+18.f, height-6.f};
                
                squaresPos[0] = {x,            y+1.f,        -0.995f};
                squaresPos[1] = {x+width+18.f, y+1.f,        -0.995f};
                squaresPos[2] = {x,            y-height+6.f, -0.995f};
                squaresPos[3] = {x+width+18.f, y+1.f,        -0.995f};
                squaresPos[4] = {x,            y-height+6.f, -0.995f};
                squaresPos[5] = {x+width+18.f, y-height+6.f, -0.995f};
                
                x += width+30.f;
                
                if(t.parent->parent->children[1]->children[t.parentNum]->shown){
                    squaresCol[0] = {50, 155, 195, 200};
                    squaresCol[1] = {50, 155, 195, 200};
                    squaresCol[2] = {50, 155, 195, 200};
                    squaresCol[3] = {50, 155, 195, 200};
                    squaresCol[4] = {50, 155, 195, 200};
                    squaresCol[5] = {50, 155, 195, 200};
                }else{
                    squaresCol[0] = {150, 155, 195, 200};
                    squaresCol[1] = {150, 155, 195, 200};
                    squaresCol[2] = {150, 155, 195, 200};
                    squaresCol[3] = {150, 155, 195, 200};
                    squaresCol[4] = {150, 155, 195, 200};
                    squaresCol[5] = {150, 155, 195, 200};
                }
                
                squaresPos += 6;
                squaresCol += 6;
            }
                break;
            case NEW_LINE:
            case TABS:
                y -= 35.f;
                x = x0;
                break;
            case SELECT_OPTION:
            {
                float width = 0, height = 0;
                if(t.text.length > 0){
                    unsigned int l = renderText(x, y, SMALL_FONT_SIZE, t.text, textPos, textCol, textCoords, &width, &height);
                    textPos += l;
                    textCol += l;
                    textCoords += l;
                }
                t.p = {x, y+2.f};
                t.size = {(float)fmax(width, 100.f), height};
                y -= 17.f;
                x = x0;
            }
                break;
            case DIV:
            {
                x = t.p.x;
                y = t.p.y;
                float width = 0, height = 0;
                if(t.text.length > 1){
                    unsigned int l = renderText(x+5.f, y-5.f, SMALL_FONT_SIZE, t.text, textPos, textCol, textCoords, &width, &height);
                    textPos += l;
                    textCol += l;
                    textCoords += l;
                    width += 10.f;
                    height += 10.f;
                }
                if(t.size.x > 0.f)
                    width = t.size.x;
                if(t.size.y > 0.f)
                    height = t.size.y;
                
                squaresPos[0] = {x,       y,        -0.98f};
                squaresPos[1] = {x+width, y,        -0.98f};
                squaresPos[2] = {x,       y-height, -0.98f};
                squaresPos[3] = {x+width, y,        -0.98f};
                squaresPos[4] = {x,       y-height, -0.98f};
                squaresPos[5] = {x+width, y-height, -0.98f};
                
                x += 5.f;
                y -= 5.f;
                
                squaresCol[0] = {255, 255, 255, 100};
                squaresCol[1] = {255, 255, 255, 100};
                squaresCol[2] = {255, 255, 255, 100};
                squaresCol[3] = {255, 255, 255, 100};
                squaresCol[4] = {255, 255, 255, 100};
                squaresCol[5] = {255, 255, 255, 100};
                
                squaresPos += 6;
                squaresCol += 6;
            } break;
            default:
                if(t.text.length > 0){
                    float width = 0;
                    unsigned int l = renderText(x, y, GOOD_FONT_SIZE, t.text, textPos, textCol, textCoords, &width, nullptr);
                    x += width+10.f;
                    textPos += l;
                    textCol += l;
                    textCoords += l;
                    t.p = {0.f, 0.f};
                    t.size = {0.f, 0.f};
                }
                break;
        }
        if(t.type == SELECT_DROPDOWN){
            for(int i=selectFocusStart; i<t.childrenNum && i<selectFocusStart+20; i++)
                renderTag(*t.children[i], x0, x, y, squaresPos, squaresCol, textPos, textCol, textCoords);
            y += 17.f*fmin(t.childrenNum-selectFocusStart, 20.f);
        }else{
            for(int i=0; i<t.childrenNum; i++)
                renderTag(*t.children[i], x0, x, y, squaresPos, squaresCol, textPos, textCol, textCoords);
        }
    }
}

void renderTags(float x, float y, Vec3 *squaresPos, RgbaColor *squaresCol, Vec3 *textPos, RgbaColor *textCol, Vec2 *textCoords){
	renderTag(uiRoot, x, x, y, squaresPos, squaresCol, textPos, textCol, textCoords);
}

void renderUi(){
    start_temp_alloc();
    
    Vec2i count = {0, 0};
    renderTagCount(uiRoot, count);
    Vec3 *squaresPos = (Vec3 *)temp_alloc(count.x*sizeof(Vec3));
    RgbaColor *squaresCol = (RgbaColor *)temp_alloc(count.x*sizeof(RgbaColor));
    
    Vec3 *textPos = nullptr;
    RgbaColor *textCol = nullptr;
    Vec2 *textCoords = nullptr;
    if(count.y > 0){
        textPos = (Vec3 *)temp_alloc(count.y*sizeof(Vec3));
        textCol = (RgbaColor *)temp_alloc(count.y*sizeof(RgbaColor));
        textCoords = (Vec2 *)temp_alloc(count.y*sizeof(Vec2));
    }
    
	renderTags(10.f, -10.f, squaresPos, squaresCol, textPos, textCol, textCoords);
    
    setVectorDynamicBuffer(uiColorBuffer.positions, (GLfloat *)squaresPos, 3*count.x);
    setVectorDynamicBuffer(uiColorBuffer.colors, (GLubyte *)squaresCol, 4*count.x);
    uiColorBuffer.verts = count.x;
    
    if(count.y > 0){
        setVectorDynamicBuffer(uiLetterBuffer.positions, (GLfloat *)textPos, 3*count.y);
        setVectorDynamicBuffer(uiLetterBuffer.colors, (GLubyte *)textCol, 4*count.y);
        setVectorDynamicBuffer(uiLetterBuffer.coords, (GLfloat *)textCoords, 2*count.y);
    }
    uiLetterBuffer.verts = count.y;
    
    end_temp_alloc();
    
    // Do the drawing
    
    glViewport(0, 0, windowSize.x, windowSize.y);
    glScissor(0, 0, windowSize.x, windowSize.y);
    
    if(uiLetterBuffer.verts > 0){
        glUseProgram(uiLetterProgram.id);
        
        glUniform1i(uiLetterProgram.samplerLocation, 1);
        glUniform4f(uiLetterProgram.infoLocation, 2.f/windowSize.x, 2.f/windowSize.y, windowSize.x/2.f, -(windowSize.y/2.f));
        
        glBindVertexArray(uiLetterVertexArray);
        glDrawArrays(GL_TRIANGLES, 0, uiLetterBuffer.verts);
    }
    
    if(uiColorBuffer.verts > 0){
        glUseProgram(uiColorProgram.id);
        glUniform4f(uiColorProgram.infoLocation, 2.f/windowSize.x, 2.f/windowSize.y, windowSize.x/2.f, -(windowSize.y/2.f));
        
        glBindVertexArray(uiColorVertexArray);
        glDrawArrays(GL_TRIANGLES, 0, uiColorBuffer.verts);
    }
    glBindVertexArray(dummyVAO);
}

Tag *isClickOnTag(Vec2 p, Tag &t){
    if(t.shown){
        Vec2 d = {p.x-t.p.x, t.p.y-p.y};
        if(t.type != DIV && d.x >= 0.f && d.y >= 0.f && d.x < t.size.x && d.y < t.size.y){
            return &t;
        }
        for(int i=0; i<t.childrenNum; i++){
            Tag *c = isClickOnTag(p, *t.children[i]);
            if(c != nullptr)
                return c;
        }
    }
    return nullptr;
}

void leftClick(Vec2 p){
    tagFocus = nullptr;
    selectFocusStart = 0;
    Tag *t = isClickOnTag(p, uiRoot);
    if(t != nullptr){
        if(t->type == TAB_TITLE){
            Tag *tabs = t->parent->parent->children[1];
            for(int i=0; i<tabs->childrenNum; i++)
                tabs->children[i]->shown = false;
            tabs->children[t->parentNum]->shown = true;
        }else if(t->type == TEXTINPUT){
            tagFocus = t;
            float x1 = t->p.x+9.f, x2 = t->p.x+9.f;
            int i;
            for(i=0; i<t->text.length-1; i++){
                x2 = x1;
                x1 = t->p.x+9.f+renderTextLength(GOOD_FONT_SIZE, t->text, i);
                if(x1 > p.x)
                    break;
            }
            if(p.x < (x1+x2)*0.5f)
                focusTextPointer = i-1;
            else
                focusTextPointer = i;
        }else if(t->type == SELECT){
            if(t->children[0]->shown)
                t->children[0]->shown = false;
            else{
                t->children[0]->shown = true;
                tagFocus = t;
                selectFocusStart = 0;
                for(int i=0; i<t->children[0]->childrenNum; i++){
                    if(t->children[0]->children[i]->text == t->text){
                        selectFocusStart = i-MAX_SELECT_OPTIONS_SHOWED_2;
                        break;
                    }
                }
                if(selectFocusStart < 0)
                    selectFocusStart = 0;
            }
        }else if(t->type == SELECT_OPTION){
            if(t->parent->shown){
                t->parent->parent->text = t->text;
                t->parent->shown = false;
            }
            if(t->parent->parent->onClick != nullptr)
                t->parent->parent->onClick(t->parent->parent);
        }else if(t->type == CHECKBOX){
            if(t->text.length > 1){
                t->text = "";
            }else{
                t->text = "X";
            }
        }
        if(t->onClick != nullptr && t->type != SELECT && t->type != SELECT_OPTION && t->type != TEXTINPUT)
            t->onClick(t);
        //t->text = "Test successful!";
    }else{
        if(uiLeftMouseDown != nullptr)
            uiLeftMouseDown(p);
    }
};
void rightClick(Vec2 p){
    if(uiRightMouseDown != nullptr)
        uiRightMouseDown(p);
}
int mouseMoved(Vec2 p, Vec2 v){
    if(uiMouseMoved != nullptr)
        uiMouseMoved(p, v);
    return CURSOR_ARROW;
}

void keyDown(char c, bool shift){
    if(tagFocus != nullptr){
        if(tagFocus->type == TEXTINPUT){
            if(c > 0){
                if(c == 5)
                    return;
                if(c == 2){
                    focusTextPointer++;
                    if(focusTextPointer > tagFocus->text.length-1)
                        focusTextPointer = (int)tagFocus->text.length-1;
                }else if(c == 4){
                    focusTextPointer--;
                    if(focusTextPointer < 0)
                        focusTextPointer = 0;
                }else{
                    if(shift){
                        if(c == '7')
                            c = '/';
                    }else{
                        if(c >= 'A' && c <= 'Z')
                            c += 'a'-'A';
                    }
                    tagFocus->text += '\0';
                    for(int i=(int)tagFocus->text.length-1; i>focusTextPointer; i--)
                        tagFocus->text.text[i] = tagFocus->text.text[i-1];
                    tagFocus->text.text[focusTextPointer] = c;
                    focusTextPointer++;
                    if(tagFocus->onClick != nullptr)
                        tagFocus->onClick(tagFocus);
                }
                
            }else if(tagFocus->text.length > 1){
                for(int i=focusTextPointer-1; i<(int)tagFocus->text.length-2; i++)
                    tagFocus->text.text[i] = tagFocus->text.text[i+1];
                tagFocus->text.text[tagFocus->text.length-2] = '\0';
                focusTextPointer--;
                tagFocus->text.length--;
                // @
                if(tagFocus->onClick != nullptr)
                    tagFocus->onClick(tagFocus);
            }
        }else if(tagFocus->type == SELECT){
            if(c == 'W' || c == ',' || c == 1){
                if(selectFocusStart > 0)
                    selectFocusStart--;
            }else if(c == 'S' || c == 'O' || c == 3){
                if(selectFocusStart < tagFocus->children[0]->childrenNum-MAX_SELECT_OPTIONS_SHOWED)
                    selectFocusStart++;
            }
        }
    }
}

void changeTagValueALittle(float st){
    if(tagFocus != nullptr){
        if(tagFocus->type == TEXTINPUT){
            tagFocus->setFloatValue(tagFocus->floatValue()+st);
            if(tagFocus->onClick != nullptr)
                tagFocus->onClick(tagFocus);
        }
    }
}
