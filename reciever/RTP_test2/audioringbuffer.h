#ifndef AUDIORINGBUFFER_H
#define AUDIORINGBUFFER_H

#include <vector>
#include <cstdint>
#include <cstddef>
#include <stdexcept>

class AudioRingBuffer {
public:
    explicit AudioRingBuffer(size_t capacitySamples);

    size_t write(const int32_t* data, size_t count);
    size_t read(int32_t* out, size_t count);

    size_t availableToRead() const;
    size_t availableToWrite() const;

    void clear();

private:
    std::vector<int32_t> buffer;
    size_t head;
    size_t tail;
    size_t capacity; // total usable slots = buffer.size() - 1
};

#endif // AUDIORINGBUFFER_H
