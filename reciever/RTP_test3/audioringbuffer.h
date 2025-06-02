#ifndef AUDIORINGBUFFER_H
#define AUDIORINGBUFFER_H


#include <cstdint>
#include <vector>
#include <map>
#include <exception>

typedef int32_t sample_t;


//just little helper
uint64_t diff_unsigned (uint64_t a, uint64_t b);



class UnexpectedPacketException : public std::exception {
public:
    UnexpectedPacketException(const char* msg) : message(msg) {}
    const char* what() const noexcept override {
        return message;
    }
private:
    const char* message;
};



struct PacketMetadata {
    uint16_t rtp_sequence_number;
    uint32_t rtp_timestamp;
};

struct ChunkMetadata {
    uint32_t rtp_timestamp;
    uint64_t absolute_start_sample;  // First sample position this packet contributed to
    uint32_t sample_count;           // Number of samples in this packet
};


class AudioRingbuffer
{
public:
    explicit AudioRingbuffer(size_t capacitySamples);
    void ingestChunk(const sample_t *samples, const size_t count, const PacketMetadata metadata);


    bool isFull() const;
    bool isEmpty() const;
    void shallowFlush();
    sample_t peek_absolute(uint64_t sample_num_absolute) const;
    void copyAudioDataToAbosulte(sample_t *target, uint64_t start_sample_num_absolute, size_t num) const;
    void debug_print();



    void test_audiobuffer();    //test only the audioringbuffer part, which doe not have public interface


    uint64_t last_sample_absolute() const;
    uint64_t getOldest_sample_absolute() const;

private:
    size_t head;
    size_t tail;
    size_t size;
    size_t capacity;
    uint64_t playhead_absolute;

    uint64_t oldest_sample_absolute;

    //uint64_t chunk_sequence_num_overflows;



    void pushSingleSample(const sample_t *sample);  //push at end, not regarding absolute
    bool pushSingleSampleAbsolute(const sample_t *sample, uint64_t absolute_position);  //push sample with regarding absolute number
    sample_t peekSingleSampleRelative(const size_t num);
    bool incrementPlayhed();
    bool incrementPlayhed(size_t num);

    std::vector<int32_t> audio_buffer;
    std::map<uint64_t, ChunkMetadata> metadata_map;

};



#endif // AUDIORINGBUFFER_H
