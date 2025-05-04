#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <omp.h>
#include <algorithm>

using namespace std;

// -----------------------------
// Utility: Time Measurement
// -----------------------------
double getTime() {
    return omp_get_wtime();
}

// -----------------------------
// Sequential Bubble Sort
// -----------------------------
void sequentialBubbleSort(vector<int>& arr) {
    int n = arr.size();
    for (int i = 0; i < n - 1; ++i)
        for (int j = 0; j < n - i - 1; ++j)
            if (arr[j] > arr[j + 1])
                swap(arr[j], arr[j + 1]);
}

// -----------------------------
// Parallel Bubble Sort using OpenMP
// -----------------------------
void parallelBubbleSort(vector<int>& arr) {
    int n = arr.size();
    for (int i = 0; i < n; i++) {
        #pragma omp parallel for
        for (int j = i % 2; j < n - 1; j += 2) {
            if (arr[j] > arr[j + 1]) {
                swap(arr[j], arr[j + 1]);
            }
        }
    }
}

// -----------------------------
// Sequential Merge Sort
// -----------------------------
void merge(vector<int>& arr, int left, int mid, int right) {
    vector<int> temp(right - left + 1);
    int i = left, j = mid + 1, k = 0;

    while (i <= mid && j <= right)
        temp[k++] = (arr[i] < arr[j]) ? arr[i++] : arr[j++];

    while (i <= mid)
        temp[k++] = arr[i++];

    while (j <= right)
        temp[k++] = arr[j++];

    for (int i = left; i <= right; ++i)
        arr[i] = temp[i - left];
}

void sequentialMergeSort(vector<int>& arr, int left, int right) {
    if (left >= right) return;

    int mid = (left + right) / 2;
    sequentialMergeSort(arr, left, mid);
    sequentialMergeSort(arr, mid + 1, right);
    merge(arr, left, mid, right);
}

// -----------------------------
// Parallel Merge Sort using OpenMP
// -----------------------------
void parallelMergeSort(vector<int>& arr, int left, int right, int depth = 0) {
    if (left >= right) return;

    int mid = (left + right) / 2;

    if (depth <= 4) { // Limit parallel depth
        #pragma omp parallel sections
        {
            #pragma omp section
            parallelMergeSort(arr, left, mid, depth + 1);
            #pragma omp section
            parallelMergeSort(arr, mid + 1, right, depth + 1);
        }
    } else {
        sequentialMergeSort(arr, left, mid);
        sequentialMergeSort(arr, mid + 1, right);
    }

    merge(arr, left, mid, right);
}

// -----------------------------
// Main Function
// -----------------------------
int main() {
    const int SIZE = 10000;
    vector<int> arr1(SIZE), arr2, arr3, arr4;

    srand(time(0));
    for (int i = 0; i < SIZE; ++i)
        arr1[i] = rand() % 10000;

    arr2 = arr3 = arr4 = arr1;

    double start, end;

    // Sequential Bubble Sort
    start = getTime();
    sequentialBubbleSort(arr1);
    end = getTime();
    cout << "Sequential Bubble Sort Time: " << (end - start) << " seconds\n";

    // Parallel Bubble Sort
    start = getTime();
    parallelBubbleSort(arr2);
    end = getTime();
    cout << "Parallel Bubble Sort Time: " << (end - start) << " seconds\n";

    // Sequential Merge Sort
    start = getTime();
    sequentialMergeSort(arr3, 0, SIZE - 1);
    end = getTime();
    cout << "Sequential Merge Sort Time: " << (end - start) << " seconds\n";

    // Parallel Merge Sort
    start = getTime();
    parallelMergeSort(arr4, 0, SIZE - 1);
    end = getTime();
    cout << "Parallel Merge Sort Time: " << (end - start) << " seconds\n";

    return 0;
}
