#ifndef RTPSERVER_H
#define RTPSERVER_H

#include <string>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
typedef int socklen_t;
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#define SOCKET int
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket close
#endif


#include <string>
#include <map>
#include <list>
#include <vector>
#include <cstdint>
#include <cstring>
#include <chrono>

#include "rtpstreamchannel.h"
#include "audioringbuffer.h"
#include "config.h"

class RTPHeader {
private:
    union {
        struct {
            uint8_t versionAndFlags_raw;  // Version, padding, extension, marker
            uint8_t payloadType_raw;      // Payload type
            uint16_t sequenceNumber_raw;  // Sequence number
            uint32_t timestamp_raw;       // Timestamp
            uint32_t ssrc_raw;            // Synchronization source identifier
        };
        uint8_t raw[12];  // Raw 12-byte header
    };

public:

    uint8_t version;  // Version, padding, extension, marker
    uint8_t payload_type;      // Payload type
    uint16_t sequence_number;  // Sequence number
    uint32_t timestamp;       // Timestamp
    uint32_t ssrc;            // Synchronization source identifier


    void parse(const uint8_t *buffer);
    void print() const;
};


class RTPServer {
private:
    SOCKET sock;
    uint16_t udp_port;
    struct sockaddr_in serverAddr;
    char socket_buffer[SOCKET_BUFFER_SIZE];
    int socket_bytes_received;
    struct sockaddr_in client_addr;
    void ingestData();

    std::map <std::string, RTPStreamChannel> channels;

    int DEBUG_ingest_counter;
public:

    RTPServer(uint16_t port = DEFAULT_UDP_PORT, size_t stream_capacity=DEFAULT_STREAM_CAPACITY, size_t samples_capacity=DEFAULT_SAMPLES_CAPACITY);

    ~RTPServer();

    bool init();
    void run();
    void test_buffer(std::string, unsigned int number_of_samples);
    RTPStreamChannel* DEBUGGetOneChannelPtr();

};

#endif // RTPSERVER_H
