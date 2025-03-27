#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#define SHM_SIZE 200
#define SHM_COUNT 6

void read_from_memory(int reader_id) {
    int shmId[SHM_COUNT];
    key_t keyShm;

    for (int i = 0; i < SHM_COUNT; i++) {
        keyShm = ftok("Shm", i);

        shmId[i] = shmget(keyShm, SHM_SIZE, 0);
        if (shmId[i] == -1) {
            perror("Ошибка доступа к разделяемой памяти");
            exit(1);
        }
    }

        char *addr = (char *)shmat(shmId[reader_id], nullptr, SHM_RDONLY);

        printf("Reader %d читает область %d: %s\n", reader_id, reader_id, addr);
        shmdt(addr);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <reader_id>\n";
        return 1;
    }
    int reader_id = atoi(argv[1]);
    read_from_memory(reader_id);
    return 0;
}

