#include <iostream>


#include "test_ringbuffer.h"
#include "rtpserver.h"



#define INPUT_TEST_NUM 10

int main() {

    test_ringbuffer();


    std::cout << "Debug info: Program starting\n";

    //Sleep(100);


    //    PA_playback audio_playback;
    RTPServer rtp_server;
    rtp_server.init();
    rtp_server.test_buffer("ahoj", 5);
    rtp_server.run();


}
