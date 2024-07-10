#include "cross.h"
#include "init.h"



int main(int argc, char** argv) {

    char * Destination = (char*) malloc(sizeof(char) * 100);

    CH_InitSDL();

    CH_CreateMenu(Destination);

    CH_InitCross(Destination);

    CH_Quit();

    return 0;
}