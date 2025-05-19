#include <iostream>
#include <chrono>
#include <thread>


#include "paplayback.h"
#include "test_ringbuffer.h"
#include "rtpserver.h"


RTPStreamChannel *streamptr;

#define INPUT_TEST_NUM 10

int main() {

    test_ringbuffer();


    std::cout << "Debug info: Program starting\n";

    //Sleep(100);



    streamptr = NULL;
    PAPlayback playback;


    using namespace std::chrono_literals;

    std::cout << "now beep" << std::endl;

    std::this_thread::sleep_for(500ms);

    //    PA_playback audio_playback;
    RTPServer rtp_server;



    rtp_server.init();
    //rtp_server.test_buffer("ahoj", 5);
    std::cout << "Now switch to stream" << std::endl;
    streamptr = rtp_server.DEBUGGetOneChannelPtr();
    rtp_server.run();




}
