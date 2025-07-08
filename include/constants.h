/**
 * @file constants.h
 * @brief It is the header file for the constants and flags used in the signature scheme.
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <stdbool.h>

/**
 * @brief It is the modulus used in the signature scheme.
 */
#define MOD 2

/**
 * @brief It is the flag to print the matrices and other information during the execution.
 * 
 * If set to true, the program will print debug information to the output file. If
 * set to false, the program will not print debug information. This can be useful
 * for debugging purposes or to reduce the output size in production runs.
 */
#define PRINT true

/**
 * @brief It is the size of the seed used for random number generation in the signature scheme.
 */
#define SEED_SIZE 32

/**
 * @brief It is the path to the parameters file.
 * 
 * This file contains the parameters for the signature scheme, such as the
 * generator matrices and concatenated codes. The parameters are used to initialize the
 * signature scheme and can be generated or read from this file.
 */
#define PARAM_PATH "params.txt"

/**
 * @brief It is the directory to the output file.
 */
#define OUTPUT_DIR "output"

/**
 * @brief It is the path to the output file.
 * 
 * This file is used to store the output of the signature scheme, such as the
 * generated signatures, debug information, and other relevant data.
 */
#define OUTPUT_PATH OUTPUT_DIR "/output.txt"

/**
 * @brief It is the directory where the matrix cache is stored.
 * 
 * This directory is used to store cached matrices used in the signature scheme. It
 * is useful for storing matrices that are frequently used or generated, allowing
 * for faster access and reducing the need to regenerate them each time the program
 * runs. The cached matrices are in binary format and can be loaded or saved as
 * needed.
 */
#define CACHE_DIR "./matrix_cache/"

/**
 * @brief It is the maximum length of a filename in the signature scheme.
 */
#define MAX_FILENAME_LENGTH 256

#endif /* CONSTANTS_H */