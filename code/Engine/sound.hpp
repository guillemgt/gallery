#ifndef engine_sound_h
#define engine_sound_h

#if OS == OS_IOS || OS == OS_WASM
#define NO_SOUND
typedef unsigned int ALuint;
#endif

#if OS ==  OS_MAC
//#include <OpenAL/al.h>
//#include <OpenAL/alc.h>
#include "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks/OpenAL.framework/Versions/A/Headers/al.h"
#include "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks/OpenAL.framework/Versions/A/Headers/alc.h"
#elif OS == OS_LINUX
#include <AL/al.h>
#include <AL/alc.h>
#elif OS == OS_WINDOWS
#include <AL/al.h>
#include <AL/alc.h>
#endif

struct Sound{
    const char *filename;
    ALuint source, buffer;
    
    void play();
    void update();
    void pause();
    void stop();
    void free();
    void setVolume(float volume);
    
    Sound(const char *name);
    Sound(const char *name, float volume);
};

void initSound();
void updateSounds();
void pauseSounds();
void playSounds();
void toggleSounds(bool on);
void cleanupSound();

#endif /* defined(__ta__sound__) */
