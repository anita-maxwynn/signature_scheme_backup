#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <signal.h>
#include <string.h>
#include <inttypes.h>
#include <flint/flint.h>
#include <flint/nmod_mat.h>

#include "matrix.h"
#include "utils.h"
#include "constants.h"
#include "keygen.h"

// ---------- graceful stop on Ctrl+C ----------
static volatile sig_atomic_t g_stop = 0;
static void handle_sigint(int sig) { (void)sig; g_stop = 1; }

// ---------- print __int128 safely ----------
static void fprint_uint128(FILE *f, unsigned __int128 x) {
    if (x == 0) { fputc('0', f); return; }
    char buf[64]; // enough for 2^128-1 (39 digits)
    int idx = 0;
    while (x > 0) {
        unsigned __int128 q = x / 10;
        unsigned int r = (unsigned int)(x - q * 10);
        buf[idx++] = (char)('0' + r);
        x = q;
    }
    while (idx--) fputc(buf[idx], f);
}

// ---------- hex dump helper ----------
static void fprint_hex(FILE *f, const unsigned char *data, size_t len) {
    for (size_t i = 0; i < len; i++) fprintf(f, "%02X", data[i]);
}

// calculate Hamming weight of a row vector
static int calculate_weight(nmod_mat_t codeword) {
    int w = 0;
    for (slong j = 0; j < codeword->c; j++) {
        if (nmod_mat_get_entry(codeword, 0, j) == 1) w++;
    }
    return w;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <m> <t>\n", argv[0]);
        return 1;
    }

    int m = atoi(argv[1]);
    int t = atoi(argv[2]);

    int n = (1 << m) - 1;   // n = 2^m - 1
    int k = m * t;          // k = t * m

    printf("Running codeword generation for m=%d, t=%d (n=%d, k=%d)\n", m, t, n, k);
    printf("⚠️  There are 2^%d possible inputs. This will run until you press Ctrl+C.\n", k);

    // files
    FILE *txt_file = fopen("codewords.txt", "w");
    if (!txt_file) { perror("codewords.txt"); return 1; }

    FILE *G_file = fopen("generator_matrix.txt", "w");
    if (!G_file) {
        perror("generator_matrix.txt");
        fclose(txt_file);
        return 1;
    }

    // initialize generator matrix G (k x n) over mod MOD
    nmod_mat_t G;
    nmod_mat_init(G, k, n, MOD);

    unsigned char seed[SEED_SIZE];
    generate_random_seed(seed);
    create_generator_matrix_from_seed(n, k, 2, G, seed, NULL);

    // dump metadata and seed
    fprintf(G_file, "# Generator matrix G for parameters:\n");
    fprintf(G_file, "# m=%d, t=%d, n=%d, k=%d, MOD=%lu\n", m, t, n, k, (unsigned long)MOD);
    fprintf(G_file, "# seed=");
    fprint_hex(G_file, seed, SEED_SIZE);
    fprintf(G_file, "\n");

    // dump G rows as 0/1 without separators (one row per line)
    for (int r = 0; r < k; r++) {
        for (int c = 0; c < n; c++) {
            fprintf(G_file, "%lu", nmod_mat_get_entry(G, r, c));
        }
        fputc('\n', G_file);
    }
    fflush(G_file); // ensure matrix is fully written

    // input and output vectors
    nmod_mat_t input, output;
    nmod_mat_init(input, 1, k, MOD);
    nmod_mat_init(output, 1, n, MOD);

    // add header to stream file
    fprintf(txt_file, "# m=%d t=%d n=%d k=%d MOD=%lu seed=",
            m, t, n, k, (unsigned long)MOD);
    fprint_hex(txt_file, seed, SEED_SIZE);
    fprintf(txt_file, "\n");

    // handle Ctrl+C cleanly
    signal(SIGINT, handle_sigint);

    // iterate inputs until Ctrl+C
    unsigned __int128 i = 0;
    while (!g_stop) {
        // set input vector bits from i (big-endian bit order)
        for (int j = 0; j < k; j++) {
            int bit = (int)((i >> (k - 1 - j)) & 1);
            nmod_mat_set_entry(input, 0, j, bit);
        }

        // compute output = input * G
        nmod_mat_mul(output, input, G);

        // iter=
        fprintf(txt_file, "iter=");
        fprint_uint128(txt_file, i);
        fprintf(txt_file, "  ");

        // input=
        fprintf(txt_file, "input=");
        for (int j = 0; j < k; j++) {
            fprintf(txt_file, "%lu", nmod_mat_get_entry(input, 0, j));
        }

        // output=
        fprintf(txt_file, "  output=");
        for (int j = 0; j < n; j++) {
            fprintf(txt_file, "%lu", nmod_mat_get_entry(output, 0, j));
        }

        // weight=
        int w = calculate_weight(output);
        fprintf(txt_file, "  weight=%d\n", w);

        fflush(txt_file); // flush every line so Ctrl+C never loses progress

        i++; // next input
    }

    // cleanup
    fprintf(txt_file, "# Interrupted by user. Last iter=");
    fprint_uint128(txt_file, i);
    fprintf(txt_file, "\n");
    fflush(txt_file);

    nmod_mat_clear(input);
    nmod_mat_clear(output);
    nmod_mat_clear(G);

    fclose(G_file);
    fclose(txt_file);

    printf("Stopped. Wrote generator_matrix.txt and codewords.txt\n");
    return 0;
}
