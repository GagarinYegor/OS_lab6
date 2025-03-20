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

int main() {
    int dr, i, shmId[6], semId[12], keySem, keyShm;
    struct sembuf unlockSem = {0, 1, 0};

    // Получаем идентификаторы семафоров
    for (int i = 0; i < 6; i++) {
        keySem = ftok("Sem", i);
        semId[i] = semget(keySem, 1, 0); // Семафор на запись
        semId[i+6] = semget(keySem, 1, 0); // Семафор на чтение

        if (semId[i] == -1 || semId[i+6] == -1) {
            perror("Ошибка при получении семафора");
            exit(1);
        }

        printf("Семафоры для области %d: semWrite=%d, semRead=%d\n", i, semId[i], semId[i+6]);
    }

    printf("Введите команду:\n");
    printf("-1: завершение работы\n");
    printf("9: количество заблокированных\n");
    printf("0: открыть семафоры на запись и чтение\n");

    for (;;) {
        scanf("%d", &dr);

        if (dr == 9) {
            for (int i = 0; i < 6; i++) {
                int blockedW = semctl(semId[i], 0, 200, NULL);
                int blockedR = semctl(semId[i+6], 0, 200, NULL);
                printf("Область %d: заблокировано писателей=%d, читателей=%d\n", i, blockedW, blockedR);
            }
        }

        if (dr == -1) {
            printf("Завершение работы по запросу пользователя.\n");

            // Завершаем все процессы
            for (int i = 0; i < 6; i++) {
                key_t keyShm = ftok("Shm", i);
                shmId[i] = shmget(keyShm, 200, 0);  // Получаем shmId
                if (shmId[i] != -1) {
                    char *addr = (char *)shmat(shmId[i], nullptr, 0);
                    if (addr != (char *)-1) {
                        strcpy(addr, "STOP");  // Устанавливаем сигнал завершения
                        shmdt(addr);
                    }
                }

                // Удаляем разделяемую память и семафоры
                shmctl(shmId[i], IPC_RMID, NULL);
                semctl(semId[i], 0, IPC_RMID, NULL);
                semctl(semId[i+6], 0, IPC_RMID, NULL);
            }

            exit(0);  // Завершаем выполнение программы
        }

        if (dr == 0) {
            for (int i = 0; i < 6; i++) {
                semop(semId[i], &unlockSem, 1); // Разблокируем запись
                semop(semId[i+6], &unlockSem, 1); // Разблокируем чтение
            }
        }

        sleep(1);  // Даем процессам время обработать "STOP"
    }

    return 0;
}
