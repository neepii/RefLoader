FLAGS := -Wall
LIB := -IC:/msys64/mingw64/include/SDL2 -Dmain=main -LC:/msys64/mingw64/lib -lmingw32 -mwindows -lSDL2main -lSDL2 -lSDL2_image

all: clean main.o funcs.o
	gcc ${FLAGS} -o exec main.o funcs.o ${LIB}

main.o:
	gcc ${FLAGS} -c src/main.c ${LIB}

funcs.o:
	gcc ${FLAGS} -c src/funcs.c ${LIB}



clean:
	rm -rf main.o funcs.o exec