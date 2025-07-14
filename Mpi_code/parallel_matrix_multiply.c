#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 4 // Rows of A and C
#define M 4 // Columns of A, Rows of B
#define P 4 // Columns of B and C

int main(int argc, char *argv[]) {
    int rank, size;
    int A[N][M], B[M][P], C[N][P];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int rows_per_proc = N / size;

    int local_A[rows_per_proc][M];
    int local_C[rows_per_proc][P];

    // Initialize matrices in root process
    if (rank == 0) {
        printf("Matrix A:\n");
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < M; j++) {
                A[i][j] = i + j + 1;
                printf("%d ", A[i][j]);
            }
            printf("\n");
        }

        printf("Matrix B:\n");
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < P; j++) {
                B[i][j] = i * j + 1;
                printf("%d ", B[i][j]);
            }
            printf("\n");
        }
    }

    // Broadcast Matrix B to all processes
    MPI_Bcast(B, M * P, MPI_INT, 0, MPI_COMM_WORLD);

    // Scatter rows of A to all processes
    MPI_Scatter(A, rows_per_proc * M, MPI_INT, local_A, rows_per_proc * M, MPI_INT, 0, MPI_COMM_WORLD);

    // Multiply local_A with B to get local_C
    for (int i = 0; i < rows_per_proc; i++) {
        for (int j = 0; j < P; j++) {
            local_C[i][j] = 0;
            for (int k = 0; k < M; k++) {
                local_C[i][j] += local_A[i][k] * B[k][j];
            }
        }
    }

    // Gather result into C in root
    MPI_Gather(local_C, rows_per_proc * P, MPI_INT, C, rows_per_proc * P, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Resultant Matrix C (A x B):\n");
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < P; j++) {
                printf("%d ", C[i][j]);
            }
            printf("\n");
        }
    }

    MPI_Finalize();
    return 0;
}
