#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <unistd.h>

#define SHM_SIZE 200  // Размер области памяти
#define SHM_COUNT 6   // Количество разделяемых областей

void write_to_all_memory(int writerIndex) {
    int shmId[SHM_COUNT], semIdW[SHM_COUNT], semIdR[SHM_COUNT];
    key_t keyShm, keySemW, keySemR;
    struct sembuf lockWrite = {0, -1, 0};  // Ожидаем разрешения на запись
    struct sembuf lockRead = {0, -1, 0};   // Ожидаем разрешения на чтение

    // Получаем идентификаторы всех областей памяти и семафоров
    for (int i = 0; i < SHM_COUNT; i++) {
        keyShm = ftok("Shm", i);
        keySemW = ftok("Sem", i);
        keySemR = ftok("Sem", i + 6);

        shmId[i] = shmget(keyShm, SHM_SIZE, 0);
        semIdW[i] = semget(keySemW, 1, 0);
        semIdR[i] = semget(keySemR, 1, 0);

        if (shmId[i] == -1 || semIdW[i] == -1 || semIdR[i] == -1) {
            perror("Ошибка доступа к памяти или семафору");
            exit(1);
        }
    }

    char message[20];
    sprintf(message, "Writer %d", writerIndex);

    // Записываем во все области памяти
    for (int i = 0; i < SHM_COUNT; i++) {
        // Подключаемся к разделяемой памяти
        char* addr = (char*)shmat(shmId[i], nullptr, 0);
        if (addr == (char*)-1) {
            perror("Ошибка подключения к разделяемой памяти");
            continue;
        }

        // Блокируем доступ для записи
        semop(semIdW[i], &lockWrite, 1);

        // Блокируем доступ для чтения
        semop(semIdR[i], &lockRead, 1);

        // Дозаписываем в память
        int currentLen = strlen(addr);
        if (currentLen + strlen(message) + 2 < SHM_SIZE) { 
            strcat(addr, message);
            strcat(addr, " ");
        } else {
            printf("Память %d заполнена!\n", i);
        }

        // Не разблокируем семафоры, программа не будет их разблокировать

        shmdt(addr); // Отключаем память
    }

    printf("Writer %d записал данные во все области памяти.\n", writerIndex);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Использование: %s <номер писателя>\n", argv[0]);
        return 1;
    }

    int writerIndex = atoi(argv[1]); // Получаем индекс писателя из аргумента командной строки
    write_to_all_memory(writerIndex);
    return 0;
}

