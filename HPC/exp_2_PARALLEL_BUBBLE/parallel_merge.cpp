#include<iostream>
#include<omp.h>
#include<vector>
using namespace std;

void merge(int arr[], int low, int mid, int high) {
    int n1 = mid - low + 1;
    int n2 = high - mid;

    vector<int> left(n1), right(n2);

    for (int i = 0; i < n1; i++) left[i] = arr[low + i];
    for (int j = 0; j < n2; j++) right[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = low;
    while (i < n1 && j < n2) {
        if (left[i] <= right[j]) arr[k++] = left[i++];
        else arr[k++] = right[j++];
    }

    while (i < n1) arr[k++] = left[i++];
    while (j < n2) arr[k++] = right[j++];
}

void parallelMergeSort(int arr[], int low, int high, int depth = 0) {
    if (low < high) {
        int mid = (low + high) / 2;

        if (depth < 4) {  // Limit recursion depth for parallelism
            #pragma omp parallel sections
            {
                #pragma omp section
                parallelMergeSort(arr, low, mid, depth + 1);

                #pragma omp section
                parallelMergeSort(arr, mid + 1, high, depth + 1);
            }
        } else {
            // Fallback to serial
            parallelMergeSort(arr, low, mid, depth + 1);
            parallelMergeSort(arr, mid + 1, high, depth + 1);
        }
        merge(arr, low, mid, high);
    }
}

void mergeSort(int arr[], int low, int high) {
    if (low < high) {
        int mid = (low + high) / 2;
        mergeSort(arr, low, mid);
        mergeSort(arr, mid + 1, high);
        merge(arr, low, mid, high);
    }
}

void printArray(int arr[], int n) {
    for (int i = 0; i < n; i++) cout << arr[i] << " ";
    cout << "\n";
}

int main() {
    int n = 100000;
    int* arr = new int[n];

    for (int i = 0, j = n; i < n; i++, j--) arr[i] = j;

    double start_time = omp_get_wtime();
    mergeSort(arr, 0, n - 1);
    double end_time = omp_get_wtime();
    cout << "Time taken by sequential algorithm: " << end_time - start_time << " seconds\n";

    // Reset array
    for (int i = 0, j = n; i < n; i++, j--) arr[i] = j;

    start_time = omp_get_wtime();
    #pragma omp parallel
    {
        #pragma omp single
        {
            parallelMergeSort(arr, 0, n - 1);
        }
    }
    end_time = omp_get_wtime();
    cout << "Time taken by parallel algorithm: " << end_time - start_time << " seconds\n";

    delete[] arr;
    return 0;
}
