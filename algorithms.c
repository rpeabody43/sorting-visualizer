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

// ----------------------------------- UTIL -----------------------------------

void print_arr (int* a, int len) {
    printf("[");
    for (int i = 0; i < len; i++) {
        printf("%d", a[i]);
        if (i < n-1) {
            printf(",");
        }
    }
    printf("]\n");
}

void array_init(int len) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    int usec = tv.tv_usec;
    srand48(usec);

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
    for (int i = 0; i < n; i++) {
        size_t j = (unsigned int) (drand48()*(i+1));
        swap(i, j);
        disp(i, j, true);
    }
}

// -------------------------------- ALGORITHMS --------------------------------

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
        //disp(-1, new_idx, false);
    }

    while (l_idx <= e1) {
        temp[new_idx++] = arr[l_idx++];
        //disp(-1, new_idx, false);
    }
    while (r_idx <= e2) {
        temp[new_idx++] = arr[r_idx++];
        //disp(-1, new_idx, false);
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
            disp(i, t, false);
        }
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

void maxheapify (int root, int end) {
    int left_idx = (root << 1);
    int right_idx = left_idx + 1;
    int swap_idx = 0;
    if (left_idx > end) {
        return;
    } else if (right_idx > end) {
        swap_idx = left_idx;
    } else {
        if (arr[left_idx-1] > arr[right_idx-1]) {
            swap_idx = left_idx;
        } else {
            swap_idx = right_idx;
        }
    }

    if (!swap_idx || arr[root-1] >= arr[swap_idx-1]) { 
        return; 
    }
    swap(root-1, swap_idx-1);
    disp(root-1, swap_idx-1, false);
    maxheapify(swap_idx, end);
}

void convert_to_maxheap () {
    for (int root = n>>1; root >= 1; root--) {
        maxheapify(root, n);
    }
}

void heap_sort () {
    convert_to_maxheap();

    for (int end = n; end > 1; end--) {
        swap(0, end-1);
        disp(0, end-1, false);
        maxheapify(1, end-1);
    }
}

void inner_count_sort (int max, int* secondary_arr) {
    // arr has max of n
    int* pos = malloc((max+1) * sizeof(int));
    int* new = malloc(n * sizeof(int));
    for (int i = 0; i <= max; i++) {
        pos[i] = 0;
    }
    for (int i = 0; i < n; i++) {
        new[i] = 0;
    }

    for (int i = 0; i < n; i++) {
        pos[secondary_arr[i]]++;
    }
    for (int i = 1; i <= max; i++) {
        pos[i] += pos[i-1];
    }
    for (int i = n-1; i >= 0; i--) {
        new[pos[secondary_arr[i]]-1] = arr[i];
        pos[secondary_arr[i]]--;
    }
    for (int i = 0; i < n; i++) {
        arr[i] = new[i];
        disp(i, -1, false);
    }
    free(pos);
    free(new);
}

void counting_sort () {
    inner_count_sort(n-1, arr);
}

void radix_sort () {
    int* digit_arr = malloc(n * sizeof(int));
    for (int e = 1; (n-1) / e > 0; e *= 10) {
        for (int i = 0; i < n; i++) {
            digit_arr[i] = (arr[i] / e) % 10;
        }
        inner_count_sort(9, digit_arr);
    }
    free(digit_arr);
}

bool is_sorted () {
    for (int i = 1; i < n; i++) {
        if (arr[i-1] > arr[i]) {
            return false;
        } 
    }
    return true;
}

void bogo_sort() {
    while (!is_sorted()) {
        shuffle();
    }
}
