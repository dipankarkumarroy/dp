#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define ARRAY_SIZE 20
#define CHUNK_SIZE 4

int main(int argc, char *argv[]) {
    int rank, size;
    int array[ARRAY_SIZE], chunk[CHUNK_SIZE];
    int partial_sum = 0, total_sum = 0;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (rank == 0) {
        // Initialize array
        printf("Initial array: ");
        for (int i = 0; i < ARRAY_SIZE; i++) {
            array[i] = i + 1; // 1 to 20
            printf("%d ", array[i]);
        }
        printf("\n");
    }
    
    // Scatter array to all processes
    MPI_Scatter(array, CHUNK_SIZE, MPI_INT, 
               chunk, CHUNK_SIZE, MPI_INT, 
               0, MPI_COMM_WORLD);
    
    // Compute partial sum
    for (int i = 0; i < CHUNK_SIZE; i++) {
        partial_sum += chunk[i];
    }
    printf("Process %d partial sum: %d\n", rank, partial_sum);
    
    // Reduce all partial sums to get total sum
    MPI_Reduce(&partial_sum, &total_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    
    if (rank == 0) {
        printf("Total sum: %d\n", total_sum);
    }
    
    MPI_Finalize();
    return 0;
}