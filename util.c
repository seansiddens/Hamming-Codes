#include "util.h"

//
// Returns the lower nibble of val
//
uint8_t lower_nibble(uint8_t val) {
    return val & 0x0f;
}

//
// Returns the upper nibble of val
//
uint8_t upper_nibble(uint8_t val) {
    return val >> 4;
}

//
// Packs two nibbles into a byte
//
uint8_t pack_byte(uint8_t upper, uint8_t lower) {
    return (upper << 4) | (lower & 0xF);
}
