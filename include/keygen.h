/**
 * @file keygen.h
 * @brief It is the header file for the key generation module.
 */

#ifndef KEYGEN_H
#define KEYGEN_H

#include <flint/nmod_mat.h>
#include <stdbool.h>
#include "matrix.h"

/**
 * @brief Generates a random seed for matrix generation
 */
void generate_random_seed(unsigned char *seed);

/**
 * @brief Creates a generator matrix from seed.
 */
void create_generator_matrix_from_seed(slong n, slong k, slong d,
                                       nmod_mat_t gen_matrix,
                                       const unsigned char *seed,
                                       FILE *output_file);

/**
 * @brief Generates a parity check matrix from a seed.
 */
void generate_parity_check_matrix_from_seed(slong n, slong k, slong d, nmod_mat_t H, 
                                           const unsigned char *seed, FILE *output_file);

/**
 * @brief Gets or generates matrix with seed.
 */
void get_or_generate_matrix_with_seed(const char* prefix, int n, int k, int d, nmod_mat_t matrix,
                                     void (*generate_func)(slong, slong, slong, nmod_mat_t, FILE*),
                                     void (*generate_from_seed_func)(slong, slong, slong, nmod_mat_t, const unsigned char*, FILE*),
                                     FILE* output_file, bool regenerate, bool use_seed_mode, 
                                     unsigned char *seed_out);

/**
 * @brief Generates keys for a signature scheme.
 */
void generate_keys(struct code* C_A, struct code* C1, struct code* C2,
                   nmod_mat_t H_A, nmod_mat_t G1, nmod_mat_t G2,
                   bool use_seed_mode, bool regenerate, FILE* output_file,
                   unsigned char* h_a_seed, unsigned char* g1_seed, unsigned char* g2_seed);

#endif