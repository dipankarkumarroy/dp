#include <stdio.h>
#include <mpi.h>

double f(double x) {
    return 4.0 / (1.0 + x * x);
}

int main(int argc, char *argv[]) {
    int rank, size, n = 1000000;
    double h, sum = 0.0, total_sum, pi;
    double start_time, end_time;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    h = 1.0 / (double) n;
    start_time = MPI_Wtime();
    
    // Each process computes a portion of the integral
    for (int i = rank; i < n; i += size) {
        double x = h * ((double)i + 0.5);
        sum += f(x);
    }
    
    // Sum all partial results
    MPI_Reduce(&sum, &total_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    
    if (rank == 0) {
        pi = h * total_sum;
        end_time = MPI_Wtime();
        printf("Approximate value of PI: %.16f\n", pi);
        printf("Time taken: %.6f seconds\n", end_time - start_time);
    }
    
    MPI_Finalize();
    return 0;
}