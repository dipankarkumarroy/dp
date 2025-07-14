#include <stdio.h>
#include <mpi.h>

double factorial(int n) {
    double result = 1.0;
    for (int i = 1; i <= n; i++) result *= i;
    return result;
}

int main(int argc, char *argv[]) {
    int rank, size, terms = 20;
    double partial_sum = 0.0, total_sum = 0.0;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    // Each process computes a portion of the series
    for (int i = rank + 1; i <= terms; i += size) {
        partial_sum += (double)i / factorial(i);
    }
    
    // Sum all partial results
    MPI_Reduce(&partial_sum, &total_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    
    if (rank == 0) {
        printf("Sum of the series 1/1! + 2/2! + ... + 20/20! = %.15f\n", total_sum);
    }
    
    MPI_Finalize();
    return 0;
}