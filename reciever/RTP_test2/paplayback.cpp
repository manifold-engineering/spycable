#include <cmath>
#include <chrono>
#include <iostream>

#include "config.h"
#include "paplayback.h"
#include "rtpstreamchannel.h"



SineWaveData data;

extern RTPStreamChannel *streamptr;

PAPlayback::PAPlayback() {
    stream = nullptr;


    data.phase = 0.0f;
    //data.amplitude = 10000000;  // 0.5 amplitude as requested
    data.frequency = 1000.0f;  // 1 kHz sine wave

    std::cout << "Initializing PortAudio..." << std::endl;

    // Initialize PortAudio with error checking
    err = Pa_Initialize();
    if (err != paNoError) {
        std::cerr << "ERROR: PortAudio initialization failed: " << Pa_GetErrorText(err) << std::endl;
        // Add a delay before returning to see error messages

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        return;

    }

    std::cout << "PortAudio initialized successfully." << std::endl;

    // Get default output device info
    int numDevices = Pa_GetDeviceCount();
    std::cout << "Number of audio devices: " << numDevices << std::endl;

    int defaultOutputDevice = Pa_GetDefaultOutputDevice();
    if (defaultOutputDevice == paNoDevice) {
        std::cerr << "ERROR: No default output device found!" << std::endl;
        Pa_Terminate();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        return;
    }


    const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(defaultOutputDevice);
    if (deviceInfo) {
        std::cout << "Using audio device: " << deviceInfo->name << std::endl;
        std::cout << "Max output channels: " << deviceInfo->maxOutputChannels << std::endl;
        std::cout << "Default sample rate: " << deviceInfo->defaultSampleRate << std::endl;
    } else {
        std::cerr << "WARNING: Could not get default output device info!" << std::endl;
    }

    // Force a delay to see output
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));


    // Open audio stream
    std::cout << "Opening audio stream..." << std::endl;

    // Try to open stream with explicit error handling
    err = Pa_OpenDefaultStream(&stream,
                               0,              // No input channels
                               2,              // One output channel
                               paInt32 | paNonInterleaved,      // 32-bit floating point output
                               SAMPLE_RATE,    // Sample rate
                               //FRAMES_PER_BUFFER, // Frames per buffer
                               0, // Frames per buffer
                               //sineWaveCallback,   // Callback function
                               streamCallback,   // Callback function
                               &data);         // User data passed to callback

    if (err != paNoError) {
        std::cerr << "ERROR: PortAudio stream open failed: " << Pa_GetErrorText(err) << std::endl;
        Pa_Terminate();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        return;
    }

    std::cout << "Stream opened successfully." << std::endl;


    // Start audio stream
    std::cout << "Starting playback..." << std::endl;
    err = Pa_StartStream(stream);
    if (err != paNoError) {
        std::cerr << "ERROR: PortAudio stream start failed: " << Pa_GetErrorText(err) << std::endl;
        Pa_CloseStream(stream);
        Pa_Terminate();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        return;
    }

    std::cout << "Playing 1 kHz sine wave at 48 kHz sample rate." << std::endl;
    std::cout << "Sound should be playing now" << std::endl;

    // Simple countdown display
    for (int i = 1; i > 0 /* && keepRunning */; i--) {
        std::cout << "Time remaining: " << i << " seconds..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    std::cout << "Stopping beep..." << std::endl;


}

PAPlayback::~PAPlayback(){
    // Stop stream
    err = Pa_StopStream(stream);
    if (err != paNoError) {
        std::cerr << "WARNING: PortAudio stream stop failed: " << Pa_GetErrorText(err) << std::endl;
    }

    // Close stream
    std::cout << "Closing stream..." << std::endl;
    err = Pa_CloseStream(stream);
    if (err != paNoError) {
        std::cerr << "WARNING: PortAudio stream close failed: " << Pa_GetErrorText(err) << std::endl;
    }

    std::cout << "Terminating PortAudio..." << std::endl;
    // Terminate PortAudio
    Pa_Terminate();

}


// PortAudio callback function
static int sineWaveCallback(const void* inputBuffer, void* outputBuffer,
                            unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void* userData) {
    // Cast output buffer to float pointer
    int32_t** out = static_cast<int32_t**>(outputBuffer);
    SineWaveData* data = static_cast<SineWaveData*>(userData);

    // Avoid unused parameter warnings
    (void)inputBuffer;
    (void)timeInfo;
    (void)statusFlags;

    // Generate sine wave
    for (unsigned long i = 0; i < framesPerBuffer; i++) {
        // Calculate sample

        int32_t amplitude = 1e9;
        out[1][i] = amplitude * sin(2.0f * M_PI * data->phase);
        out[0][i] = 0;

        // Increment phase for next sample
        data->phase += data->frequency / SAMPLE_RATE;

        // Keep phase in [0, 1) range
        if (data->phase >= 1.0f)
            data->phase -= 1.0f;
    }

    //return keepRunning ? paContinue : paComplete;
    return paContinue;
}


static int streamCallback(const void* inputBuffer, void* outputBuffer,
                          unsigned long framesPerBuffer,
                          const PaStreamCallbackTimeInfo* timeInfo,
                          PaStreamCallbackFlags statusFlags,
                          void* userData){

    // Cast output buffer to float pointer
    int32_t** out = static_cast<int32_t**>(outputBuffer);
    SineWaveData* data = static_cast<SineWaveData*>(userData);

    // Avoid unused parameter warnings
    (void)inputBuffer;
    (void)timeInfo;
    (void)statusFlags;

    if (streamptr == NULL){
        // Generate sine wave
        for (unsigned long i = 0; i < framesPerBuffer; i++) {
            // Calculate sample

            int32_t amplitude = 1e9;
            out[1][i] = amplitude * sin(2.0f * M_PI * data->phase);
            out[0][i] = 0;

            // Increment phase for next sample
            data->phase += data->frequency / SAMPLE_RATE;

            // Keep phase in [0, 1) range
            if (data->phase >= 1.0f)
                data->phase -= 1.0f;
        }

    }
    else{


        streamptr->copyAudioTo(out[1], framesPerBuffer);
        streamptr->movePlayHead(framesPerBuffer);

    }

    return paContinue;
}
