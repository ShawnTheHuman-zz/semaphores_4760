# Shawn brown
# project 3 - 4760
# makefile



CC      = gcc
CFLAGS  = -g -Wall -Wshadow


EXEC1 := runsim
SRC1 := license.c runsim.c semaphore.c
OBJ1 := $(patsubst %.cpp, %.o, $(SRC1))

all: $(EXEC1)

$(EXEC1): $(OBJ1)
	$(CC) $(CFLAGS) -o $(EXEC1) $(OBJ1)

EXEC2 := testsim
SRC2 :=  license.c testsim.c semaphore.c
OBJ2 := $(patsubst %.cpp, %.o, $(SRC2))

all: $(EXEC2)

$(EXEC2): $(OBJ2)
	$(CC) $(CFLAGS) -o $(EXEC2) $(OBJ2)

.PHONY: clean

clean:
	rm -f $(EXEC1) $(EXEC2) *.o logfile.data
