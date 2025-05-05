#include <iostream>
#include "audioringbuffer.h"

int main() {
    constexpr size_t bufSize = 8;
    AudioRingBuffer ring(bufSize);

    int32_t input[5] = {1, 2, 3, 4, 5};
    ring.write(input, 5);

    std::cout << "Available to read: " << ring.availableToRead() << "\n";

    int32_t output[5] = {};
    ring.read(output, 8);

    for (int i = 0; i < 8; ++i) {
        std::cout << output[i] << " ";
    }
    std::cout << "\n";

    //int32_t output[5] = {};
    ring.read(output, 5);

    for (int i = 0; i < 8; ++i) {
        std::cout << output[i] << " ";
    }
    std::cout << "\n";

    ring.write(input, 6);

    ring.read(output, 8);

    for (int i = 0; i < 5; ++i) {
        std::cout << output[i] << " ";
    }
    std::cout << "\n";
    return 0;
}
