#include <chrono>
#include <iostream>
#include <algorithm>
#include <exception>


#include "audioringbuffer.h"

AudioRingBuffer::AudioRingBuffer(size_t capacitySamples)
    : audio_buffer(capacitySamples),
      metadata_buffer(capacitySamples),
      head(0),
      tail(0),
      stored(0),
      capacity(capacitySamples) {}

bool AudioRingBuffer::isFull() const
{
    return this->stored == this->capacity;
}

bool AudioRingBuffer::isEmpty() const
{
    return this->stored == 0;
}

void AudioRingBuffer::pushSingle(const int32_t *sample, const SampleMetadata *metadata)
{
    std::cout << "DEBUG: writing single sample" << std::endl;
    audio_buffer[head] = *sample;
    metadata_buffer[head] = *metadata;

    if (this->stored == this->capacity)
        this->tail++;

    this->stored = std::min(this->stored +1, this->capacity);

    this->head = (this->head + 1 ) % this->capacity;


    return;
}

sample_t AudioRingBuffer::peekSingleSample(const size_t num)
{
    if (num >= this->stored){
        //raise(std::exception("not enoght data")); //maybe later
        return 0;
    }
    size_t index = (tail + num) % capacity;
    return this->audio_buffer[index];
}

bool AudioRingBuffer::copyAudioTo(int32_t *loc, size_t num)
{
    size_t num_checked = std::min(num, this->stored);


    for (unsigned int i = 0; i < num; ++i){
        loc[i] = this->peekSingleSample(i);
    }

    return false;

}

bool AudioRingBuffer::movePlayHead(const size_t num)
{
    size_t checked_num = std::min(num, this->stored);
    this->tail = (this->tail + checked_num)%this->capacity;
    return checked_num == num;
}

void AudioRingBuffer::shallowFlush()
{
    this->head = 0;
    this->tail = 0;
    this->stored = 0;
}




/* AI generated bullcrap, thanks GPT, not really bad, but not what I envisioned
size_t AudioRingBuffer::write(const int32_t* data, size_t count) {
    size_t writable = availableToWrite();
    size_t toWrite = (count < writable) ? count : writable;

    for (size_t i = 0; i < toWrite; ++i) {
        buffer[head] = data[i];
        head = (head + 1) % capacity;
    }
    return toWrite;
}
*/

/*
size_t AudioRingBuffer::read(int32_t* out, size_t count) {
    size_t readable = availableToRead();
    size_t toRead = (count < readable) ? count : readable;

    for (size_t i = 0; i < toRead; ++i) {
        out[i] = buffer[tail];
        tail = (tail + 1) % capacity;
    }
    return toRead;
}
*/

void AudioRingBuffer::clear() {
    head = tail = 0;
}

size_t AudioRingBuffer::getStored() const
{
    return stored;
}

size_t AudioRingBuffer::getCapacity() const
{
    return capacity;
}
