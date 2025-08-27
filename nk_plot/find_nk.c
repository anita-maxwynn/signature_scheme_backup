#include <stdio.h>
#include <math.h>
#include <stdlib.h>

double binary_entropy(double p) {
    if (p <= 0.0 || p >= 1.0) return 0.0;
    return -p * log2(p) - (1.0 - p) * log2(1.0 - p);
}

void gv(int m, int t, int *n, int *d, int *k) {
    *n = (1 << m) - 1;     // 2^m - 1
    *d = 2 * t + 1;
    *k = m * t;
}

void gv_k(int m, int t, int *n, int *d, int *k) {
    *n = (1 << (m + 1)) - 2;  // 2^(m+1) - 2
    *d = 4 * t + 3;
    double delta = (double)(*d) / (double)(*n);
    *k = (int)floor((*n) * (1.0 - binary_entropy(delta)));
}

int main() {
    FILE *fp = fopen("bch_codes_c.csv", "w");
    if (!fp) {
        perror("Failed to open file");
        return 1;
    }

    fprintf(fp, "m,t,n_C1/C2,d_C1/C2,k_C1/C2,n_C,d_C,k_C\n");

    for (int m = 3; m < 30; m++) {
        for (int t = 1; t < 30; t++) {
            int n1, d1, k1;
            int n2, d2, k2;

            gv(m, t, &n1, &d1, &k1);
            gv_k(m, t, &n2, &d2, &k2);

            fprintf(fp, "%d,%d,%d,%d,%d,%d,%d,%d\n",
                    m, t, n1, d1, k1, n2, d2, k2);
        }
    }

    fclose(fp);
    printf("CSV file generated: bch_codes_c.csv\n");

    return 0;
}
