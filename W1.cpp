#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/shm.h> 
#include <sys/sem.h>

struct sembuf{
    unsigned short sem_num;
    short sem_op;
    short sem_flg;
} buf{0, -1, 0};

int main(){
    int shmId, semId, keySem, keyShm;
    keySem = ftok("Sem", 1);
    if (keySem == -1) {
        perror("Sem ftok() Error");
        exit(EXIT_FAILURE);
    }

    semId = semget(keySem, 12, IPC_CREAT);
    if (semId == -1) {
        perror("Sem get Error");
        exit(EXIT_FAILURE);
    }
    printf("Подключен набор семафоров, semId=%d\n", semId);
    for(int i = 0; i < 6; i++){
        keyShm = ftok("Shm", i);
        if (keyShm == -1) {
            perror("Shm ftok Error");
            exit(EXIT_FAILURE);
        }
        printf("keySem=%d, keyShm=%d \n", keySem, keyShm);

        shmId = shmget(keyShm, 200, IPC_CREAT | 0606);
        if (shmId == -1) {
            perror("Shm get Error");
            exit(EXIT_FAILURE);
        }
        
        semop(semId[i], buf, 1);
        
        shmat(shmId, 0, 0);
        printf("shmId=%d\n", shmId);
    }
    return(0);
}
