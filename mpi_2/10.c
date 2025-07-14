#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define ARRAY_SIZE 20
#define VALUES_PER_PROCESS 4

int main(int argc, char *argv[]) {
    int rank, size;
    int array[ARRAY_SIZE];
    int partial_sum = 0, total_sum = 0;
    int *partial_sums = NULL;
    
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
        partial_sums = (int *)malloc(size * sizeof(int));
    }
    
    // Broadcast entire array to all processes
    MPI_Bcast(array, ARRAY_SIZE, MPI_INT, 0, MPI_COMM_WORLD);
    
    // Each process computes partial sum of distinct 4 values
    for (int i = 0; i < VALUES_PER_PROCESS; i++) {
        int index = (rank * VALUES_PER_PROCESS + i) % ARRAY_SIZE;
        partial_sum += array[index];
    }
    printf("Process %d partial sum: %d\n", rank, partial_sum);
    
    // Gather all partial sums to master
    MPI_Gather(&partial_sum, 1, MPI_INT, 
              partial_sums, 1, MPI_INT, 
              0, MPI_COMM_WORLD);
    
    if (rank == 0) {
        // Compute total sum
        for (int i = 0; i < size; i++) {
            total_sum += partial_sums[i];
        }
        printf("Total sum: %d\n", total_sum);
        free(partial_sums);
    }
    
    MPI_Finalize();
    return 0;
}