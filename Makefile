FLAGS := -Wall -I/usr/include/SDL2  
all: clean main.o funcs.o
	gcc $(FLAGS) -o exec main.o funcs.o -lSDL2 -lSDL2_image

main.o:
	gcc $(FLAGS) -c src/main.c -lSDL2 -lSDL2_image

funcs.o:
	gcc $(FLAGS) -c src/funcs.c -lSDL2 -lSDL2_image



clean:
	rm -rf main.o exec