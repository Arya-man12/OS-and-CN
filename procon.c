#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/wait.h>

#define SHMSIZE 128
#define SHM_R 0400
#define SHM_W 0200

const char *semName1 = "my_sema1";
const char *semName2 = "my_sema2";

int main(int argc, char **argv)
{

        sem_t *sem_id1 = sem_open(semName1, O_CREAT, O_RDWR,10);
        sem_t *sem_id2 = sem_open(semName2, O_CREAT, O_RDWR, 0);
        int i, status;

        struct shm_struct {
                int buffer[10];
                int producer_counter;
                int consumer_counter;
        };

        volatile struct shm_struct *shmp = NULL;
        char *addr = NULL;
        pid_t pid;
        int var1 = 0, var2 = 0, shmid = -1;
        struct shmid_ds *shm_buf;



        shmid = shmget(IPC_PRIVATE, SHMSIZE, IPC_CREAT | SHM_R | SHM_W);
        shmp = (struct shm_struct *) shmat(shmid, addr, 0);
        shmp->producer_counter = 0;
        shmp->consumer_counter = 0;

        pid = fork();
        if (pid != 0) {

                while (var1 < 100) {
                        if (shmp->producer_counter > 9) {
                                shmp->producer_counter = 0;
                        }


                        sem_wait(sem_id1);
                        var1++;

                        printf("Sending %d\n", var1); fflush(stdout);
                        shmp->buffer[shmp->producer_counter] = var1;

                        shmp->producer_counter++;

                        sem_post(sem_id2);
                }
                shmdt(addr);
                shmctl(shmid, IPC_RMID, shm_buf);

        sem_close(sem_id1);
                sem_close(sem_id2);
                wait(&status);
                sem_unlink(semName1);
                sem_unlink(semName2);

        } else {

                while (var2 < 100) {
                        if (shmp->consumer_counter > 9) {
                                shmp->consumer_counter = 0;
                        }

                        sem_wait(sem_id2);

                        var2 = shmp->buffer[shmp->consumer_counter];
                        shmp->consumer_counter++;
                        printf("Received %d\n", var2); fflush(stdout);

                        sem_post(sem_id1);
                }
                shmdt(addr);
                shmctl(shmid, IPC_RMID, shm_buf);

        sem_close(sem_id1);
                sem_close(sem_id2);
        }
}


