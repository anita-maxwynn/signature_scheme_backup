/**
 * @file verifier.h
 * @brief It is the header file for the verifier module.
 */

#ifndef VERIFIER_H
#define VERIFIER_H

#include <flint/nmod_mat.h>
#include <stdio.h>
#include "matrix.h"

/**
 * @brief Verifies a digital signature.
 */
void verify_signature(nmod_mat_t bin_hash, size_t message_len,
                      unsigned long sig_len, nmod_mat_t signature,
                      nmod_mat_t F, struct code C_A,
                      nmod_mat_t H_A, FILE *output_file);

#endif