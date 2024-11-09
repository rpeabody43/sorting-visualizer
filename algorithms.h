#pragma once

extern int* arr;
extern int n;
extern int max_value;

// Util
void array_init(int len);
void shuffle();
void print_arr(int* a, int len);

// Sorting algorithms
void bubble_sort();
void selection_sort();
void insertion_sort();
void merge_sort();
void quick_sort();
void quick_sort_lr(); // quick sort left right pointers
void heap_sort();
void counting_sort();
void radix_sort();
void bogo_sort();
