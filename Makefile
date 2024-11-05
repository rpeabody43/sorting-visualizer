CC=gcc
# Enable all warnings, disable the ones for multi-line comments
CCFLAGS = -Wall -Wno-comment -lm
INCLUDE_FLAGS += -lSDL2 -lSDL2main

sorting: main.o algorithms.o
	$(CC) -o sorting main.o algorithms.o $(INCLUDE_FLAGS) $(CCFLAGS)

debug: CCFLAGS += -g
debug: sorting

main.o: main.c
	$(CC) -c main.c $(CCFLAGS) 

algorithms.o: algorithms.c
	$(CC) -c algorithms.c $(CCFLAGS)

clean:
	rm sorting *.o
