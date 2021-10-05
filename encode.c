#include "bm.h"
#include "bv.h"
#include "hamming.h"
#include "util.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define OPTIONS "hi:o:"

//
// Prints the program usage and help message
//
static void print_help(void) {
    printf("SYNOPSIS\n");
    printf("   A Hamming (8, 4) systematic code generator.\n");
    printf("USAGE\n");
    printf("   ./encode [-h] [-i infile] [-o outfile]\n");
    printf("OPTIONS\n");
    printf("  -h             Program usage and help.\n");
    printf("  -i infile      Input data to encode.\n");
    printf("  -o outfile     Output of encoded data.\n");
    return;
}

//
// Creates a generator matrix for encoding
//
static BitMatrix *create_generator_matrix(void) {
    BitMatrix *G = bm_create(4, 8);
    bm_set_bit(G, 0, 0);
    bm_set_bit(G, 0, 5);
    bm_set_bit(G, 0, 6);
    bm_set_bit(G, 0, 7);
    bm_set_bit(G, 1, 1);
    bm_set_bit(G, 1, 4);
    bm_set_bit(G, 1, 6);
    bm_set_bit(G, 1, 7);
    bm_set_bit(G, 2, 2);
    bm_set_bit(G, 2, 4);
    bm_set_bit(G, 2, 5);
    bm_set_bit(G, 2, 7);
    bm_set_bit(G, 3, 3);
    bm_set_bit(G, 3, 4);
    bm_set_bit(G, 3, 5);
    bm_set_bit(G, 3, 6);
    return G;
}

int main(int argc, char *argv[]) {
    // Argument flags
    bool HELP = false;

    // Initialize default values
    char *infile_name = NULL;
    char *outfile_name = NULL;
    FILE *infile = stdin;
    FILE *outfile = stdout;

    // Process command line arguments
    int opt = 0;
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'h': HELP = true; break;
        case 'i': infile_name = strdup(optarg); break;
        case 'o': outfile_name = strdup(optarg); break;
        default: HELP = true; break;
        }
    }

    // If help option is supplied, print help message and exit program
    if (HELP) {
        print_help();
        free(infile_name);
        free(outfile_name);
        return 0;
    }

    // If an input file name is supllied, open the file stream for reading
    if (infile_name != NULL) {
        if ((infile = fopen(infile_name, "r")) == NULL) {
            fprintf(stderr, "Invalid file name!\n");
            exit(1);
        }
    }

    // Only change output permissions if an ouput file is supplied and an input
    // file is supplied.
    if ((outfile_name != NULL) && (infile_name != NULL)) {
        outfile = fopen(outfile_name, "w");

        // Fetch infile permissions and change output permissions to match
        struct stat statbuf;
        fstat(fileno(infile), &statbuf);
        fchmod(fileno(outfile), statbuf.st_mode);
    }

    // Initialize generator matrix
    BitMatrix *G = create_generator_matrix();

    // Read input bytes of input
    int input_byte; // fgetc() returns a uint8_t cast to an int
    while ((input_byte = fgetc(infile)) != EOF) {
        // Get message nibbles from byte
        uint8_t msg1 = lower_nibble((uint8_t) input_byte);
        uint8_t msg2 = upper_nibble((uint8_t) input_byte);

        // Generate (8, 4) hamming code from nibbles
        uint8_t code1 = ham_encode(G, msg1);
        uint8_t code2 = ham_encode(G, msg2);

        // Write codes to output, lower nibble first
        fputc(code1, outfile);
        fputc(code2, outfile);
    }

    // Close file streams and free data before exiting
    fclose(infile);
    fclose(outfile);
    free(infile_name);
    free(outfile_name);
    bm_delete(&G);
    return 0;
}
