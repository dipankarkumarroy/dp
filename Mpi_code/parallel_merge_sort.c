#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

// Function to merge two sorted arrays
void merge(int *arr, int l, int m, int r) {
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    // Temp arrays
    int *L = (int *)malloc(n1 * sizeof(int));
    int *R = (int *)malloc(n2 * sizeof(int));

    // Copy data
    for (i = 0; i < n1; i++) L[i] = arr[l + i];
    for (j = 0; j < n2; j++) R[j] = arr[m + 1 + j];

    i = 0; j = 0; k = l;

    // Merge temp arrays
    while (i < n1 && j < n2)
        arr[k++] = (L[i] <= R[j]) ? L[i++] : R[j++];

    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];

    free(L);
    free(R);
}

// Serial merge sort
void merge_sort(int *arr, int l, int r) {
    if (l < r) {
        int m = (l + r) / 2;
        merge_sort(arr, l, m);
        merge_sort(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}

int main(int argc, char *argv[]) {
    int rank, size, n = 8;
    int data[8] = {12, 3, 18, 24, 5, 8, 15, 6}; // Sample array
    int *local_data, *gathered_data;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int local_n = n / size;
    local_data = (int *)malloc(local_n * sizeof(int));

    // Scatter array parts to all processes
    MPI_Scatter(data, local_n, MPI_INT, local_data, local_n, MPI_INT, 0, MPI_COMM_WORLD);

    // Sort local part
    merge_sort(local_data, 0, local_n - 1);

    // Gather sorted parts
    gathered_data = (rank == 0) ? (int *)malloc(n * sizeof(int)) : NULL;
    MPI_Gather(local_data, local_n, MPI_INT, gathered_data, local_n, MPI_INT, 0, MPI_COMM_WORLD);

    // Merge all parts in root
    if (rank == 0) {
        for (int step = local_n; step < n; step *= 2) {
            for (int i = 0; i < n; i += 2 * step) {
                int mid = i + step - 1;
                int right_end = ((i + 2 * step - 1) < (n - 1)) ? (i + 2 * step - 1) : (n - 1);
                if (mid < right_end)
                    merge(gathered_data, i, mid, right_end);
            }
        }

        printf("Sorted array: ");
        for (int i = 0; i < n; i++)
            printf("%d ", gathered_data[i]);
        printf("\n");
        free(gathered_data);
    }

    free(local_data);
    MPI_Finalize();
    return 0;
}
