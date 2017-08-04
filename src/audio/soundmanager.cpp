#include "soundmanager.h"
#include <time.h>
#include <iostream>
#include <chrono>
#include <thread>

int myStaticCallback(const void *input, void *output, unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData)
{
    return ((SoundManager*)userData)->myMemberCallback(input, output, frameCount, timeInfo, statusFlags);
}


SoundManager::SoundManager(QObject *parent) : QObject(parent)
{
    is_initialized = false;
    is_stream_open = false;
}

SoundManager::~SoundManager()
{
    if(!is_initialized) Terminate();
}

void SoundManager::Initialize()
{
    if (is_initialized)
        return;
    srand((unsigned int)time(NULL));    
    PaError err;
    err = Pa_Initialize();
    if (err != paNoError) {
        std::cout << err << std::endl;
    }
    is_initialized = true;
}

void SoundManager::Terminate()
{
    PaError err;
    if (is_initialized) {
        for (int i = 0; i < NUM_MUSICS; ++i) {
            array_music[i].unload();
        }
        CloseStream();
        err = Pa_Terminate();
        if (err != paNoError) {
            std::cout << err << std::endl;
        }
        is_initialized = false;
        
    }
}

std::vector<DeviceInfo> SoundManager::getDevicesInfo()
{
    int numDevices = Pa_GetDeviceCount();
    if (numDevices < 0) {
        throw (std::to_string(numDevices));
    }
    int default_output_device = Pa_GetDefaultOutputDevice();

    const PaDeviceInfo* deviceInfo;
    int num_output_devices = 0;
    for (int i = 0; i < numDevices; i++)
    {
        deviceInfo = Pa_GetDeviceInfo(i);
        if (deviceInfo->maxOutputChannels > 0) {
            num_output_devices++;
        }
    }
    std::vector<DeviceInfo> d = std::vector<DeviceInfo>(num_output_devices);

    int j = 0;
    for (int i = 0; i<numDevices; i++)
    {
        deviceInfo = Pa_GetDeviceInfo(i);
        if (deviceInfo->maxOutputChannels <= 0) {
            continue;
        }
        PaStreamParameters outputParameters;
        outputParameters.channelCount = 2;
        outputParameters.device = i;
        outputParameters.hostApiSpecificStreamInfo = NULL;
        outputParameters.sampleFormat = paFloat32;
        outputParameters.suggestedLatency = deviceInfo->defaultLowOutputLatency;
        PaError err = Pa_IsFormatSupported( NULL, &outputParameters, SAMPLE_RATE);
        if( err != paFormatIsSupported )
            continue;
        
        
        auto api = Pa_GetHostApiInfo(deviceInfo->hostApi);
        d[j].api = std::string(api->name);
        
        d[j].name = std::string(deviceInfo->name);
        d[j].device_id = i;
        d[j].is_default = (i == default_output_device);

        d[j].defaultSampleRate = deviceInfo->defaultSampleRate;
        d[j].defaultHighInputLatency = deviceInfo->defaultHighInputLatency;
        d[j].defaultHighOutputLatency = deviceInfo->defaultHighOutputLatency;
        d[j].defaultLowInputLatency = deviceInfo->defaultLowInputLatency;
        d[j].defaultLowOutputLatency = deviceInfo->defaultLowOutputLatency;
        d[j].maxInputChannels = deviceInfo->maxInputChannels;
        d[j].maxOutputChannels = deviceInfo->maxOutputChannels;
        j++;
    }
    d.resize(j);
    return d;
}

int SoundManager::myMemberCallback(const void *input, void *output, unsigned long frameCount, const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags)
{
    input; timeInfo; statusFlags;
    memset(output, 0, sizeof(float) * frameCount * 2);

    for (int m = 0; m < NUM_MUSICS; m++) {
        Music* music = &array_music[m];
        music->writeData((float*)output, frameCount, this);
    }


    return paContinue;
}

void SoundManager::OpenStream(DeviceInfo *dev)
{
    if (is_stream_open) {
        CloseStream();
    }

    if (dev == nullptr) {
        auto *me = this;
        PaError err = Pa_OpenDefaultStream(&stream, 0, 2, paFloat32, SAMPLE_RATE, 64, &myStaticCallback, me);
        if (err != paNoError) throw (Pa_GetErrorText(err));
        err = Pa_StartStream(stream);
        if (err != paNoError) throw (Pa_GetErrorText(err));

    }
    else {
        auto *me = this;
        PaStreamParameters outputParameters;

        //bzero(&outputParameters, sizeof(outputParameters)); //not necessary if you are filling in all the fields
        outputParameters.channelCount = 2;
        outputParameters.device = dev->device_id;
        outputParameters.hostApiSpecificStreamInfo = NULL;
        outputParameters.sampleFormat = paFloat32;
        outputParameters.suggestedLatency = dev->defaultLowOutputLatency;


        PaError err = Pa_OpenStream(&stream, NULL, &outputParameters, SAMPLE_RATE, 64,
            paNoFlag, //flags that can be used to define dither, clip settings and more
            &myStaticCallback, //your callback function
            me); //data to be passed to callback. In C++, it is frequently (void *)this
        // @todo: handle errors properly, don't throw exceptions everywhere
        if (err != paNoError) throw std::exception(Pa_GetErrorText(err));
        err = Pa_StartStream(stream);
        if (err != paNoError) throw std::exception(Pa_GetErrorText(err));
    }
    is_stream_open = true;
}

void SoundManager::CloseStream()
{
    if (is_stream_open) {
        Pa_CloseStream(stream);
        is_stream_open = false;
    }
}

void SoundManager::Music_load(int music_id, std::string filename)
{
    array_music[music_id].load(filename.c_str());
}

void SoundManager::Music_loadInfiniteLooper(int music_id, std::string filename)
{
    array_music[music_id].loadInfiniteLooper(filename.c_str());
}

void SoundManager::Music_seekFrame(int music_id, unsigned long sample, bool wait)
{
    array_music[music_id].order_seek = sample * array_music[music_id].nbChannels;
    if (wait) {
        while (array_music[music_id].order_seek != -1)
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); // @check if it works  [Sleep(10);]
    }
}

void SoundManager::Music_unload(int music_id)
{
    array_music[music_id].unload();
}

void SoundManager::Music_unloadInfiniteLooper(int music_id)
{
    array_music[music_id].unloadInfiniteLooper();    
}

void SoundManager::Music_play(int music_id)
{
    array_music[music_id].setPaused(false);    
}

void SoundManager::Music_stop(int music_id)
{
    array_music[music_id].stop(true);    
}

void SoundManager::Music_pause(int music_id)
{
    array_music[music_id].setPaused(true);    
}

bool SoundManager::Music_isPaused(int music_id)
{
    return array_music[music_id].paused;    
}

unsigned long SoundManager::Music_getSampleRate(int music_id)
{
    return array_music[music_id].sampleRate;
}

float SoundManager::Music_getGain(int music_id)
{
    return array_music[music_id].gain_log;    
}

void SoundManager::Music_setGain(int music_id, float gain)
{
    array_music[music_id].desired_gain = gain;
}

float SoundManager::Music_getGlobalGain()
{
    return global_gain;   
}

void SoundManager::Music_setGlobalGain(float gain)
{
    global_gain = gain;
}

void SoundManager::Music_setBalance(int music_id, float balance)
{
    array_music[music_id].desired_balance = balance;    
}

float SoundManager::Music_getBalance(int music_id)
{
    return array_music[music_id].balance;
}

bool SoundManager::Music_isLoaded(int music_id)
{
    return array_music[music_id].loaded;    
}

bool SoundManager::Music_isLooperLoaded(int music_id)
{
    return array_music[music_id].loadedInfiniteLooper;
}

unsigned long SoundManager::Music_getPositionFrame(int music_id)
{
    return array_music[music_id].position / array_music[music_id].nbChannels;
    
}

unsigned long SoundManager::Music_getPositionBisFrame(int music_id)
{
    if (!array_music[music_id].loadedInfiniteLooper)
        return 0;
    auto position = array_music[music_id].position / array_music[music_id].nbChannels;
    auto e = array_music[music_id].looperInfos->get_jump(position);
    if (e) {
        long from = e->sampleFrom;
        long to = e->sampleTo;
        return position - from + to;
    }
    return 0;
}

unsigned long SoundManager::Music_getStoppedPositionFrame(int music_id)
{
    return array_music[music_id].stopped_position / array_music[music_id].nbChannels;    
}

unsigned long SoundManager::Music_getLengthFrame(int music_id)
{
    return array_music[music_id].nbSamples / array_music[music_id].nbChannels;
}

unsigned long SoundManager::Music_getLengthSample(int music_id)
{
    return array_music[music_id].nbSamples;    
}

float *SoundManager::Music_getData(int music_id)
{
    return array_music[music_id].data;
}

void SoundManager::Music_setRepeat(int music_id, bool repeat)
{
    array_music[music_id].repeat = repeat;    
}

bool SoundManager::Music_getRepeat(int music_id)
{
    return array_music[music_id].repeat;    
}




