#ifndef RTPSTREAMCHANNEL_H
#define RTPSTREAMCHANNEL_H

#include <string>


#include "config.h"
#include "audioringbuffer.h"

enum ChannelState {
    INACTIVE,
    ACTIVE
};

class RTPStreamChannel{
public:
    AudioRingbuffer *buff;
    //std::string name;
    ChannelState state_network;
    ChannelState state_palyback;
    //bool used;

    RTPStreamChannel(size_t capacity = DEFAULT_SAMPLES_CAPACITY);
    ~RTPStreamChannel();

    void reset();
};


#endif // RTPSTREAMCHANNEL_H
