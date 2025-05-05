import socket
import struct
import time

# RTP constants
RTP_HEADER_SIZE = 12
PAYLOAD_TYPE = 97  # Dynamic payload type for L24 (24-bit PCM)
SSRC = 0xDEADBEEF  # Example SSRC, unique per device
SAMPLE_RATE = 48000  # 48 kHz
CHANNELS = 2  # Stereo
BYTES_PER_SAMPLE = 3  # 24-bit PCM
PACKET_INTERVAL_MS = 10 #packet duration
SAMPLES_PER_PACKET = (SAMPLE_RATE // 1000) * PACKET_INTERVAL_MS

print(f"DEBUG: {SAMPLES_PER_PACKET} samples per packet, packet size about {BYTES_PER_SAMPLE * SAMPLES_PER_PACKET  + 12} bytes")

# UDP settings
DEST_IP = "127.0.0.1"  # Receiver IP
DEST_PORT = 5004       # Receiver port

# Open a raw PCM file for testing (e.g., 'audio.pcm')
FILE_PATH = "sample.pcm"

# Create UDP socket
udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

def create_rtp_header(sequence_number, timestamp):
    """Create an RTP header."""
    header = struct.pack(
        "!BBHII",
        0x80,            # Version 2, no padding, no extension
        PAYLOAD_TYPE,    # Payload type (dynamic type for L24 PCM)
        sequence_number, # Sequence number
        timestamp,       # Timestamp
        SSRC             # Synchronization Source Identifier
    )
    return header

def send_rtp_packets(file_path):
    """Send RTP packets from a PCM file."""
    sequence_number = 0
    timestamp = 0

    try:
        with open(file_path, "rb") as pcm_file:
            while True:
                # Read audio data for one packet (24-bit PCM = 3 bytes per sample)
                payload = pcm_file.read(SAMPLES_PER_PACKET * BYTES_PER_SAMPLE * CHANNELS)
                if not payload:
                    break  # End of file

                # Create RTP header
                rtp_header = create_rtp_header(sequence_number, timestamp)

                # Combine RTP header and payload
                packet = rtp_header + payload

                # Send RTP packet over UDP
                udp_socket.sendto(packet, (DEST_IP, DEST_PORT))

                # Update sequence number and timestamp
                sequence_number = (sequence_number + 1) % 65536  # Wrap around at 16 bits
                timestamp += SAMPLES_PER_PACKET

                # Wait for next packet interval
                time.sleep(PACKET_INTERVAL_MS / 1000.0)

    except FileNotFoundError:
        print(f"File not found: {file_path}")
    finally:
        udp_socket.close()

# Start sending RTP packets
send_rtp_packets(FILE_PATH)