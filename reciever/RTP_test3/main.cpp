#include <iostream>

#include "audioringbuffer.h"
#include "rtpserver.h"


#define TEST_BUFF_SIZE_A 10
#define TEST_BUFF_SIZE_B 512
#define TEST_CHUNK_SIZE 240

#define TEST_SAMPLERATE 48000
#define TEST_CLOCK_FREQUENCY 8000
#define CLOCK_TICKS_PER_CHUNK TEST_CHUNK_SIZE * TEST_CLOCK_FREQUENCY / TEST_SAMPLERATE

int main(int argc, char *argv[])
{
    RTPServer server;

    server.init();
    server.run();
}

