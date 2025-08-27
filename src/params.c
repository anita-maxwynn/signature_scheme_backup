/**
 * @file params.c
 * @brief Implementation of parameter handling functions for the signature scheme.
 * @details This file provides functions for initializing, generating, reading, and handling
 * parameters used in the signature scheme. It manages parameters for generator matrices
 * and concatenated codes, supports random and user-defined parameter generation, and
 * handles persistent storage of parameters.
 *
 * Main functionalities include:
 * - Initialization of the libsodium library for cryptographic randomness.
 * - Generation of random parameters within specified ranges.
 * - Interactive user input for code parameters, including support for BCH code parameters.
 * - Reading and writing parameter sets to a persistent file.
 * - Accessor functions for retrieving current parameter values.
 *
 * The parameters managed include those for two generator matrices (G1, G2) and a concatenated code (H_A).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sodium.h>
#include "params.h"
#include "constants.h"

/** 
 * @name Static Global Parameters
 * @brief Parameters used for the generator and concatenated codes.
 * @{
 */
static Params G1;  /**< Parameters of the generator matrix for the first code (C1). */
static Params G2;  /**< Parameters for the generator matrix for the second code (C2). */
static Params H_A; /**< Parameters for the concatenated code (C_A). */
/** @} */



/**
 * @brief Static pointer to a message string.
 *
 * This variable is used to store a message as a dynamically allocated string.
 * It is initialized to NULL and should be assigned before use.
 * Being static, it has internal linkage and is only accessible within this source file.
 */
static char *MESSAGE = NULL;

/**
 * @brief It's the length of the message.
 * @details This is used to allocate memory for the message
 * and to ensure that the message is processed correctly.
 * This is used to determine the size of the message.
 * It is initialized to 0 and will be set
 * when the message is read or generated.
*/
static size_t MESSAGE_LEN = 0;

/**
 * @brief Initializes the libsodium library.
 * @details This function should be called before using any other libsodium functions. The libsodium library is used for generating random numbers in this implementation.
 * @note If libsodium fails to initialize, the program will exit with an error message.
 * @see sodium_init
 */
void init_params(void) {
    if (sodium_init() < 0) {
        fprintf(stderr, "Failed to initialize libsodium\n");
        exit(1);
    }
}
/**
 * @brief It generates a random number in the range [min, max].
 * 
 * @details This function uses the libsodium library to generate a uniform random unsigned 32bit number. Basically, it generates a random number in the range [0, max-min] and adds with min.
 * @param min The minimum value of the range (inclusive).
 * @param max The maximum value of the range (inclusive).
 * @return uint32_t The generated random number.
 * @note This function assumes that `max` is greater than or equal to `min`.
 * @see randombytes_uniform
 */
uint32_t random_range(uint32_t min, uint32_t max) {
    return min + randombytes_uniform(max - min + 1);
}

/**
 * @brief Generates random parameters for the Params structure.
 * 
 * @details This function generates random values for n, k, and d within specified ranges.
 * It ensures that n is greater than both k and d.
 * here the n, k, and d values are generated in the range of 16 to 17 for n, 6 to 7 for k, and 3 to 4 for d.
 * 
 * @param p Pointer to the Params structure to be filled with random values.
 */
static void generate_random_params(Params *p) {
    do {
        p->n = random_range(16, 17);
        p->k = random_range(6, 7);
        p->d = random_range(3, 4);
    } while (p->n <= p->k || p->n <= p->d);
}


/**
 * @brief Get the yes no input object
 * 
 * @param prompt It is a string that will be displayed to the user as a prompt.
 * @details This function prompts the user for a yes or no response.
 * It reads the user's input and checks for the 1st character of it. It returns true for 'y' or 'Y', and false for 'n' or 'N'.
 * If the input is invalid, it will terminate the program with failure.
 * @return true if the input's 1st character is 'y' or 'Y'.
 * @return false if the input's 1st character is 'n' or 'N'.
 * 
 */
bool get_yes_no_input(const char *prompt) {
    char response[10];
    printf("%s (y/n): ", prompt);
    if(scanf("%9s", response) == 0) {
        fprintf(stderr, "Could not read user response\n");
        exit(EXIT_FAILURE);
    }
    return (response[0] == 'y' || response[0] == 'Y');
}

/**
 * @brief Get the param input object
 * 
 * @details This function prompts the user to input parameters for a given code (G1, G2 and H_A). It takes a pointer to a Params structure and a name string as arguments. The function will repeatedly ask the user for input until valid parameters are provided (i.e., n > k and n > d). If the user inputs invalid parameters, it will prompt them to try again. the name param is used to identify which code the parameters are for (e.g., "G1", "G2", or "H_A").
 * The validity of the parameters is checked to ensure that `n` is either greater than `k` or `d`. If the input is invalid, it will prompt the user to try again.
 * 
 * @param p is the pointer to the Params structure where the parameters will be stored.
 * @param name is the name of the code for which parameters are being input (e.g., "G1", "G2", or "H_A").
 */
static void get_param_input(Params *p, const char *name) {
    do {
        printf("Enter %s parameters (ensure n > k and n > d):\n", name);
        printf("n: ");
        if (scanf("%u", &p->n) != 1) {
            fprintf(stderr, "Could not read user response\n");
            exit(EXIT_FAILURE);
        }

        printf("k: ");
        if (scanf("%u", &p->k) != 1) {
            fprintf(stderr, "Could not read user response\n");
            exit(EXIT_FAILURE);
        }
        printf("d: ");
        if (scanf("%u", &p->d) != 1) {
            fprintf(stderr, "Could not read user response\n");
            exit(EXIT_FAILURE);
        }

        if (p->n <= p->k || p->n <= p->d) {
            printf("Invalid input: n must be greater than both k and d. Please try again.\n");
        }
    } while (p->n <= p->k || p->n <= p->d);
}


/**
 * @brief Get user input for parameters.
 * 
 * @details This function checks if a saved parameter file exists and prompts the user to use it. If not, it asks the user whether they want to use BCH code or input G1 and G2 parameters manually. It generates random parameters if the user chooses not to input them.
 * The function also saves the parameters to a file for future use.
 * At first it checks if a saved parameter file exists. If it does, it prompts the user to use it. 
 * If the user chooses not to use the saved parameters, it asks whether they want to use BCH code or input G1 and G2 parameters manually.
 * * For the BCH code, it calculates the parameters based on user input for `m` and `t`, ensuring that the derived values for `n`, `k`, and `d` are consistent across G1 and G2. The parameters for H_A are derived from G1 and G2. `m` is the degree of the BCH code, and `t` is the error-correcting capability. The parameters are calculated as follows:
 * * - n = 2^m - 1 (length of the codeword)
 * * - k = m * t (length of the message)
 * * - d = 2 * t + 1 (minimum distance of the code)
 * 
 * * If the user chooses to input G1 and G2 parameters manually, it prompts for each parameter (n, k, d) and checks their validity. If the user does not want to input parameters, it generates random parameters for G1 and G2.
 * 
 * 
 * After gathering the parameters, it saves them to a file named Defined as PARAM_PATH for future use. The parameters for G1, G2, and H_A are printed to the console for confirmation.
 * 
 * @param g1 Pointer to Params structure for G1 parameters.
 * @param g2 Pointer to Params structure for G2 parameters.
 * @param h Pointer to Params structure for H_A parameters.
 */
void get_user_input(Params *g1, Params *g2, Params *h) {
    bool param_choice = false;
    FILE *param_file = fopen(PARAM_PATH, "r");
    if (param_file) {

        if ((param_choice = get_yes_no_input("Saved parameter file params.txt found, use it?"))) {
            fscanf(param_file, "H_A_n %u\n", &h->n);
            fscanf(param_file, "H_A_k %u\n", &h->k);
            fscanf(param_file, "H_A_d %u\n", &h->d);
            fscanf(param_file, "G1_n %u\n", &g1->n);
            fscanf(param_file, "G1_k %u\n", &g1->k);
            fscanf(param_file, "G1_d %u\n", &g1->d);
            fscanf(param_file, "G2_n %u\n", &g2->n);
            fscanf(param_file, "G2_k %u\n", &g2->k);
            fscanf(param_file, "G2_d %u\n", &g2->d);
            fclose(param_file);
        }
    }

    if (!param_choice && get_yes_no_input("Do you want to use BCH code?")) {
        unsigned int m, t;
        printf("m: ");
        scanf("%u", &m);
        printf("t: ");
        scanf("%u", &t);

        unsigned int n = (1 << m) - 1;
        unsigned int d = 2 * t + 1;
        unsigned int k = m * t;

        g1->n = g2->n = n;
        g1->k = g2->k = k;
        g1->d = g2->d = d;

        h->n = g1->n + g2->n;
        h->d = g1->d + g2->d + 1;
        h->k = h->n * (1 - binary_entropy((double) h->d / h->n));
    } else if (!param_choice) {
        if (get_yes_no_input("Do you want to input G1 parameters?")) {
            get_param_input(g1, "G1");
        } else {
            generate_random_params(g1);
            printf("G1 parameters randomly generated.\n");
        }

        if (get_yes_no_input("Do you want to input G2 parameters?")) {
            get_param_input(g2, "G2");
            if (g1->k != g2->k) {
                fprintf(stderr, "Different values for k, setting G2->k to %u\n", g1->k);
                g2->k = g1->k;
            }
        } else {
            generate_random_params(g2);
            g2->k = g1->k;
            printf("G2 parameters randomly generated.\n");
        }

        h->n = g1->n + g2->n;
        h->k = g1->k;
        h->d = g1->d + g2->d;
    }

    param_file = fopen(PARAM_PATH, "w");
    if (param_file) {
        fprintf(param_file, "H_A_n %u\n", h->n);
        fprintf(param_file, "H_A_k %u\n", h->k);
        fprintf(param_file, "H_A_d %u\n", h->d);
        fprintf(param_file, "G1_n %u\n", g1->n);
        fprintf(param_file, "G1_k %u\n", g1->k);
        fprintf(param_file, "G1_d %u\n", g1->d);
        fprintf(param_file, "G2_n %u\n", g2->n);
        fprintf(param_file, "G2_k %u\n", g2->k);
        fprintf(param_file, "G2_d %u\n", g2->d);
        fclose(param_file);
    }

    G1 = *g1;
    G2 = *g2;
    H_A = *h;

    printf("\nC1 parameters: %u %u %u", g1->n, g1->k, g1->d);
    printf("\nC1 parameters: %u %u %u", g2->n, g2->k, g2->d);
    printf("\nC_A parameters: %u %u %u\n\n", h->n, h->k, h->d);
}



/**
 * @brief Returns the n parameter of the concatenated code H_A.
 * @return The value of H_A.n
 */
uint32_t get_H_A_n(void) { return H_A.n; }

/**
 * @brief Returns the k parameter of the concatenated code H_A.
 * @return The value of H_A.k
 */
uint32_t get_H_A_k(void) { return H_A.k; }

/**
 * @brief Returns the d parameter of the concatenated code H_A.
 * @return The value of H_A.d
 */
uint32_t get_H_A_d(void) { return H_A.d; }

/**
 * @brief Returns the n parameter of the generator matrix G1.
 * @return The value of G1.n
 */
uint32_t get_G1_n(void) { return G1.n; }

/**
 * @brief Returns the k parameter of the generator matrix G1.
 * @return The value of G1.k
 */
uint32_t get_G1_k(void) { return G1.k; }

/**
 * @brief Returns the d parameter of the generator matrix G1.
 * @return The value of G1.d
 */
uint32_t get_G1_d(void) { return G1.d; }

/**
 * @brief Returns the n parameter of the generator matrix G2.
 * @return The value of G2.n
 */
uint32_t get_G2_n(void) { return G2.n; }

/**
 * @brief Returns the k parameter of the generator matrix G2.
 * @return The value of G2.k
 */
uint32_t get_G2_k(void) { return G2.k; }

/**
 * @brief Returns the d parameter of the generator matrix G2.
 * @return The value of G2.d
 */
uint32_t get_G2_d(void) { return G2.d; }