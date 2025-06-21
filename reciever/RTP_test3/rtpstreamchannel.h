#ifndef RTPSTREAMCHANNEL_H
#define RTPSTREAMCHANNEL_H

#include <string>


#include "audioringbuffer.h"

enum ChannelState {

    INACTIVE,
    BUFFERING,
    PLAYBACK,
};

class RTPStreamChannel{
public:
    AudioRingbuffer *buff;
    //std::string name;
    ChannelState state;
    //bool used;

    RTPStreamChannel(size_t capacity);
    ~RTPStreamChannel();

    void reset();
};


#endif // RTPSTREAMCHANNEL_H
