/* for musl libc */
#ifndef F_OK
#define F_OK 0
#endif

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "which.h"

char* which(char* cmd){
    char* fullPath = strdup(getenv("PATH")); // Duplicate the PATH string

    struct stat buffer;
    int exists;
    char* fileOrDirectory = cmd;
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
