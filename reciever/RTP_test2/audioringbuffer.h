#ifndef AUDIORINGBUFFER_H
#define AUDIORINGBUFFER_H

#include <vector>
#include <cstdint>
#include <cstddef>
#include <stdexcept>
#include <chrono>


typedef int32_t sample_t;

struct SampleMetadata {
    uint16_t sequenceNumber{};
    uint32_t rtpTimestamp{};
    std::chrono::time_point<std::chrono::steady_clock> recievedTimestamp{};

    SampleMetadata() = default; // Optional explicit default constructor
};


struct taggedSample{
    sample_t sample;
    SampleMetadata metadata;
};

class AudioRingBuffer {
public:
    explicit AudioRingBuffer(size_t capacitySamples);

    void pushSingle(const sample_t *sample, const SampleMetadata *metadata);
    void pushChunk(const sample_t *sample, const SampleMetadata *metadata, const size_t count);
    sample_t peekSingleSample(const size_t num);
    size_t read(int32_t* out, size_t count);
    bool copyAudioTo(int32_t *loc, const size_t num);


    bool isFull() const;
    bool isEmpty() const;
    void clear();

    size_t getStored() const;

    size_t getCapacity() const;

private:
    std::vector<int32_t> audio_buffer;
    std::vector<SampleMetadata> metadata_buffer;
    size_t head;
    size_t tail;
    size_t capacity;
    size_t stored;
};

#endif // AUDIORINGBUFFER_H
