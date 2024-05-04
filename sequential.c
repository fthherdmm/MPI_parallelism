#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void matrix_vector_multiply(int rowA, int colA, double **A, double *x, double *y) {
    for (int i = 0; i < rowA; i++) {
        y[i] = 0;
        for (int j = 0; j < colA; j++) {
            y[i] += A[i][j] * x[j];
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Usage: %s <matrix_file> <vector_file>\n", argv[0]);
        return 1;
    }

    FILE* fptr_A = fopen(argv[1], "r");
    if (fptr_A == NULL) {
        printf("Error opening matrix file.\n");
        return 1;
    }

    int rowA, colA;
    fscanf(fptr_A, "%d %d", &rowA, &colA);

    double **A = (double **)malloc(rowA * sizeof(double *));
    for (int i = 0; i < rowA; i++) {
        A[i] = (double *)malloc(colA * sizeof(double));
        for (int j = 0; j < colA; j++) {
            if (fscanf(fptr_A, "%lf", &A[i][j]) != 1) {
                printf("Error reading matrix data.\n");
                fclose(fptr_A);
                return 1;
            }
        }
    }
    fclose(fptr_A);

    FILE* fptr_x = fopen(argv[2], "r");
    if (fptr_x == NULL) {
        printf("Error opening vector file.\n");
        return 1;
    }

    int rowX, colX;
    fscanf(fptr_x, "%d %d", &rowX, &colX);
    if (colX != 1) {
        printf("Error: The vector must have one column.\n");
        fclose(fptr_x);
        return 1;
    }

    double *x = (double *)malloc(rowX * sizeof(double));
    for (int i = 0; i < rowX; i++) {
        if (fscanf(fptr_x, "%lf", &x[i]) != 1) {
            printf("Error reading vector data.\n");
            fclose(fptr_x);
            return 1;
        }
    }
    fclose(fptr_x);

    if (colA != rowX) {
        printf("Error: Matrix column count must match vector row count.\n");
        return 1;
    }

    double *y = (double *)malloc(rowA * sizeof(double));

    clock_t start = clock();

    matrix_vector_multiply(rowA, colA, A, x, y);

    clock_t end = clock();

    double elapsed_time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Sequential execution time: %f seconds\n", elapsed_time);

    FILE* fptr_y = fopen("y.txt", "w");
    if (fptr_y == NULL) {
        printf("Error opening result file.\n");
        return 1;
    }
    for (int i = 0; i < rowA; i++) {
        fprintf(fptr_y, "%lf\n", y[i]);
    }
    fclose(fptr_y);

    for (int i = 0; i < rowA; i++) {
        free(A[i]);
    }
    free(A);
    free(x);
    free(y);

    printf("\n");
    return 0;
}
