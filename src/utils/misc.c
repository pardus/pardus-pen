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
    const char* path = getenv("PATH");
    if (path == NULL) {
        return strdup("");
    }
    char* fullPath = strdup(path);

    struct stat buffer;
    int exists;
    char* fileOrDirectory = (char*)cmd;
    char *fullfilename = calloc(4096, sizeof(char));

    char *token = strtok(fullPath, ":");

    while( token != NULL ){
        int n = snprintf(fullfilename, 4096, "%s/%s", token, fileOrDirectory);
        if (n < 0 || n >= 4096) {
            token = strtok(NULL, ":");
            continue;
        }
        exists = stat( fullfilename, &buffer);
        if ( exists == 0 ) {
            free(fullPath);
            return fullfilename;
        }

      token = strtok(NULL, ":");
    }
    free(fullPath);
    free(fullfilename);
    return strdup("");
}

static int pid = 0;

// disable eta right click
void disable_erc(){
    if(pid == 0){
        pid = getpid();
    }
    DIR* dir = opendir("/run/etap/right-click/disable");
    if(dir != NULL){
        closedir(dir);
        char path[PATH_MAX];
        sprintf(path, "/run/etap/right-click/disable/%d", pid);
        FILE *f = fopen(path, "w");
        if(f != NULL){
            fprintf(f, "pardus-pen");
            fclose(f);
        }
    }
}

// disable eta right click
void enable_erc(){
    if(pid == 0){
        pid = getpid();
    }
    DIR* dir = opendir("/run/etap/right-click/disable");
    if(dir != NULL){
        closedir(dir);
        char path[PATH_MAX];
        sprintf(path, "/run/etap/right-click/disable/%d", pid);
        unlink(path);
    }
}
