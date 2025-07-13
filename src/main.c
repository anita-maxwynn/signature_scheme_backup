/**
 * @file main.c
 * @brief Main entry point and core command handling for the signature scheme.
 * 
 * This file implements the main function and the three primary commands for the 
 * signature scheme:
 * - keygen: Key generation
 * - sign: Message signing  
 * - verify: Signature verification
 * 
 * Each command is handled by its own function, which parses command-line 
 * arguments, manages file I/O, and coordinates the use of supporting modules (matrix 
 * operations, parameter loading, etc).
 */

#include <string.h>
#include <flint/flint.h>
#include <flint/nmod_mat.h>
#include "params.h"
#include "time.h"
#include "keygen.h"
#include "signer.h"
#include "verifier.h"
#include "utils.h"
#include "constants.h"

/**
 * @brief Key generation function for the signature scheme.
 * 
 * First, it checks for command line arguments to determine if it should use seed mode or regenerate keys. It then opens the output file to write the generated keys. It retrieves user input for the parameters of the keys, initializes matrices for the codes, and generates the keys based on the specified parameters. The generated keys are written to the output file, and the matrices are cleared before closing the output file.
 * 
 * @param argc The number of command line arguments passed to the keygen function.
 * @param argv An array of strings representing the command line arguments passed to the keygen function.
 * @return int 0 on success, or a non-zero value on failure.
 * 
 * @see get_user_input
 * @see generate_keys
 * @see load_params
 * @see get_H_A_n
 * @see get_H_A_k
 * @see get_H_A_d
 * @see get_G1_n
 * @see get_G1_k
 * @see get_G1_d
 * @see get_G2_n
 * @see get_G2_k
 * @see get_G2_d
 */
int keygen(int argc, char *argv[]);
/**
 * @brief Responsible for signing a message using the signature scheme.
 * 
 * It processes command line arguments to get the message file and output signature file. First, it checks if the message file is provided, and if not, it prints usage instructions. Then it loads the parameters for the codes, reads the message from the file or generates it if not found, and normalizes the message length. It initializes matrices for the codes and generates the necessary matrices (H_A, G1, G2) using the specified parameters. The function then generates the signature by calling generate_signature, which computes the signature based on the message and the codes. Finally, it saves the generated signature, hash, and public key to the output directory, clears the matrices, and frees the allocated memory for the message.
 * 
 * @param argc The number of command line arguments passed to the sign function.
 * @param argv An array of strings representing the command line arguments passed to the sign function.
 * @return int 0 on success, or a non-zero value on failure.
 * 
 * @note This function uses Flint's nmod_mat_t for matrix operations, which are essential for the signature generation process.
 * 
 * @see load_params
 * @see read_file_or_generate
 * @see normalize_message_length
 * @see get_or_generate_matrix_with_seed
 * @see generate_signature
 * @see save_matrix
 * @see load_matrix
 */
int sign(int argc, char *argv[]);
/**
 * @brief Responsible for verifying a signature against a message using the signature scheme.
 * 
 * It processes command line arguments to get the message file and signature file. First, it checks if both the message file and signature file are provided, and if not, it prints usage instructions. It loads the parameters for the codes, reads the message from the file, and initializes matrices for the codes. The function then loads the signature from the specified file and generates the parity check matrix (H_A) using the specified parameters. It also loads the hash of the message and the public key matrix (F) from the output directory. Finally, it verifies the signature by calling verify_signature, which checks if the signature is valid for the given message and parameters. The results of the verification are written to an output file.
 * 
 * @param argc The number of command line arguments passed to the verify function.
 * @param argv An array of strings representing the command line arguments passed to the verify function.
 * @return int 0 on success, or a non-zero value on failure.
 * 
 * @note These functions use Flint's nmod_mat_t for matrix operations, which are essential for the signature verification process.
 * 
 * @see load_params
 * @see read_file
 * @see load_matrix
 * @see get_or_generate_matrix_with_seed
 * @see verify_signature
 */
int verify(int argc, char *argv[]);

/**
 * @brief Main function of the signature scheme program.
 * 
 * It handles command line arguments to either generate keys, sign a message, or
 * verify a signature. It supports three main commands: keygen, sign, and verify.
 * 
 * • keygen: Generates the keys required for the signature scheme.
 * • sign: Signs a message using the generated keys.
 * • verify: Verifies a signature against a message using the public key. It also checks
 * for the existence of necessary directories (matrix cache and output directory) and
 * initializes them if they do not exist.
 * 
 * @param argc The number of command line arguments passed to the program.
 * It is used to determine how many arguments were provided and to parse them
 * accordingly.
 * @param argv An array of strings representing the command line arguments passed to the
 * program.
 * Each element in the array corresponds to a command line argument, with argv[0] being the program name and subsequent elements being the actual arguments
 * provided by the user. This array is used to determine the command to execute (key
 * generation, signing, or verification) and to parse any additional options or
 * parameters that may be required for those commands. The program expects specific
 * commands and options to be provided, and it uses this array to handle those commands
 * appropriately.
 * 
 * @return int 0 on success, or a non-zero value on failure.
 * The return value indicates the success or failure of the operation performed by
 * the main function. If the command is recognized and executed successfully (key
 * generation, signing, or verification), it returns 0. If there are errors such as
 * missing arguments, unrecognized commands, or file I/O issues, it returns a
 * non-zero value to indicate failure. This allows the calling environment (such as a
 * shell or another program) to determine whether the operation was successful or if
 * there were errors that need to be addressed.
 * 
 * @see ensure_matrix_cache
 * @see ensure_output_directory
 */
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s {keygen|sign|verify} [options...]\n", argv[0]);
        return 1;
    }

    ensure_matrix_cache();
    ensure_output_directory();

    if (strcmp(argv[1], "keygen") == 0) {
        return keygen(argc - 1, &argv[1]);
    } else if (strcmp(argv[1], "sign") == 0) {
        return sign(argc - 1, &argv[1]);
    } else if (strcmp(argv[1], "verify") == 0) {
        return verify(argc - 1, &argv[1]);
    } else {
        fprintf(stderr, "Unknown command: %s\n", argv[1]);
        return 1;
    }
}

/**
 * @brief It is the key generation function for the signature scheme.
 * 
 * At first it checks for command line arguments to determine if it should use seed
 * mode or regenerate keys. It then opens the output file to write the generated
 * keys. It retrieves user input for the parameters of the keys, initializes
 * matrices for the codes, and generates the keys based on the specified parameters. The
 * generated keys are written to the output file, and the matrices are cleared
 * before closing the output file.
 * 
 * @param argc It is the number of command line arguments passed to the keygen function.
 * @param argv It is an array of strings representing the command line arguments passed to the
 * keygen function.
 * 
 * @return int It returns 0 on success, or a non-zero value on failure.
 * 
 * @see get_user_input
 * @see generate_keys
 * @see load_params
 * @see get_H_A_n
 * @see get_H_A_k
 * @see get_H_A_d
 * @see get_G1_n
 * @see get_G1_k
 * @see get_G1_d
 * @see get_G2_n
 * @see get_G2_k
 * @see get_G2_d
 */
int keygen(int argc, char *argv[]) {
    bool use_seed_mode = false;
    bool regenerate = false;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--use-seed") == 0) use_seed_mode = true;
        if (strcmp(argv[i], "--regenerate") == 0) regenerate = true;
    }

    FILE *output_file = fopen(OUTPUT_PATH, "w");
    if (!output_file) {
        fprintf(stderr, "Failed to open %s\n", OUTPUT_PATH);
        return 1;
    }

    Params g1, g2, h_a;
    get_user_input(&g1, &g2, &h_a);

    struct code C_A = {get_H_A_n(), get_H_A_k(), get_H_A_d()};
    struct code C1 = {get_G1_n(), get_G1_k(), get_G1_d()};
    struct code C2 = {get_G2_n(), get_G2_k(), get_G2_d()};

    nmod_mat_t H_A, G1, G2;
    nmod_mat_init(H_A, C_A.n - C_A.k, C_A.n, MOD);
    nmod_mat_init(G1, C1.k, C1.n, MOD);
    nmod_mat_init(G2, C2.k, C2.n, MOD);

    unsigned char h_a_seed[SEED_SIZE], g1_seed[SEED_SIZE], g2_seed[SEED_SIZE];

    generate_keys(&C_A, &C1, &C2, H_A, G1, G2,
                  use_seed_mode, regenerate, output_file,
                  h_a_seed, g1_seed, g2_seed);

    nmod_mat_clear(H_A);
    nmod_mat_clear(G1);
    nmod_mat_clear(G2);

    fclose(output_file);
    return 0;
}

/**
 * @brief This function is responsible for signing a message using the signature scheme.
 * 
 * It processes command line arguments to get the message file and output signature
 * file. At first it checks if the message file is provided, and if not, it prints
 * usage instructions. Then it loads the parameters for the codes, reads the
 * message from the file or generates it if not found, and normalizes the message
 * length. It initializes matrices for the codes and generates the necessary matrices
 * (H_A, G1, G2) using the specified parameters. The function then generates the
 * signature by calling generate_signature, which computes the signature based on the message and the codes. Finally, it
 * saves the generated signature, hash, and public key to the output directory,
 * clears the matrices, and frees the allocated memory for the message.
 * 
 * @param argc It is the number of command line arguments passed to the sign function.
 * @param argv It is an array of strings representing the command line arguments passed to the
 * sign function.
 * 
 * @return int It returns 0 on success, or a non-zero value on failure.
 * 
 * @note This function uses Flint's nmod_mat_t for matrix operations, which are essential
 * for the signature generation process.
 * 
 * @see load_params
 * @see read_file_or_generate
 * @see normalize_message_length
 * @see get_or_generate_matrix_with_seed
 * @see generate_signature
 * @see save_matrix
 * @see load_matrix
 */
int sign(int argc, char *argv[]) {
    const char *message_file = NULL;
    const char *signature_output = NULL;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-m") == 0 && i + 1 < argc) {
            message_file = argv[++i];
        } else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            signature_output = argv[++i];
        }
    }

    if (!message_file) {
        fprintf(stderr, "Usage: sign -m message.txt [-o sig.bin]\n");
        return 1;
    }

    struct code C_A, C1, C2;
    if (!load_params(&C_A, &C1, &C2)) return 1;

    char *raw_msg = read_file_or_generate(message_file, C1.k);
    if (!raw_msg) return 1;

    size_t raw_len = strlen(raw_msg);
    size_t msg_len = 0;
    char *msg = normalize_message_length(raw_msg, raw_len, C1.k, &msg_len);
    free(raw_msg);
    if (!msg) return 1;

    const unsigned char *message = (const unsigned char *)msg;

    FILE *output_file = fopen(OUTPUT_PATH, "w");

    nmod_mat_t H_A, G1, G2, F, signature, bin_hash;
    nmod_mat_init(H_A, C_A.n - C_A.k, C_A.n, MOD);
    nmod_mat_init(G1, C1.k, C1.n, MOD);
    nmod_mat_init(G2, C2.k, C2.n, MOD);
    nmod_mat_init(F, C_A.n - C_A.k, C1.k, MOD);
    nmod_mat_init(signature, 1, C_A.n, MOD);
    nmod_mat_init(bin_hash, 1, msg_len, MOD);

    get_or_generate_matrix_with_seed("H", C_A.n, C_A.k, C_A.d, H_A,
                                     NULL, generate_parity_check_matrix_from_seed,
                                     output_file, false, true, NULL);
    get_or_generate_matrix_with_seed("G", C1.n, C1.k, C1.d, G1,
                                     NULL, create_generator_matrix_from_seed,
                                     output_file, false, true, NULL);
    get_or_generate_matrix_with_seed("G", C2.n, C2.k, C2.d, G2,
                                     NULL, create_generator_matrix_from_seed,
                                     output_file, false, true, NULL);

    generate_signature(bin_hash, message, msg_len, C_A, C1, C2,
                 H_A, G1, G2, F, signature, output_file);


    char path[MAX_FILENAME_LENGTH]; 
    snprintf(path, sizeof(path), "%s/hash.txt", OUTPUT_DIR);
    save_matrix(path, bin_hash);
    snprintf(path, sizeof(path), "%s/signature.txt", OUTPUT_DIR);
    save_matrix(path, signature);
    snprintf(path, sizeof(path), "%s/public_key.txt", OUTPUT_DIR);
    save_matrix(path, F);

    nmod_mat_clear(H_A); nmod_mat_clear(G1); nmod_mat_clear(G2);
    nmod_mat_clear(F); nmod_mat_clear(signature); nmod_mat_clear(bin_hash);
    
    fclose(output_file); 
    free(msg);

    return 0;
}

/**
 * @brief This function is responsible for verifying a signature against a message using
 * the signature scheme.
 * 
 * It processes command line arguments to get the message file and signature file.
 * At first it checks if both the message file and signature file are provided, and
 * if not, it prints usage instructions. It loads the parameters for the codes,
 * reads the message from the file, and initializes matrices for the codes. The
 * function then loads the signature from the specified file and generates the parity
 * check matrix (H_A) using the specified parameters. It also loads the hash of the
 * message and the public key matrix (F) from the output directory. Finally, it
 * verifies the signature by calling verify_signature, which checks if the signature is valid for the given message and parameters.
 * The results of the verification are written to an output file.
 * 
 * @param argc It is the number of command line arguments passed to the verify function.
 * @param argv It is an array of strings representing the command line arguments passed to the
 * verify function.
 * 
 * @return int It returns 0 on success, or a non-zero value on failure.
 * 
 * @note These functions use Flint's nmod_mat_t for matrix operations, which are
 * essential for the signature verification process.
 * 
 * @see load_params
 * @see read_file
 * @see load_matrix
 * @see get_or_generate_matrix_with_seed
 * @see verify_signature
 */
int verify(int argc, char *argv[]) {
    const char *message_file = NULL;
    const char *signature_file = NULL;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-m") == 0 && i + 1 < argc) {
            message_file = argv[++i];
        } else if (strcmp(argv[i], "-s") == 0 && i + 1 < argc) {
            signature_file = argv[++i];
        }
    }

    if (!message_file || !signature_file) {
        fprintf(stderr, "Usage: verify -m message.txt -s sig.bin\n");
        return 1;
    }

    char *msg = read_file(message_file);
    size_t msg_len = strlen(msg);
    const unsigned char *message = (const unsigned char *)msg;

    FILE *output_file = fopen(OUTPUT_PATH, "w");

    struct code C_A, C1, C2;
    if (!load_params(&C_A, &C1, &C2)) return 1;

    nmod_mat_t H_A, F, signature, bin_hash;
    nmod_mat_init(H_A, C_A.n - C_A.k, C_A.n, MOD);
    nmod_mat_init(F, C_A.n - C_A.k, C1.k, MOD);
    nmod_mat_init(signature, 1, C_A.n, MOD);
    nmod_mat_init(bin_hash, 1, msg_len, MOD);

    load_matrix(signature_file, signature);

    get_or_generate_matrix_with_seed("H", C_A.n, C_A.k, C_A.d, H_A,
                                     NULL, generate_parity_check_matrix_from_seed,
                                     output_file, false, true, NULL);

    char path[MAX_FILENAME_LENGTH]; 
    snprintf(path, sizeof(path), "%s/hash.txt", OUTPUT_DIR);
    if (!load_matrix(path, bin_hash)) {
        fprintf(stderr, "Error: Could not load signature hash.\n");
        return 1;
    }

    snprintf(path, sizeof(path), "%s/public_key.txt", OUTPUT_DIR);
    if (!load_matrix(path, F)) {
        fprintf(stderr, "Error: Could not load F matrix (public key) from cache.\n");
        return 1;
    }

    verify_signature(bin_hash, msg_len, C_A.n, signature, F, C_A, H_A, output_file);

    nmod_mat_clear(H_A); nmod_mat_clear(F);
    nmod_mat_clear(signature); nmod_mat_clear(bin_hash);
    fclose(output_file); free(msg);
    return 0;
}

// clock_t begin = clock();
//     clock_t end = clock();
//     double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
//     fprintf(timing_file, "main(): %lf\n", time_spent);