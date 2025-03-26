#include <iostream>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <cstring>

#define SHM_COUNT 6  // Количество областей памяти
#define SHM_SIZE 200 // Размер области памяти

int main() {
    int dr, i, shmId[SHM_COUNT], semWrite[SHM_COUNT], semRead[SHM_COUNT], keySemWrite, keySemRead, keyShm;
    struct sembuf unlockSem = {0, 1, 0}; // Открытие семафора

    // Получаем идентификаторы семафоров
    for (int j = 0; j < SHM_COUNT; j++) {
        keySemWrite = ftok("Sem", j);
        keySemRead = ftok("Sem", j + 6);
        semWrite[j] = semget(keySemWrite, 1, 0); // Семафор на запись
        semRead[j] = semget(keySemRead, 1, 0); // Семафор на чтение

        if (semWrite[j] == -1 || semRead[j] == -1) {
            perror("Ошибка при получении семафора");
            exit(1);
        }

        printf("Семафоры для области %d: semWrite=%d, semRead=%d\n", j, semWrite[j], semRead[j]);
    }

    printf("Введите команду:\n");
    printf("0: открыть семафоры на запись\n");
    printf("1: открыть семафоры на чтение\n");
    printf("2: Вывести состояние\n");
    printf("3: завершить работу, удалить области разделяемой памяти\n");

    for (;;) {
        scanf("%d", &dr);
        
        if (dr == 0) {
            for (int j = 0; j < SHM_COUNT; j++) {
                semop(semWrite[j], &unlockSem, 1); // Разблокируем запись
                printf("Семафоры на запись разблокированы\n");
            }
        }
        
        if (dr == 1) {
            for (int j = 0; j < SHM_COUNT; j++) {
                semop(semRead[j], &unlockSem, 1); // Разблокируем чтение
                printf("Семафоры на чтение разблокированы\n");
            }
        }

        if (dr == 2) {
            for (int j = 0; j < SHM_COUNT; j++) {
                printf("Область %d: заблокировано %d писателей, %d читателей\n", j, semctl(semWrite[j], 0, GETNCNT, NULL), semctl(semRead[j], 0, GETNCNT, NULL));
            }
        }

        if (dr == 3) {
            printf("Завершение работы по запросу пользователя.\n");

            // Завершаем все процессы
            for (int j = 0; j < SHM_COUNT; j++) {
                key_t keyShm = ftok("Shm", j);
                shmId[j] = shmget(keyShm, SHM_SIZE, 0);  // Получаем shmId
                if (shmId[j] != -1) {
                    char *addr = (char *)shmat(shmId[j], nullptr, 0);
                    if (addr != (char *)-1) {
                        strcpy(addr, "STOP");  // Устанавливаем сигнал завершения
                        shmdt(addr);
                    }
                }

                // Удаляем разделяемую память и семафоры
                shmctl(shmId[j], IPC_RMID, NULL);
                semctl(semWrite[j], 0, IPC_RMID, NULL);
                semctl(semRead[j], 0, IPC_RMID, NULL);
            }

            exit(0);  // Завершаем выполнение программы
        }

        //sleep(1);  // Даем процессам время обработать "STOP"
    }

    return 0;
}
