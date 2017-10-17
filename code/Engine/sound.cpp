#include <stdint.h>
#include <thread>

#include "engine.hpp"
#include "sound.hpp"

#if OS != OS_IOS && 0

ALCdevice *device;
ALCcontext *context;

Array<Sound> sounds;

void checkOpenALErrors(){
    ALCenum error;
    
    error = alGetError();
    if(error != AL_NO_ERROR){
        switch(error){
            case AL_INVALID_NAME: fprintf(stderr, "OpenAL: invalid name\n"); return;
            case AL_INVALID_ENUM: fprintf(stderr, "OpenAL: invalid enum\n"); return;
            case AL_INVALID_VALUE: fprintf(stderr, "OpenAL: invalid value\n"); return;
            case AL_INVALID_OPERATION: fprintf(stderr, "OponAL: invalid operation\n"); return;
            case AL_OUT_OF_MEMORY: fprintf(stderr, "OpenAL: out of memory\n"); return;
            default: fprintf(stderr, "OpenAL: unknown (code = %i)\n", error); return;
        }
    }
}
bool loadWavFile(const char *filename, ALuint* buffer, ALsizei* size, ALsizei* frequency, ALenum* format);

Sound::Sound(const char *filename){
    alGenSources((ALuint)1, &source);
    checkOpenALErrors();
    
    alSourcef(source, AL_PITCH, 1);
    checkOpenALErrors();
    alSourcef(source, AL_GAIN, 1);
    checkOpenALErrors();
    alSource3f(source, AL_POSITION, 0, 0, 0);
    checkOpenALErrors();
    alSource3f(source, AL_VELOCITY, 0, 0, 0);
    checkOpenALErrors();
    alSourcei(source, AL_LOOPING, AL_FALSE);
    checkOpenALErrors();
    
    alGenBuffers((ALuint)1, &buffer);
    checkOpenALErrors();
    
    ALsizei size, freq;
    ALenum format;
    fprintf(stderr, "Reading %s\n", filename);
    loadWavFile(filename, &buffer, &size, &freq, &format);
    checkOpenALErrors();
    
    alSourcei(source, AL_BUFFER, buffer);
    checkOpenALErrors();
    
    sounds.push(*this);
}
Sound::Sound(const char *filename, float volume){
    alGenSources((ALuint)1, &source);
    checkOpenALErrors();
    
    alSourcef(source, AL_PITCH, 1);
    checkOpenALErrors();
    alSourcef(source, AL_GAIN, volume);
    checkOpenALErrors();
    alSource3f(source, AL_POSITION, 0, 0, 0);
    checkOpenALErrors();
    alSource3f(source, AL_VELOCITY, 0, 0, 0);
    checkOpenALErrors();
    alSourcei(source, AL_LOOPING, AL_FALSE);
    checkOpenALErrors();
    
    alGenBuffers((ALuint)1, &buffer);
    checkOpenALErrors();
    
    ALsizei size, freq;
    ALenum format;
    fprintf(stderr, "Reading %s\n", filename);
    loadWavFile(filename, &buffer, &size, &freq, &format);
    checkOpenALErrors();
    
    alSourcei(source, AL_BUFFER, buffer);
    checkOpenALErrors();
    
    sounds.push(*this);
}
void Sound::setVolume(float volume){
    alSourcef(source, AL_GAIN, volume);
}
void Sound::play(){
    alSourcePlay(source);
}
void Sound::pause(){
    alSourcePause(source);
}
void Sound::stop(){
    alSourceStop(source);
}
void Sound::update(){
    ALint source_state;
    alGetSourcei(source, AL_SOURCE_STATE, &source_state);
    if(source_state == AL_STOPPED){
        free();
    }
}

void Sound::free(){
    alDeleteSources(1, &source);
    alDeleteBuffers(1, &buffer);
}


void initSound(){
    sounds = Array<Sound>();
    
    device = alcOpenDevice(NULL);
    if(!device){
        printf("Couldn't found sound device!\n");
        return;
    }
    
    context = alcCreateContext(device, NULL);
    if(!alcMakeContextCurrent(context)){
        printf("Couldn't create OpenAL Context!\n");
        return;
    }
    
    ALfloat listenerOri[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };
    
    alListener3f(AL_POSITION, 0, 0, 1.0f);
    checkOpenALErrors();
    alListener3f(AL_VELOCITY, 0, 0, 0);
    checkOpenALErrors();
    alListenerfv(AL_ORIENTATION, listenerOri);
    checkOpenALErrors();
    
    //cleanupSound();
}

void updateSounds(){
    for(int i=0; i<sounds.size; i++)
        sounds[i].update();
}

void pauseSounds(){
    for(int i=0; i<sounds.size; i++)
        sounds[i].pause();
}
void playSounds(){
    for(int i=0; i<sounds.size; i++)
        sounds[i].play();
}
void toggleSounds(bool on){
    if(on)
        for(int i=0; i<sounds.size; i++)
            sounds[i].setVolume(1.f);
    else
        for(int i=0; i<sounds.size; i++)
            sounds[i].setVolume(0.f);
}

void cleanupSound(){
    for(int i=0; i<sounds.size; i++){
        sounds[i].stop();
        sounds[i].free();
    }
    device = alcGetContextsDevice(context);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(context);
    alcCloseDevice(device);
}


// Below is code from http://www.dunsanyinteractive.com/blogs/oliver/?p=72

/*
 * Struct that holds the RIFF data of the Wave file.
 * The RIFF data is the meta data information that holds,
 * the ID, size and format of the wave file
 */
struct RIFF_Header {
    char chunkID[4];
    int32_t chunkSize; //size not including chunkSize or chunkID
    char format[4];
};

/*
 * Struct to hold fmt subchunk data for WAVE files.
 */
struct WAVE_Format {
    char subChunkID[4];
    int32_t subChunkSize;
    int16_t audioFormat;
    int16_t numChannels;
    int32_t sampleRate;
    int32_t byteRate;
    int16_t blockAlign;
    int16_t bitsPerSample;
};

/*
 * Struct to hold the data of the wave file
 */
struct WAVE_Data {
    char subChunkID[4]; //should contain the word data
    int32_t subChunk2Size; //Stores the size of the data block
};

/*
 * Load wave file function. No need for ALUT with this
 */
bool loadWavFile(const char *filename, ALuint* buffer, ALsizei* size, ALsizei* frequency, ALenum* format) {
    //Local Declarations
    FILE* soundFile = NULL;
    WAVE_Format wave_format;
    RIFF_Header riff_header;
    WAVE_Data wave_data;
    unsigned char* data;
    
    soundFile = fopen(filename, "rb");
    if (!soundFile){
        //our catch statement for if we throw a string
        fprintf(stderr, "%s : trying to load %s\n", filename, filename);
        //clean up memory if wave loading fails
        if (soundFile != NULL)
            fclose(soundFile);
        //return false to indicate the failure to load wave
        return false;
    }
    
    // Read in the first chunk into the struct
    fread(&riff_header, sizeof(RIFF_Header), 1, soundFile);
    
    //check for RIFF and WAVE tag in memeory
    if ((riff_header.chunkID[0] != 'R' ||
         riff_header.chunkID[1] != 'I' ||
         riff_header.chunkID[2] != 'F' ||
         riff_header.chunkID[3] != 'F') ||
        (riff_header.format[0] != 'W' ||
         riff_header.format[1] != 'A' ||
         riff_header.format[2] != 'V' ||
         riff_header.format[3] != 'E')){
            printf("%c%c%c%c - %c%c%c%c (%lu)\n", riff_header.chunkID[0], riff_header.chunkID[1], riff_header.chunkID[2], riff_header.chunkID[3], riff_header.format[0], riff_header.format[1], riff_header.format[2], riff_header.format[3], sizeof(long));
            //our catch statement for if we throw a string
            fprintf(stderr, "Invalid RIFF or HEADER : trying to load %s\n", filename);
            //clean up memory if wave loading fails
            if (soundFile != NULL)
                fclose(soundFile);
            //return false to indicate the failure to load wave
            return false;
        }
    
    //Read in the 2nd chunk for the wave info
    fread(&wave_format, sizeof(WAVE_Format), 1, soundFile);
    //check for fmt tag in memory
    if (wave_format.subChunkID[0] != 'f' ||
        wave_format.subChunkID[1] != 'm' ||
        wave_format.subChunkID[2] != 't' ||
        wave_format.subChunkID[3] != ' '){
        //our catch statement for if we throw a string
        fprintf(stderr, "Invalid wave format : trying to load %s\n", filename);
        //clean up memory if wave loading fails
        if (soundFile != NULL)
            fclose(soundFile);
        //return false to indicate the failure to load wave
        return false;
    }
    
    //check for extra parameters;
    if (wave_format.subChunkSize > 16)
        fseek(soundFile, sizeof(short), SEEK_CUR);
    
    //Read in the the last byte of data before the sound file
    fread(&wave_data, sizeof(WAVE_Data), 1, soundFile);
    //check for data tag in memory
    if (wave_data.subChunkID[0] != 'd' ||
        wave_data.subChunkID[1] != 'a' ||
        wave_data.subChunkID[2] != 't' ||
        wave_data.subChunkID[3] != 'a'){
        //our catch statement for if we throw a string
        fprintf(stderr, "Invalid data header : trying to load %s\n", filename);
        //clean up memory if wave loading fails
        if (soundFile != NULL)
            fclose(soundFile);
        //return false to indicate the failure to load wave
        return false;
    }
    
    //Allocate memory for data
    data = new unsigned char[wave_data.subChunk2Size];
    
    // Read in the sound data into the soundData variable
    if (!fread(data, wave_data.subChunk2Size, 1, soundFile)){
        //our catch statement for if we throw a string
        fprintf(stderr, "Error loading wave data into the struct! : trying to load %s\n", filename);
        //clean up memory if wave loading fails
        if (soundFile != NULL)
            fclose(soundFile);
        //return false to indicate the failure to load wave
        return false;
    }
    
    //Now we set the variables that we passed in with the
    //data from the structs
    *size = wave_data.subChunk2Size;
    *frequency = wave_format.sampleRate;
    //The format is worked out by looking at the number of
    //channels and the bits per sample.
    if (wave_format.numChannels == 1) {
        if (wave_format.bitsPerSample == 8 )
            *format = AL_FORMAT_MONO8;
        else if (wave_format.bitsPerSample == 16)
            *format = AL_FORMAT_MONO16;
    } else if (wave_format.numChannels == 2) {
        if (wave_format.bitsPerSample == 8 )
            *format = AL_FORMAT_STEREO8;
        else if (wave_format.bitsPerSample == 16)
            *format = AL_FORMAT_STEREO16;
    }
    //create our openAL buffer and check for success
    alGenBuffers(1, buffer);
    checkOpenALErrors();
    //now we put our data into the openAL buffer and
    //check for success
    alBufferData(*buffer, *format, (void*)data,
                 *size, *frequency);
    checkOpenALErrors();
    //clean up and return true if successful
    fclose(soundFile);
    return true;
}

#else

void Sound::play(){}
void Sound::update(){}
void Sound::pause(){}
void Sound::stop(){}
void Sound::free(){}
void Sound::setVolume(float volume){}

void initSound(){}
void updateSounds(){}
void pauseSounds(){}
void playSounds(){}
void toggleSounds(bool on){}
void cleanupSound(){}

#endif
