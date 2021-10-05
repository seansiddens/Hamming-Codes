#include "hamming.h"

#include "bm.h"
#include "util.h"

#include <stdio.h>

int8_t ham_lookup[] = { HAM_OK, 4, 5, HAM_ERR, 6, HAM_ERR, HAM_ERR, 3, 7, HAM_ERR, HAM_ERR, 2,
    HAM_ERR, 1, 0, HAM_ERR };

//
// Generates an (8, 4) hamming code given a nibble of data stored in lower nibble
// of msg using the generator matrix G. Returns the generated hamming code
//
uint8_t ham_encode(BitMatrix *G, uint8_t msg) {
    // Create message vector from lower nibble of msg
    BitMatrix *m = bm_from_data(msg, 4);

    // Create an (8, 4) hamming code from vector-matrix multiplication
    BitMatrix *c = bm_multiply(m, G);

    uint8_t code = bm_to_data(c); // Return code as a byte of data

    // Delete matrices before returning
    bm_delete(&m);
    bm_delete(&c);

    return code;
}

//
// Decodes the Hamming code using the transposed parity-checker matrix Ht.
//
HAM_STATUS ham_decode(BitMatrix *Ht, uint8_t code, uint8_t *msg) {
    // Create code vector from the code data
    BitMatrix *c = bm_from_data(code, 8);
    //bm_print(c);

    // Get error syndrome vector from multiplying code w/ matrix
    BitMatrix *e = bm_multiply(c, Ht);
    //bm_print(e);

    // Index into lookup table w/ syndrome
    int8_t lookup_val = ham_lookup[bm_to_data(e)];
    //printf("Lookup val: %d\n", lookup_val);
    if ((lookup_val == HAM_OK) || (lookup_val == HAM_ERR)) {
        // Either there was no error, or we can't fix the error
        // Either way, code isn't modified
        *msg = lower_nibble(bm_to_data(c));

        bm_delete(&c);
        bm_delete(&e);
        return lookup_val;
    } else {
        //printf("Lookup val: %d\n", lookup_val);
        if (bm_get_bit(c, 0, lookup_val)) {
            // Bit was incorrectly on, so it needs to be cleared
            bm_clr_bit(c, 0, lookup_val);

        } else {
            // Bit was incorrectly off, so it needs to be turned on
            bm_set_bit(c, 0, lookup_val);
        }

        *msg = lower_nibble(bm_to_data(c)); // Pass back message through pointer
        bm_delete(&c);
        bm_delete(&e);
        return HAM_CORRECT;
    }
}
