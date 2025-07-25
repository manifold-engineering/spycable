#include "rtpstreamchannel.h"

#include <iostream>


RTPStreamChannel::RTPStreamChannel(size_t capacity)
{
    this->buff = new AudioRingbuffer(capacity);
    std::cout << "Konstruktor RTSP Stream Channel" << std::endl;
    this->state_network = INACTIVE;
}

RTPStreamChannel::~RTPStreamChannel()
{
    delete  this->buff;
    std::cout << "Destruktor RTP Stream Channel" << std::endl;
}

void RTPStreamChannel::reset()
{
    this->state_network=INACTIVE;
    this->buff->shallowFlush();
}
