#include "../babel/Engine/engine.hpp"
#include "../babel/Engine/os.hpp"
#include "../babel/Engine/os_wasm.hpp"

// File reading & writing

OsFile openGameFile(char *name, const char *mode){
    char dst[MAX_PATH_LENGTH];
    sprintf(dst, "../babel/Bundle/%s", name);
    FILE *fp = fopen(dst, mode);
    return fp;
}
OsFile openUserFile(char *name, const char *mode){
    char dst[MAX_PATH_LENGTH];
    sprintf(dst, "../babel/Bundle/%s", name);
    FILE *fp = fopen(dst, mode);
    return fp;
}

OsFile openGameFile(const char *name, const char *mode){
    return openGameFile((char *)name, mode);
}
OsFile openUserFile(const char *name, const char *mode){
    return openUserFile((char *)name, mode);
}
OsFile openGameFile(String name, const char *mode){
    return openGameFile((char *)name.text, mode);
}
OsFile openUserFile(String name, const char *mode){
    return openUserFile((char *)name.text, mode);
}

void getGameFilePath(char *name, char *dst){
    sprintf(dst, "../babel/Bundle/%s", name);
}
void getUserFilePath(char *name, char *dst){
    sprintf(dst, "../babel/Bundle/%s", name);
}

void readFile(void *dest, unsigned int size, unsigned int amount, OsFile fp){
    fread(dest, size, amount, fp);
}
void writeFile(void *dest, unsigned int size, unsigned int amount, OsFile fp){
    fwrite(dest, size, amount, fp);
}
void closeFile(OsFile fp){
    fclose(fp);
}
