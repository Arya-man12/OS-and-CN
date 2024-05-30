
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>


sem_t chopsticks[5];


void *Phil(void *n)
{
        while (1) {
                int p = *(int *)n;
                int sem_value_right;

                printf("Phil %d  is thinking\n", p);
                sleep(rand()%4+1);
                printf("Phil %d wants to eat\n", p);
                if (p == 0) {
                        int sem_value_left;
                        printf("Phil %d is trying to pick the right chopstick\n", p);
                        sem_wait(&chopsticks[(p+1)%5]);
                        printf("Phil %d picks the right chopstick\n", p);

                        sem_getvalue(&chopsticks[p], &sem_value_left);
                        if (sem_value_left == 1) {
                                printf("Phil %d is trying to pick the left chopstick\n", p);
                                sem_wait(&chopsticks[p]);
                                printf("Phil %d picks the left chopstick\n", p);

                                printf("=====> Phil %d is eating\n", p);
                                sleep(rand()%5+5);


                                printf("Phil %d is putting down the chopsticks\n", p);
                                sem_post(&chopsticks[(p+1)%5]);
                                sem_post(&chopsticks[p]);

                        } else {
                                // If the left is not available then put down the one you hold(left)
                                sem_post(&chopsticks[(p+1)%5]);
                                sleep(rand()%6+2);
                        }

                } else {
                                printf("Phil %d is trying to pick the left chopstick\n", p);
                                sem_wait(&chopsticks[p]);
                                printf("Phil %d picks the left chopstick\n", p);

                                sem_getvalue(&chopsticks[(p+1)%5], &sem_value_right);
                                if (sem_value_right == 1) {
                                        printf("Phil %d is trying to pick the right chopstick\n", p);
                                        sem_wait(&chopsticks[(p+1)%5]);
                                        printf("Phil %d picks the right chopstick\n", p);

                                        printf("=====> Phil %d is eating\n", p);
                                        sleep(rand()%5+5);

                                        printf("Phil %d is putting down the chopsticks\n", p);
                                        sem_post(&chopsticks[p]);
                                        sem_post(&chopsticks[(p+1)%5]);

                                } else {
                                        sem_post(&chopsticks[p]);
                                        sleep(rand()%6+2);
                        }
                }
        }
}


int main(int argc, char const *argv[])
{
        int n[5];
        pthread_t T[5];
        for (int i = 0; i < 5; i++) {
                sem_init(&chopsticks[i], 0, 1);
        }
        for (int i = 0; i < 5; i++) {
                n[i] = i;
                pthread_create(&T[i], NULL, Phil, (void *)&n[i]);
        }
        for (int i =  0; i < 5; i++) {
                pthread_join(T[i], NULL);
        }


        return 0;
}
