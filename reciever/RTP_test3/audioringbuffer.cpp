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

    int64_t chunk_number_guess = UINT16_MAX*last_overflows + rtp_sequence_number;

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

void AudioRingbuffer::ingestChunk(const char *samples, const size_t count, const PacketMetadata metadata)
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
        uint64_t chunk_num = guessChunkNum(metadata.rtp_sequence_number, last_chunk_num);
    }
    catch (const UnexpectedPacketException& e) {
        std::cout << "Incorrect incoming packet: " << e.what() << std::endl;
        return;
    }

    if (this->metadata_map.find(chunk_num) == this->metadata_map.end()){
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
    if (!insert_result.second){
        std::cout << "could not insert metadata, skipping sample" << std::endl;
        return;
    }
    else if (insert_result.first == this->metadata_map.begin()){
        //we have no sampler prior to this one, lets calculate same way as before
        first_sample_num = metadata.rtp_sequence_number * count;
    }
    else{
        //we have previous samples, lets continue numbering with them
        ChunkMetadata previous_chunk_metadata = (*(insert_result.first--)).second;
        first_sample_num = previous_chunk_metadata.absolute_start_sample + previous_chunk_metadata.sample_count;
    }

    insert_result.first->second.absolute_start_sample = first_sample_num;

    //now metadata computed and inserted


}

uint64_t AudioRingbuffer::last_sample_absolute()
{
    return this->oldest_sample_absolute + this->size;
}
