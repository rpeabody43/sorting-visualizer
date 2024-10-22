CC=gcc
# Enable all warnings, disable the ones for multi-line comments
CCFLAGS = -Wall -Wno-comment
INCLUDE_FLAGS += -lSDL2 -lSDL2main

sorting: main.o 
	$(CC) $(CCFLAGS) -o sorting main.o $(INCLUDE_FLAGS)

debug: CCFLAGS += -g
debug: sorting

main.o: main.c
	$(CC) $(CCFLAGS) -c main.c

clean:
	rm sorting *.o
