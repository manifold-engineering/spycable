#ifndef TEST_RINGBUFFER_H
#define TEST_RINGBUFFER_H

#include <iostream>
#include "audioringbuffer.h"


#define INPUT_TEST_NUM 10

void printRingBuffer(AudioRingBuffer arb){
    std::cout << "Buffer, used " << arb.getStored() << " of " << arb.getCapacity() << std::endl;
    for (int i = 0; i < arb.getStored(); ++i){
        std::cout << arb.peekSingleSample(i) << ", ";
    }
    std::cout << std::endl;

}

int test_ringbuffer() {
    constexpr size_t bufSize = 8;
    AudioRingBuffer ring(bufSize);

    int32_t input[INPUT_TEST_NUM] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    SampleMetadata dummy_metadata;

    for (unsigned int i = 0; i < INPUT_TEST_NUM; ++i){
        ring.pushSingle(&input[i], &dummy_metadata);
    }

    printRingBuffer(ring);

    std::cout << "peeking last element: " << ring.peekSingleSample(0) << std::endl;


    std::cout << "now try to copy out" << std::endl;

    constexpr size_t outSize = 5;
    int32_t outBuff[outSize];
    ring.copyAudioTo(outBuff, outSize);

    for (unsigned int i = 0; i < outSize; ++i){
        std::cout << outBuff[i] << ", ";
    }
    std::cout << std::endl;

    std::cout << "moving playhead by 1" << std::endl;

    ring.movePlayHead(1);

    std::cout << "peeking last element: " << ring.peekSingleSample(0) << std::endl;


    std::cout << "moving playhead by 2" << std::endl;

    ring.movePlayHead(2);

    std::cout << "peeking last element: " << ring.peekSingleSample(0) << std::endl;


    printRingBuffer(ring);


    return 0;
}


#endif // TEST_RINGBUFFER_H
