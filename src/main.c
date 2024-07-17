#include "image.h"
#include "init.h"
#include "curl.h"



int main(int argc, char** argv) {

    if (argc > 1) {
        char * text = argv[1];
        allocDests(1);
        if (*text == '"' && text[strlen(text)-1] == '"') {
            memmove(text, text+1, strlen(text)-2);
            text[strlen(text)-2 ]= '\0';
        }
        strncpy(dests[0], text, strlen(text));
    } else {
        CH_InitSDL();
        int exit_code = CH_CreateMenu();
        if (exit_code == EXIT_IMAGEPATH_URL) {
            DownloadImage(dests[0]);
            strncpy(dests[0],"res/lastimage.png",18);
        }
        else if (exit_code == EXIT_QUIT) {
            freeDests();
            CH_Quit();
            return 0;
        }
    }
    if (destLen ==1 ) {
        CH_InitImage(dests[0]);
    }
    else {};
    
    freeDests();
    CH_Quit();

    return 0;
}