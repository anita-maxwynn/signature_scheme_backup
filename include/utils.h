/**
 * @file utils.h
 * @brief Header file for utility functions used in the signature scheme.
 */

#ifndef UTILS_H
#define UTILS_H

#include <math.h>
#include <stdlib.h>
#include <sodium.h>
#include <stdbool.h>
#include <flint/flint.h>
#include <flint/nmod_mat.h>
#include "matrix.h"

/**
 * @brief Calculates the Hamming weight of a matrix row.
 */
long weight(nmod_mat_t array);

/**
 * @brief Calculates the binary entropy of a probability.
 */
double binary_entropy(double p);

/**
 * @brief Generates a random set of unique integers within a specified range.
 */
void generate_random_set(unsigned long upper_bound, unsigned long size, unsigned long set[size]);

/**
 * @brief Generates a filename for a matrix based on prefix and dimensions.
 */
char* generate_matrix_filename(const char* prefix, int n, int k, int d);

/**
 * @brief Saves a matrix to a text file in FLINT format.
 */
void save_matrix(const char* filename, const nmod_mat_t matrix);

/**
 * @brief Loads a matrix from a text file in FLINT format.
 */
int load_matrix(const char* filename, nmod_mat_t matrix);

/**
 * @brief Checks if a file exists.
 */
int file_exists(const char* filename);

/**
 * @brief Generates a filename for a seed based on prefix and parameters.
 */
char* generate_seed_filename(const char* prefix, int n, int k, int d);

/**
 * @brief Saves a seed to a binary file.
 */
bool save_seed(const char* filename, const unsigned char *seed);

/**
 * @brief Loads a seed from a binary file.
 */
bool load_seed(const char* filename, unsigned char *seed);

/**
 * @brief Reads the contents of a file into a string.
 */
char *read_file(const char *filename);

/**
 * @brief Reads a message from file or generates random message.
 */
char *read_file_or_generate(const char *filename, int msg_len);

/**
 * @brief Loads parameters for codes from a file.
 */
bool load_params(struct code *C_A, struct code *C1, struct code *C2);

/**
 * @brief Ensures matrix cache directory exists.
 */
void ensure_matrix_cache();

/**
 * @brief Ensures output directory exists.
 */
void ensure_output_directory();

/**
 * @brief Normalizes message length by padding or truncating.
 */
char *normalize_message_length(const char *msg, size_t msg_len, size_t target_len, size_t *final_len_out);

#endif