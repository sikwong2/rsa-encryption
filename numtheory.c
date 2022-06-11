#include <stdio.h>
#include <gmp.h>
#include <stdlib.h>
#include "numtheory.h"
#include "randstate.h"
#include <inttypes.h>

void pow_mod(mpz_t out, mpz_t base, mpz_t exponent, mpz_t modulus) {
    mpz_t p, temp, v;
    mpz_init_set(temp, exponent); //temp <-- exponent
    mpz_init_set_ui(v, 1); //v <-- 1
    mpz_init_set(p, base); //p <-- a

    while (mpz_cmp_ui(temp, 0) > 0) { //d > 0
        if (mpz_odd_p(temp) != 0) { //odd(d)
            mpz_mul(v, v, p); //v = v * p
            mpz_mod(v, v, modulus); //v <-- v % n
        }
        mpz_mul(p, p, p); //p = p * p
        mpz_mod(p, p, modulus); //p <-- p^2 % n
        mpz_fdiv_q_ui(temp, temp, 2); //d <-- d/2
    }
    mpz_set(out, v);
    mpz_clears(p, temp, v, NULL);
}

bool is_prime(mpz_t n, uint64_t iters) {
    //check if n is even, if even return false
    if (mpz_cmp_ui(n, 2) == 0) {
        return true;
    }

    if (mpz_even_p(n) == 1) {
        return false;
    }
    mpz_t N, s, r;
    mpz_init_set(N, n); //init temp N as copy of n
    mpz_init_set_ui(s, 0); //init s as 0
    mpz_init(r);
    mpz_sub_ui(N, n, 1);
    while (mpz_even_p(N) > 0) {
        mpz_fdiv_q_ui(N, N, 2);
        mpz_add_ui(s, s, 1);
    }
    mpz_set(r, N);

    mpz_sub_ui(N, n, 3);
    //a is the random number
    //j is to be used in the for loop
    mpz_t j, a, y;
    mpz_init(j);
    mpz_init(a);
    mpz_init(y);
    for (uint64_t i = 1; i < iters; i += 1) {
        //choose a random a {2, n-2}
        mpz_urandomm(a, state, N);
        mpz_add_ui(a, a, 2);

        //y = pow_mod(a, r, n)
        pow_mod(y, a, r, n);
        // if y != 1 and y != n - 1
        if ((mpz_get_ui(y) != 1) && (mpz_get_ui(y) != mpz_get_ui(n) - 1)) {
            mpz_set_ui(j, 1);

            //while j <= s - 1 and y != n - 1
            while ((mpz_get_ui(j) <= mpz_get_ui(s) - 1) && (mpz_get_ui(y) != mpz_get_ui(n) - 1)) {

                mpz_set_ui(N, 2);
                pow_mod(y, y, N, n);
                //if y == 1
                if (mpz_get_ui(y) == 1) {
                    mpz_clears(y, N, a, r, s, j, NULL);
                    return false;
                }
                mpz_add_ui(j, j, 1);
            }
            //if y != n - 1
            if (mpz_get_ui(y) != mpz_get_ui(n) - 1) {
                mpz_clears(y, N, a, r, s, j, NULL);
                return false;
            }
        }
    }
    mpz_clears(y, N, a, r, s, j, NULL);
    return true;
}

void make_prime(mpz_t p, uint64_t bits, uint64_t iters) {
    mpz_t temp;
    mpz_init(temp);
    mpz_ui_pow_ui(temp, 2, bits);

    //p is a number up to 2^n-1 bits. Add 2^n so the number is at least 2^n bits
    mpz_urandomb(p, state, bits);
    mpz_add(p, p, temp);
    while (!(is_prime(p, iters))) {
        mpz_urandomb(p, state, bits);
        mpz_ui_pow_ui(temp, 2, bits);
        mpz_add(p, p, temp);
    }
    mpz_clear(temp);
}

void gcd(mpz_t d, mpz_t a, mpz_t b) {
    mpz_t in1, in2, temp;
    mpz_init(temp);
    mpz_init_set(in1, a);
    mpz_init_set(in2, b);

    while (mpz_cmp_ui(in2, 0) != 0) {
        mpz_set(temp, in2);
        mpz_mod(in2, in1, in2);
        mpz_set(in1, temp);
    }
    mpz_set(d, in1);

    //mpz clear
    mpz_clears(in1, in2, temp, NULL);
}

void mod_inverse(mpz_t i, mpz_t a, mpz_t n) {
    mpz_t r1, r2, t1, t2, q, temp;
    mpz_init_set(r1, n);
    mpz_init_set(r2, a);
    mpz_init_set_ui(t1, 0);
    mpz_init_set_ui(t2, 1);
    mpz_init(q);
    mpz_init(temp);
    while (mpz_cmp_ui(r2, 0) != 0) {
        mpz_fdiv_q(q, r1, r2);

        //1st parallel assignment
        mpz_set(temp, r1);
        mpz_set(r1, r2);
        mpz_set(r2, temp);
        mpz_mul(temp, q, r1);
        mpz_sub(r2, r2, temp);

        //2nd parallel assignment
        mpz_set(temp, t1);
        mpz_set(t1, t2);
        mpz_set(t2, temp);
        mpz_mul(temp, q, t1);
        mpz_sub(t2, t2, temp);
    }
    if (mpz_cmp_ui(r1, 1) > 0) {
        mpz_set_ui(i, 0);

        //mpz clear
        mpz_clears(r1, r2, t1, t2, q, temp, NULL);
        return;
    }
    if (mpz_cmp_ui(t1, 0) < 0) {
        mpz_add(t1, t1, n);
    }
    //mpz_set(i, t1);
    mpz_set(i, t1);
    //mpz clear
    mpz_clears(r1, r2, t1, t2, q, temp, NULL);
}
