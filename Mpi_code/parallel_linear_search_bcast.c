#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 12  // Array size

int main(int argc, char *argv[]) {
    int rank, size, key, local_index = -1, global_index;
    int data[N] = {10, 23, 45, 9, 18, 99, 67, 12, 34, 89, 50, 21};

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Input key from root process
    if (rank == 0) {
        printf("Enter the key to search: ");
        scanf("%d", &key);
    }

    // Broadcast the array and the key to all processes
    MPI_Bcast(data, N, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&key, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Each process computes its search range
    int chunk = N / size;
    int start = rank * chunk;
    int end = (rank == size - 1) ? N : start + chunk;

    // Local search
    for (int i = start; i < end; i++) {
        if (data[i] == key) {
            local_index = i;
            break;
        }
    }

    // Reduce to find the minimum index (ignore -1s by replacing with large number)
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
