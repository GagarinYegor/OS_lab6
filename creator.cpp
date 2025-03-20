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


int main(){
    int shmId, semId, keySem, keyShm;
    unsigned short ray[1]={0};
    union semun {
        int val;
        unsigned short *array;
    } arg;

    keySem = ftok("Sem", 1);
    if (keySem == -1) {
        perror("Sem ftok() Error");
        exit(EXIT_FAILURE);
    }

    semId = semget(keySem, 12, IPC_CREAT | 0606);
    if (semId == -1) {
        perror("Sem get Error");
        exit(EXIT_FAILURE);
    }
    printf("semId=%d\n\n", semId);

    for (int i = 1; i <= 6; i++) {
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
        printf("shmId=%d\n", shmId);
    }

    arg.array = ray;
    printf("%d\n", arg.array[0]);
    if (semctl(semId, 1, SETALL, arg) == -1) {
        perror("Sem ctl Error");
        exit(EXIT_FAILURE);
    }
    return(0);
}
