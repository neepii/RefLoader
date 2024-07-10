FLAGS := -Wall
LIB := -IC:/msys64/mingw64/include/SDL2 -Dmain=main -LC:/msys64/mingw64/lib -lmingw32 -mwindows -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf
# -mwindows to disable cmd output

all: clean main.o funcs.o init.o
	gcc ${FLAGS} -o exec main.o cross.o init.o ${LIB}

main.o:
	gcc ${FLAGS} -c src/main.c ${LIB}

funcs.o:
	gcc ${FLAGS} -c src/cross.c ${LIB}

init.o:
	gcc ${FLAGS} -c src/init.c ${LIB}




clean:
	rm -rf main.o cross.o init.o exec