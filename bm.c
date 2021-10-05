#include "bm.h"

#include "bv.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

//
// Struct definition for a rows by cols bit matrix. The matrix is flattened
// and the data is stored in a single bit vector of length rows * cols
//
// rows: Number of rows of the bit matrix.
// cols: Number of columns of the bit matrix.
// vector: Pointer to the bit vector storing the data of the matrix
//
struct BitMatrix {
    uint32_t rows;
    uint32_t cols;
    BitVector *vector;
};

//
// Constructor for bit matrix
//
// rows: Number of rows for the bit matrix.
// cols: Number of columns of the bit matrix.
//
BitMatrix *bm_create(uint32_t rows, uint32_t cols) {
    BitMatrix *m = (BitMatrix *) calloc(1, sizeof(BitMatrix));
    if (m) {
        m->rows = rows;
        m->cols = cols;
        m->vector = bv_create(rows * cols);
        if (!m->vector) {
            free(m);
            m = NULL;
        }
    }
    return m;
}

//
// Destructor for a bit matrix
//
// m: Pointer to a pointer to a bit matrix
//
void bm_delete(BitMatrix **m) {
    if (*m && (*m)->vector) {
        bv_delete(&((*m)->vector));
        free(*m);
        *m = NULL;
    }
    return;
}

//
// Returns the number of rows of a bit matrix
//
// m: Pointer to a bit matrix
//
uint32_t bm_rows(BitMatrix *m) {
    return m->rows;
}

//
// Returns the number of columns of a bit matrix
//
// m: Pointer to a bit matrix
//
uint32_t bm_cols(BitMatrix *m) {
    return m->cols;
}

//
// Sets the specified bit of the bit matrix
//
// m: Pointer to bit matrix
// r: Row of bit to set
// c: Column of bit to set
//
void bm_set_bit(BitMatrix *m, uint32_t r, uint32_t c) {
    //printf("i: %u\n", r * m->cols + c);
    bv_set_bit(m->vector, r * m->cols + c);
    return;
}

//
// Returns the specified bit of the bit matrix
//
// m: Pointer to bit matrix
// r: Row of bit to return
// c: Column of bit to return
//
uint8_t bm_get_bit(BitMatrix *m, uint32_t r, uint32_t c) {
    return bv_get_bit(m->vector, r * m->cols + c);
}

//
// Clears the specified bit of the bit matrix
//
// m: Pointer to bit matrix
// r: Row of bit to clear
// c: Column of bit to clear
//
void bm_clr_bit(BitMatrix *m, uint32_t r, uint32_t c) {
    bv_clr_bit(m->vector, r * m->cols + c);
    return;
}

//
// Creates a 1 x length bit matrix from the data stored in a byte. Length
// should not be greater than 8.
//
// byte: Value of byte determines data in the bit matrix
// length: length of matrix to make
//
BitMatrix *bm_from_data(uint8_t byte, uint32_t length) {
    assert(length <= 8);
    BitMatrix *m = bm_create(1, length);
    for (uint8_t i = 0; i < length; i++) {
        // XOR bit of matrix w/ corresponding bit of the data byte
        bv_xor_bit(m->vector, i, (byte >> i) & 1);
    }
    return m;
}

//
// Extracts the first 8 bits of the bit matrix and returns it as a uint8_t
//
// m: Pointer to bit matrix
//
uint8_t bm_to_data(BitMatrix *m) {
    uint8_t byte = 0; // Byte value to return
    uint8_t length = m->cols < 8 ? m->cols : 8;
    for (uint8_t i = 0; i < length; i++) {
        // Fetch i-th bit of matrix and set i-th bit of byte to that value
        byte |= (bm_get_bit(m, 0, i) << i);
    }
    return byte;
}

//
// Multiplies bit matrix A and B together
//
BitMatrix *bm_multiply(BitMatrix *A, BitMatrix *B) {
    assert(A->cols == B->rows); // Cols of A must equal rows of B
    BitMatrix *m = bm_create(A->rows, B->cols);

    // Compute each entry of the product matrix
    for (uint8_t r = 0; r < m->rows; r++) {
        for (uint8_t c = 0; c < m->cols; c++) {
            uint8_t entry = 0;
            // (r, c)-entry is dot product of r-th row of A and c-th col of B (mod 2)
            for (uint8_t i = 0; i < A->cols; i++) {
                entry ^= (bm_get_bit(A, r, i) & bm_get_bit(B, i, c));
            }
            // m is init as zero matrix; 0 XOR'd w/ 0 is 0, 0 XOR'd w/ 1 is 1
            bv_xor_bit(m->vector, r * m->cols + c, entry);
        }
    }

    return m;
}

//
// Prints out the bit matrix
//
// m: Pointer to the bit matrix.
//
void bm_print(BitMatrix *m) {
    // Print top indices
    printf("     ");
    for (int i = 0; i < (int) m->cols; i++) {
        printf("%3d", i);
    }
    printf("\n");

    // Print top dividing line
    printf("    ┌");
    for (int i = 0; i < (int) m->cols; i++) {
        printf("───");
    }
    printf("──┐\n");

    // Print left indices and content of matrix
    for (int r = 0; r < (int) m->rows; r++) {
        printf("%3d │", r);
        for (int c = 0; c < (int) m->cols; c++) {
            printf("%3u", bv_get_bit(m->vector, r * m->cols + c));
        }
        printf("  │\n");
    }

    // Print bottom dividing line
    printf("    └");
    for (int i = 0; i < (int) m->cols; i++) {
        printf("───");
    }
    printf("──┘\n");

    return;
}
