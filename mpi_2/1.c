#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

void quickSort(int *array, int left, int right) {
    if (left < right) {
        int pivot = array[right], i = left - 1;
        for (int j = left; j <= right - 1; j++) {
            if (array[j] < pivot) {
                i++;
                int temp = array[i];
                array[i] = array[j];
                array[j] = temp;
            }
        }
        int temp = array[i + 1];
        array[i + 1] = array[right];
        array[right] = temp;
        int partition = i + 1;
        quickSort(array, left, partition - 1);
        quickSort(array, partition + 1, right);
    }
}

void merge(int *arr1, int n1, int *arr2, int n2, int *result) {
    int i = 0, j = 0, k = 0;
    while (i < n1 && j < n2) {
        if (arr1[i] < arr2[j]) result[k++] = arr1[i++];
        else result[k++] = arr2[j++];
    }
    while (i < n1) result[k++] = arr1[i++];
    while (j < n2) result[k++] = arr2[j++];
}

int main(int argc, char *argv[]) {
    int rank, size, n = 20;
    int *data = NULL, *chunk = NULL, *sorted = NULL;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (rank == 0) {
        data = (int *)malloc(n * sizeof(int));
        srand(time(NULL));
        printf("Original array: ");
        for (int i = 0; i < n; i++) {
            data[i] = rand() % 100;
            printf("%d ", data[i]);
        }
        printf("\n");
    }
    
    int chunk_size = n / size;
    chunk = (int *)malloc(chunk_size * sizeof(int));
    MPI_Scatter(data, chunk_size, MPI_INT, chunk, chunk_size, MPI_INT, 0, MPI_COMM_WORLD);
    
    quickSort(chunk, 0, chunk_size - 1);
    
    if (rank == 0) {
        sorted = (int *)malloc(n * sizeof(int));
    }
    
    MPI_Gather(chunk, chunk_size, MPI_INT, sorted, chunk_size, MPI_INT, 0, MPI_COMM_WORLD);
    
    if (rank == 0) {
        for (int step = 1; step < size; step *= 2) {
            for (int i = 0; i < size; i += 2 * step) {
                if (i + step < size) {
                    int *arr1 = sorted + i * chunk_size;
                    int n1 = chunk_size * step;
                    int *arr2 = sorted + (i + step) * chunk_size;
                    int n2 = (i + 2 * step <= size) ? chunk_size * step : n - (i + step) * chunk_size;
                    int *temp = (int *)malloc((n1 + n2) * sizeof(int));
                    merge(arr1, n1, arr2, n2, temp);
                    for (int j = 0; j < n1 + n2; j++) arr1[j] = temp[j];
                    free(temp);
                }
            }
        }
        
        printf("Sorted array: ");
        for (int i = 0; i < n; i++) printf("%d ", sorted[i]);
        printf("\n");
        
        free(data);
        free(sorted);
    }
    
    free(chunk);
    MPI_Finalize();
    return 0;
}