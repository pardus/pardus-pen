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

#include <dirent.h>
#include <unistd.h>
#include <stdio.h>

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

// disable eta right click
void disable_erc(){
    DIR* dir = opendir("/run/etap/right-click/disable");
    if(dir != NULL){
        closedir(dir);
        char path[PATH_MAX];
        sprintf(path, "/run/etap/right-click/disable/%d", getpid());
        FILE *f = fopen(path, "w");
        if(f != NULL){
            fprintf(f, "pardus-pen");
            fclose(f);
        }
        fclose(f);
    }
}

// disable eta right click
void enable_erc(){
    DIR* dir = opendir("/run/etap/right-click/disable");
    if(dir != NULL){
        closedir(dir);
        char path[PATH_MAX];
        sprintf(path, "/run/etap/right-click/disable/%d", getpid());
        unlink(path);
    }
}
