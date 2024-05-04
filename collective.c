#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

void matrix_vector_multiply(int rowA, int colA, int rowX, double *A, double *x, double *y, int start, int end) {
    for (int i = start; i < end; i++) {
        y[i] = 0;
        for (int j = 0; j < colA; j++) {
            y[i] += A[i * colA + j] * x[j];
        }
    }
}

int main(int argc, char* argv[]) {
    int size, rank; 
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (size < 2) {
        printf("This program needs at least 2 processes to run.\n");
        MPI_Finalize();
        return 1;
    }

    if (argc != 3) {
        printf("Usage: %s <matrix_file> <vector_file>\n", argv[0]);
        MPI_Finalize();
        return 1;
    }

    int rowA, colA, rowX, colX;
    double *A = NULL, *x = NULL, *y = NULL;

    if (rank == 0) {
        FILE* fptr_A = fopen(argv[1], "r");
        if (fptr_A == NULL) {
            printf("Error opening matrix file.\n");
            MPI_Finalize();
            return 1;
        }
        fscanf(fptr_A, "%d %d", &rowA, &colA);
        A = (double*)malloc(rowA * colA * sizeof(double));
        for (int i = 0; i < rowA; i++) {
            for (int j = 0; j < colA; j++) {
                if (fscanf(fptr_A, "%lf", &A[i * colA + j]) != 1) {
                    printf("Error reading matrix data.\n");
                    fclose(fptr_A);
                    MPI_Finalize();
                    return 1;
                }
            }
        }
        fclose(fptr_A);
    }

    MPI_Bcast(&rowA, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&colA, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank != 0) {
        A = (double*)malloc(rowA * colA * sizeof(double));
    }

    MPI_Bcast(A, rowA * colA, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        FILE* fptr_x = fopen(argv[2], "r");
        if (fptr_x == NULL) {
            printf("Error opening vector file.\n");
            MPI_Finalize();
            return 1;
        }
        fscanf(fptr_x, "%d %d", &rowX, &colX);
        x = (double*)malloc(rowX * sizeof(double));
        for (int i = 0; i < rowX; i++) {
            if (fscanf(fptr_x, "%lf", &x[i]) != 1) {
                printf("Error reading vector data.\n");
                fclose(fptr_x);
                MPI_Finalize();
                return 1;
            }
        }
        fclose(fptr_x);
    }

    MPI_Bcast(&rowX, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank != 0) {
        x = (double*)malloc(rowX * sizeof(double));
    }

    MPI_Bcast(x, rowX, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    int rows_per_process = rowA / size;
    int remainder = rowA % size;
    int start_row = rank * rows_per_process;
    int end_row = (rank + 1) * rows_per_process;
    if (rank == size - 1) {
        end_row += remainder;
    }

    y = (double*)malloc(rowA * sizeof(double));

    double start_time = MPI_Wtime();

    matrix_vector_multiply(rowA, colA, rowX, A, x, y, start_row, end_row);

    MPI_Gather(rank == 0 ? MPI_IN_PLACE : y + start_row, rows_per_process, MPI_DOUBLE, y, rows_per_process, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    double end_time = MPI_Wtime();

    double elapsed_time = end_time - start_time;
    if (rank == 0) {
        printf("Parallel execution time: %f seconds\n", elapsed_time);

        FILE* fptr_y = fopen("y.txt", "w");
        if (fptr_y == NULL) {
            printf("Error opening result file.\n");
            MPI_Finalize();
            return 1;
        }
        for (int i = 0; i < rowA; i++) {
            fprintf(fptr_y, "%lf\n", y[i]);
        }
        fclose(fptr_y);
    }

    free(A);
    free(x);
    free(y);

    MPI_Finalize();
    printf("\n");
    return 0;
}
