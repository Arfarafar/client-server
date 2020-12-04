#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>



#define opINIT(n, sem, op, flg) sops[n].sem_num = sem;					\
						    	sops[n].sem_op = op;					\
						    	sops[n].sem_flg = flg;					


#define opSEM(sem, op, flg)		opINIT(0, sem, op, flg)					\
	    						semop(semid, sops, 1);



#define PAGE_SIZE 4096
const char SIZEOFINT = 4;
const char SIZEOFCHAR = 1;
#define NAMETOFTOK "server.c"



int initSem(int semId, int semNum, int initVal){
    return semctl(semId, semNum, SETVAL, initVal);
}