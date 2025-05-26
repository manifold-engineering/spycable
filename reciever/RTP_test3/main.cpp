#include "audioringbuffer.h"

#define TEST_BUFF_SIZE 10


int main(int argc, char *argv[])
{
    AudioRingbuffer test_buffer(TEST_BUFF_SIZE);
    test_buffer.test_audiobuffer();
}

