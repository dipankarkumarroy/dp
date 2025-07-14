#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define ROWS 4
#define COLS 4

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
    int A[ROWS][COLS], B[ROWS][COLS], C[ROWS][COLS];
    int *local_A, *local_B, *local_C;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    int rows_per_process = ROWS / size;
    local_A = (int *)malloc(rows_per_process * COLS * sizeof(int));
    local_B = (int *)malloc(rows_per_process * COLS * sizeof(int));
    local_C = (int *)malloc(rows_per_process * COLS * sizeof(int));
    
    if (rank == 0) {
        // Initialize matrices A and B
        printf("Matrix A:\n");
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                A[i][j] = i + j;
                printf("%d ", A[i][j]);
            }
            printf("\n");
        }
        
        printf("\nMatrix B:\n");
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                B[i][j] = i - j;
                printf("%d ", B[i][j]);
            }
            printf("\n");
        }
    }
    
    // Scatter rows of A and B to all processes
    MPI_Scatter(A, rows_per_process * COLS, MPI_INT, 
                local_A, rows_per_process * COLS, MPI_INT, 
                0, MPI_COMM_WORLD);
    MPI_Scatter(B, rows_per_process * COLS, MPI_INT, 
                local_B, rows_per_process * COLS, MPI_INT, 
                0, MPI_COMM_WORLD);
    
    // Local matrix addition
    for (int i = 0; i < rows_per_process * COLS; i++) {
        local_C[i] = local_A[i] + local_B[i];
    }
    
    // Gather results to master
    MPI_Gather(local_C, rows_per_process * COLS, MPI_INT, 
               C, rows_per_process * COLS, MPI_INT, 
               0, MPI_COMM_WORLD);
    
    if (rank == 0) {
        printf("\nResult Matrix C:\n");
        printMatrix((int *)C, ROWS, COLS);
    }
    
    free(local_A);
    free(local_B);
    free(local_C);
    MPI_Finalize();
    return 0;
}