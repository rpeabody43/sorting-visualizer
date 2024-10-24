CC=gcc
# Enable all warnings, disable the ones for multi-line comments
CCFLAGS = -Wall -Wno-comment
INCLUDE_FLAGS += -lSDL2 -lSDL2main

sorting: main.o algorithms.o
	$(CC) $(CCFLAGS) -o sorting main.o algorithms.o $(INCLUDE_FLAGS)

debug: CCFLAGS += -g
debug: sorting

main.o: main.c
	$(CC) $(CCFLAGS) -c main.c

algorithms.o: algorithms.c
	$(CC) $(CCFLAGS) -c algorithms.c

clean:
	rm sorting *.o
