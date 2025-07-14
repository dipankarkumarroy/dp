#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

// Function to calculate factorial
unsigned long long factorial(int n) {
    unsigned long long fact = 1;
    for (int i = 2; i <= n; i++)
        fact *= i;
    return fact;
}

int main(int argc, char *argv[]) {
    int rank, size;
    int start, end;
    int N = 20; // Compute sum of 1! to 20!
    unsigned long long local_sum = 0, total_sum = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Divide the range among processes
    int chunk = N / size;
    start = rank * chunk + 1;
    end = (rank == size - 1) ? N : start + chunk - 1;

    // Each process computes local sum
    for (int i = start; i <= end; i++)
        local_sum += factorial(i);

    // Reduce all local sums to total sum in root process
    MPI_Reduce(&local_sum, &total_sum, 1, MPI_UNSIGNED_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0)
        printf("Sum of factorials from 1! to 20! = %llu\n", total_sum);

    MPI_Finalize();
    return 0;
}
