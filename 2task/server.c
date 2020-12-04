#include "head.h"
/*все то же самое что и для клиента только обратные действия для сервера
	*/

void File_Receiver(int semid, void* addr){

	struct sembuf sops[4];

	opINIT(0, 1, 0, 0)
	opINIT(1, 1, 1, SEM_UNDO)
	semop(semid, sops, 2);
	
	opINIT(0, 4, 0, 0);
	semop(semid, sops, 1);


	initSem(semid, 2, 2);

	opINIT(0, 1, 1, SEM_UNDO)
	opINIT(1, 2, -1, SEM_UNDO)
	semop(semid, sops, 2);
	

	opINIT(0, 0, -2, 0) 
	opINIT(1, 0, 2, 0)
	opINIT(2, 4, 1, SEM_UNDO)
	semop(semid, sops, 3);
	


	
	int indicator = PAGE_SIZE - SIZEOFINT;

	while(indicator == PAGE_SIZE - SIZEOFINT) {	
	
		//reserve read
		opSEM(3, -1, 0)
		
		if(semctl(semid, 4, GETVAL) != 2){
			if (*(int*)addr == PAGE_SIZE - SIZEOFINT)
				break;
		}

		indicator = *(int*)addr;
		write(STDOUT_FILENO, addr + SIZEOFINT , indicator);	

		//realease write
		opSEM(2, 1, 0)

	}


						
}


int main(int argc, char* argv[]){
	if (argc > 1){
		printf("Invalid argc\n");
		return 0;
	}

	int semid = semget(ftok(NAMETOFTOK, 10), 5, IPC_CREAT | 0666);
	if( semid == -1){
		perror("semid");
		exit(1);
	}
	//0-file translator
	//1-file receiver
	//2-shmwrite
	//3-shmread
	//4-synchronyze

	int shmid = shmget(ftok(NAMETOFTOK, 10), PAGE_SIZE, IPC_CREAT | 0666);
	if( shmid == -1){
		perror("shmid");
		exit(1);
	}
	void* addr = shmat(shmid, NULL, 0);


	File_Receiver(semid, addr);
				
	
	return 0;
}
