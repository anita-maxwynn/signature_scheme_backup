/**
 * @file matrix.h
 * @brief Header filr for declaration of matrix operations and error-correcting code structures for signature scheme.
 * 
 * Header filr for declaration of matrix operations and error-correcting code structures for signature scheme.
 */

#ifndef MATRIX_H
#define MATRIX_H

#include <stdio.h>
#include <flint/nmod_mat.h>

/**
 * @brief code refers to the generator matrix of code 1, code 2 and the parity check matrix H_A's attributes. All the parameters are unsigned long integers. 
 */
struct code {
    unsigned long n; /**< length of the code */
    unsigned long k; /**< length of the message */
    unsigned long d; /**< minimum distance between 2 codewords */
};

/**
 * @brief Prints the contents of a matrix to a file stream.
 */
void print_matrix(FILE *fp, nmod_mat_t matrix);

/**
 * @brief Computes the transpose of a matrix.
 */
void transpose_matrix(int rows, int cols, int matrix[rows][cols], int transpose[cols][rows]);

/**
 * @brief Performs matrix multiplication over GF(2).
 */
void multiply_matrices_gf2(nmod_mat_t C, const nmod_mat_t A, const nmod_mat_t B);

/**
 * @brief Makes a matrix systematic form.
 */
void make_systematic(unsigned long n, unsigned long k, nmod_mat_t H);

/**
 * @brief Transforms a matrix into Reduced Row Echelon Form.
 */
void rref(int num_rows, int num_cols, int (*H)[num_cols]);

#endif /* MATRIX_H */