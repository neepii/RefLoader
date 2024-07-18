FLAGS := -Wall -Dmain=main -IC:/msys64/ucrt64/include/SDL2 -IC:/msys64/ucrt64/include/curl

SRC := $(shell C:\\msys64\\usr\\bin\\find.exe ./src -name *.c)
OBJ := $(patsubst ./src/%.c,%.o,$(SRC))
LIB := -LC:/msys64/ucrt64/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lcomdlg32 -lcurl -luuid -mwindows
EXEC_NAME := RefLoader
# -mwindows to disable cmd output

all: $(EXEC_NAME)

$(EXEC_NAME): $(OBJ) resource.res
	gcc $(FLAGS) -o $@  $^ $(LIB)

%.o: src/%.c
	gcc $(FLAGS) -c $^ $(LIB)


# resource files that will be embed into executable
resource.res: src/resource.rc 
	windres $^ -O coff -o $@

clean:
	rm -rf $(OBJ) $(EXEC_NAME)