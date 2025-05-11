#include "rtpstreamchannel.h"

#include <iostream>

bool RTPStreamChannel::getUsed() const
{
    return used;
}

void RTPStreamChannel::setUsed(bool newUsed)
{
    used = newUsed;
}

void RTPStreamChannel::ingestSample(sample_t *sample , SampleMetadata *metadata)
{
    this->buff.pushSingle(sample, metadata);
}

RTPStreamChannel::RTPStreamChannel(RTPStreamChannel &&other) noexcept
    : buff(std::move(other.buff)), used(other.used)
{
    std::cout << "Moving RTPStreamChannel! This should have never happened." << std::endl;
}


RTPStreamChannel::RTPStreamChannel(size_t capacity)
    :buff(capacity)
{
    std::cout << "Konstruktor RTSP Stream Channel" << std::endl;
    used=false;
}

RTPStreamChannel::~RTPStreamChannel()
{
    std::cout << "Destruktor RTP Stream Channel" << std::endl;
}

void RTPStreamChannel::reset()
{
    this->name="";
    this->used=false;
    this->buff.shallowFlush();
}

std::string RTPStreamChannel::getName() const
{
    return name;
}

void RTPStreamChannel::setName(const std::string &newName)
{
    name = newName;
}

bool RTPStreamChannel::hasName(const std::string &ref_name) const{
    return this->name == ref_name;
}
