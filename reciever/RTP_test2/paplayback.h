#ifndef PAPLAYBACK_H
#define PAPLAYBACK_H


#include <portaudio.h>
#include <thread>
#include <chrono>


#include "rtpstreamchannel.h"


//RTPStreamChannel *streamptr;


// Callback data structure
typedef struct {
    float phase;
    float amplitude;
    float frequency;
} SineWaveData;


static int sineWaveCallback(const void* inputBuffer, void* outputBuffer,
                            unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void* userData);



static int streamCallback(const void* inputBuffer, void* outputBuffer,
                            unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void* userData);


class PAPlayback
{
private:
    PaStream *stream;
    PaError err;
public:
    PAPlayback();
    ~PAPlayback();
};

#endif // PAPLAYBACK_H
