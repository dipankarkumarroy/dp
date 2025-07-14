#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define ROWS 4
#define COLS 4

int main(int argc, char *argv[]) {
    int rank, size;
    int A[ROWS][COLS], B[ROWS][COLS], C[ROWS][COLS];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (ROWS % size != 0) {
        if (rank == 0)
            printf("Matrix row count (%d) must be divisible by number of processes (%d)\n", ROWS, size);
        MPI_Finalize();
        return 0;
    }

    int rows_per_proc = ROWS / size;
    int local_A[rows_per_proc][COLS], local_B[rows_per_proc][COLS], local_C[rows_per_proc][COLS];

    // Initialize matrices in root process
    if (rank == 0) {
        printf("Matrix A:\n");
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                A[i][j] = i + j;
                printf("%d ", A[i][j]);
            }
            printf("\n");
        }

        printf("Matrix B:\n");
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                B[i][j] = (i * j);
                printf("%d ", B[i][j]);
            }
            printf("\n");
        }
    }

    // Scatter rows of A and B to all processes
    MPI_Scatter(A, rows_per_proc * COLS, MPI_INT, local_A, rows_per_proc * COLS, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(B, rows_per_proc * COLS, MPI_INT, local_B, rows_per_proc * COLS, MPI_INT, 0, MPI_COMM_WORLD);

    // Add the local rows
    for (int i = 0; i < rows_per_proc; i++) {
        for (int j = 0; j < COLS; j++) {
            local_C[i][j] = local_A[i][j] + local_B[i][j];
        }
    }

    // Gather the result into matrix C in the root
    MPI_Gather(local_C, rows_per_proc * COLS, MPI_INT, C, rows_per_proc * COLS, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Resultant Matrix C (A + B):\n");
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                printf("%d ", C[i][j]);
            }
            printf("\n");
        }
    }

    MPI_Finalize();
    return 0;
}
