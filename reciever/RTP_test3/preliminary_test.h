#ifndef PRELIMINARY_TEST_H
#define PRELIMINARY_TEST_H




#include "audioringbuffer.h"

#define TEST_BUFF_SIZE_A 10
#define TEST_BUFF_SIZE_B 512
#define TEST_CHUNK_SIZE 240



void preliminary_test_1(){

    AudioRingbuffer test_buffer_a(TEST_BUFF_SIZE_A);
    test_buffer_a.test_audiobuffer();


    std::cout << "\n Now testing ingesting data with per chunk metadata \n \n" << std::endl;


    AudioRingbuffer test_buffer_b(TEST_BUFF_SIZE_B);

    sample_t test_audio_data_1[TEST_CHUNK_SIZE];
    sample_t test_audio_data_2[TEST_CHUNK_SIZE];
    sample_t test_audio_data_3[TEST_CHUNK_SIZE];
    sample_t test_audio_data_4[TEST_CHUNK_SIZE];
    sample_t test_audio_data_5[TEST_CHUNK_SIZE];

    for (int i = 0; i < TEST_CHUNK_SIZE; ++i){
        test_audio_data_1[i] = 42+i;
        test_audio_data_2[i] = 69+i;
        test_audio_data_3[i] = 666+i;
        test_audio_data_4[i] = 1337+i;
        test_audio_data_5[i] = 80085+i;
    }

    PacketMetadata test_metadata_1 = {
        .rtp_sequence_number =  0,
        .rtp_timestamp =  0,
    };

    PacketMetadata test_metadata_2 = {
        .rtp_sequence_number =  2,      //lets simulate one missing chunk between 1 and 2
        .rtp_timestamp = CLOCK_TICKS_PER_CHUNK * 2,
    };

    PacketMetadata test_metadata_3 = {
        .rtp_sequence_number =  3,      //now no missing chunks, lets test audio buffer wraparound
        .rtp_timestamp =  CLOCK_TICKS_PER_CHUNK * 3,
    };

    test_buffer_b.ingestChunk(test_audio_data_1, TEST_CHUNK_SIZE, test_metadata_1);
    test_buffer_b.ingestChunk(test_audio_data_2, TEST_CHUNK_SIZE, test_metadata_2);
    test_buffer_b.ingestChunk(test_audio_data_3, TEST_CHUNK_SIZE, test_metadata_3);

}


#endif // PRELIMINARY_TEST_H
