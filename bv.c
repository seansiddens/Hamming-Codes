#include "bv.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

//
// Struct definition of a bit vector ADT.
// length: Length of bit vector (in bits)
// vector: Pointer to an array of bytes which store the bits
//
struct BitVector {
    uint32_t length;
    uint8_t *vector;
};

//
// Constructor for a bit vector
//
// length: length of bit vector (in bits)
//
BitVector *bv_create(uint32_t length) {
    BitVector *v = (BitVector *) calloc(1, sizeof(BitVector));
    if (v) {
        v->length = length;
        uint32_t byte_num = length % 8 == 0 ? length / 8 : (length / 8) + 1;
        v->vector = (uint8_t *) calloc(byte_num, sizeof(uint8_t));
        if (!v->vector) {
            free(v);
            v = NULL;
        }
    }

    return v;
}

//
// Destructor for a bit vector
//
// m: pointer to a pointer to a bit vector to delete
//
void bv_delete(BitVector **v) {
    if (*v && (*v)->vector) {
        free((*v)->vector);
        free(*v);
        *v = NULL;
    }
    return;
}

//
// Returns length of a given bit vector
//
// v: Pointer to a bit vector
//
uint32_t bv_length(BitVector *v) {
    return v->length;
}

//
// Sets the ith bit of the given bit vector
//
// v: Pointer to a bit vector
// i: Index of bit to set
//
void bv_set_bit(BitVector *v, uint32_t i) {
    assert(i < v->length);
    v->vector[i / 8] |= 0x1 << (i % 8);
    return;
}

//
// Clears the ith bit of the given bit vector
//
// v: Pointer to a bit vector
// i: Index of bit to clear
//
void bv_clr_bit(BitVector *v, uint32_t i) {
    assert(i < v->length);
    v->vector[i / 8] &= ~(0x1 << (i % 8));
    return;
}

//
// XOR's the ith bit of the given bitvector with bit
//
// v: Pointer to bit vector
// i: Index of bit to clear
// bit: Bit to XOR with
//
void bv_xor_bit(BitVector *v, uint32_t i, uint8_t bit) {
    assert(i < v->length);
    v->vector[i / 8] ^= bit << (i % 8);
    return;
}

//
// Returns the ith bit of the given bit vector
//
// v: Pointer to the bit vector
// i: Index of bit to get
//
uint8_t bv_get_bit(BitVector *v, uint32_t i) {
    assert(i < v->length);
    return 1 & (v->vector[i / 8] >> i % 8);
}

//
// Prints a bit vector (LSB on left, MSB on right)
//
// v: Pointer to bit vector to print
//
void bv_print(BitVector *v) {
    for (int i = 0; i < (uint8_t) v->length; i++) {
        printf("%c", '0' + bv_get_bit(v, i));
    }
    printf("\n");
    return;
}
