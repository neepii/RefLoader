#include "image.h"
#include "init.h"
#include "curl.h"



int main(int argc, char** argv) {

    char * Destination = (char*) malloc(sizeof(char) * 200);

    CH_InitSDL();

    int exit_code = CH_CreateMenu(Destination);

    if (exit_code == EXIT_IMAGEPATH_URL) {
        DownloadImage(Destination);
        strncpy(Destination,"res/lastimage.png",18);
    }

    CH_InitImage(Destination);
    free(Destination);
    CH_Quit();

    return 0;
}