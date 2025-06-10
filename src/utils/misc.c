/* for musl libc */
#ifndef F_OK
#define F_OK 0
#endif

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include <sys/time.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/file.h>
#include <errno.h>

size_t _cur_time;
size_t get_epoch(){
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return 1000000 * tv.tv_sec + tv.tv_usec;
}


char* which(const char* cmd){
    char* fullPath = strdup(getenv("PATH")); // Duplicate the PATH string

    struct stat buffer;
    int exists;
    char* fileOrDirectory = (char*)cmd;
    char *fullfilename = calloc(1024, sizeof(char));

    char *token = strtok(fullPath, ":");

    /* walk through other tokens */
    while( token != NULL ){
        sprintf(fullfilename, "%s/%s", token, fileOrDirectory);
        exists = stat( fullfilename, &buffer);
        if ( exists == 0 ) {
            free(fullPath); // Free the duplicated string
            return fullfilename;
        }

      token = strtok(NULL, ":"); /* next token */
    }
    free(fullPath); // Free the duplicated string
    free(fullfilename);
    return "";
}