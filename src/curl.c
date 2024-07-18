#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <stdbool.h>
#include <shlobj.h>
#include "init.h"



bool isHTTPS(char * path) {
    char first_eight[9]; // including null-terminator
    strncpy(first_eight, path, 8);
    int output = strcmp(first_eight, "https://"); 
    return (output == 0) ? true : false;
}
size_t write_data(void* ptr, size_t size, size_t nmemb, FILE* stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

bool DownloadImage(char* path) {
    CURL* curl;
    CURLcode res;
    FILE * fp;
    wchar_t * wpath = L"D:\\proj\\refloader\\res\\lastimage.png";
    // SHGetKnownFolderPath(&FOLDERID_RoamingAppData, 0, NULL, &wpath);
    // wpath = wcscat(wpath, L"\\RefLoader\\lastimage.png");


    fp = _wfopen(wpath, L"wb");
    if (fp == NULL) {
        fprintf(stderr, "ERROR: Cant open file");
        return false;
    }
    curl = curl_easy_init();
    if (curl == NULL) {
        fprintf(stderr, "ERROR: Curl is NULL");
        return false;
    }

    curl_easy_setopt(curl, CURLOPT_URL, path);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "Error:%s\n", curl_easy_strerror(res));
        return false;
    }
    curl_easy_cleanup(curl);


    curl_easy_cleanup(curl);
    fclose(fp);
    return true;
}
