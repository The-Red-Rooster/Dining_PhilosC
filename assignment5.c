/*
NAME: Henry Unruh
CLASS: CS360
ASSIGNMENT: Assignment 5, Dining Philosophers
SYNOPSIS: This is a program that tackles a computer science concept known as the "Dining Philosophers" problem; deadlocking and cycles.
*/

//Include statements
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "random.h"

//Function declaration
int diningPhilosopher(int semaphore, int thinker);

//Main function
int main(int argc, char *argv[]){
	int semaphore;
	semaphore = semget(IPC_PRIVATE, 5, S_IRUSR | S_IWUSR); //creating a semaphore, 5 for chopsticks
	
	int thinker = 0; //the philosophers
	
	if(semaphore < 0){ //Error checking
		fprintf(stderr, "ERR: problem with semget...\n");
		exit(1);
	}
	
	int i = 0;
	while(i < 5){ //perform semaphore operations
		struct sembuf set = {i, 1, 0};
		semop(semaphore, &set, 1);
		i++;
	}
	
	int j = 0;
	while(j < 5){ //initializing the Dining Philosophers process
		if(!fork()){
			diningPhilosopher(semaphore, thinker);
		}
		thinker++;
		j++;
	}
	
	int k = 0;
	while(k < 5){
		wait(NULL);
		k++;
	}
	
	semctl(semaphore, 0, IPC_RMID, 0); //If successful, free shared memory
	return 0;
}

//Dining philosopher function
int diningPhilosopher(int semaphore, int thinkerID){
	int ponderingTime, eatingTime; //integer values corresopnding to when they eat/think
	int eatingTotalTime, ponderTotalTime; //total amount of time for eating and thinking
	
	eatingTotalTime = 0; ponderTotalTime = 0;
	
	srand(getpid());
	
	struct sembuf unlock[2] = {{thinkerID, 1, 0}, {(thinkerID + 1) % 5, 1, 0}};
	struct sembuf lock[2] = {{thinkerID, -1, 0}, {(thinkerID + 1) % 5, -1, 0}};
	
	while(eatingTotalTime < 100){ //all philosophers have to eat for at least 100 seconds
		
		//thinking cycle
		ponderingTime = randomGaussian(11, 7); //randomGaussian(thinking mean, thinking std. dev.)
		if(ponderingTime < 0){
			ponderingTime = 0;
		}
		ponderTotalTime = ponderingTime + ponderTotalTime;
		printf("Philosopher %d has been pondering for %d seconds. (total time thinking = %d)\n", thinkerID, ponderingTime, ponderTotalTime);
		sleep(ponderingTime); //continue pondering
		
		//eating cycle
		eatingTime = randomGaussian(9, 3); //randomGaussian(eating mean, eating std. dev.)
		if(eatingTime < 0){//If they don't eat... they're not eating.
			eatingTime = 0;
		}
		eatingTotalTime = eatingTime + eatingTotalTime;
		printf("Philosopher %d has been eating for %d seconds. (total time eating = %d)\n", thinkerID, eatingTime, eatingTotalTime);
		sleep(eatingTime); //continue eating
		
		if(semop(semaphore, &lock[0], 2) < 0){//Error checking
			fprintf(stderr, "ERR: problem with locking %s...\n", strerror(errno));
			exit(1);
		}
				
		if(semop(semaphore, &unlock[0], 2) < 0){//Error checking
			fprintf(stderr, "ERR: problem with unlocking %s...\n", strerror(errno));
		}
	}
	exit(0);	
}
