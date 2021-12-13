/*
 * testsim.c
 */

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "config.h"



int shmid;
struct nLicenses *shm;

int main ( int argc, char *argv[] ) {

	signal(SIGINT, signal_handler);
	int repeat_factor, sleep_time, i;

	if ( argc != 3 ) {
		perror("ERROR: testsim:  invalid number of arguments");
		return 1; 
	}
	
	repeat_factor = atoi(argv[2]);
	sleep_time = atoi(argv[1]);
	


	if((shmid = shmget(SHMKEY, sizeof(struct nLicenses) * 2, 0666)) < 0) {
		perror("testsim: Error: shmget ");
		exit(1);
	}

	if((shm = (struct nLicenses *)shmat(shmid, NULL, 0)) == (struct nLicenses *) -1) {
		perror("testsim: Error: shmat ");
		exit(1);
	}


	int c, j, k, n = shm->children;
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
		pid_t id = getpid();
		char pid[50];
		char num[50];
	
		sprintf(pid, "%d", id);
		sprintf(num, "%d", (c + 1));

		printf("Printing msg to file: %s %s of %s\n", pid, num, argv[2]);

		/* prints log to file */ 
		logmsg(pid, num, argv[2]);

		sleep(sleep_time);

		shm->turn[(n - 1)] = 0;

	}
	/* adding to licenses and detaching shared memory */
	addtolicenses(1);
	shmdt(shm);
	return 0;

}

void signal_handler(int s){
	pid_t id = getpid();
	shmdt(shm);
	killpg(id, SIGINT);
	exit(1);

}
