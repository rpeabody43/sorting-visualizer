#include <sys/time.h>
#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "algorithms.h"
#include "display.h"

int* arr;
int n;
int max_value;

// -------------------------------- ALGORITHMS --------------------------------

void print_arr () {
    printf("[");
    for (int i = 0; i < n; i++) {
        printf("%d", arr[i]);
        if (i < n-1) {
            printf(",");
        }
    }
    printf("]\n");
}

void array_init(int len) {
    arr = malloc(len * sizeof(int));
    for (int i = len-1; i >= 0; i--) {
        arr[i] = i;
    }
    n = len;
    max_value = len-1;
}

void swap (int i, int j) {
    int temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
}

// Fisher-Yates Shuffle
void shuffle () {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    int usec = tv.tv_usec;
    srand48(usec);

    for (int i = 0; i < n-1; i++) {
        size_t j = (unsigned int) (drand48()*(i+1));
        swap(i, j);
        disp(i, j, true);
    }
}
// ----------------------------------- UTIL -----------------------------------

void bubble_sort () {
    int i, j;
    for (i = 0; i < n-1; i++) {
        for (j = 0; j < n-i-1; j++) {
            if (arr[j] > arr[j+1]) {
                swap(j, j+1);
            }
            disp(j, -1, false);
        }
    }
}

void selection_sort () {
    int minindex, i, j;
    for (i = 0; i < n-1; i++) {
        minindex = i;
        for (j = i+1; j < n; j++) {
            if (arr[j] < arr[minindex]) {
                minindex = j;
            }
            disp(i, minindex, false);
        }
        swap(i, minindex);
    }
}

void insertion_sort () {
    int key, i, j;
    for (i = 1; i < n; i++) {
        key = arr[i];
        for (j = i-1; j >= 0 && key < arr[j]; j--) {
            arr[j+1] = arr[j];
            disp(j+1, -1, false);
        }
        arr[j+1] = key;
    }
}

void merge (int s1, int e1, int s2, int e2) {
    // Make a copy of the array
    int* temp = malloc(n * sizeof(int));
    int i;
    for (i = s1; i <= e2; i++) {
        temp[i] = arr[i];
    }

    int l_idx = s1, r_idx = s2, new_idx = s1;

    while (l_idx <= e1 && r_idx <= e2) {
        if (arr[l_idx] <= arr[r_idx]) {
            temp[new_idx++] = arr[l_idx++];
        } else {
            temp[new_idx++] = arr[r_idx++];
        }
        disp(-1, new_idx, false);
    }

    while (l_idx <= e1) {
        temp[new_idx++] = arr[l_idx++];
        disp(-1, new_idx, false);
    }
    while (r_idx <= e2) {
        temp[new_idx++] = arr[r_idx++];
        disp(-1, new_idx, false);
    }

    // Copy back to original
    for (i = s1; i <= e2; i++) {
        arr[i] = temp[i];
        disp(i, -1, false);
    }

    free(temp);
}

void ms_recurse (int start, int end) {
    if (start < end) {
        int middle = (start + end) >> 1;
        ms_recurse(start, middle);
        ms_recurse(middle+1, end);
        merge(start, middle, middle+1, end);
    }
}

void merge_sort () {
    ms_recurse(0, n-1);
}

typedef int(*partition_t)(int, int);

int lomuto_partition (int l, int r) {
    int pivot, i, t;
    pivot = arr[r];
    t = l;
    for (i = l; i < r; i++) {
        if (arr[i] <= pivot) {
            swap(t++, i);
        }
        disp(i, t, false);
    }
    swap(t, r);
    return t;
}

int hoare_partition (int l, int r) {
    int pivot = arr[(l + r) / 2];
    swap(l, r);
    while (l < r) {
        swap(l, r);
        while (arr[l] < pivot) {
            l++;
        }
        while (arr[r] > pivot) {
            r--;
        }
        disp(l, r, false);
    }
    return r;
}

void qs_recurse (int l, int r, partition_t partition, bool right_inclusive) {
    if (l < r) {
        int pivot_idx = partition(l, r);
        if (right_inclusive) {
            qs_recurse(l, pivot_idx, partition, right_inclusive);
        } else {
            qs_recurse(l, pivot_idx-1, partition, right_inclusive);
        }
        qs_recurse(pivot_idx+1, r, partition, right_inclusive);
    }
}

void quick_sort () {
    qs_recurse(0, n-1, lomuto_partition, false);
}

void quick_sort_lr () {
    qs_recurse(0, n-1, hoare_partition, true);
}
