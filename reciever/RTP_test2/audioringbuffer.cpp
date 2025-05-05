
#include "audioringbuffer.h"

AudioRingBuffer::AudioRingBuffer(size_t capacitySamples)
    : buffer(capacitySamples + 1), head(0), tail(0), capacity(capacitySamples + 1) {}

size_t AudioRingBuffer::availableToRead() const {
    if (head >= tail)
        return head - tail;
    else
        return capacity - tail + head;
}

size_t AudioRingBuffer::availableToWrite() const {
    return capacity - 1 - availableToRead();
}

size_t AudioRingBuffer::write(const int32_t* data, size_t count) {
    size_t writable = availableToWrite();
    size_t toWrite = (count < writable) ? count : writable;

    for (size_t i = 0; i < toWrite; ++i) {
        buffer[head] = data[i];
        head = (head + 1) % capacity;
    }
    return toWrite;
}

size_t AudioRingBuffer::read(int32_t* out, size_t count) {
    size_t readable = availableToRead();
    size_t toRead = (count < readable) ? count : readable;

    for (size_t i = 0; i < toRead; ++i) {
        out[i] = buffer[tail];
        tail = (tail + 1) % capacity;
    }
    return toRead;
}

void AudioRingBuffer::clear() {
    head = tail = 0;
}
