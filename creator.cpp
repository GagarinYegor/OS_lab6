#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <cstdlib>
#include <cstdio>

#define SHM_COUNT 6
#define SHM_SIZE 200

union semun {
    int val;
};

int main() {
    int shmId[SHM_COUNT], semWrite[SHM_COUNT], semRead[SHM_COUNT];
    key_t keyShm, keySemWrite, keySemRead;
    union semun arg;

    for (int i = 0; i < SHM_COUNT; i++) {
        keyShm = ftok("Shm", i);
        keySemWrite = ftok("Sem", i);
        keySemRead = ftok("Sem", i + 6);

        shmId[i] = shmget(keyShm, SHM_SIZE, IPC_CREAT | 0666);
        semWrite[i] = semget(keySemWrite, 1, IPC_CREAT | 0666);
        semRead[i] = semget(keySemRead, 1, IPC_CREAT | 0666);

        if (shmId[i] == -1 || semWrite[i] == -1 || semRead[i] == -1) {
            perror("Ошибка при создании памяти или семафоров");
            exit(1);
        }

        // Инициализация: закрываем семафоры
        arg.val = 0;
        semctl(semWrite[i], i, SETVAL, arg);  // Закрываем для записи
        semctl(semRead[i], i, SETVAL, arg);   // Закрываем для чтения

        printf("Создана область %d: shmId=%d, semWrite=%d, semRead=%d\n", 
               i, shmId[i], semWrite[i], semRead[i]);
    }
    return 0;
}
