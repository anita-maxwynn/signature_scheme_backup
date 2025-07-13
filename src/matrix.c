/**
 * @file matrix.c
 * @brief Implementation of matrix operations for the signature scheme.
 * 
 * Implementation of matrix operations for the signature scheme. This file provides functions for matrix operations such as printing 
 * matrices, transposing matrices, multiplying matrices over GF(2), and transforming matrices into systematic form. It also includes functions for performing row reduction 
 * to echelon form. The operations are primarily used for handling parity check matrices and generator matrices in the context of error-correcting codes.
 */

#include <stdio.h>
#include <sodium.h>
#include <flint/flint.h>
#include <flint/nmod_mat.h>

/**
 * @brief The print_matrix function is designed to output the contents of a matrix to a specified file stream, such as a file or the console.
 * 
 * It takes two parameters: a FILE *fp, which is the output stream, and an nmod_mat_t matrix, which represents the matrix to be printed. The function begins by printing the dimensions of the matrix in the format <rows x columns matrix>, using the r and c fields of nmod_mat_t matrix.
 * 
 * It then iterates through each row and column of the matrix, printing each entry in a formatted manner. Each row is enclosed in square brackets, and entries are separated by spaces. After printing all entries in a row, it moves to the next line for the next row.
 * 
 * @param fp The file pointer to which the matrix will be printed. This can be a file opened in write mode or stdout for console output.
 * 
 * @note The function assumes that the matrix is non-empty and that the nmod_mat_t structure is properly initialized. It does not handle any errors related to file operations or matrix initialization.
 * 
 * @param matrix The matrix to be printed, represented as an nmod_mat_t type from the FLINT library. This structure allows efficient access and manipulation of matrix data under modular arithmetic.
 * 
 * @return void This function does not return a value; it performs output operations directly to the specified file stream.
 */
void print_matrix(FILE *fp, nmod_mat_t matrix) {
    fprintf(fp, "<%ld x %ld matrix>\n", matrix->r, matrix->c);
    for (int i = 0; i < matrix->r; i++) {
        fprintf(fp, "[ ");
        for (int j = 0; j < matrix->c; j++) {
            fprintf(fp, "%ld ", nmod_mat_get_entry(matrix, i, j));
        }
        fprintf(fp, "]");
        fprintf(fp, "\n");
    }
}

/**
 * @brief The function computes the transpose of a two-dimensional integer matrix.
 * 
 * This function takes a matrix defined by its number of rows and columns, and fills a new matrix with the transposed values. The transpose of a matrix is obtained by swapping its rows and columns, meaning that the element at position (i, j) in the original matrix becomes the element at position (j, i) in the transposed matrix. So that the elements of the firsr row in the original matrix becomes the elements at the first column in the transposed matrix.
 * 
 * @param rows The number of rows in the original matrix.
 * @param cols The number of columns in the original matrix.
 * 
 * @note The function assumes that the input matrix is well-formed and that the transpose matrix has been allocated with appropriate dimensions.
 * 
 * @param matrix The original matrix to be transposed, represented as a two-dimensional array of integers.
 * @param transpose The transposed matrix, which will be filled with the transposed values of the original matrix.
 * 
 * @return void This function does not return a value; it modifies the transpose matrix in place.
 */
void transpose_matrix(int rows, int cols, int matrix[rows][cols], int transpose[cols][rows]) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            transpose[j][i] = matrix[i][j];
        }
    }
}

/**
 * @brief The function performs matrix multiplication over the finite field GF(2)
 * 
 * This function multiplies two matrices A and B, both defined over the finite field GF(2), and stores the result in matrix C. The multiplication is performed using bitwise operations, where addition is equivalent to XOR and multiplication is equivalent to AND. The function multiplies matrices over GF(2) using three nested loops. The outer loops iterate over rows of A and columns of B to compute each entry of the result matrix C. For each (i, j) entry, it initializes C[i][j] to 0, then uses the inner loop to XOR the bitwise AND of A[i][k] and B[k][j] into C[i][j]. This performs matrix multiplication using bitwise operations, with & as multiplication and ^ as addition in GF(2). The function assumes the matrices are properly initialized and dimensionally compatible.
 * 
 * @param A The first matrix to be multiplied, represented as an nmod_mat_t type from the FLINT library.
 * @param B The second matrix to be multiplied, also represented as an nmod_mat_t type from the FLINT library.
 * 
 * @note The function assumes that the matrices A and B are compatible for multiplication, meaning the number of columns in A must equal the number of rows in B. It also assumes that the result matrix C has been initialized with appropriate dimensions to hold the product of A and B.
 * 
 * The function uses the nmod_mat_get_entry and nmod_mat_set_entry functions from the FLINT library to access and modify matrix entries. It iterates through each row of A and each column of B, computing the product for each entry in C.
 * 
 * @param C The result matrix where the product of A and B will be stored, also represented as an nmod_mat_t type from the FLINT library.
 * 
 * @return void This function does not return a value; it modifies the matrix C in place to store the result of the multiplication.
 */
void multiply_matrices_gf2(nmod_mat_t C, const nmod_mat_t A, const nmod_mat_t B) {
    for (size_t i = 0; i < A->r; i++) {
        for (size_t j = 0; j < B->c; j++) {
            nmod_mat_set_entry(C, i, j, 0);

            for (size_t k = 0; k < B->r; k++) {
                int temp = nmod_mat_get_entry(C, i, j);
                temp ^= (nmod_mat_get_entry(A, i, k) & nmod_mat_get_entry(B, k, j));  
                nmod_mat_set_entry(C, i, j, temp);
            }
        }
    }
}

/**
 * @brief The function swaps two(first and second) columns in the matrix H for a specified range of rows.
 * 
 * This function is designed to swap two columns in a matrix H, specifically for the first n-k rows of the matrix. The function takes the number of rows n, the number of columns k, and the indices of the two columns to be swapped (first and second). It iterates through each row from 0 to n-k-1 and swaps the entries in the specified columns.
 * 
 * @param n The total number of rows in the matrix H.
 * @param k The number of columns in the matrix H, which is used to determine the range of rows that will be affected by the column swap.
 * @param first The index of the first column to be swapped.
 * @param second The index of the second column to be swapped.
 * 
 * @note The function assumes that the indices first and second are valid column indices within the range of the matrix H, and that n and k are correctly defined such that n >= k. It does not perform any bounds checking on the indices.
 * 
 * @param H The matrix in which the columns will be swapped, represented as an nmod_mat_t type from the FLINT library.
 * 
 * The function uses the nmod_mat_get_entry and nmod_mat_set_entry functions from the FLINT library to access and modify matrix entries. It iterates through each row from 0 to n-k-1, swapping the entries in the specified columns.
 */
static void swap_columns(size_t n, size_t k, size_t first, size_t second, nmod_mat_t H) {
    for (size_t i = 0; i < n - k; ++i) {
        int temp = nmod_mat_get_entry(H, i, first);
        nmod_mat_set_entry(H, i, first, nmod_mat_get_entry(H, i, second));
        nmod_mat_set_entry(H, i, second, temp);
    }
}

/**
 * @brief The function transforms a parity check matrix H into systematic form.
 * 
 * This function takes a parity check matrix H, which is represented as an nmod_mat_t type from the FLINT library, and transforms it into systematic form. Systematic form means that the first k columns of the matrix will be an identity matrix, and the remaining columns will contain the parity check bits. The function computes r = n - k and scans columns to find unit vectors (columns with a single 1 in the top r rows). When such a column is found, it is swapped into the correct position to form an identity matrix. This continues until r such columns are placed, at which point the matrix is in (partial) systematic form. It's a greedy approach that works well if the matrix is already close to systematic.
 * 
 * @note The function assumes that the matrix H is well-formed and that the number of rows n and columns k are correctly defined such that n >= k. It does not perform any bounds checking on the indices.
 * 
 * @param n The total number of rows in the matrix H, which is used to determine the range of rows that will be affected by the transformation.
 * @param k The number of columns in the matrix H, which is used to determine the number of rows that will be transformed into an identity matrix.
 * 
 * The function uses the nmod_mat_get_entry and nmod_mat_set_entry functions from the FLINT library to access and modify matrix entries. It iterates through each column of the matrix, checking for unit vectors and swapping them into the correct position.
 * 
 * @param H The parity check matrix that will be transformed into systematic form, represented as an nmod_mat_t type from the FLINT library.
 * 
 * @return void This function does not return a value; it modifies the matrix H in place to transform it into systematic form.
 */
void make_systematic(size_t n, size_t k, nmod_mat_t H) {
    size_t r = n - k;
    unsigned long sum, position, count = 0;

    for (size_t i = 0; i < n; ++i) {
        sum = position = 0;

        for (size_t j = 0; j < r; ++j) {
            if (nmod_mat_get_entry(H, j, i) == 1) {
                position = j;
                sum += 1;
            }
        }

        if (sum == 1) {
            swap_columns(n, k, i, k + position, H);
            ++count;
        }

        if (count == r) 
            break;
    }
}

/**
 * @brief The function transforms a binary matrix into its Reduced Row Echelon Form(RREF).
 * 
 * This function takes a binary matrix H, represented as a two-dimensional array of integers, and transforms it into its Reduced Row Echelon Form (RREF). The RREF is a form where each leading entry in a row is 1, and all entries in the column above and below each leading 1 are 0. The function performs forward and back substitution to achieve this form. It iterates through the columns of the matrix, finding non-zero elements to use as pivot points, and then eliminates other entries in the same column by XORing rows.
 * 
 * @param num_rows The number of rows in the matrix H, which is used to determine the range of rows that will be affected by the transformation.
 * @param num_cols The number of columns in the matrix H, which is used to determine the range of columns that will be affected by the transformation.
 * @param H The binary matrix that will be transformed into Reduced Row Echelon Form, represented as a two-dimensional array of integers. Each entry in the matrix is either 0 or 1, representing elements in GF(2).
 * 
 * @note The notation (*H)[num_cols] in the function parameter list means that H is a pointer to an array of num_cols integers. In other words, H points to the first element of a 2D array where each row contains num_cols elements. This allows you to use H[i][j] inside the function to access the element at row i and column j, just like with a regular 2D array. The compiler needs to know the size of each row (num_cols) to correctly compute the memory offset for each element. This is why num_cols must be specified in the parameter type.
 */
void rref(int num_rows, int num_cols, int (*H)[num_cols]) {
    int current_col, current_row = 1;

    for (current_col = num_cols - num_rows + 1; current_col <= num_cols; current_col++) {
        if (H[current_row - 1][current_col - 1] == 0) {
            // Find a non-zero element in the current column to swap with the current row
            for (int swap_row = current_row; swap_row < num_rows; swap_row++) {
                if (H[swap_row][current_col - 1] != 0) {
                    // Swap the current row with the row containing the non-zero element
                    for (int j = 0; j < num_cols; j++) {
                        int temp = H[current_row - 1][j];
                        H[current_row - 1][j] = H[swap_row][j];
                        H[swap_row][j] = temp;
                    }
                    break;
                }
            }   
        }

        // Check if the matrix is singular
        if (H[current_row - 1][current_col - 1] == 0) {
            fprintf(stderr, "\nThe parity check matrix is singular\n");
            return;
        }

        // Forward substitution
        for (int swap_row = current_row; swap_row < num_rows; swap_row++) {
            if (H[swap_row][current_col - 1] == 1) {
                for (int j = 0; j < num_cols; j++) {
                    H[swap_row][j] ^= H[current_row - 1][j];
                }
            }
        }

        // Back substitution
        for (int swap_row = 0; swap_row < current_row - 1; swap_row++) {
            if (H[swap_row][current_col - 1] == 1) {
                for (int j = 0; j < num_cols; j++) {
                    H[swap_row][j] ^= H[current_row - 1][j];
                }
            }
        }

        current_row++;
    }
}