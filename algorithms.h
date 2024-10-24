#pragma once

extern int* arr;
extern int n;
extern int max_value;

// Util
void array_init(int len, int max);
void fill (int len, int max);
void shuffle();
void print_arr();

// Sorting algorithms
void bubble_sort();
void selection_sort();
void insertion_sort();
void merge_sort();
