#include <stdio.h>
#include <gmp.h>
#include "randstate.h"
#include "numtheory.h"
#include "rsa.h"
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

void usage(char *exec) {
    fprintf(stderr,
        "SYNOPSIS\n"
        "   Generates an RSA public/private key pair.\n"
        "USAGE\n"
        "   %s [-hv] [-b bits] -n pbfile -d pvfile\n"
        "OPTIONS\n"
        "   -h              Display program help and usage.\n"
        "   -v              Display verbose program output.\n"
        "   -b bits         Minimum bits needed for public key n.\n"
        "   -c confidence   Miller-Rabin iterations for testing primes (default: 50).\n"
        "   -n pbfile       Public key file (default: rsa.pub).\n"
        "   -d pvfile       Private key file (default: rsa.priv).\n"
        "   -s seed         Random seed for testing.\n",
        exec);
}

int main(int argc, char **argv) {
    uint64_t b = 256; //min # of bits for public modulus n
    uint64_t i = 50; //# of iterations

    char *pubfile = "rsa.pub";
    char *privfile = "rsa.priv";

    unsigned seed = time(NULL);

    bool verbose = false;

    int opt = 0;

    while ((opt = getopt(argc, argv, "b:i:n:d:s:vh")) != -1) {
        switch (opt) {
        case 'b': b = strtol(optarg, NULL, 10); break;
        case 'i': i = strtol(optarg, NULL, 10); break;
        case 'n': pubfile = optarg; break;
        case 'd': privfile = optarg; break;
        case 's': seed = strtol(optarg, NULL, 10); break;
        case 'v': verbose = true; break;
        case 'h': usage(argv[0]); return EXIT_FAILURE;
        default: usage(argv[0]); return EXIT_FAILURE;
        }
    }
    FILE *pbfile = fopen(pubfile, "w");
    FILE *pvfile = fopen(privfile, "w");

    uint32_t fd = fileno(pvfile);
    fchmod(fd, S_IRUSR | S_IWUSR);

    randstate_init(seed);
    mpz_t p, q, n, e, d, s, m;
    mpz_inits(p, q, n, e, d, s, m, NULL);

    //p is first large prime
    //q is second large prime
    //n is the product of p and q
    //e is the public exponent
    //m is the message
    //d is the private key

    //make pub key
    rsa_make_pub(p, q, n, e, b, i);
    rsa_make_priv(d, e, p, q);

    char *user = getenv("USER");
    mpz_set_str(m, user, 62);
    rsa_sign(s, m, d, n);

    //write pub and priv key
    rsa_write_pub(n, e, s, user, pbfile);
    rsa_write_priv(n, d, pvfile);
    //verbose
    if (verbose) {
        printf("user = %s\n", user);
        gmp_printf("s (%d bits) = %Zd\n", mpz_sizeinbase(s, 2), s);
        gmp_printf("p (%d bits) = %Zd\n", mpz_sizeinbase(p, 2), p);
        gmp_printf("q (%d bits) = %Zd\n", mpz_sizeinbase(q, 2), q);
        gmp_printf("n (%d bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        gmp_printf("e (%d bits) = %Zd\n", mpz_sizeinbase(e, 2), e);
        gmp_printf("d (%d bits) = %Zd\n", mpz_sizeinbase(d, 2), d);
    }

    //clear mem
    randstate_clear();
    fclose(pbfile);
    fclose(pvfile);
    mpz_clears(p, q, n, e, d, s, m, NULL);
    return 0;
}
