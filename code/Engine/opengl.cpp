#include <fstream>
#include <vector>
#include <string>

#include "engine.hpp"
#include "opengl.hpp"
#include "freetype.hpp"

#if OS != OS_WASM
#include "Include/stb_image/stb_image_aug.h"
#else
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#endif

GLuint dummyVAO;

Vec2i windowSize;

bool initEngineOpenGL(){
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#if !OPENGL_ES
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.1);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
#endif
    
    glEnable(GL_SCISSOR_TEST);
    
#if OS != OS_WASM
    glGenVertexArrays(1, &dummyVAO);
#endif
    
    // glActiveTexture(GL_TEXTURE0);
    // loadTexture(dTexture, (resourcePath()+"textures.png").c_str());
    
    return true;
}

bool cleanupEngineOpenGL(){
    // @Incomplete: Do things
    return true;
}

void setVectorStaticBuffer(GLuint buffer, GLfloat *vert, unsigned int size){
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, size*sizeof(GLfloat), vert, GL_STATIC_DRAW);
}
void setVectorDynamicBuffer(GLuint buffer, GLfloat *vert, unsigned int size){
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, size*sizeof(GLfloat), vert, GL_DYNAMIC_DRAW);
}
void setVectorStaticBuffer(GLuint buffer, GLubyte *vert, unsigned int size){
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, size*sizeof(GLubyte), vert, GL_STATIC_DRAW);
}
void setVectorDynamicBuffer(GLuint buffer, GLubyte *vert, unsigned int size){
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, size*sizeof(GLubyte), vert, GL_DYNAMIC_DRAW);
}
void setVectorStaticBuffer(GLuint buffer, Vertex_PNC *vert, unsigned int size){
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, size*sizeof(Vertex_PNC), vert, GL_STATIC_DRAW);
}
void setVectorDynamicBuffer(GLuint buffer, Vertex_PC *vert, unsigned int size){
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, size*sizeof(Vertex_PC), vert, GL_DYNAMIC_DRAW);
}
void setVectorStreamBuffer(GLuint buffer, Vertex_PC *vert, unsigned int size){
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, size*sizeof(Vertex_PC), vert, GL_STREAM_DRAW);
}
void setVectorDynamicBuffer(GLuint buffer, Vertex_PT *vert, unsigned int size){
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, size*sizeof(Vertex_PT), vert, GL_DYNAMIC_DRAW);
}

void _checkOpenGLError(int line, const char *file){
    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR){
        printf("[%s:%i] OpenGL Error: ", file, line);
        switch(err){
            case GL_INVALID_ENUM:
                printf("GL_INVALID_ENUM");
                break;
            case GL_INVALID_VALUE:
                printf("GL_INVALID_VALUE");
                break;
            case GL_INVALID_OPERATION:
                printf("GL_INVALID_OPERATION");
                break;
#ifdef GL_STACK_OVERFLOW
            case GL_STACK_OVERFLOW:
                printf("GL_STACK_OVERFLOW");
                break;
#endif
#ifdef GL_STACK_UNDERFLOW
            case GL_STACK_UNDERFLOW:
                printf("GL_STACK_UNDERFLOW");
                break;
#endif
            case GL_OUT_OF_MEMORY:
                printf("GL_OUT_OF_MEMORY");
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                printf("GL_INVALID_FRAMEBUFFER_OPERATION");
                break;
#ifdef GL_TABLE_TOO_LARGE
            case GL_TABLE_TOO_LARGE:
                printf("GL_TABLE_TOO_LARGE");
                break;
#endif
            default:
                printf("Unknown [%i]", err);
                break;
        }
        printf("\n");
    }
}

GLuint loadShaders(const char *file_path){
    return loadShaders(file_path, "");
}

GLuint loadShaders(const char *file_path, const char *prefix){
    start_temp_alloc();
    
    // Create the shaders
    GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    
    // Read the Vertex Shader code from the file
    /*
     FILE *fp = fopen(file_path, "r");
     if(!fp){
     printf("Couldn't read shader file '%s'\n", file_path);
     return -1;
     }
     fseek(fp, 0, SEEK_END);
     long length = ftell(fp);
     fseek(fp, 0, SEEK_SET);
     String vertexShaderCode((u32)(strlen(prefix)+30+length), temporaryStorage.allocator);
     String fragmentShaderCode((u32)(strlen(prefix)+30+length), temporaryStorage.allocator);
     
     fscanf(fp, "%[^\n]s", vertexShaderCode.text);
     
     fclose(fp);
     */
    
    std::string vertexShaderCode, fragmentShaderCode;
    std::ifstream shaderStream(file_path, std::ios::in);
    if(shaderStream.is_open()){
        std::string line = "";
        if(getline(shaderStream, line)){ // Version number
            vertexShaderCode += "\n"+line;
            fragmentShaderCode += "\n"+line;
        }
        vertexShaderCode += "\n #define VERTEX_SHADER 1\n";
        fragmentShaderCode += "\n #define FRAGMENT_SHADER 1\n";
        vertexShaderCode += prefix;
        fragmentShaderCode += prefix;
        while(getline(shaderStream, line)){
            vertexShaderCode += "\n" + line;
            fragmentShaderCode += "\n" + line;
        }
        shaderStream.close();
    }else{
        printf("Shader: Error opening file\n");
    }
    
    GLint result = GL_FALSE;
    int infoLogLength;
    
    // Compile Vertex Shader
    char const *vertexSourcePointer = vertexShaderCode.c_str();
    glShaderSource(vertexShaderID, 1, &vertexSourcePointer , NULL);
    glCompileShader(vertexShaderID);
    
    // Check Vertex Shader
    glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &result);
    if(result == GL_FALSE){
        glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
        if(infoLogLength > 0){
            String vertexShaderErrorMessage(infoLogLength, temporaryStorage.allocator);
            glGetShaderInfoLog(vertexShaderID, infoLogLength, NULL, vertexShaderErrorMessage.text);
            fprintf(stdout, "Couldn't compile vertex shader:\n%s\n\n", vertexShaderErrorMessage.text);
        }
    }
    
    // Compile Fragment Shader
    char const * fragmentSourcePointer = fragmentShaderCode.c_str();
    glShaderSource(fragmentShaderID, 1, &fragmentSourcePointer , NULL);
    glCompileShader(fragmentShaderID);
    
    // Check Fragment Shader
    glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &result);
    if(result == GL_FALSE){
        glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
        if(infoLogLength > 0){
            String fragmentShaderErrorMessage(infoLogLength, temporaryStorage.allocator);
            glGetShaderInfoLog(fragmentShaderID, infoLogLength, NULL, fragmentShaderErrorMessage.text);
            fprintf(stdout, "Couldn't compile fragment shader:\n%s\n\n", fragmentShaderErrorMessage.text);
        }
    }
    
    // Link the program
    //fprintf(stdout, "Linking program\n");
    GLuint programID = glCreateProgram();
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    glLinkProgram(programID);
    
    // Check the program
    glGetProgramiv(programID, GL_LINK_STATUS, &result);
    if(result == GL_FALSE){
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
        if(infoLogLength > 0){
            String programErrorMessage(infoLogLength, temporaryStorage.allocator);
            glGetProgramInfoLog(programID, infoLogLength, NULL, programErrorMessage.text);
            fprintf(stdout, "Couldn't link shader: %s\n", programErrorMessage.text);
            glValidateProgram(programID);
        }
    }
    
    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);
    
    end_temp_alloc();
    return programID;
}

GLuint loadShadersByText(const char *text){
    start_temp_alloc();
    
    String vertexShaderCode(temporaryStorage.allocator);
    String fragmentShaderCode(temporaryStorage.allocator);
    
    // Create the shaders
    GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    
#if !OPENGL_ES
    vertexShaderCode = "#version 120 \n#define VERTEX_SHADER 1\n";
    vertexShaderCode += text;
    fragmentShaderCode = "#version 120 \n#define FRAGMENT_SHADER 1\n";
    fragmentShaderCode += text;
#else
    vertexShaderCode = "#version 100 \n#define VERTEX_SHADER 1\n";
    vertexShaderCode += text;
    fragmentShaderCode = "#version 100 \n#define FRAGMENT_SHADER 1\n";
    fragmentShaderCode += text;
#endif
    
    
    GLint result = GL_FALSE;
    int infoLogLength;
    
    // Compile Vertex Shader
    char const *vertexSourcePointer = vertexShaderCode.text;
    glShaderSource(vertexShaderID, 1, &vertexSourcePointer , NULL);
    glCompileShader(vertexShaderID);
    
    // Check Vertex Shader
    glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &result);
    if(result == GL_FALSE){
        glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
        if(infoLogLength > 0){
            String vertexShaderErrorMessage(infoLogLength, temporaryStorage.allocator);
            glGetShaderInfoLog(vertexShaderID, infoLogLength, NULL, vertexShaderErrorMessage.text);
            fprintf(stdout, "Couldn't compile vertex shader:\n%s\n\n", vertexShaderErrorMessage.text);
        }
    }
    
    // Compile Fragment Shader
    char const * fragmentSourcePointer = fragmentShaderCode.text;
    glShaderSource(fragmentShaderID, 1, &fragmentSourcePointer , NULL);
    glCompileShader(fragmentShaderID);
    
    // Check Fragment Shader
    glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &result);
    if(result == GL_FALSE){
        glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
        if(infoLogLength > 0){
            String fragmentShaderErrorMessage(infoLogLength, temporaryStorage.allocator);
            glGetShaderInfoLog(fragmentShaderID, infoLogLength, NULL, fragmentShaderErrorMessage.text);
            fprintf(stdout, "Couldn't compile fragment shader:\n%s\n\n", fragmentShaderErrorMessage.text);
        }
    }
    
    // Link the program
    //fprintf(stdout, "Linking program\n");
    GLuint programID = glCreateProgram();
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    glLinkProgram(programID);
    
    // Check the program
    glGetProgramiv(programID, GL_LINK_STATUS, &result);
    if(result == GL_FALSE){
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
        if(infoLogLength > 0){
            String programErrorMessage(infoLogLength, temporaryStorage.allocator);
            glGetProgramInfoLog(programID, infoLogLength, NULL, programErrorMessage.text);
            fprintf(stdout, "Couldn't link shader: %s\n", programErrorMessage.text);
            glValidateProgram(programID);
        }
    }
    
    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);
    
    end_temp_alloc();
    return programID;
}

#if OS != OS_WASM
u8 *loadImage(const char *path, int *width, int *height, int *channels){
    return stbi_load(path, width, height, channels, 0);
}
void freeImage(u8 *img){
    stbi_image_free(img);
}
#else
static SDL_Surface* lastLoadedImage = nullptr;
static u8* lastLoadedPixels = nullptr;
u8 *loadImage(const char *path, int *width, int *height, int *channels){
    assert(lastLoadedImage == nullptr);
    SDL_Surface *image;
    if(!(image = IMG_Load(path))){
        printf("Couldn't load image %s\n", path);
        return nullptr;
    }
    *width = image->w;
    *height = image->h;
    *channels = image->format->BytesPerPixel;
    lastLoadedImage = image;
    lastLoadedPixels = (u8 *)image->pixels;
    return (u8 *)image->pixels;
}
void freeImage(u8 *pixels){
    assert(pixels == lastLoadedPixels);
    SDL_FreeSurface(lastLoadedImage);
    lastLoadedImage = nullptr;
    lastLoadedPixels = nullptr;
}
#endif

void loadTexture(GLuint texture, const char *path){
    int width, height, channels;
    u8* image = loadImage(path, &width, &height, &channels);
    
    // Flip texture Y axis
    int i, j;
    for(j = 0; j*2<height; j++){
        int index1 = j * width * channels;
        int index2 = (height - 1 - j) * width * channels;
        for(i=width*channels; i>0; i--){
            u8 temp = image[index1];
            image[index1] = image[index2];
            image[index2] = temp;
            ++index1;
            ++index2;
        }
    }
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    freeImage(image);
}
