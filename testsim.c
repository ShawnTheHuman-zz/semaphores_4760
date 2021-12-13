/*
 * testsim.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "config.h"

int main ( int argc, char *argv[] ) {

	
	int repeat_factor;
	int sleep_time;

	if ( argc != 3 ) {
		perror("ERROR: testsim:  invalid number of arguments");
		return 1; 
	}
	
	repeat_factor = atoi(argv[1]);
	sleep_time = atoi(argv[2]);

	int i;
	for( i = 0; i < repeat_factor; i++) {
	
		sleep(sleep_time);
		fprintf(stderr, "pid: %d, repeat: %d, sleep: %d\n", getpid(), repeat_factor, sleep_time);
	
	}

	return 0;

}
