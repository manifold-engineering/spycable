#include <iostream>
#include "audioringbuffer.h"



void printRingBuffer(AudioRingBuffer arb){
    std::cout << "Buffer, used " << arb.getStored() << " of " << arb.getCapacity() << std::endl;
    for (int i = 0; i < arb.getStored(); ++i){
        std::cout << arb.peekSingleSample(i) << ", ";
    }
    std::cout << std::endl;

}

#define INPUT_TEST_NUM 10

int main() {
    constexpr size_t bufSize = 8;
    AudioRingBuffer ring(bufSize);

    int32_t input[INPUT_TEST_NUM] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    SampleMetadata dummy_metadata;

    for (unsigned int i = 0; i < INPUT_TEST_NUM; ++i){
        ring.pushSingle(&input[i], &dummy_metadata);
    }

    printRingBuffer(ring);
    return 0;
}
