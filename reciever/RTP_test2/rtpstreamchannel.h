#ifndef RTPSTREAMCHANNEL_H
#define RTPSTREAMCHANNEL_H

#include <string>


#include "audioringbuffer.h"


class RTPStreamChannel{
    AudioRingBuffer buff;
    std::string name;
    bool used;
public:

    RTPStreamChannel(size_t capacity);
    ~RTPStreamChannel();
    void reset();
    std::string getName() const;
    void setName(const std::string &newName);
    bool hasName(const std::string &ref_name) const;
    bool getUsed() const;
    void setUsed(bool newUsed);
    void ingestSample(sample_t*, SampleMetadata*);


    // Move constructor
    RTPStreamChannel(RTPStreamChannel&& other) noexcept;

    // Delete copy constructor to prevent accidental copies
    RTPStreamChannel(const RTPStreamChannel&) = delete;
    RTPStreamChannel& operator=(const RTPStreamChannel&) = delete;

    bool copyAudioTo(int32_t *loc, const size_t num);
    bool movePlayHead(const size_t num);
    //  AudioRingBuffer getBuff() const;    //ugly hack, not worth it

};


#endif // RTPSTREAMCHANNEL_H
