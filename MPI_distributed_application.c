// implementing merge sort using MPI
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define ARRAY_SIZE 1000

void merge(int *arr, int l , int m, int r){
    int i , j , k;
    int n1 = m-l+1;
    int n2 = r - m;

    int *L = (int *)malloc(n1 * sizeof(int));
    int *R = (int *)malloc(n2 * sizeof(int));

    for (i = 0; i < n1; i++){
        L[i] = arr[l + i];
    }
    for (j = 0; j < n2; j++){
        R[j] = arr[m + 1 + j];
    }

    i = 0; j = 0; k = l;

    while(i < n1 && j < n2){
        if (L[i] <= R[j]){
            arr[k++] = L[i++];
        }
        else{
            arr[k++] = R[j++];
        }
    }

    while(i < n1){
        arr[k++] = L[i++];
    }
    while(j < n2){
        arr[k++] = R[j++];
    }

    free(L);
    free(R);
}

void merge_sort(int *arr, int l, int r){
    if (l < r){
        int m = l + (r-l) / 2;
        merge_sort(arr, l , m);
        merge_sort(arr, m+1, r);
        merge(arr, l , m ,r);
    }
}

int main(int argc, char *argv[]){
    int rank, size;
    int *arr = NULL;
    int *sub_arr = NULL;
    int sub_arr_size;
    int *sorted_arr = NULL;
    int *recv_buf = NULL;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0){
        arr = (int *)malloc(ARRAY_SIZE * sizeof(int));
        for(int i = 0; i < ARRAY_SIZE; i++){
            arr[i] = rand() % 10000;
        }
        sorted_arr = (int *)malloc(ARRAY_SIZE * sizeof(int));
    }
    sub_arr_size = ARRAY_SIZE / size;
    sub_arr = (int *)malloc(sub_arr_size * sizeof(int));
    MPI_Scatter(arr, sub_arr_size, MPI_INT, sub_arr, sub_arr_size, MPI_INT, 0, MPI_COMM_WORLD);

    merge_sort(sub_arr, 0, sub_arr_size - 1);

    MPI_Gather(sub_arr, sub_arr_size, MPI_INT, sorted_arr, sub_arr_size, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0){
        int segment_size = sub_arr_size;
        while (segment_size < ARRAY_SIZE) {
            for (int i = 0; i < ARRAY_SIZE; i += 2 * segment_size) {
                int mid = i + segment_size - 1;
                int end = (i + 2 * segment_size - 1 < ARRAY_SIZE) ? i + 2 * segment_size - 1 : ARRAY_SIZE - 1;
                merge(sorted_arr, i, mid, end);
            }
            segment_size *= 2;
        }
        printf("Sorted array:\n");
        for (int i = 0; i < ARRAY_SIZE; i++) {
            printf("%d ", sorted_arr[i]);
        }
        printf("\n");
        free(arr);
        free(sorted_arr);
    }

    free(sub_arr);

    MPI_Finalize();
    return 0;

}
