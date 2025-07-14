#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int rank, size, n = 1000000;
    double step, x, local_sum = 0.0, total_sum = 0.0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    step = 1.0 / (double)n;

    // Each process computes a partial sum
    for (int i = rank; i < n; i += size) {
        x = (i + 0.5) * step;
        local_sum += 4.0 / (1.0 + x * x);
    }

    // Reduce all partial sums to get total sum
    MPI_Reduce(&local_sum, &total_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        double pi = step * total_sum;
        printf("Approximate value of Pi = %.15f\n", pi);
    }

    MPI_Finalize();
    return 0;
}
