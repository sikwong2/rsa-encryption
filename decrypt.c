#include <stdio.h>
#include <gmp.h>
#include "rsa.h"
#include "numtheory.h"
#include "randstate.h"
#include <unistd.h>
#include <stdlib.h>

void usage(char *exec) {
    fprintf(stderr,
        "SYNOPSIS\n"
        "   Decrypts data using RSA decryption.\n"
        "   Encrypted data is encrypted by the encrypt program.\n"
        "USAGE\n"
        "   %s [-hv] [-i infile] [-o outfile] -d privkey\n"
        "OPTIONS\n"
        "   -h              Display program help and usage.\n"
        "   -v              Display verbose program output.\n"
        "   -i infile       Input file of data to decrypt (default: stdin).\n"
        "   -o outfile      Output file for decrypted data (default: stdout).\n"
        "   -d pvfile       Private key file (default: rsa.priv).\n",
        exec);
}

int main(int argc, char **argv) {
    FILE *infile = stdin;
    FILE *outfile = stdout;
    char *privfile = "rsa.priv";

    int opt = 0;

    bool verbose = false;

    while ((opt = getopt(argc, argv, "i:o:d:vh")) != -1) {
        switch (opt) {
        case 'i': infile = fopen(optarg, "r"); break;
        case 'o': outfile = fopen(optarg, "w"); break;
        case 'd': privfile = optarg; break;
        case 'v': verbose = true; break;
        case 'h': usage(argv[0]); return EXIT_FAILURE;
        default: usage(argv[0]); return EXIT_FAILURE;
        }
    }

    //open priv key file
    FILE *pvfile = fopen(privfile, "r");
    if (pvfile == NULL) {
        fprintf(stderr, "Error opening %s\n", privfile);
    }

    //scan priv file
    mpz_t n, d;
    mpz_inits(n, d, NULL);
    rsa_read_priv(n, d, pvfile);

    if (verbose) {
        gmp_printf("n (%d bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        gmp_printf("d (%d bits) = %Zd\n", mpz_sizeinbase(d, 2), d);
    }
    //decrypt file
    rsa_decrypt_file(infile, outfile, n, d);

    //clearing mem
    fclose(pvfile);
    fclose(infile);
    fclose(outfile);

    mpz_clears(n, d, NULL);

    return 0;
}
