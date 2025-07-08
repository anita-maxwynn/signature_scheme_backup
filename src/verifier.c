/**
 * @file verifier.c
 * @brief This file contains the implementation of the signature verification function.
 * 
 * This file contains the implementation of the signature verification function. It verifies a digital signature by checking if 
 * the product of the binary hash of the message and the generator matrix equals the product of the parity-check matrix and the transposed signature. It uses the 
 * FLINT library for matrix operations and prints debug information.
 */

#include <stdio.h>
#include <stdbool.h>
#include "verifier.h"
#include "matrix.h"
#include "utils.h"
#include "constants.h"

/**
 * @brief Verifies a digital signature.
 * 
 * This function verifies a digital signature by checking if the product of the binary hash of the message and the generator matrix equals the product of the parity-check matrix and the transposed signature. It uses the FLINT library for matrix operations and prints debug information.
 * 
 * @param bin_hash Binary hash matrix for the message
 * @param message_len Length of the message
 * @param sig_len Length of the signature
 * @param signature Signature matrix to verify
 * @param F Combined matrix F = H_A * G*^T
 * @param C_A Code parameters for the concatenated code
 * @param H_A Parity check matrix for the concatenated code
 * @param output_file File to write verification output to
 */
void verify_signature(nmod_mat_t bin_hash, size_t message_len,
                      unsigned long sig_len, nmod_mat_t signature,
                      nmod_mat_t F, struct code C_A,
                      nmod_mat_t H_A, FILE *output_file)
{
    nmod_mat_t hash_T;
    nmod_mat_init(hash_T, message_len, 1, MOD);
    nmod_mat_transpose(hash_T, bin_hash);

    if (PRINT) {
        fprintf(output_file, "\nHash:\n\n");
        print_matrix(output_file, hash_T);
    }

    nmod_mat_t left;
    nmod_mat_init(left, F->r, 1, MOD);
    nmod_mat_mul(left, F, hash_T);
    fprintf(output_file, "\nLHS:\n\n");
    print_matrix(output_file, left);

    nmod_mat_t sig_T;
    nmod_mat_init(sig_T, sig_len, 1, MOD);
    nmod_mat_transpose(sig_T, signature);

    nmod_mat_t right;
    nmod_mat_init(right, C_A.n - C_A.k, 1, MOD);
    nmod_mat_mul(right, H_A, sig_T);
    fprintf(output_file, "\nRHS:\n\n");
    print_matrix(output_file, right);
    
    fprintf(output_file, "\nVerified: %s", (nmod_mat_equal(left, right)) ? "True" : "False");

    nmod_mat_clear(hash_T);
    nmod_mat_clear(left);
    nmod_mat_clear(sig_T);
    nmod_mat_clear(right);
}
