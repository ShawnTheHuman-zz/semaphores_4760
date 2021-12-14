/*
	Shawn Brown
	Project 3 - 4760
	runsim.c
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <signal.h>
#include <ctype.h>
#include <errno.h>

#include "config.h"


/* forward declarations  */
void docommand( char* prog, char* name, char* arr1, char* arr2, char* arr3);
char** make_argv(char* str);
void terminate_processes();
void signal_handler();
void usage();


int shmid;
struct nLicenses *shm;



int main(int argc, char* argv[]){
	
	/* initiate signal handler */ 
	signal(SIGINT, signal_handler);

	int license_count;
	int child_count = 0;
	int max_time = MAX_TIME;
	
	key_t SHMKEY = ftok("./", 'R'); // key for shared memory
	key_t SEMKEY = ftok("./config.h", 'X'); // key for semaphore

	// if( argc != 2 ) {
		
	// 	perror("ERROR: runsim: invalid arguments\n");
	// 	usage();
	// 	exit(1);

	// }

	// /* checks that n is a digit, then sets license count to the number */	

	int opterr = 0;
	int opt;
    while ((opt = getopt(argc, argv, "t:")) != -1) {
        switch (opt) {

            case 't':
                max_time = atoi(optarg);
                break;
            case '?': // Unknown arguement                
                if (isprint (optopt))
                {
                    errno = EINVAL;
                    perror("Unknown option");
                }
                else
                {
                    errno = EINVAL;
                    perror("Unknown option character");
                }
                return EXIT_FAILURE;
            default:    // An bad input parameter was entered
                // Show error because a bad option was found
                perror ("Error: Runsim: Illegal option found");
                usage();
                return EXIT_FAILURE;
        }
    }

	if(strspn(argv[4], "0123456789") == strlen(argv[4])){
		license_count = atoi(argv[3]);
		if( license_count <= 0 ){
			perror("ERROR: runsim: n must be a number greating than zero\n");
			usage();
			exit(1);
		}
	}
	else{
		perror("ERROR: runsim: Invalid argument\n");
		usage();
		exit(1);
	}

	/* get shared memory id */
	if(( shmid = shmget(SHMKEY, sizeof(struct nLicenses)*2, 0666 | IPC_CREAT )) < 0 ) {
		perror("ERROR: runsim: error getting memory");
		exit(1);
	}
	/* attach data to shared memory */
	if( (shm = (struct nLicenses *)shmat(shmid, NULL, 0)) == ((struct nLicenses *)(-1)) ) {
		perror("ERROR: runsim: unable to attach memory");
		exit(1);

	}

	shm->available = license_count;
	shm->proc_running++;

	/* initiate license  */
	initlicense();

	/* data buffer array */
	char buf[MAX_CANON];

	/* 2d array for storing data from the file */
	char lines[BUFFER][BUFFER];

	int i,j = 0;

	/* arrays to store the arguments read in from the file */
	char prog_name[50] = "./";
	char sleep_arr[50];
	char repeat_arr[50];


	/* reads data from command line */
	while(fgets(buf, MAX_CANON, stdin) != NULL){
		strcpy(lines[child_count], buf);
		child_count++;
	}

	shm->children = child_count;

	
	int proc_running = 0;

	int index = 0;

	pid_t pid, child[child_count];

	while(max_time > 0){
		if(proc_running < 20){
			while(getlicense() == 1){
				if(license_count == 1){
					max_time--;
					sleep(1);

					if(max_time < 0){
						perror("ERROR: runsim ran out of time. aborting all processes\n");
						signal_handler();
						exit(1);
					}
				}
			}
			if(index < child_count){
				/*
					for loops that read in the arguments the do_command function
					it reads until the first space, storing the contents in the program
					name array, then the same for the sleep and repeat factors in respective arrays. 
				 
				*/
				for( i; lines[index][i] != ' '; i++){
					prog_name[i + 2] = lines[index][i];
				}

				i++;

				for( i; lines[index][i] != ' '; i++ ){
					sleep_arr[j] = lines[index][i];
					//printf("sleep factor %d\n", sleep_arr[j]);
					j++;
				}

				j = 0;
				i++;

				for( i; i < strlen(lines[index]) - 1; i++){

					repeat_arr[j] = lines[index][i];
					//printf("repeat factor %d\n", repeat_arr[j]);
					j++;
				}

				i = 0;
				j = 0;

				removelicenses(1);

				pid = fork();

				child[index] = pid;

				proc_running++;

				shm->proc_running++;

				printf("RUNSIM: running process: %i\n", shm->proc_running);
				index++;
			}
			if(pid == -1){
				perror("RUNSIM: fork error");
				terminate_processes();
				exit(1);

			}
			else if(pid == 0){
				char ch[50];
				sprintf(ch, "%d", index);
				//printf(" RUNNING TESTSIM\n");

				//execl(prog_name, "testsim", sleep_arr, repeat_arr, ch,(char *)NULL);
				docommand(prog_name, "testsim", sleep_arr, repeat_arr, ch);
			}
			max_time--;
			sleep(1);
		}
		else{
			perror("ERROR: runsim: exceeded process limit. aborting");
			signal_handler();
			exit(1);
			
		}
		shm->proc_running--;
		int verify = procs_remaining(child, child_count);
		if(verify == 1){
			break;
		}

	}

	if((wait(NULL) > 0) && (shm->children) != 0){
		perror("ERROR: runsim: exceeded maximum time. aborting.");
		terminate_processes();
	}

	return 0;

}

void usage(){
	printf("./runsim [ -t secs ] n < testing.data -- where n is an integer number of licenses");
}
/* simple function that just takes arguments to exec */
void docommand( char* prog, char* name, char* arr1, char* arr2, char* c){
	execl(prog, "testsim", arr1, arr2, c,(char *)NULL);
}
/* removes shared memory */
void terminate_processes(){
	shmctl(shmid, IPC_RMID, NULL);
	shmdt(shm);
}
/* unuti tokenizes input to make argument array */
char** make_argv( char* str ){
	
	char* substr;
	char** _argv = malloc(10 * sizeof(char));

	substr = strtok(str, " ");

	int i = 0;
	while( substr != NULL){
		_argv[i] = malloc(20 * sizeof(char));
		_argv[i] = substr;
		substr = strtok(NULL, " ");
		i++;
	}
	_argv[i] = NULL;
	
	return _argv;
}

/* checks if any child processes are still running */
int procs_remaining(pid_t procs[], int size)
{
	int i, status;
	
	for(i = 0; i < size; i++){
		pid_t wait;
		wait = waitpid(procs[i], &status, WNOHANG);
		
		if(wait != 0){
			procs[i] = 0;
		}
	}
	for(i = 0; i < size; i++){
		if(procs[i] == 0){
			continue;
		}
		else{ return 0;}
	}
	return 1;
}

void signal_handler(int s){

	printf("\nKilled with manual interrupt.\n");
	pid_t id = getpgrp();
	terminate_processes();
	killpg(id, SIGINT);
	exit(1);

}
