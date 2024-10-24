#include <sys/time.h>
#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

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

void array_init(int len, int max) {
    arr = malloc(len * sizeof(int));
    fill(len, max);
    n = len;
    max_value = max;
}

void fill (int len, int max) {
    float increment = (float) max / len;
    int val = 0;
    for (int i = len-1; i >= 0; i--) {
        arr[i] = val += increment;
    }
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
        disp(i, j);
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
        }
        disp(j, -1);
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
        }
        swap(i, minindex);
        disp(i, minindex);
    }
}

void insertion_sort () {
    int key, i, j;
    for (i = 1; i < n; i++) {
        key = arr[i];
        for (j = i-1; j >= 0 && key < arr[j]; j--) {
            arr[j+1] = arr[j];
        }
        arr[j+1] = key;
        disp(j+1, -1);
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
    }

    while (l_idx <= e1) {
        temp[new_idx++] = arr[l_idx++];
    }
    while (r_idx <= e2) {
        temp[new_idx++] = arr[r_idx++];
    }

    // Copy back to original
    for (i = s1; i <= e2; i++) {
        arr[i] = temp[i];
        disp(i, -1);
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
