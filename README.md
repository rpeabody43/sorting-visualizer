# Sorting Visualizer
A simple sorting algorithm visualizer featuring pretty colors. Written using 
C and [SDL2](https://www.libsdl.org/)

Disclaimer: Although many of these algorithms perform similarly to each other in 
the real world, they vary wildly in runtime here due to how they are drawn to the
screen. This project is meant to demonstrate how sorting algorithms look, not how
fast they run.  

## Build instructions
I used Ubuntu 22.04 via WSL for this project but things shouldn't be that 
different across distros.

1. Clone the repo
2. Install SDL2

Many ways to do this, Debian based distros can run
```
sudo apt-get install libsdl2-2.0-0
```
3. Run `make`

## Usage
```
sorting {algorithm} {items=100} {delay=0}
```

The available algorithms are:

- `bubble` - Bubble Sort
- `insertion` - Insertion Sort
- `selection` - Selection Sort
- `merge` - Merge Sort
- `quick` - Quicksort, Left/Left Pointers (Lomuto Partition)
- `quicklr` - Quicksort, Left/Right Pointers (Hoare Partition)
- `heap` - Heapsort
- `counting` - Counting Sort
- `radix` - Radix Sort
- `bogo` - Bogosort

# Example
https://github.com/user-attachments/assets/01ea9292-b1ef-4bb3-a22e-6a91a9a68f8c

