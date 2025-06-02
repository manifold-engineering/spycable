#include <cmath>
#include <cstdint>
#include <iostream>

#include "audioringbuffer.h"



#define CHUNK_OO_TOL 40

AudioRingbuffer::AudioRingbuffer(size_t capacitySamples):
    audio_buffer(capacitySamples),
    head(0),
    tail(0),
    size(0),
    capacity(capacitySamples),
    oldest_sample_absolute(0)
    //chunk_sequence_num_overflows(0)
{
}

uint64_t diff_unsigned (uint64_t a, uint64_t b){
    return (a > b) ? (a - b) : (b - a);
}

uint64_t guessChunkNum (uint16_t rtp_sequence_number, uint64_t last_chunk_number){

    uint64_t last_overflows = last_chunk_number/UINT16_MAX;

    uint64_t chunk_number_guess = UINT16_MAX*last_overflows + rtp_sequence_number;

    if (diff_unsigned(chunk_number_guess, last_chunk_number) < CHUNK_OO_TOL){
        //probably no overflow of RTP sequence between incoming and last chunk
        return chunk_number_guess;
    }
    chunk_number_guess = UINT16_MAX*(last_overflows + 1)+ rtp_sequence_number;

    if (diff_unsigned(chunk_number_guess, last_chunk_number) < CHUNK_OO_TOL){
        //just overflowed, no problem
        return chunk_number_guess;
    }
    chunk_number_guess = UINT16_MAX*(last_overflows - 1)+ rtp_sequence_number;

    if (diff_unsigned(chunk_number_guess, last_chunk_number) < CHUNK_OO_TOL){
        //overflowed last time, but now received OO packet
        return chunk_number_guess;
    }
    throw UnexpectedPacketException("Packet too far from order");
}

void AudioRingbuffer::ingestChunk(const sample_t *samples, const size_t count, const PacketMetadata metadata)
{
    /*
    uint64_t chunk_number = static_cast<uint64_t>(metadata.rtp_sequence_number) + UINT16_MAX*this->chunk_sequence_num_overflows;
    this->chunk_sequence_num_overflows = chunk_number/UINT16_MAX;
    */  //initial approach, correct, but woul not work with out of order packets

    uint64_t last_chunk_num = 0;
    if (!this->metadata_map.empty())
        last_chunk_num = (--this->metadata_map.end())->first;

    uint64_t chunk_num;
    try{
        chunk_num = guessChunkNum(metadata.rtp_sequence_number, last_chunk_num);
    }
    catch (const UnexpectedPacketException& e) {
        std::cout << "Incorrect incoming packet: " << e.what() << std::endl;
        return;
    }

    if (this->metadata_map.find(chunk_num) != this->metadata_map.end()){
        std::cout << "Duplicit incoming packet: " << chunk_num << std::endl;
        return;
    }

    //now I know this chunk absolute number
    //now I have to calculate first sample absolute number
    uint64_t first_sample_num;
    if(this->metadata_map.empty()){
        first_sample_num = metadata.rtp_sequence_number * count;
    }
    //in case my was not empty, I cold calculate it the same way
    //but the number of samples may change, so i will try to be more carefull here

    ChunkMetadata new_chunk_metadata{
       .rtp_timestamp = metadata.rtp_timestamp,
        .sample_count = static_cast<uint32_t>(count),
    };

    auto insert_result = this->metadata_map.insert({chunk_num, new_chunk_metadata});
    std::map<uint64_t, ChunkMetadata>::iterator new_chunk_it;
    if (!insert_result.second){
        std::cout << "could not insert metadata, skipping chunk" << std::endl;
        return;
    }
    else{
        new_chunk_it = insert_result.first;
    }

    if (new_chunk_it == this->metadata_map.begin()){
        //we have no sample prior to this one, lets calculate same way as before
        first_sample_num = metadata.rtp_sequence_number * count;
    }
    else{
        //we have previous samples, lets continue numbering with them
        ChunkMetadata previous_chunk_metadata = (*(std::prev(new_chunk_it))).second;
        uint64_t previous_chunk_absolute_num = (*(std::prev(new_chunk_it))).first;
        unsigned int chunk_num_diff = chunk_num - previous_chunk_absolute_num;
        first_sample_num = previous_chunk_metadata.absolute_start_sample + previous_chunk_metadata.sample_count * chunk_num_diff;
    }

    insert_result.first->second.absolute_start_sample = first_sample_num;

    //now metadata computed and inserted

    for (unsigned int sample_num = 0; sample_num < count; ++sample_num){
        this->pushSingleSampleAbsolute(&samples[sample_num], first_sample_num + sample_num);
    }
}

bool AudioRingbuffer::isEmpty() const
{
    return this->size == 0;
}

void AudioRingbuffer::shallowFlush()
{
    this->head = 0;
    this->tail = 0;
    this->size = 0;
    this->playhead_absolute = 0;
    this->metadata_map.clear();
}

sample_t AudioRingbuffer::peek_absolute(uint64_t sample_num_absolute) const
{
    if(this->oldest_sample_absolute < sample_num_absolute || this->last_sample_absolute() > sample_num_absolute){
        //requested sample is not in this buffer
        return 0;
    }
    size_t diff = sample_num_absolute - oldest_sample_absolute;
    size_t index = (this->tail + diff) % this->capacity;
    return this->audio_buffer[index];
}

void AudioRingbuffer::copyAudioDataToAbosulte(sample_t *target, uint64_t start_sample_num_absolute, size_t num) const
{
    for (unsigned int i = 0; i<num; ++i){
        target[i] =this->peek_absolute(start_sample_num_absolute + i);
    }
}

void AudioRingbuffer::test_audiobuffer()
{
    std::cout << "Clearing before testing" << std::endl;
    this->shallowFlush();
    unsigned int n1 = 6;
    std::cout << "Test writing " << n1 << " samples" << std::endl;
    for (unsigned int i = 0; i < n1; ++i){
        sample_t dato = 666 + i;
        this->pushSingleSample(&dato);
    }
    std::cout << "current buffer content: " << this->size << " samples of " <<this->capacity<< std::endl;
    for(unsigned int i = 0; i<this->size; ++i){
        std::cout << this->peekSingleSampleRelative(i) << ", ";
    }
    std::cout << std::endl;

    unsigned int n2 = 8;

    std::cout << "Test writing another" << n2 << " samples" << std::endl;
    for (unsigned int i = 0; i < n2; ++i){
        sample_t dato = 42 + i;
        this->pushSingleSample(&dato);
    }
    std::cout << "current buffer content: " << this->size << " samples of " <<this->capacity<< std::endl;
    for(unsigned int i = 0; i<this->size; ++i){
        std::cout << this->peekSingleSampleRelative(i) << ", ";
    }
    std::cout << std::endl;

    unsigned int n3 = 3;

    std::cout << "incrementing playhead by " << n3 << "samples";

    this->incrementPlayhed();
    this->incrementPlayhed();
    this->incrementPlayhed();

    std::cout << "current buffer content: " << this->size << " samples of " <<this->capacity<< std::endl;
    for(unsigned int i = 0; i<this->size; ++i){
        std::cout << this->peekSingleSampleRelative(i) << ", ";
    }
    std::cout << std::endl;


    unsigned int n4 = 12;

    std::cout << "Test writing another" << n4 << " samples" << std::endl;
    for (unsigned int i = 0; i < n4; ++i){
        sample_t dato = 69 + i;
        this->pushSingleSample(&dato);
    }
    std::cout << "current buffer content: " << this->size << " samples of " <<this->capacity<< std::endl;
    for(unsigned int i = 0; i<this->size; ++i){
        std::cout << this->peekSingleSampleRelative(i) << ", ";
    }
    std::cout << std::endl;


    std::cout << "Clearing after testing" << std::endl;
    this->shallowFlush();


}

uint64_t AudioRingbuffer::last_sample_absolute() const
{
    return this->oldest_sample_absolute + this->size;
}

uint64_t AudioRingbuffer::getOldest_sample_absolute() const
{
    return oldest_sample_absolute;
}

void AudioRingbuffer::pushSingleSample(const sample_t *sample)
{
    //std::cout << "DEBUG: writing single sample: " << *sample << std::endl;
    audio_buffer[head] = *sample;

    if (this->size == this->capacity){
        this->tail = (this->tail + 1 ) % this->capacity;
        this->oldest_sample_absolute ++;
    }

    this->size = std::min(this->size +1, this->capacity);

    this->head = (this->head + 1 ) % this->capacity;
}

bool AudioRingbuffer::pushSingleSampleAbsolute(const sample_t *sample, uint64_t absolute_position)
{
    if (this->isEmpty()){
        //we are inserting first sample, hooray
        this->pushSingleSample(sample);
        this->oldest_sample_absolute = absolute_position;
        return true;
    }

    if (absolute_position < this->oldest_sample_absolute){
        //incoming sample is to old, our buffer is already ahaed, discarding incoming sample
        return false;
    }

    /*
    if (absolute_position > this->oldest_sample_absolute + this->capacity){
        //we are far into future
        //none of current buffer is relevant
        this->shallowFlush();
        this->pushSingleSampleAbsolute(sample, absolute_position);
        //looks like a recursion, but since buffer was emptied it should fall into first case of this method anyways
    }
    */

    if (absolute_position >= this->last_sample_absolute()){
        sample_t dummy_zero = 0;
        size_t pad_samples_num = absolute_position - this->last_sample_absolute();
        for (unsigned int i = 0; i< pad_samples_num; ++i){
            this->pushSingleSample(&dummy_zero);
        }
        this->pushSingleSample(sample);
        return true;
    }

    //now I basically know the incoming sample needs to "fit" inside current buffer
    //so I dont need to move head/tails
    uint64_t diff = absolute_position - this->oldest_sample_absolute;
    size_t index = (tail + diff)% this->capacity;
    this->audio_buffer[index] = *sample;
    return true;

}

sample_t AudioRingbuffer::peekSingleSampleRelative(const size_t num)
{
    if (num >= this->size){
        //raise(std::exception("not enoght data")); //maybe later
        return 0;
    }
    size_t index = (tail + num) % capacity;

    //std::cout << "DEBUG: peeking single sample: " << this->audio_buffer[index] << std::endl;
    return this->audio_buffer[index];
}

bool AudioRingbuffer::incrementPlayhed(size_t num)
{
    size_t incrementing_num = std::min(this->size, num);
    for(unsigned int i = 0; i<incrementing_num; ++i){
        this->incrementPlayhed();
    }
    return incrementing_num == num;
}

bool AudioRingbuffer::incrementPlayhed()
{
    if (this->isEmpty()){
        //todo: maybe exception...
        return false;
    }
    this->size --;
    this->tail = (this->tail + 1) % capacity;
    this->oldest_sample_absolute ++;
    return true;
}

