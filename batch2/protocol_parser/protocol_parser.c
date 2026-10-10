/*
### Exercise 4: Custom Bitwise Protocol Parser with Type-Length-Value (TLV) Encoding
* **Concept:** Bitwise operations, shifting, masking, endianness handling, packing data, and parsing untrusted byte streams.
* **Security Relevance:** Network security research, protocol reverse engineering, and finding integer overflow or buffer overflow bugs in custom network packet parsers.
* **The Task:** Implement a secure parser for a custom binary network protocol using a Type-Length-Value (TLV) format.
  * Define a binary packet format where each message consists of a 1-byte Type, a 2-byte Length (Network Byte Order / Big Endian), and a variable-length Value payload.
  * Write a function `int parse_tlv_packet(const uint8_t *stream, size_t stream_len, TLVPacket *out_packet);` that validates bounds safely before extracting fields.
* **Detailed Steps to Follow:**
  1. **Endianness Conversion:** Implement manual conversion functions or use standard macros (`ntohs`, `htons`) to handle 16-bit and 32-bit integers correctly regardless of host machine architecture (Little Endian vs. Big Endian).
  2. **Bounds Checking (Crucial for Security):** Before reading the Length field or copying the Value payload into a buffer, verify that `stream_len` is large enough to contain the header and the specified length. Explicitly check for integer overflows if lengths are added together (e.g., `header_len + payload_len`).
  3. **Robust Error Handling:** Return specific negative error codes for truncated packets, unknown types, or oversized payloads rather than crashing or reading out-of-bounds memory.
*/

#include <stdio.h>
#include <stdint.h>

typedef struct TLVPacket {
    uint8_t type;
    uint16_t length;
    const uint8_t *value;
} TLVPacket;

typedef enum Endianess {
    BIG_ENDIAN,
    LITTLE_ENDIAN,
} Endianess;

// Check endianess of the current machine (it looks like it is not needed after all)
Endianess check_endianess() {
    uint16_t x = 0x0001;
    uint8_t *ptr = (uint8_t *)&x;

    if (*ptr == 0x00) {
        return BIG_ENDIAN;
    }

    return LITTLE_ENDIAN;
}

int parse_tlv_packet(const uint8_t *stream, size_t stream_len, TLVPacket *out_packet) {
    
    size_t header_size = sizeof(out_packet->type)+sizeof(out_packet->length);

    if (stream_len < header_size) {
        return -1; // Truncated packet
    }
    
    out_packet->type = stream[0];
    out_packet->length = (uint16_t) (stream[1] << 8 | stream[2]); // This should be ok also if my machine is little endian
    
    if (out_packet->length > stream_len-header_size) {
        return -2; // Packet is bigger than expected
    }

    out_packet->value = stream + header_size;
    return 0;
}

int main() {
    TLVPacket pkt;

    // --- Test 1: Valid Packet ---
    // Type: 0x01, Length: 0x0004 (4 bytes), Value: "ABCD" (4 bytes)
    // Total stream length = 3 bytes header + 4 bytes payload = 7 bytes
    uint8_t valid_stream[] = { 0x01, 0x00, 0x04, 'A', 'B', 'C', 'D' };
    
    int result = parse_tlv_packet(valid_stream, sizeof(valid_stream), &pkt);
    if (result == 0) {
        printf("[PASS] Test 1: Valid packet parsed successfully!\n");
        printf("       Type: 0x%02X, Length: %u\n", pkt.type, pkt.length);
    } else {
        printf("[FAIL] Test 1: Unexpected error code %d\n", result);
    }

    // --- Test 2: Truncated Header ---
    // Only 2 bytes provided, but header requires 3
    uint8_t short_stream[] = { 0x01, 0x00 };
    
    result = parse_tlv_packet(short_stream, sizeof(short_stream), &pkt);
    if (result == -1) {
        printf("[PASS] Test 2: Correctly caught truncated header (returned -1).\n");
    } else {
        printf("[FAIL] Test 2: Expected -1, got %d\n", result);
    }

    // --- Test 3: Oversized Length Claim ---
    // Claims a length of 10 bytes (0x000A), but only 2 payload bytes are available
    uint8_t oversized_stream[] = { 0x01, 0x00, 0x0A, 'X', 'Y' };
    
    result = parse_tlv_packet(oversized_stream, sizeof(oversized_stream), &pkt);
    if (result == -2) {
        printf("[PASS] Test 3: Correctly caught oversized packet (returned -2).\n");
    } else {
        printf("[FAIL] Test 3: Expected -2, got %d\n", result);
    }

    return 0;
}
