/*
	Shawn Brown
	project 3 - 4760
	testsim.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "config.h"
#include "semaphore.h"




int shmid;
struct nLicenses *shm;

void signal_handler(int s){
	pid_t id = getpid();
	shmdt(shm);
	killpg(id, SIGINT);
	exit(1);

}


int main ( int argc, char *argv[] ) {
	
	//printf("TEST SIM\n");
	key_t SHMKEY = ftok("./", 'R'); // key for shared memory
	key_t SEMKEY = ftok("./config.h", 'X'); // key for semaphore
	
	Semaphore s(SEMKEY, true, 1);

	signal(SIGINT, signal_handler);

	int repeat_factor, 
		sleep_time, 
		i;

	
	sleep_time = atoi(argv[1]);
	repeat_factor = atoi(argv[2]);
	
	
	//printf("sleep time %d\n", sleep_time);

	if((shmid = shmget(SHMKEY, sizeof(struct nLicenses) * 2, 0666 | IPC_CREAT)) < 0) {
		perror("testsim: Error: shmget ");
		exit(1);
	}

	if((shm = (struct nLicenses *)shmat(shmid, NULL, 0)) == (struct nLicenses *) - 1) {
		perror("testsim: Error: shmat ");
		exit(1);
	}


	int c, j, n = shm->children;
	int max = 0;
	shm->choosing[(n-1)] = 1;

	/*
		Implementation of the Bakery algorithm
	*/

	for(c = 0 ; c < repeat_factor; c++) {
		if((shm->turn[c]) > max) {
			max = (shm->turn[c]);
		}
		shm->turn[(n-1)] = 1 + max;

		shm->choosing[(n-1)] = 0;
		for(j = 0 ; j < n ; j++) {
			while(shm->choosing[j] == 1) {}
				while((shm->turn[j] != 0) && (shm->turn[j] < shm->turn[(n-1)])) {}
		}
		/* CRITICAL SECTION */
		s.Wait();

		pid_t id = getpid();
		char pid[50];
		char num[50];
	
		sprintf(pid, "%d", id);
		sprintf(num, "%d", (c + 1));

		printf("Printing to file: PID: %s LOOP: %s of %s\n", pid, num, argv[2]);

		/* prints log to file */ 
		logmsg(pid, num, argv[2]);
		s.Signal();
		sleep(sleep_time);

		shm->turn[(n - 1)] = 0;

	}
	/* adding to licenses and detaching shared memory */
	addtolicenses(1);
	shmdt(shm);
	return 0;

}

