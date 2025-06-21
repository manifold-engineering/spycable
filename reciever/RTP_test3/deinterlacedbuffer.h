#ifndef DEINTERLACEDBUFFER_H
#define DEINTERLACEDBUFFER_H

#include <vector>
#include <cstdint>
#include <cassert>
#include "audioringbuffer.h"

template<typename SampleT = sample_t>
class DeinterlacedBuffer {
public:
    DeinterlacedBuffer(size_t channels, size_t samples)
        : m_channels(channels), m_samples(samples),
        m_data(channels * samples) {}

    // Optional: Access single sample by (channel, index)
    SampleT& operator()(size_t channel, size_t sample_index) {
        assert(channel < m_channels && sample_index < m_samples);
        return m_data[channel * m_samples + sample_index];
    }

    const SampleT& operator()(size_t channel, size_t sample_index) const {
        assert(channel < m_channels && sample_index < m_samples);
        return m_data[channel * m_samples + sample_index];
    }

    // Optional: access raw pointer
    SampleT* data() { return m_data.data(); }
    const SampleT* data() const { return m_data.data(); }

    // Optional: access per-channel slice (for use in PortAudio callback)
    SampleT* channel_data(size_t channel) {
        assert(channel < m_channels);
        return &m_data[channel * m_samples];
    }

    size_t channels() const { return m_channels; }
    size_t samples() const { return m_samples; }

private:
    size_t m_channels;
    size_t m_samples;
    std::vector<SampleT> m_data;
};

#endif
