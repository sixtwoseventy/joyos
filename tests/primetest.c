/*
 * The MIT License
 *
 * Copyright (c) 2007 MIT 6.270 Robotics Competition
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <math.h>
#include <uart.h>
#include <global.h>
#include <thread.h> // DEBUG

#define N_PRIMES 100

const uint16_t real_primes[] = {
    2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59,
    61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113,
    127, 131, 137, 139, 149, 151, 157, 163, 167, 173,
    179, 181, 191, 193, 197, 199, 211, 223, 227, 229,
    233, 239, 241, 251, 257, 263, 269, 271, 277, 281,
    283, 293, 307, 311, 313, 317, 331, 337, 347, 349,
    353, 359, 367, 373, 379, 383, 389, 397, 401, 409,
    419, 421, 431, 433, 439, 443, 449, 457, 461, 463,
    467, 479, 487, 491, 499, 503, 509, 521, 523, 541 };

uint8_t is_prime (uint16_t p) {
    for (int i = 2; i <= sqrt(p); i++) {
        if (!(p % i))
            return 0;
    }

    return 1;
}

int primetest (void) {
    uint8_t i = 0;
    uint16_t x = 2;
    uint16_t primes[N_PRIMES];

    printf("begin prime test\n");
    printf("computing %d primes...\n", N_PRIMES);
    while (i < N_PRIMES) {
        if (is_prime(x)) {
            //uart_printf(" %d is prime\n", x);
            primes[i++] = x;
        }
        x++;
    }

    printf("checking %d primes...\n", i);
    for (uint8_t j = 0; j < i; j++) {
        if (real_primes[j] != primes[j]) {
            printf("prime (index %u) %d does not match expected %d\n",
                    j, primes[j], real_primes[j]);
            panic ("primetest failed");
        }
    }

    printf("passed primetest!\n");
    return 0;
}
