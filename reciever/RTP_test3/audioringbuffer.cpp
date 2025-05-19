#include "audioringbuffer.h"

AudioRingbuffer::AudioRingbuffer(size_t capacitySamples):
    audio_buffer(capacitySamples),
    head(0),
    tail(0),
    size(0),
    capacity(capacitySamples),
    oldest_sample_absolute(0),
    sequence_num_overflows(0)
{

}

void AudioRingbuffer::ingestChunk(const char *samples, const size_t count, const PacketMetadata *metadata)
{
    uint64_t chunk_number = 0;

}
