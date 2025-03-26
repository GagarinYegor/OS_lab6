#include <iostream>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#define SHM_SIZE 200
#define SHM_COUNT 6

int read(int reader_id) {
    //int shmId[SHM_COUNT], semRead[SHM_COUNT];
    int shmId, semRead;
    key_t keyShm, keySemRead;
    struct sembuf lockRead = {0, -1, 0};
    keyShm = ftok("Shm", reader_id);
    keySemRead = ftok("Sem", reader_id + 6); // Семафоры на чтение

    shmId = shmget(keyShm, SHM_SIZE, 0);
    semRead = semget(keySemRead, 1, 0);

    if (shmId == -1 || semRead == -1) {
        printf("Ошибка доступа к памяти или семафору\n");
    }
    
    // Получаем идентификаторы разделяемой памяти и семафоров
    /*
    for (int i = 0; i < SHM_COUNT; i++) {
        keyShm = ftok("Shm", i);
        keySemRead = ftok("Sem", i + 6); // Семафоры на чтение

        shmId[i] = shmget(keyShm, SHM_SIZE, 0);
        semRead[i] = semget(keySemRead, 1, 0);

        if (shmId[i] == -1 || semRead[i] == -1) {
            printf("Ошибка доступа к памяти или семафору %d\n", i);
            continue;
        }
    }

    // Блокируем семафоры для чтения
    for (int i = 0; i < SHM_COUNT; i++) {
        if (semRead[i] == -1) continue;  // Пропускаем недоступные семафоры

        semop(semRead[i], &lockRead, 1); // Ждем доступ к чтению

        char *addr = (char *)shmat(shmId[i], nullptr, SHM_RDONLY);
        if (addr == (char *)-1) {
            perror("Ошибка подключения к разделяемой памяти");
            exit(1);
        }

        printf("Читатель читает область %d: %s\n", i, addr);
        shmdt(addr); // Отключаем память
    }
    */
    semop(semRead, &lockRead, 1); // Ждем доступ к чтению

    char *addr = (char *)shmat(shmId, nullptr, SHM_RDONLY);
    if (addr == (char *)-1) {
        perror("Ошибка подключения к разделяемой памяти");
        exit(1);
    }

    printf("Читатель читает область %d: %s\n", reader_id, addr);
    shmdt(addr); // Отключаем память
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <reader_id>\n";
        return 1;
    }
    int reader_id = atoi(argv[1]);
    read(reader_id - 1);
    return 0;
}


