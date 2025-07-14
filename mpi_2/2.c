#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define M 4
#define N 3
#define P 2

void printMatrix(int *matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%d ", matrix[i * cols + j]);
        }
        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    int rank, size;
    int A[M][N], B[N][P], C[M][P];
    int *local_A, *local_C;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    int rows_per_process = M / size;
    local_A = (int *)malloc(rows_per_process * N * sizeof(int));
    local_C = (int *)malloc(rows_per_process * P * sizeof(int));
    
    if (rank == 0) {
        // Initialize matrices A and B
        printf("Matrix A (%dx%d):\n", M, N);
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N; j++) {
                A[i][j] = i + j;
                printf("%d ", A[i][j]);
            }
            printf("\n");
        }
        
        printf("\nMatrix B (%dx%d):\n", N, P);
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < P; j++) {
                B[i][j] = i - j;
                printf("%d ", B[i][j]);
            }
            printf("\n");
        }
    }
    
    // Scatter rows of A to all processes
    MPI_Scatter(A, rows_per_process * N, MPI_INT, 
                local_A, rows_per_process * N, MPI_INT, 
                0, MPI_COMM_WORLD);
    
    // Broadcast matrix B to all processes
    MPI_Bcast(B, N * P, MPI_INT, 0, MPI_COMM_WORLD);
    
    // Local matrix multiplication
    for (int i = 0; i < rows_per_process; i++) {
        for (int j = 0; j < P; j++) {
            local_C[i * P + j] = 0;
            for (int k = 0; k < N; k++) {
                local_C[i * P + j] += local_A[i * N + k] * B[k][j];
            }
        }
    }
    
    // Gather results to master
    MPI_Gather(local_C, rows_per_process * P, MPI_INT, 
               C, rows_per_process * P, MPI_INT, 
               0, MPI_COMM_WORLD);
    
    if (rank == 0) {
        printf("\nResult Matrix C (%dx%d):\n", M, P);
        printMatrix((int *)C, M, P);
    }
    
    free(local_A);
    free(local_C);
    MPI_Finalize();
    return 0;
}