#ifndef CONFIG_H
#define CONFIG_H

#define MAX_CANON 20	
#define MAX_TIME 100
#define BUFFER 100

#define SHMKEY 102938

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>

extern int shmid; // global for shared memory id
extern struct nLicenses *shm; // global license object to use shared memory

struct nLicenses{
    
    /* license object members */
    int available,
        children,
        proc_running,

        /* For the Bakery algorithm */
        choosing[100],
        turn[100],
        block;
};

/* function declarations */
void signal_handler(int);
extern int getlicense(void);
extern int returnlicense(void);
extern int initlicense(void);
extern void addtolicenses(int n);
extern void removelicenses(int n);
extern void logmsg(char* msg, char*, char*); 
extern void print_time(FILE*);




#endif
