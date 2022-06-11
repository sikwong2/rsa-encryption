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
        "   Encrypts data using RSA encryption.\n"
        "   Encrypted data is decrypted by the decrypt program.\n"
        "USAGE\n"
        "   %s [-hv] [-i infile] [-o outfile] -n pubkey -d privkey\n"
        "OPTIONS\n"
        "   -h              Display program help and usage.\n"
        "   -v              Display verbose program output.\n"
        "   -i infile       Input file of data to encrypt (default: stdin).\n"
        "   -o outfile      Output file for encrypted data (default: stdout\n"
        "   -n pbfile       Public key file (default: rsa.pub).\n",
        exec);
}
int main(int argc, char **argv) {
    FILE *infile = stdin;
    FILE *outfile = stdout;
    char *pubfile = "rsa.pub";

    int opt = 0;

    bool verbose = false;

    while ((opt = getopt(argc, argv, "i:o:n:vh")) != -1) {
        switch (opt) {
        case 'i': infile = fopen(optarg, "r"); break;
        case 'o': outfile = fopen(optarg, "w"); break;
        case 'n': pubfile = optarg; break;
        case 'v': verbose = true; break;
        case 'h': usage(argv[0]); return EXIT_FAILURE;
        default: usage(argv[0]); return EXIT_FAILURE;
        }
    }

    //open pub key file
    FILE *pbfile = fopen(pubfile, "r");
    if (pbfile == NULL) {
        fprintf(stderr, "Error opening %s.\n", pubfile);
    }

    //scan pub file
    mpz_t n, e, s, m;
    mpz_inits(n, e, s, m, NULL);
    char username[256] = "user";

    rsa_read_pub(n, e, s, username, pbfile);

    if (verbose) {
        printf("user = %s\n", username);
        gmp_printf("s (%d bits) = %Zd\n", mpz_sizeinbase(s, 2), s);
        gmp_printf("n (%d bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        gmp_printf("e (%d bits) = %Zd\n", mpz_sizeinbase(e, 2), e);
    }

    //signature verify
    mpz_set_str(m, username, 62);
    if (!rsa_verify(m, s, e, n)) {
        fprintf(stderr, "Signature couldn't be verified\n");
        return EXIT_FAILURE;
    }

    //encrypt file
    rsa_encrypt_file(infile, outfile, n, e);

    mpz_clears(n, e, s, m, NULL);
    fclose(pbfile);
    fclose(infile);
    fclose(outfile);
    return 0;
}
