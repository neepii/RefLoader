FLAGS := -Wall -I/usr/include/SDL2  
all: main.o
	gcc $(FLAGS) -o exec main.o -lSDL2 -lSDL2_image

main.o:
	gcc $(FLAGS) -c main.c -lSDL2 -lSDL2_image

clean:
	rm -rf main.o exec