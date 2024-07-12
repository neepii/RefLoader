#include "image.h"
#include "init.h"



int main(int argc, char** argv) {

    char * Destination = (char*) malloc(sizeof(char) * 100);

    CH_InitSDL();

    if (!CH_CreateMenu(Destination)) {
        CH_InitImage(Destination);
    } 
    free(Destination);
    CH_Quit();

    return 0;
}