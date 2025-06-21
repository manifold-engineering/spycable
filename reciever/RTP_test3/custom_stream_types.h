#ifndef CUSTOM_STREAM_TYPES_H
#define CUSTOM_STREAM_TYPES_H

#include <cstdint>

#include "deinterlacedbuffer.h"
#include "audioringbuffer.h"
//todo: this values should be probably loaded from external config, but hardcoded for simplicity now

#define BYTES_PER_SAMPLE_96 3
#define BYTES_PER_SAMPLE_97 3

#define CHANNELS_96 1
#define CHANNELS_97 2


unsigned int get_bytes_per_sample(unsigned int stream_type){
    switch (stream_type){
    case 96:
        return BYTES_PER_SAMPLE_96;
    case 97:
        return BYTES_PER_SAMPLE_97;
    }
    return 0;
}

unsigned int get_channels(unsigned int stream_type){
    switch (stream_type){
    case 96:
        return CHANNELS_96;
    case 97:
        return CHANNELS_97;
    }
    return 0;
}


int32_t decode24le(const std::byte* b) {
    int32_t value =
        static_cast<uint8_t>(b[0]) |
        (static_cast<uint8_t>(b[1]) << 8) |
        (static_cast<uint8_t>(b[2]) << 16);
    if (value & 0x00800000)
        value |= 0xFF000000;
    return value;
}


unsigned int deinterlace_pad_incoming_data(std::byte *input, DeinterlacedBuffer<sample_t> *output, unsigned int samples_num, unsigned int stream_type){
    unsigned int bytes_per_sample = get_bytes_per_sample(stream_type);
    unsigned int channel_num = get_channels(stream_type);

    for (unsigned int i_sample = 0; i_sample < samples_num; ++i_sample){
        for (unsigned int i_channel = 0; i_channel < channel_num; ++ i_channel){
            size_t data_index = bytes_per_sample * (i_sample * channel_num + i_channel);


            int32_t sample;

            if (stream_type == 96 || stream_type == 97){
                sample = decode24le(input + data_index);
            }

            else{
                //other types implement here
                return false;
            }

            (*output)(i_channel, i_sample) = sample;


        }
    }

    return true;
}


#endif // CUSTOM_STREAM_TYPES_H
