#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <QObject>
#include <portaudio.h>
#include "music.h"

#include "defines.h"


class LooperInfos;

struct DeviceInfo {
    int device_id;
    std::string name;
    int maxInputChannels;
    int maxOutputChannels;
    double defaultLowInputLatency;
    double defaultLowOutputLatency;
    double defaultHighInputLatency;
    double defaultHighOutputLatency;
    double defaultSampleRate;
    bool is_default;
};

class SoundManager : public QObject
{
    Q_OBJECT
public:
    explicit SoundManager(QObject *parent = nullptr);
    virtual ~SoundManager();
    void Initialize();
    void Terminate();
    std::vector<DeviceInfo> getDevicesInfo();
    int myMemberCallback(const void *input, void *output, unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags);

    void OpenStream(DeviceInfo* dev);
    void CloseStream();

    void Music_load(int music_id, std::string filename);
    void Music_loadInfiniteLooper(int music_id, std::string filename);
    void Music_seekFrame(int music_id, unsigned long sample, bool wait);
    void Music_unload(int music_id);
    void Music_unloadInfiniteLooper(int music_id);
    void Music_play(int music_id);
    void Music_stop(int music_id);
    void Music_pause(int music_id);
    bool Music_isPaused(int music_id);
    unsigned long Music_getSampleRate(int music_id);
    float Music_getGain(int music_id);
    void Music_setGain(int music_id, float gain);
    float Music_getGlobalGain();
    void Music_setGlobalGain(float gain);
    void Music_setBalance(int music_id, float balance);
    float Music_getBalance(int music_id);
    bool Music_isLoaded(int music_id);
    bool Music_isLooperLoaded(int music_id);
    unsigned long Music_getPositionFrame(int music_id);
    unsigned long Music_getPositionBisFrame(int music_id);
    unsigned long Music_getStoppedPositionFrame(int music_id);
    unsigned long Music_getLengthFrame(int music_id);
    unsigned long Music_getLengthSample(int music_id);
    float* Music_getData(int music_id);
    float* Music_getVisualData(int music_id, int width); // @todo

    void Music_setRepeat(int music_id, bool repeat);
    bool Music_getRepeat(int music_id);

signals:

public slots:

private:
    bool is_initialized;
    bool is_stream_open;
    
public:
    PaStream* stream;
    Music array_music[NUM_MUSICS];
    float global_gain = 1.f;
};


#endif // SOUNDMANAGER_H
