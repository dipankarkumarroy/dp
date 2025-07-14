#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 12  // Total number of elements in array

int main(int argc, char *argv[]) {
    int rank, size, key, local_index = -1, global_index = -1;
    int data[N] = {10, 23, 45, 9, 18, 99, 67, 12, 34, 89, 50, 21};
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (N % size != 0) {
        if (rank == 0)
            printf("Array size (%d) must be divisible by number of processes (%d)\n", N, size);
        MPI_Finalize();
        return 0;
    }

    int chunk = N / size;
    int local_data[chunk];

    // Scatter the array
    MPI_Scatter(data, chunk, MPI_INT, local_data, chunk, MPI_INT, 0, MPI_COMM_WORLD);

    // Root process takes key input
    if (rank == 0) {
        printf("Enter the key to search: ");
        scanf("%d", &key);
    }

    // Broadcast the key to all processes
    MPI_Bcast(&key, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Local linear search
    for (int i = 0; i < chunk; i++) {
        if (local_data[i] == key) {
            local_index = rank * chunk + i;
            break;
        }
    }

    // Find the minimum index using MPI_Reduce (ignore -1 by replacing it with large number)
    int local_result = (local_index == -1) ? N + 1 : local_index;
    MPI_Reduce(&local_result, &global_index, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        if (global_index == N + 1)
            printf("Key %d not found in the array.\n", key);
        else
            printf("Key %d found at index %d (0-based).\n", key, global_index);
    }

    MPI_Finalize();
    return 0;
}
