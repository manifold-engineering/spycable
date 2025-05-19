#ifndef AUDIORINGBUFFER_H
#define AUDIORINGBUFFER_H


#include <cstdint>
#include <vector>
#include <map>

typedef int32_t sample_t;


struct PacketMetadata {
    uint16_t rtp_sequence_number;
    uint32_t rtp_timestamp;
    uint64_t absolute_start_sample;  // First sample position this packet contributed to
    uint32_t sample_count;           // Number of samples in this packet
};


class AudioRingbuffer
{
public:
    explicit AudioRingbuffer(size_t capacitySamples);
    void ingestChunk(const char *samples, const size_t count, const PacketMetadata *metadata);


    bool isFull() const;
    bool isEmpty() const;
    void shallowFlush();

    void debug_print();

private:
    size_t head;
    size_t tail;
    size_t size;
    size_t capacity;

    uint64_t oldest_sample_absolute;
    uint64_t last_sample_absolute();
    uint32_t sequence_num_overflows;


    void pushSingleSample(const sample_t *sample);
    sample_t peekSingleSample(const size_t num);

    std::vector<int32_t> audio_buffer;
    std::map<uint64_t, PacketMetadata> metadata_map;

};

#endif // AUDIORINGBUFFER_H
