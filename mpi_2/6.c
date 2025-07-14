#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define ARRAY_SIZE 20

int linearSearch(int *array, int size, int key) {
    for (int i = 0; i < size; i++) {
        if (array[i] == key) return 1;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    int rank, size, key = 7;
    int array[ARRAY_SIZE], *local_array;
    int local_found = 0, global_found = 0;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    int chunk_size = ARRAY_SIZE / size;
    local_array = (int *)malloc(chunk_size * sizeof(int));
    
    if (rank == 0) {
        // Initialize array
        printf("Array: ");
        for (int i = 0; i < ARRAY_SIZE; i++) {
            array[i] = i % 10; // Values 0-9 repeating
            printf("%d ", array[i]);
        }
        printf("\nSearching for key: %d\n", key);
    }
    
    // Scatter array to all processes
    MPI_Scatter(array, chunk_size, MPI_INT, 
               local_array, chunk_size, MPI_INT, 
               0, MPI_COMM_WORLD);
    
    // Local linear search
    local_found = linearSearch(local_array, chunk_size, key);
    
    // Reduce using logical OR to check if any process found the key
    MPI_Reduce(&local_found, &global_found, 1, MPI_INT, MPI_LOR, 0, MPI_COMM_WORLD);
    
    if (rank == 0) {
        if (global_found) printf("Key %d found in the array.\n", key);
        else printf("Key %d not found in the array.\n", key);
    }
    
    free(local_array);
    MPI_Finalize();
    return 0;
}