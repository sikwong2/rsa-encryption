#include "rsa.h"
#include "numtheory.h"
#include "randstate.h"
#include <stdio.h>
#include <gmp.h>
#include <stdlib.h>
#include <time.h>
#include <inttypes.h>

void lcm(mpz_t i, mpz_t a, mpz_t b) {
    mpz_t temp;
    mpz_init(temp);

    mpz_mul(temp, a, b);
    mpz_abs(i, temp);
    gcd(temp, a, b);
    mpz_fdiv_q(i, i, temp);

    mpz_clear(temp);
}

void rsa_make_pub(mpz_t p, mpz_t q, mpz_t n, mpz_t e, uint64_t nbits, uint64_t iters) {
    mpz_t temp, temp2, lamda;
    mpz_inits(temp, temp2, lamda, NULL);

    uint64_t pbits = (random() % (nbits / 2) + (nbits / 4));
    uint64_t qbits = nbits - pbits;

    //generate two primes
    make_prime(p, pbits, iters);
    make_prime(q, qbits, iters);

    //generate n
    mpz_mul(n, p, q);

    //generate e
    mpz_sub_ui(temp, p, 1);
    mpz_sub_ui(temp2, q, 1);
    lcm(lamda, temp, temp2);

    mpz_urandomb(e, state, nbits);
    gcd(temp, lamda, e);

    //if number and lamda are not coprime then gen new number
    while (mpz_cmp_ui(temp, 1) != 0) {
        mpz_urandomb(e, state, nbits);
        gcd(temp, lamda, e);
    }

    //mpz clear
    mpz_clears(temp, temp2, lamda, NULL);
}

void rsa_write_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
    gmp_fprintf(pbfile, "%Zx\n", n);
    gmp_fprintf(pbfile, "%Zx\n", e);
    gmp_fprintf(pbfile, "%Zx\n", s);
    fprintf(pbfile, "%s\n", username);
}

void rsa_read_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
    gmp_fscanf(pbfile, "%Zx\n", n);
    gmp_fscanf(pbfile, "%Zx\n", e);
    gmp_fscanf(pbfile, "%Zx\n", s);
    fscanf(pbfile, "%s\n", username);
}

void rsa_make_priv(mpz_t d, mpz_t e, mpz_t p, mpz_t q) {
    mpz_t lamda, temp1, temp2;
    mpz_init(lamda);
    mpz_init(temp1);
    mpz_init(temp2);

    mpz_sub_ui(temp1, p, 1);
    mpz_sub_ui(temp2, q, 1);
    lcm(lamda, temp1, temp2);

    mod_inverse(d, e, lamda);

    //mpz clear
    mpz_clears(lamda, temp1, temp2, NULL);
}

void rsa_write_priv(mpz_t n, mpz_t d, FILE *pvfile) {
    gmp_fprintf(pvfile, "%Zx\n", n);
    gmp_fprintf(pvfile, "%Zx\n", d);
}

void rsa_read_priv(mpz_t n, mpz_t d, FILE *pvfile) {
    gmp_fscanf(pvfile, "%Zx\n", n);
    gmp_fscanf(pvfile, "%Zx\n", d);
}

void rsa_encrypt(mpz_t c, mpz_t m, mpz_t e, mpz_t n) {
    pow_mod(c, m, e, n);
}

void rsa_encrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t e) {
    mpz_t m, c;
    mpz_inits(m, c, NULL);
    uint32_t k = (mpz_sizeinbase(n, 2) - 1) / 8;
    size_t j;
    uint8_t *array = (uint8_t *) calloc(k, sizeof(uint8_t));
    array[0] = 0xFF;

    //fill array with read bytes
    while ((j = fread(&array[1], sizeof(uint8_t), k - 1, infile)) > 0) {

        //convert read bytes to mpz_t
        mpz_import(m, j + 1, 1, sizeof(uint8_t), 1, 0, &array[0]);

        //encrypt message
        rsa_encrypt(c, m, e, n);

        //print message to outfile
        gmp_fprintf(outfile, "%Zx\n", c);
    }

    //clear mem
    mpz_clears(m, c, NULL);
    free(array);
    array = NULL;
}

void rsa_decrypt(mpz_t m, mpz_t c, mpz_t d, mpz_t n) {
    pow_mod(m, c, d, n);
}

void rsa_decrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t d) {
    mpz_t m, c;
    mpz_init(m);
    mpz_init(c);
    uint32_t k = (mpz_sizeinbase(n, 2) - 1) / 8;
    size_t j; //number of bytes converted
    uint8_t *array = (uint8_t *) calloc(k, sizeof(uint8_t));

    //scan hexstring into mpz_t c
    while ((gmp_fscanf(infile, "%Zx\n", c)) != EOF) {

        //decrypt message
        rsa_decrypt(m, c, d, n);

        //convert from mpz_t to bytes
        mpz_export(array, &j, 1, sizeof(uint8_t), 1, 0, m);

        //writing to outfile
        fwrite(&array[1], sizeof(uint8_t), j - 1, outfile);
    }

    //mem clear
    mpz_clears(c, m, NULL);
    free(array);
    array = NULL;
}

void rsa_sign(mpz_t s, mpz_t m, mpz_t d, mpz_t n) {
    pow_mod(s, m, d, n);
}

bool rsa_verify(mpz_t m, mpz_t s, mpz_t e, mpz_t n) {
    mpz_t t;
    mpz_init(t);

    pow_mod(t, s, e, n);

    if (mpz_cmp(t, m) == 0) {
        mpz_clear(t);
        return true;
    }

    mpz_clear(t);
    return false;
}
