FLAGS := -Wall
SRC := $(shell C:\\msys64\\usr\\bin\\find.exe ./src -name *.c)
OBJ := $(patsubst ./src/%.c,%.o,$(SRC))
LIB := -IC:/msys64/mingw64/include/SDL2 -Dmain=main -LC:/msys64/mingw64/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf
EXEC_NAME := exec
# -mwindows to disable cmd output

all: $(EXEC_NAME)

$(EXEC_NAME): *.o
	gcc $(FLAGS) -o $@ $^ $(LIB)

%.o: src/%.c
	gcc $(FLAGS) -c $^ $(LIB)

clean:
	rm -rf $(OBJ) $(EXEC_NAME)