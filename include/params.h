/**
 * @file params.h
 * @brief Header file for declaration of parameter handling functions and Params structure.
 * 
 * Header file for declaration of parameter handling functions and Params structure.
 */

#ifndef PARAMS_H
#define PARAMS_H

#include <stdint.h>
#include <stdbool.h>
#include "utils.h"

/**
 * @brief the derived datatype of the codes. it has the 3 parameters n, k, and d. n: length of the codeword, k: length of the message, d: minimum distance of the code.
 */
typedef struct {
    uint32_t n; /**< Length of the code */
    uint32_t k; /**< Length of the message */
    uint32_t d; /**< Minimum distance of the code */
} Params;

/**
 * @brief Initializes the libsodium library.
 */
void init_params(void);

/**
 * @brief Gets yes/no input from user.
 */
bool get_yes_no_input(const char *prompt);

/**
 * @brief Get user input for parameters.
 * 
 * This function checks if a saved parameter file exists and prompts the user to
 * use it. If not, it asks the user whether they want to use BCH code or input G1 and
 * G2 parameters manually. It generates random parameters if the user chooses not
 * to input them. The function also saves the parameters to a file for future use.
 * At first it checks if a saved parameter file exists. If it does, it prompts the
 * user to use it. If the user chooses not to use the saved parameters, it asks
 * whether they want to use BCH code or input G1 and G2 parameters manually.
 * 
 * • For the BCH code, it calculates the parameters based on user input for m and t, ensuring that the derived values for n, k, and d are consistent across G1 and G2. The parameters for H_A are derived from G1 and
 * G2. m is the degree of the BCH code, and t is the error-correcting capability. The parameters are calculated as follows:
 * • - n = 2^m - 1 (length of the codeword)
 * • - k = m * t (length of the message)
 * • - d = 2 * t + 1 (minimum distance of the code)
 * • If the user chooses to input G1 and G2 parameters manually, it prompts for each
 * parameter (n, k, d) and checks their validity. If the user does not want to
 * input parameters, it generates random parameters for G1 and G2.
 * 
 * After gathering the parameters, it saves them to a file named Defined as
 * PARAM_PATH for future use. The parameters for G1, G2, and H_A are printed to the
 * console for confirmation.
 * 
 * @param g1 Pointer to Params structure for G1 parameters.
 * @param g2 Pointer to Params structure for G2 parameters.
 * @param h Pointer to Params structure for H_A parameters.
 */
void get_user_input(Params *g1, Params *g2, Params *h);

/**
 * @brief It generates a random number in the range [min, max].
 * 
 * This function uses the libsodium library to generate a uniform random unsigned
 * 32bit number. Basically, it generates a random number in the range [0, max-min]
 * and adds with min.
 * 
 * @param min The minimum value of the range (inclusive).
 * @param max The maximum value of the range (inclusive).
 * 
 * @return uint32_t The generated random number.
 * 
 * @note This function assumes that max is greater than or equal to min.
 * 
 * @see randombytes_uniform
 */
uint32_t random_range(uint32_t min, uint32_t max);

/**
 * @brief Returns the n parameter of the concatenated code H_A.
 * 
 * @return The value of H_A.n
 */
uint32_t get_H_A_n(void);

/**
 * @brief Returns the k parameter of the concatenated code H_A.
 * 
 * @return The value of H_A.k
 */
uint32_t get_H_A_k(void);

/**
 * @brief Returns the d parameter of the concatenated code H_A.
 * 
 * @return The value of H_A.d
 */
uint32_t get_H_A_d(void);

/**
 * @brief Returns the n parameter of the generator matrix G1.
 * 
 * @return The value of G1.n
 */
uint32_t get_G1_n(void);

/**
 * @brief Returns the k parameter of the generator matrix G1.
 * 
 * @return The value of G1.k
 */
uint32_t get_G1_k(void);

/**
 * @brief Returns the d parameter of the generator matrix G1.
 * 
 * @return The value of G1.d
 */
uint32_t get_G1_d(void);

/**
 * @brief Returns the n parameter of the generator matrix G2.
 * 
 * @return The value of G2.n
 */
uint32_t get_G2_n(void);

/**
 * @brief Returns the k parameter of the generator matrix G2.
 * 
 * @return The value of G2.k
 */
uint32_t get_G2_k(void);

/**
 * @brief Returns the d parameter of the generator matrix G2.
 * 
 * @return The value of G2.d
 */
uint32_t get_G2_d(void);

#endif