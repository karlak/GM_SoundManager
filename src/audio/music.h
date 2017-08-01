#ifndef MUSIC_H
#define MUSIC_H

#include "defines.h"
#include <vector>
#include <map>

#define _USE_MATH_DEFINES
#include <math.h>

class SoundManager;


struct LooperElement {
    float score, time1, time2;
    long sampleFrom, sampleTo, length;
    bool last = false; 
};

class LooperInfos {
public:
    LooperInfos(const char* filename);
    inline LooperElement* get_jump(long current_sample);
    inline void exit_jump();
    long current_jump_progression;
    long last_jump_position;
private:
    std::vector< LooperElement > elems;
    std::map< long, std::vector< long > > map_sample_from_index;
    LooperElement* current_jump;
    
};

class Music
{
public:
    Music();
    ~Music();
    void load(const char* filename);
    void loadInfiniteLooper(const char* filename);
    void unload();
    void unloadInfiniteLooper();
    void stop(bool wait = false);
    void setPaused(bool pause);
    //void setRepeat(bool repeat);

    void writeData(float* out, unsigned long frameCount, SoundManager* data);
    void update_gain();
    void update_balance();

    //
    // Modified by Program whenever
    bool loadedInfiniteLooper;
    LooperInfos* looperInfos;
    bool repeat;
    bool paused;
    float desired_gain;
    float desired_balance;
    float paused_gain;

    // Modified by Program, only after ensuring music is stopped
    bool loaded;
    unsigned long long nbSamples;
    long sampleRate_file;
    long sampleRate;
    int nbChannels;
    float* data;

    // Modified by Program, then callback in response
    long order_seek;
    bool order_stop;

    // Modified by Callback only, or after ensuring music is stopped
    unsigned long position;
    unsigned long stopped_position; // position when stop() is called
    float gain;
    float gain_log;
    float balance;
    float balance_left_gain;
    float balance_right_gain;
};

#endif // MUSIC_H
