#include "bm.h"
#include "bv.h"
#include "hamming.h"
#include "util.h"

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define OPTIONS "hvi:o:"

//
// Prints the program usage and help message
//
static void print_help(void) {
    printf("SYNOPSIS\n");
    printf("   A Hamming (8, 4) systematic code decoder.\n");
    printf("USAGE\n");
    printf("   ./decode [-h] [-v] [-i infile] [-o outfile]\n");
    printf("OPTIONS\n");
    printf("  -h             Program usage and help.\n");
    printf("  -v             Print statistics of decoding to stderr.\n");
    printf("  -i infile      Input data to decode.\n");
    printf("  -o outfile     Output of decoded data.\n");
    return;
}

//
// Creates the transpose of the parity checker matrix for decoding
//
static BitMatrix *create_parity_checker_matrix(void) {
    BitMatrix *Ht = bm_create(8, 4);
    bm_set_bit(Ht, 0, 1);
    bm_set_bit(Ht, 0, 2);
    bm_set_bit(Ht, 0, 3);
    bm_set_bit(Ht, 1, 0);
    bm_set_bit(Ht, 1, 2);
    bm_set_bit(Ht, 1, 3);
    bm_set_bit(Ht, 2, 0);
    bm_set_bit(Ht, 2, 1);
    bm_set_bit(Ht, 2, 3);
    bm_set_bit(Ht, 3, 0);
    bm_set_bit(Ht, 3, 1);
    bm_set_bit(Ht, 3, 2);
    bm_set_bit(Ht, 4, 0);
    bm_set_bit(Ht, 5, 1);
    bm_set_bit(Ht, 6, 2);
    bm_set_bit(Ht, 7, 3);
    return Ht;
}

int main(int argc, char *argv[]) {
    // Argument flags
    bool HELP = false;
    bool VERBOSE = false;

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
        case 'v': VERBOSE = true; break;
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
    BitMatrix *Ht = create_parity_checker_matrix();

    // Vars for statistics
    uint64_t total_bytes = 0;
    uint64_t uncorrected_errs = 0;
    uint64_t corrected_errs = 0;
    HAM_STATUS status;

    // Read pairs of bytes from input
    int code_byte1;
    int code_byte2;
    while (true) {
        // Read in bytes until we reach EOF.
        if ((code_byte1 = fgetc(infile)) == EOF) {
            break;
        } else if ((code_byte2 = fgetc(infile)) == EOF) {
            break;
        }

        total_bytes += 2;

        // Decode first byte
        uint8_t msg_lower = 0;
        status = ham_decode(Ht, code_byte1, &msg_lower);
        if (status == HAM_ERR) {
            uncorrected_errs += 1;
        } else if (status == HAM_CORRECT) {
            corrected_errs += 1;
        }

        // Decode second byte
        uint8_t msg_upper = 0;
        status = ham_decode(Ht, code_byte2, &msg_upper);
        if (status == HAM_ERR) {
            uncorrected_errs += 1;
        } else if (status == HAM_CORRECT) {
            corrected_errs += 1;
        }

        fputc(pack_byte(msg_upper, msg_lower), outfile);
    }

    // Print statistics if verbose flag is enabled
    if (VERBOSE) {
        fprintf(stderr, "Total bytes processed: %" PRIu64 "\n", total_bytes);
        fprintf(stderr, "Uncorrected errors: %" PRIu64 "\n", uncorrected_errs);
        fprintf(stderr, "Correct errors: %" PRIu64 "\n", corrected_errs);
        fprintf(stderr, "Error rate: %.6f\n", uncorrected_errs / (double) total_bytes);
    }

    // Close file streams and free data before exiting
    fclose(infile);
    fclose(outfile);
    free(infile_name);
    free(outfile_name);
    bm_delete(&Ht);
    return 0;
}
