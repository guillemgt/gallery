#ifndef engine_os_h
#define engine_os_h
#include "engine.hpp"

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

#if OS == OS_WINDOWS
#include "os_windows.hpp"
#elif OS == OS_MAC
#include "os_osx.hpp"
#elif OS == OS_IOS
#include "os_ios.hpp"
#endif

//struct OsFile;
typedef FILE* OsFile;
typedef u64 OsTime;

OsFile openGameFile(const char *name, const char *mode);
OsFile openUserFile(const char *name, const char *mode);
OsFile openGameFile(String name, const char *mode);
OsFile openUserFile(String name, const char *mode);
void getGameFilePath(const char *name, char *dst);
void getUserFilePath(const char *name, char *dst);
void readFile(void *dest, unsigned int size, unsigned int amount, OsFile fp);
void writeFile(void *dest, unsigned int size, unsigned int amount, OsFile fp);
void closeFile(OsFile);

Array<const char *> filesInGameDirectory(const char *);
void renameGameDirectory(const char *src, const char *dst);
void createGameDirectory(const char *name);
Array<const char *> filesInUserDirectory(const char *);
void renameUserDirectory(const char *src, const char *dst);
void createUserDirectory(const char *name);

void monitorFile(const char *cpath, void (*callback)());
OsTime fileModificationDate(const char *path);

#endif
