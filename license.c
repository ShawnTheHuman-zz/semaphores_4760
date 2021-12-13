/*
 * license.c
 *
 */

#include "config.h"


int getlicense() {
    if((shm->proc_running >= shm->available) || shm->available ==1){
        shm->block = 1;
    }
    else{
        shm->block = 0;
    }

    return shm->block;
}

int returnlicense(){
    shm->children--;
    return shm->children;

}

int initlicense() {
    shm->block = 0;

    printf("Available licenses: %d\n", shm->available);

    return shm->children;

}

void addtolicenses( int n ){
    shm->children - n;
    shm->proc_running--;

}

void removelicenses( int n ){
    shm->children + n;

}

void logmsg ( char* pid, char* c, char* r ) {
    FILE *out;

    if((out = fopen("logfile.data", "a")) == NULL){
        perror("ERROR: unable to open log file.");
        exit(1);
    }

    

    print_time(out);

    fprintf(out, "%s %s of %s\n", pid, c, r);

    fclose(out);

}

void print_time(FILE* out){
    time_t tm;
    time(&tm);

    struct tm *tp = localtime(&tm);

    fprintf(out, "%.2d:%.2d:%.2d ", tp->tm_hour, tp->tm_min, tp->tm_sec);

}
