#include <stdio.h>
#ifdef DEBUG
#include <stdarg.h>
#endif

#include "engine.hpp"
#include "freetype.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H


#define TEX_SIZE  512
#define TEX_SIZEf 512.f

struct {
    GLuint id;
    GLuint positionLocation, texCoordLocation, colorLocation, infoLocation, samplerLocation;
} ftProgram;

extern GLuint dummyVAO;


int font_sizes[] = {12, 16, 32, 64};

typedef struct{
    char letter;
    Vec2 coords1, coords2, size, offset, advance;
} Glyph;

FT_Face ftFace;
Glyph *glyphs[4];
unsigned int glyphNum;

#if OS == OS_IOS
#define USE_GPU 0
#else
#define USE_GPU 0
#endif

int initFreetype(){
    const char freeTypeShader[] =
#include "Shaders/letterShader.glsl"
    ;
    ftProgram.id = loadShadersByText(freeTypeShader);
    ftProgram.positionLocation = glGetAttribLocation(ftProgram.id, "a_position");
    ftProgram.texCoordLocation = glGetAttribLocation(ftProgram.id, "a_texCoords");
    ftProgram.colorLocation = glGetAttribLocation(ftProgram.id, "a_color");
    ftProgram.infoLocation = glGetUniformLocation(ftProgram.id, "u_info");
    ftProgram.samplerLocation = glGetUniformLocation(ftProgram.id, "u_sampler");
    return 0;
}

int cleanupFreetype(){
    // @Incomplete: must release freetype shader program
    return 0;
}

int loadFont(char *path){
    FT_Library ft;
    if(FT_Init_FreeType(&ft)) {
        fprintf(stderr, "Freetype: Could not init freetype library\n");
        return 1;
    }
    
    if(FT_New_Face(ft, path, 0, &ftFace)) {
        fprintf(stderr, "Freetype: Could not open font\n");
        return 1;
    }
    
    const char text[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz 0123456789.:,;+-*/=_!?\"#$%&()[]{}<>";
    for(glyphNum=0; text[glyphNum]!='\0'; glyphNum++);
    
    glyphs[0] = (Glyph *)malloc(glyphNum*FONT_SIZES*sizeof(Glyph));
    for(int i=1; i<FONT_SIZES; i++)
        glyphs[i] = glyphs[i-1]+glyphNum;
    
    
    glActiveTexture(GL_TEXTURE1);
    
    GLuint letterTex;
    glGenTextures(1, &letterTex);
    glBindTexture(GL_TEXTURE_2D, letterTex);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
#if USE_GPU
    GlBuffer letterBuffer;
    
    initPositionsColorsCoords(&letterBuffer);
    
    // Initialize framebuffer
    GLuint framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    // Initialize letter texture
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, TEX_SIZE, TEX_SIZE, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
    
    
    // Bind texture
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, letterTex, 0);
    
    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers);
    // ^ This causes an error on ios
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        printf("Freetype: Framebuffer not complete\n");
    
    
    
    // Initialize temporary texture
    GLuint tmpTex;
    glActiveTexture(GL_TEXTURE2);
    glGenTextures(1, &tmpTex);
    glBindTexture(GL_TEXTURE_2D, tmpTex);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glViewport(0, 0, TEX_SIZE, TEX_SIZE);
    glUseProgram(ftProgram.id);
    glUniform1i(ftProgram.samplerLocation, 2);
    glUniform4f(ftProgram.infoLocation, 1.f, 1.f, 0.f, 0.f);
    
    
    
    GLuint ftVAO;
    glGenVertexArrays(1, &ftVAO);
    glBindVertexArray(ftVAO);
    
    glEnableVertexAttribArray(ftProgram.positionLocation);
    glEnableVertexAttribArray(ftProgram.texCoordLocation);
    glEnableVertexAttribArray(ftProgram.colorLocation);
    
    glBindBuffer(GL_ARRAY_BUFFER, letterBuffer.positions);
    glVertexAttribPointer(ftProgram.positionLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, letterBuffer.coords);
    glVertexAttribPointer(ftProgram.texCoordLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, letterBuffer.colors);
    glVertexAttribPointer(ftProgram.colorLocation, 4, GL_FLOAT, GL_FALSE, 0, 0);
    
    glBindVertexArray(0);
    
    GLfloat colors[4][4] = {
        {1.f, 0.f, 0.f, 1.f},
        {1.f, 0.f, 0.f, 1.f},
        {1.f, 0.f, 0.f, 1.f},
        {1.f, 0.f, 0.f, 1.f}
    };
    
#else
    uint8 *image = (uint8 *)malloc(TEX_SIZE*TEX_SIZE*sizeof(uint8));
    int ix = 0, iy = 0;
#endif
    
    float sx = 1.f;
    float sy = 1.f;
    float x = 1.f*sx;
    float y = 1.f*sy;
    
    
    float maxY = 0.f;
    for(int j=0; j<FONT_SIZES; j++){
        FT_Set_Pixel_Sizes(ftFace, 0, font_sizes[j]);
        if(FT_Load_Char(ftFace, 'X', FT_LOAD_RENDER)) {
            fprintf(stderr, "Freetype: Could not load character 'X'\n");
            return 1;
        }
        
        FT_GlyphSlot g = ftFace->glyph;
        
        int i = 0;
        for(const char *p=text; *p; p++, i++){
            if(FT_Load_Char(ftFace, *p, FT_LOAD_RENDER))
                continue;
            
            float x2 = 2.f*(x+g->bitmap_left*sx)/TEX_SIZEf-1.f;
            float w = 2.f*g->bitmap.width*sx/TEX_SIZEf;
            while(x2+w > 1.f){
                y = maxY+1.f;
                x = 1.f*sx;
                x2 = 2.f*(x+g->bitmap_left*sx)/TEX_SIZEf-1.f;
                w = 2.f*g->bitmap.width*sx/TEX_SIZEf;
#if !USE_GPU
                ix = 0;
                iy = maxY+1;
#endif
            }
            float y2 = -2.f*y/TEX_SIZEf+1.f;
            float h = -2.f*(g->bitmap.rows*sy/TEX_SIZEf);
            
#if USE_GPU
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, g->bitmap.width, g->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);
            
            GLfloat positions[4][3] = {
                {x2,   y2,   0.f},
                {x2+w, y2,   0.f},
                {x2,   y2+h, 0.f},
                {x2+w, y2+h, 0.f}
            };
            GLfloat coords[4][2] = {
                {0.f, 0.f},
                {1.f, 0.f},
                {0.f, 1.f},
                {1.f, 1.f}
            };
            
            glBindBuffer(GL_ARRAY_BUFFER, letterBuffer.positions);
            glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, letterBuffer.coords);
            glBufferData(GL_ARRAY_BUFFER, sizeof(coords), coords, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, letterBuffer.colors);
            glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_DYNAMIC_DRAW);
            
            glBindVertexArray(ftVAO);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
#else
            ix = TEX_SIZEf*(x2+1.f)/2.f;
            iy = TEX_SIZEf*(-y2+1.f)/2.f;
            if(ix < 0)
                ix = 0;
            if(ix >= TEX_SIZE)
                ix = TEX_SIZE-1;
            if(iy < 0)
                iy = 0;
            if(iy >= TEX_SIZE)
                iy = TEX_SIZE-1;
            for(int i=0; i<g->bitmap.rows; i++){
                for(int j=0; j<g->bitmap.width; j++){
                    image[(TEX_SIZE-1-(iy+i))*TEX_SIZE+(ix+j)] = g->bitmap.buffer[i*g->bitmap.width+j];
                }
            }
#endif
            
            x += (g->advance.x) * sx / 64.f;
            y += (g->advance.y) * sy / 64.f;
            
            if(y+g->bitmap.rows*sy > maxY)
                maxY = y+g->bitmap.rows*sy;
            glyphs[j][i] = {text[i], 0.5f*Vec2(x2+1.f, y2+1.f), 0.5f*Vec2(x2+1.f, y2+1.f)+0.5f*Vec2(w, h), Vec2(g->bitmap.width, -(float)g->bitmap.rows), Vec2(g->bitmap_left*sx, g->bitmap_top*sy), Vec2(g->advance.x/64.f*sx, g->advance.y/64.f*sy)};
        }
        
        y = maxY+1.f;
        x = 1.f*sx;
    }
    
#if USE_GPU
    glDisableVertexAttribArray(ftProgram.positionLocation);
    glDisableVertexAttribArray(ftProgram.texCoordLocation);
    glDisableVertexAttribArray(ftProgram.colorLocation);
    
    glDeleteVertexArrays(0, &ftVAO);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &framebuffer);
    
    cleanupPositionsColorsCoords(&letterBuffer);
#else
#if OS != OS_WASM
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, TEX_SIZE, TEX_SIZE, 0, GL_RED, GL_UNSIGNED_BYTE, image);
#else
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, TEX_SIZE, TEX_SIZE, 0, GL_ALPHA, GL_UNSIGNED_BYTE, image);
#endif
    free(image);
#endif
    
    printf("Initialized freetype\n");
    return 0;
}

#define MAX_LOG_LENGTH 1000
char printLog[MAX_LOG_LENGTH] = "";

void _cleanScreenLog(){
    printLog[0] = '\0';
}
void _screenLog(const char *ftext, ...){
    va_list args;
    va_start(args, ftext);
    char text[500];
    vsprintf(text, ftext, args);
    
    // Update log
    int textLength;
    for(textLength=0; text[textLength]!='\0'; textLength++);
    if(textLength >= MAX_LOG_LENGTH)
        return;
    
    int logLength;
    int lineNum;
    for(logLength=0; printLog[logLength]!='\0'; logLength++);
    if(logLength > 0){
        logLength++;
        lineNum = 1;
        for(int i=0; i<logLength; i++)
            if(printLog[i] == '\n')
                lineNum++;
    }else
        lineNum = 0;
    while(logLength+textLength >= MAX_LOG_LENGTH || lineNum > 20){
        int lineLength;
        for(lineLength=0; printLog[lineLength]!='\n' && lineLength<logLength; lineLength++);
        if(lineLength == logLength)
            logLength = 0;
        else{
            lineLength++; // Also count the \n!
            for(int i=lineLength; printLog[i-1]!='\0'; i++){
                printLog[i-lineLength] = printLog[i];
            }
            for(logLength=0; printLog[logLength]!='\0'; logLength++);
            logLength++; // Also count the \0!
            lineNum = 1;
            for(int i=0; i<logLength; i++)
                if(printLog[i] == '\n')
                    lineNum++;
        }
    }
    if(logLength > 0)
        printLog[logLength-1] = '\n';
    for(int i=0, j=logLength; i<=textLength; i++, j++) // Also copy the \0
        printLog[j] = text[i];
    //logLength += textLength+1; // Not counting the \0 !!!1
}

int textVertNum(char *printLog){
    // Render log
    // Count the total squares
    int logLength;
    for(logLength=0; printLog[logLength]!='\0'; logLength++);
    
    int squares = 0;
    for(int i=0; i<logLength; i++){
        for(int j=0; j<glyphNum; j++)
            if(printLog[i] == glyphs[0][j].letter){
                if(glyphs[0][j].size.x > 0)
                    squares += 6;
                break;
            }
    }
    return squares;
}
int textVertNum(String &s){
    // Render log
    // Count the total squares
    
    int squares = 0;
    for(int i=0; i<s.length; i++){
        for(int j=0; j<glyphNum; j++)
            if(s.text[i] == glyphs[0][j].letter){
                if(glyphs[0][j].size.x > 0)
                    squares += 6;
                break;
            }
    }
    return squares;
}

/*void renderText(float x, float y, float size, char *printLog){
    // Render log
    // Count the total squares
    float x0 = x;
    int logLength;
    for(logLength=0; printLog[logLength]!='\0'; logLength++);
    
    int squares = 0;
    for(int i=0; i<logLength; i++){
        for(int j=0; j<glyphNum; j++)
            if(printLog[i] == glyphs[j].letter){
                if(glyphs[j].size.x > 0)
                    squares += 6;
                break;
            }
    }
    
    // Start writing to buffers
    Vec3 *pos = (Vec3 *)malloc(squares*sizeof(Vec3));
    RgbaColor *col = (RgbaColor *)malloc(squares*sizeof(RgbaColor));
    Vec2 *coords = (Vec2 *)malloc(squares*sizeof(Vec2));
    y += -size*0.05f;
    int k = 0, l = 0, m = 0;
    for(int i=0; i<logLength; i++){
        if(printLog[i] == '\n'){
            x = x0;
            y -= size*0.05f;
        }
        for(int j=0; j<glyphNum; j++)
            if(printLog[i] == glyphs[j].letter){
                Glyph &g = glyphs[j];
                if(g.size.x > 0){
                    if(x+g.offset.x+g.size.x > 500.f){
                        x = x0;
                        y -= size*0.05f;
                    }
                    float x1 = x+size*g.offset.x, y1 = y+size*g.offset.y;
                    float x2 = x1+size*g.size.x, y2 = y1+size*g.size.y;
                    pos[k++] = {x1, y1, -1.f};
                    pos[k++] = {x2, y1, -1.f};
                    pos[k++] = {x1, y2, -1.f};
                    pos[k++] = {x2, y1, -1.f};
                    pos[k++] = {x1, y2, -1.f};
                    pos[k++] = {x2, y2, -1.f};
                    col[l++] = {0, 30, 0, 155};
                    col[l++] = {0, 30, 0, 155};
                    col[l++] = {0, 30, 0, 155};
                    col[l++] = {0, 30, 0, 155};
                    col[l++] = {0, 30, 0, 155};
                    col[l++] = {0, 30, 0, 155};
                    coords[m++] = {g.coords1.x, g.coords1.y};
                    coords[m++] = {g.coords2.x, g.coords1.y};
                    coords[m++] = {g.coords1.x, g.coords2.y};
                    coords[m++] = {g.coords2.x, g.coords1.y};
                    coords[m++] = {g.coords1.x, g.coords2.y};
                    coords[m++] = {g.coords2.x, g.coords2.y};
                }
                
                x += size*g.advance.x;
                y -= size*g.advance.y;
                break;
            }
    }
    setVectorDynamicBuffer(letterPositionBuffer, (GLfloat *)pos, 3*k);
    setVectorDynamicBuffer(letterColorBuffer, (GLubyte *)col, 4*l);
    setVectorDynamicBuffer(letterCoordBuffer, (GLfloat *)coords, 2*m);
    free(pos);
    free(col);
    free(coords);
    letterVertNum = squares;
}*/

float renderTextLength(uint8 size, String &s){
    float x = 0.f;
    float maxX = 0.f;
    
    for(int i=0; i<s.length; i++){
        if(s.text[i] == '\n'){
            if(x > maxX)
                maxX = x;
            x = 0.f;
        }
        for(int j=0; j<glyphNum; j++)
            if(s.text[i] == glyphs[size][j].letter){
                Glyph &g = glyphs[size][j];
                
                x += g.advance.x;
                break;
            }
    }
    if(x > maxX)
        maxX = x;
    
    return maxX;
}
float renderTextLength(uint8 size, String &s, int length){
    float x = 0.f;
    float maxX = 0.f;
    
    for(int i=0; i<s.length && i<length; i++){
        if(s.text[i] == '\n'){
            if(x > maxX)
                maxX = x;
            x = 0.f;
        }
        for(int j=0; j<glyphNum; j++)
            if(s.text[i] == glyphs[size][j].letter){
                Glyph &g = glyphs[size][j];
                
                x += g.advance.x;
                break;
            }
    }
    if(x > maxX)
        maxX = x;
    
    return maxX;
}
unsigned int renderText(float x, float y, uint8 size, String &s, Vec3 *pos, RgbaColor *col, Vec2* coords, float *width, float *height, RgbaColor color){
    // Render log
    // Count the total squares
    float x0 = x;
    float y0 = y;
    
    float maxX = x;
    
    int squares = 0;
    for(int i=0; i<s.length; i++){
        for(int j=0; j<glyphNum; j++)
            if(s.text[i] == glyphs[size][j].letter){
                if(glyphs[size][j].size.x > 0)
                    squares += 6;
                break;
            }
    }
    
    y += -font_sizes[size];
    int k = 0, l = 0, m = 0;
    for(int i=0; i<s.length; i++){
        if(s.text[i] == '\n'){
            if(x > maxX)
                maxX = x;
            x = x0;
            y -= font_sizes[size];
        }
        for(int j=0; j<glyphNum; j++)
            if(s.text[i] == glyphs[size][j].letter){
                Glyph &g = glyphs[size][j];
                if(g.size.x > 0){
                    /*if(x+g.offset.x+g.size.x > 500.f){
                        if(x > maxX)
                            maxX = x;
                        x = x0;
                        y -= font_sizes[size];
                    }*/
                    float x1 = x+g.offset.x, y1 = y+g.offset.y;
                    float x2 = x1+g.size.x, y2 = y1+g.size.y;
                    pos[k++] = {x1, y1, -1.f};
                    pos[k++] = {x2, y1, -1.f};
                    pos[k++] = {x1, y2, -1.f};
                    pos[k++] = {x2, y1, -1.f};
                    pos[k++] = {x1, y2, -1.f};
                    pos[k++] = {x2, y2, -1.f};
                    col[l++] = color;
                    col[l++] = color;
                    col[l++] = color;
                    col[l++] = color;
                    col[l++] = color;
                    col[l++] = color;
                    coords[m++] = {g.coords1.x, g.coords1.y};
                    coords[m++] = {g.coords2.x, g.coords1.y};
                    coords[m++] = {g.coords1.x, g.coords2.y};
                    coords[m++] = {g.coords2.x, g.coords1.y};
                    coords[m++] = {g.coords1.x, g.coords2.y};
                    coords[m++] = {g.coords2.x, g.coords2.y};
                }
                
                x += g.advance.x;
                y -= g.advance.y;
                break;
            }
    }
    if(x > maxX)
        maxX = x;
    
    if(width != nullptr)
        *width = maxX-x0;
    if(height != nullptr)
        *height = y0-y+font_sizes[size];
    return k;
}

unsigned int renderText(float x, float y, uint8 size, String &s, Vec3 *pos, RgbaColor *col, Vec2* coords, float *width, float *height){
    return renderText(x, y, size, s, pos, col, coords, width, height, {0, 0, 0, 155});
}
